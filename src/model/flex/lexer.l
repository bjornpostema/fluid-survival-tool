%{
/* #include "lexglobal.h" */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser_bison_class.tab.h"
#include <math.h>
#include "Formula.h"
#include "IntervalSet.h"

using namespace model;
void yyerror(char*);
//void yyterminate();
int varindex(char *var);

int COL_COUNTER = 1;
int COL_ERROR = -1;

%}

%%

[ \t]		{ COL_COUNTER += (int) strlen(yytext); } /* ignore but count white space */

(([0-9]+(\.[0-9]*)?)|([0-9]*\.[0-9]+)) { COL_COUNTER += (int) strlen(yytext); yylval.dval=atof(yytext);
		    return DOUBLE;}

[a-z][a-z0-9]*	{ COL_COUNTER += (int) strlen(yytext); yylval.ivar = varindex(yytext);
		    return VARIABLE; }

"PR"		{ COL_COUNTER += 2; return S_PR; }
"AND"           { COL_COUNTER += 3; return S_AND; }
"U"           { COL_COUNTER += 1; return S_UNTIL; }
"NEG"           { COL_COUNTER += 3; return S_NEG; }
"TT"        { COL_COUNTER += 1; return S_TRUE; }
"<="		{ COL_COUNTER += 2; return S_LEQ; }
"<"		{ COL_COUNTER += 1; return S_LESSER; }
">"		{ COL_COUNTER += 1; return S_GREATER; }
">="		{ COL_COUNTER += 2; return S_GEQ; }
"["		{ COL_COUNTER += 1; return LCOLON; }
"]"		{ COL_COUNTER += 1; return RCOLON; }
","         { COL_COUNTER += 1; return COMMA; }
"("		{ COL_COUNTER += 1; return LPAREN; }
")"		{ COL_COUNTER += 1; return RPAREN; }
"="		{ COL_COUNTER += 1; return S_EQUALS; }
"X."		{ COL_COUNTER += 2; return S_FLUID; }
"M."		{ COL_COUNTER += 2; return S_DISCRETE; }

.       { COL_COUNTER += (int) strlen(yytext); return 0; }
%%
/**
 * reset the line and column count
 *
 *
 */
void reset_lexer(void)
{
  COL_COUNTER  = 1;
}

int getColError() {
    return COL_ERROR;
}

/**
 * yyerror() is invoked when the lexer or the parser encounter
 * an error. The error message is passed via *s
 *
 *
 */
void yyerror(char *s)
{
    COL_ERROR = COL_COUNTER;
//  fprintf(stderr,"Lexer error: %s\n",s);
    fprintf(stderr,"Lexer error: %s at col: %d\n",s,COL_COUNTER);
}

int yywrap(void)
{
  return 1;
}
