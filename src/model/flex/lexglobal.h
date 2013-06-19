#ifndef YYSTYPE
typedef union {
  double    dval;
  char*	    id;
  struct symtab *symp;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif


/* extern YYSTYPE yylval; */
YYSTYPE yylval; 
