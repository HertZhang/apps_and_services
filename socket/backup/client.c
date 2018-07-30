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
	int socketfd;
	struct sockaddr_in sockaddr;
	char  recvline[MAXLINE], sendline[MAXLINE];
	int n;
	unsigned int count = 0, usleep_time = 1000000;
    
	if(argc != 3)
	{
		printf("client <ipaddress> <usleep>\n");
		exit(0);
	}
	
	servInetAddr = argv[1];
	printf("ipaddress: %s\n", servInetAddr);
	usleep_time = atoi(argv[2]);
	printf("usleep: %d\n", usleep_time);

	socketfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(10004);
	inet_pton(AF_INET,servInetAddr,&sockaddr.sin_addr);
    
	if((connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))) < 0)
	{
		printf("connect error %s errno: %d\n",strerror(errno),errno);
		exit(0);
	}

	printf("send message to server\n");

	fgets(sendline,1024,stdin);

	if((send(socketfd,sendline,strlen(sendline),0)) < 0)
        {
        	printf("send mes error: %s errno : %d\n",strerror(errno),errno);
        	exit(0);
        }	

	while(1) {
		usleep(usleep_time);
		memset(sendline, 0, MAXLINE);
		//memset(tmp, 0, strlen(tmp));
		//itoa(count, tmp, 10);
		sprintf(sendline, "count=%d\n", count);
		count ++;

    		socketfd = socket(AF_INET,SOCK_STREAM,0);
	        memset(&sockaddr,0,sizeof(sockaddr));
	        sockaddr.sin_family = AF_INET;
	        sockaddr.sin_port = htons(10004);
	        inet_pton(AF_INET,servInetAddr,&sockaddr.sin_addr);
	
	        if((connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))) < 0)
	        {
	                printf("connect error %s errno: %d\n",strerror(errno),errno);
	                exit(0);
	        }

		if((send(socketfd,sendline,strlen(sendline),0)) < 0)
		{
			printf("send mes error: %s errno : %d\n",strerror(errno),errno);
			exit(0);
		}
		
		printf("send msg to server: count=%d\n", count);
	}

	close(socketfd);
    	printf("exit\n");
	exit(0);
}


