#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

float x_position = 0;

int main(int argc, char * argv[])
{
    while(1)
    {
        char * fifo_mot_commX = "/tmp/comm_motX";
        mkfifo(fifo_mot_commX,0666);

        int fd;
        char input_string[80];

        fd = open(fifo_mot_commX, O_RDONLY);
        read(fd, input_string, 80);
        close(fd);
        if(atoi(input_string) == 100)
        {
            if(x_position < 5)
            {
                x_position += 1;
                printf("X position = %f\n",x_position);
                fflush(stdout);
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
                x_position -= 1;
                printf("X position = %f\n",x_position);
                fflush(stdout);
            }
            else
            {
                printf("X position cannot be decreased any more!\n");
                fflush(stdout);
            }
        }
    }
}