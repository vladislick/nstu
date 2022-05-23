#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main(int argc , char *argv[])
{
	int sock, server_time;
	struct sockaddr_in6 server;
    time_t rawtime;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET6 , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	inet_pton(AF_INET6, "::1", &server.sin6_addr);
	server.sin6_family = AF_INET6;
	server.sin6_port = htons( 12445 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");

    //Receive a reply from the server
    if( recv(sock , server_reply , 2000 , 0) < 0)
    {
        puts("recv failed");
        return -1;
    }

    time(&rawtime);
    printf("Answer from server: %ss\nCurrent time: %ds\n", server_reply, rawtime);
    
    server_time = atoi(server_reply);
    memset(message, 0, 1000);
    sprintf(message, "%d", server_time - rawtime);

    //Send some data
    if( send(sock , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return -2;
    }

    printf("Message %s sent successfully\n", message);
	
	close(sock);
	return 0;
}