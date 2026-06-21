%define parse.error custom
%locations

%{
  #include "compiler.h"
  #include <stdlib.h>
  #include <stdio.h>
  
  int yylex();
  void yyerror(const char *s);

%}


%union {
  int as_int;
  char* as_string;
  double as_double;
}

%token <as_int> TK_CYCLIST_CODE TK_SECONDS
%token <as_double> TK_COORDINATES
%token <as_string> TK_CITY_CODE TK_CITY_NAME TK_CYCLIST_NAME 
%token TK_ENDLINE TK_OPEN_PAR TK_CLOSE_PAR TK_COMMA TK_CYCLIST_SEP TK_SECTION_SEP

%start axiom;

%%
sec1_stmt: TK_CITY_CODE TK_CITY_NAME TK_OPEN_PAR TK_COORDINATES TK_COMMA TK_COORDINATES TK_CLOSE_PAR
{ 
  visitSec1Stmt(
    (Sec1StmtContext) {
      {$1, @1}, 
      {$2, @2}, 
      {$4, @4}, 
      {$6, @6}
    }
  ); 
};

sec2_stmt: TK_CYCLIST_CODE TK_ENDLINE TK_CYCLIST_NAME TK_ENDLINE TK_CITY_CODE
{ 
  visitSec2Stmt(
    (Sec2StmtContext) {
      {$1, @1},
      {$3, @3}, 
      {$5, @5}
    }
  ); 
};

sec3_stmt: TK_CYCLIST_CODE TK_CITY_CODE TK_SECONDS
{ 
  visitSec3Stmt(
    (Sec3StmtContext) {
      {$1, @1},
      {$2, @2}, 
      {$3, @3}
    }
  ); 
};

sec1_continue: TK_ENDLINE section1 | TK_ENDLINE;

section1: sec1_stmt sec1_continue;

sec2_continue: TK_ENDLINE TK_CYCLIST_SEP TK_ENDLINE section2 | TK_ENDLINE;

section2: sec2_stmt sec2_continue;

sec3_continue: TK_ENDLINE section3 | %empty;

section3: sec3_stmt sec3_continue;

endline_list_helper: TK_ENDLINE endline_list;

endline_list: %empty | endline_list_helper;

section1_opt: section1 | endline_list;
section2_opt: section2 | endline_list;
section3_opt: TK_ENDLINE section3 | endline_list;

axiom: section1_opt TK_SECTION_SEP TK_ENDLINE section2_opt TK_SECTION_SEP section3_opt;
%%



const char* getSymbolName(int symbol) {
  return yysymbol_name(symbol);
}

int getCurrentToken(const void* ctx) {
  return yypcontext_token((const yypcontext_t*)ctx);
}

YYLTYPE* getCurrentTokenLocation(const void* ctx) {
  return yypcontext_location((const yypcontext_t*)ctx);
}

int getExpectedTokens(const void* yyctx, int yyarg[], int yyargn) {
  return yypcontext_expected_tokens((const yypcontext_t*)yyctx, (yysymbol_kind_t*)yyarg, yyargn);
}

int yyreport_syntax_error(const yypcontext_t* yyctx) {
  return reportSyntaxError((void*)yyctx);
}
