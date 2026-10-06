#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
printSixFive(int num)
{
  if (num == -1)
    return;
  if (num % 5 == 0 || num % 6 == 0) {
    printf("%d\n", num);
  }
}

int
isdigit(char c)
{
  if (c >= '0' && c <= '9') {
    return 1;
  }
  return 0;
}

void
processInput(int fd)
{
  char punc[] = " -\r\t\n./,";

  // set to -1 to skip case non digit is at index 0
  int num = -1;

  char c;

  // check if current num is valid
  int acptNum = 1;

  int n;
  while ((n = read(fd, &c, 1)) > 0) {
    if (c == '\0') {
      continue;
    } else if (strchr(punc, c)) {
      if (acptNum) {
        printSixFive(num);
      }
      acptNum = 1;
      num = -1;
    } else if (!isdigit(c) || !acptNum) {
      // mark invalid num
      acptNum = 0;
    } else {
      if (num == -1)
        num = 0;
      num = num * 10 + c - '0';
    }
  }

  // process trailing num
  if (acptNum) {
    printSixFive(num);
  }
}

int
main(int argc, char *argv[])
{
  int fd;

  // receive input directly from standard input
  if (argc == 1) {
    processInput(0);
  }
  // receive from file
  else {
    int i = 0;
    while (++i < argc) {
      if ((fd = open(argv[i], O_RDONLY)) < 0) {
        fprintf(2, "sixfive: cannot open %s\n", argv[i]);
        continue;
      }
      processInput(fd);
      close(fd);
    }
  }

  exit(0);
}
