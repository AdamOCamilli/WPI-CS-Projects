#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 64
#define PAGE_LIMIT 4
#define MAX_PROCESSES 4
#define SEED 4

typedef struct page {
  int owner; // PID that has this page mapped to it
  int start; 
  int offset;
  int frame;
  int used;
} page;

typedef struct instruction {
  int pid;              // [0-3]
  char *type;           // map, store, and load
  unsigned int v_addr;  // [0-63]
  unsigned int value;   // [0-255]
} instruction;

unsigned char memory[SIZE];
unsigned char disk[SIZE*SIZE];
FILE* swap_space;
page **free_list;
int registers[MAX_PROCESSES];
int disk_offsets[MAX_PROCESSES * 2];
int next_evict;                      // Which page to evict in the event of a swap

int map(instruction instruc);
int store(instruction instruc);
int load(instruction instruc);
int swap(int from, int to, int where);

int main (int argc, char *argv[]) {

  srand(4);
  
  // Create the physical pages
  page pg1 = {.owner = -1, .start = 0, .offset = 15, .frame = 0, .used = 0};
  page pg2 = {.owner = -1, .start = 16, .offset = 31, .frame = 1, .used = 0};
  page pg3 = {.owner = -1, .start = 32, .offset = 47, .frame = 2, .used = 0};
  page pg4 = {.owner = -1, .start = 48, .offset = 63, .frame = 3, .used = 0};

  // Place them on free list
  free_list = malloc(sizeof(page*) * 4);
  free_list[0] = &pg1;
  free_list[1] = &pg2;
  free_list[2] = &pg3;
  free_list[3] = &pg4;
  
  // Mark registers and memory as unused
  for (int i = 0; i < MAX_PROCESSES; i++)
    registers[i] = -1;

  // Open swap file in append mode
  swap_space = fopen("swap_space.txt","a");
  
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
      if (!map(instruc)) {
	switch(instruc.pid) {
	case 0:
	  pg1.owner = instruc.pid;
	  break;
	case 1:
	  pg2.owner = instruc.pid;
	  break;
	case 2:
	  pg3.owner = instruc.pid;
	  break;
	case 3:
	  pg4.owner = instruc.pid;
	  break;
	}
      }
    } else if (!strcmp(instruc.type, "store")) {
      store(instruc);
    } else if (!strcmp(instruc.type, "load")) {
      load(instruc);
    } else {
      printf("\"%s\" is not a valid instruction.\n",instruc.type);
    }
  }

  fclose(swap_space);
  free(free_list);
  return 0;
}

int map(instruction instruc) {
  
  // Check if value field is correct for mapping (0 for read or 1 for write)
  if (instruc.value > 1 || instruc.value < 0) {
    printf("Value bit must be 0 or 1 for mapping.\n");
    return 1;
  }

  // Check if virtual address has already been mapped for this process
  if (registers[instruc.pid] > -1) {
    for (int i = registers[instruc.pid] + 1; i < registers[instruc.pid] + 13; i += 4) {
      if (memory[i] == instruc.v_addr / 16) {
	printf("Virtual address %d has already been mapped to physical frame %d for process %d\n", instruc.v_addr, memory[i+1], instruc.pid);
	return 1;
      }
    }
  }

  // Convert virtual address to appropriate multiple of 16
  instruc.v_addr = (instruc.v_addr / 16) * 16;
  // Check if process already has a page table
  int no_page_table = 0; 
  if (registers[instruc.pid] >= 0)
    no_page_table = 1;
  
  
  int pgtable_pfn = -1; // Physical frame number for page mapped to page table if it doesn't already exist 
  int process_vpn = -1; // Virtual page number for page mapped to this process
  int process_pfn = -1;
  // If page table is already allocated, only one page is needed from free list
  if (no_page_table) {
    for (int i = 0; i < PAGE_LIMIT; i++) {
      if (!free_list[i]->used) 
	if (process_vpn < 0) {
	  process_pfn = i;
	  process_vpn = instruc.v_addr / 16;
	}
    } if (process_vpn < 0) {
      printf("Not enough pages left!\n");
      return 1;
    }
  } else { // Else if page table is needed, we need to locate two pages on free list
   for (int i = 0; i < PAGE_LIMIT; i++) {
      //printf("freelist: %i", free_list[i]->start);
      if (!free_list[i]->used) { 
	if (pgtable_pfn < 0) 
	  pgtable_pfn = i;
	else if (process_vpn < 0)  {
	  process_pfn = i;
	  process_vpn = instruc.v_addr / 16;
	}
      }
   } if (process_pfn < 0 || pgtable_pfn < 0 || process_vpn < 0) {
	printf("Not enough pages left!\n");
	return 1;
   }
  }

  // Place page table entries in newly created page table, and place index in memory where page table starts
  // into the index of register array corresponding to our PID
  if (!no_page_table) {
    int pgtable_start = free_list[pgtable_pfn]->start;
    registers[instruc.pid] = pgtable_start;
    
    // Store map info in page table
    memory[pgtable_start] = 4 + instruc.pid;                 // Stored in memory
    memory[pgtable_start + 1] = process_vpn;   // Our VPN
    memory[pgtable_start + 2] = process_pfn;   // PFN corresponding to VPN
    memory[pgtable_start + 3] = instruc.value; // Read/write
    
    printf("Put page table for PID %d into physical frame %d\n", instruc.pid, pgtable_pfn);
    printf("Mapped virtual address %d (page %d) to physical frame %d\n", instruc.v_addr, process_vpn, process_pfn);
  } else { // else we must find page table of this process
    // Locate start of our page table
    int pgtable_start = registers[instruc.pid];
    
    // Find empty index in memory
    int free_index;
    for (int i = pgtable_start; i < pgtable_start + 12; i += 4) 
      if (memory[i] = -1) {
	  free_index = i;
	  break;
      }

    // Store map info in page table
    memory[free_index] = 4 + instruc.pid;   // Stored to memory
    memory[free_index + 1] = process_vpn;   // Our VPN
    memory[free_index + 2] = process_pfn;   // PFN corresponding to VPN
    memory[free_index + 3] = instruc.value; // Read/write 

    printf("Mapped virtual address %d (page %d) to physical frame %d\n",instruc.v_addr, process_vpn, process_pfn);
  }

  if (pgtable_pfn > -1)
    free_list[pgtable_pfn]->used = 1; // Mark page as used
  free_list[process_pfn]->used = 1; // Mark page as used

  return 0;
}
 
int store(instruction instruc) {

  int pgtable_start = registers[instruc.pid];
  // First check if process has any pages allocated at all
  if (pgtable_start == -1) {
    printf("No pages have been mapped to process %d\n", instruc.pid);
    return 1;
  }

  // Now check if we are allowed to write to this page
  if (!memory[pgtable_start+3]) {
    printf("Virtual page %d (physical frame %d) for process %d is not writeable\n", memory[pgtable_start+1], memory[pgtable_start+2], instruc.pid);
    return 1;
  }

  // Now check if value is too big or negative
  if (instruc.value < 0 || instruc.value > 255) {
    printf("Value out of range\n");
    return 1;
  }
  
  int p_addr = -1; // The physical address where we will store the value

  // Determine if address translation is possible with frame(s) currently mapped to process
  for(int i = pgtable_start; i < pgtable_start + 16; i += 4) {
    if (memory[i+1] ==  instruc.v_addr / 16) { // if virtual address is part of an existing virtual page
      p_addr = (16 * memory[i+2]) + (instruc.v_addr % 16); // translate address
      break;
    }
  } if (p_addr < 0) {
    printf("Virtual address %d is not mapped for process %d\n", instruc.v_addr, instruc.pid);
    return 1;
  } 

  memory[p_addr] = instruc.value;
  
  printf("Stored value %d at virtual address %d (physical address %d)\n", memory[p_addr], instruc.v_addr, p_addr);

  return 0;
}

int load(instruction instruc){
  
  int pgtable_start = registers[instruc.pid];
  // First check if process has any pages allocated at all
  if (pgtable_start == -1) {
    printf("No pages have been mapped to process %d\n", instruc.pid);
    return -1;
  }
  
  int p_addr = -1; // The physical address where we will store the value

  // Determine if address translation is possible with frame(s) currently mapped to process
  for(int i = pgtable_start; i < pgtable_start + 16; i += 4) {
    if (memory[i] ==  instruc.pid + 4) {  // if this page belongs to this pid
      p_addr = (16 * memory[i+2]) + (instruc.v_addr % 16); // translate address
      break;
    }
  } if (p_addr < 0) {
    printf("Virtual address %d is not mapped for process %d\n", instruc.v_addr, instruc.pid);
    return -1;
  } else if (memory[p_addr] == 0) {
    printf("0 found at virtual address %d (physical frame %d): Could be empty or 0...\n", memory[pgtable_start+1], memory[pgtable_start+2]);
    return -1;
  }

  printf("The value %d is virtual address %d (physical address %d)\n", memory[p_addr], instruc.v_addr, p_addr);

  return memory[p_addr];  
}

int swap(int from, int to, int to_disk) { // where = 0 -> disk, where = 1 -> memory

  if (from % 16) {
    printf("Bad from %d was passed to swap\n", from);
    return -1;
  }

  // Check if trying to swap an empty page 
  if (to_disk) {
    if (!memory[from]) {
      printf("Tried to swap empty frame %d in memory\n", from / 16);
      return -1;
    }
  } else {
    if (!disk[from + 2]) { // Will represent a PFN, should always be greater than 0 if in use
      printf("Tried to swap from from %d in disk to frame %d in disk, but it was empty\n", from, to / 16);
      return -1;
    }
  }

  // Clear registers and (if memory) mark as unused in free_list
  if (to_disk) {
    free_list[memory[from] - 4]->used = 0;
    registers[memory[from] - 4] = -1;
  } else disk_offsets[disk[from]] = -1;
  
  // Append page contents from memory/disk to end of swap file
  int start = from;
  int file_start = (int) ftell(swap_space);

  while (from < start + 16) {
    if (to_disk) {
      fputc((int) memory[from++], swap_space);
      memory[from - 1] = 0;
    } else {
      fputc((int) disk[from++], swap_space);
      disk[from - 1] = 0;
    }
  } fputc('\n', swap_space); // For readability
  
  // Now transfer contents from file to disk/memory

  start = to;
  fseek(swap_space, file_start, SEEK_SET); // get back to where we began putting content into swap_space

  int ch;
  while ((ch = fgetc(swap_space)) != EOF && ch != '\n' && to < start + 16) {
    if (to_disk) {
      if (to == start)
	disk[to++] = (unsigned char) (ch - 4);
      else
	disk[to++] = (unsigned char) ch;
    } else { // assume a page has been allocated for us already
      if (to == start)
	memory[to++] = (unsigned char) (ch + 4);
      else
	memory[to++] = (unsigned char) (ch);
    }
  }

  // Check swap was successful (if so the whole program is basically messed up and we must Ctrl-C)
  if (ch == EOF) {
    printf("Somehow encountered end of file while swapping file index %d to %d\n", file_start, to);
    return -1;
  } else if (to != start + 16) {
    printf("Somehow went to %d from file from %d\n", to, file_start);
    return -1;
  }

  // Notify user and exit
  if (to_disk) {
    printf("Swapped frame %d to disk at %d\n", from / 16, to - 16);
    return 0;
  } else {
    printf("Swapped disk offset %d into frame %d\n", from, to / 16);
    return 0;
  }

}
    
