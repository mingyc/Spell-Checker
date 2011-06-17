%{
    /* definitions of manifest constants */
	#include<stdio.h>
	#include<ctype.h>
  #include <string.h>
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

  if (wordPtr != NULL)
    wordPtr = (char *)realloc(wordPtr, (yyleng+1) * sizeof(char));
  else
    wordPtr = (char *)malloc((yyleng+1) * sizeof(char));

  strncpy(wordPtr, yytext, yyleng);
	for(i = 0; i < yyleng; i++){
    if (!isalpha(yytext[i])) break;
		*(wordPtr + i) = tolower(*(yytext + i));
	}
	*(wordPtr + i) = '\0';
  *(yytext + i) = '\0';
}

int yywrap(){
	return 1;
}
