#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 64
#define DISK_SIZE 256
#define PAGE_LIMIT 4
#define MAX_PROCESSES 4

// Alternative to bit-masking:
// A byte-wide struct containing info about allocated pages
typedef struct pte {
  u_int8_t io:1;   // 0 if page is in memory, 1 if in disk
  u_int8_t pid:2;  // [0-3] for up to 4 processes
  u_int8_t vpn:2;  // [0-3] for virtual page numbers 
  u_int8_t pfn:2;  // [0-3] for physical page numbers
  u_int8_t rw:1;   // 0 for read only, 1 for read/write
} pte;

typedef struct instruction {
  int pid;              // [0-3]
  char *type;           // map, store, and load
  unsigned int v_addr;  // [0-63]
  unsigned int value;   // [0-255]
} instruction;

unsigned char memory[SIZE];
unsigned char disk[SIZE*SIZE];
FILE* swap_space;
int free_list[4];
int mem_registry[MAX_PROCESSES];
int disk_registry[MAX_PROCESSES];
// Which page to evict in the event of a swap, follows a round-robin strategy
int next_evict = 0;

int map(instruction instruc);
int store(instruction instruc);
int load(instruction instruc);
/* Swaps contents of memory/disk (to_disk or !to_disk respectively) in page starting 
 * at from offset to a swap file, and from there to 
 * page in disk/memory starting at to offset
 */
int swap(pte entry, int to_disk);

int main (int argc, char *argv[]) {

  // Place offsets into free list
  free_list[0] = -1; // Reserved for page table
  free_list[1] = 16;
  free_list[2] = 32;
  free_list[3] = 48;

  // Mark all registers as unused except the one for our page table
  for (int i = 0; i < MAX_PROCESSES; i++) { 
    if (!i) mem_registry[i] = 0;
    else mem_registry[i] = -1;
    disk_registry[i] = -1;
  } 

  // Open swap file
  swap_space = fopen("swap_space.txt","w+");
  
  // Instruction buffer
  int loop = 1;
  char buf[100];
  // Instruction prompter
  while(loop) {
    printf("Instruction: ");
    fgets(buf, sizeof buf, stdin);

    // Tokenize string into four instruction arguments
    strtok(buf,"\n"); // Clear endline so last token doesn't keep it
    char *tok;
    char *buf_chunks[4];
    int i = 0;
    for (tok = strtok(buf,", "); tok != NULL; tok = strtok(NULL," ,")) {
      buf_chunks[i] = tok;
      i++;
    } if (i != 4) { // Exactly 4 arguments must be passed in
      printf("Please enter exactly 4 arguments for instruction. You entered %d\n",i);
      continue;
    }

    // Populate struct with converted tokens
    instruction instruc;
    instruc.pid = (int) strtol(buf_chunks[0], (char **)NULL, 10);
    instruc.type = buf_chunks[1];
    instruc.v_addr = (unsigned int) strtol(buf_chunks[2], (char **)NULL, 10);
    instruc.value = (unsigned int) strtol(buf_chunks[3], (char **)NULL, 10);

    if (!strcmp(instruc.type, "map")) {
      map(instruc);
    } else if (!strcmp(instruc.type, "store")) {
      store(instruc);
    } else if (!strcmp(instruc.type, "load")) {
      load(instruc);
    } else if (!strcmp(instruc.type, "end")) {
      printf("Goodbye!\n");
      loop = 0;
    } else {
      printf("\"%s\" is not a valid instruction.\n",instruc.type);
    }
  }

  fclose(swap_space);
  return 0;
}

int map(instruction instruc) {

  int first_map = 1;
  
  // Check if value field is correct for mapping (0 for read or 1 for write)
  if (instruc.value > 1 || instruc.value < 0) {
    printf("Value must be 0 or 1 for mapping.\n");
    return 1;
  }
  
  // Check if virtual address has already been mapped for this process
  instruc.v_addr = (instruc.v_addr / 16) * 16; // Convert virtual address to appropriate multiple of 16
  for (int i = 0; i < 16; i++) {
    if (memory[i]) { // Will be non zero if containing a page table entry struct
      pte* entry = (pte*) &memory[i];
      if (entry->pid == instruc.pid && entry->vpn == instruc.v_addr / 16) {
	printf("Virtual address %d has is already in physical frame %d for process %d\n", entry->vpn, entry->pfn, entry->pid);
	return -1;
      } else if (entry->pid == instruc.pid && entry->pfn)
	first_map = 0;
    }
  }

  pte our_entry = {.io = 1, .pid = instruc.pid, .rw = instruc.value}; // Page table entry for this map

  int j;
  for (j = 0; j < MAX_PROCESSES; j++) {
    if (free_list[j] > -1) {
      our_entry.vpn = instruc.v_addr / 16;
      our_entry.pfn = j;
      break;
    }
  } if (j == 4) {
    printf("Not enough pages left!\n");
    return -1;
  }


  if (first_map)
    printf("Placed page table for PID %d into frame 0\n",instruc.pid);
  
  for (int i = 0; i < 16; i++) {
    if (!memory[i]) {
      memcpy(&memory[i], &our_entry, sizeof(pte));
      printf("Mapped virtual address %d (page %d) into physical frame %d\n", instruc.v_addr, our_entry.vpn, our_entry.pfn);
      break;
    } else if (i == 15) {
      printf("Somehow page table is full\n");
      return -1;
    }
  }

  mem_registry[instruc.pid] = free_list[our_entry.pfn];
  free_list[our_entry.pfn] = -1;
    
  return 0;
}

int store(instruction instruc) {

  // First check if value is too big or negative
  if (instruc.value < 0 || instruc.value > 255) {
    printf("Value out of range\n");
    return 1;
  }

  // Now try to find corresponding page table entry for this PID and virtual address
  pte our_entry;
  int entry_index = -1;
  for (int i = 0; i < 16; i++) {
    if (memory[i]) {
      pte* entry = (pte*) &memory[i];
      if (entry->pid == instruc.pid && entry->vpn == instruc.v_addr / 16) {
	entry_index = i;
	our_entry = *entry;
	break;
      }
    }
  }

  if (entry_index == -1) {
    printf("Virtual address %d has not been mapped to process %d\n", instruc.v_addr, instruc.pid);
    return -1;
  } else if (!our_entry.rw) {
    printf("Page %d of process %d is not writeable\n", our_entry.vpn, instruc.pid);
    return -1;
  }
  
  int p_addr = (16 * our_entry.pfn) + (instruc.v_addr % 16); // Perform address translation
  memory[p_addr] = instruc.value;
  printf("Stored value %d at virtual address %d (physical address %d)\n",instruc.value,instruc.v_addr,p_addr);

  return 0;
}

int load(instruction instruc) {

  // Try to find corresponding page table entry for this PID and virtual address
  pte our_entry;
  int entry_index = -1;
  for (int i = 0; i < 16; i++) {
    if (memory[i]) {
      pte* entry = (pte*) &memory[i];
      if (entry->pid == instruc.pid && entry->vpn == instruc.v_addr / 16) {
	entry_index = i;
	our_entry = *entry;
	break;
      }
    }
  }

  if (entry_index == -1) {
    printf("Virtual address %d has not been mapped to process %d\n", instruc.v_addr, instruc.pid);
    return -1;
  } 

  int p_addr = (16 * our_entry.pfn) + (instruc.v_addr % 16); // Perform address translation

  if (memory[p_addr] == 0) {
    printf("0 found at virtual address %d (physical address %d): Could be empty or 0...\n", instruc.v_addr, p_addr);
    return -1;
  }

  printf("The value %d is virtual address %d (physical address %d)\n", memory[p_addr], instruc.v_addr, p_addr);
  return 0;
}

/*
int swap(pte *entry, int to_disk) { 

  // Ensure entry is actually in page table
  int entry_index = -1;
  for (int i = 0; i < 16; i++) {
    if (memory[i]) {
      pte* temp = (pte*) &memory[i];
      if (temp->pid == entry->pid && temp->vpn == entry->vpn) {
	entry_index = i;
        break;
      }
    }
  } if (entry_index == -1) {
    printf("Tried to swap nonexistent page\n");
    return -1;
  }  
  
  // Clear appropriate registers and (if memory) mark as unused in free_list
  int 
  if (to_disk)
    
  
  if (to_disk) {
    free_list[entry->pid] = 16 * entry->pfn;
    mem_registry[entry->pid] = -1;
  } else {
    disk_registry[entry->pid] = -1;
  }
  
  // Append page contents from memory/disk to end of swap file
  int start = 
  int file_start = (int) ftell(swap_space);

  while (from < start + 16) {
    if (to_disk) {
      if (is_page_table && from == start + 2)  // If a page table, make sure to change PFN index, as it is no longer accurate
	memory[from] = to / 16;
      printf("%ld %d\n", ftell(swap_space), fputc(memory[from++], swap_space));
      memory[from - 1] = 0; // After copying, erase in memory
    } else {
      if (is_page_table && from == start + 2) // If a page table, make sure to change PFN index, as it is no longer accurate
	disk[from] = to / 16;
      fputc(disk[from++], swap_space);
      disk[from - 1] = 0; // After copying, erase in disk
    }
  } fputc('\n', swap_space); // End pages with newline for readability of file
  
  // Now transfer contents from file to disk/memory
  start = to;
  fseek(swap_space, file_start, SEEK_SET); // get back to where we began putting content into swap_space

  int ch;
  while ((ch = fgetc(swap_space)) != EOF && ch != '\n' && to < start + 16) {
    if (to_disk) {
      if (to == start)
	disk[to++] = (unsigned char) (ch - 4); // Set leftmost bit to 0 to mark that we are in disk
      else
	disk[to++] = (unsigned char) ch;
    } else { // assume a page has been allocated for us already
      if (to == start)
	memory[to++] = (unsigned char) (ch + 4); // Set rightmost bit 1 to mark that we are in memory
      else
	memory[to++] = (unsigned char) (ch);
    }
  }

  // Check swap was successful (if not whole program is basically messed up and we must Ctrl-C)
  if (ch == EOF) {
    printf("ERROR: Somehow encountered end of file while swapping file index %d to %d\n", file_start, to);
    return -1;
  } else if (to != start + 16) {
    printf("ERROR: Somehow went to %d from file from %d\n", to, file_start);
    return -1;
  }

  // Update registers appropriately
  if (to_disk) { 
    if (is_page_table)
      disk_pgtable_offsets[pid] = start;
    else
      disk_page_offsets[pid] = start;
  } else {
    if (is_page_table)
      mem_pgtable_offsets[pid] = start;
    else
      mem_page_offsets[pid] = start;
  }
  
  // Adjust next_evict to follow round-robin eviction strategy
  if (next_evict == 3) next_evict = 0;
  else next_evict++;
  
  // Notify user and exit
  if (to_disk) {
    printf("Swapped frame %d into disk at %d\n", start / 16, to - 16);
    return 0;
  } else {
    printf("Swapped disk offset %d into frame %d\n", from - 16, start / 16);
    return 0;
  }

}

