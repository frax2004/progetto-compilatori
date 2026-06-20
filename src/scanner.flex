%option noyywrap

//Riconoscere nella prima sezione: sigle (del tipo XXX),
//nome città circondato da "" (del tipo "Xxxxxxx"),
//coordinate float (del tipo (xxx.xx,xxx.xx))

//ogni sezione è separata da %%%
//ogni ciclista è separato da &&&

//utilities
UPPERCASE_LETTER [A-ZÀÁÈÉÌÍÒÓÙÚ]
LOWERCASE_LETTER [a-zúùàáèéóòíì]
WORD [{UPPERCASE_LETTER}{LOWERCASE_LETTER}]+
WHITE_SPACE [ \t]
DIGIT [0-9]
NON_ZERO_DIGIT [1-9]
OPEN_PAR "("
CLOSE_PAR ")"

//prima sezione
CITY_CODE {UPPERCASE_LETTER}{3}
CITY_NAME \"({WORD}({WHITE_SPACE}+{WORD})*)\"
COORDINATES ({NON_ZERO_DIGIT}{DIGIT}*|"0")\.{DIGIT}+

//seconda sezione
CYCLIST_CODE {NON_ZERO_DIGIT}{DIGIT}{0,2}|"1000"
CYCLIST_NAME {WORD}({WHITE_SPACE}+{WORD})+

//terza sezione
SECONDS "0"|{NON_ZERO_DIGIT}{DIGIT}*

%%

{CITY_CODE} |
{CITY_NAME} |
{COORDINATES} |
{CYCLIST_CODE} |
{CYCLIST_NAME} |
{SECONDS} {ECHO;}

%%
