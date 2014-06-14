%{
#include <stdlib.h>
#include <string.h>


#include "def.tab.hh"

extern int yylineno;
int yyerror(const char *msg, ...);

%}
%%
-?[0-9]+\.[0-9]*	{yylval.fval = atof(yytext); return LR;}
-?[0-9]+			{yylval.ival = atoi(yytext); return LC;}
\=\=			    {return OP_COMPARE;}
\<\=			    {return OP_LE;}
\>\=			    {return OP_GE;}
\+			        {return '+';} 
\=			        {return '=';} 
\&\&			    {return OP_AND;}
\|\|			    {return OP_OR;}
\!			        {return '!';}
\<			        {return '<';}
\>			        {return '>';}
\,			        {return ',';}
\*			        {return '*';}
\(			        {return '(';}
\)			        {return ')';}
\[			        {return '[';}
\]			        {return ']';}
\{			        {return '{';}
\}			        {return '}';}
\/			        {return '/';}
\%			        {return '%';}
\-			        {return '-';}
\^			        {return '^';}
\|			        {return '|';}
\&			        {return '&';}
\?			        {return '?';}
\:			        {return ':';}

if					{ return IF; }
else				{ return ELSE; }
while				{ return WHILE;}
for					{ return FOR;}
do					{ return DO;}
int					{ return TYPE_INT;}
float				{ return TYPE_FLOAT;}
bool				{ return TYPE_BOOL;}
true				{ return BOOL_TRUE;}
false				{ return BOOL_FALSE;}
	
[A-Za-z_][A-Za-z0-9_]*	{yylval.text = strdup(yytext); return ID;}

\ |\t				{;}
\;					{return ';';}
\n					{yylineno++;}
.					{printf("%s", yytext); }
%%

int yyerror(const char *msg, ...)
{
	printf("%d: %s\n", yylineno, msg);
	exit(1);
}

