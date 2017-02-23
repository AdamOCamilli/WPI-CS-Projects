Adam Camilli
Sydney Fisher

Part 1:
     Page table implementation:
     	  Our page tables (one for each process) are each located within their designated page in memory. The indices of memory representing the starts of their pages are stored as
	  integers in a global array named registers, as the stored indices are meant to simulate hardware registers. In memory itself they follow a pattern:

	  Index of Memory:  [0/16/32/48] [+1] [+2] [+3] ... (Same pattern four times) ... [15/31/47/63] (End of page)
 	  Content:              PID      VPN  PFN  R/W  ...          ...                       R/W

	  That is, the start of the page (which will be a multiple of 16 since memory is 64 chars divided into 4 pages) holds the ID of the process whose maps it contains info about;
	  The first index afterwards (Could be 1,17,etc.) holds the virtual page number of the page mapped, the second index holds physical frame number of the page in memory that was
	  actually mapped to the process, and the third index holds the read/write value of the map. This pattern continues throughout the page in memory held 
