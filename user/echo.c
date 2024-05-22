#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char *formatString(char *argv)
{
  if (argv[0] == '"' && argv[strlen(argv) - 1] == '"')
  {
    char *res = (char *)malloc(sizeof(char) * (strlen(argv) - 1));

    int resIdx = 0;
    for (int i = 1; i < strlen(argv) - 1; i++)
    {
      if (argv[i] == '\\' && argv[i + 1] == 'n')
      {
        res[resIdx++] = '\n';
        i++;
      }
      else
      {
        res[resIdx++] = argv[i];
      }
    }
    return res;
  }

  return argv;
}

int main(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++)
  {
    argv[i] = formatString(argv[i]);
    write(1, argv[i], strlen(argv[i]));
    if (i + 1 < argc)
    {
      write(1, " ", 1);
    }
    else
    {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
