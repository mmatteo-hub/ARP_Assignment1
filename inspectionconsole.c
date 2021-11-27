#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int fd_x_write, fd_z_write;
int fd_x_read, fd_z_read;

char input_string_x[80];
char input_string_z[80];
char format_string[80] = "%f";

int pidX_got, pidZ_got;
int fdX, fdZ;
char pids[80];
char format_string_pid[80] = "%d,%d";

int main(int argc, char * argv[])
{
    char * myfifo_inspection = "/tmp/fifo_inspection";

    // the following pipes are used to read values of x and z from the motors
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * fifo_motZinsp = "/tmp/motZ_insp";

    mkfifo(myfifo_inspection, 0666);
    mkfifo(fifo_motXinsp, 0666);
    mkfifo(fifo_motZinsp, 0666);

    int fd_pids = open(myfifo_inspection, O_RDONLY);
    read(fd_pids, pids, 80);
    sscanf(pids, format_string_pid, &pidX_got, &pidZ_got);
    close(fd_pids);
    printf("%d\n", pidX_got); fflush(stdout);

    char ch1[80];
    char out_str[80];
    char var;
    fd_set rfds;
    struct timeval tv;
    int retval;
    int fd_stdin;

    while(1)
    {
        // using the pipe to read the position from motorX
        fd_x_read = open(fifo_motXinsp, O_RDONLY | O_NONBLOCK);
        read(fd_x_read, input_string_x, 80);
        // using the pipe to read the position from motorZ
        fd_z_read = open(fifo_motZinsp, O_RDONLY | O_NONBLOCK);
        read(fd_z_read, input_string_z, 80);
       
        // initialise the set and add the file descriptors of the pipe to detect
        FD_ZERO(&rfds);
        FD_SET(fd_stdin,&rfds);

        // setting the time select has to wait for
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        printf("X = %s\nZ = %s\n", input_string_x, input_string_z);
        fflush(stdout);
        sleep(1);

        printf("PRESS: \n s to STOP both motors for an emergency\n r to RESET both motors\n");
        fflush(stdout);

        // calling the select to detect changes in the pipes
        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        switch(retval)
        {
            case -1: // select error
                perror("select()");
                fflush(stdout);
                break;

            case 1:
                break;

            case 0:
                switch(var)
                {
                    // reset
                    case 114: // case r
                    case 82: // case R
                        printf("RESET WAS PRESSED\n");
                        fflush(stdout);
                        kill(pidX_got,SIGUSR1);
                        //kill(pidZ_got,SIGUSR1);
                        break;

                    // emergency stop
                    case 115: // case s
                    case 83: // case S
                        printf("EMERGENCY STOP WAS PRESSED\n");
                        fflush(stdout);
                        kill(pidX_got,SIGUSR2);
                        //kill(pidZ_got,SIGUSR2);
                        break;

                    default:
                        printf("Wrong input, key pressed: %c\n", var);
                        fflush(stdout);
                        break;
                }
                break;
            default:
                ch1[0] = read(fd_stdin,out_str,80);
                sprintf(out_str, format_string, ch1[0]); // prints the char according to a format string
                var = ch1[0];
                break;
        }

        // closing pipes
        close(fd_z_read);
        close(fd_x_read);
    }
    unlink(fifo_motXinsp);
    unlink(fifo_motZinsp);
}