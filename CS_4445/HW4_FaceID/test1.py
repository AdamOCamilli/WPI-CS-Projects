from problem1 import *
import numpy as np
import sys

'''
    Unit test 1:
    This file includes unit tests for problem2.py.
    You could test the correctness of your code by typing `nosetests -v test1.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 1 (35 points in total)---------------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)

#-------------------------------------------------------------------------
def test_centering_X():
    '''(3 points) centering_X'''
     #-------------------------------
    # an example matrix (2 dimensional)
    X = np.mat([[0., 2.],
                [2., 0.],
                [1., 1.]])

    # call the function
    Xc,mu = centering_X(X)

    # test whether or not X is a numpy matrix 
    assert type(Xc) == np.matrixlib.defmatrix.matrix
    assert Xc.shape == (3,2)

    assert type(mu) == np.matrixlib.defmatrix.matrix
    assert mu.shape == (1,2)

    assert np.allclose(Xc,[[-1.,1.],[1.,-1],[0.,0.]],atol=1e-2)
    assert np.allclose(mu,[1.,1.],atol=1e-2)

    #-------------------------------
    # test on random matrix 
    for _ in range(20):
        n = np.random.randint(3,40)
        p = np.random.randint(3,40)
        X = np.random.random((n,p))
        X = np.asmatrix(X)
        # call the function
        Xc,mu = centering_X(X)

        # test the result
        assert Xc.shape == (n,p)
        assert mu.shape == (1,p)
        assert np.allclose(Xc.sum(),0.,atol=1e-2) 

        i = np.random.randint(p)
        assert np.allclose(X[:,i].sum(), mu[0,i]*n, atol=1e-2)


    

#-------------------------------------------------------------------------
def test_compute_C():
    '''(2 points) compute_C'''

    #-------------------------------
    # an example matrix (2 dimensional)
    Xc = np.mat([[-1., 1.],
                 [ 1.,-1.],
                 [ 0., 0.]])

    # call the function
    C = compute_C(Xc)

    # test whether or not C is a numpy matrix 
    assert type(C) == np.matrixlib.defmatrix.matrix

    # true answer
    C_true = np.mat([[1., -1.],
                     [-1., 1.]])

    # test the result
    assert np.allclose(C,C_true, atol=1e-2)


    #-------------------------------
    # an example matrix (3 dimensional)
    Xc = Xc*2
       
    # call the function
    C = compute_C(Xc)

    C_true = C_true * 4 

    # test the result
    assert np.allclose(C,C_true,atol=1e-2)
    
    #-------------------------------
    # test on random matrix 
    for _ in range(20):
        n = np.random.randint(3,40)
        p = np.random.randint(3,40)
        X = np.random.random((p,n))
        X = np.asmatrix(X)
        # call the function
        C = compute_C(X)

        # test the result
        assert np.allclose(C.T, C) 
        # test whether C is symmetric 
        assert (C.T == C).all()




#-------------------------------------------------------------------------
def test_compute_eigen_pairs():
    '''(5 points) compute_eigen_pairs'''

    C= np.mat([[ 1.,-1.],
               [-1., 1.]])

    E, v = compute_eigen_pairs(C)
    
    assert type(E) == np.matrixlib.defmatrix.matrix
    assert type(v) == np.ndarray
    E_true1 = np.mat([[ 0.70710678, 0.70710678],
                      [-0.70710678, 0.70710678]])
    E_true2 = np.mat([[-0.70710678, 0.70710678],
                      [ 0.70710678, 0.70710678]])
    assert np.allclose(E,E_true1,atol = 1e-2) or np.allclose(E,E_true2, atol=1e-2) 
    assert np.allclose(v,[2.,0.],atol = 1e-2)



#-------------------------------------------------------------------------
def test_compute_P():
    '''(5 points) compute_P'''
    E= np.mat([[ 0.70710678, 0.70710678],
               [-0.70710678, 0.70710678]])
    v = np.array([2.,0.])
    
    P = compute_P(E,v,k=1) 
    
    assert type(P) == np.matrixlib.defmatrix.matrix
    P_true = np.mat([[ 0.70710678],
                     [-0.70710678]])
    assert np.allclose(P,P_true,atol=1e-2) 

    # test another example
    P = compute_P(E,v,k=2) 
    P_true= np.mat([[ 0.70710678, 0.70710678],
                    [-0.70710678, 0.70710678]])
    assert np.allclose(P,P_true,atol=1e-2) 

    # test another example
    v = np.array([0.,2.])
    P = compute_P(E,v,k=2) 
    P_true= np.mat([[ 0.70710678, 0.70710678],
                    [ 0.70710678,-0.70710678]])
    assert np.allclose(P,P_true,atol=1e-2) 

    # test another example
    v = np.array([0.,2.])
    P = compute_P(E,v,k=1) 
    P_true= np.mat([[ 0.70710678],
                    [ 0.70710678]])
    assert np.allclose(P,P_true,atol=1e-2) 

#-------------------------------------------------------------------------
def test_compute_Xp():
    '''(5 points) compute_Xp'''
    Xc = np.mat([[-1., 1.],
                 [ 1.,-1.],
                 [ 0., 0.]])
    P = np.mat([[ 0.70710678],
                [-0.70710678]])
    
    Xp = compute_Xp(Xc,P) 
    
    assert type(Xp) == np.matrixlib.defmatrix.matrix
    assert Xp.shape == (3,1)
    Xp_true = np.mat([[-1.41421356],
                      [ 1.41421356],
                      [ 0.        ]])

    assert np.allclose(Xp, Xp_true,atol=1e-2)
    
    
    # test another example
    P= np.mat([[ 0.70710678, 0.70710678],
               [-0.70710678, 0.70710678]])

    Xp = compute_Xp(Xc,P) 
    assert Xp.shape == (3,2)
    Xp_true = np.mat([[-1.41421356, 0.],
                      [ 1.41421356, 0.],
                      [ 0.        , 0.]])

    assert np.allclose(Xp, Xp_true,atol=1e-2)
 

#-------------------------------------------------------------------------
def test_PCA():
    '''(10 points) PCA'''

    #-------------------------------
    # an example matrix 
    #X = np.random.random((100,10)) # generate an N = 100, D = 10 random data matrix
    X = np.mat([[0., 2.],
                [2., 0.],
                [1., 1.]])

    # call the function
    Xp, P = PCA(X)
 
    assert type(P) == np.matrixlib.defmatrix.matrix
    assert type(Xp) == np.matrixlib.defmatrix.matrix
    assert Xp.shape ==(3,1)
    assert P.shape ==(2,1)
  

    P_true = np.mat([[ .707],
                     [-.707]])
    Xp_true = np.mat([[-1.414],
                      [ 1.414],
                      [ 0    ]])
 
    # test the result
    assert np.allclose(P,P_true, atol=1e-2) or np.allclose(P,-P_true, atol=1e-2)
    assert np.allclose(Xp, Xp_true, atol=1e-2) or np.allclose(Xp, -Xp_true, atol=1e-2)

    #-------------------------------
    # an example matrix 
    X = np.mat([[0., 2., 2.],
                [2., 0., 0]])

    # call the function
    Xp, P = PCA(X)

    assert Xp.shape ==(2,1)
    assert P.shape ==(3,1)

    # test the result
    P_true = np.mat([[ .577],
                     [-.577],
                     [-.577]])
    Xp_true = np.mat([[-1.73205081],
                      [ 1.73205081]])

    assert np.allclose(P,P_true, atol=1e-2) or np.allclose(P,-P_true, atol=1e-2)
    assert np.allclose(Xp,Xp_true, atol=1e-2) or np.allclose(Xp,-Xp_true, atol=1e-2)

    #-------------------------------
    # an example matrix 
    #X = np.random.random((100,10)) # generate an N = 100, D = 10 random data matrix
    X = np.mat([[2., 2.],
                [0., 0]])


    # call the function
    Xp, P = PCA(X,2)

    assert Xp.shape ==(2,2)
    assert P.shape ==(2,2)

    P_true = np.mat([[.707, -.707],
                     [.707,  .707]])

    Xp_true = np.mat([[ 1.414, 0],
                      [-1.414, 0]])
    assert np.allclose(P,P_true, atol=1e-2) or np.allclose(P,-P_true, atol=1e-2)
    assert np.allclose(Xp,Xp_true, atol=1e-2) or np.allclose(Xp,-Xp_true, atol=1e-2)
    #-------------------------------
    # test on random matrix 
    for _ in range(20):
        n = np.random.randint(3,40)
        p = np.random.randint(3,40)
        k = np.random.randint(p-1)+1
        X = np.random.random((n,p))
        X = np.asmatrix(X)
        # call the function
        Xp, P = PCA(X,k)

        # test the result
        assert Xp.shape == (n,k) 
        assert P.shape == (p,k) 


