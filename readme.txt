Adam Camilli
Sydney Fisher

Part 1:
     Page table implementation:
     	  Our page tables (one for each process) are each located within their designated page in memory. The indices of memory representing the starts of their pages are stored as
	  integers in a global array named registers, as the stored indices are meant to simulate hardware registers. In memory itself they follow a pattern:

	  Index of Memory:  [0/16/32/48] [+1] [+2] [+3] ... (Same pattern four times) ... [15/31/47/63] (End of page)
 	  Content:            Disk/Mem   VPN  PFN  R/W  ...          ...                       R/W

	  That is, the start of the page (which will be a multiple of 16 since memory is 64 chars divided into 4 pages) holds the ID of the process whose maps it contains info about;
	  The first index afterwards (Could be 1,17,etc.) holds the virtual page number of the page mapped, the second index holds physical frame number of the page in memory that was
	  actually mapped to the process, and the third index holds the read/write value of the map. This pattern continues throughout the page.

     Tests:
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

	0,map,32,1
	Not enough pages left!

	0,load,0,NA
	Nothing stored at virtual address 0 (physical address 1)

	0,store,0,212
	Stored value 212 at virtual address 0 (physical address 16)

	0,load,1,NA
	Nothing stored at virtual address 0 (physical address 1)

	0,load,0,NA
	The value 212 is virtual address 0 (physical address 16)
	
	  
				
