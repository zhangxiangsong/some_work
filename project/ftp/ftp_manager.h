#ifndef CFTPMANAGER_H
#define CFTPMANAGER_H

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
#include <string>
#include <list>

#define INVALID_SOCKET    -1
#define MAX_PATH					260
#define trace						printf

#define FTP_PARAM_BASE
#define FTP_DEFAULT_PORT			"21"
#define FTP_DEFAULT_BUFFER			1024*4
#define FTP_DEFAULT_PATH			"/mnt/"

#define FTP_COMMAND_BASE			           1000
#define FTP_COMMAND_END				           FTP_COMMAND_BASE + 30
#define FTP_COMMAND_USERNAME		           FTP_COMMAND_BASE + 1
#define FTP_COMMAND_PASSWORD		           FTP_COMMAND_BASE + 2
#define FTP_COMMAND_QUIT			           FTP_COMMAND_BASE + 3
#define FTP_COMMAND_CURRENT_PATH	           FTP_COMMAND_BASE + 4
#define FTP_COMMAND_TYPE_MODE		           FTP_COMMAND_BASE + 5
#define FTP_COMMAND_PORT_MODE		           FTP_COMMAND_BASE + 6
#define FTP_COMMAND_DIR				           FTP_COMMAND_BASE + 7
#define FTP_COMMAND_CHANGE_DIRECTORY           FTP_COMMAND_BASE + 8
#define FTP_COMMAND_DELETE_FILE		           FTP_COMMAND_BASE + 9
#define FTP_COMMAND_DELETE_DIRECTORY           FTP_COMMAND_BASE + 10
#define FTP_COMMAND_CREATE_DIRECTORY           FTP_COMMAND_BASE + 11
#define FTP_COMMAND_RENAME_BEGIN               FTP_COMMAND_BASE  +12
#define FTP_COMMAND_RENAME_END                 FTP_COMMAND_BASE + 13
#define FTP_COMMAND_FILE_SIZE		           FTP_COMMAND_BASE + 14
#define FTP_COMMAND_DOWNLOAD_POS	           FTP_COMMAND_BASE + 15
#define FTP_COMMAND_DOWNLOAD_FILE	           FTP_COMMAND_BASE + 16
#define FTP_COMMAND_UPLOAD_FILE		           FTP_COMMAND_BASE + 17
#define FTP_COMMAND_APPEND_FILE		           FTP_COMMAND_BASE + 18

class CFTPManager
{
    public:
    	//enum type {binary = 0x31,ascii};

        CFTPManager();
        ~CFTPManager();

        int LoginServer(const std::string &  serverip,unsigned int port,const std::string & username ,const std::string & passwd);

        int QuitServer();

        /// get the server current path

        std::string PWDCommand();

        /// port translation mode

        int PortMode();

        int SetBinaryMode();

        std::string  List(const std::string& path);



    protected:

        int Connect(int socketfd, const std::string &serverIP, unsigned int nPort);

        int Send(int fd, const std::string &cmd);

        int Send(int fd, const char *cmd, const size_t len);

        std::string ParseCommand(const unsigned int command, const std::string &strParam);

        std::string ServerResponse(int sockfd);

        int GetData(int fd, char *strBuf, unsigned long length);


    private:

    int m_socket;

    std::string   m_serverip;
    unsigned int  m_port;
    std::string   m_username;
    std::string   m_passwd;
};

#endif // CFTPMANAGER_H







