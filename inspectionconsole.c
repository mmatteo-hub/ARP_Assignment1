#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int main(int argc, char * argv[])
{
    char * myfifo_inspection = "/tmp/fifo_inspection";
    char * fifo_inspmotX = "/tmp/insp_motX";
    char * fifo_inspmotZ = "/tmp/insp_motZ";
    mkfifo(myfifo_inspection,0666);
    mkfifo(fifo_inspmotX,0666);
    mkfifo(fifo_inspmotZ,0666);

    char input_string[80];
    float valX;
    char format_string[80] = "%f";

    while(1)
    {
        int fd_x = open(fifo_inspmotX, O_RDONLY);
        read(fd_x, input_string, 80);
        sscanf(input_string, format_string, &valX);
        printf("X = %s\n",input_string);
        fflush(stdout);
        printf("X con scanf = %f\n",valX);
        fflush(stdout);
        close(fd_x);
    }
}