#include "kernel/types.h"
#include "user/user.h"

#define MAX_N 35
#define PIPE_WRITE 1
#define PIPE_READ 0

#define stdin 0
#define stdout 1
#define stderr 2

// --------------------------------------
// p = get a number from left neighbor
// print p
// loop:
//     n = get a number from left neighbor
//     if (p does not divide n)
//         send n to right neighbor

__attribute__((noreturn))
void prime(int leftPipe[2]) {
  close(leftPipe[PIPE_WRITE]);

  int rightPipe[2], pid, p, n, readStatus;

  if ((readStatus = read(leftPipe[PIPE_READ], &p, sizeof(int))) < 0) {
    fprintf(stderr, "primes: read error\n");
    fprintf(stderr, "%d\n", p);
    exit(1);
  } else if (readStatus == 0) {
    close(leftPipe[PIPE_READ]);
    exit(0);
  }

  printf("prime %d\n", p);

  pipe(rightPipe);
  if ((pid = fork()) == 0) {
    close(leftPipe[PIPE_READ]);
    prime(rightPipe);
    exit(0);
  } else {
    close(rightPipe[PIPE_READ]);
    while (read(leftPipe[PIPE_READ], &n, sizeof(int)) == sizeof(int)) {
      if (n % p != 0) {
        write(rightPipe[PIPE_WRITE], &n, sizeof(int));
      }
    }
    close(leftPipe[PIPE_READ]);
    close(rightPipe[PIPE_WRITE]);
    wait(0);
    exit(0);
  }
}

int main(int argc, char *argv[])
{
  int p[2], pid;
  pipe(p);

  if ((pid = fork()) == 0) {
    prime(p);
    exit(0);
  } else {
    close(p[PIPE_READ]);
    for (int i = 2; i <= 35; i++) {
      write(p[PIPE_WRITE], &i, sizeof(int));
    }
    close(p[PIPE_WRITE]);
    wait(0);
    exit(0);
  }
}

void prime_old_version()
{
  int prime[MAX_N + 1], notPrimeNum, p[2];
  pipe(p);

  for (int i = 2; i <= MAX_N; i++)
  {
    if (prime[i] == 0)
    {
      printf("prime %d\n", i);

      if (fork() == 0)
      {
        for (int j = i + i; j <= MAX_N; j += i)
        {
          write(p[1], &j, sizeof(int));
        }
        exit(0);
      }
      else
      {
        wait(0);
        close(p[1]);
        while (read(p[0], &notPrimeNum, sizeof(int)) > 0)
        {
          prime[notPrimeNum] = 1;
        }
        close(p[0]);
      }
    }
  }

  exit(0);
}