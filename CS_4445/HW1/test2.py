from problem2 import *
import numpy as np
import sys

'''
    Unit test 2:
    This file includes unit tests for problem2.py.
    You could test the correctness of your code by typing `nosetests -v test2.py` in the terminal.
'''
#-------------------------------------------------------------------------
def test_python_version():
    ''' ---------- Problem 2 (10 points in total) ------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)



#-------------------------------------------------------------------------
def test_matrix_vector_multiplication():
    '''(10 points) matrix_vector_multiplication'''

    # create a matrix  [[1., 2.],
    #                   [3., 4.],
    #                   [5., 6.]]
    # of shape (3 by 2)
    X = np.mat([[1.,2.],[3.,4.],[5.,6.]])
    print('X:', X)

    # create a vector of shape (2 by 1): [[1.],
    #                                     [2.]]
    y = np.mat('1.; 2.')

    # call the function
    z= matrix_vector_multiplication(X,y) 

    # test whether or not z is a numpy matrix 
    assert type(z) == np.matrixlib.defmatrix.matrix

    # test the shape of the vector
    assert z.shape == (3,1)

    # check the correctness of the result 
    z_real = np.mat('5.; 11.; 17')
    assert np.allclose(z, z_real)


    # test on random matrix
    for _ in range(20):
        m,n = np.random.randint(2,20,size= 2) 
        X = np.asmatrix(np.random.random((m,n)))
        y = np.asmatrix(np.random.random(n)).T
        z = matrix_vector_multiplication(X,y)
        i = np.random.randint(m)
        assert np.allclose(z[i],  X[i]*y)

