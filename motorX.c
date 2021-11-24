#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <math.h>

// maximum err
#define delta 0.25

float x_position = 0; // motorX positiion
float err; // error in motorX position

sig_atomic_t sig = 0;

int sign(void)
{
    int n = rand() % 100;
    if(floor(n) > 49 ) return 1;
    else return 0;
}

// manages signal from watchdog 
void sig_handler(int signo)
{
    if (signo == SIGUSR1) sig = 1;
    else if(signo == SIGUSR2) sig = 2;
}

int watchdogPid;

int main(int argc, char * argv[])
{
    signal(SIGUSR1,sig_handler);
    
    // pipe communicating with the command console for passing the pid of the process
    char * fifo_mot_commX = "/tmp/comm_motX";

    // pipe to read the value from command console
    char * fifo_valX = "/tmp/fifo_valX";

    // pipe to write commands and sent them to the inspection console
    char * fifo_motXinsp = "/tmp/motX_insp";

    // pipe to read commands from the inspection console
    char * fifo_inspmotX = "/tmp/insp_motX";

    // pipe from watchdog to motX
    char * watchdog_motX = "/tmp/watchdog_motX";
    
    mkfifo(fifo_mot_commX,0666);
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_motXinsp,0666);
    mkfifo(fifo_inspmotX,0666);
    mkfifo(watchdog_motX,0666);

    // send the process pid via pipe to the command console
    char pid_string[80];
    char format_pid_string[80] = "%d";

    // using the pipe to take the process pid and write on it
    int fd = open(fifo_mot_commX,O_WRONLY);
    sprintf(pid_string,format_pid_string,(int)getpid());
    write(fd,pid_string,strlen(pid_string)+1);
    close(fd);

    char pid[80];

    fd = open(watchdog_motX,O_RDONLY);
    read(fd, pid, 80);
    sscanf(pid, format_pid_string, &watchdogPid);
    close(fd);
    unlink(watchdog_motX);
    
    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string_insp[80];
    char format_string[80] = "%f";
    char input_str;
    char passVal[80];

    while(1)
    {
        // compute the error
        float err = (rand() % 1)/4;
        // compute the sign
        int s = sign();
        // open pipes
        int fd_val = open(fifo_valX, O_RDONLY | O_NONBLOCK);
        sleep(0.5);
        int fd_insp = open(fifo_inspmotX, O_RDONLY | O_NONBLOCK);
        int fd_x = open(fifo_motXinsp, O_WRONLY | O_NONBLOCK);
        // initialise the set and add the file descriptors of the pipe to detect
        FD_ZERO(&rfds);
        FD_SET(fd_val,&rfds);
        FD_SET(fd_insp,&rfds);

        // setting the time select has to wait for
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        // calling the select to detect changes in the pipes
        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        switch(sig)
        {
            case 1: // go to the reset
                x_position = 0;
                printf("X = %f\n",x_position);
                fflush(stdout);
                sprintf(passVal,format_string,x_position);
                write(fd_x,passVal,strlen(passVal)+1);
                sleep(1);
                break;

            case 2: // go to the stop
                sprintf(passVal,format_string,x_position);
                write(fd_x,passVal,strlen(passVal)+1);
                sleep(1);
                break;

            default:
                break;
        }

        switch(retval)
        {
            case -1: // select error
                perror("select()");
                fflush(stdout);
                break;

            case 0: // no new value
                switch(atoi(input_string_insp))
                {
                    // left
                    case 97: // case a
                    case 65: // case A
                        if(x_position - err > 0)
                        {
                            if(s)
                            {
                                x_position -= (delta+err);
                                printf("X = %f\n",x_position);
                                fflush(stdout);
                                sprintf(passVal,format_string,x_position);
                                write(fd_x,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                            else if(!s)
                            {
                                x_position -= (delta-err);
                                printf("X = %f\n",x_position);
                                fflush(stdout);
                                sprintf(passVal,format_string,x_position);
                                write(fd_x,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                        }
                        else
                        {
                            printf("X cannot be decreased any more\n");
                            fflush(stdout);
                        }
                        kill(watchdogPid,SIGUSR1);
                        break;

                    // right
                    case 100: // case d
                    case 68: // case D
                        if(x_position < 20)
                        {
                            printf("here\n");
                            fflush(stdout);
                            if(s)
                            {
                                x_position += (delta+err);
                                printf("X = %f\n",x_position);
                                fflush(stdout);
                                sprintf(passVal,format_string,x_position);
                                write(fd_x,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                            else if(!s)
                            {
                                x_position += (delta-err);
                                printf("X = %f\n",x_position);
                                fflush(stdout);
                                sprintf(passVal,format_string,x_position);
                                write(fd_x,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                        }
                        else
                        {
                            printf("X cannot be increased any more\n");
                            fflush(stdout);
                        }
                        kill(watchdogPid,SIGUSR1);
                        break;

                    // stop
                    case 113: // case q
                    case 81: // case Q
                        sprintf(passVal,format_string,x_position);
                        write(fd_x,passVal,strlen(passVal)+1);
                        sleep(1);
                        break;

                    default:
                        break;
                }
                
                switch(atoi(input_string_insp)) // switch among the command taken from the inspection console
                {
                    // reset
                    case 114: // case r
                    case 82: // case R
                        x_position = 0;
                        printf("X = %f\n",x_position);
                        fflush(stdout);
                        sprintf(passVal,format_string,x_position);
                        write(fd_x,passVal,strlen(passVal)+1);
                        sleep(1);
                        break;

                    // emergency stop
                    case 115: // case s
                    case 83: // case S
                        sprintf(passVal,format_string,x_position);
                        write(fd_x,passVal,strlen(passVal)+1);
                        sleep(1);
                        break;

                    default:
                        break;
                }
                break;

            default: // got a new value
                // check with the if statements which fd the change belongs to
                if(FD_ISSET(fd_val,&rfds))
                {
                    read(fd_val, input_string_insp, 80);
                }
                if(FD_ISSET(fd_insp,&rfds))
                {
                    read(fd_insp, input_string_insp, 80);
                }
                break;
        }

        // close pipes
        close(fd_x);
        close(fd_insp);
        close(fd_val);
    }
    unlink(fifo_motXinsp);
    unlink(fifo_inspmotX);
    unlink(fifo_valX);
}