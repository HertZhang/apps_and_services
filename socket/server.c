#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 1024

int main(int argc,char **argv)
{
	int server_fd, client_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char recvline[MAXLINE];
	char sendline[MAXLINE];
	int length;
    	
	memset(&server_addr, 0, sizeof(server_addr));
    	
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(10004);
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("create socket error: %s errno :%d\n", strerror(errno), errno);
		return -1;
	}
	
	if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind socket error: %s errno :%d\n", strerror(errno), errno);
                return -1;
	}
	
	listen(server_fd, 1024);
	
	printf("Please wait for the client information ...\n");
	int client_size = sizeof(struct sockaddr_in);
	
	while((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_size)) < 0);
	
	printf("accept client %s\n", inet_ntoa(client_addr.sin_addr));
	sprintf(sendline, "Welcome to server!\n");
	send(client_fd, sendline, strlen(sendline), 0);
	
	while(1)
	{
		length = recv(client_fd, recvline, MAXLINE, 0);
		recvline[length] = '\0';
		if(length > 0)
			printf("recv msg from client: %s", recvline);

		if(send(client_fd, recvline, length, 0) < 0)
		{
			printf("send socket error: %s errno :%d\n", strerror(errno), errno);
			return -1;
		}

		if(length > 0)
			printf("send msg to client:   %s", recvline);

		//scanf("%s", recvline);
                //if(!strcmp(recvline, "quit"))
                //        break;
	}
	
	close(client_fd);
	close(server_fd);

	return 0;
}

