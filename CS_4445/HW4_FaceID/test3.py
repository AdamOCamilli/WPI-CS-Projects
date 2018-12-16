from problem2 import load_dataset 
from problem3 import * 
import sys
import numpy as np
import scipy.misc

'''
    Unit test 3:
    This file includes unit tests for problem3.py.
    You could test the correctness of your code by typing `nosetests test3.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 3 (20 points in total)---------------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)


#-------------------------------------------------------------------------
def test_compute_distance():
    '''(10 points) compute_distance'''
    #-------------------------------
    # an example matrix (two features, 3 instances)
    X = np.array([[0., 1.],
                  [1., 0.],
                  [0., 0.]])
    q = np.array([1., 1.])
    # call the function
    s = compute_distance(X,q)
    
    assert type(s) == np.ndarray
    assert s.shape == (3,)
    assert s.dtype == float

    assert np.allclose(s, [1,1,1.41421356])




#-------------------------------------------------------------------------
def test_face_recognition():
    '''(10 points) face_recognition'''
    #-------------------------------
    # an example matrix (two features, 3 instances)
    X = np.array([[0., 1.],
                  [1., .5],
                  [0., 0.]])
    q = np.array([1., 1.])
    # call the function
    ids = face_recognition(X,q)
    
    assert type(ids) == np.ndarray
    assert ids.shape == (3,)
    assert ids.dtype == int 

    assert ids.tolist() == [1,0,2]

    #-------------------------------
    qid = 7*10  # use the first image as the query image

    Xp = np.load('face_pca.npy')
    q = Xp[qid,:]

    # call the function
    ids =face_recognition(Xp,q)
    
    assert type(ids) == np.ndarray
    assert ids.shape == (400,)
    assert ids.dtype == int 

    # the most similar image should be the query image itself
    assert ids[0] == qid 
    X, l, images = load_dataset()
    scipy.misc.imsave('query.jpg', images[qid]) 

    # save the top 10 similar images to the query
    for i in range(10):
        x = images[ids[i]]
        scipy.misc.imsave('result_%d.jpg' % (i+1), x) 
    
    assert ids[:4].tolist() ==[70, 71, 75, 74]
