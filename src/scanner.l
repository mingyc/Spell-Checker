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
	int i, j;

  if (wordPtr != NULL)
    wordPtr = (char *)realloc(wordPtr, (yyleng+1) * sizeof(char));
  else
    wordPtr = (char *)malloc((yyleng+1) * sizeof(char));

  strncpy(wordPtr, yytext, yyleng);
	for(i = j = 0; i < yyleng; i++){
    if (yytext[i] == '\'') break;
    if (yytext[i] == '-')  
      memcpy(yytext+i, yytext+i+1, strlen(yytext+i+1)+1);
		wordPtr[j++] = tolower(yytext[i]);
	}
	*(wordPtr + j) = '\0';
  *(yytext + i) = '\0';
}

int yywrap(){
	return 1;
}
