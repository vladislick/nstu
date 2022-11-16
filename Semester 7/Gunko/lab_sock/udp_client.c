// ������ ������� UDP �������
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// ������� ��������� ������
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, n; // �����������
   unsigned int length; // ������ ��������� ������
   struct sockaddr_in server, from; // ��������� ������� ������� � ������� �������������
   struct hostent *hp; // ��������� �����
   char buffer[256]; // �����
   
   if (argc != 3) { printf("Usage: server port\n");
                    exit(1);
   }
	// ��� 1 - �������� ������
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");
	
	// ��� 2 - ����������
	server.sin_family = AF_INET; // �������� ���������
	hp = gethostbyname(argv[1]); // ���������� �����
	if (hp==0) error("Unknown host");
	bcopy((char *)hp->h_addr, 
		(char *)&server.sin_addr,
		 hp->h_length);
	server.sin_port = htons(atoi(argv[2])); // ���������� �����
	length=sizeof(struct sockaddr_in);
	
	// ��� 3 - ����� � �������� ���������
	while(1)
	{

		// �������� ����������
		printf("\nS<=C:");
		bzero(buffer,256);
		fgets(&buffer[0], sizeof(buffer) - 1, stdin);
		//fgets(buffer,255,stdin);
		// �������� �� quit
		if (!strcmp(&buffer[0], "quit\n")) break;
		
		n=sendto(sock,&buffer[0],strlen(buffer),0,(const struct sockaddr *)&server,length);
		if (n < 0) error("Sendto");
		
		// ��������� sockaddr_in � �� ������
        	struct sockaddr_in server_addr;
       		int server_addr_size = sizeof(server_addr);
		
		// ����� ����������
		//n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
		int n = recvfrom(sock, &buffer[0], sizeof(buffer) - 1, 0,(struct sockaddr *)&server_addr, &server_addr_size);
		if (n < 0) error("recvfrom");
		/*write(1,"Got an ack: ",12);
		write(1,buffer,n);*/
		// ����� ��������� � ������� ��������� �� �����
        	printf("\nS=>C:%s", &buffer[0]);
	}
	// ��� 4 - �������� ������
	close(sock);
	return 0;
}
