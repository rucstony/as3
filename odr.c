
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include	"hw_addrs.h"
#include "unp.h"
#define PROTOCOL_VALUE 108817537
#define ROUTING_BUF_SIZE 100


typedef struct 
{
char* destination_canonical_ip_address;
char* next_hop_node_ethernet_address;
char* outgoing_interface_index;
int number_of_hops_to_destination;
struct timeval made_or_last_reconfirmed_or_updated_timestamp;
}routing_entry;

/*
ROUTING TABLE 

Build and maintain a routing table. For each destination in the table,
the routing table structure should include, at a minimum, the next-hop node
(in the form of the Ethernet address for that node) and outgoing interface index,
the number of hops to the destination, and a timestamp of when the the routing table
entry was made or last “reconfirmed” / updated. Note that a destination node in the table 
is to be identified only by its ‘canonical’ IP address, and not by any other IP addresses the node has.
*/
//DEFINE structure for RREQ/RREP messages
struct req_msgs
{
	char source_addr[INET_ADDRSTRLEN];
	//source_sequence_#
	int broadcast_id; //incremented every time the source issues new RREQ
	char destination_canonical_ip_address[INET_ADDRSTRLEN];
	//dest_sequence_#
	int number_of_hops_to_destination;
	int control_msg_type; //(RREQ or RREP)
	int RREP_sent_flag;
};


int check_if_route_exists(char * destination_canonical_ip_presentation_format,routing_entry *existing_entry);	
routing_entry routing_table[ROUTING_BUF_SIZE];
routing_entry recv_ODR();
long staleness_param;


int main(int argc, char const *argv[])
{
	struct hwa_info	*hwa, *hwahead;
	struct sockaddr	*sa;
	char   *ptr;
	int    i, j, prflag,route_exists,broadcast_id;
	int packet_socket;
	int					sockfd;
	struct sockaddr_un	cliaddr, servaddr;
	char data_stream[MAXLINE];
	char *msg_fields[MAXLINE];
	char str_from_sock[MAXLINE], source_addr[INET_ADDRSTRLEN];
	char*	 destination_canonical_ip_presentation_format;
	int	 destination_port_number;
	char*	 message_to_be_sent;
	int	 route_rediscovery_flag;
	struct req_msgs RREQ,RREP;
	routing_entry existing_entry;
	struct req_msgs req_type;

	if(argc<2)
	{
		printf("invalid args: ODR <seconds>\n");
	}
	else
	{
		printf( "Staleness parameter is : %s\n", staleness_param );
		staleness_param = atoi( argv[1] );
	}
/*
The ODR process runs on each of the ten vm machines. It is evoked with a single command line argument which gives a “staleness” time parameter, in seconds.
*/


/* PF_PACKET creation */
/*
The socket(s) should have a protocol value (given as a network-byte-order parameter in the call(s) to function socket)
that identifies your ODR protocol. The <linux/if_ether.h> include file (i.e., the file /usr/include/linux/if_ether.h)
contains protocol values defined for the standard protocols typically found on an Ethernet LAN, as well as other values
such as ETH_P_ALL. You should set protocol to a value of your choice which is not a <linux/if_ether.h> value, but which
is, hopefully, unique to yourself. Remember that you will all be running your code using the same root account on the 
vm1 , . . . . . , vm10 nodes. So if two of you happen to choose the same protocol value and happen to be running on 
the same vm node at the same time, your applications will receive each other’s frames. For that reason, try to choose 
a protocol value for the socket(s) that is likely to be unique to yourself (something based on your Stony Brook student 
ID number, for example). This value effectively becomes the protocol value for your implementation of ODR, as opposed 
to some other cse 533 student's implementation. Because your value of protocol is to be carried in the frame type field of
the Ethernet frame header, the value chosen should be not less than 1536 (0x600) so that it is not misinterpreted as the length
of an Ethernet 802.3 frame.
*/

//packet_socket = socket(PF_PACKET, int socket_type, int protocol);
packet_socket = socket(PF_PACKET, SOCK_RAW, htonl(PROTOCOL_VALUE));

/*
It uses get_hw_addrs (available to you on minix in ~cse533/Asgn3_code) to obtain the index, and associated (unicast) IP 
and Ethernet addresses for each of the node’s interfaces, except for the eth0 and lo (loopback) interfaces,
 which should be ignored.

    In the subdirectory ~cse533/Asgn3_code (/home/users/cse533/Asgn3_code) on minix I am providing you with two functions,
     get_hw_addrs and prhwaddrs. These are analogous to the get_ifi_info_plus and prifinfo_plus of Assignment 2. 
     Like get_ifi_info_plus, get_hw_addrs uses ioctl. get_hw_addrs gets the (primary) IP address, alias IP addresses 
     (if any), HW address, and interface name and index value for each of the node's interfaces (including the loopback
      interface lo). prhwaddrs prints that information out. You should modify and use these functions as needed.

    Note that if an interface has no HW address associated with it
(this is, typically, the case for the loopback interface lo for example), 
then ioctl returns get_hw_addrs a HW address which is the equivalent of 00:00:00:00:00:00 .
  get_hw_addrs stores this in the appropriate field of its data structures as it would with any HW address
   returned by ioctl, but when prhwaddrs comes across such an address, it prints a blank line instead of
   its usual ‘HWaddr = xx:xx:xx:xx:xx:xx’. 
*/

/*
struct hwa_info {
  char    if_name[IF_NAME];     // interface name, null terminated 
  char    if_haddr[IF_HADDR];   // hardware address 
  int     if_index;             // interface index 
  short   ip_alias;             // 1 if hwa_addr is an alias IP address 
  struct  sockaddr  *ip_addr;   // IP address 
  struct  hwa_info  *hwa_next;  // next of these structures 
};

*/

//parse through hwa to get all addrs and store into struct array of 
//write below loop by refering to the pr_hw_addrs function and fill hw_addrs

for (hwahead = hwa = Get_hw_addrs(); hwa != NULL; hwa = hwa->hwa_next) {
	
	printf("%s :%s", hwa->if_name, ((hwa->ip_alias) == IP_ALIAS) ? " (alias)\n" : "\n");
	
	if ( (sa = hwa->ip_addr) != NULL)
		printf("         IP addr = %s\n", Sock_ntop_host(sa, sizeof(*sa)));
			
	prflag = 0;
	i = 0;
	do {
		if (hwa->if_haddr[i] != '\0') {
			prflag = 1;
			break;
		}
	} while (++i < IF_HADDR);

	if (prflag) {
		printf("         HW addr = ");
		ptr = hwa->if_haddr;
		i = IF_HADDR;
		do {
			printf("%.2x%s", *ptr++ & 0xff, (i == 1) ? " " : ":");
		} while (--i > 0);
	}

	printf("\n         interface index = %d\n\n", hwa->if_index);
}


/*
The ODR process creates one or more PF_PACKET sockets.
*/

/*
If, when you read from the socket and provide a sockaddr_ll structure, 
the kernel returns to us the index of the interface on which the incoming frame was received,
then one socket will be enough. Otherwise, somewhat in the manner of Assignment 2, you shall 
have to create a PF_PACKET socket for every interface of interest (which are all the interfaces 
of the node, excluding interfaces lo and eth0 ), and bind a socket to each interface. Furthermore, 
if the kernel also returns to us the source Ethernet address of the frame in the sockaddr_ll structure,
then you can make do with SOCK_DGRAM type PF_PACKET sockets; otherwise you shall have to use SOCK_RAW type
sockets (although I would prefer you to use SOCK_RAW type sockets anyway, even if it turns out you can make 
do with SOCK_DGRAM type).
*/


/*
Note from the man pages for packet(7) that frames are passed to and from the
socket without any processing in the frame content by the device driver on 
the other side of the socket, except for calculating and tagging on the 4-byte
CRC trailer for outgoing frames, and stripping that trailer before delivering
incoming frames to the socket. Nevertheless, if you write a frame that is 
less than 60 bytes, the necessary padding is automatically added by the 
device driver so that the frame that is actually transmitted out is the minimum
Ethernet size of 64 bytes. When reading from the socket, however, any such 
padding that was introduced into a short frame at the sending node to bring it
up to the minimum frame size is not stripped off - it is included in what you
receive from the socket (thus, the minimum number of bytes you receive should
never be less than 60). Also, you will have to build the frame header for 
outgoing frames yourselves (assuming you use SOCK_RAW type sockets). Bear in
mind that the field values in that header have to be in network order. 
*/


/*
The ODR process also creates a domain datagram socket for communication with application processes
 at the node, and binds the socket to a ‘well known’ sun_path name for the ODR service.
*/


sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);

bzero(&servaddr, sizeof(servaddr));
servaddr.sun_family = AF_LOCAL;
strcpy(servaddr.sun_path, UNIXDG_PATH);

bind(sockfd, (SA *) &servaddr, sizeof(servaddr));




/*

Because it is dealing with fixed topologies, ODR is, by and large, considerably simpler than AODV.
In particular, discovered routes are relatively stable and there is no need for all the paraphernalia 
that goes with the possibility of routes changing (such as maintenance of active nodes in the routing tables
 and timeout mechanisms; timeouts on reverse links; lifetime field in the RREP messages; etc.)

Nor will we be implementing source_sequence_#s (in the RREQ messages), and dest_sequence_# (in RREQ and RREP messages).
In reality, we should (though we will not, for the sake of simplicity, be doing so) implement some sort of sequence number
mechanism if we are to avoid possible scenarios of routing loops in a “count to infinity” context
(I shall explain this point in class).
*/

/*
However, we want ODR to discover shortest-hop paths, and we want it to do so in a reasonably efficient manner.
This necessitates having one or two aspects of its operations work in a different, possibly slightly more 
complicated, way than AODV does. ODR has several basic responsibilities :
*/
/*

FLOODING THE RREQ
Generate a RREQ in response to a time client calling msg_send for a destination for which ODR 
has no route (or for which a route exists, but msg_send has the flag parameter set or the 
route has gone ‘stale’ – see below), and ‘flood’ the RREQ out on all the node’s interfaces 
(except, of course, for interfaces eth0 and lo).

Flooding is done using an Ethernet broadcast 
destination address (0xff:ff:ff:ff:ff:ff) in the outgoing frame header.

Note that a copy of the broadcast packet is supposed to / might be looped back to the node that sends it (see p. 535).
ODR will have to take care not to treat these copies as new incoming RREQs.

Also note that ODR at the client node increments the broadcast_id every time it issues a new RREQ for any destination node. 
*/

while(fgets(sockfd,data_stream,MAXLINE)!=NULL) //data will be written into sockfd when client msg_send()s
{

	msg_fields[0] = strtok(str_from_sock, "|"); //get pointer to first token found and store in 0
                                       //place in array
    while(msg_fields[i]!= NULL) 
    {   //ensure a pointer was found
        i++;
        msg_fields[i] = strtok(NULL, "|"); //continue to tokenize the string
    }
    
    for(j = 0; j <= i-1; j++) {
        printf("%s\n", msg_fields[j]); //print out all of the tokens
    }
    
	
	 destination_canonical_ip_presentation_format=msg_fields[0];
	 destination_port_number=atoi(msg_fields[1]);
	 message_to_be_sent=msg_fields[2];
	 route_rediscovery_flag=atoi(msg_fields[3]);
	 
	
	if(!route_rediscovery_flag)
	{
		route_exists=check_if_route_exists(destination_canonical_ip_presentation_format,&existing_entry); //pass address of existing_entry so that its value can be set inside the function
	}else
	{	
		route_exists=0;
	}

	if(route_exists)
	{
		//send_RREP(existing_entry);


	}else
	{
		
		strcpy(RREQ.source_addr,source_addr);
		RREQ.broadcast_id=broadcast_id++;//incremented every time the source issues new RREQ
		strcpy(RREQ.destination_canonical_ip_address,destination_canonical_ip_presentation_format);
		RREQ.number_of_hops_to_destination=0; //sending RREQ from source
		RREQ.control_msg_type=1; //(RREQ = 1)
		//send_RREQ(RREQ);
	}
}


/*
RECIEVING RREQs AND GENERATING RREPs
When a RREQ is received, ODR has to generate a RREP if it is at the destination node, or if it is at an intermediate
node that happens to have a route (which is not ‘stale’ – see below) to the destination. Otherwise, it must propagate
the RREQ by flooding it out on all the node’s interfaces (except the interface the RREQ arrived on).

Note that as it processes received RREQs, ODR should enter the ‘reverse’ route back to the source node into its routing
table, or update an existing entry back to the source node if the RREQ received shows a shorter-hop route, or a route
with the same number of hops but going through a different neighbour. The timestamp associated with the table entry
should be updated whenever an existing route is either “reconfirmed” or updated. Obviously, if the node is going to generate a RREP, updating an existing entry back to the source node with a more efficient route, or a same-hops route using a different neighbour, should be done before the RREP is generated.

Unlike AODV, when an intermediate node receives a RREQ for which it generates a RREP, it should
nevertheless continue to flood the RREQ it received if the RREQ pertains to a source node whose
existence it has heretofore been unaware of, or the RREQ gives it a more efficient route than it
knew of back to the source node (the reason for continuing to flood the RREQ is so that other 
nodes in the intranet also become aware of the existence of the source node or of the potentially
more optimal reverse route to it, and update their tables accordingly). However, since an RREP for
this RREQ is being sent by our node, we do not want other nodes who receive the RREQ propagated by
our node, and who might be in a position to do so, to also send RREPs. So we need to introduce a field
in the RREQ message, not present in the AODV specifications, which acts like a “RREP already sent” field.
Our node sets this field before further propagating the RREQ and nodes receiving an RREQ with this field
set do not send RREPs in response, even if they are in a position to do so.


ODR may, of course, receive multiple, distinct instances of the same RREQ (the combination of source_addr
and broadcast_id uniquely identifies the RREQ). Such RREQs should not be flooded out unless they have
a lower hop count than instances of that RREQ that had previously been received.

By the same token, if ODR is in a position to send out a RREP, and has already done so for this,
now repeating, RREQ ,  it should not send out another RREP unless the RREQ shows a more efficient,
previously unknown, reverse route back to the source node. In other words, ODR should not generate
essentially duplicative RREPs, nor generate RREPs to instances of RREQs that reflect reverse routes
to the source that are not more efficient than what we already have. 

Relay RREPs received back to the source node (this is done using the ‘reverse’ route entered into
the routing table when the corresponding RREQ was processed). At the same time, a ‘forward’ path
to the destination is entered into the routing table. ODR could receive multiple, distinct RREPs
for the same RREQ. The ‘forward’ route entered in the routing table should be updated to reflect
the shortest-hop route to the destination, and RREPs reflecting suboptimal routes should not be
relayed back to the source.
    
In general, maintaining a route and its associated timestamp in the table in response to RREPs
received is done in the same manner described above for RREQs.

Forward time client/server messages along the next hop.

(The following is important – you will lose points if you do not implement it.)
Note that such application payload messages (especially if they are the initial request 
from the client to the server, rather than the server response back to the client) can be
like “free” RREPs, enabling nodes along the path from source (client) to destination 
(server) node to build a reverse path back to the client node whose existence they were 
heretofore unaware of (or, possibly, to update an existing route with a more optimal one).
Before it forwards an application payload message along the next hop, ODR at an intermediate 
node (and also at the final destination node) should use the message to update its routing 
table in this way. Thus, calls to msg_send by time servers should never cause ODR at the server
node to initiate RREQs, since the receipt of a time client request implies that a route back to
the client node should now exist in the routing table. The only exception to this is if the 
server node has a staleness parameter of zero (see below). 

A routing table entry has associated with it a timestamp that gives the time the entry
was made into the table. When a client at a node calls msg_send, and if an entry for 
the destination node already exists in the routing table, ODR first checks that the
routing information is not ‘stale’. A stale routing table entry is one that is older
than the value defined by the staleness parameter given as a command line argument to
the ODR process when it is executed. ODR deletes stale entries (as well as non-stale
entries when the flag parameter in msg_send is set) and initiates a route rediscovery
by issuing a RREQ for the destination node. This will force periodic updating of the 
routing tables to take care of failed nodes along the current path, Ethernet addresses
that might have changed, and so on. Similarly, as RREQs propagate through the intranet,
existing stale table entries at intermediate nodes are deleted and new route discoveries
propagated. As noted above when discussing the processing of RREQs and RREPs, the associated
timestamp for an existing table entry is updated in response to having the route either 
“reconfirmed” or updated (this applies to both reverse routes, by virtue of RREQs received,
and to forward routes, by virtue of RREPs). Finally, note that a staleness parameter of 0 
essentially indicates that the discovered route will be used only once, when first discovered,
 and then discarded. Effectively, an ODR with staleness parameter 0 maintains no real routing
 table at all ;  instead, it forces route discoveries at every step of its operation.

As a practical matter, ODR should be run with staleness parameter values that are considerably
larger than the longest RTT on the intranet, otherwise performance will degrade considerably
(and collapse entirely as the parameter values approach 0). Nevertheless, for robustness,
we need to implement a mechanism by which an intermediate node that receives a RREP or
application payload message for forwarding and finds that its relevant routing table
entry has since gone stale, can intiate a RREQ to rediscover the route it needs.


RREQ, RREP, and time client/server request/response messages will all have to be carried as
encapsulated ODR protocol messages that form the data payload of Ethernet frames. So we need
to design the structure of ODR protocol messages. The format should contain a type field 
(0 for RREQ, 1 for RREP, 2 for application payload ). The remaining fields in an ODR message
will depend on what type it is. The fields needed for (our simplified versions of AODV’s) RREQ
and RREP should be fairly clear to you, but keep in mind that you need to introduce two extra fields:

The “RREP already sent” bit or field in RREQ messages, as mentioned above.

A “forced discovery” bit or field in both RREQ and RREP messages:

When a client application forces route rediscovery, this bit should be set in the RREQ issued by
the client node ODR.

Intermediate nodes that are not the destination node but which do have a route to the destination
node should not respond with RREPs to an RREQ which has the forced discovery field set. Instead,
they should continue to flood the RREQ so that it eventually reaches the destination node which 
will then respond with an RREP.

The intermediate nodes relaying such an RREQ must update their ‘reverse’ route back to the source
node accordingly, even if the new route is less efficient (i.e., has more hops) than the one they
currently have in their routing table.

The destination node responds to the RREQ with an RREP in which this field is also set.

Intermediate nodes that receive such a forced discovery RREP must update their ‘forward’ route
to the destination node accordingly, even if the new route is less efficient (i.e., has more hops)
than the one they currently have in their routing table.

This behaviour will cause a forced discovery RREQ to be responded to only by the destination node
itself and not any other node, and will cause intermediate nodes to update their routing tables 
to both source and destination nodes in accordance with the latest routing information received,
to cover the possibility that older routes are no longer valid because nodes and/or links along
their paths have gone down.
*/
/*

while(req_type=recv_ODR()) //recv RREQ or RREP
{
	if(req_type.control_msg_type == 1)//RREQ
	{
		 req_type.number_of_hops_to_destination++;
		 destination_canonical_ip_presentation_format=req_type.destination_canonical_ip_address;
		 destination_port_number=atoi(req_type.destination_canonical_ip_address);
		 message_to_be_sent=msg_fields[2];
		 route_rediscovery_flag=atoi(msg_fields[3]);
		 

		 route_exists=check_if_route_exists(destination_canonical_ip_presentation_format,&existing_entry); //pass address of existing_entry so that its value can be set inside the function
	
		if(route_exists)
		{
			if(req_type.RREP_sent_flag==0)
			{
				send_RREP(existing_entry);
				req_type.RREP_sent_flag = 1; 
			}else{
				
				send_RREQ(req_type);
			}

		}else
		{
			update_routing_table();
			send_RREQ(req_type);
			
		}
	}
}
*/
	/* code */
	return 0;
}

long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
	long msec;
	
	msec = (finishtime->tv_sec-starttime->tv_sec)*1000;
	msec += (finishtime->tv_usec-starttime->tv_usec)/1000;
	return msec;
}

int check_if_route_exists(char * destination_canonical_ip_presentation_format,routing_entry *existing_entry)
{
  	long msec;
	int count=0;
	int route_found=0;
	struct timeval curr_time_ms; 
	routing_entry route;
	while( routing_table[count].destination_canonical_ip_address!=NULL )
	{
		 route=routing_table[count];
		if( strcmp(route.destination_canonical_ip_address, destination_canonical_ip_presentation_format) == 0)
		{
			gettimeofday(&curr_time_ms, NULL);
		  	msec = timevaldiff( &(route.made_or_last_reconfirmed_or_updated_timestamp), &curr_time_ms );
				
			if(msec >= staleness_param )
			{
				printf("Time diff : %ld\n", msec);
			//	routing_table[count].remove();
			}
			else{
				route_found = 1;
				route.made_or_last_reconfirmed_or_updated_timestamp = curr_time_ms;
			}
			break;
		}
		count++;
	}
	if( route_found == 1 )
	{
		existing_entry = &routing_table[count];
	}
return route_found;

}
routing_entry recv_ODR()
{
	routing_entry entry;
	return entry;
}
/*
A type 2, application payload, message needs to contain the following type of information :

    type  =  2
    ‘canonical’ IP address of source node
    ‘port’ number of source application process (This, of course, is not a real port number in the TCP/UDP sense, but simply a value that ODR at the source node uses to designate the sun_path name for the source application’s domain socket.)
    ‘canonical’ IP address of destination node
    ‘port’ number of destination application process (This is passed to ODR by the application process at the source node when it calls msg_send. Its designates the sun_path name for an application’s domain socket at the destination node.)
    hop count (This starts at 0 and is incremented by 1 at each hop so that ODR can make use of the message to update its routing table, as discussed above.)
    number of bytes in application message

The fields above essentially constitute a ‘header’ for the ODR message. Note that fields which
you choose to have carry numeric values (rather than ascii characters, for example) must be in
network byte order. ODR-defined numeric-valued fields in type 0, RREQ, and type 1, RREP, messages
must, of course, also be in network byte order.

Also note that only the ‘canonical’ IP addresses are used for the source and destination nodes
in the ODR header. The same has to be true in the headers for type 0, RREQ, and type 1, RREP,
messages. The general rule is that ODR messages only carry ‘canonical’ IP node addresses.

The last field in the type 2 ODR message is essentially the data payload of the message.

    application message given in the call to msg_send

An ODR protocol message is encapsulated as the data payload of an Ethernet frame whose header it fills in as follows :

    source address  =  Ethernet address of outgoing interface of the current node where ODR is processing the message.
    destination address  =  Ethernet broadcast address for type 0 messages; Ethernet address of next hop node for type 1 & 2 messages.
    protocol field  =  protocol value for the ODR PF_PACKET socket(s).

Last but not least, whenever ODR rites an Ethernet frame out through its socket, it prints out on stdout the message
     ODR at node  vm i1 : sending      frame  hdr    src  vm i1      dest  addr
                                                      ODR msg      type n     src  vm i2      dest  vm i3
where addr is in presentation format (i.e., hexadecimal xx:xx:xx:xx:xx:xx) and gives the destination Ethernet address
in the outgoing frame header. Other nodes in the message should be identified by their vm name. A message should be 
printed out for each packet sent out on a distinct interface.

*/

