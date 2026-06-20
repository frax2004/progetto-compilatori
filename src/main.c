#include "compiler.h"

#include <string.h>

int main(int argc, const char** argv) {
  if(argc < 2) return -1;


  compile(argv[1], argv[2]);
  
  return 0;
}