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
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * fifo_motZinsp = "/tmp/motZ_insp";
    mkfifo(myfifo_inspection,0666);
    mkfifo(fifo_inspmotX,0666);
    mkfifo(fifo_inspmotZ,0666);

    char input_string_x[80];
    char input_string_z[80];
    char format_string[80] = "%f";

    while(1)
    {
        int fd_x = open(fifo_inspmotX, O_RDONLY);
        int fd_z = open(fifo_inspmotZ, O_RDONLY);
        read(fd_x, input_string_x, 80);
        read(fd_z, input_string_z, 80);
        printf("X = %s\nZ = %s\n",input_string_x, input_string_z);
        fflush(stdout);
        close(fd_z);
        close(fd_x);
    }
}