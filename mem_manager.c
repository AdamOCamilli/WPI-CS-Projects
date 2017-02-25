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
int free_list[PAGE_LIMIT];
int disk_free[PAGE_LIMIT * SIZE];
int page_table_in_memory = 1;
// Which page to evict in the event of a swap, follows a round-robin strategy
int next_evict = 1;

int map(instruction instruc);
int store(instruction instruc);
int load(instruction instruc);
/* Swaps contents of memory/disk (to_disk or !to_disk respectively) in page starting 
 * at from offset to a swap file, and from there to 
 * page in disk/memory starting at to offset
 */
int swap(pte *entry, int to_disk);

int main (int argc, char *argv[]) {

  // Place offsets into free list
  free_list[0] = -1; // Initially reserved for page table
  free_list[1] = 16;
  free_list[2] = 32;
  free_list[3] = 48;

  for (int i = 0; i < PAGE_LIMIT * PAGE_LIMIT; i++)
    disk_free[i] = 16*i;

  // Open swap file
  swap_space = fopen("swap_space.txt","wb+");
  
  // Instruction buffer
  char buf[100];
  // Instruction prompter
  while(1) {
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
    } if (i != 4 && strcmp(buf_chunks[0],"end") && strcmp(buf_chunks[0],"status")) {
      printf("Please enter exactly 4 arguments for instruction. You entered %d\n\n",i);
      continue;
    } if (!strcmp(buf_chunks[0],"end")) {
      printf("Goodbye!\n");
      break; // Exit while loop
    } else if (!strcmp(buf_chunks[0], "status")) {
      printf("\n\tMemory: \n");
      for (int i = 0; i < PAGE_LIMIT; i++) {
	printf("Page %d:   ",i);
	for (int j = 16*i; j < 16*(i+1); j++)
	  printf("%d ",memory[j]);
	printf("\n");
      }
      printf("\n\tDisk: \n");
      for (int i = 0; i < PAGE_LIMIT; i++) {
	printf("Sector %d:  ",i);
	for (int j = 16*i; j < 16*(i+1); j++)
	  printf("%d ", disk[j]);
	printf("\n");
      }
      printf("\n");
      continue; // Ask for next instruction
    }
    
    // Populate struct with converted tokens
    instruction instruc;
    instruc.pid = (int) strtol(buf_chunks[0], (char **)NULL, 10);
    instruc.type = buf_chunks[1];
    instruc.v_addr = (unsigned int) strtol(buf_chunks[2], (char **)NULL, 10);
    instruc.value = (unsigned int) strtol(buf_chunks[3], (char **)NULL, 10);

    if (instruc.pid > 3 || instruc.pid < 0 || instruc.v_addr > 63 || instruc.v_addr < 0) {
      printf("PID and virtual address must be greater than 0 and less than 4 and 64, respectively\n\n");
      continue;
    }

    if (!strcmp(instruc.type, "map")) {
      map(instruc);
    } else if (!strcmp(instruc.type, "store")) {
      store(instruc);
    } else if (!strcmp(instruc.type, "load")) {
      load(instruc);
    } else {
      printf("\"%s\" is not a valid instruction.\n",instruc.type);
    }
    printf("\n");
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
  // And if whether it currently resides in memory or disk
  instruc.v_addr = (instruc.v_addr / 16) * 16; // Convert virtual address to appropriate multiple of 16
  for (int i = 0; i < 16; i++) {
    if (memory[i]) {  // Will be non zero if containing a page table entry struct
      pte* temp = (pte*) &memory[i];
      if (temp->pid == instruc.pid && temp->vpn == instruc.v_addr / 16) {
	printf("Virtual page %d is already mapped to this process\n",temp->vpn);
	return -1;
      } else if (temp->pid == instruc.pid) 
	first_map = 0;
    }
  }
  
  pte our_entry = {.io = 1, .pid = instruc.pid, .rw = instruc.value}; // Page table entry for this map

  int found = 0;
  for (int j = 0; j < MAX_PROCESSES; j++) {
    if (free_list[j] > -1) {
      our_entry.vpn = instruc.v_addr / 16;
      our_entry.pfn = j;
      found++;
      break;
    }
  } if (!found) {
    for (int i = 0; i < MAX_PROCESSES; i++)
      if (disk_free[i] > -1) {
	swap( ((pte*) &memory[next_evict-1]), disk_free[i]);
	break;
      }
    return map(instruc);
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

  free_list[our_entry.pfn] = -1;
    
  return 0;
}

int store(instruction instruc) {

  // First check if value is too big or negative
  if (instruc.value < 0 || instruc.value > 255) {
    printf("Value out of range\n");
    return 1;
  }

  int in_memory;
  
  // Now try to find corresponding page table entry for this PID and virtual address
  // and if it currently in disk or memory
  pte* our_entry;
  int entry_index = -1;
  for (int i = 0; i < 16; i++) {
    if (memory[i]) {
      pte* entry = (pte*) &memory[i];
      if (entry->pid == instruc.pid && entry->vpn == instruc.v_addr / 16) {
	entry_index = i;
	our_entry = entry;
	in_memory = our_entry->io;
	break;
      }
    }
  }

  if (entry_index == -1) {
    printf("Page Fault: Virtual address %d has not been mapped to process %d\n", instruc.v_addr, instruc.pid);
    return -1;
  } else if (!our_entry->rw) {
    printf("Writes are not allowed to this page\n");
    return -1;
  }

  // If not in memory, swap it in
  if (!in_memory) {
    int in_use = 0;
    for (int i = 0; i < 16; i++) {
      if (memory[i]) {  // Will be non zero if containing a page table entry struct
	pte* temp = (pte*) &memory[i];
	if (temp->pfn == next_evict) {
	  in_use = 1;
	  break;
	}
      }
    }
    // If next page in evict order is in use, swap it first so it
    // is not overwritten
    if (in_use) {
      pte* old_page = (pte*) &memory[next_evict];
      int free_disk_offset;

      for (int i = 0; i < PAGE_LIMIT * PAGE_LIMIT; i++)
	if (disk_free[i] > -1) {
	  free_disk_offset = disk_free[i];
	  break;
	}
      
      swap(old_page, free_disk_offset);
    }
    swap(our_entry, free_list[next_evict]);
    return store(instruc);
  }
  
  int p_addr = (16 * our_entry->pfn) + (instruc.v_addr % 16); // Perform address translation
  memory[p_addr] = instruc.value;
  printf("Stored value %d at virtual address %d (physical address %d)\n",instruc.value,instruc.v_addr,p_addr);

  return 0;
}

int load(instruction instruc) {

  int in_memory = 1;
  
  // Try to find corresponding page table entry for this PID and virtual address
  pte* our_entry;
  int entry_index = -1;
  for (int i = 0; i < 16; i++) {
    if (memory[i]) {
      pte* entry = (pte*) &memory[i];
      if (entry->pid == instruc.pid && entry->vpn == instruc.v_addr / 16) {
	entry_index = i;
	our_entry = entry;
	in_memory = our_entry->io;
	break;
      }
    }
  }

  if (entry_index == -1) {
    printf("Page Fault: Virtual address %d has not been mapped to process %d\n", instruc.v_addr, instruc.pid);
    return -1;
  }

    // If not in memory, swap it in
  if (!in_memory) {
    int in_use = 0;
    for (int i = 0; i < 16; i++) {
      if (memory[i]) {  // Will be non zero if containing a page table entry struct
	pte* temp = (pte*) &memory[i];
	if (temp->pfn == next_evict) {
	  in_use = 1;
	  break;
	}
      }
    }
    // If next page in evict order is in use, swap it first so it
    // is not overwritten
    if (in_use) {
      pte* old_page = (pte*) &memory[next_evict];
      int free_disk_offset;

      for (int i = 0; i < PAGE_LIMIT * PAGE_LIMIT; i++)
	if (disk_free[i] > -1) {
	  free_disk_offset = disk_free[i];
	  break;
	}
      
      swap(old_page, free_disk_offset);
    }
    swap(our_entry, free_list[next_evict]);
    return load(instruc);
  }

  int p_addr = (16 * our_entry->pfn) + (instruc.v_addr % 16); // Perform address translation

  if (memory[p_addr] == 0) {
    printf("0 found at virtual address %d (physical address %d): Could be empty or 0...\n", instruc.v_addr, p_addr);
    return -1;
  }

  printf("The value %d is virtual address %d (physical address %d)\n", memory[p_addr], instruc.v_addr, p_addr);
  return 0;
}

int swap(pte *entry, int new_offset) {
  
  // Mark as free lists unused
  int prev_offset = entry->pfn * 16;
  int to_disk = entry->io;
  
  if (to_disk) {
    free_list[entry->pfn] = 16 * entry->pfn;
  } else {
    disk_free[entry->pfn] = 16 * entry->pfn;
  }
  
  // Write page contents from memory/disk to start of swap file
  fseek(swap_space,0,SEEK_SET);
  for (int i = prev_offset; i < prev_offset + 16; i++) {
    if (to_disk) {
      fputc(memory[i], swap_space);
      memory[i] = 0; // After copying, erase in memory
    } else {
      fputc(disk[i], swap_space);
      disk[i] = 0; // After copying, erase in disk
    }
  }
  
  // Now transfer contents from file to disk/memory
  fseek(swap_space,0,SEEK_SET);
  unsigned char ch;
  for (int i = new_offset; (ch = fgetc(swap_space)) != '\n' && i < new_offset + 16; i++) {
    if (to_disk) {
      disk[i] = ch;
    } else 
      memory[i] = ch;
  }

  // Update page table entry fields
  if (to_disk) entry->io = 0;
  else entry->io = 1;
  
  int old_pfn = entry->pfn;
  entry->pfn = new_offset / 16;

  // Update free lists appropriately
  if (to_disk) {
    disk_free[entry->pfn] = -1;
  } else {
    free_list[entry->pfn] = -1;
  }
  
  // Adjust next_evict to follow round-robin eviction strategy
  if (next_evict == 3) next_evict = 1;
  else next_evict++;
  
  // Notify user and exit
  if (to_disk) {
    printf("Swapped frame %d into disk at %d\n", old_pfn, new_offset);
    return 0;
  } else {
    printf("Swapped disk offset %d into frame %d\n", prev_offset, entry->pfn);
    return 0;
  }

}

