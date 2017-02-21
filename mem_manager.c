#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 64
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
char page_table[SIZE / PAGE_LIMIT];
int used_pids[PAGE_LIMIT] = {-1,-1,-1,-1};

int map(instruction instruc);
int store(instruction instruc);

int main (int argc, char *argv[]) {

  srand(4);

  page pg1 = {.start = 0, .offset = 15, .frame = 0};
  page pg2 = {.start = 16, .offset = 31, .frame = 1};
  page pg3 = {.start = 32, .offset = 47, .frame = 2};
  page pg4 = {.start = 48, .offset = 63, .frame = 3};

  free_list = malloc(sizeof(page*)*4);
  
  free_list[0] = &pg1; // Reserved for page table
  free_list[1] = &pg2;
  free_list[2] = &pg3;
  free_list[3] = &pg4;

  instruction page_table_map = {.pid = 0, .type="map", .v_addr = 0, .value = 1};
  map(page_table_map);
  
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
  // Check if value field is correct for mapping (0 for read or 1 for write)
  if (instruc.value > 1 || instruc.value < 0) {
    printf("Needs a value of 0 or 1.\n");
    return 1;
  }

  // Check if pid is already in use
  int i = 0;
  for (; i < PAGE_LIMIT; i++) {
    if (instruc.pid == used_pids[i]) {
      printf("PID %d is already in use!\n",instruc.pid);
      return 1;
    }
  }
  
  // Try to locate two empty pages: If they cannot be found, we don't have enough to map
  int j = 0;
  int pg_process = -1;
  
  for (i = 0; i < 4; i++) {
    //printf("freelist: %i", free_list[i]->start);
    if (free_list[i] != NULL) {
      j++;
      //printf("j: %i", j); 
      pg_process = i;
      break;
    }
  }
  if (j < 1) {
    printf("Not enough pages left!\n");
    return 1;
  }

  used_pids[i] = instruc.pid; // Add PID to list of PIDS in use
  
  int frame = free_list[pg_process]->frame;
  if (frame == 0)
    printf("Mapped page table into physical frame %d.\n", frame); // Frame 0 is reserved for page table
  else 
    printf("Mapped virtual address %d (page %d) into physical frame %d.\n", instruc.v_addr, pg_process, frame);

  // Put information in page table
  // Usage of frame is accurate for page_table indices because pages are allocated to processes in ascending order (FIFO)
  if (frame > 0) { // We don't care about info regarding page table itself
    char info_byte = instruc.pid + frame + instruc.v_addr + instruc.value;
    page_table[frame-1] = info_byte; 
    printf("Placed info byte %d into %d on page table\n", info_byte, frame-1);
    for(i = free_list[frame]->start; i < free_list[frame]->offset; i++) {
      memory[i] = page_table[i];
    }
  }
  
  free_list[frame] = NULL; // Mark page as used

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
    printf("Hasn't been mapped yet.\n");
  }
  
  if(frame == 1){
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


