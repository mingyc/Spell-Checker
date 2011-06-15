%{
    /* definitions of manifest constants */
	#include<stdio.h>
	#include<ctype.h>
	#define STR 1
	void installSTR();
	char* wordPtr = NULL;
%}

delim   [ \t\n\r]
ws      {delim}+
letter  [A-Za-z]
str     (({letter}+([-'_]|{letter}*){letter}+)|{letter})

%%

{ws} {/* no action and no return */}
{str} {installSTR(); return(STR);}
. {}

%%

void installSTR(){
	int i;
	for(i = 0; i < yyleng; i++){
		*(yytext + i) = tolower(*(yytext + i));
	}
	*(yytext + yyleng) = '\0';
}

int yywrap(){
	return 1;
}
