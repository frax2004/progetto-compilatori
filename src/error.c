#include "error.h"
#include <stdarg.h>
#include <stdio.h>


const char* ErrorTypeToString(ErrorType kind) {
  switch(kind) {
    case LEXICAL_ERROR: return "Lexical Error";
    case SYNTAX_ERROR: return "Syntax Error";
    case SEMANTIC_ERROR: return "Semantic Error";
    default: return "<Unkown-Error>";
  }
}

void emitError(ErrorType kind, const char* fmt, ...) {
  extern int yylineno;

  printf("%s At line %d:", ErrorTypeToString(kind), yylineno);
  
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  putchar('\n');
}

void yyerror(const char* msg) {
  emitError(SYNTAX_ERROR, msg);
}
