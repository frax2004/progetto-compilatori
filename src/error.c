#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "compiler.h"
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

#define MAX_EXPECTED_TOKENS_COUNT 8

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

int reportSyntaxError(const void* yyctx) {
  CompilationContext* ctx = getCompilationContext();
  ctx->frontEndOk = 0;

  int token_kind = getCurrentToken(yyctx);
  const char* lexeme = getSymbolName(token_kind);
  YYLTYPE where = getCurrentTokenLocation(yyctx) != NULL ? *getCurrentTokenLocation(yyctx) : (YYLTYPE) {};

  int expectedBuffer[MAX_EXPECTED_TOKENS_COUNT];
  int expectedCount = getExpectedTokens(yyctx, expectedBuffer, MAX_EXPECTED_TOKENS_COUNT);

  int lookahead = getCurrentToken(yyctx);
  const char* unexpected = lookahead >= 0 ? getSymbolName(lookahead) : "EPSILON";
  
  where.last_column = where.first_column + strlen(unexpected) - 1;

  emitError(SYNTAX_ERROR, where, "%s", unexpected);

  int res = 0;
  if(expectedCount < 0) {
    // Esaurimento della memoria, fai gestire l'errore a bison
    res = expectedCount;
  } else {
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

    fprintf(stderr, "Expected one of these tokens: [");

    FOR(i, 0, expectedCount) fprintf(
      stderr,
      "%s%s%s%s%s",
      BOLD,
      RGB(39, 174, 125),
      getSymbolName(expectedBuffer[i]),
      RESET,
      i < expectedCount - 1 ? ", " : ""
    );


    fprintf(stderr, "], got %s%s%s%s\n", BOLD, RED, unexpected, RESET);
  }
  
  fflush(stderr);
  return res;
}

void yyerror(const char* msg) {
  CompilationContext* ctx = getCompilationContext();
  ctx->frontEndOk = 0;

  fprintf(stderr, "[%s%sBuy more RAM! (lol)%s]: %s", RED, BOLD, RESET, msg);
}
