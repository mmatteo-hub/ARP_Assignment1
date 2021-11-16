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
    char * fifo_mot_commX = "/tmp/comm_motX";
    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * fifo_inspmotX = "/tmp/insp_motX";
    mkfifo(fifo_mot_commX,0666);
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_motXinsp,0666);
    mkfifo(fifo_inspmotX,0666);

    // send the process pid via pipe to the command console
    char pid_string[80];
    char format_pid_string[80] = "%d";
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
        // open pipe
        int fd_val = open(fifo_valX, O_RDONLY);
        int fd_insp = open(fifo_inspmotX, O_RDONLY);
        int fd_x = open(fifo_motXinsp, O_WRONLY);

        FD_ZERO(&rfds);
        FD_SET(fd_val,&rfds);
        FD_SET(fd_insp,&rfds);

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
                switch(atoi(input_string_comm))
                {
                    case 97: // left
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

                    case 100: // right
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

                    case 113: // stop
                        sprintf(passVal,format_string,x_position);
                        write(fd_x,passVal,strlen(passVal)+1);
                        sleep(1);
                        break;

                    default:
                        break;
                }
                switch(atoi(input_string_insp)) 
                {
                    case 114: // reset
                        x_position = 0;
                        sprintf(passVal,format_string,x_position);
                        write(fd_x,passVal,strlen(passVal)+1);
                        sleep(1);
                        break;

                    case 115: // emergency stop
                        sprintf(passVal,format_string,x_position);
                        write(fd_x,passVal,strlen(passVal)+1);
                        sleep(1);
                        break;

                    default:
                        break;
                }
                break;

            default: // got a new value
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
        close(fd_x);
        close(fd_insp);
        close(fd_val);
    }
}