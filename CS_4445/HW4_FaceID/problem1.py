#-------------------------------------------------------------------------
# Note: please don't use any additional package except the following packages
import numpy as np
import math
#-------------------------------------------------------------------------
'''
    Problem 1: PCA 
    In this problem, you will implement a version of the principal component analysis method to reduce the dimensionality of data.
    You could test the correctness of your code by typing `nosetests -v test1.py` in the terminal.

    Notations:
            ---------- input data ------------------------
            n: the number of data instances (for example, # of images), an integer scalar.
            p: the number of dimensions (for example, # of pixels in each image), an integer scalar.
            X: the feature matrix, a float numpy matrix of shape n by p. 
            ---------- computed data ----------------------
            mu: the average vector of matrix X, a numpy float matrix of shape 1 by p. 
                Each element mu[0,i] represents the average value in the i-th column of matrix X.
            Xc: the centered matrix X, a numpy float matrix of shape n by p. 
                Each column has a zero mean value.
            C:  the covariance matrix of matrix X, a numpy float matrix of shape p by p. 
            k:  the number of dimensions to reduce to (k should be smaller than p), an integer scalar
            E:  the eigen vectors of matrix X, a numpy float matrix of shape p by p. 
                Each column of E corresponds to an eigen vector of matrix X.
            v:  the eigen values of matrix X, a numpy float array of length p. 
                Each element corresponds to an eigen value of matrix X. E and v are paired.
            Xp: the projected feature matrix with reduced dimensions, a numpy float matrix of shape n by k. 
             P: the projection matrix, a numpy float matrix of shape p by k. 
            -----------------------------------------------
'''

#--------------------------
def centering_X(X):
    '''
        Centering matrix X, so that each column has zero mean.
        Input:
            X:  the feature matrix, a float numpy matrix of shape n by p. Here n is the number of data records, p is the number of dimensions.
        Output:
            Xc:  the centered matrix X, a numpy float matrix of shape n by p. 
            mu:  the average row vector of matrix X, a numpy float matrix of shape 1 by p. 
        Note: please don't use the np.cov() function. There seems to be a bug in their code which will result in an error in later test cases. 
              Please implement this function only using basic numpy functions, such as np.mean().
    '''

    #########################################
    ## INSERT YOUR CODE HERE

    mean = np.mean(X);
    mu = X.mean(axis=0)
    Xc = X - mean

    #########################################
    return Xc, mu


#--------------------------
def compute_C(Xc):
    '''
        Compute the covariance matrix C. 
        Input:
            Xc:  the centered feature matrix, a float numpy matrix of shape n by p. Here n is the number of data records, p is the number of dimensions.
        Output:
            C:  the covariance matrix, a numpy float matrix of shape p by p. 
        Note: please don't use the np.cov() function here. Implement the function using matrix multiplication.
    '''

    #########################################
    ## INSERT YOUR CODE HERE
    
    rows = Xc.shape[0]
    cols = Xc.shape[1]
    
    e = np.ones((rows, 1))
    X = Xc - (e * e.transpose() * Xc)/rows
    Y = X.transpose() * X
    C = Y / (rows - 1) # (rows - 1) since Xc is sample data
    
    #########################################
    return C


#--------------------------
def compute_eigen_pairs(C):
    '''
        Compute the eigen vectors and eigen values of C. 
        Input:
            C:  the covariance matrix, a numpy float matrix of shape p by p. 
        Output:
            E:  the eigen vectors of matrix C, a numpy float matrix of shape p by p. Each column of E corresponds to an eigen vector of matrix C.
            v:  the eigen values of matrix C, a numpy float array of length p. Each element corresponds to an eigen value of matrix C. E and v are paired.
        Hint: you could use np.linalg.eig() to compute the eigen vectors of a matrix. 
    '''

    #########################################
    ## INSERT YOUR CODE HERE

    E = np.linalg.eig(C)[1] # returns 2-tuple
    v = np.linalg.eigvals(C)

    # print(type(C), type(E))
    # print(C)
    # print(E)
    #########################################
    return E,v

#--------------------------
def compute_P(E,v,k):
    '''
        Compute the projection matrix P by combining the eigen vectors with the top k largest eigen values. 
        Input:
            E:  the eigen vectors of matrix X, a numpy float matrix of shape p by p. Each column of E corresponds to an eigen vector of matrix X.
            v:  the eigen values of matrix X, a numpy float array of length p. Each element corresponds to an eigen value of matrix X. E and v are paired.
            k:  the number of dimensions to reduce to (k should be smaller than p), an integer scalar
        Output:
            P: the projection matrix, a numpy float matrix of shape p by k. 
    '''
    
    #########################################
    ## INSERT YOUR CODE HERE
    v_sorted = sorted(v, reverse=True)
    k_largest = [v_sorted[i] for i in range(len(v_sorted)) if i < k]
    
    P_all = []  # will be list of eigenvectors (columns of E)
    for eigval in k_largest:
        index = v.tolist().index(eigval)
        P_all.append(E[:,index])
    
    P = np.concatenate(P_all,axis=1)
    # print(v, "->", k_largest)
    # print(P)
    # print()

    
    #########################################
    return P


#--------------------------
def compute_Xp(Xc,P):
    '''
        Compute the projected feature matrix Xp by projecting data Xc using matrix P. 
        Input:
            Xc:  the feature matrix after centering, a float numpy matrix of shape n by p. Here n is the number of data records, p is the number of dimensions.
             P: the projection matrix, a numpy float matrix of shape p by k. 
        Output:
            Xp: the feature matrix after projection (dimension reduced), a numpy float matrix of shape p by k. 
    '''

    #########################################
    ## INSERT YOUR CODE HERE

    Xp = Xc * P

    #########################################
    return Xp



#--------------------------
def PCA(X, k=1):
    '''
        Compute PCA of matrix X. 
        Input:
            X:  the feature matrix, a float numpy matrix of shape n by p. Here n is the number of data records, p is the number of dimensions.
            k:  the number of dimensions to output (k should be smaller than p)
        Output:
            Xp: the feature matrix with reduced dimensions, a numpy float matrix of shape n by k. 
             P: the projection matrix, a numpy float matrix of shape p by k. 
        Note: in this problem, you cannot use existing package for PCA, such as scikit-learn
    '''

    #########################################
    ## INSERT YOUR CODE HERE

    # centering matrix X 
    Xc,mu = centering_X(X)

    # compute covariance matrix C
    C = compute_C(Xc)

    # compute eigen pairs of L
    E,v = compute_eigen_pairs(C) 

    # compute the projection matrix 
    P = compute_P(E,v,k)

    # project the data into lower dimension using projection matrix P and centered data matrix X
    Xp = compute_Xp(Xc,P)

    #########################################
    return Xp, P 


