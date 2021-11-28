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

int pidX_got, pidZ_got, pidWD_got;
int fdX, fdZ;
char pid[80];
char format_string_pid[80] = "%d,%d";

int main(int argc, char * argv[])
{
    //char * myfifo_inspection = "/tmp/fifo_inspection";

    // the following pipes are used to read values of x and z from the motors
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * fifo_motZinsp = "/tmp/motZ_insp";
    char * pid_motX = "/tmp/pid_motX";
    char * pid_motZ = "/tmp/pid_motZ";
    char * watchdog_insp = "/tmp/watchdog_insp";

    //mkfifo(myfifo_inspection, 0666);
    mkfifo(fifo_motXinsp, 0666);
    mkfifo(fifo_motZinsp, 0666);
    mkfifo(pid_motX,0666);
    mkfifo(pid_motX,0666);
    mkfifo(watchdog_insp,0666);

    // getting  motorX pid
    int fd_pid = open(pid_motX, O_RDONLY);
    read(fd_pid,pid,80);
    pidX_got = atoi(pid);
    close(fd_pid);

    // getting  motorZ pid
    fd_pid = open(pid_motZ, O_RDONLY);
    read(fd_pid,pid,80);
    pidZ_got = atoi(pid);
    close(fd_pid);

    int fd_pid_w = open(watchdog_insp, O_RDONLY);
    read(fd_pid_w,pid,80);
    pidWD_got = atoi(pid);
    close(fd_pid_w);

    char ch1[80];
    char out_str[80];
    char var;
    fd_set rfds;
    fd_set rd_stdin;
    struct timeval tv;
    int ret1, ret2;
    int fd_stdin = fileno(stdin);

    while(1)
    {
        system("clear");
        // using the pipe to read the position from motorX
        fd_x_read = open(fifo_motXinsp, O_RDONLY | O_NONBLOCK);
        
        // using the pipe to read the position from motorZ
        fd_z_read = open(fifo_motZinsp, O_RDONLY | O_NONBLOCK);
       
        // initialise the set and add the file descriptors of the pipe to detect
        FD_ZERO(&rfds);
        FD_ZERO(&rd_stdin);
        FD_SET(fd_x_read,&rfds);
        FD_SET(fd_z_read,&rfds);
        FD_SET(fileno(stdin),&rd_stdin);

        // setting the time select has to wait for
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        printf("X = %s |  Z = %s\n", input_string_x, input_string_z);
        fflush(stdout);

        printf("PRESS: \n s to STOP both motors for an emergency\n r to RESET both motors\n");
        fflush(stdout);

        sleep(1);

        // calling the select to detect changes in the pipes
        ret1 = select(FD_SETSIZE+1,&rd_stdin,NULL,NULL,&tv);
        ret2 = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        switch(ret1)
        {
            case -1: // select error
                perror("Error during the program");
                fflush(stdout);
                break;

            case 0:
                break;

            default:
                read(fd_stdin,out_str,80);
                out_str[strcspn(out_str,"\n")] = 0; // detele \n
                fflush(stdin);
                if(strlen(out_str) > 1)
                {
                    printf("Wrong input. Input has to be 1 character only!\n");
                    fflush(stdout);
                }
                else
                {
                    switch(out_str[0])
                    {
                        // reset
                        case 114: // case r
                        case 82: // case R
                            printf("RESET WAS PRESSED\n");
                            fflush(stdout);
                            kill(pidX_got,SIGUSR1);
                            kill(pidZ_got,SIGUSR1);
                            break;

                        // emergency stop
                        case 115: // case s
                        case 83: // case S
                            printf("EMERGENCY STOP WAS PRESSED\n");
                            fflush(stdout);
                            kill(pidX_got,SIGUSR2);
                            kill(pidZ_got,SIGUSR2);
                            break;

                        default:
                            break;
                    }
                }
                break;
            }
            switch(ret2)
            {
                case -1:
                    perror("Error during the program");
                    fflush(stdout);
                    break;
                
                case 0:
                    break;
                
                default:
                    if(FD_ISSET(fd_x_read,&rfds))
                    {
                        read(fd_x_read, input_string_x, 80);
                    }
                    if(FD_ISSET(fd_z_read,&rfds))
                    {
                        read(fd_z_read, input_string_z, 80);
                    }
                    break;
            }

        // closing pipes
        close(fd_z_read);
        close(fd_x_read);
    }
    unlink(fifo_motXinsp);
    unlink(fifo_motZinsp);
}