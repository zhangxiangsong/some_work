#include <sys/types.h>	   /* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>   /* for winsize */
#include <sys/ioctl.h>
#include <stdio.h>		   /* for convenience */
#include <stdlib.h>		   /* for convenience */
#include <stddef.h>		   /* for offsetof */
#include <string.h>		   /* for convenience */
#include <unistd.h>		   /* for convenience */
#include <signal.h>		   /* for SIG_ERR */
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <errno.h>		   /* for definition of errno */
#include <stdarg.h>		   /* ISO C variable aruments */

#define	MAXLINE	4096			/* max line length */

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE, fmt, ap);
	if (errnoflag)
	{
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",strerror(error));
	}
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}


/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...)
{
	va_list		ap;

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
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}

void daemonize(const char *cmd)
{
	int	i, fd0, fd1, fd2;
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
		err_msg("parent process exit\n");
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
		err_msg("parent process exit again\n");
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

int lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}


#define LOCKFILE "/var/run/synctools.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

extern int lockfile(int);

int already_running(void)
{
	int		fd;
	char	buf[16];

	fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0) 
	{
		syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}
	if (lockfile(fd) < 0) 
	{
		if (errno == EACCES || errno == EAGAIN) 
		{
			close(fd);
			return(1);
		}
		syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);
	return(0);
}

/*-------------------------------------------------------------*/

volatile sig_atomic_t _running = 1;

void sigterm_handle(int arg)
{
	_running = 0;
}

int main(int argc, char *argv[])
{
	//pthread_t tid;
	char* cmd;
	//struct sigaction sa;

	if ((cmd = strrchr(argv[0],'/')) == NULL)
	{
		cmd = argv[0];
	}
	else
		cmd++;

	printf("cmd:%s\n",cmd);

	signal(SIGTERM,sigterm_handle);

	//syslog(LOG_ERR,"check deamon is already running!\n");
	//if (already_running())
	//{
	//	syslog(LOG_ERR,"deamon already running");
	//	return 0;
	//}

	daemonize(cmd);

	const char* str = "hello,world!\n";

	while(1)
	{
		int fd = open("/root/Desktop/test.txt",O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);
		if (fd < 0)
		{
			/// 表示文件不存在
			fd = open("/root/Desktop/test.txt",O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
			if (fd < 0)
			{
				syslog(LOG_ERR,"open test.txt failed (error info :%s).\n",strerror(errno));
				return 0;
			}
		}
		lseek(fd,0,SEEK_END);
		write(fd,str,strlen(str));
		close(fd);
		sleep(4);
	}

	return 0;
}























