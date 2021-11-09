#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

float x_position = 0;
char comm_pid[80];
char str_p[80];
char format_str_p[80] = "%d";
int fd;

int v = 0; // value to change according to the signal

void sig_handler_1(int signo)
{
    if(signo == SIGUSR1)
    {
        v = 1;
        printf("Right or Left pressed, signal SIGUSR1, v = %d\n",v);
        fflush(stdout);
    }
    else if(signo == SIGUSR2)
    {
        v = 0;
        printf("Stop pressed, signal SIGUSR2, v = %d\n",v);
        fflush(stdout);
    }
    else
    {
        v = -1;
        printf("Inside else sig_handler, v = %d\n",v);
        fflush(stdout);
    }
}

int main(int argc, char * argv[])
{
    struct sigaction sa;
    sa.sa_handler = sig_handler_1;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    char * fifo_mot_commX = "/tmp/comm_motX";
    char * fifo_valX = "/tmp/fifo_valX";
    mkfifo(fifo_mot_commX,0666);
    mkfifo(fifo_valX,0666);

    // send the process pid via pipe to the command console
    fd = open(fifo_mot_commX,O_WRONLY);
    sprintf(str_p,format_str_p,(int)getpid());
    write(fd,str_p,strlen(str_p)+1);
    close(fd);

    while(1)
    {
        int fd;
        char input_string[80];

        //get the command from the command console
        fd = open(fifo_valX, O_RDONLY);
        read(fd, input_string, 80);
        close(fd);

        while(v)
        {
            if(atoi(input_string) == 100)
            {
                if(x_position < 5)
                {
                    x_position += 0.25;
                    printf("X position = %f\n",x_position);
                    fflush(stdout);
                    sleep(1);
                }
                else
                {
                    printf("X position cannot be incremented any more!\n");
                    fflush(stdout);
                }
            }
            else if(atoi(input_string) == 97)
            {
                if(x_position > 0)
                {
                    x_position -= 0.25;
                    printf("X position = %f\n",x_position);
                    fflush(stdout);
                    sleep(1);
                }
                else
                {
                    printf("X position cannot be decreased any more!\n");
                    fflush(stdout);
                    v = 0;
                }
            }
            else if(atoi(input_string) == 115)
            {
                printf("STOP!\n");
                fflush(stdout);
                break;
            }
            else if(atoi(input_string) == 114)
            {
                printf("Resetting ... \n");
                fflush(stdout);
                x_position = 0;
                printf("X position = %f\n",x_position);
                fflush(stdout);
                break;
            }
        }
    }
}