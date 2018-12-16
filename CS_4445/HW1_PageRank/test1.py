from problem1 import *
import sys

'''
    Unit test 1:
    This file includes unit tests for problem1.py.
    You could test the correctness of your code by typing `nosetests -v test1.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ---------- Problem 1 (10 points in total) ------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)

#-------------------------------------------------------------------------
def test_swap():
    '''(5 points) swap()'''
    A = [1,2,3] # create a list example
    swap(A,0,2) # call the swap function

    # test whether or not A is a python array
    assert type(A) == list

    # check whether or not the two elements in the list are switched
    assert A[0]== 3 
    assert A[2]== 1
    assert A[1]== 2


def test_bubblesort():
    '''(5 points) bubblesort()'''
    A = [8,5,3,1,9,6,0,7,4,2,5] # create a list example
    bubblesort(A) # call the function

    # test whether or not A is a python array
    assert type(A) == list

    # check whether or not the list is sorted 
    assert A == [0,1,2,3,4,5,5,6,7,8,9]

    A = [8,5,3,1,9,6,5,7,4,2,0] # create a list example
    bubblesort(A) # call the function

    # test whether or not A is a python array
    assert type(A) == list

    # check whether or not the list is sorted 
    assert A == [0,1,2,3,4,5,5,6,7,8,9]
    
