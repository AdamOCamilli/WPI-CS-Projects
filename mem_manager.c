#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 64
#define PAGE_LIMIT 4
#define MAX_PROCESSES 2
#define SEED 4

typedef struct page {
  int start; 
  int offset;
  int frame;
} page;

typedef struct instruction {
  int pid;              // [0-3]
  char *type;           // map, store, load
  unsigned int v_addr;  // [0-63]
  int value;            // int from [0-255]
} instruction;

char memory[SIZE];
page **free_list;
char page_table[SIZE / PAGE_LIMIT];
int used_pids[MAX_PROCESSES];

int map(instruction instruc);
int store(instruction instruc);
int load(instruction instruc);

int main (int argc, char *argv[]) {

  srand(4);

  page pg1 = {.start = 0, .offset = 15, .frame = 0};
  page pg2 = {.start = 16, .offset = 31, .frame = 1};
  page pg3 = {.start = 32, .offset = 47, .frame = 2};
  page pg4 = {.start = 48, .offset = 63, .frame = 3};

  free_list = malloc(sizeof(page*)*4);
  
  free_list[0] = &pg1;
  free_list[1] = &pg2;
  free_list[2] = &pg3;
  free_list[3] = &pg4;

  int k;
  for (k = 0; k < MAX_PROCESSES; k++)
    used_pids[k] = -1;
  
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
    } if (i != 4) { // If not exactly 4 args are passed in
      printf("Please enter exactly 4 arguments for instruction. You entered %d\n",i);
      exit(1);
    }

    // Populate struct with converted instruction tokens
    instruction instruc;
    instruc.pid = (int) strtol(buf_chunks[0], (char **)NULL, 10);
    instruc.type = buf_chunks[1];
    instruc.v_addr = (unsigned int) strtol(buf_chunks[2], (char **)NULL, 10);
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
  int no_page_table = 0; // If this is first memory allocation for this process and it needs two pages
  
  // Check if value field is correct for mapping (0 for read or 1 for write)
  if (instruc.value > 1 || instruc.value < 0) {
    printf("Value bit must be 0 or 1.\n");
    return 1;
  }

  // Check if process already has memory allocated (and therefore has a page table already)
  int i = 0, j = 0;
  int temp;
  for (; i < MAX_PROCESSES; i++) 
    if (instruc.pid == used_pids[i]) 
      no_page_table++;
  
  int pg_process = -1;
  int pg_pgtable = -1;
  // If page table is already allocated, only one free page is needed
  if (no_page_table) {
    for (i = 0; i < PAGE_LIMIT; i++) {
      if (free_list[i] != NULL) 
	if (pg_process < 0) 
	  pg_process = i;
    } if (pg_process < 0) {
      printf("Not enough pages left!\n");
      return 1;
    }
  } else { // Else if page table is needed we need to locate two pages on free list
   for (i = 0; i < PAGE_LIMIT; i++) {
      //printf("freelist: %i", free_list[i]->start);
      if (free_list[i] != NULL) { 
	if (pg_pgtable < 0) 
	  pg_pgtable = i;
	else if (pg_process < 0) 
	  pg_process = i;
      }
    } if (pg_process < 0 || pg_pgtable < 0) {
	printf("Not enough pages left!\n");
	return 1;
    } 
  }

  // Mark PID as used
  for (i = 0; i < MAX_PROCESSES; i++)
    if (used_pids[i] < 0)
      used_pids[i] = instruc.pid;

  // Notify user where we have mapped their PID and its page table  
  int pgtable_frame;
  int process_frame;
  if (pg_pgtable > -1) {
    pgtable_frame = free_list[pg_pgtable]->frame;
    printf("Mapped page table into physical frame %d.\n", pgtable_frame);
  }
  process_frame = free_list[pg_process]->frame;
  printf("Mapped virtual address %d (page %d) into physical frame %d.\n", instruc.v_addr, (instruc.v_addr / 16), process_frame);

  // Put information in first free spot on page table
  if (pg_pgtable > -1) { 
    char info_byte = instruc.v_addr + process_frame;
    for (i = free_list[pg_pgtable]->start; i < free_list[pg_pgtable]->offset; i++) {
      if (!memory[i]) {
	memory[i] = info_byte;
	break;
      }
    }
    printf("Placed info byte %d into %d on page table\n", info_byte, pgtable_frame);
  }

  if (pg_pgtable > -1)
    free_list[pg_pgtable] = NULL; // Mark page as used
  free_list[pg_process] = NULL; // Mark page as used

  return 0;
}

int store(instruction instruc){
  int p_add = 0;
  int frame = 0;
  for(int i = 0; i < 4; i++){
    if (instruc.pid == used_pids[i]){
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
