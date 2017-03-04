README for project 1
Adam Camilli
Trang Ha

Checkpoint: Our current version has a (mostly) finished phase 1 and a phase 2 with ok progress:
		
		Phase 1 can successfully run all 3 commands, but has one issue: it foolishly uses scanf(), and so can bug out with wrong input (particularly empty input). We plan to replace it with
 			something more stable (fgets() or getline() + sscanf()) as continue to work on it.
		Phase 2 can currently successfully add new user commands to a dynamically grown list and subsequently to the prompt, but it cannot yet run them. The other commands such as exit and 
			change directory have not yet been implemented.

Final:	
	Phase 1: mc0.c is designed very literally, with a function to type the shell prompt and a single main that handles all valid user inputs.
		 	
			type_prompt(): Simply contains multiple printf() statements to prompt user.
			
			main(int argc, char** argv[]): 	contains a continuously running while() loop that calls type_prompt() and then fgets().
						       	The user input is then run through and if-else chain to determine proper response, including
						      	if input is incorrect. Within the if() statements, for valid commands, fork() is called and an inner if-else chain
							handles correct execvp() use for parent, child, or a failed fork(). 
		Test Cases:
				1) Run ./mc0 and test each command individually in various orders. Should execute their respective linux commands correctly.			
				2) Enter nothing. Should return error and reprompt. 
				3) Enter nothing, then enter 0. Should return error, then provide result current username and reprompt.
				4) Enter 0 and a single space. Should provide current username and reprompt.
				5) Enter a single space and 0. Should provide current username and reprompt.
				6) Enter 2 and input a nonexistent option for ls. Should say " ls: invalid option "

	
	Phase 2: mc1.c is comparatively more complex, featuring not only more commands but the ability to add custom commands. It makes use of a fixed array to
		 hold added commands as strings, and type_prompt() now accordingly takes in thisarray to print all user-made commands. The executions done in main() have also
		 been moved to a helper function execute() which also takes in a string as a parameter.

			execute(char* cmd):		-Takes in a string. It first tokenizes the C string pointed to by the
							 index of array field into a command (first word), up to 32 arguments, and a path, all separated by spaces using strtok(). 
							 The total length of the C string cannot exceed 128 bytes.
							-Now forks and execs, using using a newly formatted C string array containing the command as its first index, NULL as its last index,
							 and possible individual args and paths in between as individual indices.

		Test Cases:
				1) Run ./mc1 and enter p. Now enter c and a directory, and then enter p again. It should return what you changed the working directory to.
				2) Enter e. Should print exit message and go back to linux command line.
				3) Enter some of the old commands and then enter e. Should print exit message and go back to linux command line.
				4) Enter a and add a linux command along with any needed arguments or path. Should be added to list of phase 1 commands in the prompt, and
				   be described as a "User added command".
				5) Enter the number associated with added command. The command should execute as it would if typed on regular terminal line, and then reprompt.
				6) Enter a and add something that is not a real command (i.e. the first word is not a real Unix command). Should print statement
				   "Invalid Command!" and reprompt.
				7) Enter a and add a valid unix command with invalid options. Linux should print "[command]: invalid options"
	
	Phase 3: 	mc2.c was an attempt to add the ability to handle background commands, but it is not able to fully do this. Our current implementation
			does allow a background process to wait until the shell prompts the user, but it then immediately executes without waiting for additional input.
			As such out of order processes and most other test cases cannot be tested.

				We attempted to add a struct that would keep track of all ongoing background processes by number and pid, but as we could not get processes
				to actaully wait, we did not get to testing or using this struct.

			
			
			
			
		


