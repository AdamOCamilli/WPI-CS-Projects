#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 63
#define PAGE_LIMIT 4
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
  int value;            // int from [0-63]
} instruction;

char memory[SIZE];
page **free_list;

int map(instruction instruc);

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
      printf("Please enter exactly 4 arguments for instruction. %d\n",i);
      exit(1);
    }

    
    instruction instruc;
    instruc.pid = (int) strtol(buf_chunks[0], (char **)NULL, 10);
    instruc.type = buf_chunks[1];
    instruc.v_addr = (unsigned int) strtol(buf_chunks[2], (char **)NULL, 10);
    instruc.value = (int) strtol(buf_chunks[3], (char **)NULL, 10);

    if (!strcmp(instruc.type, "map")) {
      map(instruc);
    } else if (!strcmp(instruc.type, "store")) {
      printf("Not yet implemented\n");
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
  // Check if value field is correct for mapping (0 for read or 1 for write)
  if (instruc.value > 1 || instruc.value < 0) {
    printf("Needs a value of 0 or 1.\n");
    return 1;
  }

  // Try to locate two empty pages: If they cannot be found, we don't have enough to map
  int i;
  int j = 0;
  int pg_process = -1;
  int pg_pgtable = -1;
  
  for (i = 0; i < 4; i++) {
    //printf("freelist: %i", free_list[i]->start);
    if (free_list[i] != NULL) {
      j++;
      //printf("j: %i", j);
      if (pg_pgtable < 0) 
	pg_pgtable = i;
      else if (pg_process < 0 && i != pg_pgtable) 
	pg_process = i; 
    }
  }
  if (j < 2) {
    printf("Not enough pages left!\n");
    return 1;
  }
  
  char page_table[16];
  int page_num1 = free_list[pg_pgtable]->frame;
  int page_num2 = free_list[pg_process]->frame;
  page_table[0] = page_num1;
  printf("Put page table for PID %d into physical frame %d.\n", instruc.pid, page_num1);
  page_table[1] = page_num2;
  printf("Mapped virtual address %d (page %d) into physical frame %d.\n", instruc.v_addr, 0, page_num2); 

  for(i = free_list[pg_pgtable]->start; i < free_list[pg_pgtable]->offset; i++) {
    memory[i] = page_table[i];
  }
  free_list[pg_pgtable] = NULL; // Mark page as used
  free_list[pg_process] = NULL; // Mark page as used

  return 0;
}



  
