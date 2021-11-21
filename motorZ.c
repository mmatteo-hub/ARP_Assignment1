#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

// maximum err
#define delta 0.25

float z_position = 0; // motorX positiion

int sign(void)
{
    int n = rand() % 100;
    if(floor(n) > 49 ) return 1;
    else return 0;
}

int main(int argc, char * argv[])
{
    // pipe communicating with the command console for passing the pid of the process
    char * fifo_mot_commZ = "/tmp/comm_motZ";

    // pipe to read the value from command console
    char * fifo_valZ = "/tmp/fifo_valZ";

    // pipe to write commands and sent them to the inspection console
    char * fifo_motZinsp = "/tmp/motZ_insp";

    // pipe to read commands from the inspection console
    char * fifo_inspmotZ = "/tmp/insp_motZ";

    mkfifo(fifo_mot_commZ,0666);
    mkfifo(fifo_valZ,0666);
    mkfifo(fifo_motZinsp,0666);
    mkfifo(fifo_inspmotZ,0666);

    // send the process pid via pipe to the command console
    char pid_string[80];
    char format_pid_string[80] = "%d";

    // using the pipe to take the process pid and write on it
    int fd = open(fifo_mot_commZ,O_WRONLY);
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
        // compute the error
        err = (rand() % 1)/4;
        // compute the sign
        int s = sign();

        // open pipes
        int fd_val = open(fifo_valZ, O_RDONLY);
        int fd_insp = open(fifo_inspmotZ, O_RDONLY);
        int fd_z = open(fifo_motZinsp, O_WRONLY);

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
                    // down
                    case 122: // case z
                    case 90: // case Z
                        if(z_position > 0)
                        {
                            if(s)
                            {
                                z_position -= (delta+err);
                                sprintf(passVal,format_string,z_position);
                                write(fd_z,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                            else
                            {
                                z_position -= (delta-err);
                                sprintf(passVal,format_string,z_position);
                                write(fd_z,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                        }
                        else
                        {
                            printf("Z cannot be decreased any more\n");
                            fflush(stdout);
                        }
                        break;

                    // up
                    case 119: // case w
                    case 87: // case W
                        if(z_position < 20)
                        {
                            if(s)
                            {
                                z_position += (delta+err);
                                sprintf(passVal,format_string,z_position);
                                write(fd_z,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                            else
                            {
                                z_position += (delta-err);
                                sprintf(passVal,format_string,z_position);
                                write(fd_z,passVal,strlen(passVal)+1);
                                sleep(1);
                            }
                        else
                        {
                            printf("Z cannot be increased any more\n");
                            fflush(stdout);
                        }
                        break;

                    // stop
                    case 101: // case e
                    case 69: // case E
                        sprintf(passVal,format_string,z_position);
                        write(fd_z,passVal,strlen(passVal)+1);
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
                        z_position = 0;
                        sleep(1);
                        break;

                    // emergency stop
                    case 115: // case s
                    case 83: // case S
                        sprintf(passVal,format_string,z_position);
                        write(fd_z,passVal,strlen(passVal)+1);
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
        close(fd_z);
        unlink(fifo_motZinsp);
        close(fd_insp);
        unlink(fifo_inspmotZ);
        close(fd_val);
        unlink(fifo_valZ);
    }
}