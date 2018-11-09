from problem6 import *
import sys
'''
    Unit test 6: 
    This file includes unit tests for problem6.py. 
    You could test the correctness of your code by typing `nosetests test6.py` in the terminal.
'''
#-------------------------------------------------------------------------
def test_python_version():
    ''' ---------- Problem 6 (20 points in total) ------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)



#-------------------------------------------------------------------------
def test_import_A():
    '''(10 points) import_A '''

    # call the function
    A = import_A(filename = 'A.csv') 

    # test whether or not A is a numpy array
    assert type(A) == np.matrixlib.defmatrix.matrix

    # test the shape of the vector
    assert A.shape == (3, 3)

    # call the function
    A = import_A() 

    # test whether or not A is a numpy array
    assert type(A) == np.matrixlib.defmatrix.matrix

    # test the shape of the vector
    assert A.shape == (1181, 1181)
    
    assert A[3,0] == 1. 
    assert A[2,1] == 1. 
    assert A[1,1] == 0. 
    assert A[-9,-1] == 1. 
    assert A[-1,-1] == 0. 

#-------------------------------------------------------------------------
def test_score2rank():
    '''(5 points) score2rank'''
    # a numpy array of pagerank scores 
    x = np.mat( [[ 2. ],
                 [ 0.5],
                 [ 1. ]] )
    
    # call the function
    sorted_ids = score2rank(x) 

    # test whether or not the sorted_ids is a python array
    assert type(sorted_ids) == list

    # test the result 
    assert sorted_ids == [0,2,1]


#-------------------------------------------------------------------------
def test_node_ranking():
    '''(5 points) node_ranking'''

    # call the function
    sorted_ids = node_ranking(filename='A.csv',alpha=1) 

    # test whether or not the sorted_ids is a python array
    assert type(sorted_ids) == list

    # test the shape of the vector
    assert len(sorted_ids) == 3

    # test the result 
    assert sorted_ids ==[0,2,1]

    #-----------------------------
    # test a larger network

    # call the function
    sorted_ids = node_ranking() 

    # test whether or not the sorted_ids is a python array
    assert type(sorted_ids) == list


    # test the shape of the vector
    assert len(sorted_ids) == 1181

    # test the result 
    assert sorted_ids[0]== 461
    assert sorted_ids[1]== 210
    assert sorted_ids[2]== 811 
