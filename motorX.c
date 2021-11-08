#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

int main(int argc, char * argv[])
{
    char * fifo_mot_commX = "/tmp/comm_motX";
    mkfifo(fifo_mot_commX,0666);

    int fd;
    char str[80];

    fd = open(fifo_mot_commX, O_RDONLY);
    read(fd, str, 80);
    close(fd);

    

    //sscanf(str,format_string,&var);s
    printf("%c\n", atoi(str));
    sleep(30);
}