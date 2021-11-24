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
char pid_motX[80];
char pid_motZ[80];
char format_string[80] = "%d";

float x_position, z_position;

void sig_handler(int signo)
{
    if (signo == SIGUSR1) sig = 1;
    else sig = 0;
}

int main(int argc, char * argv[])
{
    //t = time(NULL);
    signal(SIGUSR1,sig_handler);
    //int fd_exec;
    // pipes from motors to watchdog
    char * fifo_pid_motX = "/tmp/comm_motX";
    char * fifo_pid_motZ = "/tmp/comm_motZ";
    
    char * watchdog_motX = "/tmp/watchdog_motX";
    char * watchdog_motZ= "/tmp/watchdog_motZ";

    mkfifo(fifo_pid_motX,0666);
    mkfifo(fifo_pid_motZ,0666);
    mkfifo(watchdog_motX,0666);
    mkfifo(watchdog_motZ,0666);

    char pid_w[80];

    // getting watchdog pid
    int fd_watchdog = open(watchdog_motX,O_WRONLY);
    sprintf(pid_w,format_string,(int)getpid());
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);
    unlink(watchdog_motX);

    fd_watchdog = open(watchdog_motZ,O_WRONLY);
    write(fd_watchdog,pid_w,strlen(pid_w)+1);
    close(fd_watchdog);
    unlink(watchdog_motZ);

    // takes the pid of motorX and stores it into a variable
    fdX = open(fifo_pid_motX,O_RDONLY);
    read(fdX, pid_motX, 80);
    sscanf(pid_motX, format_string, &pidX_got);
    close(fdX);
    unlink(fifo_pid_motX);

    // takes the pid of motorZ and stores it into a variable
    fdZ = open(fifo_pid_motZ,O_RDONLY);
    read(fdZ, pid_motZ, 80);
    sscanf(pid_motZ, format_string, &pidZ_got);
    close(fdZ);
    unlink(fifo_pid_motZ);

    char strp[80];
    char passVal[80];
    char format_string[80] = "%d,%d,%d,%d,%d";
    int pid1, pid2, pid3, pid4, pid5;

    /*fd_exec = open(myfifo_watchdog,O_RDONLY); 
    read(fd_exec, strp, 80); 
    sscanf(strp, format_string, &pid1, &pid2, &pid3, &pid4, &pid5);
    close(fd_exec);
    unlink(myfifo_watchdog);
        
    printf("Inside watchdog\n");
    fflush(stdout);
    printf("PIDs are:\n(PID_1 = %d)\n(PID_2 = %d)\n(PID_3 = %d)\n(PID_4 = %d)\n(PID_5 = %d)\n", pid1,  pid2, pid3, pid4, pid5); 
    fflush(stdout);*/
    
    int flag = 1;

    while(1)
    {
        do
        {   
            flag = 0;
            sleep(secs);
        } while(flag);

        printf("No signals received: reset incoming!\n"); fflush(stdout);

        kill(pidX_got,SIGUSR1);
        kill(pidZ_got,SIGUSR1);
    }
}