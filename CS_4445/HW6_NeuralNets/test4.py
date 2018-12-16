from problem4 import *
import numpy as np
import sys
from sklearn.datasets import make_classification

'''
    Unit test 4:
    This file includes unit tests for problem4.py.
    You could test the correctness of your code by typing `nosetests -v test4.py` in the terminal.
'''
#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 4 (30 points in total)--------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)


#-------------------------------------------------------------------------
def test_compute_z1():
    '''(1 point) compute_z1'''
    x = np.mat('1.; 2.; 3.')
    
    W1 = np.mat([[0.5,-0.6,0.3],
                  [0.6,-0.5,0.2]])
    b1 = np.mat('0.2; 0.3')

    z1 = compute_z1(x,W1,b1)

    assert type(z1) == np.matrixlib.defmatrix.matrix 
    assert z1.shape == (2,1)
    assert np.allclose(z1, np.mat([0.4,0.5]).T, atol = 1e-3) 

    x = np.mat([2., 5.,2.]).T
    z1 = compute_z1(x,W1,b1)

    assert np.allclose(z1.T, [-1.2,-0.6], atol = 1e-3) 


#-------------------------------------------------------------------------
def test_compute_a1():
    '''(2 point) compute_a1'''
    z1 = np.mat([0.,1.]).T
    a1 = compute_a1(z1)
    assert type(a1) == np.matrixlib.defmatrix.matrix 
    assert a1.shape == (2,1)
    assert np.allclose(a1.T, [0.5,0.731], atol = 1e-3) 
    
    z1 = np.mat([-1.,-100., 100]).T
    a1 = compute_a1(z1)
    assert a1.shape == (3,1)
    assert np.allclose(a1.T, [0.2689, 0, 1], atol = 1e-2) 
    
    np.seterr(all='raise')
    z1 = np.mat([1000., 1000.]).T
    a1 = compute_a1(z1)
    assert np.allclose(a1.T, [1., 1.], atol = 1e-2) 
    assert np.allclose(z1.T, [1000, 1000]) 

    z1 = np.mat([-1000., -1000.]).T
    a1 = compute_a1(z1)
    assert np.allclose(a1.T, [0., 0.], atol = 1e-2) 
    assert np.allclose(z1.T, [-1000, -1000]) 
    
    a1 = compute_a1(np.mat([1000., 100.]).T)
    assert np.allclose(a1.T, [1., 1.], atol = 1e-2) 

    a = compute_a1(np.mat([-1000., -10.]).T)
    assert np.allclose(a.T, [0., 0.], atol = 1e-2) 
 
#-------------------------------------------------------------------------
def test_compute_z2():
    '''(1 point) compute_z2'''
    x = np.mat([1., 2., 3.]).T
    
    W2 = np.mat([[0.5,-0.6,0.3],
                  [0.6,-0.5,0.2]])
    b2 = np.mat([0.2, 0.3]).T

    z2 = compute_z2(x,W2,b2)

    assert type(z2) == np.matrixlib.defmatrix.matrix
    assert z2.shape == (2,1)
    assert np.allclose(z2.T, [0.4,0.5], atol = 1e-3) 

    x = np.mat([2., 5.,2.]).T
    z2 = compute_z2(x,W2,b2)

    assert np.allclose(z2.T, [-1.2,-0.6], atol = 1e-3) 

#-------------------------------------------------------------------------
def test_compute_a2():
    '''(2 point) compute_a2'''
    z = np.mat([1., 1.]).T
    a = compute_a2(z)
    assert type(a) == np.matrixlib.defmatrix.matrix
    assert np.allclose(a.T, [0.5, 0.5], atol = 1e-2) 
    assert np.allclose(z.T, [1., 1.]) 


    a = compute_a2(np.mat([1., 1.,1., 1.]).T)
    assert np.allclose(a.T, [0.25, 0.25, 0.25, 0.25], atol = 1e-2) 


    a = compute_a2(np.mat([-1., -1.,-1., -1.]).T)
    assert np.allclose(a.T, [0.25, 0.25, 0.25, 0.25], atol = 1e-2) 


    a = compute_a2(np.mat([-2., -1.,1., 2.]).T)
    assert np.allclose(a.T, [ 0.01275478,0.03467109,0.25618664,0.69638749], atol = 1e-2)

    a = compute_a2(np.mat([100., 100.]).T)
    assert np.allclose(a.T, [0.5, 0.5], atol = 1e-2) 

    a = compute_a2(np.mat([-100., -100.]).T)
    assert np.allclose(a.T, [0.5, 0.5], atol = 1e-2) 
    
    np.seterr(all='raise')
    z = np.mat([1000., 1000.]).T
    a = compute_a2(z)
    assert np.allclose(a.T, [0.5, 0.5], atol = 1e-2) 
    assert np.allclose(z.T, [1000, 1000]) 

    z = np.mat([-1000., -1000.]).T
    a = compute_a2(z)
    assert np.allclose(a.T, [0.5, 0.5], atol = 1e-2) 
    assert np.allclose(z.T, [-1000, -1000]) 
    
    a = compute_a2(np.mat([1000., 10.]).T)
    assert np.allclose(a.T, [1., 0.], atol = 1e-2) 

    a = compute_a2(np.mat([-1000., -10.]).T)
    assert np.allclose(a.T, [0., 1.], atol = 1e-2) 
 

#-------------------------------------------------------------------------
def test_forward():
    '''(1 point) forward'''
    x = np.mat([1., 2.,3.,4]).T

    # first layer with 3 neurons
    W1 = np.mat([[0.,0.,0.,0.],
                 [0.,0.,0.,0.],
                 [0.,0.,0.,0.]])
    b1 = np.mat([0.,0.,0.]).T

    # second layer with 2 neurons
    W2 = np.mat([[0.,0.,0.],
                 [0.,0.,0.]])
    b2 = np.mat([100.,0.]).T

    z1, a1, z2, a2 = forward(x,W1,b1,W2,b2) 
    
    assert type(z1) == np.matrixlib.defmatrix.matrix
    assert type(a1) == np.matrixlib.defmatrix.matrix
    assert z1.shape == (3,1)
    assert a1.shape == (3,1)
    assert type(z2) == np.matrixlib.defmatrix.matrix
    assert type(a2) == np.matrixlib.defmatrix.matrix
    assert z2.shape == (2,1)
    assert a2.shape == (2,1)

    assert np.allclose(z1.T, [0,0,0], atol = 1e-3)
    assert np.allclose(a1.T, [0.5,0.5,0.5], atol = 1e-3)
    assert np.allclose(z2.T, [100,0], atol = 1e-3)
    assert np.allclose(a2.T, [1,0], atol = 1e-3)

#-------------------------------------------------------------------------
def test_compute_dL_da2():
    '''(1 point) compute_dL_da2'''
    a  = np.mat([0.5,0.5]).T
    y = 1
    dL_da = compute_dL_da2(a,y)

    assert type(dL_da) == np.matrixlib.defmatrix.matrix
    assert dL_da.shape == (2,1) 
    assert np.allclose(dL_da.T, [0.,-2.], atol= 1e-3)

    a  = np.mat([0.5,0.5]).T
    y = 0
    dL_da = compute_dL_da2(a,y)
    assert np.allclose(dL_da.T, [-2.,0.], atol= 1e-3)

    a  = np.mat([0.1,0.6,0.1,0.2]).T
    y = 3
    dL_da = compute_dL_da2(a,y)
    assert np.allclose(dL_da.T, [0.,0.,0.,-5.], atol= 1e-3)

    a  = np.mat([1.,0.]).T
    y = 1
    dL_da = compute_dL_da2(a,y)

    np.seterr(all='raise')
    assert np.allclose(dL_da[0], 0., atol= 1e-3)
    assert dL_da[1] < -1e5
    assert dL_da[1] > -float('Inf')
    assert np.allclose(a.T, [1.,0.])
#-------------------------------------------------------------------------
def test_compute_da2_dz2():
    '''(1 point) compute_da2_dz2'''

    a  = np.mat([0.3, 0.7]).T
    da_dz = compute_da2_dz2(a)

    assert type(da_dz) == np.matrixlib.defmatrix.matrix
    assert da_dz.shape == (2,2)
    assert np.allclose(da_dz, [[.21,-.21],[-.21,.21]], atol= 1e-3)

    a  = np.mat([0.1, 0.2, 0.7]).T
    da_dz = compute_da2_dz2(a)
    assert da_dz.shape == (3,3)

    da_dz_true = np.mat( [[ 0.09, -0.02, -0.07],
                         [-0.02,  0.16, -0.14],
                         [-0.07, -0.14,  0.21]])

    assert np.allclose(da_dz,da_dz_true,atol= 1e-3)


#-------------------------------------------------------------------------
def test_compute_dz2_dW2():
    '''(1 point) compute_dz2_dW2'''
    x = np.mat([1., 2.,3.]).T
    dz_dW = compute_dz2_dW2(x,2)

    assert type(dz_dW) == np.matrixlib.defmatrix.matrix
    assert dz_dW.shape == (2,3) 

    dz_dW_true = np.mat([[1., 2.,3],[1., 2.,3]])
    assert np.allclose(dz_dW, dz_dW_true, atol=1e-2) 

#-------------------------------------------------------------------------
def test_compute_dz2_db2():
    '''(1 point) compute_dz2_db2'''
    dz_db = compute_dz2_db2(2)

    assert type(dz_db) == np.matrixlib.defmatrix.matrix
    assert dz_db.shape == (2,1) 

    dz_db_true = np.mat([1.,1.])
    assert np.allclose(dz_db, dz_db_true, atol=1e-2) 



#-------------------------------------------------------------------------
def test_compute_dz2_da1():
    '''(1 point) compute_dz2_da1'''
    W2= np.mat([[1.,
                   .4,3.],
                  [8.,.5,
                  .2]])+.32
    dz2_da1 = compute_dz2_da1(W2)

    assert type(dz2_da1) == np.matrixlib.defmatrix.matrix
    assert dz2_da1.shape == (2,3)
    assert np.allclose(dz2_da1, [[ 1.32, 0.72, 3.32], [ 8.32, 0.82, 0.52]], atol= 1e-3)

#-------------------------------------------------------------------------
def test_compute_da1_dz1():
    '''(1 point) compute_da1_dz1'''
    a1= np.mat([.5,.5,.3,.6]).T
    da1_dz1 = compute_da1_dz1(a1)

    assert type(da1_dz1) == np.matrixlib.defmatrix.matrix
    assert da1_dz1.shape == (4,1)
    assert np.allclose(da1_dz1.T, [.25,.25,.21,.24], atol= 1e-3)

#-------------------------------------------------------------------------
def test_compute_dz1_dW1():
    '''(1 point) compute_dz1_dW1'''
    x = np.mat([1., 2.,3.]).T
    dz_dW = compute_dz1_dW1(x,2)

    assert type(dz_dW) == np.matrixlib.defmatrix.matrix
    assert dz_dW.shape == (2,3) 

    dz_dW_true = np.mat([[1., 2.,3],[1., 2.,3]])
    assert np.allclose(dz_dW, dz_dW_true, atol=1e-2) 


#-------------------------------------------------------------------------
def test_compute_dz1_db1():
    '''(1 point) compute_dz1_db1'''
    dz_db = compute_dz1_db1(2)

    assert type(dz_db) == np.matrixlib.defmatrix.matrix
    assert dz_db.shape == (2,1) 

    dz_db_true = np.mat([1.,1.])
    assert np.allclose(dz_db, dz_db_true, atol=1e-2) 


#-------------------------------------------------------------------------
def test_backward():
    '''(3 point) backward'''
    x = np.mat([1., 2.,3.,4]).T
    y = 1

    # first layer with 3 hidden neurons
    W1 = np.mat([[0.,0.,0.,0.],
                 [0.,0.,0.,0.],
                 [0.,0.,0.,0.]])
    b1 = np.mat([0.,0.,0.]).T

    # second layer with 2 hidden neurons
    W2 = np.mat([[0.,0.,0.],
                 [0.,0.,0.]])
    b2 = np.mat([0.,0.]).T

    z1, a1, z2, a2 = forward(x, W1, b1, W2, b2)

    dL_da2, da2_dz2, dz2_dW2, dz2_db2, dz2_da1, da1_dz1, dz1_dW1, dz1_db1= backward(x,y,a1,a2, W2) 
    
    assert type(dL_da2) == np.matrixlib.defmatrix.matrix
    assert dL_da2.shape == (2,1)
    np.allclose(dL_da2.T,[0.,-2.],atol=1e-3)

    assert type(da2_dz2) == np.matrixlib.defmatrix.matrix
    assert da2_dz2.shape == (2,2)
    np.allclose(da2_dz2,[[.25,-.25],[-.25,.25]],atol=1e-3)

    assert type(dz2_dW2) == np.matrixlib.defmatrix.matrix
    assert dz2_dW2.shape == (2,3)
    np.allclose(dz2_dW2,[[.5,.5,.5],[.5,.5,.5]],atol=1e-3)

    assert type(dz2_db2) == np.matrixlib.defmatrix.matrix
    assert dz2_db2.shape == (2,1)
    np.allclose(dz2_db2.T,[1,1],atol=1e-3)

    assert type(dz2_da1) == np.matrixlib.defmatrix.matrix
    assert dz2_da1.shape == (2,3)
    t = [[ 0., 0., 0.],
         [ 0., 0., 0.]]
    np.allclose(dz2_da1,t,atol=1e-3)

    assert type(da1_dz1) == np.matrixlib.defmatrix.matrix
    assert da1_dz1.shape == (3,1)
    np.allclose(da1_dz1.T,[.25,.25,.25],atol=1e-3)

    assert type(dz1_dW1) == np.matrixlib.defmatrix.matrix
    assert dz1_dW1.shape == (3,4)
    t = [[ 1.,  2.,  3.,  4.],
         [ 1.,  2.,  3.,  4.],
         [ 1.,  2.,  3.,  4.]] 
    np.allclose(dz1_dW1,t,atol=1e-3)

    assert type(dz1_db1) == np.matrixlib.defmatrix.matrix
    assert dz1_db1.shape == (3,1)
    np.allclose(dz1_db1.T,[1,1,1],atol=1e-3)


#-------------------------------------------------------------------------
def test_compute_dL_da1():
    '''(2 point) compute_dL_da1'''
    dL_dz2 = np.mat([ 0.09554921, 0.14753129, 0.47769828,-0.72077878]).T
    dz2_da1 = np.mat([[ 0.26739761, 0.73446399, 0.24513834],
                        [ 0.80682023, 0.7841972 , 0.01415917],
                        [ 0.70592854, 0.73489433, 0.91355454],
                        [ 0.8558265 , 0.84993468, 0.24702029]]) 

    dL_da1 = compute_dL_da1(dL_dz2,dz2_da1)

    assert type(dL_da1) == np.matrixlib.defmatrix.matrix
    assert dL_da1.shape == (3,1) 

    dL_da1_true = np.mat([-0.13505987,-0.07568605, 0.28386814]).T
    assert np.allclose(dL_da1, dL_da1_true, atol=1e-3) 


#-------------------------------------------------------------------------
def test_compute_dL_dz1():
    '''(1 point) compute_dL_dz1'''
    dL_da1  = np.mat([-0.03777044, 0.29040313,-0.42821076,-0.28597724 ]).T
    da1_dz1 = np.mat([ 0.03766515, 0.09406613, 0.06316817, 0.05718137]).T

    dL_dz1 = compute_dL_dz1(dL_da1, da1_dz1)

    assert type(dL_dz1) == np.matrixlib.defmatrix.matrix
    assert dL_dz1.shape == (4,1) 

    dL_dz1_true = np.mat([-0.00142263, 0.0273171,  -0.02704929,-0.01635257]).T
    assert np.allclose(dL_dz1, dL_dz1_true, atol=1e-3) 



##-------------------------------------------------------------------------
def test_compute_gradients():
    '''(3 point) compute_gradients'''
    x = np.mat([1., 2.,3.,4]).T
    y = 1

    # first layer with 3 hidden neurons
    W1 = np.mat([[0.,0.,0.,0.],
                 [0.,0.,0.,0.],
                 [0.,0.,0.,0.]])
    b1 = np.mat([0.,0.,0.]).T

    # second layer with 2 hidden neurons
    W2 = np.mat([[0.,0.,0.],
                 [0.,0.,0.]])
    b2 = np.mat([0.,0.]).T

    # forward pass
    z1, a1, z2, a2 = forward(x, W1, b1, W2, b2)

    # backward pass: prepare local gradients
    dL_da2, da2_dz2, dz2_dW2, dz2_db2, dz2_da1, da1_dz1, dz1_dW1, dz1_db1= backward(x,y,a1,a2, W2) 
    # call the function 
    dL_dW2, dL_db2, dL_dW1, dL_db1 = compute_gradients(dL_da2, da2_dz2, dz2_dW2, dz2_db2, dz2_da1, da1_dz1, dz1_dW1, dz1_db1)
    
    assert type(dL_dW2) == np.matrixlib.defmatrix.matrix
    assert dL_dW2.shape == (2,3)
    t = [[ 0.25, 0.25, 0.25],
         [-0.25,-0.25,-0.25]]
    np.allclose(dL_dW2,t,atol=1e-3)
 
    assert type(dL_db2) == np.matrixlib.defmatrix.matrix
    assert dL_db2.shape == (2,1)
    t = [0.5,-0.5]
    np.allclose(dL_db2.T,t,atol=1e-3)

    assert type(dL_dW1) == np.matrixlib.defmatrix.matrix
    assert dL_dW1.shape == (3,4)
    t = np.zeros((3,4)) 
    np.allclose(dL_dW1,t,atol=1e-3)

    assert type(dL_db1) == np.matrixlib.defmatrix.matrix
    assert dL_db1.shape == (3,1)
    t = [0,0,0]
    np.allclose(dL_db1.T,t,atol=1e-3)


##-------------------------------------------------------------------------
def test_check_compute_gradients():
    '''(2 point) check gradients'''
    for _ in range(20):
        p = np.random.randint(2,10) # number of features
        c = np.random.randint(2,10) # number of classes
        h = np.random.randint(2,10) # number of neurons in the 1st layer 
        x = np.asmatrix(10*np.random.random((p,1))-5)
        y = np.random.randint(c) 
        W1 = np.asmatrix(2*np.random.random((h,p))-1)
        b1 = np.asmatrix(np.random.random((h,1)))
        W2 = np.asmatrix(2*np.random.random((c,h))-1)
        b2 = np.asmatrix(np.random.random((c,1)))
        z1, a1, z2, a2 = forward(x, W1, b1, W2, b2)
        dL_da2, da2_dz2, dz2_dW2, dz2_db2, dz2_da1, da1_dz1, dz1_dW1, dz1_db1= backward(x,y,a1,a2, W2) 

        # analytical gradients
        dL_dW2, dL_db2, dL_dW1, dL_db1 = compute_gradients(dL_da2, da2_dz2, dz2_dW2, dz2_db2, dz2_da1, da1_dz1, dz1_dW1, dz1_db1)
        # numerical gradients
        dL_dW2_true = check_dL_dW2(x,y, W1,b1,W2,b2)
        assert np.allclose(dL_dW2, dL_dW2_true, atol=1e-4) 

        dL_dW1_true = check_dL_dW1(x,y, W1,b1,W2,b2)
        assert np.allclose(dL_dW1, dL_dW1_true, atol=1e-4) 

#-------------------------------------------------------------------------
def test_fully_connected():
    '''(4 point) train and predict'''
    # create a multi-class classification dataset
    n_samples = 400
    X,y = make_classification(n_samples= n_samples,
                              n_features=5, n_redundant=0, n_informative=4,
                              n_classes= 3,
                              class_sep = 5.,
                              random_state=1)
        
    X = np.asmatrix(X)
    Xtrain, Ytrain, Xtest, Ytest = X[::2], y[::2], X[1::2], y[1::2]
    W1,b1,W2,b2 = train(Xtrain, Ytrain,alpha=.01, n_epoch=100)
    Y, P = predict(Xtrain, W1, b1, W2, b2)
    accuracy = sum(Y == Ytrain)/(n_samples/2.)
    print('Training accuracy:', accuracy)
    assert accuracy > 0.9
    Y, P = predict(Xtest, W1, b1, W2, b2)
    accuracy = sum(Y == Ytest)/(n_samples/2.)
    print('Test accuracy:', accuracy)
    assert accuracy > 0.9

