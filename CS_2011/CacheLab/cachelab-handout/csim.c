#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "cachelab.h"

/* csim.c - Cache simulator
 * Written by Adam Camilli (aocamilli) for CS 2011
 * 4/22/16
 */


/***************************************
 ******** Custom Types/Globals *********
 ***************************************/

/* 64-bit wide variable to hold memory addresses*/
typedef unsigned long long int mem_addr_t;

/* Holds all attributes of cache given by user:
 *
 * -v     - verbose (0 or 1)
 * -s <s> - s (<s>), num_sets (2**s) 
 * -E <E> - num_lines (<E>)
 * -b <b> - b (<b>), block_size (2**b)
 */ 
typedef struct {
  int s; // 2**s cache sets 
  int b; // cacheline block size 2**b bytes 
  int E; // number of cachelines per set 
  int S; // number of sets, derived from S = 2**s 
  int B; // cacheline block size (bytes), derived from B = 2**b 

  int hits;
  int misses;
  int evictions;
} cache_attr_t;

/* Represents a cache line, composed of cache blocks with two attributes:
 * 
 * last_used - most recently used line
 * valid - whether data of blocks is valid
 * tag - Memory address which also serves as a unique identifer for this line
 */
typedef struct {
  int last_used;     // When this line was last used
  int valid;         // Whether data in block is valid 
  mem_addr_t tag;    // Unique identifer (starting address) for this line
  char *block;       // Data (irrelevant in this assignment, but dataless lines would be kinda useless...)
} set_line_t;

/* Represents a cache set, composed of cache lines */
typedef struct {
  set_line_t *lines;
} cache_set_t;

/* Represents a cache, composed of cache sets */
typedef struct {
  cache_set_t *sets;
} cache_t;

int verbose; // Are we being verbose today?

int step; // Used to calculate LRU for cache replacement



/*************************************
 ******** Custom Functions ***********
 *************************************/

/* printUsage - Print usage info
 *
 * input - argv
 */
void printUsage(char** input) {
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", input[0]);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n");
  printf("Examples:\n");
  printf("  %s -s 6 -E 2 -b 8 -t traces/yi.trace\n",input[0]);
  printf("  %s -v -s 4 -E 1 -b 16 -t traces/yi.trace\n",input[0]);
  printf("  %s -h [arbitrary other input]\n",input[0]);
  exit(0);
}

/* Builds cache according to specified attributes: 
 *
 * num_sets   - 2**<s>, number of sets in cache
 * num_lines  - number of lines per set
 * block_size - 2**<b>, size of each block in line
 */
cache_t build_cache(long long num_sets, int num_lines, long long block_size) {
  cache_t new_cache;
  cache_set_t set;
  set_line_t line;
  new_cache.sets = (cache_set_t *) malloc(sizeof(cache_set_t) * num_sets);

  for (int i = 0; i < num_sets; i ++) {
    set.lines =  (set_line_t *) malloc(sizeof(set_line_t) * num_lines);
    new_cache.sets[i] = set;
    for (int j = 0; j < num_lines; j ++) {
      line.last_used = 0;
      line.valid = 0;
      line.tag = 0;
      set.lines[j] = line;
    }
  }

  return new_cache;
}

/* clear_cache - Frees memory allocated to cache sets and lines when cache was built.
 * Only called at the end of the program.
 *
 * cache      - cache to clear
 * num_sets   - 2**<s>, number of sets in cache
 * num_lines  - number of lines per set
 * block_size - 2**<b>, size of each block in line

 */
void clear_cache(cache_t cache, long long num_sets, int num_lines, long long block_size) {
  for (int i = 0; i < num_sets; i ++) {
      cache_set_t set = cache.sets[i];
      if (set.lines != NULL) 
	free(set.lines);
  }

  if (cache.sets != NULL) 
    free(cache.sets);
}

/* find_empty_line - Helper function for simulator, finds empty line in a cache set.
 * 
 * set  - Set we are searching in
 * attr - Attributes of cache whose set we are searching in 
 */
int find_empty_line(cache_set_t set, cache_attr_t attr) {
  int num_lines = attr.E;
  set_line_t line;

  for (int i = 0; i < num_lines; i++) {
    line = set.lines[i];
    if (line.valid == 0) {
      return i;
    }
  }
  // No empty line found
  return -1;
}

/* find_evict_line - Returns index of least recently used line, which according to LRU policy
 * is the one we should evict. Helper function for simulator.
 *
 * set  - Set we are searching in
 * attr - Attributes of cache whose set we are searching in 
 * extrema - Value proivded by calling function to hold least used ([0]) 
 *           and most used ([0]) for the convenience of caller.
 */
int find_evict_line(cache_set_t set, cache_attr_t attr, int *extrema) {
  int num_lines = attr.E;
  int max_used = set.lines[0].last_used;
  int min_used = set.lines[0].last_used;
  int min_used_index = 0;

  set_line_t line;
  for (int i = 1; i < num_lines; i++) {
    line = set.lines[i];
    if (min_used >= line.last_used) {
      min_used_index = i;
      min_used = line.last_used;
    }
    if (max_used <= line.last_used) 
      max_used = line.last_used;
  }

  extrema[0] = min_used;
  extrema[1] = max_used;
  return min_used_index;
}

/* simulate_cace - Run simulation of the hit/miss behavior of a cache with given attributes on
 * on a given trace
 *
 * cache - Cache we are simulating
 * set   - Set we are searching in
 * attr  - Attributes of cache whose set we are searching in  
 */
cache_attr_t simulate_cache(cache_t cache, cache_attr_t attr, mem_addr_t address) {
  int cache_is_full = 1;    // Cache has no empty lines
  int num_lines = attr.E;
  int prev_hits = attr.hits;

  int tag_size = (64 - (attr.s + attr.b));
  mem_addr_t input_tag = address >> (attr.s + attr.b);
  unsigned long long set_index = (address << (tag_size)) >> (tag_size + attr.b);
  cache_set_t set = cache.sets[set_index];

  // Locate hits and if cache is full or not
  for (int i = 0; i < num_lines; i++) {
    set_line_t line = set.lines[i];
    if (line.valid) {
      if (line.tag == input_tag) {
        attr.hits++; // We hit something!
	step++; // Increment global step variable
	line.last_used = step;
	if (verbose)
	  printf("hit ");
	set.lines[i] = line;
      }
    } else if (!(line.valid) && (cache_is_full)) // Found an empty line, cache ain't full...
      cache_is_full = 0;
  }

  /* If wrong mount of hits found we missed something, else we're done! */
  if (prev_hits == attr.hits) {
    // Miss. D'oh!
    attr.misses++;
    if (verbose)
      printf("miss ");
  } else 
    return attr;

  /* We missed, so evict where we need to and write data into cache */
  int *extrema = (int*) malloc(sizeof(int) * 2); // [0] = least used, [1] = most used
  int min_used_index = find_evict_line(set, attr, extrema);
  if (cache_is_full) {
    attr.evictions++;
    if (verbose)
      printf("eviction ");
    // Found LRU, overwrite it.
    set.lines[min_used_index].tag = input_tag;
    set.lines[min_used_index].last_used = extrema[1] + 1;
  } else {
    int empty_index = find_empty_line(set, attr);  
    // Found first empty line, write to it.
    set.lines[empty_index].tag = input_tag;
    set.lines[empty_index].valid = 1;
    set.lines[empty_index].last_used = extrema[1] + 1;
  }

  free(extrema);
  return attr;
}


int main(int argc, char **argv) {
  // Cache variables
  cache_t cache;
  cache_attr_t attr;
  memset(&attr, '\0', sizeof(attr));

  // Trace variables
  FILE *trace;
  char cmd;
  mem_addr_t address;
  int size;

  /* Parse input using handy getopt function */
  char *trace_file;
  char ch;
  while((ch = getopt(argc,argv,"s:E:b:t:vh")) != -1) {
    switch(ch) {
    case 's':
      attr.s = atoi(optarg);
      attr.S = 1 << attr.s; // Number of sets = 2**s
      break;
    case 'E':
      attr.E = atoi(optarg);
      break;
    case 'b':
      attr.b = atoi(optarg); // Block size = 2**b
      attr.B = 1 << attr.b;
      break;
    case 't':
      trace_file = optarg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      printUsage(argv);
      exit(1);
    default:
      printUsage(argv);
      exit(1);
    }
  }

  /* Check if any input is missing or invalid */
  if (attr.s <= 0 || attr.E <= 0 || attr.b <= 0 || trace_file == NULL) {
    printf("Missing or invalid required command line argument\n");
    printUsage(argv);
    exit(0);
  }

  /* Finish setting the rest of our attributes and build cache */
  attr.hits = 0;
  attr.misses = 0;
  attr.evictions = 0;
  cache = build_cache(attr.S, attr.E, attr.B);

  /* Initiate global step variable for calculating LRU */
  step = 0;
  
  /* Now parse lines from trace to miss, hit, and evict like a real cache (sort of) */
  trace = fopen(trace_file, "r");
  if (trace != NULL) {
    while (fscanf(trace, " %c %llx,%d", &cmd, &address, &size) == 3) {
      switch(cmd) {
      case 'I': // Ignore instruction loads, shouldn't get them anyway due to space in format string
	break;
      case 'L': // Load
	if (verbose)
	  printf("L %llx,%d ", address,size);
	attr = simulate_cache(cache, attr, address);
	if (verbose)
	  printf("\n");
	break;
      case 'S': // Store
	if (verbose)
	  printf("S %llx,%d ", address,size);
	attr = simulate_cache(cache, attr, address);
	if (verbose)
	  printf("\n");
	break;
      case 'M': // Modify, must do two simulations
	if (verbose)
	  printf("M %llx,%d ", address,size);
	attr = simulate_cache(cache, attr, address);
	attr = simulate_cache(cache, attr, address);
	if (verbose)
	  printf("\n");
	break;
      default:
	break;
      }
    }
  }

  /* And we out */
  printSummary(attr.hits, attr.misses, attr.evictions);
  clear_cache(cache, attr.S, attr.E, attr.B);
  fclose(trace);
  return 0;
}
