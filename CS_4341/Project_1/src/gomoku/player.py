'''
Created on Jan 30, 2018

@author: aocamilli
'''

import os
import random
import traceback
import time
from WPI.CS4341 import referee

MAX_TURN_TIME = 10
BOARD_HEIGHT = 15
BOARD_WIDTH = 15
FREE_SPOT_SYMBOL = '-'
VERBOSE = False
ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"



''' 
Represents a Gomoku player
'''
class Player(object):
    # Constructor
    def __init__(self, name, symbols, strategy, first_turn = True):
        self.name = name
        self.symbols = symbols # List of two playing symbols on board, 'x'/'o', '1'/'2', etc.
        self.strategy = strategy
        # Initialize board with list compression
        self.board = [[FREE_SPOT_SYMBOL for j in range(BOARD_WIDTH)] for i in range(BOARD_HEIGHT)]
        self.end_game = "end_game"
        self.move_file = "move_file"
        self.play_file = self.name + ".go"
        
    # Evaluates board based on the player's chosen strategy
    def evaluate(self):
        try:
            return self.strategy(self.board)
        except:
            print("Stategy function for player " + self.name + " not correctly implemented.")
            traceback.print_exc()
            return -1
            
    # Update the board with a given move    
    def update_board(self, row, col, is_our_move):
        if (is_our_move):
            self.board[row][col] = self.symbols[0]
        else:
            self.board[row][col] = self.symbols[1]
            
    # Reset board
    def reset_board(self):
        self.board = [[FREE_SPOT_SYMBOL for j in range(BOARD_WIDTH)] for i in range(BOARD_HEIGHT)]

    # Display player's current visualization of board
    def display_board(self):
        print("  ", end = " ")
        for i in range(1,BOARD_HEIGHT+1):
            if (i >= 10):
                print(i, end = " ")
            else:
                print(i, end = "  ")
        print()
        
        for i in range(0,BOARD_HEIGHT):
            print(ALPHABET[i], end = "  ")
            for j in range(0, BOARD_WIDTH):
                print(self.board[i][j], end = "  ")
            print()
    
    '''
    This function handles the following actions associated with taking a turn in Gomoku:
        1) Checks if end_game exists, if so prints its contents and exits
        2) Inspects move_file, updates board accordingly
        3) Determines what move it wants to do and writes it to <self.name>.go
        
        Returns 1 if end_game is detected
        Returns 0 if write to <self.name>.go is successful
        Returns -1 if write unsuccessful (i.e. if an OSError is thrown)
    '''
    def play(self):
        time.sleep(1)
        # Check for end_game
        if (os.path.isfile(self.end_game)):
            try:
                end_fid = open(self.end_game, "r")
                for line in end_fid:
                    print(line)
                end_fid.close()
                return 1
            except OSError:
                print("end_game detected but couldn't open it")
                traceback.print_exc()
                return -1
        
        # Check if it is our turn, hang while not
        if (VERBOSE):
            print("Waiting for turn", end = '', flush = True)
        while (not (os.path.isfile(self.play_file))):
            time.sleep(0.1)
            if (VERBOSE):
                print(".", end = '', flush = True)
        if (VERBOSE):
            print()
            print()
        
        try:
            play_fid = open(self.play_file, "w")
            # Inspect move_file
            try:
                move_fid = open(self.move_file, "r")
                if (os.path.getsize(self.move_file) == 0):
                    our_move = self.evaluate()
                    self.update_board(our_move[0], our_move[1], True)
                    # Erase old .go contents
                    play_fid.truncate(0)
                    # Write our move to move_file
                    # + 1 to since array is 0-14 while board is labled 1-15
                    write_string = self.name + ".go " + ALPHABET[our_move[0]] + " " + str(our_move[1] + 1)
                    play_fid.write(write_string)
                    if (VERBOSE):
                        print("Made move: " + write_string)
                    return 0;
                else:
                    tokens = move_fid.read().split()        # Tokenize line by whitespace to get (<opponent>, <col>, <row>)
                    colnum = ALPHABET.find(tokens[1])       
                    # Update our board to reflect opponent's move if it exists
                    # - 1 to account for rows being counted from 0 in actual self.board array
                    self.update_board(colnum, int(tokens[2]) - 1, False)
                    if (VERBOSE):
                        print("Noticed " + tokens[0] + " made move " + str(tokens[1]) + " " + str(tokens[2]))
                    # Make our own move
                    our_move = self.evaluate()
                    self.update_board(our_move[0], our_move[1], True)
                    # Erase old move_file contents
                    play_fid.truncate(0)
                    # Write our move to move_file
                    write_string = self.name + ".go " + ALPHABET[our_move[0]] + " " + str(our_move[1])
                    play_fid.write(write_string)
                    if (VERBOSE):
                        print("Made move: " + write_string)
                    return 0;
            except OSError:
                print("Unable to open or read move_file, no action taken")
                return -1
        except OSError:
            print("Unable to open or write to .go file, no action taken")
            return -1
    
    def start_game(self):
        result = 0;
        while (result == 0):
            result = self.play()
            
                
# Placeholder for real strategies, simply goes with first random non-filled spot on board
def random_strategy(board):
    row = random.randint(0, BOARD_HEIGHT - 1)
    col = random.randint(0, BOARD_WIDTH - 1)
    while (board[row][col] != FREE_SPOT_SYMBOL):
        row = random.randint(0, BOARD_HEIGHT - 1)
        col = random.randint(0, BOARD_WIDTH - 1)
    return [row,col]
   
''' 
-------------------------------------------------
----------------------Tests----------------------
-------------------------------------------------
'''
# Instantiation               
p1 = Player("p1", ["x","o"], random_strategy)
p2 = Player("p1", ["x","o"], random_strategy)
p1.start_game()
p2.start_game()


'''
# display_board(): Check it is empty and 15x15
p1.display_board()

# check board updates correctly

print()
print()
p1.update_board(1, 1, True)
p1.update_board(1, 2, False)
p1.display_board()

# check strategy is followed and doesn't select invalid spots
play_file = open(p1.play_file,"w")
play_file.close()
for i in range(0,225):
    p1.play()
    p1.display_board()
    print()
    print()
os.remove(p1.play_file)

# Check we can read move_file and update board accordingly
move_file = open(p1.move_file,"w")
move_file.write("p2 F 11")
move_file.close()
play_file = open(p1.play_file,"w")
play_file.close()
for i in range(0,1):
    p1.play()
    p1.display_board()
    print()
    print()
os.remove(p1.play_file)
'''
        
        












            