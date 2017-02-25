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
	Map:
		0,map,0,1
		Mapped virtual address 0 (page 0) to physical frame 1

		0,map,15,1
		Virtual address 15 has already been mapped to physical frame 1 for process 0

		1,map,62,0
		Mapped virtual address 48 (page 3) to physical frame 3

		1,store,63,12
		Virtual page 3 (physical frame 3) for process 1 is not writeable

		2,map,17,1
		Not enough pages left!

Part 2:

     
