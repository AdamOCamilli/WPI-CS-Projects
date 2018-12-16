from problem2 import * 
import numpy as np
import sys
#import scipy.misc
import imageio

'''
    Unit test 2:
    This file includes unit tests for problem2.py.
    You could test the correctness of your code by typing `nosetests -v test2.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 2 (25 points in total)---------------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)

#-------------------------------------------------------------------------
def test_reshape_to_image():
    ''' (3 points) reshap_to_image'''
    x = np.random.random(4096)
    x = np.asmatrix(x)

    # call the function
    image = reshape_to_image(x)
    
    assert type(image) == np.matrixlib.defmatrix.matrix
    assert image.shape == (64,64)

    for _ in range(20):
        i = np.random.randint(64)
        j = np.random.randint(64)
        assert image[i,j] == x[0,i*64+j]
    

#-------------------------------------------------------------------------
def test_load_dataset():
    ''' (2 points) load_dataset'''
    # call the function
    X, y, images = load_dataset()
    assert type(X) == np.matrixlib.defmatrix.matrix
    assert X.shape == (400,4096)
    assert type(y) == np.matrixlib.defmatrix.matrix
    assert y.shape == (400,1)
    assert type(images) == np.ndarray
    assert images.shape == (400,64,64)


#-------------------------------------------------------------------------
def test_compute_mu_image():
    ''' (5 points) compute_mu_image'''
    

    # call the function
    X, _, _ = load_dataset()
    mu = compute_mu_image(X)
    
    assert type(mu) == np.matrixlib.defmatrix.matrix
    assert mu.shape == (64,64)

    # write average face image to file 'mu.jpg'
    #scipy.misc.imsave('mu.jpg', mu)
    imageio.imwrite('mu.jpg', mu)
    
    # you could take a lot at the average face image in your folder
    
    assert np.allclose([0.40013435, 0.43423545, 0.4762809],mu[0,:3],atol=1e-2)
    assert np.allclose([0.36046496, 0.36678693, 0.37106389],mu[-1,:3],atol=1e-2)

#-------------------------------------------------------------------------
def test_compute_eigen_faces():
    ''' (15 points) compute_eigen_faces'''
    X = np.asmatrix(np.zeros((400,4096)))
    P_images, Xp = compute_eigen_faces(X,k= 2)
    assert type(P_images) == list
    assert type(Xp) == np.matrixlib.defmatrix.matrix
    assert len(P_images) == 2
    assert type(P_images[0]) == np.matrixlib.defmatrix.matrix
    assert P_images[0].shape == (64,64) 
    assert Xp.shape == (400,2) 

   
    # load the face image dataset 
    X, _, _ = load_dataset()
    P_images, Xp = compute_eigen_faces(X,k= 20)
    assert len(P_images) == 20

    for i,image in enumerate(P_images):
        #scipy.misc.imsave('eigen_face_%d.jpg' % (i+1), image) 
        imageio.imwrite('eigen_face_%d.jpg' % (i+1), image) 
    # If you are getting errors in the above line, the error is likely to be related to the `centering_X` function in problem2.
    # Note: in order to center X, you don't have to multiply mu with an all-one vector. Use the broadcasting method in numpy: Xc = X - mu

    c = P_images[0]
    assert np.allclose([-0.0041911,  -0.0071095,  -0.00933609],c[0,:3],atol=1e-3) or np.allclose([0.0041911,  0.0071095,  0.00933609],c[0,:3],atol=1e-3)
    assert np.allclose([0.01161627,  0.01290446,  0.01308161],c[-1,:3],atol=1e-3) or np.allclose([-0.01161627, -0.01290446, -0.01308161],c[-1,:3],atol=1e-3)

    # save the results into a file
    np.save('face_pca.npy',Xp)
