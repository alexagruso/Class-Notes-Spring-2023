#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** args) {
  fork();
  wait(NULL);
  printf("Process ID: %d\n", getpid());
  exit(0);

  return 0;
}