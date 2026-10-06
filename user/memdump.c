#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
printError(char c)
{
  fprintf(2, "memdump: not enough data for '%c'\n", c);
  exit(1);
}

void
checkSafeLen(int *len, int bytes, char c)
{
  if (*len - bytes < 0) {
    printError(c);
  } else
    *len -= bytes;
}

void
memdump(char *fmt, char *data, int len)
{
  for (char *p = fmt; *p != '\0'; p++) {
    if (*p == 'i') {
      checkSafeLen(&len, sizeof(int), 'i');

      printf("%d\n", *(int *)data);
      data += sizeof(int);
    }
    if (*p == 'p') {
      checkSafeLen(&len, sizeof(uint64), 'p');

      printf("%lx\n", *(uint64 *)data);
      data += sizeof(uint64);
    }
    if (*p == 'h') {
      checkSafeLen(&len, sizeof(short), 'h');

      printf("%d\n", *(short *)data);
      data += sizeof(short);
    }
    if (*p == 'c') {
      checkSafeLen(&len, sizeof(char), 'c');

      printf("%c\n", *data);
      data += sizeof(char);
    }
    if (*p == 's') {
      checkSafeLen(&len, sizeof(char *), 's');

      char *str = *(char **)data;
      printf("%s\n", str);
      data += sizeof(char *);
    }
    if (*p == 'S') {
      printf("%s\n", data);
      len = 0;
    }
  }
}
