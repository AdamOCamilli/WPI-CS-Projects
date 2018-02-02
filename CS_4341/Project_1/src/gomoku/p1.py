'''
Created on Jan 31, 2018

@author: aocamilli
'''

import os
import random
import traceback
import time
import sys
from telnetlib import WONT

MAX_TURN_TIME = 10
BOARD_HEIGHT = 15
BOARD_WIDTH = 15
FREE_SPOT_SYMBOL = '-'                      # How free spots on a board are marked
VERBOSE = True                              # Have players print out what they are doing as they play
DEBUG = True
ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"     # Because Eclipse refuses to understand ASCII


''' 
Represents a Gomoku player

@todo: ...(more class doc)...
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
        self.symbols = ["x","o"] # List of two playing symbols on board
        self.strategy = strategy
        # Initialize board with list comprehension
        self.board = [[FREE_SPOT_SYMBOL for j in range(BOARD_WIDTH)] for i in range(BOARD_HEIGHT)]
        self.end_game = "end_game"
        self.move_file = "move_file"
        self.play_file = self.name
    
    def valid_move(self,row,col):
        if (DEBUG and self.board[row][col] == FREE_SPOT_SYMBOL):
            self.display_board()
            print("I say that " + str(row) + "," + str(col))
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
        if (self.valid_move(row,col)):
            if (is_our_move):
                self.board[row][col] = self.symbols[0]
            else:
                self.board[row][col] = self.symbols[1]
            return True
        else:
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
        print("   ", end = " ")
        for i in range(0,BOARD_HEIGHT):
            print(ALPHABET[i], end = "  ")
        print()
        
        for i in range(0,BOARD_HEIGHT):
            if (i >= 9): # Note index 9 corresponds to 10 when marking rows
                print(i+1, end = "  ")
            else:
                print(" " + str(i+1), end = "  ")
            for j in range(0, BOARD_WIDTH):
                print(self.board[i][j], end = "  ")
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
                move_fid = open(self.move_file, "w")
                our_move = self.evaluate()
                self.update_board(our_move[0], our_move[1], True)
                # Write our move to .go file
                # + 1 since array is 0-14 while board is labled 1-15
                write_string = self.name + " " + ALPHABET[our_move[0]].upper() + " " + str(our_move[1] + 1)
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
                if tokens[1].upper() in ALPHABET:
                    row = int(tokens[2])
                    col = ALPHABET.find(tokens[1].upper())
                else:
                    row = int(tokens[1])
                    col = ALPHABET.find(tokens[2].upper())      
                # Update our board to reflect opponent's move if it exists
                # - 1 to account for rows being counted from 0 in actual self.board array
                self.update_board(row - 1, col, False)
                if (VERBOSE):
                    print(self.name + ": Noticed " + tokens[0] + " made move " + tokens[1].upper() + " " + tokens[2])
                move_fid.close()
                move_fid = open(self.move_file, "w")
                # Make our own move
                our_move = self.evaluate()
                self.update_board(our_move[0], our_move[1], True)
                # Write our move to move file
                write_string = self.name + " " + ALPHABET[our_move[0]]+ " " + str(our_move[1] + 1)
                move_fid.write(write_string)
                if (VERBOSE):
                    print(self.name + ": " + write_string)
                move_fid.close()
                return 0;
        except OSError:
            print("Unable to open or read move_file, no action taken")
            traceback.print_exc()
            return -1
'''   
    def five_in_a_row(self, row, col):
        # South
        if (row <= BOARD_HEIGHT - 5):
            won = True
            for y in range(row+1,BOARD_WIDTH):
                won = (self.board[y][col] == self.board[row][col])
            return won
        # North
        if (row >= 4):
            won = True
            for y in range(row,0):
                won = (self.board[y][col] == self.board[row][col])
            return won
        # East
        if (col <= BOARD_WIDTH - 5):
            won = True
            for x in range
'''

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



'''
-----------------------------------------------------
------------------------Main-------------------------
-----------------------------------------------------
'''
p1 = Player("p1",random_strategy)
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
