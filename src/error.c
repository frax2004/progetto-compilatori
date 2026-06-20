#include "compiler.h"
#include <stdarg.h>
#include <stdio.h>
#include "parser.h"

#define RGB(r, g, b) "\033[38;2;"#r";"#g";"#b"m"


#define RED RGB(227, 61, 61)
#define GREEN RGB(121, 237, 121)
#define BLUE RGB(21, 163, 224)
#define PURPLE RGB(220, 11, 199)
#define YELLOW RGB(255, 215, 0)
#define ORANGE RGB(253, 160, 40)
#define WHITE RGB(255, 255, 255)
#define GRAY RGB(100, 100, 100)
#define RESET "\033[0m"
#define BOLD "\033[1m"




const char* ErrorTypeToString(ErrorType kind) {
  switch(kind) {
    case LEXICAL_ERROR: return "Lexical Error";
    case SYNTAX_ERROR: return "Syntax Error";
    case SEMANTIC_ERROR: return "Semantic Error";
    default: return "<Unkown-Error>";
  }
}


void emitError(ErrorType kind, const char* fmt, ...) {
  CompilationContext* ctx = getCompilationContext();

  extern int yylineno;

  fprintf(
    stderr,
    "[%s%s%s%s] in file %s%s:%d%s: ", 
    BOLD,
    RED, 
    ErrorTypeToString(kind), 
    RESET, 
    GRAY,
    ctx->inputPath, 
    yylineno,
    RESET
  );

  fprintf(stderr, "%s", RED);
  
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  fprintf(stderr, "%s\n", RESET);
  fflush(stderr);
}

void yyerror(const char* msg) {
  emitError(SYNTAX_ERROR, msg);
}
