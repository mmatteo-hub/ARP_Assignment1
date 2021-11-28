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

//time_t t;
sig_atomic_t sig;

int pidX_got, pidZ_got;
int fdX, fdZ;
char pid[80];
char format_string[80] = "%d";
char format_string_pid[80] = "%d,%d";

float x_position, z_position;
/*
void sig_handler(int signo)
{
    if (signo == SIGUSR1) printf("SIGUSR1 received!"); fflush(stdout);//sig = 0;
}
*/
int main(int argc, char * argv[])
{
    //t = time(NULL);
    //signal(SIGUSR1,sig_handler);
    //int fd_exec;
    
    char * watchdog_insp = "/tmp/watchdog_insp";
    char * watchdog_motZ= "/tmp/watchdog_motZ";

    char * pid_motX_watchdog = "/tmp/pid_motX_watch";

    mkfifo(watchdog_insp,0666);
    mkfifo(watchdog_motZ,0666);
    mkfifo(pid_motX_watchdog, 0666);

    char pid[80];

    int x_pid_w = open(pid_motX_watchdog, O_RDONLY);
    read(x_pid_w, pid, 80);
    pidX_got = atoi(pid);
    close(x_pid_w);
    //printf("pidX = %d\n", pidX_got); fflush(stdout);

    char pid_w[80];

    // writing watchdog pid
    int fd_watchdog = open(watchdog_insp,O_WRONLY);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);
    //printf(" WD = %d\n", (int)getpid()); fflush(stdout);

    fd_watchdog = open(watchdog_motZ,O_WRONLY | O_NONBLOCK);
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);

    int flag = 0;

    while(1)
    {
        do
        {   
            //sig = 1;
            flag = 1;
            sleep(secs);
        } while(!flag);

        printf("No signals received: reset incoming!\n"); fflush(stdout);

        kill(pidX_got,SIGUSR1);
        //kill(pidZ_got,SIGUSR1);
    }
}