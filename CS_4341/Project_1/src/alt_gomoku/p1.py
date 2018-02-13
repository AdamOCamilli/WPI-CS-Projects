'''
Created on Feb 5, 2018

@author: aocamilli
'''

import os
import random
import traceback
import time
import sys
import copy
from board import Board


DEPTH = 2
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
    @param name: The name of this player (i.e. what it is added before .go
    @param symbols: The two symbols this player uses to represent their moves (symbols[0]) vs their opponents (symbols[1])
    @param strategy: The function this player uses to decide its moves relative to its internally maintained board
    '''
    def __init__(self, name, strategy):
        self.name = name
        self.symbols = ["X","O"] # List of two playing symbols on board
        self.strategy = strategy
        # Initialize board with list comprehension
        self.board = Board(BOARD_WIDTH,BOARD_HEIGHT,"X","O",FREE_SPOT_SYMBOL)
        self.end_game = "end_game"
        self.move_file = "move_file"
        self.play_file = self.name + ".go"
        # If the player has the second move, they are to directly overwrite the first player's move, which otherwise
        # results in a loss. Therefore we must keep track of the first move.
        self.has_first_move = True # Set to false the first time move_file is non-empty, reset to true upon detecting end_game
    
    ''' Returns the best move on the current board based on the player's chosen strategy
    @return: (row,col) of strategical move 
    @return: -1 if strategy function throws exception
    '''
    def evaluate(self):
        try:
            if (self.has_first_move):
                return self.strategy(self)
            else:
                move = self.strategy(self)
                if move in self.board.get_empties():
                    return move
                else:
                    raise Exception
        except:
            print("ERROR: Stategy function for player " + self.name + " not correctly implemented.")
            traceback.print_exc()
            return -1

    ''' Prints player's current visualization of board along with numbered columns and lettered rows.
    Board is labeled with A-O by column and 1-15 by row.
    '''
    def display_board(self):
        if (VERBOSE):
            print(self.name + "'s board")
        print("   ", end = "")
        for i in range(0,self.board.y):
            print(ALPHABET[i], end = " ")
        print()
        
        for i in range(0,self.board.y):
            if (i >= 9): # Note index 9 corresponds to 10 when marking rows
                print(i+1, end = " ")
            else:
                print(" " + str(i+1), end = " ")
            for j in range(0, self.board.x):
                print(self.board.board[i][j], end = " ")
            print()
            
    def is_our_turn(self):
        return os.path.isfile(self.play_file)
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
                self.board.place(our_move[0], our_move[1], self.symbols[0])
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
                # Clearly we don't have the first move if move_file is non-empty
                if (self.has_first_move):
                    self.has_first_move = False
                    if (VERBOSE): 
                        print("Darn, I'm second to move. Overwrite the first move!")
                    our_move = (row - 1, col)
                    self.symbols[0] = "O"
                    self.symbols[1] = "X"
                else:
                    self.board.place(row-1, col, self.symbols[1])
                    our_move = self.evaluate()
                    # Update our board to reflect opponent's move if it exists
                    # - 1 to account for rows being counted from 0 in actual self.board array
                    if (VERBOSE):
                        print(self.name + ": Noticed " + tokens[0] + " made move " + ALPHABET[col].upper() + " " + str(row))
                
                move_fid.close()
                # Make our own move            
                move_fid = open(self.move_file, "w")
                self.board.place(our_move[0], our_move[1], self.symbols[0])
                # Write our move to move file: <teamname> <col> <row>
                write_string = self.name + " " + ALPHABET[our_move[1]].upper() + " " + str(our_move[0] + 1)
                move_fid.write(write_string)
                if (VERBOSE):
                    print(self.name + ": " + write_string)
                move_fid.close()
                return 0;
        except OSError:
            print("ERROR: Unable to open or read move_file, no action taken")
            traceback.print_exc()
            return -1
            
'''
-----------------------------------------------------
----------------------Stategies----------------------
-----------------------------------------------------
'''
                
''' Placeholder for real strategies, simply goes with first random non-filled spot on board
'''
def random_strategy(player_obj):
    board = player_obj.board
    available = board.get_empties()
    index = random.randint(0,len(available)-1)
    return available[index]

''' Minimax with alpha-beta pruning
@param board: Board object we are playing with
@param player: Which player is minimizing/maximizing
@param d: Depth
@param alpha: alpha (Our best)
@param beta: beta (Opponent's best)
@return: [row,col,score]
'''
def minimax(board,player,d,alpha,beta):
    movelist = []
    # Our possible moves
    our_moves = []
    # Get all of our current places
    places = board.get_player_spots(player)
    if (VERBOSE):
        print("Places: " + str(places))
    if (len(places) == 0):
        return [7,7,0]
    # And populate our_moves with all empty spots around each of them
    # Many of these entries might be empty lists []
    for coords in places:
        our_moves.extend(board.get_empties_around(coords[0][0],coords[0][1]))
    # So remove them
    our_moves = [value for value in our_moves if value != []]
    # If we have no available adjacent moves, we pick from a list of empties
    if (len(our_moves) == 0):
        movelist = board.get_empties()
    else: # Else just copy our_moves
        movelist = list(our_moves)
    best_move = []
    
    # Evaluate if at leaf
    if (d == 0):
        return [movelist[0][0], movelist[0][1], board.near_wins(player)]
    while (len(movelist) > 0):
        new_board = board.copy()
        new_move = movelist[0]
        if (player == board.p1):
            new_board.place(new_move[0],new_move[1],board.p2)
            temp = minimax(new_board, board.p2, d-1, -beta, -alpha)
        else:
            new_board.place(new_move[0],new_move[1],board.p1)
            temp = minimax(new_board, board.p1, d-1, -beta, -alpha)
        
        temp_score = -int(temp[2])
        
        if (temp_score > alpha):
            alpha = temp_score
            best_move.extend(new_move)
        if (alpha > beta):
            return [best_move[0],best_move[1],alpha]
        del movelist[0]
    return [best_move[0],best_move[1],alpha]
        
def minimax_strategy(player_obj):
    return minimax(player_obj.board, player_obj.symbols[0], DEPTH, -(sys.maxsize) - 1, sys.maxsize)
    
'''
-----------------------------------------------------
------------------------Main-------------------------
-----------------------------------------------------
'''
p1 = Player("p1", minimax_strategy)
while (not os.path.isfile("end_game")):
    print(p1.name + ": Waiting for turn...")
    while(not os.path.isfile("p1.go")):
        time.sleep(0.5)
    #print(os.listdir("."))
    print("p1: taking turn")
    p1.play()
    print(p1.symbols[0] + " 3-Row NWs: " + str(p1.board.row_near_wins(3,p1.symbols[0])))
    print(p1.symbols[0] + " 3-Col NWs: " + str(p1.board.col_near_wins(3,p1.symbols[0])))
    print(p1.symbols[0] + " 3-Diag NWs: " + str(p1.board.diag_near_wins(3,p1.symbols[0])))    
    time.sleep(0.5)
    # Need to allow referee to clear .go file
    #os.remove(p1.play_file)

    


'''
-----------------------------------------------------
------------------------Tests------------------------
-----------------------------------------------------
@todo maybe create formalized test module/functions


p1 = Player("p1", minimax_strategy)
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
'''

