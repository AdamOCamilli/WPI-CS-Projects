
#-------------------------------------------------------------------------
# Note: please don't use any additional package except the following packages
import numpy as np
#-------------------------------------------------------------------------
'''
    Problem 1: TicTacToe and MiniMax 
    In this problem, you will implement a version of the TicTacToe game and a minimax player.
    You could test the correctness of your code by typing `nosetests -v test1.py` in the terminal.
'''

#-------------------------------------------------------
class PlayerRandom:
    '''a random player, which chooses valid moves randomly. '''
    # ----------------------------------------------
    def play(self,s,x=1):
        '''
           The policy function, which chooses one move in the game.  
           Here we choose a random valid move.
           Input:
                s: the current state of the game, an integer matrix of shape 3 by 3. 
                    s[i,j] = 0 denotes that the i-th row and j-th column is empty
                    s[i,j] = 1 denotes that the i-th row and j-th column is taken by "X". 
                    s[i,j] = -1 denotes that the i-th row and j-th column is taken by the "O".
               x: the role of the player, 1 if you are the "X" player in the game
                    -1 if you are the "O" player in the game. 
           Outputs:
                r: the row number, an integer scalar with value 0, 1, or 2. 
                c: the column number, an integer scalar with value 0, 1, or 2. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE
        rows,cols = s.shape[0], s.shape[1]
        valid_moves = [[i,j] for i in range(rows) for j in range(cols) if s[i][j] == 0]
        np.random.shuffle(valid_moves)

        r,c = valid_moves[0]
        #########################################
        return r,c


#-------------------------------------------------------
class TicTacToe:
    '''TicTacToe is a game engine. '''
    # ----------------------------------------------
    def __init__(self):
        ''' Initialize the game. 
            Input:
                self.s: the current state of the game, a numpy integer matrix of shape 3 by 3. 
                        self.s[i,j] = 0 denotes that the i-th row and j-th column is empty
                        self.s[i,j] = 1 denotes that the i-th row and j-th column is "X"
                        self.s[i,j] = -1 denotes that the i-th row and j-th column is "O"
        '''
        self.s = np.zeros((3,3))


    # ----------------------------------------------
    def play_x(self, r, c):
        '''
           X player takes one step with the location (row and column number)
            Input:
                r: the row number, an integer scalar with value 0, 1, or 2. 
                c: the column number, an integer scalar with value 0, 1, or 2. 
        '''
        assert  self.s[r,c]==0
        #########################################
        ## INSERT YOUR CODE HERE

        self.s[r,c] = 1
        
        #########################################

    # ----------------------------------------------
    def play_o(self, r, c):
        '''
           O player take one step with the location (row and column number)
            Input:
                r: the row number, an integer scalar with value 0, 1, or 2. 
                c: the column number, an integer scalar with value 0, 1, or 2. 
        '''
        assert  self.s[r,c]==0
        #########################################
        ## INSERT YOUR CODE HERE

        self.s[r,c] = -1
        
        #########################################


    # ----------------------------------------------
    @staticmethod
    def avail_moves(s):
        '''
           Get a list of avaiable (valid) next moves from the given state s of the game 
            Input:
                s: the current state of the game, an integer matrix of shape 3 by 3. 
                    s[i,j] = 0 denotes that the i-th row and j-th column is empty
                    s[i,j] = 1 denotes that the i-th row and j-th column is taken by "X" player. 
                    s[i,j] = -1 denotes that the i-th row and j-th column is taken by "O" player.
            Outputs:
                m: a list of possible next moves, where each next move is a (r,c) tuple, 
                   r denotes the row number, c denotes the column number. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        rows,cols = s.shape[0], s.shape[1]
        m = [[i,j] for i in range(rows) for j in range(cols) if s[i][j] == 0]
        
        #########################################
        return m

    # ----------------------------------------------
    @staticmethod
    def check(s):
        '''
            check if the game has ended.  
            Input:
                s: the current state of the game, an integer matrix of shape 3 by 3. 
                    s[i,j] = 0 denotes that the i-th row and j-th column is empty
                    s[i,j] = 1 denotes that the i-th row and j-th column is taken by "X" player. 
                    s[i,j] = -1 denotes that the i-th row and j-th column is taken by "O" player.
            Outputs:
                e: the result, an integer scalar with value 0, 1 or -1.
                    if e = None, the game doesn't end yet.
                    if e = 0, the game is a draw.
                    if e = 1, X player won the game.
                    if e = -1, O player won the game.
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # check the 7 lines in the board to see if the game has ended.
        # rows
        r,c,d = None, None, None
        for x in range(0,3):
            row = set([s[x][0],s[x][1],s[x][2]])
            if len(row) == 1:
                if s[x][0] != 0.:
                    return s[x][0]
                else:
                    r = 2
            elif len(row) != 3 and 0 in row:
                r = 2
                
        # columns
        for x in range(0,3):
            column = set([s[0][x],s[1][x],s[2][x]])
            if len(column) == 1:
                if s[0][x] != 0.:
                    return s[0][x]
                else:
                    c = 2
            elif len(column) != 3 and 0 in column:
                c = 2
                    
        # diagonals
        diag1 = set([s[0][0],s[1][1],s[2][2]])
        diag2 = set([s[0][2],s[1][1],s[2][0]])
        if len(diag1) == 1 or len(diag2) == 1:
            if s[1][1] != 0.:
                return s[1][1]
            else:
                d = 2
        elif (len(diag1) != 3 or len(diag2) != 3) and (0 in diag1 or 0 in diag2):
            d = 2

        #########################################
        if ([r,c,d].count(2) > 0):
            return None
        return 0


    # ----------------------------------------------
    def game(self,x,o):
        '''
            run a tie-tac-toe game starting from the current state of the game, letting X and O players to play in turns.
            Here we assumes X player moves first in a game, then O player moves.
            let x player and o player take turns to play and after each move check if the game ends 
            Input:
                x: the "X" player (the first mover), such as PlayerRandom, you could call x.play() to let this player to choose ome move.
                o: the "O" player (the second mover)
            Outputs:
                e: the result of the game, an integer scalar with value 0, 1 or -1.
                    if e = 0, the game ends with a draw/tie.
                    if e = 1, X player won the game.
                    if e = -1, O player won the game.
        '''
        #########################################
        ## INSERT YOUR CODE HERE    
        e = TicTacToe.check(self.s)
        while (e == None and len(np.where(self.s==0)) > 0):
            r,c = x.play(self.s,1)
            self.play_x(r,c)
            e = TicTacToe.check(self.s)
            if (e == None and len(np.where(self.s==0)) > 0):
                r,c = o.play(self.s,-1)
                self.play_o(r,c)
                e = TicTacToe.check(self.s)
            #print("zero",np.where(self.s==0))

        #########################################
        return e

            
#-----------------------------------------------
#   MiniMax Player
#-----------------------------------------------
class Node:
    '''
        Search Tree Node
        Inputs: 
            s: the current state of the game, an integer matrix of shape 3 by 3. 
                s[i,j] = 0 denotes that the i-th row and j-th column is empty
                s[i,j] = 1 denotes that the i-th row and j-th column is taken by "X". 
                s[i,j] = -1 denotes that the i-th row and j-th column is taken by the "O".
            x: the role of the player, 1 if you are the "X" player in the game
                    -1 if you are the "O" player in the game. 
    '''
    def __init__(self,s,x=1,c=None,m=None,v=None):
        self.s = s
        self.c= [] # a list of children nodes
        self.x=x # the role of the player in the game (X player:1, or O player:-1)
        self.m=m # the move that it takes from the parent node to reach this node. 
                 # m is a tuple (r,c), r:row of move, c:column of th move 
        self.v=v # the value of the node (X player will win:1, tie: 0, lose: -1)

    # ----------------------------------------------
    def expand(self):
        '''
         Expand the current tree node for one-level.
         Add one child node for each possible next move in the game.
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # if the game in the current state has already ended,  return/exit
        if (TicTacToe.check(self.s) != None):
            return

        # if the game has not ended yet, expand the current node with one child node for each valid move 
        valid_moves = TicTacToe.avail_moves(self.s)
        for move in valid_moves:
            new_s = np.copy(self.s)
            i = move[0]
            j = move[1]
            new_s[i][j] = self.x
            Child = Node(s=new_s, x=self.x * -1, m=(i,j))
            self.c.append(Child)            

        #########################################
 
    # ----------------------------------------------
    def build_tree(self):
        '''
        Given a node of the current state of the game, build a fully-grown search tree without computing the score of each node. 
        Hint: you could use recursion to build the search tree        
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        if (TicTacToe.check(self.s) != None):
            return

        # expand the current node
        self.expand()
        # recursively build a subtree from each child node
        for child in self.c:
            child.build_tree()        

        #########################################

    # ----------------------------------------------
    def compute_v(self):
        '''
           compute score of the current node of a search tree using minimax algorithm
           Here we assume that the whole search-tree is fully grown, but no score on any node has been computed yet before calling this function.
           After computing value of the current node, assign the value to n.v and assign the optimal next move to node.p
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # if the game has already ended, the score of the node is the result of the game (X won: 1, tie: 0, or O won:-1)
        check = TicTacToe.check(self.s)
        if (check != None):
            self.v = check
            return

        # otherwise: compute scores/values of all children nodes
        # Hint: you could use recursion to solve this problem. 
        for child in self.c:
            child.compute_v()
        

        # set the value of the current node with the value of the best move
        # Hint: depending on whether the current node is "X" or "O" player, you need to compute either max (if X player) or min (O player) of the values among the children nodes

        child_scores = [c.v for c in self.c] 
        if (self.x == 1):
            self.v = max(child_scores)
        else:
            self.v = min(child_scores)
            
        #########################################



#-------------------------------------------------------
class PlayerMiniMax:
    '''
        Minimax player, who choose optimal moves by searching the subtree with min-max.  
    '''

    # ----------------------------------------------
    def play(self,s,x=1):
        '''
          The policy function of the minimax player, which chooses one move in the game.  
          We need to first build a tree rooted with the current state, and compute the values of all the nodes in the tree. 
           Inputs:
                s: the current state of the game, an integer matrix of shape 3 by 3. 
                    s[i,j] = 0 denotes that the i-th row and j-th column is empty
                    s[i,j] = 1 denotes that the i-th row and j-th column is taken by "X". 
                    s[i,j] = -1 denotes that the i-th row and j-th column is taken by the "O".
                x: the role of the player, 1 if you are the "X" player in the game
                    -1 if you are the "O" player in the game. 
           Outputs:
                r: the row number, an integer scalar with value 0, 1, or 2. 
                c: the column number, an integer scalar with value 0, 1, or 2. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # build a search tree with the current state as the root node
        current_state = Node(s,x)
        current_state.build_tree()
        
        # compute value
        current_state.compute_v()

        # find the best next move
        move = []
        if (x == 1):
            vminmax = -4
            for c in current_state.c:
                if c.v > vminmax:
                    vminmax = c.v
                    move = c.m
        else:
            vminmax = 4
            for c in current_state.c:
                if c.v < vminmax:
                    vminmax = c.v
                    move = c.m

        r = move[0]
        c = move[1]



        #########################################
        return r,c


