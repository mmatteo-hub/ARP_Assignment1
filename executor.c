#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

#define BUFFSIZE 80

// function to start the execution of the processes
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
  int fd_watchdog;
  int fd_inspection;

  pid_t child_pid;

  // pipe communicating with the inspection console
  //char * myfifo_inspection = "/tmp/fifo_inspection";

  // pipe communicating with the command console
  char * myfifo_command = "/tmp/fifo_command";

  // pipes communicating with motors
  char * myfifo_motorX = "/tmp/fifo_motX";
  char * myfifo_motorZ = "/tmp/fifo_motZ";

  // pipe to pass pids of processes to the watchdog process
  //char * myfifo_watchdog = "/tmp/fifo_watchdog";

  //mkfifo(myfifo_inspection,0666);
  mkfifo(myfifo_command,0666);
  mkfifo(myfifo_motorX,0666);
  mkfifo(myfifo_motorZ,0666);
  //mkfifo(myfifo_watchdog,0666);

  char input_string[BUFFSIZE];
  char outup_string[80];
  char motor_pids_string[80];
  char out_str[80];
  char format_string[80] = "%d,%d";
  
  // creation of 5 processes
  int pid1, pid2, pid3, pid4, pid5;
  
  // execl with the params for the konsoles
  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid1 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole", "/usr/bin/konsole", "--hold",  "-e", "./commandconsole", (char*) NULL) == -1) perror("exec failed");
	exit(1);
  }
  printf("1st konsole (PID = %d)\n", pid1);
  fflush(stdout);
  
  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid2 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole", "/usr/bin/konsole", "--hold",  "-e", "./inspectionconsole", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("2nd konsole (PID = %d)\n", pid2);
  fflush(stdout);

  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid3 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole","/usr/bin/konsole",  "--hold", "-e", "./motorX", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("3rd konsole (PID = %d)\n", pid3);
  fflush(stdout);

  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid4 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole","/usr/bin/konsole",  "--hold", "-e", "./motorZ", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("4th konsole (PID = %d)\n", pid4);
  fflush(stdout);

  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid5 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole","/usr/bin/konsole",  "--hold", "-e", "./watchdog", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("5th konsole (PID = %d)\n", pid5);
  fflush(stdout);
  // saving all 5 processes' pids
  //sprintf(outup_string, format_string, pid3, pid4);

  // passing pids to watchdog via pipe
  /*fd_watchdog = open(myfifo_watchdog, O_WRONLY);
  write(fd_watchdog, outup_string, strlen(outup_string)+1);
  close(fd_watchdog);*/

  // passing pids to inspection via pipe
  /*fd_inspection = open(myfifo_inspection, O_WRONLY);
  write(fd_inspection, outup_string, strlen(outup_string)+1);
  close(fd_inspection);
*/
  printf ("Main program exiting...\n");
  fflush(stdout);
  return 0;
}