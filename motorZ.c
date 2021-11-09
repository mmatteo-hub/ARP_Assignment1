#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

float z_position = 0;

int main(int argc, char * argv[])
{
    while(1)
    {
        char * fifo_mot_commZ = "/tmp/comm_motZ";
        mkfifo(fifo_mot_commZ,0666);

        int fd;
        char input_string[80];

        fd = open(fifo_mot_commZ, O_RDONLY);
        read(fd, input_string, 80);
        close(fd);
        if(atoi(input_string) == 119)
        {
            if(z_position < 5)
            {
                z_position += 1;
                printf("Z position = %f\n",z_position);
                fflush(stdout);
            }
            else
            {
                printf("Z position cannot be incremented any more!\n");
                fflush(stdout);
            }
        }
        else if(atoi(input_string) == 122)
        {
            if(z_position > 0)
            {
                z_position -= 1;
                printf("Z position = %f\n",z_position);
                fflush(stdout);
            }
            else
            {
                printf("Z position cannot be decreased any more!\n");
                fflush(stdout);
            }
        }
    }
}