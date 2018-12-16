#-------------------------------------------------------------------------
# Note: please don't use any additional package except the following packages
import numpy as np
from problem1 import TicTacToe,PlayerRandom
from problem2 import UCBplayer 
#-------------------------------------------------------------------------
'''
    Problem 3: Monte Carlo Tree Search (MCTS) 
    In this problem, you will implement a MCTS player for TicTacToe.
    You could test the correctness of your code by typing `nosetests -v test3.py` in the terminal.
'''
            
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
    def __init__(self,s,x=1, parent=None):
        self.s = s
        self.parent = parent # the parent node of the current node
        self.c= [] # a list of children nodes
        self.x=x # the role of the player in the game (X:1, or O:-1)
        self.N=0 # number of times being selected in the simulation
        self.w=0 # sum of values (results) in the simulation 

    # ----------------------------------------------
    def expand(self):
        '''
         Expand the current tree node for one-level.
         Add one child node for each possible next move in the game.
         Inputs:
                node: the current tree node to be expanded 
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # if the game has already ended,  return/exit without expanding the tree
        if (TicTacToe.check(self.s) != None):
            return

        # if the game has not ended yet, expand the current node with one child node for each valid move 

        valid_moves = TicTacToe.avail_moves(self.s)
        for move in valid_moves:
            new_s = np.copy(self.s)
            i = move[0]
            j = move[1]
            new_s[i][j] = self.x
            Child = Node(s=new_s, x=self.x * -1, parent=self)
            self.c.append(Child)     
        
        #########################################

    # ----------------------------------------------
    def rollout(self):
        '''
         Monte Carlo simulation: simulate a randomized game from the current node until it reaches an end of the game.
         Outputs:
             e: the result of the game (X player won:1, tie:0, lose: -1), an integer scalar. 
         Hint: you could use PlayerRandom in problem 1.
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        P1 = PlayerRandom()
        P2 = PlayerRandom()
        s = np.copy(self.s)
        e = TicTacToe.check(s)
        if (self.x == 1):
            while (e == None and len(np.where(s==0)) > 0):
                r,c = P1.play(s,1)
                s[r][c] = 1
                e = TicTacToe.check(s)
                if (e == None and len(np.where(s==0)) > 0):
                    r,c = P2.play(s,-1)
                    s[r][c] = -1
                    e = TicTacToe.check(s)
        else:
            while (e == None and len(np.where(s==0)) > 0):
                r,c = P1.play(s,-1)
                s[r][c] = -1
                e = TicTacToe.check(s)
                if (e == None and len(np.where(s==0)) > 0):
                    r,c = P2.play(s,1)
                    s[r][c] = 1
                    e = TicTacToe.check(s)
        #########################################
        return e

    # ----------------------------------------------
    def backprop(self,e):
        '''
         back propagation: update the game result in parent nodes recursively until reaching the root node. 
         Along the way, update w (sum of simulation results) and N (count of simulations).
          Inputs:
                e: the result of the game (X player won:1, tie:0, lose: -1), an integer scalar. 
          Hint: you could use recursion to solve this problem.
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        self.N += 1
        self.w += e
        if (self.parent != None):
            self.parent.backprop(e)

        #########################################
 

    # ----------------------------------------------
    def selection(self):
        '''
         select a child node of this node with the highest UCB bound,  
         then recursively select the child node's child node until reaching a leaf node (with no child)
         Note: When computing UCB, you need to compute the winning rate of each child node. 
               Depending on the role of the current node (X player or O player), the value should be computed differently.
               For example, if the current node is an X player, and in one of its children node, the sum of results (10), then the winning rate of the child node will be 10/N.
               However, if the current node is an O player, then the winning rate of the child node will be -10/N.
          Outputs:
                n: the leaf node selected
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # stopping condition: leaf node (no child)
        if (len(self.c) == 0):
            return self

        # child ucbs
        ucbs = {}
        for c in self.c:
            # winning rate        
            if (self.x == 1):
                if (c.N == 0):
                    wr = 0
                else:
                    wr = c.w
            else:
                if (c.N == 0):
                    wr = 0
                else:
                    wr = -1*c.w
            ucb = UCBplayer.UCB(wr,c.N,self.N)
            ucbs[c] = ucb

        # recurse for max
        maxv = float('-inf')
        selected = []
        for key,value in ucbs.items():
            if value >= maxv:
                maxv = value
                selected = key

                
        n = selected.selection()
        
        #########################################
        return n

    # ----------------------------------------------
    def build_tree(self, n_iter=100):
        '''
        Given the root node of the game (current state of a game), 
        build a search tree by n iteration of (selection->expand(selection)->rollout->backprop).
        After expanding a node, you need to run another selection operation starting from the expanded node, 
        before performing rollout.
        Inputs: 
            n_iter: number of iterations, an interger scalar
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # iterate n_iter times
        for i in range(n_iter):
            # selection from root node (step 1)
            s = self.selection()
            # expand the selected in step 1 (step 2)
            s.expand()
            # selection from expanded node (step 3) 
            es = s.selection()
            # rollout from the selected node in step 3
            e = es.rollout()
            # backprop the result of the rollout
            es.backprop(e)
        #########################################


#-------------------------------------------------------
class PlayerMCTS:
    '''a player, that chooses optimal moves by Monte Carlo tree search. '''

    # ----------------------------------------------
    def play(self,s,x=1,n_iter=100):
        '''
           the policy function of the MCTS player, which chooses one move in the game.  
           Build a search tree with the current state as the root. Then find the most visited child node as the next action.
           Inputs:
                s: the current state of the game, an integer matrix of shape 3 by 3. 
                    s[i,j] = 0 denotes that the i-th row and j-th column is empty
                    s[i,j] = 1 denotes that the i-th row and j-th column is taken by you. (for example, if you are the "O" player, then i, j-th slot is taken by "O") 
                    s[i,j] = -1 denotes that the i-th row and j-th column is taken by the opponent.
                x: the role of the player, 1 if you are the "X" player in the game
                    -1 if you are the "O" player in the game. 
                n_iter: number of iterations when building the tree, an interger scalar
           Outputs:
                r: the row number, an integer scalar with value 0, 1, or 2. 
                c: the column number, an integer scalar with value 0, 1, or 2. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE
        # build a search tree, which the current state as the root
        state = Node(s,x)
        state.build_tree(n_iter)

        # find the best next move: the children node which has been visited(selected) the most
        maxN = 0
        bestc = None
        for c in state.c:
            if (c.N > maxN):
                maxN = c.N
                bestc = c
                
        r = 0
        c = 0
        print(state.s)
        print("what")
        print(bestc.s)

        print("s",state.s)
        print("t",bestc.s)
        for i in range(len(state.s)):
            for j in range(len(state.s[i])):
                if (state.s[i][j] != bestc.s[i][j]):
                    r = i
                    c = j



        #########################################
        return r,c






