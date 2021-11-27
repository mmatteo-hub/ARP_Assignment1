#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

float x_position = 0; // motorX positiion

int fd_val, fdX_write;
char pid_string[80];
char format_pid_string[80] = "%d";

sig_atomic_t sig = 0;

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
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_motXinsp,0666);

    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string[80];
    char format_string[80] = "%f";
    char input_str;
    char passVal[80];

    while(1)
    {
        // open pipe
        fd_val = open(fifo_valX,O_RDONLY);
        fdX_write = open(fifo_motXinsp, O_WRONLY | O_NONBLOCK);
        
        switch(sig)
        {
            case 1: // reset
                x_position = 0;
                sprintf(passVal,format_string,x_position);
                write(fdX_write,passVal,strlen(passVal)+1);
                sig = 0;
                break;
            
            default:
                break;
        }

        FD_ZERO(&rfds);
        FD_SET(fd_val,&rfds);

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
                    case 97: // left
                        if(x_position > 0)
                        {
                            x_position -= 0.25;
                            sprintf(passVal,format_string,x_position);
                            write(fdX_write,passVal,strlen(passVal)+1);
                            printf("%s\n",passVal); fflush(stdout);
                            printf("X = %f\n",x_position);
                            fflush(stdout);
                            sleep(1);
                        }
                        else
                        {
                            printf("X cannot be decreased any more\n");
                            fflush(stdout);
                        }
                        break;

                    case 100: // right
                        if(x_position < 20)
                        {
                            x_position += 0.25;
                            sprintf(passVal,format_string,x_position);
                            write(fdX_write,passVal,strlen(passVal)+1);
                            printf("%s\n",passVal); fflush(stdout);
                            printf("X = %f\n",x_position);
                            fflush(stdout);
                            sleep(1);
                        }
                        else
                        {
                            printf("X cannot be increased any more\n");
                            fflush(stdout);
                        }
                        break;

                    case 114: // reset
                        printf("Resetting...\n");
                        fflush(stdout);
                        x_position = 0;
                        sleep(1);
                        break;

                    case 115: // stop
                        printf("Stop X = %f\n",x_position);
                        fflush(stdout);
                        sleep(1);
                        break;
                    
                    default:
                        break;
                }
                break;

            default: // got a new value
                read(fd_val, input_string, 80);
                break;
        }
        close(fdX_write);
        close(fd_val);
    }
    unlink(fifo_valX);
    unlink(fifo_motXinsp);
}