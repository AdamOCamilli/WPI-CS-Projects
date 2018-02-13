import copy

class State:
    state = [[]]    # Holds the state of the board with labels for all game pieces
    children = []   # Holds children states that are generated for minimax tree
    xMove = -1      # x-coordinate/column value for move that was made in between the parent state and the current state
    yMove = -1      # y-coordinate/column value for move that was made in between the parent state and the current state
    value = 0       # Value of current state. Used for move selection in Minimax algorithm.

    '''
    Constructor
    @param initState:    A two dimensional array, where each element is a spot on a gomoku board. 0 represent empty,
        1 represents a piece belonging to the current player, and -1 represents a piece belonging to the opponent
    '''
    def __init__(self, initState):
        self.state = initState
        self.children = []

    '''get_emptoes_around
    Gets a list of ordered pairs, which are in the order row,col, around a given point on the board
    @param row:  row/y coordinate of point on board whose surroundings are considered
    @param col:  col/x coordinate of point on board whose surroundings are considered
    '''
    def get_empties_around(self,row,col):
        empties = []
        our_symbol = self.state[row][col]
        # Just check all eight adjacent spots individually
        # North
        if (row - 1 >= 0 and self.state[row - 1][col] == 0):
            empties.append([row-1,col])
        # South
        if (row + 1 < len(self.state) and self.state[row + 1][col] == 0):
            empties.append([row+1,col])
        # East
        if (col - 1 >= 0 and self.state[row][col - 1] == 0):
            empties.append([row,col-1])
        # West
        if (col + 1 < len(self.state) and self.state[row][col + 1] == 0):
            empties.append([row,col+1])
        # Northeast (--)
        if (row - 1 >= 0 and col - 1 >= 0 and self.state[row - 1][col - 1] == 0):
            empties.append([row-1,col-1])
        # Northwest (-+)
        if (row - 1 >= 0 and col + 1 < len(self.state) and self.state[row - 1][col + 1] == 0):
            empties.append([row-1,col+1])
        # Southeast (+-)
        if (row + 1 < len(self.state) and col - 1 >= 0 and self.state[row + 1][col - 1] == 0):
            empties.append([row+1,col-1])
        # Southwest (++)
        if (row + 1 < len(self.state) and col + 1 < len(self.state) and self.state[row + 1][col + 1] == 0):
            empties.append([row+1,col+1])
        return empties

    '''findChildren
    Recursively finds children of current state to build a minimax tree. Integrates alpha beta pruning during 
        generation by evaluating alpha, beta, and current value of a state as it is generating the tree. Only 
        considers places on the board that surround pieces already on the board, and the center piece. 
    @param depth:    How far down the function will recursively call. Decreases by one every count. As more pieces are 
        placed on the board, the depth decreases towards zero faster to limit the depth of the tree as the game goes on.
    @param turn:     Who's turn it currently is for children generation purposes. 1 is for Max, -1 is for Min.
    @param alpha:   Alpha value that is passed down recursively. Contains largest value max has seen in its children.
    @param beta:    Beta value that is passed down recursively. Contains smallest value min has seen it its children.
    @return: Value of the board that the person playing (either Max or Min) would choose as their move.
    '''
    def findChildren(self,depth,turn,alpha,beta):
        locAlpha = alpha
        locBeta = beta
        minimax = 0
        if(depth <= 0): # if at end of depth, return value of board
            self.value = self.evaluate()
            return self.value
        spotsToConsider = [[7,6]]
        for y in range(0,len(self.state)):# iterate through board and save spots near spots that have pieces in them
            for x in range(0,len(self.state[y])):
                if not self.state[y][x] == 0:
                    spotsToConsider += self.get_empties_around(y,x)
        for spot in spotsToConsider:
            # generate new board as a possible move, then add it to children and evaluate minimax
            if self.state[spot[0]][spot[1]] == 0:
                tempboard = copy.deepcopy(self.state)
                tempboard[spot[0]][spot[1]] = turn
                newState = State(tempboard)
                newState.xMove = spot[1]
                newState.yMove = spot[0]
                self.children.append(newState)
                if len(spotsToConsider) < 32:
                    minimax = newState.findChildren(depth-1,turn * -1,locAlpha,locBeta)
                elif len(spotsToConsider) < 90:
                    minimax = newState.findChildren(depth-2,turn * -1,locAlpha,locBeta)
                else:
                    minimax = newState.findChildren(depth-3,turn * -1,locAlpha,locBeta)

                if turn == -1:
                    if minimax < locBeta:
                        locBeta = minimax
                    # if the alpha passed down is greater than the beta we've found, the max player will ignore this choice
                    # so we can prune this tree
                    if locAlpha >= locBeta:
                        self.value = locBeta
                        #print("Pruned:",x,y)
                        return locBeta
                if turn == 1:
                    if minimax > locAlpha:
                        locAlpha = minimax
                    # if the beta passed down is less than the alpha we've found, the min player will ignore this choice
                    # so we can prune this tree
                    if locAlpha >= locBeta:
                        self.value = locAlpha
                        print("Pruned:",spot[1],spot[0])
                        return locAlpha
        if turn == -1:
            self.value = locBeta
            return locBeta
        else:
            self.value = locAlpha
            return locAlpha


    '''checkString
    Checks if board has a string of characters in a row. A longer length string in a row returns a larger state 
        value by a factor of 10.
    @param player:  1 for if we are playing, -1 if opponent is playing.
    @param length:  How many characters in a row the method checks for.
    @return:        Value of current state in relation to our script winning the game.
    '''
    def checkString(self, player,length):
        coord = None
        val = 0
        for y in range(0, len(self.state)): #iterate through board
            for x in range(0, len(self.state[y])):
                if self.state[y][x] == player: #check string only for certain player
                    #check if string is of same characters horizontally by grouping the string into a set and checking length
                    if x <= 15-length and len(set([self.state[y][i] for i in range(x, x + length)])) == 1:
                        coord = [[y,x],[y,x+length-1]] #we do x+length-1 to account for 0 index
                        val += self.checkEnds(player,length,coord) * 10**length
                    #check if string is of same characters vertically by grouping the string into a set and checking length
                    if y <= 15-length and len(set([self.state[i][x] for i in range(y, y + length)])) == 1:
                        coord = [[y,x],[y+length-1,x]] #we do y+length-1 to account for 0 index
                        val += self.checkEnds(player,length,coord) * 10**length
                    #check if string is of same characters diagonally from top left to bottom right
                    #by grouping the string into a set and checking length
                    if x <= 15-length and y <= 15-length and len(set([self.state[y+i][x+i] for i in range(0,length)])) == 1:
                        coord = [[y,x],[y+length-1,x+length-1]] #we do y+length-1 to account for 0 index
                        val += self.checkEnds(player,length,coord) * 10**length
                    #check if string is of same characters diagonally from top right to bottom left
                    #by grouping the string into a set and checking length
                    if x >= length and y <= 15-length and len(set([self.state[y+i][x-i] for i in range(0,length)])) == 1:
                        coord = [[y,x],[y+length-1,x-length+1]] #we do y+length-1 to account for 0 index
                        val += self.checkEnds(player,length,coord) * 10**length
        return val

    '''checkEnds
    Checks the state of strings of pieces of same type on the current board. Returns a modifier for altering how heavy
        the value determined in checkString is weighted. Strings that are "blocked" by pieces of different color on 
        the ends are worth less than strings with "open" ends
    @param length:  The length of the string of pieces of same type that are described in coord
    @param coord:   A list of coordinate pairs that are the ends of a string of pieces of the same type on the board.
    @return:        Returns 0 for strings with both ends blocked, 0.5 for strings with only one end blocked, and 1 for
        strings with no ends blocked. 
    '''
    def checkEnds(self,player,length,coord):
        if length >= 5:
            return 1
        print(coord,length)
        if len(set([c[0] for c in coord])) == 1:#horizontal case
            if coord[0][1] == 0 or coord[1][1] == len(self.state)-1:#horizontal string is in a corner
                return 0.5
            elif self.state[coord[0][0]][coord[0][1]-1] == 0 and self.state[coord[1][0]][coord[1][1]+1] == 0: #both ends of string are empty
                return 1
            elif not self.state[coord[0][0]][coord[0][1]-1] == 0 or not self.state[coord[1][0]][coord[1][1]+1] == 0: #only one side of the string is open,
                return 0.5
            else: #both sides of string have pieces next to them.
                return 0
        elif len(set([c[1] for c in coord])) == 1: #vertical case
            if coord[0][0] == 0 or coord[1][0] == len(self.state)-1:#vertical string is in a corner
                return 0.5
            elif self.state[coord[0][0]-1][coord[0][1]] == 0 and self.state[coord[1][0]+1][coord[1][1]]== 0: #both ends of string are empty
                return 1
            elif not self.state[coord[0][0]-1][coord[0][1]] == 0 or not self.state[coord[1][0]+1][coord[1][1]] == 0: #only one side of the string is open,
                return 0.5
            else:#both sides of string have pieces next to them
                return 0
        else:
            if coord[0][1] < coord[1][1]: #top left to bottom right diagonal case
                if (coord[0][0] == 0 and coord[1][1] == len(self.state)-1) or (coord[0][1] == 0 and coord[1][0] == len(self.state)-1): #case for when diagonal string is in corner
                    return 0
                elif coord[0][0] == 0 or coord[0][1] == 0 or coord[1][0] == len(self.state)-1 or coord[1][1] == len(self.state)-1: #case for when diagonal string is near edge of board
                    return 0.5
                elif self.state[coord[0][0]-1][coord[0][1]-1] == 0 and self.state[coord[1][0]+1][coord[1][1]+1] == 0: #both ends of string are empty
                    return 1
                elif self.state[coord[0][0]-1][coord[0][1]-1] != self.state[coord[1][0]+1][coord[1][1]+1] == 0:#only one side of the string is open,
                    return 0.5
                else: #both sides of string have pieces next to them
                    return 0
            elif coord[0][1] > coord[1][1]:#top right to bottom left diagonal case
                if (coord[0][0] == 0 and coord[1][1] == 0) or (coord[0][1] == len(self.state)-1 and coord[1][0] == len(self.state)-1): #case for when diagonal string is in corner
                    return 0
                elif coord[0][0] == 0 or coord[0][1] == len(self.state)-1 or coord[1][1] == 0 or coord[1][0] == len(self.state)-1: #case for when diagonal string is near edge of board
                    return 0.5
                elif self.state[coord[0][0]-1][coord[0][1]+1] == 0 and self.state[coord[1][0]+1][coord[1][1]-1] == 0:
                    return 1
                elif self.state[coord[0][0]-1][coord[0][1]+1] != self.state[coord[1][0]+1][coord[1][1]-1]:
                    return 0.5
                else:
                    return 0

    '''evaluate
    Determines the value of the current state of the board using checkString()
    @return: Total value of current state.
    '''
    def evaluate(self):
        val = 0
        for i in range(0,15):
            for j in range(0,15):
                if i > 5 and i < 9 and j > 5 and i < 9:
                    val += self.state[i][j]
                if i > 3 and i < 11 and j > 3 and j < 11:
                    val += self.state[i][j]
                val += self.state[i][j]
        for i in range(2,6):
            val += self.checkString(1,i)
            val -= self.checkString(-1,i)
        return val
