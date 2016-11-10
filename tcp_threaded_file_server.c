/* TCP server to find the square root of the given number */

#include <stdio.h>
#define _USE_BSD 1
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>


#define MAX_SEND_BUF 1000
#define MAX_RECV_BUF 1000
#define MAX_DATA 1000

extern int errno;

int errexit(const char *format,...);

int connectTCP(const char *service,int portnum);
int connectsock(const char *service,int portnum,const char *transport);
void handler(int);

/*------------------------------------------------------------------------------------
 * connectsock-Allocate and connect socket for TCP
 *------------------------------------------------------------------------------------
*/

int connectsock(const char *service,int portnum,const char *transport)
{
/*
Arguments:
*service   - service associated with desired port
*transport - name of the transport protocol to use
*/
struct sockaddr_in server;                                                //an internet endpoint address

int server_socket,type,b,l,accept_socket,num;                             //two socket descriptors for listening and accepting 

memset(&server,0,sizeof(server));

server.sin_addr.s_addr=htons(INADDR_ANY);                                 //INADDR_ANY to match any IP address
server.sin_family=AF_INET;                                                //family name
server.sin_port=htons(portnum);                                              //port number

 
/*
 * to determine the type of socket
 */

if(strcmp(transport,"udp")==0)
{
type=SOCK_DGRAM;
}
else
{
type=SOCK_STREAM;
}


server_socket=socket(AF_INET,type,0);                                    //allocate a socket

if(server_socket<0)
{
printf("Socket can't be created\n");
exit(0);
}

/* to set the socket options- to reuse the given port multiple times */
num=1;

if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEPORT,(const char*)&num,sizeof(num))<0)
{
printf("setsockopt(SO_REUSEPORT) failed\n");
exit(0);
}


/* bind the socket to known port */
b=bind(server_socket,(struct sockaddr*)&server,sizeof(server));

if(b<0)
{
printf("Error in binding\n");
exit(0);
}


/* place the socket in passive mode and make the server ready to accept the requests and also 
   specify the max no. of connections
 */
l=listen(server_socket,10);
if(l<0)
{
printf("Error in listening\n");
exit(0);
}

return server_socket;

}



/*------------------------------------------------------------------------
 * connectTCP-connect to a specified TCP service on specified host
 -------------------------------------------------------------------------*/
int connectTCP(const char *service,int portnum)
{
/*
 Arguments:
 *service-service associated with desired port
 */
 return connectsock(service,portnum,"tcp");
}




int errexit(const char* format,...)
{
va_list args;

va_start(args,format);
vfprintf(stderr,format,args);
va_end(args);
exit(1);
}


/* function for handling connection with client by thread*/

void *handle(void *ssock)
{
         int sock=*(int*)ssock;
		char msg[1000];
         int data_len;
	
	char send_buf[MAX_SEND_BUF];



		
			data_len = recv(sock,msg,MAX_DATA,0);                                      //recieve the file name from client
			
			
			if(data_len)
			{
				printf("Connected to multithreaded connection oriented server\n");
				printf("File name recieved: %s\n", msg);
				
			}
						
			
			int file;                                                                  //for reading local file(server file)
			if((file = open(msg,O_RDONLY))<0)
			{       
				
				printf("File not found\n");
                                printf("Client disconnected\n");
				
			}
			else
			{	
				
				printf("File opened successfully\n");
					
						
		

				
				
				ssize_t read_bytes;
 				ssize_t sent_bytes;
				 
				char send_buf[MAX_SEND_BUF]; 
				
	
				 //printf("Sending file: %s\n", data);
				 while( (read_bytes = read(file, send_buf, MAX_RECV_BUF)) > 0 )      //read the contents of file
				 {
					 printf("%s",send_buf);
					 if( (sent_bytes = send(sock, send_buf, read_bytes, 0)) < read_bytes )   //send the data back to client
					 {
					 printf("send error");
					 
					 }
					 
				 }
				 close(file);
				printf("\nClient disconnected\n");                                                         //file closed			 
			}

         close(sock);                                                                                 //connected socket closed
         
    
}

/*
Main function for connection oriented multithreaded server
 */

int main(char argc,char *argv[])
{

	char *service="echo";
        int alen;
        int portnum=atoi(argv[1]);
        pthread_t thread;

	int msock,ssock;
	

	/* call connectTCP to create a socket, bind it and place it in passive mode
	   once the call returns call accept on listening socket to accept the incoming requests
	 */

	msock=connectTCP(service,portnum);
        printf("Listening to client\n");
	


	while(1)
	{
            struct sockaddr_in fsin;
	    alen=sizeof(struct sockaddr_in);
	    ssock=accept(msock,(struct sockaddr*)&fsin,&alen);                             //continuosly calls accept to recieve the new connection

	    
           

	    if(ssock<0)
		{
		
                   errexit("Accept: %s\n",strerror(errno));
		}

	     

	     
               int *thread_arg=malloc(sizeof(int));
                *thread_arg=ssock;
	         if(pthread_create(&thread,NULL,handle,thread_arg)!=0)                     //thread created
                    {
                       printf("error in creating the thread\n");
                       return 1;
                     }
               
	}
        
           
	close(msock);                                                                      //closing of parent socket
	return 0;

}












