Adam Camilli
Sydney Fisher

Part 1:
     Page table implementation:
     	  Our page table implementation made use of a byte-wide struct to represent the page table entries:

	  typedef struct pte {
	  	  u_int8_t io:1;  // In disk or memory
		  u_int8_t pid:2; // PID
		  u_int8_t vpn:2; // Virtual page number
		  u_int8_t pfn:2; // Physical frame number
		  u_int8_t rw:1;  // Read or read/write
	  } pte;

	  In order to store this struct inside a char in memory, we use memcpy() with a pointer to a pte struct and one to a char in memory. Then, to access a field of the struct from the char in
	  memory, we simply cast a reference to it to one of type pte*, and access the field we want from the result:

	  pte* entry = (pte*) &memory[0]  // or &disk[0], where 0 is part of a page known to contain the page table
	  entry->pid; // process this page table entry refers to
	  entry->io;  // whether page refered to by this page table entry is in disk or memory
	  ...

      Tests:
      
	Only interesting tests for part 1 are those for illegal input:
		0,map,51,2
		Value must be 0 or 1 for mapping.

		0,map,10000,1
		PID and virtual address must be greater than 0 and less than 4 and 64, respectively

		0,map,0,0
		Placed page table for PID 0 into frame 0
		Mapped virtual address 0 (page 0) into physical frame 1
		0,store,11,123
		Writes are not allowed to this page

		0,load,19,NA
		Virtual address 19 has not been mapped to process 0

		0,store,10,9999999
		Value out of range

		What's up, doc?
		Please enter exactly 4 arguments for instruction. You entered 3

		0,fightme,0,1
		"fightme" is not a valid instruction

Part 2:
     In order to perform swapping, we open a swap file in main and a single swap function, swap from memory/disk/ to the file to the disk/memory. The page table entry struct described above is
     used often to determine relevant info (whether page being swapped is memory, for example) while within swap function, which requires one as a parameter.

     Our eviction strategy is a simple round-robin strategy that begins at 1 and iterates up to 3 and back to one. The page containing our page table is never swapped out within this stategy.

     Swap() is called when needed by each of the three part 1 functions. 

	
     
