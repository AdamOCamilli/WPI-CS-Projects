import math
import numpy as np

#-------------------------------------------------------------------------
'''
    Problem 2: Multi-armed bandit problem 
    In this problem, you will implement an AI player for Multi-armed bandit problem using UCB (Upper Confidence Bound).
    The main goal of this problem is to get familiar with a simplified problem in reinforcement learning, and how to train the model parameters on the data from a game.
    You could test the correctness of your code by typing `nosetests test1.py` in the terminal.
'''

#-------------------------------------------------------
class Bandit:
    '''Bandit is the Multi-armed bandit machine. Instead of one slot machine lever, you have a number of arms. Each lever/arm corresponds to a probability of winning. However these odds/probabilities are hidden from the players. '''
    # ----------------------------------------------
    def __init__(self, p):
        ''' Initialize the game. 
            Inputs:
                p: the vector of winning probabilities, a numpy vector of length n. 
                    Here n is the number of arms of the bandit. 
            Outputs:
                self.p: the vector of winning probabilities, a numpy vector of length n. 
        '''
        self.p = p 


    # ----------------------------------------------
    def play(self, a):
        '''
           Given an action (the id of the arm being pulled), return the reward based upon the winning probability of the arm. 
            Input:
                a: the index of the lever being pulled by the agent. a is an integer scalar between 0 and n-1. 
                    n is the number of arms in the bandit.
            Output:
                r: the reward returned to the agent, a float scalar. The "win" return 1., if "lose", return 0. as the reward.
                   The winning probabilty of this step should be the same as that of the lever being pulled by the agent.
        '''
        p = self.p[a]
        r = np.random.choice([0.,1.], 1, p=[1.-p,p])
        return r


#-------------------------------------------------------
class UCBplayer:
    '''The agent is trying to maximize the sum of rewards (payoff) in the game using UCB (Upper Confidence Bound).
       The agent will 
                (1) choose the lever with the largest bound value, (index of the arm is a tie-breaker); 
                (2) update the statistics of each arm after getting the result of a game.'''
    # ----------------------------------------------
    def __init__(self, n,c=1.142):
        ''' Initialize the agent. 
            Inputs:
                n: the number of arms of the bandit, an integer scalar. 
                c: exploration parameter, a float scalar
            Outputs:
                self.n: the number of levers, an integer scalar. 
                self.c: exploration parameter, a float scalar. 
                self.ni: the number of simultions choosing the i-th arm, an integer vector of length n. 
                self.N: total number of simulations, an integer scalar
                self.w: the sum of game results after choosing each arm, a float vector of length n. 
                        w[i] represents the sum of scores achieved by pulling the i-th arm. 
        '''
        self.n = n
        self.c = c
        self.ni =np.zeros(n)
        self.w =np.zeros(n)
        self.N = 0

    # ----------------------------------------------
    @staticmethod
    def UCB(wi,ni,N,c=1.142):
        '''
          compute UCB (Upper Confidence Bound) of a child node (say the i-th child node).
          the average payoffs of the current node vi = wi/ni
          Inputs:
                wi: the sum of game results after choosing the i-th child node, an integer scalar 
                ni: the number of simultions choosing the i-th child node, an integer scalar 
                N: total number of simulations for the parent node
                c: exploration parameter
            Outputs:
                b: the UCB score of the node, a float scalar. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        if (ni == 0):
            return float('inf')
        else:
            vi = wi/ni
            
        b = vi + c * np.sqrt(np.log(N) / ni)


        #########################################
        return b


   # ----------------------------------------------
    def policy(self):
        '''
            The policy function of the agent.
            The agent will choose the lever with the largest bound value, (when there is a tie, use index of the arms as tie-breaker); 
            Output:
                a: the index of the lever to pull. a is an integer scalar between 0 and n-1. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        max_vals = []
        current_max = 0
        for arm in range(self.n):
            ucb = self.UCB(self.w[arm],self.ni[arm],self.N)
            if ucb > current_max:
                current_max = ucb
                max_vals.append([arm,ucb])

        if len(max_vals) == 1:
            a = max_vals[0][0]
        else:
            a = max_vals[len(max_vals)-1][0]

        #########################################
        return a


    #-----------------------------------------------------------------
    def update(self, a, r):
        '''
            Update the parameters of the player after collecting one game result.
            (1) increase the count of the lever and total count.
            (2) update the sum of reward based upon the received reward r.
            Input:
                a: the index of the arm being pulled. a is an integer scalar between 0 and n-1. 
                r: the reward returned, a float scalar. 
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        self.N += 1
        self.ni[a] += 1
        self.w[a] += r

        #########################################


    #-----------------------------------------------------------------
    def play(self, g, n_steps=1000):
        '''
            Play the game for n_steps steps. In each step,
            (1) pull a lever and receive the reward from the game
            (2) update the parameters 
            Input:
                g: the game machine, a multi-armed bandit object. 
                n_steps: number of steps to play in the game, an integer scalar. 
            Note: please do NOT use g.p in this function, which is hidden from the player. The player can only call the g.play() function.
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        # run n_steps iterations
        for i in range(n_steps):
            # take an action
            a = self.policy()
            # run a game to collect the result
            r = g.play(a)
            # update statistics 
            self.update(a,r)

        #########################################


