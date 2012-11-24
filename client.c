#include    "unp.h"
#define HOSTNAME_LEN 255
#define UNIXDG_PATH "testpath"
#include <setjmp.h>
static sigjmp_buf jmpbuf;
static void message_receive_timeout(int signo);

//void msg_send( int sockfd_for_write, char *destination_canonical_ip_presentation_format, char *destination_port_number, char *message_to_be_sent, char* route_rediscovery_flag );
//void msg_recv( int sockfd_for_read,char *message_received,  char *source_canonical_ip_presentation_format, char  *source_port_number);
 char server_vm[HOSTNAME_LEN], client_vm[HOSTNAME_LEN];
 int                 sockfd;
 char message_to_be_sent[100] = "Request TIME from server";
 char template[100];
 char destination_canonical_ip_presentation_format[100];
char  route_rediscovery_flag[]="0";


int main(int argc, char **argv)
{
    int                 l;
    socklen_t           len;
    struct sockaddr_un  cliaddr, addr2, servaddr;
    struct hostent *hptr;
    char *ptr, **pptr;
    char message_received[MAXLINE];  
    char source_canonical_ip_presentation_format[100];
    char source_port_number[10];
    int fd;

    char message_to_be_sent[MAXLINE];
    char str;
    char  buf[100];
    time_t ticks;
    
  //  l=strlen(template);
    //printf("Previous length : %d\n", l );
    strcpy(template,"/tmp/fileXXXXXX");
    mkstemp(template);
    //l=strlen(template);
    //printf("New length : %d\n", l );
//template[l]=0;
    bzero(&cliaddr, sizeof(cliaddr));      
    cliaddr.sun_family = AF_LOCAL; 
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0); 
    strcpy(cliaddr.sun_path, template);
    printf("sun path %s :  SUN_LEN(&cliaddr) %d \n",cliaddr.sun_path , SUN_LEN(&cliaddr));   
    bind(sockfd, (SA *) &cliaddr, SUN_LEN(&cliaddr));
    len = sizeof(addr2);
    getsockname(sockfd, (SA *) &addr2, &len);
    printf("bound name = %s, returned len = %d servaddr.sun_path= %s \n", addr2.sun_path, len,template);


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
    signal(SIGALRM,message_receive_timeout);
    while(1)
    {
        requestClient:
        printf("Please select the server VM : vm1,vm2, ... vm10 :\n");
        scanf("%s",&server_vm);
    
        gethostname( client_vm, sizeof(client_vm) );
                 
        printf("Client at node %s sending request to server at  %s\n", client_vm, server_vm);
        retrieveDestinationCanonicalIpPresentationFormat(server_vm, destination_canonical_ip_presentation_format);      
         //   strcpy(message_to_be_sent,"trace message\n");
        strcpy(route_rediscovery_flag,"0");
        alarm(5);
        printf("after alarm set\n");
        msg_send( sockfd,  destination_canonical_ip_presentation_format, template,  message_to_be_sent, route_rediscovery_flag );
        
        printf("before recv\n");
        if(sigsetjmp(jmpbuf,1)!=0)
        {
            goto requestClient;
        }
        msg_recv( sockfd, message_received, source_canonical_ip_presentation_format, source_port_number);

        alarm(0);

        //retrieveHostName( source_canonical_ip_presentation_format, server_vm );
        ticks=time(NULL);
        memset(buf,0,sizeof(buf));
 
        snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
             printf("Time: %s\n",buf );
        printf("Client at node  %s received from %s at : %s\n", client_vm, server_vm, buf);
        /*if(msg_recv_timeout)
        {
            printf("Client at node  vm i1 : timeout on response from   vm i2\n");
            route_rediscovery_flag="1";
            msg_retransmit();
        }
        */
            
    }

    
    exit(0);
}
static void message_receive_timeout(int signo)
{
     
      printf("Client at node   %s  : timeout on response from   %s\n", client_vm, server_vm);
      printf("Retransmitting message with Forced Route Discovery\n");
      strcpy(route_rediscovery_flag,"1");
      msg_send( sockfd,  destination_canonical_ip_presentation_format, template,  message_to_be_sent, route_rediscovery_flag );
       siglongjmp(jmpbuf,1);
        return;
}

