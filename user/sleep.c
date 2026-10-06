#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // if argument is not 2
  if (argc != 2) {
    fprintf(2, "Usage: sleep <ticks> \n");
    exit(1);
  }

  // convert char -> int for sleep time
  int time = atoi(argv[1]);

  pause(time);
  exit(0);
}
