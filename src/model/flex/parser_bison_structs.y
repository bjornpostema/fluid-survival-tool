%{   
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>  
#include "Formula_struct.h"
#include "Interval_struct.h"

int yylex(void);

void yyerror(char *s);
#define NVARS 100
char *vars[NVARS]; double vals[NVARS]; int nvars=0;

struct Formula *resFML;
%}

%union { double dval; int ivar; struct Formula *fml; }

%token <dval> DOUBLE
%token <ivar> VARIABLE
%type <fml> expr
%type <fml> atomdisexpr
%type <fml> atomcontexpr

%token	PR
%token 	S_LEQ
%token	LPAREN RPAREN
%token	S_NEG S_AND S_UNTIL
%token	LCOLON RCOLON
%token 	COMMA
%token	S_FLUID S_DISCRETE
%token	S_EQUALS

%left S_NEG
%left S_AND
%left S_OR
%left S_UNTIL
%left S_FLUID
%left S_DISCRETE
%left PR
%nonassoc LPAREN RPAREN LCOLON RCOLON
%nonassoc DOUBLE COMMA
%nonassoc S_LEQ S_EQUALS
%nonassoc VARIABLE

%%
%start main;

main : PR S_LEQ DOUBLE LPAREN expr RPAREN {
	/*resFML = new ProbFormula($5,0,$3,LEQ);*/
	
	/*ProbFormula *resFML;*/
	resFML->leftChild = $5;
	resFML->rightChild = 0;
	resFML->op = LEQ;
	resFML->prob = $3;

	std::cout << "Probability Formula detected." << std::endl;
};

expr : expr S_AND expr {
	/*$$ = new AndFormula($1,$3);*/
	
	/*AndFormula $$;*/
	$$->leftChild = $1;
	$$->rightChild = $3;

	std::cout << "And Formula detected." << std::endl;
}
     | S_NEG expr {
	/*$$ = new NegFormula($2,0);*/
	
	/*NegFormula $$;*/
	$$->leftChild = $2;
	$$->rightChild = 0;

	std::cout << "Negation Formula detected." << std::endl;
}
     | expr S_UNTIL LCOLON DOUBLE COMMA DOUBLE RCOLON expr  {
	/*$$ = new UntilFormula($1,$8,Interval($4,$6));*/
	
	/*UntilFormula $$;*/
	$$->leftChild = $1;
	$$->rightChild = $8;
	Interval ival;
	ival.start = $4;
	ival.end = $6;
	$$->bound = ival;

	std::cout << "Until Formula detected." << std::endl;
}

     | atomdisexpr { $$=$1; }
     | atomcontexpr { $$=$1; };

atomdisexpr : S_DISCRETE LPAREN VARIABLE S_EQUALS DOUBLE RPAREN { 
	/*$$ = new AtomDisFormula(0,0,vals[$3],$5);*/
	
	/*AtomDisFormula $$;*/
	$$->leftChild = 0;
	$$->rightChild = 0;
	$$->placeIndex = vars[$3];
	$$->constant = $5;
	
};
atomcontexpr : S_FLUID LPAREN VARIABLE S_LEQ DOUBLE RPAREN { 
	/*$$ = new AtomContFormula(0,0,vals[$3],$5);*/
	
	/*AtomContFormula $$;*/
	$$->leftChild = 0;
	$$->rightChild = 0;
	$$->placeIndex = vars[$3];
	$$->constant = $5;
	
};

%%

int varindex(char *var)
{
int i;
for (i=0; i<nvars; i++)
if (strcmp(var,vars[i])==0) return i;
vars[nvars] = strdup(var);
return nvars++;
}

/* Called by yyparse on error. */
/*void
yyerror (char const *s)
{
fprintf (stderr, "%s\n", s);
}*/

int main(void)
{
/*yydebug=1;*/
yyparse();
return 0;
}

