#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include <iostream>
#include <string>


int CreatePortServer(const std::string& address,const int port)
{
	int dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in  addr ,clientaddr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	memset(&clientaddr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port	= htons(23*256+115);
	addr.sin_addr.s_addr = inet_addr("192.168.1.130");
	bzero(&(addr.sin_zero), 8);

	if(bind(dataSocket,(struct sockaddr*)&addr,sizeof(struct sockaddr)) == -1)
	{
	    return 0;
	}

    if(listen(dataSocket,10) == -1)
	{
	    return 0;
	}

    unsigned int sin_size = sizeof(struct sockaddr_in);
    int new_fd = -1;

    while(1)
	{
	    if((new_fd = accept(dataSocket,(struct sockaddr*)&clientaddr,&sin_size)) == -1)
        {
            return 0;
        }

        char buffer[200];
        memset(buffer,0,200);
        int recvsize = recv(new_fd,buffer,200,0);
        if (recvsize == -1)
        {
            printf(" fd : %d  ,  error : %s\n",new_fd,strerror(errno));
        }
        printf("client successed : %d   %s.\n",recvsize,buffer);
        return 0;
	}
	return 0;
}
