from mrjob.job import MRJob
#-------------------------------------------------------------------------
'''
    Problem 3: 
    In this problem, you will get familiar with the mapreduce framework. 
    In this problem, please install the following python package:
        * mrjob 
    Numpy is the library for writing Python programs that run on Hadoop.
    To install mrjob using pip, you could type `pip install mrjob` in the terminal.
    Alternatively, you could install from source code:
        (1) download source code from: https://github.com/Yelp/mrjob
        (2) in the code folder, type "python setup.py install" in the terminal.
    You could test the correctness of your code by typing `nosetests test3.py` in the terminal.
'''

#--------------------------
class CharCount(MRJob):
    ''' a character count for class, which compute the count of characters in a text document'''
    #----------------------
    def mapper(self, in_key, in_value):
        ''' 
            mapper function, which process a key-value pair in the data and generate intermediate key-value pair(s).
            It should return one key-value pair with value as the number of characters in the line (in_value)
            Input:
                    in_key: the key of a data record (in this example, can be ignored)
                    in_value: the value of a data record, (in this example, it is a line of text string in the document)
            Yield: 
                    (out_key, out_value) :intermediate key-value pair(s). 
                                          In this example, the out_key can be anything, 
                                          and out_value is the count of characters in the line, an integer value
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        #########################################

    #----------------------
    def reducer(self, in_key, in_values):
        ''' 
            reducer function, which processes a key and value list and produces output key-value pair(s)
            Input:
                    in_key: the key of a data record (in this example, can be ignored)
                    in_values: the python list of values, (in this example, it is a line of integer counts from different lines of the document)
            Yield: 
                    (out_key, out_value) : output key-value pair(s). 
                                          In this example, the out_key can be anything, 
                                          and out_value is the count of characters in the document, an integer value
        '''
        #########################################
        ## INSERT YOUR CODE HERE

        #########################################

