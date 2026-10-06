#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

void
Execute(char *new_argv[])
{
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  } else if (pid == 0) {
    exec(new_argv[0], new_argv);

    fprintf(2, "exec failed\n");
    exit(1);
  } else {
    wait(0);
  }

  return;
}

void
find(char *path, char *target, char *execOption[], int execOptionArgc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      printf("find: path too long\n");
      close(fd);
      return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0) {
        continue;
      }

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (strcmp(de.name, target) == 0) {
        if (execOption != 0) {
          if (execOptionArgc + strlen(buf) > MAXARG) {
            printf("exec: arguments too long\n");
          } else {
            execOption[execOptionArgc] = buf;
            execOption[execOptionArgc + 1] = 0;

            Execute(execOption);
          }
        } else {
          printf("%s\n", buf);
        }
      }

      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      if (st.type == T_DIR) {
        find(buf, target, execOption, execOptionArgc);
      }
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: find <path> <name>\n");
  } else if (argc == 3) {
    find(argv[1], argv[2], 0, 0);
  } else if (argc > 3) {
    if (strcmp(argv[3], "-exec") != 0) {
      printf("Usage: find <path> <name> -exec <arguments>\n");
    } else {
      char *new_argv[MAXARG];
      char **p = new_argv;
      for (int i = 4; i < argc; i++) {
        *p++ = argv[i];
      }
      p = 0;
      find(argv[1], argv[2], new_argv, argc - 4);
    }
  }

  exit(0);
}
