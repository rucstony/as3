#include  "unp.h"

void msg_send( int sockfd_for_write, char *destination_canonical_ip_presentation_format, 
                char  *destination_port_number, char *message_to_be_sent, char *route_rediscovery_flag )
{
    // this  function will write this info in a single char format to sockfd_for_write
    char output_to_sock[MAXLINE];
    sprintf(output_to_sock,"%s|%s|%s|%s", destination_canonical_ip_presentation_format,destination_port_number,message_to_be_sent,route_rediscovery_flag);
    printf("%s\n", output_to_sock);
    write(sockfd_for_write,output_to_sock,sizeof(output_to_sock));

}

void msg_recv( int sockfd_for_read,char *message_received,  char *source_canonical_ip_presentation_format, 
                char  *source_port_number)
{
    // this  function will write this info in a single char format to sockfd_for_write
    char str_from_sock[MAXLINE];
    char *msg_fields[MAXLINE];
    int i=0,j;
    //sprintf(str_from_sock,"%s|%s|%s|%s", destination_canonical_ip_presentation_format,destination_port_number,message_to_be_sent,route_rediscovery_flag);
    //strcpy(str_from_sock,"5|mesg|170.99.12.3|78888\n");
    fgets(sockfd_for_read,str_from_sock,MAXLINE);
    printf("%s\n", str_from_sock);
  //   write(sockfd_for_read,output_to_sock,MAXLINE);
  //  msg_fields[i]=data_stream.split("|");
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
    
     
     
     message_received=msg_fields[1];
     source_canonical_ip_presentation_format=msg_fields[2];
     source_port_number=atoi(msg_fields[3]);
    
     return atoi(msg_fields[0]); //port to read from


}
