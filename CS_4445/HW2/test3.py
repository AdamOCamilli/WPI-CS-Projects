from problem3 import *
import sys
from io import BytesIO

'''
    Unit test 3:
    This file includes unit tests for problem3.py.
    You could test the correctness of your code by typing `nosetests test3.py` in the terminal.
'''

#-------------------------------------------------------------------------
def test_python_version():
    ''' ----------- Problem 3 (20 points in total)--------------'''
    assert sys.version_info[0]==3 # require python 3 (instead of python 2)


#-------------------------------------------------------------------------
def test_mapper():
    '''(5 points) mapper'''

    # create an object
    w = CharCount()

    # call the function
    outs = w.mapper(None,'hello') 

    # read one key-value pair from the output generator 
    out_pair = next(outs)

    # test whether or not the returned count is an integer
    assert type(out_pair[1]) == int

    # test the correctness of the result
    assert out_pair[1] == 5 

    #-------------------------
    # test another example

    # call the function
    outs = w.mapper(None,'hello world') 

    # test the correctness of the result
    n_pairs = 0 
    for key, value in outs: 
        assert value == 11 
        n_pairs +=1
    assert n_pairs ==1 

#-------------------------------------------------------------------------
def test_reducer():
    '''(5 points) reducer'''

    # create an object
    w = CharCount()

   # call the function
    outs = w.reducer(None,[1,2,3]) 

    # read one key-value pair from the output generator 
    out_pair = next(outs)

    # test whether or not the returned count is an integer
    assert type(out_pair[1]) == int

    # test the correctness of the result
    assert out_pair[1] == 6

    #-------------------------
    # test another example

    # call the function
    outs = w.reducer(None,[1,2,3,4]) 

    # test the correctness of the result
    out_pair = next(outs)
    assert out_pair[1] == 10 



#-------------------------------------------------------------------------
def test_CharCount():
    '''(10 points) CharCount'''
    # cast lines of text into standard Input
    num_lines = 5
    stdin = BytesIO(b'hello\n' * num_lines)

    # create a map reduce job (in a sandbox)
    job = CharCount()
    job.sandbox(stdin=stdin)

    # create a job runner
    runner = job.make_runner()

    # run the mapreduce job 
    runner.run()

    # parse the outputs
    results = []
    for line in runner.stream_output():
        # Use the job's specified protocol to read an output key-value pair
        key, value = job.parse_output_line(line)
        # append the value into a list 
        results.append(value)

    # test the correctness of the result
    assert results ==[25]



