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

#define FOR(i, a, b) for(int i = a; i < b; ++i)



const char* ErrorTypeToString(ErrorType kind) {
  switch(kind) {
    case LEXICAL_ERROR: return "Lexical Error";
    case SYNTAX_ERROR: return "Syntax Error";
    case SEMANTIC_ERROR: return "Semantic Error";
    default: return "<Unkown-Error>";
  }
}

void emitNote(YYLTYPE where, const char* fmt, ...) {
  CompilationContext* ctx = getCompilationContext();

  fprintf(
    stderr,
    "[%s%sNote%s] in file %s%s:%d:%d%s: ", 
    BOLD,
    BLUE,  
    RESET, 
    GRAY,
    ctx->inputPath, 
    where.first_line,
    where.first_column,
    RESET
  );

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  fprintf(stderr, "\n");
  fflush(stderr);
}

void emitError(ErrorType kind, YYLTYPE where, const char* fmt, ...) {
  CompilationContext* ctx = getCompilationContext();
  ctx->frontEndOk = 0;

  fprintf(
    stderr,
    "[%s%s%s%s] in file %s%s:%d:%d%s:\n", 
    BOLD,
    RED, 
    ErrorTypeToString(kind), 
    RESET, 
    GRAY,
    ctx->inputPath, 
    where.first_line,
    where.first_column,
    RESET
  );

  fprintf(stderr, " %s%6d |%s", GRAY, where.first_line, RESET);
  FOR(i, 0, where.first_column) fputc(' ', stderr);
  fprintf(stderr, "%s", RED);

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  fprintf(stderr, "%s\n", RESET);
  fprintf(stderr, " %s%6d |%s", GRAY, where.first_line+1, RESET);
  FOR(i, 0, where.first_column) fputc(' ', stderr);
  fprintf(stderr, RED);
  FOR(i, where.first_column, where.last_column) fputc('~', stderr);
  fprintf(stderr, "^\n");
  fprintf(stderr, RESET);

  fflush(stderr);
}


int yypcontext_token(const void* ctx);
YYLTYPE* yypcontext_location(const void* ctx);
yypcontext_expected_tokens(const void* yyctx, int yyarg[], int yyargn);


int yyreport_syntax_error(const void* yyctx) {
  int token_kind = yypcontext_token(yyctx);
  YYLTYPE where = yypcontext_location(yyctx) != NULL ? *yypcontext_location(yyctx) : (YYLTYPE) {};
  
}

void yyerror(const char* msg) {
  extern int yylineno;

  emitError(
    SYNTAX_ERROR, 
    (YYLTYPE) {
      .first_line = yylineno,
      .last_line = yylineno,
      .first_column = 0,
      .last_line = 0
    }, 
    msg
  );
}
