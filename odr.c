
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#define PROTOCOL_VALUE 108817537	

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
packet_socket = socket(PF_PACKET, int socket_type, int protocol);

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

get_hw_addrs();

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


/*
ROUTING TABLE 

Build and maintain a routing table. For each destination in the table,
the routing table structure should include, at a minimum, the next-hop node
(in the form of the Ethernet address for that node) and outgoing interface index,
the number of hops to the destination, and a timestamp of when the the routing table
entry was made or last “reconfirmed” / updated. Note that a destination node in the table 
is to be identified only by its ‘canonical’ IP address, and not by any other IP addresses the node has.
*/

destination_canonical_ip_address, next_hop_node_ethernet_address, outgoing_interface_index, number_of_hops_to_destination, made_or_last_reconfirmed_or_updated_timestamp
