Checkpoint:

	Currently Ubuntu immediately crashes upon inserting module of new_sys_calls, and started doing so after implementation of sys_read was added. Other calls seemed to work fine based on test cases
	in testcalls.c, but I was unable to test read without having Ubuntu crash on me.
