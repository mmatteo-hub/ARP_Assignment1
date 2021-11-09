#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

#define BUFFSIZE 80

int spawn(const char * program, char ** arg_list) 
{
  pid_t child_pid = fork();
  if (child_pid != 0)
    return child_pid;
  else 
  {
    execvp (program, arg_list);
    perror("exec failed");
    return 1;
  }
}

int main() 
{
  int fd;
  int fdp;
  int val;
  char * myfifo_inspection = "/tmp/fifo_inspection";
  char * myfifo_command = "/tmp/fifo_command";
  char * myfifo_motorX = "/tmp/fifo_motX";
  char * myfifo_motorZ = "/tmp/fifo_motZ";
  char * myfifo_watchdog = "/tmp/fifo_watchdog";
  mkfifo(myfifo_inspection,0666);
  mkfifo(myfifo_command,0666);
  mkfifo(myfifo_motorX,0666);
  mkfifo(myfifo_motorZ,0666);
  mkfifo(myfifo_watchdog,0666);

  char input_string[BUFFSIZE];
  char outup_string[80];
  char motor_pids_string[80];
  char out_str[80];
  char format_string_multiple[80] = "%d,%d,%d,%d,%d";
  char format_string_mot[80] = "%d,%d";

  char * arg_list_1[] = { "/usr/bin/konsole",  "-e", "./motorX"};
  char * arg_list_2[] = { "/usr/bin/konsole",  "-e", "./motorZ"};
  char * arg_list_3[] = { "/usr/bin/konsole",  "-e", "./commandconsole"};
  char * arg_list_4[] = { "/usr/bin/konsole",  "-e", "./inspectionconsole"}; 
  char * arg_list_5[] = { "/usr/bin/konsole" , "-e", "./watchdog"};
  
  // creation of 5 processes
  int pid1, pid2, pid3, pid4, pid5;
  pid1 = spawn("/usr/bin/konsole", arg_list_1);
  printf("1st konsole (PID = %d)\n", pid1);
  fflush(stdout);
  
  pid2 = spawn("/usr/bin/konsole", arg_list_2);
  printf("2nd konsole (PID = %d)\n", pid2);
  fflush(stdout);

  pid3 = spawn("/usr/bin/konsole", arg_list_3);
  printf("3rd konsole (PID = %d)\n", pid3);
  fflush(stdout);

  pid4 = spawn("/usr/bin/konsole", arg_list_4);
  printf("4th konsole (PID = %d)\n", pid4);
  fflush(stdout);

  pid5 = spawn("/usr/bin/konsole", arg_list_5);
  printf("5th konsole (PID = %d)\n", pid5);
  fflush(stdout);

  // saving all 5 processes' pids
  sprintf(outup_string, format_string_multiple, pid1, pid2, pid3, pid4, pid5);

  // passing pids to watch dog through pipe
  fd = open(myfifo_watchdog, O_WRONLY);
  write(fd, outup_string, strlen(outup_string)+1);
  close(fd);

  printf ("Main program exiting...\n");
  fflush(stdout);
  return 0;
}