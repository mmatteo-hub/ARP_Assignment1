#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#define secs 60
time_t t;

FILE *f;
time_t clk;

// definintion for all variables used inside the program
int pidX_got, pidZ_got;
int fdX, fdZ;
char pid[80];
float x_position, z_position;
char pid[80];
int x_pid_w;
int z_pid_w;
char pid_w[80];
int fd_watchdog;
int fd_wdComm;

// defining format string to prepare the string for the pipe
char format_string[80] = "%d";
char format_string_pid[80] = "%d,%d";

void sig_handler(int signo)
{
    f = fopen("./../log/logfile.txt","a");
    if (signo == SIGALRM)
    {
        clk = time(NULL);
        fprintf(f,"Watchdog, ACTION signal (SIGALRM) received at : %s",ctime(&clk));
        fflush(f);
        t = time(NULL);
    }
}

int main(int argc, char * argv[])
{
    f = fopen("./../log/logfile.txt","a");

    t = time(NULL);
    signal(SIGALRM,sig_handler);
    
    char * watchdog_insp = "/tmp/watchdog_insp";
    char * watchdog_motZ= "/tmp/watchdog_motZ";
    char * pid_motX_watchdog = "/tmp/pid_motX_watch";
    char * pid_motZ_watchdog = "/tmp/pid_motZ_watch";
    char * comm_wd = "/tmp/commd_wd";

    x_pid_w = open(pid_motX_watchdog, O_RDONLY);
    read(x_pid_w, pid, 80);
    pidX_got = atoi(pid);
    close(x_pid_w);

    z_pid_w = open(pid_motZ_watchdog, O_RDONLY);
    read(z_pid_w, pid, 80);
    pidZ_got = atoi(pid);
    close(z_pid_w);

    // writing watchdog pid
    fd_watchdog = open(watchdog_insp,O_WRONLY);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);

    fd_wdComm = open(comm_wd,O_WRONLY);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_wdComm,pid_w,strlen(pid_w)+1);
    close(fd_wdComm);

    while(1)
    {   
        if(difftime(time(NULL),t) > secs)
        {
            printf("No signals received: reset incoming!\n"); fflush(stdout);

            sleep(0.1);

            kill(pidX_got,SIGUSR1);
            fseek(f,0,SEEK_END);
            clk = time(NULL);
            fprintf(f,"Watchdog, signal SIGUSR1 sent to motor X at : %s",ctime(&clk));
            fflush(f);

            kill(pidZ_got,SIGUSR1);
            fseek(f,0,SEEK_END);
            clk = time(NULL);
            fprintf(f,"Watchdog, signal SIGUSR1 sent to motor Z at : %s",ctime(&clk));
            fflush(f);

            t = time(NULL);
        }
    }
}