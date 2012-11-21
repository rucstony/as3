#include 	"unp.h"
/*
A time server runs on each of the ten vm machines. The client code should also be available on each vm so that it can be
evoked at any of them.

Normally, time clients/servers exchange request/reply messages using the TCP/UDP socket API that,
effectively, enables them to receive service (indirectly, via the transport layer) from the local 
IP mechanism running at their nodes. You are to implement an API using Unix domain sockets to 
access the local ODR service directly (somewhat similar, in effect, to the way that raw sockets
 permit an application to access IP directly). Use Unix domain SOCK_DGRAM, rather than SOCK_STREAM,
 sockets (see Figures 15.5 & 15.6, pp. 418 - 419).
*/


 

/*
*    msg_send():    
*    int          giving the socket descriptor for write
*    char*     giving the ‘canonical’ IP address for the destination node, in presentation format
*    int          giving the destination ‘port’ number
*    char*     giving message to be sent
*    int flag  if set, force a route rediscovery to the destination node even if a non-‘stale’ route already exists (see below) 
*
*/


//The server creates a domain datagram socket.
int main()
{
	int					sockfd;
	struct sockaddr_un	cliaddr, servaddr;
    char message_received[MAXLINE];  
    char source_canonical_ip_presentation_format[100];
    char source_port_number[10];

	sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXDG_PATH);

	bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

	while(1)
	{
        msg_recv( sockfd, message_received, source_canonical_ip_presentation_format, source_port_number);
		printf("Server at node  vm i1  responding to request from  vm i2\n");
        msg_send(  sockfd,  source_canonical_ip_presentation_format, "72217",  "message_to_be_sent(from-server)", route_rediscovery_flag );

	}

	return 0;
}
/*
The server socket is assumed to have a (node-local) ‘well-known’ sun_path name which it binds to.
 This ‘well-known’ sun_path name is designated by a (network-wide) ‘well-known’ ‘port’ value.
The time client uses this ‘port’ value to communicate with the server.

The server enters an infinite sequence of calls to msg_recv followed by msg_send,
 awaiting client requests and responding to them.
 When it responds to a client request, it prints out on stdout the message
               server at node  vm i1  responding to request from  vm i2
*/               
