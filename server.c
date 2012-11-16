The server creates a domain datagram socket. 
The server socket is assumed to have a (node-local) ‘well-known’ sun_path name which it binds to.
 This ‘well-known’ sun_path name is designated by a (network-wide) ‘well-known’ ‘port’ value.
The time client uses this ‘port’ value to communicate with the server.

The server enters an infinite sequence of calls to msg_recv followed by msg_send, awaiting client requests and responding to them.
 When it responds to a client request, it prints out on stdout the message
               server at node  vm i1  responding to request from  vm i2