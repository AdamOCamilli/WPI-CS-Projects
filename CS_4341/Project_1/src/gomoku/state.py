import copy

class State:
    state = [[]]
    children = []
    xMove = -1
    yMove = -1
    value = 0

    def __init__(self, initState):
        self.state = initState
        self.children = []

    def getState(self,x,y):
        for state in self.children:
            if state.xMove == x and state.yMove == y:
                return state
        return None

    # recursively finds children until depth is 0, then returns. turn is -1 if opponent is to make next move, 1 if you are making next move
    def findChildren(self,depth,turn,alpha,beta):
        locAlpha = alpha
        locBeta = beta
        minimax = 0
        if(depth == 0):
            self.value = self.evaluate()
            return self.evaluate()
        if not self.children:
            for y in range(0,len(self.state)):
                for x in range(0,len(self.state[y])):
                    if self.state[y][x] == 0:
                        tempboard = copy.deepcopy(self.state)
                        tempboard[y][x] = turn
                        newState = State(tempboard)
                        newState.xMove = x
                        newState.yMove = y
                        self.children.append(newState)
                        minimax = newState.findChildren(depth-1,turn * -1,locAlpha,locBeta)
                        if turn == -1:
                            if minimax < locBeta:
                                locBeta = minimax
                            if locAlpha >= locBeta:
                                self.value = locBeta
                                return locBeta
                        if turn == 1:
                            if minimax > locAlpha:
                                locAlpha = minimax
                            if locAlpha >= locBeta:
                                self.value = locAlpha
                                return locAlpha
            if turn == -1:
                self.value = locBeta
                return locBeta
            else:
                self.value = locAlpha
                return locAlpha
            return minimax
        else:
            for child in self.children:
                minimax = child.findChildren(depth-1,turn*-1,locAlpha,locBeta)
                if turn == -1:
                    if minimax < locBeta:
                        locBeta = minimax
                    if locAlpha >= locBeta:
                        self.value = locBeta
                        return locBeta
                if turn == 1:
                    if minimax > locAlpha:
                        locAlpha = minimax
                    if locAlpha >= locBeta:
                        self.value = locAlpha
                        return locAlpha
            if turn == -1:
                self.value = locBeta
                return locBeta
            else:
                self.value = locAlpha
                return locAlpha



    # checks win conition for certain player.
    def checkWin(self, player):
        for y in range(0, len(self.state)):
            for x in range(0, len(self.state[y])):
                if self.state[y][x] == player:
                    if (x < 10 and len(set([self.state[y][i] for i in range(x, x + 5)])) == 1) or \
                            (x > 5 and len(set([self.state[y][i] for i in range(x, x - 5)])) == 1) or \
                            (y < 10 and len(set([self.state[i][x] for i in range(y, y + 5)])) == 1) or \
                            (y > 5 and len(set([self.state[i][x] for i in range(y, y - 5)])) == 1):
                        print(x, y)
                        return True
        return False

    # TODO Actually write this
    def evaluate(self):
        val = 0
        for i in range(0,15):
            for j in range(0,15):
                if i > 6 and i < 9 and j > 6 and i < 9:
                    val += self.state[i][j]
                if i > 3 and i < 12 and j > 3 and j < 12:
                    val += self.state[i][j]
                    val += self.state[i][j]
        if self.checkWin(1):
            val += 100
        elif self.checkWin(-1):
            val -= 100
        return val
