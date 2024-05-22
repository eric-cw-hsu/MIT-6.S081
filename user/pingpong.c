#include "kernel/types.h"
#include "user/user.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

#define stdin 0
#define stdout 1
#define stderr 2

int main(int argc, char *argv[])
{
  char buffer[5];
  int p[2], pid;
  pipe(p);

  if ((pid = fork()) == 0)
  {
    read(p[PIPE_READ], buffer, 4);
    printf("%d: received %s\n", getpid(), buffer);
    close(p[PIPE_READ]);

    write(p[PIPE_WRITE], "pong", 4);
    close(p[PIPE_WRITE]);

    exit(0);
  }
  else
  {
    write(p[PIPE_WRITE], "ping", 4);
    close(p[PIPE_WRITE]);

    read(p[PIPE_READ], buffer, 4);
    printf("%d: received %s\n", getpid(), buffer);
    close(p[PIPE_READ]);

    wait(0);
    exit(0);
  }
}
