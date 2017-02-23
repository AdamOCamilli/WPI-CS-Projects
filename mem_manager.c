#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 64
#define PAGE_LIMIT 4
#define MAX_PROCESSES 2
#define SEED 4

// Byte-wide struct for page table entry
typedef struct entry {
  u_int8_t used:1;   // In use bit
  u_int8_t io:1;    // Memory/Disk bit
  u_int8_t rw:1;    // Read/Write bit
  u_int8_t vpn:2;   // Virtual process number
  u_int8_t pfn:2;   // Physical frame number
  u_int8_t pid:2;   // PID
} entry;

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
  int value;            // [0-255]
} instruction;

char memory[SIZE];
page **free_list;
entry registers[PAGE_LIMIT];

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

  // Place them on a free list
  free_list = malloc(sizeof(page*) * 4);
  free_list[0] = &pg1;
  free_list[1] = &pg2;
  free_list[2] = &pg3;
  free_list[3] = &pg4;

  for (int i = 0; i < PAGE_LIMIT; i++)
    registers[i].used= 0;
  
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
    unsigned int vaddr = (unsigned int) strtol(buf_chunks[2], (char **)NULL, 10);
    vaddr = (vaddr / 16) * 16; // Converts, for example, 21 to 16, 35 to 32, etc.
    
    instruc.pid = (int) strtol(buf_chunks[0], (char **)NULL, 10);
    instruc.type = buf_chunks[1];
    instruc.v_addr = vaddr;
    instruc.value = (int) strtol(buf_chunks[3], (char **)NULL, 10);

    if (!strcmp(instruc.type, "map")) {
      map(instruc);
    } else if (!strcmp(instruc.type, "store")) {
      store(instruc);
    } else if (!strcmp(instruc.type, "load")) {
      printf("Not yet implemented\n");
    } else {
      printf("\"%s\" is not a valid instruction.\n",instruc.type);
    }
  }

  free(free_list);
  return 0;
}

int map(instruction instruc) {
  int i = 0, j = 0;
  int no_page_table = 0; // Whether this is the first memory allocation for this process and it needs an
                         // additional page for page table

  // Check if PID already has page(s) mapped to it
  for (i = 0; i < PAGE_LIMIT; i++)
    if (registers[i].used && registers[i].pid == instruc.pid) {
      if (registers[i].vpn == instruc.v_addr + 1) {
	printf("Virtual address %d has already been allocated for process %d\n",instruc.v_addr,instruc.pid);
	return 1;
      }
      no_page_table++;
    }
  
  // Check if value field is correct for mapping (0 for read or 1 for write)
  if (instruc.value > 1 || instruc.value < 0) {
    printf("Value bit must be 0 or 1.\n");
    return 1;
  }  
  
  int process_vpn = -1; // Virtual page number for this process's page
  int pgtable_vpn = -1; // Virtual page number for process's page table
  // If page table is already allocated, only one free page is needed
  if (no_page_table) {
    for (i = 0; i < PAGE_LIMIT; i++) {
      if (!free_list[i]->used) 
	if (process_vpn < 0) 
	  process_vpn = (instruc.v_addr / 16) + 1; // +1 because VPN 0 belongs to page table
    } if (process_vpn < 0) {
      printf("Not enough pages left!\n");
      return 1;
    }
  } else { // Else if page table is needed we need to locate two pages on free list
   for (i = 0; i < PAGE_LIMIT; i++) {
      //printf("freelist: %i", free_list[i]->start);
      if (!free_list[i]->used) { 
	if (pgtable_vpn < 0) 
	  pgtable_vpn = (instruc.v_addr / 16);
	else if (process_vpn < 0) 
	  process_vpn = (instruc.v_addr / 16) + 1;
      }
    } if (process_vpn < 0 || pgtable_vpn < 0) {
	printf("Not enough pages left!\n");
	return 1;
    } 
  }

  // Notify user where we will map their PID and its page table  
  int process_pfn; // Physical frame number of page being mapped to process
  int pgtable_pfn; // Physical frame number of page being mapped to page table
  if (pgtable_vpn > -1) {
    pgtable_pfn = free_list[pgtable_vpn]->frame;
    printf("Mapped page table into physical frame %d.\n", pgtable_pfn);
  }
  process_pfn = free_list[process_vpn]->frame;
  printf("Mapped virtual address %d (page %d) into physical frame %d.\n", instruc.v_addr, (instruc.v_addr / 16), process_pfn);

  // Place page table entries in register, and the register index into memory
  if (!no_page_table) {
    // Create page table entry 
    entry our_entry = {.used = 1, .io = 0, .rw = instruc.value, .vpn = process_vpn, .pfn = process_pfn, .pid = instruc.pid};
    // Find empty register for our entry
    for (i = 0; i < PAGE_LIMIT; i++) 
      if (!registers[i].used) {
	registers[i] = our_entry;
	break;
      }

    // Place index in registers of our entry into memory 
    memory[free_list[pgtable_vpn]->start] = i;
  } else { // else we must find page table of this process
    // Create our page table entry
    entry our_entry = {.used = 1, .io = 0, .rw = instruc.value, .vpn = process_vpn, .pfn = process_pfn, .pid = instruc.pid};
    // Find empty register for our entry
    for (i = 0; i < PAGE_LIMIT; i++) 
      if (registers[i].pid == -1) {
	registers[i] = our_entry;
	break;
      }
    // Find where in memory what page our page table is located in
    int check;
    for (int k = 0; k < PAGE_LIMIT; k++) {
      check = memory[16*k];
      if (!registers[check].used)
	if (our_entry.vpn != registers[check].vpn && registers[check].pid == instruc.pid) 
	  break;
    }

    for (; check < SIZE; check++) 
      if (memory[check] == 0) {
	memory[check] = j; // index of our entry in registers
	break;
      }
  }

  if (pgtable_vpn > -1)
    free_list[pgtable_vpn]->used = 1; // Mark page as used
  free_list[process_vpn]->used = 1; // Mark page as used

  return 0;
}

int store(instruction instruc){
  int p_add = 0;
  int frame = 0;
  for(int i = 0; i < PAGE_LIMIT; i++){
    if (instruc.pid == registers[i].pid){
      frame = i;
    }
  }
  if(frame == 0){
    printf("No more space!\n");
  }
  
  if(frame == 1) {
    p_add = instruc.v_addr + 16;
  }
  else if(frame == 2){
    p_add = instruc.v_addr + 32;
  }
  else if(frame == 3){
    p_add = instruc.v_addr + 48;
  }

  printf("Stored value %d at virtual address %d (physical address %d)\n", instruc.value, instruc.v_addr, p_add);

  return 0;
}
/*
int load(instruction instruc){
  int value;
  int i,j;

  if (instruc.value != 0){
    prinf("The value field has to be zero!\n");
    return 1;
  }
  for(i = 0; i < 4; i++){
    if (instruc.pid == used_pids[i]){
      for(j = free_list[; j; j++){
	if (memory[j] == used_v_addr[j]){
	  value = instruc.v_addr;
	  printf("Load value %d at virtual address %d (physical address __)\n", value, instruc.v_addr);
	  return 0;
	}
      }
    }
  }
}
*/
