'''
Created on Feb 5, 2018

@author: aocamilli
'''
''' Represents a Gomoku board where 
'''
class Board(object):
    
    ''' Constructor
    @param x: The row length
    @param y: The column length
    @param p1: Symbol representing p1
    @param p2: Symbol representing p2
    @param free: Symbol representing free spot
    @param board: A 2d array of strings representing either a player's symbol or a free spot 
    '''
    def __init__(self,x,y,p1,p2,free):
        self.x = x
        self.y = y
        self.p1 = p1
        self.p2 = p2
        self.free = free
        self.board = [[free for j in range(x)] for i in range(y)]
    
    ''' Return a deep copy of another board
    @param other: A Board object to be deep copied
    '''
    def copy(self,other):
        x = other.x
        y = other.y
        win_num = other.win_num
        p1 = other.p1
        p2 = other.p2
        free = other.free
        return Board(x, y, p1, p2, free)
    
    def isfree(self,x,y):
        return (self.board[x][y] == self.free)
    
    ''' Place move on board. Note this function does not check validity of move!
    @param row
    @param col
    @param player: Symbol to place on board

    '''
    def place(self,row,col,player):
        self.board[row][col] = player
    
    ''' Return list of empty locations in the form of an array of tuples (x,y)
    '''
    def get_empties(self):
        empties = []
        for i in range(0,self.x):
            for j in range(0,self.y):
                if (self.board[i][j] == self.free):
                    empties.append([i,j])
        return empties
        
    ''' Return list of empty spots around a location
    '''
    def get_empties_around(self,row,col):
        empties = []
        our_symbol = self.board[row][col]
        # Just check all eight adjacent spots individually
        # North
        if (row - 1 >= 0 and self.board[row - 1][col] == our_symbol):
            empties.append([row-1,col])
        # South
        if (row + 1 < self.y and self.board[row + 1][col] == our_symbol):
            empties.append([row+1,col])
        # East
        if (col - 1 >= 0 and self.board[row][col - 1] == our_symbol):
            empties.append([row,col-1])
        # West
        if (col + 1 < self.x and self.board[row][col + 1] == our_symbol):
            empties.append([row,col+1])
        # Northeast (--)
        if (row - 1 >= 0 and col - 1 >= 0 and self.board[row - 1][col - 1] == our_symbol):
            empties.append([row-1,col-1])
        # Northwest (-+)
        if (row - 1 >= 0 and col + 1 < self.x and self.board[row - 1][col + 1] == our_symbol):
            empties.append([row-1,col+1])
        # Southeast (+-)
        if (row + 1 < self.y and col - 1 >= 0 and self.board[row + 1][col - 1] == our_symbol):
            empties.append([row+1,col-1])
        # Southwest (++)
        if (row + 1 < self.y and col + 1 < self.x and self.board[row + 1][col + 1] == our_symbol):
            empties.append([row+1,col+1])    
        return empties
        
    ''' Return list of player's locations
    '''
    def get_places(self,player):
        places = []
        for i in range(0,self.x):
            for j in range(0,self.y):
                if (self.board[i][j] == player):
                    places.append([i,j])
        return places
    
    ''' Return list of locations occupied by a given player
    @param player: Symbol of player to look for
    '''
    def get_player_spots(self,player):
        spots = []
        for i in range(0,self.x):
            for j in range(0,self.y):
                if (self.board[i][j] == player):
                    spots.append([i,j])
        return spots          

    ''' Returns the number of near wins for a given vertical length (i.e. a number of rows).
        @param length: Length of pieces in a row we are searching for
        @param symbol: Which player's piece we are looking for ("X" or "O")
        @return: How many near wins are on the board for given player
    '''
    def col_near_wins(self, length, symbol):
        col_nwins = 0;
        # We need to check two directions for column near wins: 
        
        # North
        for i in range(0,self.x):
            for j in range(0,self.y):
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
                    if (i - length - 1 >= 0):
                        if (self.board[i-length-1][j] == symbol):
                            valid_near_win = False
                    if (valid_near_win):
                        col_nwins += 1
        # South
        for i in range(0,self.x):
            for j in range(0,self.y):
                # Ignore rows which cannot have south wins of given length
                if (i + length > self.y - 1): # Row too large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length): 
                        if (self.board[i+k][j] != symbol):
                            valid_near_win = False
                            break
                    if (i + length + 1 < self.y):
                        if (self.board[i+length+1][j] == symbol):
                            valid_near_win = False
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
        for i in range(0,self.x):
            for j in range(0,self.x):
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
                    if (j - length - 1 >= 0):
                        if (self.board[i][j-length-1] == symbol):
                            valid_near_win = False
                    if (valid_near_win):
                        row_nwins += 1
        # East
        for i in range(0,self.x):
            for j in range(0,self.y):
                # Ignore rows which cannot have south wins of given length
                if (j + length > self.x): # Row too large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length): 
                        if (self.board[i][j+k] != symbol):
                            valid_near_win = False
                            break
                    if (j + length + 1 < self.x):
                        if (self.board[i][j+length+1] == symbol):
                            valid_near_win = False
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
        for i in range(0,self.x):
            for j in range(0,self.x):
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
                    # Make sure series is only <length> long and not part of a larger near win
                    if (i - length -1 >= 0 and j - length - 1 >= 0):
                        if (self.board[i-length-1][j-length-1] == symbol):
                            valid_near_win = False
                    if (valid_near_win):
                        diag_nwins += 1
        
        # Northwest (-i,+j)
        for i in range(0,self.x):
            for j in range(0,self.x):
                # Ignore rows which cannot have north wins of given length
                if (i < length or j + length > self.x): # Too small/large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i-k][j+k] != symbol):
                            valid_near_win = False
                            break
                    # Make sure series is only <length> long and not part of a larger near win
                    if (i - length - 1 >= 0 and j + length + 1 < self.x):
                        if (self.board[i-length-1][j+length+1] == symbol):
                            valid_near_win = False
                    if (valid_near_win):
                        diag_nwins += 1
        
        # Southeast (+i,-j)
        for i in range(0,self.x):
            for j in range(0,self.x):
                # Ignore rows which cannot have north wins of given length
                if (i + length > self.y or j < length): # Too large/small
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i+k][j-k] != symbol):
                            valid_near_win = False
                            break
                    # Make sure series is only <length> long and not part of a larger near win
                    if (i + length + 1 < self.y and j - length - 1 >= 0):
                        if (self.board[i+length+1][j-length-1] == symbol):
                            valid_near_win = False
                    if (valid_near_win):
                        diag_nwins += 1
        
        # Southwest (+i,+j)
        for i in range(0,self.x):
            for j in range(0,self.x):
                # Ignore rows which cannot have north wins of given length
                if (i + length > self.y or j + length > self.x): # Too large/large
                    break
                elif (self.board[i][j] != symbol): # This spot doesn't have symbol we are searching for
                    continue
                else:
                    valid_near_win = True
                    for k in range(0,length):
                        if (self.board[i+k][j+k] != symbol):
                            valid_near_win = False
                            break
                    # Make sure series is only <length> long and not part of a larger near win
                    if (i + length + 1 < self.y and j + length + 1 < self.x):
                        if (self.board[i+length+1][j+length+1] == symbol):
                            valid_near_win = False
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
