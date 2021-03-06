#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>

#define increment 0.0025 //mm
#define max_err 0.0001 // mm
#define max_pos 10 // m
#define min_pos 0 // m

#define USEC 10000

FILE *f;
time_t clk;

// definintion for all variables used inside the program
float z_position = 0; 
int fd_valZ, fdZ_write;
char pid_string[80];
char passVal[80];
int sig = 0;
char pid[80];
int s;
int retval;
char input_string[80];
char input_str;
int z_pid;
int z_pid_w;
double err;

// defining format string to prepare the string for the pipe
char format_pid_string[80] = "%d";
char format_string[80] = "%f";

// error on position
double randomErr()
{
    srand(time(NULL));
    return (((double)rand()) / ((double)RAND_MAX)) * (double)max_err;
}

// sign
int sign()
{
    srand(time(NULL));
    float n = (double)rand() / (double)RAND_MAX;
    if(n >  0.5) return 1;
    else return 0;
}

// signal handler
void sig_handler(int signo)
{
    // opening the log file in append mode to add to the existing file
    f = fopen("./../log/logfile.txt","a");

    // depenfing on which signal has arrived the handler set a different value of sig to make the motor do the action choosen
    if (signo == SIGUSR1)
    {
        fseek(f,0,SEEK_END);
        clk = time(NULL);
        fprintf(f,"Motor Z, RESET signal (SIGUSR1) received at : %s",ctime(&clk));
        fflush(f);
        sig = 1; // reset
    }
    else if(signo == SIGUSR2)
    {
        fseek(f,0,SEEK_END);
        clk = time(NULL);
        fprintf(f,"Motor Z, STOP signal (SIGUSR2) received at : %s",ctime(&clk));
        fflush(f);
        sig = 2; // stop
    }
}

int main(int argc, char * argv[])
{
    // opening the log file in append mode to add to the existing file
    f = fopen("./../log/logfile.txt","a");

    // signals from inspection
    signal(SIGUSR1,sig_handler);
    signal(SIGUSR2,sig_handler);

    char * fifo_valZ = "/tmp/fifo_valZ";
    char * fifo_motZinsp = "/tmp/motZ_insp";
    char * pid_motZ = "/tmp/pid_motZ";
    char * pid_motZ_watchdog = "/tmp/pid_motZ_watch";

    char pid[80];

    // writing pid
    z_pid = open(pid_motZ, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(z_pid, pid, strlen(pid)+1);
    close(z_pid);
    z_pid_w = open(pid_motZ_watchdog, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(z_pid_w, pid, strlen(pid)+1);
    close(z_pid_w);

    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    // open pipe
    fd_valZ = open(fifo_valZ,O_RDONLY);
    fdZ_write = open(fifo_motZinsp, O_WRONLY);

    while(1)
    {   
        s = sign();
        err = randomErr();

        FD_ZERO(&rfds);
        FD_SET(fd_valZ,&rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 0;

         // select to detect if a new command has arrived
        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        // switch the value for retval
        switch(retval)
        {
             // computing the x position according to an error err and a sign s
            case -1: // select error
                fseek(f,0,SEEK_END);
                clk = time(NULL);
                fprintf(f,"Motor Z, error during program at : %s",ctime(&clk));
                fflush(f);
                perror("select()");
                fflush(stdout);
                break;

            case 0: // no new value
                if(sig || sig == 2) break;
                else
                {
                    switch(atoi(input_string))
                    {
                        // down
                        case 90: // Z
                        case 122: // z
                            if(z_position - (increment + err) > min_pos)
                            {
                                if(s)
                                {
                                    z_position -= (increment+err);
                                    sprintf(passVal,format_string,z_position);
                                    write(fdZ_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                else if(!s)
                                {
                                    z_position -= (increment-err);
                                    sprintf(passVal,format_string,z_position);
                                    write(fdZ_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor Z, decreasing position, Z = %f m at : %s",z_position,ctime(&clk));
                                fflush(f);
                            }
                            else
                            {
                                z_position = min_pos;
                                sprintf(passVal,format_string,z_position);
                                write(fdZ_write,passVal,strlen(passVal)+1);
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor Z, lower limit reached, Z = %f m at : %s",z_position,ctime(&clk));
                                fflush(f);
                                strcpy(input_string,"");
                            }
                            break;

                        // up
                        case 87: // W
                        case 119: // w
                            if(z_position + (increment + err) < max_pos)
                            {
                                if(s)
                                {
                                    z_position += (increment + err);
                                    sprintf(passVal,format_string,z_position);
                                    write(fdZ_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                else if(!s)
                                {
                                    z_position += (increment - err);
                                    sprintf(passVal,format_string,z_position);
                                    write(fdZ_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor Z, increasing position, Z = %f m at : %s",z_position,ctime(&clk));
                                fflush(f);
                            }
                            else
                            {
                                z_position = max_pos;
                                sprintf(passVal,format_string,z_position);
                                write(fdZ_write,passVal,strlen(passVal)+1);
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor Z, upper limit reached, Z = %f m  at : %s",z_position,ctime(&clk));
                                fflush(f);
                                strcpy(input_string,"");
                            }
                            break;

                        // stop Z
                        case 69: // E
                        case 101: // e
                            fseek(f,0,SEEK_END);
                            clk = time(NULL);
                            fprintf(f,"Motor Z, stopping position, Z = %f m at : %s",z_position,ctime(&clk));
                            fflush(f);
                            usleep(USEC);
                            break;
                        
                        default:
                            break;
                    }
                }
                break;

            default: // got a new value
                read(fd_valZ, input_string, 80);
                sig = 0;
                break;
        }

        // switching the value of sig determined by the signal handler
        switch(sig)
        {
            case 1: // reset
                z_position = 0;
                sprintf(passVal,format_string,z_position);
                write(fdZ_write,passVal,strlen(passVal)+1);
                usleep(USEC);
                break;
            
            case 2: // stop
                break;
            
            default:
                break;
        }
    }
    close(fdZ_write);
    close(fd_valZ);
}