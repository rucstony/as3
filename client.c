/*
*    msg_send():    
*    int          giving the socket descriptor for write
*    char*     giving the ‘canonical’ IP address for the destination node, in presentation format
*    int          giving the destination ‘port’ number
*    char*     giving message to be sent
*    int flag  if set, force a route rediscovery to the destination node even if a non-‘stale’ route already exists (see below) 
*
*/

void msg_send( int sockfd_for_write, char *destination_canonical_ip_presentation_format, 
                int destination_port_number, char *message_to_be_sent, int route_rediscovery_flag )
{
// this  function will write this info in a single char format to sockfd_for_write
char output_to_sock[MAXLINE];
output_to_sock= destination_canonical_ip_presentation_format+ "|"+destination_port_number+"|"+message_to_be_sent+"|"+route_rediscovery_flag;
write(sockfd_for_write,output_to_sock,sizeof(output_to_sock));
}

/*
When a client is evoked at a node, it creates a domain datagram socket.

    The client should bind its socket to a ‘temporary’ (i.e., not ‘well-known’) sun_path name obtained from a call to tmpnam() 
    (cf. line 10, Figure 15.6, p. 419) so that multiple clients may run at the same node.
*/
int fd; 
char template[] = "fileXXXXXX";
fd = mkstemp(template);

bzero(&cliaddr, sizeof(cliaddr));       /* bind an address for us */
cliaddr.sun_family = AF_LOCAL;
strcpy(cliaddr.sun_path, template);

/*
    Note that tmpnam() is actually highly deprecated. You should use the mkstemp() function instead - look up the
     online man pages on minix (‘man mkstemp’) for details.

    As you run client code again and again during the development stage, the temporary files created by the calls
     to tmpnam / mkstemp 
    start to proliferate since these files are not automatically removed when the client code terminates. 
    You need to explicitly remove the file created by the client evocation by issuing a call to unlink() or to remove() 
    in your client code just before the client code exits. 
    the online man pages on minix (‘man unlink’, ‘man remove’) for details. 


The client then enters an infinite loop repeating the steps below.

    The client prompts the user to choose one of vm1 , . . . . . , vm10 as a server node.

    Client msg_sends a 1 or 2 byte message to server and prints out on stdout the message
         client at node  vm i1  sending request to server at  vm i2
    (In general, throughout this assignment, “trace” messages such as the one above should give the vm names and not IP addresses of the nodes.)

    Client then blocks in msg_recv awaiting response. This attempt to read from the domain socket should be backed up by a 
    timeout in case no response ever comes. I leave it up to you whether you ‘wrap’ the call to msg_recv in a timeout,
     or you implement the timeout inside msg_recv itself.
    
    When the client receives a response it prints out on stdout the message
         client at node  vm i1 : received from   vm i2  <timestamp>
    If, on the other hand, the client times out, it should print out the message
         client at node  vm i1 : timeout on response from   vm i2
    The client then retransmits the message out, setting the flag parameter in msg_send to force a route rediscovery,
     and prints out an appropriate message on stdout. This is done only once, when a timeout for a given message to the server occurs for the first time.

    Client repeats steps 1. - 3.
*/
while(1)
{
    char *server_vm;
    printf("Please select the server VM : vm1,vm2, ... vm10 :\n");
    scanf("%s",server_vm);
    printf("Client at node  vm i1  sending request to server at  vm i2\n", );

    msg_send();
    msg_recv();
    printf("Client at node  vm i1 : received from   vm i2  <timestamp>\n");
    if(msg_recv_timeout)
    {
        printf("Client at node  vm i1 : timeout on response from   vm i2\n");
        route_rediscovery_flag=1
        msg_retransmit();
    }    
}

