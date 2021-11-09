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

int v; // value to change according to the signal

void sig_handler(int signo)
{
    if(signo == SIGUSR1) v = 1;
    else v = 0;
}

int main(int argc, char * argv[])
{
    signal(SIGUSR1, sig_handler);
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
                if(x_position < 50)
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
            }
        }
    }
}