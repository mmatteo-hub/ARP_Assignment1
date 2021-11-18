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

int main(int argc, char * argv[])
{
    // pipe communicating with the command console for passing the pid of the process
    char * fifo_mot_commX = "/tmp/comm_motX";

    // pipe to read the value from command console
    char * fifo_valX = "/tmp/fifo_valX";

    // pipe to write commands and sent them to the inspection console
    char * fifo_motXinsp = "/tmp/motX_insp";

    // pipe to read commands from the inspection console
    char * fifo_inspmotX = "/tmp/insp_motX";
    
    mkfifo(fifo_mot_commX,0666);
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_motXinsp,0666);
    mkfifo(fifo_inspmotX,0666);

    // send the process pid via pipe to the command console
    char pid_string[80];
    char format_pid_string[80] = "%d";

    // using the pipe to take the process pid and write on it
    int fd = open(fifo_mot_commX,O_WRONLY);
    sprintf(pid_string,format_pid_string,(int)getpid());
    write(fd,pid_string,strlen(pid_string)+1);
    close(fd);
    
    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string_comm[80];
    char input_string_insp[80];
    char format_string[80] = "%f";
    char input_str;
    char passVal[80];

    while(1)
    {
        // open pipes
        int fd_val = open(fifo_valX, O_RDONLY);
        int fd_insp = open(fifo_inspmotX, O_RDONLY);
        int fd_x = open(fifo_motXinsp, O_WRONLY);

        // initialise the set and add the file descriptors of the pipe to detect
        FD_ZERO(&rfds);
        FD_SET(fd_val,&rfds);
        FD_SET(fd_insp,&rfds);

        // setting the time select has to wait for
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        // calling the select to detect changes in the pipes
        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        switch(retval)
        {
            case -1: // select error
                perror("select()");
                fflush(stdout);
                break;

            case 0: // no new value
                switch(atoi(input_string_comm))
                {
                    // left
                    case 97: // case a
                    case 65: // case A
                        if(x_position > 0)
                        {
                            x_position -= 0.25;
                            sprintf(passVal,format_string,x_position);
                            write(fd_x,passVal,strlen(passVal)+1);
                            sleep(1);
                        }
                        else
                        {
                            printf("X cannot be decreased any more\n");
                            fflush(stdout);
                        }
                        break;

                    // right
                    case 100: // case d
                    case 68: // case D
                        if(x_position < 20)
                        {
                            x_position += 0.25;
                            sprintf(passVal,format_string,x_position);
                            write(fd_x,passVal,strlen(passVal)+1);
                            sleep(1);
                        }
                        else
                        {
                            printf("X cannot be increased any more\n");
                            fflush(stdout);
                        }
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
                    read(fd_val, input_string_comm, 80);
                }
                if(FD_ISSET(fd_insp,&rfds))
                {
                    read(fd_insp, input_string_insp, 80);
                }
                break;
        }

        // close pipes
        close(fd_x);
        unlink(fifo_motXinsp);
        close(fd_insp);
        unlink(fifo_inspmotX);
        close(fd_val);
        unlink(fifo_valX);
    }
}