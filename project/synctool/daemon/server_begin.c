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


#define MAXLINE 4096            /* max line length */

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
    char    buf[MAXLINE];

    vsnprintf(buf, MAXLINE, fmt, ap);
    if (errnoflag)
    {
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",strerror(error));
    }
    strcat(buf, "\n");
    fflush(stdout);     /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(NULL);       /* flushes all stdio output streams */
}


/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...)
{
    va_list     ap;

    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void err_msg(const char *fmt, ...)
{
    va_list     ap;

    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    // Get maximum number of file descriptors

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        err_quit("%s: can't get file limit", cmd);
    }

    // Become a session leader to lose controlling TTY.

    if ((pid = fork()) < 0)
    {
        err_quit("%s: can't fork", cmd);
    }
    else if (pid != 0) /* parent */
    {
        //err_msg("parent process exit\n");
        exit(0);
    }

    setsid();

    // Ensure future opens won't allocate controlling TTYs.

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        err_quit("%s: can't ignore SIGHUP");
    }
    if ((pid = fork()) < 0)
    {
        err_quit("%s: can't fork", cmd);
    }
    else if (pid != 0) /* parent */
    {
        //err_msg("parent process exit again\n");
        exit(0);
    }

    // Change the current working directory to the root so
    // we won't prevent file systems from being unmounted.

    if (chdir("/") < 0)
    {
        err_quit("%s: can't change directory to /");
    }

    // Close all open file descriptors.

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < (int)rl.rlim_max; i++)
        close(i);

    // Attach file descriptors 0, 1, and 2 to /dev/null.

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    //openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) 
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d",fd0, fd1, fd2);
        exit(1);
    }
}


#define LOCKFILE  (char*)"/var/run/synctool.pid"
#define LOCKDAEM  (char*)"/var/run/sync_daemon.pid"

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

#define LOGNAME  (char*)"/home/zhangxs/Desktop/log.txt"

int write_log(const char* log)
{
    int fd = open(LOGNAME,O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0)
    {
        /// 表示文件不存在
        /// 
        fd = open(LOGNAME,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        if (fd < 0)
        {
            return -1;
        }
    }
    lseek(fd,0,SEEK_END);
    write(fd,log,strlen(log));
    close(fd);
    return 0;
}
/*
int write_log_file(const char* filename,const char* log)
{
    int fd = open(filename,O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0)
    {
        /// 表示文件不存在
        /// 
        fd = open(filename,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        if (fd < 0)
        {
            return -1;
        }
    }
    else
    {
        remove(filename);
    }
    lseek(fd,0,SEEK_END);
    write(fd,log,strlen(log));
    close(fd);
    return 0;
}
*/
int record_pid(char* filename)
{
    int fd;
    int pid = (int)getpid();

    char buf[10];
    memset(buf,0,10);
    sprintf(buf,"%d\n",pid);
    //write_log(buf);

    fd = open(filename,O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd >= 0)
    {
        /// 表示文件存在 删除文件
        remove(filename);
        close(fd);
    }

    fd = open(filename, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) 
    {
        //printf("can't open %s: %s\n", LOCKFILE, strerror(errno));
        return -1;
    }/*
    if (lockfile(fd) < 0) 
    {
        if (errno == EACCES || errno == EAGAIN) 
        {
            close(fd);
            return 1;
        }
        printf("can't lock %s: %s\n", LOCKFILE, strerror(errno));
        return -2;
    }*/
    lseek(fd,0,SEEK_SET);
    write(fd,buf,strlen(buf));
    close(fd);
    return 0;
}

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

volatile sig_atomic_t _running = 1;

void sigterm_handle(int arg)
{
    _running = 0;
    remove(LOCKDAEM);
    int sync_pid;
    if(just_running(LOCKFILE,&sync_pid) == 0)
    {
        kill(sync_pid,SIGINT);
        remove(LOCKFILE);
    }
    exit(1);
}


int main(int argc, char *argv[])
{
    char* cmd;

    if ((cmd = strrchr(argv[0],'/')) == NULL)
    {
        cmd = argv[0];
    }
    else
        cmd++;

    printf("server_begin program name :%s\n",cmd);
    //remove(LOGNAME);

    const int   size = 256;
    const char* server_name = "synctool";
    char path[size];
    char server_path[size];
    memset(path,0,size);
    memset(server_path,0,size);
    getcwd(path,size);
    strcat(path,"/");
    memcpy(server_path,path,size);
    strcat(server_path,server_name);

    printf("server path : %s\n",path);

    /// 先判断守护进程是否已经运行
    if(just_running(LOCKDAEM,NULL) == 0)
    {
        printf("the sync_daemon is already running !\n");
        return 0;
    }
    daemonize(cmd);

    signal(SIGTERM,sigterm_handle);

    /// 记录守护进程pid

    if(record_pid(LOCKDAEM) != 0)
    {
        printf("sync_daemon write pid failed !\n");
        return 0;
    }

    while(_running)
    {
        pid_t pid;
        if((pid = fork()) < 0)
        {
            write_log("fork error\n");
        }
        else if(pid == 0)
        {
            if(record_pid(LOCKFILE) == 0)
            {
                if(execl(server_path,server_name,path,(char*)0) < 0)
                {
                    write_log(strerror(errno));
                    remove(LOCKFILE);
                }
                else
                {
                    write_log("start synctool server successed !\n");
                }
                write_log("kill server successed !\n");
                remove(LOCKFILE);
/*
                int sync_pid;
                if(just_running(LOCKFILE,&sync_pid) == 0)
                {
                    /// 表示synctool进程正在运行
                    
                    if(kill(sync_pid,SIGINT) == 0)
                    {
                        write_log("kill server successed !\n");
                    }
                    remove(LOCKFILE);
                }*/
            }
        }
        else
        {
            //sleep(10);
        }
    }
    return 0;
}

