'''
Created on Jan 31, 2018

@author: aocamilli
'''

import os
import random
import traceback
import time
import sys
import copy
from state import State

MAX_TURN_TIME = 10
BOARD_HEIGHT = 15
BOARD_WIDTH = 15
FREE_SPOT_SYMBOL = '-'                      # How free spots on a board are marked
VERBOSE = True                              # Have players print out what they are doing as they play
ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"     # Because Eclipse refuses to understand ASCII


''' 
Represents a Gomoku player AI who plays using a chosen strategy and an internally maintained board that matches the 
referee's board. 
'''
class Player(object):
    '''
    Constructor.
    @param name: The name of this player (i.e. what it is added before .go)
    @param symbols: The two symbols this player uses to represent their moves (symbols[0]) vs their opponents (symbols[1])
    @param strategy: The function this player uses to decide its moves relative to its internally maintained board
    '''
    def __init__(self, name, strategy):
        self.name = name
        self.symbols = ["X","O"] # List of two playing symbols on board
        self.strategy = strategy
        # Initialize board with list comprehension
        self.board = [[FREE_SPOT_SYMBOL for j in range(BOARD_WIDTH)] for i in range(BOARD_HEIGHT)]
        self.end_game = "end_game"
        self.move_file = "move_file"
        self.play_file = self.name
        # If the player has the second move, they are to directly overwrite the first player's move, which otherwise
        # results in a loss. Therefore we must keep track of the first move.
        self.has_first_move = True # Just permanently set to false the first time move_file is non-empty, reset to true upon detecting end_game

    def valid_move(self,row,col):
        if (VERBOSE and self.board[row][col] == FREE_SPOT_SYMBOL):
            self.display_board()
            print("I say that " + str(row) + "," + str(col) + " is valid")
        return (self.board[row][col] == FREE_SPOT_SYMBOL)

    ''' Returns the best move on the current board based on the player's chosen strategy
    @return: (row,col) of strategical move 
    @return: -1 if strategy function throws exception
    '''
    def evaluate(self):
        try:
            move = self.strategy(self.board)
            if (self.valid_move(move[0], move[1])):
                return move
            else:
                raise Exception
        except:
            print("ERROR: Stategy function for player " + self.name + " not correctly implemented.")
            traceback.print_exc()
            return -1

    ''' Updates the board with a given move 
    @param row: Row where move was made
    @param col: Column where move was made
    @param is_our_move: Whether or not this is the player's move 
    @return: True if move successful, false if not
    '''
    def update_board(self, row, col, is_our_move):
        if (is_our_move):
            self.board[row][col] = self.symbols[0]
            return True
        else:
            self.board[row][col] = self.symbols[1]
            return False

    ''' Resets board (i.e. populates entire board array with FREE_SPOT_SYMBOL)
    '''
    def reset_board(self):
        self.board = [[FREE_SPOT_SYMBOL for j in range(BOARD_WIDTH)] for i in range(BOARD_HEIGHT)]

    ''' Prints player's current visualization of board along with numbered columns and lettered rows.
    Board is labeled with A-O by column and 1-15 by row.
    '''
    def display_board(self):
        if (VERBOSE):
            print(self.name + "'s board")
        print("   ", end = "")
        for i in range(0,BOARD_HEIGHT):
            print(ALPHABET[i], end = " ")
        print()

        for i in range(0,BOARD_HEIGHT):
            if (i >= 9): # Note index 9 corresponds to 10 when marking rows
                print(i+1, end = " ")
            else:
                print(" " + str(i+1), end = " ")
            for j in range(0, BOARD_WIDTH):
                print(self.board[i][j], end = " ")
            print()

    def is_our_turn(self):
        return os.path.isfile(self.play_file + ".go")
    def game_over(self):
        return os.path.isfile(self.end_game)

    '''
    This function handles the following actions associated with taking a turn in Gomoku:
        1) Checks if end_game exists, if so prints its contents and exits
        2) Checks if it is our turn (if <self.play_file> exists), if not exists
        2) Inspects move_file, updates board accordingly
        3) Determines what move it wants to do and writes it to <self.name>.go
        
        Returns 2 if end_game is detected
        Returns 1 if <self.name>.go is not detected
        Returns 0 if write to <self.name>.go is successful
        Returns -1 if write unsuccessful (i.e. if an OSError is thrown reading/writing one of the files)
    '''
    def play(self):
        # Check for end_game
        if (self.game_over()):
            self.has_first_move = True # Might be true next time...
            try:
                if (VERBOSE):
                    end_fid = open(self.end_game, "r")
                    for line in end_fid:
                        print(line)
                    end_fid.close()
                return 2
            except OSError:
                print("ERROR: end_game detected but couldn't open it")
                traceback.print_exc()
                return -1
        
        # Check if it is not our turn
        if (not self.is_our_turn()):
            return 1
        
        # All good, make our move
        # Inspect move_file for opponent's last move
        try:
            # If move_file is empty, call evaluate() and make a move
            if (os.path.getsize(self.move_file) == 0):                
                # We have first move, change the color of first move (i.e. overwrite it)
                if (self.has_first_move):
                    print("Hey, I've got the first move!")
                    self.has_first_move = False
                    self.symbols[0] = "X"
                    self.symbols[1] = "O"
                else:
                    self.has_first_move = False
                
                our_move = self.evaluate()
                self.update_board(our_move[0], our_move[1], True)
                # Write our move to .go file
                # + 1 since array is 0-14 while board is labled 1-15
                # Order is <col> <row> for output
                move_fid = open(self.move_file, "w")
                write_string = self.name + " " + ALPHABET[our_move[1]].upper() + " " + str(our_move[0] + 1)
                move_fid.write(write_string)
                if (VERBOSE):
                    print(self.name + ": " + write_string)
                move_fid.close()
                return 0;
            else:
                # Need to open to read since opening in write/append mode truncates file in Python
                move_fid = open(self.move_file, "r")
                # If move_file is not empty, we must interpret it, since it contains the opponent's last move
                tokens = move_fid.read().split()        # Tokenize line by whitespace to get (<opponent>, <col>, <row>)
                # Order from move_file is somewhat arbitrary (I believe it changes based on player's implementation)
                # So we have to consider both cases where it is <teamname> <col> <row> and <teamname> <row> <col>
                if tokens[1].upper() in ALPHABET:
                    row = int(tokens[2])
                    col = ALPHABET.find(tokens[1].upper())
                else:
                    row = int(tokens[1])
                    col = ALPHABET.find(tokens[2].upper())
                # Update our board to reflect opponent's move if it exists
                # - 1 to account for rows being counted from 0 in actual self.board array
                if (VERBOSE):
                    print(self.name + ": Noticed " + tokens[0] + " made move " + ALPHABET[col].upper() + " " + str(row))
                
                # Clearly we don't have the first move if move_file is non-empty
                if (self.has_first_move):
                    self.has_first_move = False
                    if (VERBOSE): 
                        print("Darn, I'm second to move. Overwrite first move!")
                    our_move = (row - 1,col)
                    self.symbols[0] = "O"
                    self.symbols[1] = "X"
                else:
                    self.update_board(row-1, col, False)
                    our_move = self.evaluate()
                    
                move_fid.close()
                move_fid = open(self.move_file, "w")
                # Make our own move
                self.update_board(our_move[0], our_move[1], True)
                # Write our move to move file: <teamname> <col> <row>
                write_string = self.name + " " + ALPHABET[our_move[1]].upper() + " " + str(our_move[0] + 1)
                move_fid.write(write_string)
                if (VERBOSE):
                    print(self.name + ": " + write_string)
                move_fid.close()
                return 0;
        except OSError:
            print("Unable to open or read move_file, no action taken")
            traceback.print_exc()
            return -1
    
    ''' Returns the number of near wins for a given vertical length (i.e. a number of rows)
        @param length: Length of pieces in a row we are searching for
        @param symbol: Which player's piece we are looking for ("X" or "O")
        @return: How many near wins are on the board for given player
    '''
    def col_near_wins(self, length, symbol):
        col_nwins = 0;
        # We need to check two directions for column near wins: 
        
        # North
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_HEIGHT):
                # Ignore rows which cannot have north wins of given length
                if (i < length): # Row too small
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length): 
                        if (self.board[i-k][j] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        col_nwins += 1
        # South
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_HEIGHT):
                # Ignore rows which cannot have south wins of given length
                if (i + length > BOARD_HEIGHT - 1): # Row too large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length): 
                        if (self.board[i+k][j] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        col_nwins += 1
                     
        return col_nwins
    
    ''' Returns the number of near wins for a given horizontal length (i.e. a number of columns)
        @param length: Length of pieces in a row we are searching for
        @param symbol: Which player's piece we are looking for ("X" or "O")
        @return: How many near wins are on the board for given player
    '''
    def row_near_wins(self, length, symbol):
        row_nwins = 0;
        # We need to check two directions for column near wins: 
        
        # West
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_WIDTH):
                # Ignore rows which cannot have north wins of given length
                if (j < length): # Row too small
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0, length): 
                        if (self.board[i][j-k] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        row_nwins += 1
        # East
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_HEIGHT):
                # Ignore rows which cannot have south wins of given length
                if (j + length > BOARD_WIDTH): # Row too large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length): 
                        if (self.board[i][j+k] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        row_nwins += 1
                                
        return row_nwins
                    
    ''' Returns the number of near wins for a given diagonal length (i.e. a number of rows and columns)
        @param length: Length of pieces in a row we are searching for
        @param symbol: Which player's piece we are looking for ("X" or "O")
        @return: How many near wins are on the board for given player
    '''
    def diag_near_wins(self, length, symbol):
        diag_nwins = 0;
        # We need to check in four directions for column near wins: 
        
        # Northeast (-i,-j)
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_WIDTH):
                # Ignore rows which cannot have north wins of given length
                if (i < length or j < length): # Too small/small
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i-k][j-k] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        diag_nwins += 1
        
        # Northwest (-i,+j)
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_WIDTH):
                # Ignore rows which cannot have north wins of given length
                if (i < length or j + length > BOARD_WIDTH): # Too small/large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i-k][j+k] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        diag_nwins += 1
        
        # Southeast (+i,-j)
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_WIDTH):
                # Ignore rows which cannot have north wins of given length
                if (i + length > BOARD_HEIGHT or j < length): # Too large/small
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i+k][j-k] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        diag_nwins += 1
        
        # Southwest (+i,+j)
        for i in range(0,BOARD_WIDTH):
            for j in range(0,BOARD_WIDTH):
                # Ignore rows which cannot have north wins of given length
                if (i + length > BOARD_HEIGHT or j + length > BOARD_WIDTH): # Too large/large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i+k][j+k] != symbol):
                            valid_near_win = False
                            break
                    if (valid_near_win):
                        diag_nwins += 1
                        
        return diag_nwins
    
    ''' Returns the number of "near wins" (pieces in a row) for a given length.
        @param length: Length of pieces in a row we are searching for
        @param player: Which player's piece we are looking for (True for us, False for opponent)
        @return: How many near wins are on the board for given player
    '''
    def near_wins(self, length, player):
        if (player):
            symbol = self.symbols[0]
        else:
            symbol = self.symbols[1]
        return self.col_near_wins(length, symbol) + self.row_near_wins(length, symbol) + self.diag_near_wins(length, symbol)
    
class Node(object):
    def __init__(self, children, value):
        self.children = children
        self.value = value

    def is_terminal(self):
        return self.children == None

    def depth(self):
        if (self.is_terminal()):
            return 1
        else:
            return 1 + self.children[0].depth()

'''
-----------------------------------------------------
----------------------Stategies----------------------
-----------------------------------------------------
'''

''' Placeholder for real strategies, simply goes with first random non-filled spot on board
'''
def random_strategy(board):
    row = random.randint(0, BOARD_HEIGHT - 1)
    col = random.randint(0, BOARD_WIDTH - 1)
    while (board[row][col] != FREE_SPOT_SYMBOL):
        if(VERBOSE):
            print("Chilling in random loop")
        row = random.randint(0, BOARD_HEIGHT - 1)
        col = random.randint(0, BOARD_WIDTH - 1)

    return [row,col]

def minimax(node, depth, heuristic, maximize):
    if (depth == 0 or node.is_terminal()):
        return heuristic(node)
    else:
        print(node.value)

    if (maximize):
        bestValue = -(sys.maxsize) - 1
        for child in node.children:
            value = minimax(child, depth - 1, heuristic, False)
            bestValue = max(bestValue,value)
        return bestValue
    else:
        bestValue = sys.maxsize
        for child in node.children:
            value = minimax(child, depth - 1, heuristic, True)
            bestValue = min(bestValue,value)
        return bestValue

def minimax2(board):
    print('running minimax')
    boardCopy = copy.deepcopy(board)
    for i in range(0, 15):
        for j in range(0, 15):
            if boardCopy[i][j] == FREE_SPOT_SYMBOL:
                boardCopy[i][j] = 0
            elif boardCopy[i][j] == 'x':
                boardCopy[i][j] = 1
            else:
                boardCopy[i][j] = -1
    state = State(boardCopy)
    state.findChildren(3,1,-99999,99999)
    bestState = state.children[0]
    for s in state.children:
        if s.value > bestState.value:
            bestState = s
    print(bestState.value)
    return [bestState.yMove, bestState.xMove]


'''
-----------------------------------------------------
------------------------Main-------------------------
-----------------------------------------------------
'''
p1 = Player("p1", random_strategy)
while (not os.path.isfile("end_game")):
    print(p1.name + ": Waiting for turn...")
    while(not os.path.isfile("p1.go")):
        time.sleep(0.5)
    #print(os.listdir("."))
    print("p1: taking turn")
    p1.play()
    time.sleep(0.5)
    # Need to allow referee to clear .go file
    #os.remove(p1.play_file)





'''
-----------------------------------------------------
------------------------Tests------------------------
-----------------------------------------------------
@todo maybe create formalized test module/functions
'''

'''
p1 = Player("p1", random_strategy)
p2 = Player("p2", random_strategy)
p1.display_board()
p2.display_board()

move_file = open(p1.move_file, "w")
move_file.close()
p1_file = open(p1.play_file,"w")
p1_file.close()
p1.play()
p1.display_board()
p2_file = open(p2.play_file,"w")
p2_file.close()
p2.play()
p2.display_board()

os.remove("move_file")
os.remove(p1.play_file)
os.remove(p2.play_file)


node1b2a = Node(None,2)
node1b2b = Node(None,5)
node1a2b = Node(None,3)
node1a2a = Node(None,1)

a_nodes = [node1a2a,node1a2b]
b_nodes = [node1b2a,node1b2b]
node1a = Node(a_nodes, 12)
node1b = Node(b_nodes, 9)
root = Node([node1a,node1b],0)

def simple_heuristic(node):
    return node.value

print(minimax(root, root.depth(), simple_heuristic, True))

p1 = Player("GoSanMoku", random_strategy)
p2 = Player("AnotherPlayer", random_strategy)
move_file = open("move_file","w")
move_file.close()
while (p1.play() != 2 and p2.play() != 2):
    if ()
'''
