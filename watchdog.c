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
char pids[80];
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

    char * fifo_watchdog = "/tmp/fifo_watchdog";
    
    char * watchdog_motX = "/tmp/watchdog_motX";
    char * watchdog_motZ= "/tmp/watchdog_motZ";

    mkfifo(watchdog_motX,0666);
    mkfifo(watchdog_motZ,0666);

    char pid_w[80];

    // getting watchdog pid
    int fd_watchdog = open(watchdog_motX,O_WRONLY | O_NONBLOCK);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);

    fd_watchdog = open(watchdog_motZ,O_WRONLY | O_NONBLOCK);
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);

    int fd_pid = open(fifo_watchdog, O_RDONLY);
    read(fd_pid, pids, 80);
    sscanf(pids, format_string_pid, &pidX_got, &pidZ_got);
    close(fd_pid);
    printf("%d\n", pidX_got); fflush(stdout);


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