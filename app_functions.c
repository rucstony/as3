#include  "unp.h"

/*
  This function will write this info in a single char format to sockfd_for_write.
*/
void msg_send( int sockfd_for_write, 
               char *destination_canonical_ip_presentation_format, 
               char  *destination_port_number, 
               char *message_to_be_sent, 
               char *route_rediscovery_flag )
{
    char output_to_sock[MAXLINE];
    sprintf(output_to_sock,"%s|%s|%s|%s", destination_canonical_ip_presentation_format, 
                                          destination_port_number,
                                          message_to_be_sent,
                                          route_rediscovery_flag);
    printf("%s\n", output_to_sock);
    write(sockfd_for_write,output_to_sock,sizeof(output_to_sock));
}

/*
 This function will write this info in a single char format to sockfd_for_write
*/
int msg_recv( int sockfd_for_read,char *message_received,
               char *source_canonical_ip_presentation_format, 
               char  *source_port_number )
{
    char str_from_sock[MAXLINE];
    char *msg_fields[MAXLINE];
    int i=0,j;
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


void retrieveDestinationCanonicalIpPresentationFormat(const char *server_vm, char *destination_canonical_ip_presentation_format)
{
  struct hostent *hptr;
  char *ptr, **pptr;
  if((hptr=gethostbyname(server_vm))==NULL)
  {
    err_msg("gethostbyname error for host: %s : %s",server_vm,hstrerror(h_errno));
    return ;
  }
  printf("gethostbyname worked! \n");
        //for(pptr=hptr->h_aliases;*pptr!=NULL;pptr++)
        //    printf("\talias: %s\n",*pptr);
  if(hptr->h_addrtype==NULL)
  {
    fprintf(stderr,"Invalid IP address\n");
    return ;
  }
  printf("Address type: ....%ld\n",hptr->h_addrtype);
        //  printf("\talias: %s\n",hptr);

  switch(hptr->h_addrtype)
  {
    case AF_INET:
    printf("AF_INET type");

    pptr=hptr->h_addr_list;
    if(pptr!=NULL)
    {
      inet_ntop(hptr->h_addrtype,*pptr,destination_canonical_ip_presentation_format,100);
      printf("destination_canonical_ip_presentation_format: %s\n", destination_canonical_ip_presentation_format);
      //printf("%s\n", inet_ntop(hptr->h_addrtype,*pptr,destination_canonical_ip_presentation_format,100) );
    }
    break;

    default:
    fprintf(stderr,"unknown address type\n");
    return ;
    break;
  }
  //return destination_canonical_ip_presentation_format;
}

void retrieveHostName( const char *address , char * h_name)
{
  struct hostent     *hptr;
  in_addr_t           ipAdd;

  ipAdd =  inet_addr( address );

  if( (hptr = gethostbyaddr( (char*) &ipAdd, sizeof( ipAdd ), AF_INET )) == NULL )
  {
    printf("Address is invalid..\n");
    err_msg("gethostbyaddr error for host: %s: %s",
              address, hstrerror(h_errno));
    return;
  }
   h_name = hptr->h_name;
  printf("Client Hostname : `%s`\n",h_name);
 
  //return hptr->h_name;
 } 