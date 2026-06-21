%option noyywrap yylineno

%{
  #include <string.h>
  #include "parser.h"
  #include <compiler.h>
  int yycolumn = 1;

  #define YY_USER_ACTION \
  yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; \
  yylloc.last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng;
%}

/*
Riconoscere nella prima sezione: sigle (del tipo XXX),
nome città circondato da "" (del tipo "Xxxxxxx"),
coordinate float (del tipo (xxx.xx,xxx.xx))

ogni sezione è separata da %%%
ogni ciclista è separato da &&&
*/

/*utilities*/
UPPERCASE_LETTER [A-ZÀÁÈÉÌÍÒÓÙÚ]
LOWERCASE_LETTER [a-zúùàáèéóòíì]
WORD ({UPPERCASE_LETTER}|{LOWERCASE_LETTER})+
WHITE_SPACE [ \t]
DIGIT [0-9]
NON_ZERO_DIGIT [1-9]
OPEN_PAR "("
CLOSE_PAR ")"
COMMA ","
CYCLIST_SEP "&&&"
SECTION_SEP "%%%"
ENDLINE \n

/*prima sezione*/
CITY_CODE {UPPERCASE_LETTER}{3}
CITY_NAME \"({WORD}({WHITE_SPACE}+{WORD})*)\"
COORDINATES ({NON_ZERO_DIGIT}{DIGIT}*|"0")\.{DIGIT}+

/*seconda sezione*/
CYCLIST_CODE {NON_ZERO_DIGIT}{DIGIT}{0,2}|"1000"
CYCLIST_NAME {WORD}({WHITE_SPACE}+{WORD})+

/*terza sezione*/
SECONDS "0"|{NON_ZERO_DIGIT}{DIGIT}*


%%
{CITY_CODE} { yylval.as_string = strdup(yytext); return TK_CITY_CODE; }
{CITY_NAME} { yylval.as_string = strndup(yytext+1, yyleng-2); return TK_CITY_NAME; }
{COORDINATES} { yylval.as_double = atof(yytext); return TK_COORDINATES; }
{CYCLIST_CODE} { yylval.as_int = atoi(yytext); return TK_CYCLIST_CODE; }
{CYCLIST_NAME} { yylval.as_string = strdup(yytext); return TK_CYCLIST_NAME; }
{OPEN_PAR} { return TK_OPEN_PAR; }
{CLOSE_PAR} { return TK_CLOSE_PAR; }
{COMMA} { return TK_COMMA; }
{CYCLIST_SEP} { return TK_CYCLIST_SEP; }
{SECTION_SEP} { return TK_SECTION_SEP; }
{SECONDS} { yylval.as_int = atoi(yytext); return TK_SECONDS; } 
{ENDLINE} { yycolumn = 1; return TK_ENDLINE; }
{WHITE_SPACE} {}

{WORD} {
  emitError(LEXICAL_ERROR, yylloc, "%s", yytext);
  emitNote(yylloc, "Unrecognized token '%s'", yytext);
}
. {
  emitError(LEXICAL_ERROR, yylloc, "%c", *yytext);
  emitNote(yylloc, "Unrecognized symbol '%c'", *yytext);
}
%%
