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
#define max_pos 5 // m
#define min_pos 0 // m

#define USEC 10000

FILE *f;
time_t clk;

float x_position = 0; // motorX positiion

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

int fd_valX, fdX_write;
char pid_string[80];
char format_pid_string[80] = "%d";

char passVal[80];
char format_string[80] = "%f";

int sig = 0;

// signals from inspection
void sig_handler(int signo)
{
    f = fopen("./../log/logfile.txt","a");
    if (signo == SIGUSR1)
    {
        fseek(f,0,SEEK_END);
        clk = time(NULL);
        fprintf(f,"Motor X, RESET signal (SIGUSR1) received at : %s",ctime(&clk));
        fflush(f);
        sig = 1; // reset
    }
    else if(signo == SIGUSR2)
    {
        fseek(f,0,SEEK_END);
        clk = time(NULL);
        fprintf(f,"Motor X, STOP signal (SIGUSR2) received at : %s",ctime(&clk));
        fflush(f);
        sig = 2; // stop
    }
}

int main(int argc, char * argv[])
{
    f = fopen("./../log/logfile.txt","a");

    // signals from inspection
    signal(SIGUSR1,sig_handler);
    signal(SIGUSR2,sig_handler);

    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * pid_motX = "/tmp/pid_motX";
    char * pid_motX_watchdog = "/tmp/pid_motX_watch";
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_motXinsp,0666);
    mkfifo(pid_motX,0666);
    mkfifo(pid_motX_watchdog,0666);

    char pid[80];

    // writing pid
    int x_pid = open(pid_motX, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(x_pid, pid, strlen(pid)+1);
    close(x_pid);
    int x_pid_w = open(pid_motX_watchdog, O_WRONLY);
    sprintf(pid, format_pid_string, (int)getpid());
    write(x_pid_w, pid, strlen(pid)+1);
    close(x_pid_w);

    // initialise struct for the select
    fd_set rfds;
    struct timeval tv;
    int retval;

    char input_string[80];
    char input_str;

    // open pipe
    fd_valX = open(fifo_valX,O_RDONLY);
    fdX_write = open(fifo_motXinsp, O_WRONLY);

    while(1)
    {   
        int s = sign();
        double err = randomErr();

        FD_ZERO(&rfds);
        FD_SET(fd_valX,&rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        retval = select(FD_SETSIZE+1,&rfds,NULL,NULL,&tv);

        switch(retval)
        {
            case -1: // select error
                fseek(f,0,SEEK_END);
                clk = time(NULL);
                fprintf(f,"Motor X, error during program at : %s",ctime(&clk));
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
                        // left
                        case 65: // A
                        case 97: // a
                            if(x_position - (increment + err) > min_pos)
                            {
                                if(s)
                                {
                                    x_position -= (increment+err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                else if(!s)
                                {
                                    x_position -= (increment-err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor X, decreasing position, X = %f m at : %s",x_position,ctime(&clk));
                                fflush(f);
                            }
                            else
                            {
                                x_position = min_pos;
                                sprintf(passVal,format_string,x_position);
                                write(fdX_write,passVal,strlen(passVal)+1);
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor X, lower limit reached, X = %f m at : %s",x_position,ctime(&clk));
                                fflush(f);
                                strcpy(input_string,"");
                            }
                            break;

                        // right
                        case 68: // D
                        case 100: // d
                            if(x_position + (increment + err) < max_pos)
                            {
                                if(s)
                                {
                                    x_position += (increment + err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                else if(!s)
                                {
                                    x_position += (increment - err);
                                    sprintf(passVal,format_string,x_position);
                                    write(fdX_write,passVal,strlen(passVal)+1);
                                    usleep(USEC);
                                }
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor X, increasing position, X = %f m at : %s",x_position,ctime(&clk));
                                fflush(f);
                            }
                            else
                            {
                                x_position = max_pos;
                                sprintf(passVal,format_string,x_position);
                                write(fdX_write,passVal,strlen(passVal)+1);
                                fseek(f,0,SEEK_END);
                                clk = time(NULL);
                                fprintf(f,"Motor X, upper limit reached, X = %f m at : %s",x_position,ctime(&clk));
                                fflush(f);
                                strcpy(input_string,"");
                            }
                            break;

                        // stop X
                        case 81: // Q
                        case 113: // q
                            fseek(f,0,SEEK_END);
                            clk = time(NULL);
                            fprintf(f,"Motor X, stopping position, X = %f m at : %s",x_position,ctime(&clk));
                            fflush(f);
                            usleep(USEC);
                            break;
                        
                        default:
                            break;
                    }
                }
                break;

            default: // got a new value
                read(fd_valX, input_string, 80);
                sig = 0;
                break;
        }

        switch(sig)
        {
            case 1: // reset
                x_position = 0;
                sprintf(passVal,format_string,x_position);
                write(fdX_write,passVal,strlen(passVal)+1);
                usleep(USEC);
                break;
            
            case 2: // stop
                break;
            
            default:
                break;
        }
    }
    close(fdX_write);
    close(fd_valX);
    unlink(fifo_valX);
    unlink(fifo_motXinsp);
}