#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 64
#define PAGE_LIMIT 4
#define MAX_PROCESSES 4
#define SEED 4

typedef struct page {
  int start; 
  int offset;
  int frame;
  int used;
} page;

typedef struct instruction {
  int pid;              // [0-3]
  char *type;           // map, store, and load
  unsigned int v_addr;  // [0-63]
  unsigned int value;            // [0-255]
} instruction;

char memory[SIZE];
page **free_list;
int registers[MAX_PROCESSES];

int map(instruction instruc);
int store(instruction instruc);
int load(instruction instruc);

int main (int argc, char *argv[]) {

  srand(4);
  
  // Create the physical pages
  page pg1 = {.start = 0, .offset = 15, .frame = 0, .used = 0};
  page pg2 = {.start = 16, .offset = 31, .frame = 1, .used = 0};
  page pg3 = {.start = 32, .offset = 47, .frame = 2, .used = 0};
  page pg4 = {.start = 48, .offset = 63, .frame = 3, .used = 0};

  // Place them on free list
  free_list = malloc(sizeof(page*) * 4);
  free_list[0] = &pg1;
  free_list[1] = &pg2;
  free_list[2] = &pg3;
  free_list[3] = &pg4;

  // Mark registers and memory as unused
  for (int i = 0; i < MAX_PROCESSES; i++)
    registers[i] = -1; 
  for (int i = 0; i < SIZE; i++)
    memory[i] = -1; 
  
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
      map(instruc);
    } else if (!strcmp(instruc.type, "store")) {
      store(instruc);
    } else if (!strcmp(instruc.type, "load")) {
      load(instruc);
    } else {
      printf("\"%s\" is not a valid instruction.\n",instruc.type);
    }
  }

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
    memory[pgtable_start] = instruc.pid;       // Our PID
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
    memory[free_index] = instruc.pid;       // Our PID
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

  // Now check if value is too big or negative
  if (instruc.value < 0 || instruc.value > 127) {
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
    if (memory[i+1] ==  instruc.v_addr / 16) { // if virtual address is part of an existing virtual page
      p_addr = (16 * memory[i+2]) + (instruc.v_addr % 16); // translate address
      break;
    }
  } if (p_addr < 0 || memory[p_addr] < 0) {
    printf("Virtual address %d is not mapped for process %d\n", instruc.v_addr, instruc.pid);
    return -1;
  }

  printf("The value %d is virtual address %d (physical address %d)\n", memory[p_addr], instruc.v_addr, p_addr);

  return memory[p_addr];  
}
