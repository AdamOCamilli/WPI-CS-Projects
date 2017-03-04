Assumptions:
	-No two nodes can occupy the same location on a grid.
	
	-Nodes only receive messages from some Node A if they are listening to the same channel as A and if they themsleves (not their range) 
	are overlapped in A's range. If for example A was located at (50,50) and B at (50,56), B would detect collisions if it and A tried to
	broadcast at the same time on the same channel, but they would not receive each others' messages.
	
 
	-If a message is sent to a node on a different channel, it is lost

	-If a node has more than one message to rebroadcast, it will rebroadcast all of them after broadcasting its own message

	

