Our group members were:
	Adam Camilli (aocamilli@wpi.edu)
	William Kelley (wakelley@wpi.edu)
	Skyler Kim (snkim@wpi.edu)

Our team name was 
	GoSanMoku

---------------------------------------------------------------------------------------------------------------------------------------------------
To run our program:
	1) Ensure that GoSanMoku.py and state.py are in the same directory as referee.py so that it can read move_file and *.go files.
	2) Open three terminals: One for running referee.py, one for GoSanMoku.py, and presumably one for our opponent.
	3) Run GoSanMoku.py using the command python3.x if python2.x will result in errors due to large syntactic changes between
		python 3 and python 2 (notably xrange() -> range())
---------------------------------------------------------------------------------------------------------------------------------------------------
Utility/Evaluation function:
Our evaluation function is two steps. The first step gives a minor priority to the center spaces due to how the center has the most opportunities
to make the string of 5 pieces in a row as compared to the edges or corners. The code for evaluating these spaces are located at line 211-217 in state,py.
The second evaluates existing and potential strings and gives weights that increase with the lengths of the strings. This makes our program try to get
longer strings and prevent its opponent from getting longer strings. The code for this is located in the checkString method, which is at line 126-150
in state.py.
Specifically:
	The closest to center (the inner box, or 6 to 8 for rows and cols) is given 3 points, two out from the inner box (4-5,9-10) is given 2 points, and 
	everything else (1-3,11-15) given 1 point. 
	
	The strings (symbols in a row) are weighted by powers of 10^length, with further modifications if they are open or closed. For example:
	-OOO- = 10^3       = 1000
	XOOO- = 0.5 * 10^3 = 500
	XOOOX = 0 * 10 ^ 4 = 0
	The code for the modifier is located in the checkEnds method, which is at line 161 to 203 in state.py.

Heuristic function:
Our program determines the next move it should take be utilizing minimax with alpha-beta pruning on potential board states using our evaluation 
function to determine the values of leaf nodes. We refine the search space by having our program only consider the spaces around currently occupied
spaces as valid moves. This is done with the get_empties_around() method, which is located at lines 24 to 52 in state.py. We refine this even
further by reducing the amount of recursions based upon the number of pieces on the board to prevent our program from taking too much time. The
depth limiter is located at lines 86 to 91 in state.py

NOTE: The evaluation function determines the value of a given board configuration, and our heuristic methodology determines what the best potential 
board configuration from the current node is.
---------------------------------------------------------------------------------------------------------------------------------------------------
Results:

	GoSanMoku vs. Random: Always wins, but runs slower due to random opponent moves increasing the branching factor of the recursive minimax function.
	GoSanMoku vs. Older Versions: Always wins, runs much faster due to opponents behaving as expected and limiting the branching factor.
	GoSanMoku vs. GoSanMoku: The games are very close, as the programs block each other aggressively at the start, and as the number of played spots increases 
\		      		 so does the branching factor. The program then tends to play more defensively as it begins to only consider the immediate 
				 consequences of its next move, as the sheer number of three and four in a rows means both programs have to continue block each other.
				 They usually take winning moves when available, but preemptively block each other from achieving win conditions.
							 
---------------------------------------------------------------------------------------------------------------------------------------------------
Justification:

	Utility/Evaluation:	The logic is for our decision on the center weights was that the outer space is limited in the win conditions it can produce. 
						The inner box has the least limitations, and therefore should be valued highest. As for the specific weights of string lengths,
						were determined at the beginning: Longer string lengths should exponentially increase weight so that a single longer string is worth
						more than several shorter ones. 
	Heuristic: 	If the entire board is considered, branching factor is too large to feasibly use minimax at any sort of depth in a time frame of 10 seconds.
				Furthermore, in a game of Gomoku, the most intelligent moves are almost always connected to existing ones, since the goal of the game is to form
				strings or block your opponent from doing so. As the branching increases due to more pieces being placed, the depth is correspondingly reduced to maintain
				sufficient time complexity. As a final measure, as soon as the branching factor becomes prohibitively large, the depth approaches 1, which is near 
				instantaneous to compute.
	

---------------------------------------------------------------------------------------------------------------------------------------------------

Our assumptions for how Gomoku is played for the purposes of this project are the following:

1) The game sequence is as follows (from assignment description):

    1. A random selection method is used to determine which player will use the white stones 
        and which player will use the black stones. In what follows, the player who gets to 
        use the white stones is called P1 and the player who gets to use the black stones 
        is called P2.
        
    2. First player is randomly decided.
    
    3. After first player has made its first move, second is given the chance to change the color
        of the stone on the board to black. This is done to limit the advantages of playing first.

    4. After that, players 1 and 2 take turns making moves until the game ends with one player winning 
        or a tie. There is a 10 second time limit for a player to make its move.
        
    5. Game board is 15x15
        
2) The program must utilize the Minimax algorithm with alpha-beta pruning, with our choice of heuristic.

3) The program must only produce valid moves, and must be able to read and process opponents' moves.

4) A game between our program and an opponent will be conducted by a third party program called Referee.

    1. Referee maintains four text files:
        1) move_file
        2) <Our group name>.go
        3) <Opponent group name>.go
        4) end_game 
        
    2. Referee creates end_game if and only if one of the following conditions are met:
        1) A player gets five-in-a-row (Wins)
        2) A player makes an invalid moves (Loses)
        3) A player takes longer than 10 sec to make a move (Loses)
        4) A player makes a move out-of-order (Loses)
        5) Board is full (Tie)
    
    2. Game is played in practice via the following process:
        1) Both programs check if end_game or their .go file exists
        2) Randomly selected player (P1) writes its first move to move file after checking for end game (P1CHECK -> P1GOES)
        3) Ref reads both .go file, checks if an end condition is met, if yes creates end_game, otherwise makes other .go file (REFCHECK)
        3) P2 checks if end_game exists (P2CHECK)
        4) Non-selected player (P2) decides whether or not to change stone color
        5) P2 reads move_file, decides on a move, then writes it to its move file (P2GOES)
        5) REFCHECK
        6) P1CHECK
        7) P1GOES
        8) REFCHECK
        9) P2CHECK
        10) P2GOES
        11) REFCHECK
        12) P1CHECK
        13) P1GOES
        ...
        ...
        N)   REFCHECK creates end_game
        N+1) P1 and P2 read end_game to learn result
    
    3. Referee code is provided at <https://github.com/samogden/WPI.CS4341>
 
	4. Moves are written to move_file in the following format: 
		"<Group Name> <Column> <Row>"
		where columns are <A-O> and rows are <1-15>, and move is on one line.
		Example: "Group1 E 5"

---------------------------------------------------------------------------------------------------------------------------------------------------

Our Gomoku AI is comprised of two programs: 
	GoSanMoku.py 
	state.py
GoSanMoku.py is our player program and state.py contains the State class which we use to keep track of potential board configurations for minimax. They were split
only to avoid having one giant program for the sake of readability.
	   

