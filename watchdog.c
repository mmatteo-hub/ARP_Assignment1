#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <time.h>
#include <signal.h>

time_t t;

float x_position, z_position;

void sig_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        printf("Signal received\n");
        t = time(NULL);
    }
}

int main(int argc, char * argv[])
{
    t = time(NULL);
    signal(SIGUSR1,sig_handler);
    int fd_exec;
    char * myfifo_watchdog = "/tmp/fifo_watchdog";
    char * watchdog_motX = "/tmp/watchdog_motX";
    char * watchdog_motZ= "/tmp/watchdog_motZ";
    mkfifo(myfifo_watchdog,0666);
    mkfifo(watchdog_motX,0666);
    mkfifo(watchdog_motZ,0666);

    char strp[80];
    char passVal[80];
    char format_string[80] = "%d,%d,%d,%d,%d";
    int pid1, pid2, pid3, pid4, pid5;

    fd_exec = open(myfifo_watchdog,O_RDONLY); 
    read(fd_exec, strp, 80); 
    sscanf(strp, format_string, &pid1, &pid2, &pid3, &pid4, &pid5);
    close(fd_exec);
    unlink(myfifo_watchdog);
        
    printf("Inside watchdog\n");
    fflush(stdout);
    printf("PIDs are:\n(PID_1 = %d)\n(PID_2 = %d)\n(PID_3 = %d)\n(PID_4 = %d)\n(PID_5 = %d)\n", pid1,  pid2, pid3, pid4, pid5); 
    fflush(stdout);
    
    while(1)
    {
        /*if(difftime(time(NULL),t) > 15)
        {
            printf("Resetting ... by watchdog\n");
            fflush(stdout);

            int fdX = open(watchdog_motX, O_WRONLY);
            int fdZ = open(watchdog_motZ, O_WRONLY);
            x_position = 0;
            z_position = 0;
            sprintf(passVal,format_string,x_position);
            write(fdX,passVal,strlen(passVal)+1);
            sprintf(passVal,format_string,z_position);
            write(fdZ,passVal,strlen(passVal)+1);
            close(fdZ);
            unlink(watchdog_motZ);
            close(fdX);
            unlink(watchdog_motX);
        }*/

        if(difftime(time(NULL),t) > 1000)
        {
            kill(pid1,SIGKILL);
            printf("Process with (PID = %d) killed\n", pid1);
            fflush(stdout);
            sleep(1);
            kill(pid2,SIGKILL);
            printf("Process with (PID = %d) killed\n", pid2);
            fflush(stdout);
            sleep(1);
            kill(pid3,SIGKILL);
            printf("Process with (PID = %d) killed\n", pid3);
            fflush(stdout);
            sleep(1);
            kill(pid4,SIGKILL);
            printf("Process with (PID = %d) killed\n", pid4);
            fflush(stdout);
            sleep(1);
            printf("I'll terminate watchdog process, (PID = %d), in 5 seconds\n", pid5);
            fflush(stdout);
            sleep(5);
            kill(pid5,SIGKILL);
        }
    }
}