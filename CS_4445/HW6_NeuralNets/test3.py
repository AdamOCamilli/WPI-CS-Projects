from problem3 import *
import numpy as np
import sys
from sklearn.datasets import make_classification

'''
    Unit test 3:
    This file includes unit tests for problem3.py.
    You could test the correctness of your code by typing `nosetests -v test3.py` in the terminal.
'''
#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 3 (30 points in total)--------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)


#-------------------------------------------------------------------------
def test_compute_z():
    '''(1 point) compute_z'''
    x = np.mat('1.; 2.; 3.')
    
    W = np.mat([[0.5,-0.6,0.3],
                [0.6,-0.5,0.2]])
    b = np.mat('0.2; 0.3')

    z = compute_z(x,W,b)

    assert type(z) == np.matrixlib.defmatrix.matrix
    assert z.shape == (2,1)
    assert np.allclose(z, np.mat('0.4;0.5'), atol = 1e-3) 

    x = np.mat('2.; 5.;2.')
    z = compute_z(x,W,b)

    assert np.allclose(z, np.mat('-1.2;-0.6'), atol = 1e-3) 

#-------------------------------------------------------------------------
def test_compute_a():
    '''(2 point) compute_a'''
    z = np.mat('1.; 1.')
    a = compute_a(z)
    assert type(a) == np.matrixlib.defmatrix.matrix
    assert np.allclose(a, np.mat('0.5; 0.5'), atol = 1e-2) 
    assert np.allclose(z, np.mat('1.; 1.')) 


    a = compute_a(np.mat('1.; 1.; 1.; 1.'))
    assert np.allclose(a, np.mat('0.25; 0.25; 0.25; 0.25'), atol = 1e-2) 


    a = compute_a(np.mat('-1.; -1.; -1.; -1.'))
    assert np.allclose(a, np.mat('0.25; 0.25; 0.25; 0.25'), atol = 1e-2) 


    a = compute_a(np.mat('-2.; -1.;1.; 2.'))
    assert np.allclose(a, np.mat('0.01275478;0.03467109;0.25618664;0.69638749'), atol = 1e-2)

    a = compute_a(np.mat('100.; 100.'))
    assert np.allclose(a, np.mat('0.5; 0.5'), atol = 1e-2) 

    a = compute_a(np.mat('-100.; -100.'))
    assert np.allclose(a, np.mat('0.5; 0.5'), atol = 1e-2) 
    
    np.seterr(all='raise')
    z = np.mat('1000.; 1000.')
    a = compute_a(z)
    assert np.allclose(a, np.mat('0.5; 0.5'), atol = 1e-2) 
    assert np.allclose(z, np.mat('1000; 1000')) 

    z = np.mat('-1000.; -1000.')
    a = compute_a(z)
    assert np.allclose(a, np.mat('0.5; 0.5'), atol = 1e-2) 
    assert np.allclose(z, np.mat('-1000; -1000')) 
    
    a = compute_a(np.mat('1000.; 10.'))
    assert np.allclose(a, np.mat('1.; 0.'), atol = 1e-2) 

    a = compute_a(np.mat('-1000.; -10.'))
    assert np.allclose(a, np.mat('0.; 1.'), atol = 1e-2) 

    z = np.mat('1000.; 3000.')
    a = compute_a(z)
    assert np.allclose(a, np.mat('0.; 1.'), atol = 1e-2) 
    assert np.allclose(z, np.mat('1000; 3000')) 
 
    
#-------------------------------------------------------------------------
def test_compute_L():
    '''(2 point) compute_L'''
    a = np.mat('0.;1.') 
    L = compute_L(a,1)

    assert type(L) == float
    assert np.allclose(L, 0., atol = 1e-3) 
    assert np.allclose(a, np.mat('0.;1.')) 

    a = np.mat('0.; 0.; 1.')
    L = compute_L(a, 2)
    assert np.allclose(L, 0., atol = 1e-3) 

    a = np.mat('0.;1.') 
    L= compute_L(a, 1)
    assert np.allclose(L, 0., atol = 1e-3) 

    a = np.mat('.5;.5') 
    L= compute_L(a, 0)
    assert np.allclose(L, 0.69314718056, atol = 1e-3) 

    L= compute_L(a, 1)
    assert np.allclose(L, 0.69314718056, atol = 1e-3) 

    np.seterr(all='raise')
    a = np.mat('1.;0.') 
    L= compute_L(a, 1)
    assert L > 1e5

    a = np.mat('0.;1.') 
    L= compute_L(a, 0)
    assert L > 1e5

#-------------------------------------------------------------------------
def test_forward():
    '''(1 point) forward'''
    x = np.mat('1.; 2.;3.')
    W = np.mat('0., 0.,0.; 0.,0.,0.')
    b = np.mat('0.;0.')
    y = 1

    z, a, L= forward(x,y,W,b) 
    
    assert type(z) == np.matrixlib.defmatrix.matrix
    assert type(a) == np.matrixlib.defmatrix.matrix
    assert z.shape == (2,1) 
    assert a.shape == (2,1) 

    z_true = np.mat('0.;0.')
    a_true = np.mat('0.5;0.5')
    L_true = 0.69314718056  
    assert np.allclose(z, z_true, atol = 1e-3)
    assert np.allclose(a, a_true, atol = 1e-3)
    assert np.allclose(L, L_true, atol = 1e-3)

#-------------------------------------------------------------------------
def test_compute_dL_da():
    '''(1 point) compute_dL_da'''
    a  = np.mat('0.5;0.5')
    y = 1
    dL_da = compute_dL_da(a,y)

    assert type(dL_da) == np.matrixlib.defmatrix.matrix
    assert dL_da.shape == (2,1) 
    assert np.allclose(dL_da, np.mat('0.;-2.'), atol= 1e-3)

    a  = np.mat('0.5;0.5')
    y = 0
    dL_da = compute_dL_da(a,y)
    assert np.allclose(dL_da, np.mat('-2.;0.'), atol= 1e-3)

    a  = np.mat('0.1;0.6;0.1;0.2')
    y = 3
    dL_da = compute_dL_da(a,y)
    assert np.allclose(dL_da, np.mat('0.;0.;0.;-5.'), atol= 1e-3)

    a  = np.mat('1.;0.')
    y = 1
    dL_da = compute_dL_da(a,y)

    np.seterr(all='raise')
    assert np.allclose(dL_da[0], 0., atol= 1e-3)
    assert dL_da[1] < -1e5
    assert dL_da[1] > -float('Inf')
    assert np.allclose(a, np.mat('1.;0.'))


#-------------------------------------------------------------------------
def test_check_dL_da():
    '''(1 point) check dL_da'''
    for _ in range(20):
        c = np.random.randint(2,10) # number of classes
        a  = np.asmatrix(np.random.random((c,1))) # activation
        a = (a + 1)/(a+1).sum()
        y = np.random.randint(c) # label 
        # analytical gradients
        da = compute_dL_da(a,y)
        # numerical gradients
        da_true = check_dL_da(a,y)
        assert np.allclose(da, da_true, atol= 1e-2)


#-------------------------------------------------------------------------
def test_compute_da_dz():
    '''(1 point) compute_da_dz'''
    a  = np.mat('0.3; 0.7')
    da_dz = compute_da_dz(a)

    assert type(da_dz) == np.matrixlib.defmatrix.matrix
    assert da_dz.shape == (2,2)
    assert np.allclose(da_dz, [[.21,-.21],[-.21,.21]], atol= 1e-3)

    a  = np.mat('0.1; 0.2; 0.7')
    da_dz = compute_da_dz(a)
    assert da_dz.shape == (3,3)

    da_dz_true = np.mat( [[ 0.09, -0.02, -0.07],
                         [-0.02,  0.16, -0.14],
                         [-0.07, -0.14,  0.21]])

    assert np.allclose(da_dz,da_dz_true,atol= 1e-3)


#-------------------------------------------------------------------------
def test_check_da_dz():
    '''(1 point) check da_dz'''
    for _ in range(20):
        c = np.random.randint(2,10)
        z = np.asmatrix(np.random.random((c,1)))
        a  = compute_a(z)
        # analytical gradients
        dz = compute_da_dz(a)
        # numerical gradients
        dz_true = check_da_dz(z)
        assert np.allclose(dz, dz_true, atol= 1e-3)



#-------------------------------------------------------------------------
def test_check_dz_dW():
    '''(2 points) check dz_dW'''
    for _ in range(20):
        c = np.random.randint(2,10)
        p = np.random.randint(2,20)
        x = np.asmatrix(np.random.random((p,1)))
        W = np.asmatrix(np.random.random((c,p)))
        b = np.random.random(c)
        # analytical gradients
        dz_dW = compute_dz_dW(x,c)
        # numerical gradients
        dz_dW_true = check_dz_dW(x,W,b)

        assert np.allclose(dz_dW, dz_dW_true, atol=1e-3) 

#-------------------------------------------------------------------------
def test_check_dz_db():
    '''(2 points) check dz_db'''
    for _ in range(20):
        c = np.random.randint(2,10)
        p = np.random.randint(2,20)
        x = np.asmatrix(np.random.random((p,1)))
        W = np.asmatrix(np.random.random((c,p)))
        b = np.asmatrix(np.random.random((c,1)))
        #b = np.random.random(c)
        # analytical gradients
        dz_db = compute_dz_db(c)
        # numerical gradients
        dz_db_true = check_dz_db(x,W,b)

        assert np.allclose(dz_db, dz_db_true, atol=1e-3) 

#-------------------------------------------------------------------------
def test_backward():
    '''(2 points) backward'''
    x = np.mat('1.; 2.;3.')
    y = 1
    a = np.mat('.5; .5')

    da, dz, dW, db = backward(x,y,a) 
    
    assert type(da) == np.matrixlib.defmatrix.matrix
    assert type(dz) == np.matrixlib.defmatrix.matrix
    assert type(dW) == np.matrixlib.defmatrix.matrix
    assert type(db) == np.matrixlib.defmatrix.matrix
    assert da.shape == (2,1) 
    assert dz.shape == (2,2) 
    assert dW.shape == (2,3) 
    assert db.shape == (2,1) 

    da_true = np.mat('0.;-2.')
    dz_true = [[0.25, -0.25],[-0.25,0.25]]
    dW_true = [[ 1., 2., 3.], [ 1., 2., 3.]] 
    db_true = np.mat('1.;1.')

    assert np.allclose(da, da_true, atol = 1e-3)
    assert np.allclose(dz, dz_true, atol = 1e-3)
    assert np.allclose(dW, dW_true, atol = 1e-3)
    assert np.allclose(db, db_true, atol = 1e-3)
    

#-------------------------------------------------------------------------
def test_compute_dL_dz():
    '''(2 points) compute_dL_dz'''
    dL_da = np.mat('0.;-2.')
    da_dz = np.mat([[0.25, -0.25],[-0.25,0.25]])
    dz_dW = np.mat([[ 1., 2., 3.], [ 1., 2., 3.]])

    dL_dz = compute_dL_dz(dL_da, da_dz) 
    
    assert type(dL_dz) == np.matrixlib.defmatrix.matrix
    assert dL_dz.shape == (2,1) 

    dL_dz_true = np.mat('0.5; -0.5')
    assert np.allclose(dL_dz, dL_dz_true, atol = 1e-3)


#-------------------------------------------------------------------------
def test_compute_dL_dW():
    '''(2 points) compute_dL_dW'''
    dL_dz = np.mat('0.5; -0.5')
    dz_dW = np.mat([[ 1., 2., 3.], [ 1., 2., 3.]] )

    dL_dW = compute_dL_dW(dL_dz, dz_dW) 
    
    assert type(dL_dW) == np.matrixlib.defmatrix.matrix
    assert dL_dW.shape == (2,3) 

    dL_dW_true = [[ 0.5,  1.0,  1.5],
               [-0.5, -1.0, -1.5]]
    assert np.allclose(dL_dW, dL_dW_true, atol = 1e-3)

#-------------------------------------------------------------------------
def test_check_dL_dW():
    '''(2 points) check dL_dW'''
    for _ in range(20):
        p = np.random.randint(2,10) # number of features
        c = np.random.randint(2,10) # number of classes
        x = np.asmatrix(np.random.random((p,1)))
        y = np.random.randint(c) 
        W = np.asmatrix(np.random.random((c,p)))
        b = np.asmatrix(np.random.random((c,1)))
        z, a, L = forward(x,y,W,b)
        dL_da, da_dz, dz_dW, dz_db = backward(x,y,a)
        dL_dz = compute_dL_dz(dL_da, da_dz)

        # analytical gradients
        dL_dW = compute_dL_dW(dL_dz,dz_dW)
        # numerical gradients
        dL_dW_true = check_dL_dW(x,y,W,b) 
        
        assert np.allclose(dL_dW, dL_dW_true, atol = 1e-3)


#-------------------------------------------------------------------------
def test_check_dL_db():
    '''(1 point) check dL_db'''
    for _ in range(20):
        p = np.random.randint(2,10) # number of features
        c = np.random.randint(2,10) # number of classes
        x = np.asmatrix(np.random.random((p,1)))
        y = np.random.randint(c) 
        W = np.asmatrix(np.random.random((c,p)))
        b = np.asmatrix(np.random.random((c,1)))
        z, a, L = forward(x,y,W,b)
        dL_da, da_dz, dz_dW, dz_db = backward(x,y,a)
        dL_dz = compute_dL_dz(dL_da, da_dz)

        # analytical gradients
        dL_db = compute_dL_db(dL_dz,dz_db)
        # numerical gradients
        dL_db_true = check_dL_db(x,y,W,b)
        print("true",dL_db_true)
        
        assert np.allclose(dL_db, dL_db_true, atol = 1e-3)


#-------------------------------------------------------------------------
def test_update_W():
    '''(1 point) update_W'''

    W = np.mat([[0., 0.,0.],[0.,0.,0.]])
    dL_dW = np.mat([[ 0.5,  1.0,  1.5],
                    [-0.5, -1.0, -1.5]])

    W = update_W(W, dL_dW, alpha=1.) 

    W_true = [[-0.5,-1.,-1.5],
               [ 0.5, 1., 1.5]]
    assert np.allclose(W, W_true, atol = 1e-3)


    W = np.mat([[0., 0.,0.],[0.,0.,0.]])
    W = update_W(W, dL_dW, alpha=10.)
    W_true = [[-5., -10., -15.],
               [ 5.,  10.,  15.]]
    assert np.allclose(W, W_true, atol = 1e-3)


#-------------------------------------------------------------------------
def test_update_b():
    '''(1 point) update_b'''
    b = np.mat('0.;0.')
    dL_db = np.mat('0.5; -0.5')

    b = update_b(b, dL_db, alpha=1.)

    b_true = np.mat('-0.5;0.5')
    assert np.allclose(b, b_true, atol = 1e-3)

    b = np.mat('0.;0.')
    b = update_b(b, dL_db, alpha=10.)
    b_true = np.mat('-5.;5.')
    assert np.allclose(b, b_true, atol = 1e-3)


#-------------------------------------------------------------------------
def test_train():
    '''(2 point) train'''
    # an example feature matrix (4 instances, 2 features)
    Xtrain  = np.mat( [[0., 1.],
                       [1., 0.],
                       [0., 0.],
                       [1., 1.]])
    Ytrain = [0, 1, 0, 1]

    # call the function
    W, b = train(Xtrain, Ytrain)
   
    assert b[0] > b[1] # x3 is negative 
    assert W[1,0] + W[1,1] + b[1] > W[0,0] + W[0,1] + b[0] # x4 is positive
    assert W[1,1] + b[1] < W[0,1] + b[0] # x1 is negative 
    assert W[1,0] + b[1] > W[0,0] + b[0] # x2 is positive 

  
#-------------------------------------------------------------------------
def test_predict():
    '''(1 points) test'''

    # an example feature matrix (4 instances, 2 features)
    Xtest  = np.mat( [ [0., 1.],
                       [1., 0.],
                       [0., 0.],
                       [1., 1.]])
    
    W = np.mat([[0.4, 0.],
                [0.6, 0.]])
    b = np.mat('0.1; 0.')

    # call the function
    Ytest, Ptest = predict(Xtest, W, b )

    assert type(Ytest) == np.ndarray
    assert Ytest.shape == (4,)
    assert type(Ptest) == np.matrixlib.defmatrix.matrix
    assert Ptest.shape == (4,2)

    Ytest_true = [0, 1,0, 1]
    Ptest_true = [[ 0.52497919, 0.47502081],
                  [ 0.47502081, 0.52497919],
                  [ 0.52497919, 0.47502081],
                  [ 0.47502081, 0.52497919]] 

    # check the correctness of the result 
    assert np.allclose(Ytest, Ytest_true, atol = 1e-2)
    assert np.allclose(Ptest, Ptest_true, atol = 1e-2)


#-------------------------------------------------------------------------
def test_softmax_regression():
    '''(2 point) softmax regression'''

    # create a multi-class classification dataset
    n_samples = 400
    X,y = make_classification(n_samples= n_samples,
                              n_features=5, n_redundant=0, n_informative=4,
                              n_classes= 3,
                              class_sep = 3.,
                              random_state=1)
        
    X = np.asmatrix(X)
    Xtrain, Ytrain, Xtest, Ytest = X[::2], y[::2], X[1::2], y[1::2]
    w,b = train(Xtrain, Ytrain,alpha=.01, n_epoch=100)
    Y, P = predict(Xtrain, w, b)
    accuracy = sum(Y == Ytrain)/(n_samples/2.)
    print('Training accuracy:', accuracy)
    assert accuracy > 0.9
    Y, P = predict(Xtest, w, b)
    accuracy = sum(Y == Ytest)/(n_samples/2.)
    print('Test accuracy:', accuracy)
    assert accuracy > 0.9

