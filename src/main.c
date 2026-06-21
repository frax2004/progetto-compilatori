#include "compiler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void help();
void version();

int main(int argc, const char** argv) {
  if(argc < 2) {
    help();
  }
  
  const char* output = "out.txt";

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) version();
    else if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-H") == 0) help();
    else if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if(i+1 >= argc) {
        fprintf(stderr, "Missing output file.");
        exit(-1);
      }
      output = argv[i+1];
    }
  }

  compile(argv[1], output);
  
  return 0;
}


void help() {
  fprintf(
    stdout,
    "Usage: compiler.exe <input> [<options>]\n"
    "\n"
    "  Commands:\n"
    "  -H, --help           Display this page.\n"
    "  -v, --version        Display version info page.\n"
    "  -o, --output <file>  Specify an output file.\n"
    "\n"
  );

  exit(0);
}

void version() {
  fprintf(
    stdout,
    "Compiler (built by Francesco Pollara' & Giovanni Butera on 06-2026) 1.0.0.\n"
    "This software is released under the GNU GPLv3 licence.\n"
    "This is free software; see the source for copying conditions.  There is NO\n"
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
  );
  exit(0);
}
