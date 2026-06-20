#include "compiler.h"



int main(int argc, const char** argv) {
  if(argc < 2) return -1;

  compile(argv[1]);
  
  return 0;
}