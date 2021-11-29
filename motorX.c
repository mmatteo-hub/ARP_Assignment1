#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>

#define increment 0.25


float x_position = 0; // motorX positiion

// error on position
float randomErr()
{
    srand(time(NULL));
    return (((double)rand()) / ((double)RAND_MAX)) * (double)0.10;
}

// sign
int sign()
{
    srand(time(NULL));
    float n = (double)rand() / (double)RAND_MAX;
    if(n >  0.5) return 1;
    else return 0;
}

int fd_valX, fdX_write;
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

    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * pid_motX = "/tmp/pid_motX";
    char * pid_motX_watchdog = "/tmp/pid_motX_watch";
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_motXinsp,0666);
    mkfifo(pid_motX,0666);
    mkfifo(pid_motX_watchdog,0666);

    char pid[80];

    // writing pid
    int x_pid = open(pid_motX, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(x_pid, pid, strlen(pid)+1);
    close(x_pid);
    int x_pid_w = open(pid_motX_watchdog, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(x_pid_w, pid, strlen(pid)+1);
    close(x_pid_w);

    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string[80];
    char input_str;

    while(1)
    {   int s = sign();
        int err = randomErr();
        // open pipe
        fd_valX = open(fifo_valX,O_RDONLY | O_NONBLOCK);
        fdX_write = open(fifo_motXinsp, O_WRONLY | O_NONBLOCK);

        FD_ZERO(&rfds);
        FD_SET(fd_valX,&rfds);

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
                if(sig || sig == 2) break;
                else
                {
                    switch(atoi(input_string))
                    {
                        // left
                        case 65: // A
                        case 97: // a
                            if(x_position - (increment + err) > 0)
                            {
                                if(s)
                                {
                                    x_position -= (increment+err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    sleep(1);
                                }
                                else if(!s)
                                {
                                    x_position -= (increment-err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    sleep(1);
                                }
                            }
                            else
                            {
                                system("clear");
                                printf("X cannot be decreased any more\n");
                                fflush(stdout);
                            }
                            break;

                        // right
                        case 68: // D
                        case 100: // d
                            if(x_position + (increment + err) < 25)
                            {
                                if(s)
                                {
                                    x_position += (increment + err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    sleep(1);
                                }
                                else if(!s)
                                {
                                    x_position += (increment - err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    sleep(1);
                                }
                            }
                            else
                            {
                                system("clear");
                                printf("X cannot be increased any more\n");
                                fflush(stdout);
                            }
                            break;

                        // stop X
                        case 81: // Q
                        case 113: // q
                            sleep(1);
                            break;
                        
                        default:
                            break;
                    }
                }
                break;

            default: // got a new value
                read(fd_valX, input_string, 80);
                sig = 0;
                break;
        }

        switch(sig)
        {
            case 1: // reset
                x_position = 0;
                sprintf(passVal,format_string,x_position);
                write(fdX_write,passVal,strlen(passVal)+1);
                sleep(1);
                break;
            
            case 2: // stop
                break;
            
            default:
                break;
        }

        close(fdX_write);
        close(fd_valX);
    }
    unlink(fifo_valX);
    unlink(fifo_motXinsp);
}