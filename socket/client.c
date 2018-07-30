#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024

int main(int argc,char **argv)
{
	char *servInetAddr = "127.0.0.1";
	int client_fd;
	struct sockaddr_in server_addr;
	char  recvline[MAXLINE], sendline[MAXLINE];
	int length = 0;
	unsigned int count = 0, usleep_time = 1000000;
    
	if(argc != 3)
	{
		printf("Usage:   ./client <ipaddress> <usleep>\n");
		printf("Example: ./client 192.168.10.2 1000000\n");
		exit(0);
	}
	
	servInetAddr = argv[1];
	printf("ipaddress: %s\n", servInetAddr);
	usleep_time = atoi(argv[2]);
	printf("usleep: %d\n", usleep_time);
	
	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("connect error %s errno: %d\n", strerror(errno), errno);
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(10004);
	inet_pton(AF_INET, servInetAddr, &server_addr.sin_addr);
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
	if((connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
	{
		printf("connect error %s errno: %d\n", strerror(errno), errno);
		exit(0);
	}

	printf("connecting to server ...\n");
	length = recv(client_fd, recvline, MAXLINE, 0);
	recvline[length] = '\0';
	printf("%s\n", recvline);

	printf("please input a string:\n");
	fgets(sendline, 1024, stdin);

	if((send(client_fd, sendline, strlen(sendline),0)) < 0)
        {
        	printf("send mes error: %s errno : %d\n", strerror(errno), errno);
        	exit(0);
        }
	length = recv(client_fd, recvline, MAXLINE, 0);
        recvline[length] = '\0';
        printf("recv msg from server: %s\n", recvline);

	while(1) {
		usleep(usleep_time);
		memset(sendline, 0, MAXLINE);
		sprintf(sendline, "count=%d\n", count);
		
		if((send(client_fd, sendline, strlen(sendline),0)) < 0)
		{
			printf("send mes error: %s errno : %d\n", strerror(errno), errno);
			exit(0);
		}
		printf("send msg to server:   count=%d\n", count);
		
		length = recv(client_fd, recvline, MAXLINE, 0);
		recvline[length] = '\0';
		printf("recv msg from server: %s", recvline);
		
		count ++;

		//scanf("%s", recvline);
                //if(!strcmp(recvline, "quit"))
                //        break;
	}

	close(client_fd);
    	printf("exit\n");
	exit(0);
}


