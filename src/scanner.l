%{
#include <string.h>

%}

digit                         [0-9]
letter                        [a-zA-Z_]


%%



[ \t\r\n]                     {}
.                             { return (int)yytext[0]; } /* Can't we ignore unmatched characters ? */

%%

int yywrap() 
{
  return 1;
}

