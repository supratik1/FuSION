%{
#include "dag_parser.tab.h"
#include <stdlib.h>
#include<string>
using namespace std;

%}

%%


"("               { return t_OBrack;}
")"               { return t_CBrack;}
","               { return t_Comma;}


"="               {return t_Eq;}




[-\.a-zA-Z_0-9/\[\]\$]+     { dag_parserlval.string_value=strdup(yytext); dag_parserlval.string_value[yyleng]='\0';  return t_String;}


%%
int yywrap(void) {
return 1;
}

