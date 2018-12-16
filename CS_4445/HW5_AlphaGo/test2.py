from problem2 import *
import sys
import numpy as np

'''
    Unit test 2:
    This file includes unit tests for problem2.py.
    You could test the correctness of your code by typing `nosetests -v test2.py` in the terminal.
'''
#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 2 (20 points in total)--------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)


#-------------------------------------------------------------------------
def test_ucb():
    '''(5 points) UCBplayer_UCB'''
    p = UCBplayer(3)
    u = p.UCB(1,1,1)
    assert u==1.

    u = p.UCB(1,2,2)
    assert np.allclose(u,1.172, atol=1e-3)

    u = p.UCB(6,50,100)
    assert np.allclose(u,0.4665, atol=1e-3)

    u = p.UCB(6,50,100,c=2.)
    assert np.allclose(u,0.7269, atol=1e-3)

    u = p.UCB(0,0,100,c=2.)
    assert np.allclose(u,float('inf'), atol=1e-3)
 
#-------------------------------------------------------------------------
def test_ucb_policy():
    '''(5 points) ucb_policy '''
    m = UCBplayer(3) 
    a = m.policy()
    assert a ==0

    m.N=1
    m.ni=np.array([1,0,0])
    m.w=np.array([1,0,0])
    a = m.policy()
    assert a ==1

    m.N=2
    m.w=np.array([1,1,0])
    m.ni=np.array([1,1,0])
    a = m.policy()
    assert a ==2

    m.N=4
    m.w=np.array([1,1,1])
    m.ni=np.array([2,1,1])
    a = m.policy()
    assert a ==1


 
#-------------------------------------------------------------------------
def test_update():
    '''(5 point) UCBplayer_update '''
    m = UCBplayer(3) 
    m.update(1,1.)
    assert m.N==1
    assert np.allclose(m.ni, [0,1,0])
    assert np.allclose(m.w, [0.,1.,0.])
    m.update(1,0.)
    assert m.N==2
    assert np.allclose(m.ni, [0,2,0])
    assert np.allclose(m.w, [0.,1.,0.])
    m.update(1,1.)
    assert m.N==3
    assert np.allclose(m.ni, [0,3,0])
    assert np.allclose(m.w, [0.,2.,0.],atol = 1e-2)

    m.update(2,0.)
    assert m.N==4
    assert np.allclose(m.ni, [0,3,1])
    assert np.allclose(m.w, [0.,2.,0.],atol = 1e-2)

    m.update(2,1.)
    assert m.N==5
    assert np.allclose(m.ni, [0,3,2])
    assert np.allclose(m.w, [0.,2.,1.],atol = 1e-2)


#-------------------------------------------------------------------------
def test_play():
    '''(5 point) UCBplayer_play '''
    p = [.2,.6,.8]
    g = Bandit(p)
    m = UCBplayer(3) 
    m.play(g,n_steps=3)
    assert np.allclose(m.ni,np.ones(3),atol=1e-1)
    assert m.N ==3

    m.play(g)
    assert np.argmax(m.ni)==2
    print(m.ni)
    print(m.w)
    assert np.allclose(m.w[2]/m.ni[2],.8,atol=1e-1)

    p = [.5,.6,.3]
    g = Bandit(p)
    m = UCBplayer(3) 
    m.play(g)
    assert np.argmax(m.ni)==1
    print(m.ni)
    print(m.w)
    assert np.allclose(m.w[1]/m.ni[1],.6,atol=1e-1)

    p = [.5,.4,.3]
    g = Bandit(p)
    m = UCBplayer(3) 
    m.play(g)
    assert np.argmax(m.ni)==0
    print(m.ni)
    print(m.w)
    assert np.allclose(m.w[0]/m.ni[0],.5,atol=1e-1)




