Adam Camilli
Sydney Fisher

Part 1:
     Page table implementation:
     	     To implement the page table within memory, we used a struct called entry that represents pertinent info to a page table entry, such as the PID of the process it belongs to,
	     a virtual page number and the physical frame it translate to, the read/write value, etc. A global array of these structs is used to store them, and in memory, within the page
	     belonging to a page table are the indices of the global array its entries are located in.
	     So for example, there might be some entry the array

	     entries[0] = {.used = 1,
	     	      	   .io = 0,
			   .rw = 1,
	     	      	   .vpn = 0,
			   .pfn = 1,
			   .pid = 0
			  }

	     whose fields rw, vpn, pfn, and pid represent respectively the read/write value 1, the virtual page number 0, the physical frame 1 the vpn corresponds to, and the process id 0.
	     The index 0 that locates this entry within the global array is stored in memory in physical frame 0, where process 0's page table is located.
	     If another entry were created at entries[1] representing another page that was mapped to process 0, 1 would be stored after 0 in memory. 
			 
