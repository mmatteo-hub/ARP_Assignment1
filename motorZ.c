#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

float z_position = 0; // motorZ positiion

int fd_valZ, fdZ_write;
char pid_string[80];
char format_pid_string[80] = "%d";

char passVal[80];
char format_string[80] = "%f";

int sig = 0;

// signals from inspection
void sig_handler(int signo)
{
    if (signo == SIGUSR1) sig = 1; // reset
    else if(signo == SIGUSR2) sig = 2; // stop
}

int main(int argc, char * argv[])
{
    // signals from inspection
    signal(SIGUSR1,sig_handler);
    signal(SIGUSR2,sig_handler);

    char * fifo_valZ = "/tmp/fifo_valZ";
    char * fifo_motZinsp = "/tmp/motZ_insp";
    char * pid_motZ = "/tmp/pid_motZ";
    char * pid_motZ_watchdog = "/tmp/pid_motZ_watch";
    mkfifo(fifo_valZ,0666);
    mkfifo(fifo_motZinsp,0666);
    mkfifo(pid_motZ,0666);
    mkfifo(pid_motZ_watchdog,0666);

    char pid[80];

    // writing pid
    int z_pid = open(pid_motZ, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(z_pid, pid, strlen(pid)+1);
    close(z_pid);
    int z_pid_w = open(pid_motZ_watchdog, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(z_pid_w, pid, strlen(pid)+1);
    close(z_pid_w);
    //printf("%d\n", (int)getpid()); fflush(stdout);

    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string[80];
    char input_str;

    while(1)
    {   
        // open pipe
        fd_valZ = open(fifo_valZ,O_RDONLY | O_NONBLOCK);
        fdZ_write = open(fifo_motZinsp, O_WRONLY | O_NONBLOCK);

        FD_ZERO(&rfds);
        FD_SET(fd_valZ,&rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        switch(retval)
        {
            case -1: // select error
                perror("select()");
                fflush(stdout);
                break;

            case 0: // no new value
                switch(atoi(input_string))
                {
                    // down
                    case 122: // z
                    case 90: // Z
                        if(z_position > 0)
                        {
                            z_position -= 0.25;
                            sprintf(passVal,format_string,z_position);
                            write(fdZ_write,passVal,strlen(passVal)+1);
                            //printf("Z = %f\n",z_position);
                            //fflush(stdout);
                            sleep(1);
                        }
                        else
                        {
                            printf("Z cannot be decreased any more\n");
                            fflush(stdout);
                        }
                        break;

                    // up
                    case 119: // w
                    case 87: // W
                        if(z_position < 20)
                        {
                            z_position += 0.25;
                            sprintf(passVal,format_string,z_position);
                            write(fdZ_write,passVal,strlen(passVal)+1);
                            //printf("Z = %f\n",z_position);
                            //fflush(stdout);
                            sleep(1);
                        }
                        else
                        {
                            printf("Z cannot be increased any more\n");
                            fflush(stdout);
                        }
                        break;

                    // stop Z
                    case 101: // e
                    case 69: // E
                        //printf("Stop X = %f\n",z_position);
                        //fflush(stdout);
                        sleep(1);
                        break;
                    
                    default:
                        break;
                }
                break;

            default: // got a new value
                read(fd_valZ, input_string, 80);
                sig = 0;
                break;
        }

        switch(sig)
        {
            case 1: // reset
                z_position = 0;
                sprintf(passVal,format_string,z_position);
                write(fdZ_write,passVal,strlen(passVal)+1);
                sleep(1);
                break;
            
            case 2: // stop
                sleep(1);
                break;
            
            default:
                break;
        }

        close(fdZ_write);
        close(fd_valZ);
    }
    unlink(fifo_valZ);
    unlink(fifo_motZinsp);
}