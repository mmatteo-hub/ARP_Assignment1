#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

float z_position = 0; // motorX positiion

int main(int argc, char * argv[])
{
    char * fifo_mot_commZ = "/tmp/comm_motZ";
    char * fifo_valZ = "/tmp/fifo_valZ";
    char * fifo_motZinsp = "/tmp/insp_motZ";
    mkfifo(fifo_mot_commZ,0666);
    mkfifo(fifo_valZ,0666);
    mkfifo(fifo_motZinsp,0666);

    // send the process pid via pipe to the command console
    char pid_string[80];
    char format_pid_string[80] = "%d";
    int fd = open(fifo_mot_commZ,O_WRONLY);
    sprintf(pid_string,format_pid_string,(int)getpid());
    write(fd,pid_string,strlen(pid_string)+1);
    close(fd);
    

    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string[80];
    char format_string[80] = "%f";
    char input_str;

    while(1)
    {
        // open pipe
        int fd_val = open(fifo_valZ,O_RDONLY);

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
                    case 122: // down
                        if(z_position > 0)
                        {
                            z_position -= 0.25;
                            printf("Z = %f\n",z_position);
                            fflush(stdout);
                            sleep(1);
                        }
                        else
                        {
                            printf("Z cannot be decreased any more\n");
                            fflush(stdout);
                        }
                        break;

                    case 119: // up
                        if(z_position < 20)
                        {
                            z_position += 0.25;
                            printf("Z = %f\n",z_position);
                            fflush(stdout);
                            sleep(1);
                        }
                        else
                        {
                            printf("Z cannot be increased any more\n");
                            fflush(stdout);
                        }
                        break;

                    case 114: // reset
                        printf("Resetting...\n");
                        fflush(stdout);
                        z_position = 0;
                        sleep(1);
                        break;

                    case 115: // stop
                        printf("Stop Z = %f\n",z_position);
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
        close(fd_val);
    }
}