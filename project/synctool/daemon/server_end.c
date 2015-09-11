#include <sys/types.h>     /* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>   /* for winsize */
#include <sys/ioctl.h>
#include <stdio.h>         /* for convenience */
#include <stdlib.h>        /* for convenience */
#include <stddef.h>        /* for offsetof */
#include <string.h>        /* for convenience */
#include <unistd.h>        /* for convenience */
#include <signal.h>        /* for SIG_ERR */
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <errno.h>         /* for definition of errno */
#include <stdarg.h>        /* ISO C variable aruments */

#define LOCKFILE  (char*)"/var/run/synctool.pid"
#define LOCKDAEM  (char*)"/var/run/sync_daemon.pid"

int just_running(char* filename,int* sync_pid)
{
    int fd;

    fd = open(filename, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) 
    {
        return -1;
    }
    if (sync_pid)
    {
        char buf[10];
        int tmp_id;
        memset(buf,0,10);
        read(fd, buf,10);
        tmp_id = atoi(buf);
        memcpy(sync_pid,&tmp_id,sizeof(int));
    }
    close(fd);
    return 0;
}

int main()
{
    int  syntool_id = 0;
    int  daemon_id = 0;

    if(just_running(LOCKDAEM,&daemon_id) == 0)
    {
        kill(daemon_id,SIGINT);
        remove(LOCKDAEM);
    }
    else
    {
        /// 守护进程没有运行
        /// 
        printf("syn_daemon server is not runing !\n");
    }

    if(just_running(LOCKFILE,&syntool_id) == 0)
    {
        kill(syntool_id,SIGINT);
        remove(LOCKFILE);
    }
    else
    {
        /// 服务器没有运行
        /// 
        printf("synctool server is not runing !\n");
    }
    return 0;
}

