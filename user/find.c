#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

#define stdin 0
#define stdout 1
#define stderr 2

#define PIPE_READ 0
#define PIPE_WRITE 1

char *getFileNameFromPath(char *path)
{
  char *buf;
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;

  buf = (char *)malloc(sizeof(char) * (strlen(p) + 1));
  memmove(buf, p, strlen(p));
  return buf;
}

char *joinPath(char *parPath, char *subPath)
{
  int n = strlen(parPath), m = strlen(subPath);
  char *path = (char *)malloc(sizeof(char) * (n + m + 2));

  memmove(path, parPath, n);
  if (parPath[n - 1] != '/' && subPath[0] != '/')
  {
    path[n] = '/';
    memmove(path + n + 1, subPath, m);
    path[n + 1 + m] = '\0';
  }
  else
  {
    memmove(path + n, subPath, m);
    path[n + m] = '\0';
  }

  return path;
}

__attribute__((noreturn)) void find(int leftPipe[2], char *targetFileName)
{
  int fd, pid;
  struct stat st;
  struct dirent de;
  char path[128], *subPath;

  // read path from pipe
  close(leftPipe[PIPE_WRITE]);
  if (read(leftPipe[PIPE_READ], path, sizeof(path)) < 0)
  {
    fprintf(stderr, "find: read error...\n");
    close(leftPipe[PIPE_READ]);
    exit(1);
  }
  close(leftPipe[PIPE_READ]);

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    exit(1);
  }

  switch (st.type)
  {
  case T_FILE:
    if (strcmp(getFileNameFromPath(path), targetFileName) == 0)
    {
      write(1, path, strlen(path) + 1);
      write(1, "\n", 1);
    }
    break;
  case T_DIR:
    // open dir
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;

      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      int rightPipe[2];
      pipe(rightPipe);
      if ((pid = fork()) == 0)
      {
        find(rightPipe, targetFileName);
      }
      else
      {
        subPath = joinPath(path, de.name);
        write(rightPipe[PIPE_WRITE], subPath, strlen(subPath) + 1);
        close(rightPipe[PIPE_READ]);
        close(rightPipe[PIPE_WRITE]);
      }
    }

    break;
  }

  close(fd);
  exit(0);
}

int main(int argc, char *argv[])
{
  int p[2], pid;
  // check arguments
  if (argc < 3)
  {
    fprintf(2, "find: too few arguments\n");
    exit(-1);
  }

  pipe(p);
  if ((pid = fork()) == 0)
  {
    find(p, argv[2]);
  }
  else
  {
    write(p[PIPE_WRITE], argv[1], strlen(argv[1]));
    close(p[PIPE_WRITE]);
    close(p[PIPE_READ]);
    wait(0);
  }

  exit(0);
}