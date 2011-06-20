%{
    /* definitions of manifest constants */
	#include<stdio.h>
	#include<ctype.h>
	#include<string.h>
	#define STR 1
	void installSTR();
	char *dicttext = NULL;
	char *spelltext = NULL;
%}

delim   [ \t\n\r]
ws      {delim}+
letter  [A-Za-z]
punct   [:punct:]

str     [A-Za-z']+(?:[^A-Za-z'])
/* str 	{letter}[[:alnum:][:punct:]]* */
/*str     (({letter}+([-'_]|{letter}*){letter}+)|{letter})*/

%%

{ws} {/* no action and no return */}
{str} {installSTR(); return(STR);}
. {}

%%

void installSTR(){
	int i, j, k;
  /* TODO:
     This might cause huge memory leakage. 
     Don't know why realloc cannot work properly  */
    dicttext  = (char *)realloc(dicttext, (yyleng+1) * sizeof(char));
    spelltext = (char *)realloc(spelltext, (yyleng+1) * sizeof(char));


	for(i = j = k = 0; i < yyleng; i++){
		if (yytext[i] == '\'') break;
		if (isalpha(yytext[i])){
			dicttext[j++] = tolower(yytext[i]);
			spelltext[k++] = yytext[i];
		}
	}
	
	*(dicttext + j) = '\0';
	*(spelltext + k) = '\0';
}

//
// int yywrap()
//   This function will be call at the end of input file stream.
//   Return 1 then flex will stop parsing.
//
int yywrap(){
  if (dicttext)  free(dicttext);
  if (spelltext) free(spelltext);
	return 1;
}
