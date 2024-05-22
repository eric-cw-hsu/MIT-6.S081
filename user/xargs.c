#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define stdin 0
#define stderr 2
#define MAX_ARG_SIZE 1024

void execCmd(char *argv[])
{
  int pid = fork();
  if (pid == 0)
  {
    exec(argv[0], argv);
    exit(0);
  }
  else
  {
    wait(0);
  }
}

int main(int argc, char *argv[])
{

  if (argc < 2)
  {
    fprintf(2, "xargs: too few arguments\n");
    exit(1);
  }

  int argNum = 1, startIdx = 0;
  char *newArgv[MAXARG];

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-n") == 0)
    {
      if (i + 1 >= argc)
      {
        fprintf(stderr, "xargs: option requires an argument -- n\n");
        exit(1);
      }

      if ((argNum = atoi(argv[i + 1])) == 0)
      {
        fprintf(stderr, "xargs: -n %s: invalid\n", argv[i + 1]);
        exit(1);
      }
      i += 2;
      startIdx = 0;
    }

    newArgv[startIdx++] = argv[i];
  }

  if (startIdx + argNum > MAXARG)
  {
    fprintf(stderr, "xargs: too many arguments\n");
    exit(1);
  }

  char buf, arg[MAX_ARG_SIZE];
  int argIdx = 0, argNumCnt = 0, n;
  while ((n = read(stdin, &buf, 1)) > 0)
  {
    if (buf == ' ' || buf == '\n')
    {
      newArgv[startIdx + argNumCnt] = (char *)malloc(sizeof(char) * (argIdx + 1));
      memmove(newArgv[startIdx + argNumCnt], arg, argIdx);
      argNumCnt++;
      argIdx = 0;

      if (argNumCnt == argNum)
      {

        execCmd(newArgv);
        argNumCnt = 0;
      }
    }
    else
    {
      arg[argIdx++] = buf;
    }
  }

  if (n < 0)
  {
    fprintf(stderr, "xargs: read error\n");
    exit(1);
  }

  if (argNumCnt != 0)
  {
    for (int i = argNumCnt; i < argNum; i++)
    {
      free(newArgv[startIdx + i]);
      newArgv[startIdx + i] = 0;
    }
    execCmd(newArgv);
  }

  exit(0);
}
