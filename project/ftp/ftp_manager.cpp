#include "ftp_manager.h"

CFTPManager::CFTPManager()
{
    //ctor
    m_socket = -1;
}

CFTPManager::~CFTPManager()
{
    //dtor
}

int CFTPManager::LoginServer(const std::string& serverip,unsigned int port,const std::string& username ,const std::string& passwd)
{
    m_serverip = serverip;
    m_port = port;
    m_username = username;
    m_passwd = passwd;

    m_socket = socket(AF_INET,SOCK_STREAM,0);
    if(m_socket < 0)
    {
        printf("create socket failed.\n");
        return -1;
    }
    if(Connect(m_socket,m_serverip,m_port) < 0)
    {
        return -1;
    }
    std::string str_commandLine = ParseCommand(FTP_COMMAND_USERNAME,username);
    if(Send(m_socket,str_commandLine) < 0)
    {
        return -1;
    }
    printf("%s\n",ServerResponse(m_socket).c_str());
    str_commandLine = ParseCommand(FTP_COMMAND_PASSWORD,passwd);
    if(Send(m_socket,str_commandLine) < 0)
    {
        return -1;
    }
    printf("%s\n",ServerResponse(m_socket).c_str());
    return 0;
}

int CFTPManager::QuitServer()
{
    if(m_socket >= 0)
    {
        close(m_socket);
    }
    return 0;
}

 std::string  CFTPManager::PWDCommand()
 {
	std::string strCmdLine = ParseCommand(FTP_COMMAND_CURRENT_PATH, "");
	if (Send(m_socket, strCmdLine) < 0)
	{
		return "";
	}
	else
	{
		return ServerResponse(m_socket);
	}
	return "";
 }

 int CFTPManager::PortMode()
 {
 	std::string strCmdLine = ParseCommand(FTP_COMMAND_PORT_MODE, "192,168,245,128,23,115");
 	printf("%s\n",strCmdLine.c_str());
	if (Send(m_socket, strCmdLine) < 0)
	{
	    printf("%s\n",ServerResponse(m_socket).c_str());
		return -1;
	}
	else
	{
		printf("%s\n",ServerResponse(m_socket).c_str());
		return 0;
	}
	return -1;
 }

 int CFTPManager::SetBinaryMode()
 {
  	std::string strCmdLine = ParseCommand(FTP_COMMAND_TYPE_MODE,"I");
  	printf("%s\n",strCmdLine.c_str());
	if (Send(m_socket, strCmdLine) < 0)
	{
	    printf("%s\n",ServerResponse(m_socket).c_str());
		return -1;
	}
	else
	{
		printf("%s\n",ServerResponse(m_socket).c_str());
		return 0;
	}
	return -1;
 }

std::string  CFTPManager::List(const std::string& path)
{
/*
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
	    return "";
	}

    if(listen(dataSocket,10) == -1)
	{
	    return "";
	}
    while(1)
	{
	    unsigned int sin_size = sizeof(struct sockaddr_in);
        int new_fd;
        if((new_fd == accept(dataSocket,(struct sockaddr*)&clientaddr,&sin_size)) == -1)
        {
            return "";
        }

	}

*/

	std::string strCmdLine = ParseCommand(FTP_COMMAND_DIR,"/");
    printf("%s\n",strCmdLine.c_str());
	if (Send(m_socket,strCmdLine) < 0)
	{
		//printf("@@@@Response: %s\n", ServerResponse(m_socket).c_str());
		//close(dataSocket);
		return "";
	}
	else
	{
	    std::string m_strResponse = ServerResponse(m_socket);
		printf("%s\n", m_strResponse.c_str());

		//m_strResponse = ServerResponse(dataSocket);
		//printf("@@@@Response: \n%s\n", m_strResponse.c_str());
		//close(dataSocket);

		//return m_strResponse;
	}
    return "";
}





int CFTPManager::Connect(int socketfd, const std::string &serverIP, unsigned int nPort)
{
	if (socketfd == INVALID_SOCKET)
	{
		return -1;
	}

	unsigned int argp = 1;
	int error = -1;
	int len = sizeof(int);
	struct sockaddr_in  addr;
	bool ret = false;
	timeval stime;
	fd_set  set;

	ioctl(socketfd, FIONBIO, &argp);

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port	= htons(nPort);
	addr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	bzero(&(addr.sin_zero), 8);

	printf("Address: %s %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	if (connect(socketfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1)
	{
		stime.tv_sec = 20;
		stime.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(socketfd, &set);

		if (select(socketfd + 1, NULL, &set, NULL, &stime) > 0)
		{
			getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len);
			if (error == 0)
			{
				ret = true;
			}
			else
			{
				ret = false;
			}
		}
	}
	else
	{	printf("Connect Immediately!!!\n");
		ret = true;
	}

	argp = 0;
	ioctl(socketfd, FIONBIO, &argp);

	if (!ret)
	{
		close(socketfd);
		fprintf(stderr, "cannot connect server!!\n");
		return -1;
	}

	//fprintf(stdout, "Connect!!!\n");

	return 0;
}


int CFTPManager::Send(int fd, const std::string &cmd)
{
	if (fd == INVALID_SOCKET)
	{
		return -1;
	}

	return Send(fd, cmd.c_str(), cmd.length());
}

int CFTPManager::Send(int fd, const char *cmd, const size_t len)
{
/*
	if((FTP_COMMAND_USERNAME != m_nCurrentCommand)
		&&(FTP_COMMAND_PASSWORD != m_nCurrentCommand)
		&&(!m_bLogin))
	{
		return -1;
	}*/

	timeval timeout;
	timeout.tv_sec  = 1;
	timeout.tv_usec = 0;

	fd_set  writefd;
	FD_ZERO(&writefd);
	FD_SET(fd, &writefd);

	if(select(fd + 1, 0, &writefd , 0 , &timeout) > 0)
	{
		size_t nlen  = len;
		int nSendLen = 0;
		while (nlen >0)
		{
			nSendLen = send(fd, cmd , (int)nlen , 0);

			if(nSendLen == -1)
				return -2;

			nlen = nlen - nSendLen;
			cmd +=  nSendLen;
		}
		return 0;
	}
	return -1;
}



std::string CFTPManager::ParseCommand(const unsigned int command, const std::string &strParam)
{
	if (command < FTP_COMMAND_BASE || command > FTP_COMMAND_END)
	{
		return "";
	}

	std::string strCommandLine;

	//m_nCurrentCommand = command;
	std::string commandstr;

	switch (command)
	{
	case FTP_COMMAND_USERNAME:
		strCommandLine = "USER ";
		break;
	case FTP_COMMAND_PASSWORD:
		strCommandLine = "PASS ";
		break;
	case FTP_COMMAND_QUIT:
		strCommandLine = "QUIT ";
		break;
	case FTP_COMMAND_CURRENT_PATH:
		strCommandLine = "PWD ";
		break;
	case FTP_COMMAND_TYPE_MODE:
		strCommandLine = "TYPE ";
		break;
	case FTP_COMMAND_PORT_MODE:
		strCommandLine = "PORT ";
		break;
	case FTP_COMMAND_DIR:
		strCommandLine = "LIST ";
		break;
	case FTP_COMMAND_CHANGE_DIRECTORY:
		strCommandLine = "CWD ";
		break;
	case FTP_COMMAND_DELETE_FILE:
		strCommandLine = "DELE ";
		break;
	case FTP_COMMAND_DELETE_DIRECTORY:
		strCommandLine = "RMD ";
		break;
	case FTP_COMMAND_CREATE_DIRECTORY:
		strCommandLine = "MKD ";
		break;
	case FTP_COMMAND_RENAME_BEGIN:
		strCommandLine = "RNFR ";
		break;
	case FTP_COMMAND_RENAME_END:
		strCommandLine = "RNTO ";
		break;
	case FTP_COMMAND_FILE_SIZE:
		strCommandLine = "SIZE ";
		break;
	case FTP_COMMAND_DOWNLOAD_FILE:
		strCommandLine = "RETR ";
		break;
	case FTP_COMMAND_DOWNLOAD_POS:
		strCommandLine = "REST ";
		break;
	case FTP_COMMAND_UPLOAD_FILE:
		strCommandLine = "STOR ";
		break;
	case FTP_COMMAND_APPEND_FILE:
		strCommandLine = "APPE ";
		break;
	default :
		break;
	}

	strCommandLine += strParam;
	strCommandLine += "\r\n";

	commandstr = strCommandLine;
	//trace("parseCommand: %s\n", m_commandStr.c_str());

	return commandstr;
}


std::string CFTPManager::ServerResponse(int sockfd)
{
	if (sockfd == INVALID_SOCKET)
	{
		return "";
	}

	int nRet = -1;
	char buf[MAX_PATH] = {0};

	std::string m_strResponse;

	while ((nRet = GetData(sockfd, buf, MAX_PATH)) > 0)
	{
		buf[MAX_PATH - 1] = '\0';
		m_strResponse += buf;
	}

	return m_strResponse;
}

int CFTPManager::GetData(int fd, char *strBuf, unsigned long length)
{
	assert(strBuf != NULL);

	if (fd == INVALID_SOCKET)
	{
		return -1;
	}

	memset(strBuf, 0, length);
	timeval stime;
	int nLen;

	stime.tv_sec = 1;
	stime.tv_usec = 0;

	fd_set	readfd;
	FD_ZERO( &readfd );
	FD_SET(fd, &readfd );

	if (select(fd + 1, &readfd, 0, 0, &stime) > 0)
	{
		if ((nLen = recv(fd, strBuf, length, 0)) > 0)
		{
			return nLen;
		}
		else
		{
			return -2;
		}
	}
	return 0;
}













