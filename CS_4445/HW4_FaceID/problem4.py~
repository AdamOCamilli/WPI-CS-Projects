import numpy as np
import math
#-------------------------------------------------------------------------
'''
    Problem 4: Graph Clustering (Spectral Clustering) 
    In this problem, you will implement a version of the spectral clustering method to cluster the nodes in a graph into two groups.
    You could test the correctness of your code by typing `nosetests -v test1.py` in the terminal.

    Notations:
            ---------- input data ------------------------
            n: the number of nodes in the network, an integer scalar.
            A:  the adjacency matrix, a float numpy matrix of shape n by n. 
                If there is a link between node i an node j, then A[i][j] = A[j][i] = 1. 
            ---------- computed data ------------------------
            D:  the degree matrix, a numpy float matrix of shape n by n. 
                All off-diagonal elements are 0. Each diagonal element represents the degree of the node (number of links). 
            L:  the Laplacian matrix, a numpy float matrix of shape n by n.  
                L = D-A
            E:  the eigen vectors of matrix L, a numpy float matrix of shape n by n. Each column of E corresponds to an eigen vector of matrix L.
            v:  the eigen values of matrix L, a numpy float array of length n. Each element corresponds to an eigen value of matrix L. E and v are paired.
            e2:  the eigen vector corresponding to the smallest non-zero eigen value, a numpy float matrix of shape (n by 1). 
            tol: the tolerance threshold for eigen values, a float scalar. A very small positive value. If an eigen value is smaller than tol, then we consider the eigen value as being 0. 
            x:  the binary vector of shape (n by 1), a numpy float vector of (0/1) values.
                It indicates a binary partition on the graph, such as [1.,1.,1., 0.,0.,0.].
            --------------------------------------------------
'''

#--------------------------
def compute_D(A):
    '''
        Compute the degree matrix D. 
        Input:
            A:  the adjacency matrix, a float numpy matrix of shape n by n. Here n is the number of nodes in the network.
                If there is a link between node i an node j, then A[i][j] = A[j][i] = 1. 
        Output:
            D:  the degree matrix, a numpy float matrix of shape n by n. 
                All off-diagonal elements are 0. Each diagonal element represents the degree of the node (number of links). 
    '''

    #########################################
    ## INSERT YOUR CODE HERE

    # degree vector of the nodes


    # diagonal matrix


    #########################################
    return D

#--------------------------
def compute_L(D,A):
    '''
        Compute the Laplacian matrix L. 
        Input:
            D:  the degree matrix, a numpy float matrix of shape n by n. 
                All off-diagonal elements are 0. Each diagonal element represents the degree of the node (number of links). 
            A:  the adjacency matrix, a float numpy matrix of shape n by n. Here n is the number of nodes in the network.
                If there is a link between node i an node j, then A[i][j] = A[j][i] = 1. 
        Output:
            L:  the Laplacian matrix, a numpy float matrix of shape n by n. 
    '''

    #########################################
    ## INSERT YOUR CODE HERE



    #########################################
    return L

#--------------------------
def compute_eigen_pairs(L):
    '''
        Compute the eigen vectors and eigen values of L. 
        Input:
            L:  the Laplacian matrix, a numpy float matrix of shape n by n. 
        Output:
            E:  the eigen vectors of matrix L, a numpy float matrix of shape p by p. Each column of E corresponds to an eigen vector of matrix L.
            v:  the eigen values of matrix L, a numpy float array of length p. Each element corresponds to an eigen value of matrix L. E and v are paired.
        Hint: you could use np.linalg.eig() to compute the eigen vectors of a matrix. 
        Note: in the returned eigen vectors "eig_vecs", each column (not row) is considered as an eigen vector. 
    '''

    #########################################
    ## INSERT YOUR CODE HERE


    #########################################
    return E,v


#--------------------------
def find_e2(E,v, tol= 1e-4):
    '''
        find the eigen vector that corresponds to the smallest non-zeros eigen values.
        For example, if the eigen values are [0.5, 0.000001, 2.], the first eigen vector is the answer.
        For example, if the eigen values are [2., 0.000001, 0.3], the last eigen vector is the answer.
        For example, if the eigen values are [0.00003, 0.000001, 0.3], the last eigen vector is the answer.
        Input:
            E:  the eigen vectors of matrix L, a numpy float matrix of shape n by n. Each column of E corresponds to an eigen vector of matrix L.
            v:  the eigen values of matrix L, a numpy float array of length n. Each element corresponds to an eigen value of matrix L. E and v are paired.
            tol: the tolerance threshold for eigen values, a float scalar. A very small positive value. If an eigen value is smaller than tol, then we consider the eigen value as being 0. 
        Output:
            e2:  the eigen vector corresponding to the smallest non-zero eigen value, a numpy float matrix of shape (n by 1). 
    '''

    #########################################
    ## INSERT YOUR CODE HERE

    # sort the eigen values





    #########################################
    return e2 


#--------------------------
def compute_x(e2):
    '''
        Compute the partition on the graph from the thresholding an eigen vector with 0 threshold. 
        Input:
            e2:  the eigen vector corresponding to the smallest non-zero eigen value, a numpy float matrix of shape (n by 1). 
        Output:
            x:  the binary vector of shape (n by 1), a numpy float vector of (0/1) values.
                It indicates a binary partition on the graph, such as [1.,1.,1., 0.,0.,0.].
    '''

    #########################################
    ## INSERT YOUR CODE HERE



    #########################################
    return x



#--------------------------
def spectral_clustering(A):
    '''
        Spectral clustering of a graph. 
        Input:
            A:  the adjacency matrix, a float numpy matrix of shape n by n. Here n is the number of nodes in the network.
                If there is a link between node i an node j, then A[i][j] = A[j][i] = 1. 
        Output:
            x:  the binary vector of shape (n by 1), a numpy float vector of (0/1) values.
                It indicates a binary partition on the graph, such as [1.,1.,1., 0.,0.,0.].
        Note: you cannot use any existing python package for spectral clustering, such as scikit-learn.
        Hint: x is related to the eigen vector of L with the smallest positive eigen values. 
              For example, if the eigen vector is [0.2,-0.1, -0.2], the values larger than zero will be 1, so x=[1,0,0] in this example.
              Note: if the eigen value is small enough (say smaller than 0.000001), we can treat it as being zero. 
    (5 points)
    '''
   
    #########################################
    ## INSERT YOUR CODE HERE

    # compute degree matrix


    # compute laplacian matrix


    # compute eigen pairs of L


    # find the eigen vector with the smallest non-zero eigen value


    # compute the graph partition


    #########################################
    return x 


