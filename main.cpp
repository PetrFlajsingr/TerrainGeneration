#include "marching_cubes.h"
#include <io/print.h>

int main(int argc, char *argv[]) {
  printT(LogLevel::Info, "Start");
  main_marching_cubes(argc, argv);
  return 0;
}
