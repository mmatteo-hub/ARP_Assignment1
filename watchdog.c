#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#define secs 20
time_t t;

int pidX_got, pidZ_got;
int fdX, fdZ;
char pid[80];
char format_string[80] = "%d";
char format_string_pid[80] = "%d,%d";

float x_position, z_position;

void sig_handler(int signo)
{
    if (signo == SIGALRM) t = time(NULL);
}

int main(int argc, char * argv[])
{
    t = time(NULL);
    signal(SIGALRM,sig_handler);
    
    char * watchdog_insp = "/tmp/watchdog_insp";
    char * watchdog_motZ= "/tmp/watchdog_motZ";
    char * pid_motX_watchdog = "/tmp/pid_motX_watch";
    char * pid_motZ_watchdog = "/tmp/pid_motZ_watch";
    char * comm_wd = "/tmp/commd_wd";

    mkfifo(watchdog_insp,0666);
    mkfifo(watchdog_motZ,0666);
    mkfifo(pid_motX_watchdog, 0666);
    mkfifo(pid_motZ_watchdog, 0666);
    mkfifo(comm_wd,0666);

    char pid[80];

    int x_pid_w = open(pid_motX_watchdog, O_RDONLY);
    read(x_pid_w, pid, 80);
    pidX_got = atoi(pid);
    close(x_pid_w);

    int z_pid_w = open(pid_motZ_watchdog, O_RDONLY);
    read(z_pid_w, pid, 80);
    pidZ_got = atoi(pid);
    close(z_pid_w);

    char pid_w[80];

    // writing watchdog pid
    int fd_watchdog = open(watchdog_insp,O_WRONLY);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);

    int fd_wdComm = open(comm_wd,O_WRONLY);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_wdComm,pid_w,strlen(pid_w)+1);
    close(fd_wdComm);

    while(1)
    {   
        if(difftime(time(NULL),t) > secs)
        {
            printf("No signals received: reset incoming!\n"); fflush(stdout);

            kill(pidX_got,SIGUSR1);
            kill(pidZ_got,SIGUSR1);
            t = time(NULL);
        }
    }
}