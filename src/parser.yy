%{
int yylex();
void yyerror(const char *s);
%}

%start program;

%%
program: %empty;

%%

void yyerror(const char*) {}