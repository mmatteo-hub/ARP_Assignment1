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
    mkfifo(fifo_mot_commX,0666);
    mkfifo(fifo_valX,0666);

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

    char input_string[80];
    char format_string[80] = "%d";
    char input_str;

    while(1)
    {
        // open pipe
        int fd_val = open(fifo_valX,O_RDONLY);
        
        printf("pipe opened\n");
        fflush(stdout);
        sleep(1);

        FD_ZERO(&rfds);
        FD_SET(fd_val,&rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);
        printf("retval.. = %d\n",retval);
        fflush(stdout);
        sleep(1);
        switch(retval)
        {
            case -1: // select error
                perror("select()");
                fflush(stdout);
                break;

            case 0: // no new value
                switch(atoi(input_string))
                {
                    case 100: // right
                        if(x_position < 3)
                        {
                            x_position += 0.25;
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

                    case 97: // left
                        if(x_position > 0)
                        {
                            x_position -= 0.25;
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
                    
                    default:
                        break;
                }
                break;

            default: // got a new value
                read(fd_val, input_string, 80);
                //sprintf(input_str, format_string, input_string);
                printf("input str, case retval1 = %s\n",input_string);
                fflush(stdout);
                sleep(1);
                printf("retval1 = %d\n",retval);
                fflush(stdout);
                break;
        }
        close(fd_val);
        printf("pipe closed\n");
        fflush(stdout);
        sleep(1);
        printf("finished loop\n");
        fflush(stdout);
    }
}