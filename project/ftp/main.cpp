#include <iostream>
#include "ftp_manager.h"

using namespace std;

void* PortServerThread( void* pArg )
{

}

int main()
{
    //cout << "Hello world!" << endl;
    pthread_t threadid;
    if (pthread_create(&threadid,NULL,PortServerThread,NULL) != 0)
    {
        cout << "create work_thread failed.\n";
        return false;
    }

    CFTPManager  ftp;
    int errorcode = ftp.LoginServer("192.168.245.1",21,"admin","123456");
    if(errorcode < 0)
    {
        cout << "LoginServer failed" << endl;
        return 0;
    }
    std::string pwd = ftp.PWDCommand();
    ftp.SetBinaryMode();
    ftp.PortMode();
    ftp.List(pwd);


    return 0;
}
