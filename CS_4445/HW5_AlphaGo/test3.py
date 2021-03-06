from problem3 import *
from problem1 import PlayerMiniMax
import numpy as np
import sys

'''
    Unit test 3:
    This file includes unit tests for problem3.py.
    You could test the correctness of your code by typing `nosetests -v test3.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 3 (40 points in total)---------------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)

#-------------------------------------------------------------------------
def test_expand():
    '''(5 points) expand'''
    s=np.array([[ 0,-1,-1],
                [ 0, 1, 1],
                [-1, 1,-1]])
    n = Node(s,x=1)
    n.expand()
    assert n.N==0
    assert n.w==0
    assert len(n.c) ==2 
    assert n.x==1
    s_=np.array([[ 0,-1,-1],
                 [ 0, 1, 1],
                 [-1, 1,-1]])
    assert np.allclose(n.s,s_)
    for c in n.c:
        assert c.parent == n
        assert c.x==-1

    s=np.array([[ 0,-1,-1],
                [ 1, 1, 1],
                [-1, 1,-1]])
    c = False
    for x in n.c:
        if np.allclose(x.s,s):
            c=True
    assert c

    s=np.array([[ 1,-1,-1],
                [ 0, 1, 1],
                [-1, 1,-1]])
    c = False
    for x in n.c:
        if np.allclose(x.s,s):
            c=True
    assert c



    s=np.array([[ 0,-1,-1],
                [-1, 1, 1],
                [-1, 1,-1]])
    n = Node(s,-1)
    n.expand()
    assert n.x==-1
    assert len(n.c) ==1
    assert n.c[0].x==1


    s=np.array([[ 0,-1,-1],
                [ 1, 1, 1],
                [-1, 1,-1]])
    n = Node(s)
    n.expand()
    assert len(n.c) ==0


#-------------------------------------------------------------------------
def test_rollout():
    '''(5 points) rollout'''
    # the game has already endded
    s=np.array([[-1, 0, 0],
                [ 1,-1, 1],
                [ 0, 0,-1]])
    n = Node(s)
    for _ in range(100):
        assert n.rollout()==-1
    
    s_=np.array([[-1, 0, 0],
                 [ 1,-1, 1],
                 [ 0, 0,-1]])
    assert np.allclose(n.s,s_)

    s=np.array([[-1,-1, 1],
                [ 1, 1,-1],
                [-1, 1,-1]])
    n = Node(s)
    for _ in range(100):
        assert n.rollout()==0


 
    s=np.array([[ 0,-1,-1],
                [ 0, 1, 1],
                [-1, 1,-1]])
    w = 0
    n = Node(s)
    for _ in range(1000):
        w += n.rollout()

    print(w)
    assert np.abs(w-500) <100
    s_=np.array([[ 0,-1,-1],
                 [ 0, 1, 1],
                 [-1, 1,-1]])
    assert np.allclose(s,s_)

    s=np.array([[ 0, 0, 0],
                [ 0, 1, 0],
                [ 0, 0, 0]])
    n = Node(s)
    w = 0
    for _ in range(1000):
        w += n.rollout()
    assert np.abs(w-800) <100


    s=np.array([[ 0, 0, 0],
                [ 0,-1, 0],
                [ 0, 0, 0]])
    n = Node(s)
    w = 0
    for _ in range(1000):
        w += n.rollout()
    assert np.abs(w+500) <100

    s=np.array([[ 0,-1,-1],
                [ 0, 1, 1],
                [-1, 1,-1]])
    w = 0
    n = Node(s,x=-1)
    for _ in range(1000):
        w += n.rollout()
    print(w)
    assert np.abs(w+500) <100
 

#-------------------------------------------------------------------------
def test_backprop():
    '''(5 points) backprop'''
    s=np.array([[ 0,-1,-1],
                [ 0, 1, 1],
                [ 0, 1,-1]])
    n = Node(s) 
    n.expand() # expand one level of children nodes
    c = n.c[1] #what if the X player choose the second position
    c.backprop(1) # X player won, back propagate
    assert c.w ==1 # won one game in the simulation
    assert c.N ==1 # number of simulations in the node 
    assert n.w ==1 
    assert n.N ==1


    c = n.c[2] # what if the X player choose the third position
    c.expand() # expand the tree with one level of children nodes
    cc = c.c[0] # what if O player choose the first position
    cc.backprop(-1) # then O player won the game
    assert cc.w ==-1 
    assert cc.N ==1
    assert c.w ==-1 
    assert c.N ==1
    assert n.w ==0 #because X player won and lose in the previous two backprop operations.
    assert n.N ==2


    cc = c.c[1] # what if O player choose the second position
    cc.backprop(0) # a tie in the game
    assert cc.w ==0 
    assert cc.N ==1
    assert c.w ==-1 
    assert c.N ==2
    assert n.w ==0 
    assert n.N ==3

#-------------------------------------------------------------------------
def test_selection():
    '''(5 points) selection'''

    #leaf node
    s=np.array([[ 0,-1,-1],
                [ 1, 1, 1],
                [-1, 1,-1]])
    n = Node(s)
    node = n.selection()
    assert node ==n

    s=np.array([[ 0,-1,-1],
                [ 0, 1, 1],
                [-1, 1,-1]])
    n = Node(s)
    node = n.selection()
    assert node ==n

    # UCB with on level of children
    s=np.array([[ 0,-1,-1],
                [ 0, 1, 1],
                [-1, 1,-1]])
    n = Node(s)
    n.expand()
    c1 = n.c[0]
    c1.backprop(-1)
    node = n.selection()
    c2 = n.c[1]
    assert node ==c2


    # if the current node is O player
    n = Node(s,x=-1)
    n.expand()
    c1 = n.c[0]
    c2 = n.c[1]
    c1.backprop(-1)
    c2.backprop(0)
    node = n.selection()
    assert node ==c1
    
    # recursion
    s=np.array([[ 0, 1,-1],
                [ 0,-1, 1],
                [ 1,-1, 0]])
    n = Node(s)
    n.expand() # 1st level of children nodes
    c1 = n.c[0]
    c2 = n.c[1]
    c3 = n.c[2]
    assert c1.x==-1
    assert c2.x==-1
    assert c3.x==-1
    c1.backprop(0)
    c2.backprop(1)
    c3.backprop(0)
    node = n.selection()
    assert node == c2
    
    c2.expand() # 2nd level of children nodes
    assert len(c2.c)==2
    c2c1=c2.c[0]
    c2c2=c2.c[1]
    assert c2c1.x==1
    assert c2c2.x==1
    c2c1.backprop(0) 
    node = n.selection()
    # print(c2c2.w)
    print("c2c2",c2c2)
    # ucb = UCBplayer.UCB(c2c2.w/c2c2.parent.N,c2c2.N,c2c2.parent.N)
    # print("ucb",ucb,c2c2)
    assert node ==c2c2 
    c2c2.backprop(1)
    node = n.selection()
    assert node ==c2c1
   
    c2c1.backprop(0)
    node = n.selection()
    assert node ==c1


#-------------------------------------------------------------------------
def test_build_tree():
    '''(5 points) build_tree'''
    s=np.array([[ 0, 1,-1],
                [ 0,-1,-1],
                [ 1,-1, 1]])

    n = Node(s)
    n.build_tree(1)
    assert len(n.c)==2 
    c0=n.c[0]
    c1=n.c[1]
    print(c0.s)
    s0=np.array([[ 1, 1,-1],
                 [ 0,-1,-1],
                 [ 1,-1, 1]])
    assert np.allclose(s0,c0.s)
    assert c0.x==-1
    print(c1.s)
    s1=np.array([[ 0, 1,-1],
                 [ 1,-1,-1],
                 [ 1,-1, 1]])
    assert np.allclose(s1,c1.s)
    assert c1.x==-1
    print("c",c0.N)
    assert c0.N==1
    assert c1.N==0
    assert n.N==1
    assert c0.w==-1
    assert c1.w==0
    assert n.w==-1
    assert len(c0.c)==0
    assert len(c0.c)==0
     
    n.build_tree(1)
    assert c0.N==1
    assert c1.N==1
    assert n.N==2
    assert c0.w==-1
    assert c1.w==0
    assert n.w==-1
    assert len(c0.c)==0
    assert len(c1.c)==1
    c1c = c1.c[0]
    assert c1c.x==1
    assert c1c.N==1
    assert c1c.w==0
    
    n.build_tree(1)
    assert c0.N==1
    assert c1.N==2
    assert n.N==3
    assert c0.w==-1
    assert c1.w==0
    assert n.w==-1
    assert len(c0.c)==0
    assert len(c1.c)==1
    assert c1c.N==2
    assert c1c.w==0


    n = Node(s)
    n.build_tree(8)
    assert n.c[0].N==1
    assert n.c[1].N==7
    assert n.c[0].w==-1
    assert n.c[1].w==0

    n = Node(s)
    n.build_tree(9)
    assert n.c[0].N==2
    assert n.c[1].N==7
    assert n.c[0].w==-2
    assert n.c[1].w==0


    s=np.array([[ 0, 1, 1],
                [ 0,-1, 0],
                [ 0, 0, 0]])
    r = Node(s,x=-1)
    r.build_tree(1000)
    print( r.c[0].N)
    print( r.c[0].w)
    assert r.c[0].N >900 
    assert r.c[0].w < 50
    assert r.c[0].w >-50

    print( r.c[0].c[-1].N)
    print( r.c[0].c[-1].w)
    assert r.c[0].c[-1].N >800 
    assert r.c[0].c[-1].w <50
    assert r.c[0].c[-1].w >-50

    print( r.c[0].c[-1].c[1].N)
    print( r.c[0].c[-1].c[1].w)
    assert r.c[0].c[-1].c[1].N >750 
    assert r.c[0].c[-1].c[1].w <50
    assert r.c[0].c[-1].c[1].w >-50
    
    print( r.c[0].c[-1].c[1].c[0].N)
    print( r.c[0].c[-1].c[1].c[0].w)
    assert r.c[0].c[-1].c[1].c[0].N >700 
    assert r.c[0].c[-1].c[1].c[0].w <50
    assert r.c[0].c[-1].c[1].c[0].w >-50
    



#-------------------------------------------------------------------------
def test_play():
    '''(5 points) play'''
    p =PlayerMCTS()
    s=np.array([[ 0,-1,-1],
                [ 0, 1, 0],
                [ 0, 0, 0]])
     
    r,c=p.play(s)
    assert r ==0
    assert c ==0

    s=np.array([[ 0, 0,-1],
                [ 0, 1,-1],
                [ 0, 0, 0]])
     
    r,c=p.play(s)
    assert r ==2
    assert c ==2


    s=np.array([[ 0, 0, 1],
                [ 0,-1, 1],
                [ 0, 0, 0]])
     
    r,c=p.play(s,x=-1)
    assert r ==2
    assert c ==2


#-------------------------------------------------------------------------
def test_players():
    '''(5 points) random vs MCTS'''

    p1 = PlayerMCTS()
    p2 = PlayerRandom()
    w=0
    for i in range(10):
        g = TicTacToe()
        g.s=np.array([[ 0,-1, 1],
                      [-1, 1,-1],
                      [ 0,-1,-1]])
        e = g.game(p1,p2)
        w += e
    assert w==10

    w=0
    for i in range(10):
        g = TicTacToe()
        g.s=np.array([[ 0,-1, 1],
                      [-1, 1,-1],
                      [-1, 1, 0]])
        e = g.game(p1,p2)
        w += e
    assert w==0


#-------------------------------------------------------------------------
def test_players2():
    '''(5 points) Minimax vs MCTS '''

    p1 = PlayerMCTS()
    p2 = PlayerMiniMax()
    w=0
    for i in range(10):
        g = TicTacToe()
        g.s=np.array([[ 0, 0, 1],
                      [ 0,-1, 0],
                      [ 1,-1, 0]])
        e = g.game(p1,p2)
        w += e
    assert w==0


    w=0
    for i in range(10):
        g = TicTacToe()
        g.s=np.array([[ 0, 0, 0],
                      [ 0, 0, 0],
                      [ 1,-1, 0]])
        e = g.game(p1,p2)
        w += e
    print(w)
    assert w>5
