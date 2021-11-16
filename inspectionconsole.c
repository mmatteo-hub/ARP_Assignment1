#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int fd_x_write, fd_z_write;
int fd_x_read, fd_z_read;

char input_string_x[80];
char input_string_z[80];
char format_string[80] = "%f";

int main(int argc, char * argv[])
{
    char * myfifo_inspection = "/tmp/fifo_inspection";

    // the following pipes are used to send signals of reset and stop to the motors
    char * fifo_inspmotX = "/tmp/insp_motX";
    char * fifo_inspmotZ = "/tmp/insp_motZ";

    // the following pipes are used to read values of x and z from the motors
    char * fifo_motXinsp = "/tmp/motX_insp";
    char * fifo_motZinsp = "/tmp/motZ_insp";

    mkfifo(myfifo_inspection, 666);
    mkfifo(fifo_inspmotX, 0666);
    mkfifo(fifo_inspmotZ, 0666);
    mkfifo(fifo_motXinsp, 0666);
    mkfifo(fifo_motZinsp, 0666);

    char ch1[80];
    char var;

    while(1)
    {
        printf("dentro\n");
        fflush(stdout);
        fd_x_read = open(fifo_motXinsp, O_RDONLY);
        printf("dentro x read\n");
        fflush(stdout);
        read(fd_x_read, input_string_x, 80);
        close(fd_x_read);

        fd_z_read = open(fifo_motZinsp, O_RDONLY);
        printf("dentro z read\n");
        fflush(stdout);
        read(fd_z_read, input_string_z, 80);
        close(fd_z_read);

        fd_x_write = open(fifo_inspmotX, O_WRONLY);
        printf("dentro x write\n");
        fflush(stdout);
        fd_z_write = open(fifo_inspmotZ, O_WRONLY);
        printf("dentro z write\n");
        fflush(stdout);

        printf("X = %s\nZ = %s\n", input_string_x, input_string_z);
        fflush(stdout);
        printf("PRESS: \n s to STOP both motors for an emergency\n r to RESET both motors\n");
        fflush(stdout);
        scanf("%s", ch1);

        if(strlen(ch1) != 1)
        {
            printf("Wrong input. Input has to be 1 character only!\n");
            fflush(stdout);
        }

        else
        {
            char out_str[80];
            sprintf(out_str, format_string, ch1[0]);
            var = ch1[0];
            switch(var)
            {
                case 114:
                    printf("RESET WAS PRESSED\n");
                    fflush(stdout);
                    write(fd_x_write, out_str, strlen(out_str)+1);
                    write(fd_z_write, out_str, strlen(out_str)+1);
                    break;

                case 115:
                    printf("EMERGENCY STOP WAS PRESSED\n");
                    fflush(stdout);
                    write(fd_x_write, out_str, strlen(out_str)+1);
                    write(fd_z_write, out_str, strlen(out_str)+1);

                default:
                    printf("Wrong input, key pressed: %c\n", var);
                    fflush(stdout);
                    break;
            }
        }
        close(fd_z_write);
        close(fd_x_write);
    }
}