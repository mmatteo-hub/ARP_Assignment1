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
char pid_motX[80];
char pid_motZ[80];
char format_string_pid[80] = "%d";

int main(int argc, char * argv[])
{
    char * myfifo_inspection = "/tmp/fifo_inspection";

    // the following pipes are used to send signals of reset and stop to the motors
    char * fifo_inspmotX = "/tmp/insp_motX";
    char * fifo_inspmotZ = "/tmp/insp_motZ";

    // the following pipes are used to read values of x and z from the motors
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * fifo_motZinsp = "/tmp/motZ_insp";

    char * inspection_motX = "/tmp/inspection_motX";
    char * inspection_motZ= "/tmp/inspection_motZ";

    mkfifo(myfifo_inspection, 666);
    mkfifo(fifo_inspmotX, 0666);
    mkfifo(fifo_inspmotZ, 0666);
    mkfifo(fifo_motXinsp, 0666);
    mkfifo(fifo_motZinsp, 0666);
    mkfifo(inspection_motX,0666);
    mkfifo(inspection_motZ,0666);

    // takes the pid of motorX and stores it into a variable
    fdX = open(inspection_motX,O_RDONLY);
    read(fdX, pid_motX, 80);
    sscanf(pid_motX, format_string_pid, &pidX_got);
    close(fdX);
    unlink(inspection_motX);

    // takes the pid of motorZ and stores it into a variable
    fdZ = open(inspection_motZ,O_RDONLY);
    read(fdZ, pid_motZ, 80);
    sscanf(pid_motZ, format_string_pid, &pidZ_got);
    close(fdZ);
    unlink(inspection_motZ);

    char ch1[80];
    char out_str[80];
    char var;
    fd_set rfds;
    struct timeval tv;
    int retval;
    int fd_stdin;

    while(1)
    {
        // opening the pipes comunicating with the motors to use them for passing reset and emergency commands
        fd_x_write = open(fifo_inspmotX, O_WRONLY | O_NONBLOCK);
        fd_z_write = open(fifo_inspmotZ, O_WRONLY | O_NONBLOCK);

        // using the pipe to read the position from motorX
        fd_x_read = open(fifo_motXinsp, O_RDONLY | O_NONBLOCK);
        read(fd_x_read, input_string_x, 80);
        // using the pipe to read the position from motorZ
        fd_z_read = open(fifo_motZinsp, O_RDONLY | O_NONBLOCK);
        read(fd_z_read, input_string_z, 80);
       
        // initialise the set and add the file descriptors of the pipe to detect
        FD_ZERO(&rfds);
        FD_SET(fd_x_write,&rfds);
        FD_SET(fd_z_write,&rfds);
        FD_SET(fd_x_read,&rfds);
        FD_SET(fd_z_read,&rfds);

        // setting the time select has to wait for
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        printf("PRESS: \n s to STOP both motors for an emergency\n r to RESET both motors\n");
        fflush(stdout);

        // calling the select to detect changes in the pipes
        retval = select(fd_stdin+1,&rfds,NULL,NULL,&tv);

        switch(retval)
        {
            case -1: // select error
                perror("select()");
                fflush(stdout);
                break;

            case 0:
                // printing the position got from motors
                sleep(2);
                printf("X = %s\nZ = %s\n", input_string_x, input_string_z);
                fflush(stdout);
                break;

            default:
                ch1[0] = read(fd_stdin,out_str,80);
                {
                    //sprintf(out_str, format_string, ch1[0]); // prints the char according to a format string
                    var = ch1[0];
                    switch(var)
                    {
                        // reset
                        case 114: // case r
                        case 82: // case R
                            printf("RESET WAS PRESSED\n");
                            fflush(stdout);
                            kill(pidX_got,SIGUSR1);
                            kill(pidZ_got,SIGUSR1);
                            /*
                            write(fd_x_write, out_str, strlen(out_str)+1);
                            write(fd_z_write, out_str, strlen(out_str)+1);
                            */
                            break;

                        // emergency stop
                        case 115: // case s
                        case 83: // case S
                            printf("EMERGENCY STOP WAS PRESSED\n");
                            fflush(stdout);
                            kill(pidX_got,SIGUSR2);
                            kill(pidZ_got,SIGUSR2);
                            /*
                            write(fd_x_write, out_str, strlen(out_str)+1);
                            write(fd_z_write, out_str, strlen(out_str)+1);*/
                            break;
                        default:
                            printf("Wrong input, key pressed: %c\n", var);
                            fflush(stdout);
                            break;
                    }
                }
                break;
        }

        // closing pipes
        close(fd_z_write);
        close(fd_x_write);
        close(fd_z_read);
        close(fd_x_read);
    }
    unlink(fifo_motXinsp);
    unlink(fifo_motZinsp);
    unlink(fifo_inspmotZ);
    unlink(fifo_inspmotX);
}