from problem1 import *
import sys
import numpy as np

'''
    Unit test 1:
    This file includes unit tests for problem1.py.
    You could test the correctness of your code by typing `nosetests test1.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 1 (20 points in total)--------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)


#-------------------------------------------------------------------------
def test_batting_average():
    '''(2 points) batting_average'''

    BA = batting_average(3, 17)

    # test whether or not the result is a float number 
    assert type(BA) == float 

    # check the correctness of the result 
    assert np.allclose(BA, .1765, atol = 1e-3)

    BA = batting_average(672, 9079)
    assert np.allclose(BA, .074, atol = 1e-3)


#-------------------------------------------------------------------------
def test_on_base_percentage():
    '''(3 points) on_base_percentage'''

    OBP = on_base_percentage(3, 37, 29, 5, 2)

    # test whether or not the result is a float number 
    assert type(OBP) == float 

    # check the correctness of the result 
    assert np.allclose(OBP, .507, atol = 1e-3)


    OBP = on_base_percentage(2, 35, 28, 6, 1)
    assert np.allclose(OBP, .514, atol = 1e-3)


#-------------------------------------------------------------------------
def test_slugging_percentage():
    '''(3 points) slugging_percentage'''

    SLG = slugging_percentage(11, 12, 13, 14, 123)

    # test whether or not the result is a float number 
    assert type(SLG) == float 

    # check the correctness of the result 
    assert np.allclose(SLG, .7398, atol = 1e-3)


    SLG = slugging_percentage(12, 13, 14,15, 234)
    assert np.allclose(SLG, .4188, atol = 1e-3)



#-------------------------------------------------------------------------
def test_runs_created():
    '''(3 points) runs_created'''

    RC = runs_created(11, 12, 13, 14, 15, 123)

    # test whether or not the result is a float number 
    assert type(RC) == float 

    # check the correctness of the result 
    assert np.allclose(RC, 17.145, atol = 1e-2)


    RC = runs_created(12, 13, 14,15,16, 234)
    assert np.allclose(RC, 10.976, atol = 1e-3)


#-------------------------------------------------------------------------
def test_win_ratio():
    '''(2 points) win_ratio'''

    WR = win_ratio(884,645) # this is the goal of OAK team in year 2002.

    # test whether or not the result is a float number 
    assert type(WR) == float 

    # check the correctness of the result 
    assert np.allclose(WR, .6526, atol = 1e-3)

#-------------------------------------------------------------------------
def test_eval_player():
    '''(5 points) eval_player'''

    # ------- the new players hired in 2002 ---------
    # Scott Hatteberg
    BA, OBP, SLG  = eval_player('hattesc01') 
    assert np.allclose(BA, .2445, atol = 1e-3)
    assert np.allclose(OBP, .332278, atol = 1e-3)
    assert np.allclose(SLG, .34532, atol = 1e-3)


    # David Justice 
    BA, OBP, SLG  = eval_player('justida01') 
    assert np.allclose(BA, .2414, atol = 1e-3)
    assert np.allclose(OBP, .33257, atol = 1e-4)
    assert np.allclose(SLG, .43045, atol = 1e-3)


    # Jeremy Giambi 
    BA, OBP, SLG  = eval_player('giambje01') 
    assert np.allclose(BA, .283, atol = 1e-3)
    assert np.allclose(OBP, .3909, atol = 1e-4)
    assert np.allclose(SLG, .4501, atol = 1e-3)



    # ------- the players lost in 2002 ---------

    # Jason Giambi 
    BA, OBP, SLG  = eval_player('giambja01') 
    assert np.allclose(BA, .3423, atol = 1e-3)
    assert np.allclose(OBP, .4769, atol = 1e-4)
    assert np.allclose(SLG, .6596, atol = 1e-3)

    # Johnny Damon 
    BA, OBP, SLG  = eval_player('damonjo01') 
    assert np.allclose(BA, .256, atol = 1e-3)
    assert np.allclose(OBP, .3235, atol = 1e-3)
    assert np.allclose(SLG, .3633, atol = 1e-3)

    # Jason Isringhausen (no data from year 2001) 






#-------------------------------------------------------------------------
def test_salary():
    '''(2 points) salary'''

    # ------- the new players hired in 2002 ---------
    # Scott Hatteberg
    S = salary('hattesc01') 
    assert np.allclose(S,  900000, atol = 1.)

    # David Justice 
    S = salary('justida01') 
    assert np.allclose(S, 7000000, atol = 1.) # note: OAK is only paid half of his salary in 2002, Boston Red Sox paid the other half.

    # Jeremy Giambi
    S = salary('giambje01') 
    assert np.allclose(S, 1065000, atol = 1.) 



    # ------- the players lost in 2002 ---------

    # Jason Giambi 
    S = salary('giambja01') 
    assert np.allclose(S,10428571, atol = 1.)

    # Johnny Damon 
    S = salary('damonjo01') 
    assert np.allclose(S, 7250000, atol = 1.)

    # Jason Isringhausen (no data from year 2001) 
    S = salary('isrinja01') 
    assert np.allclose(S, 2750000, atol = 1.)



