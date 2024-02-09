#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// print pid and then demonize c program
void demonize() {
  pid_t pid, sid;

  // for parent
  pid = fork();
  
  // exit parent
  if (pid < 0)
    exit(EXIT_FAILURE);
  if (pid > 0)
    exit(EXIT_SUCCESS);

  sid = setsid();
  if (sid < 0)
    exit(EXIT_FAILURE);

  // change current dir to root
  if (chdir("/") < 0)
    exit(EXIT_FAILURE);

  // redirect standart files to /dev/null
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);
}

void sigurg_handler(int signum) {
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

  if (signal(SIGURG, sigurg_handler) == SIG_ERR) {
    exit(EXIT_SUCCESS);
  }

  char path[256];
  snprintf(path, sizeof(path), "/proc/self/stat");
  FILE * file = fopen(path, "r");
  if (file == NULL) {
    perror("error while opening file");
    exit(EXIT_FAILURE);
  }

  int pid, ppid;
  char comm[256];
  fscanf(file, "%d %s %*c %d", &pid, comm, &ppid);
  printf("%d\n", ppid);

  demonize();

  while (1) {
    sleep(5);
  }

  return EXIT_SUCCESS;
}
