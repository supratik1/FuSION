
%{
#include "rules_parser.tab.h"
#include <stdlib.h>
#include<string>
using namespace std;

%}

%%


"("               { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';  return t_OBrack;}
")"               { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';  return t_CBrack;}
","               { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';  return t_Comma;}


"="               { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0'; return t_String;}
"-->"             { /*printf("%s\n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0'; return t_Arrow;} 
"true"            { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0'; return t_True;} 
"false"            { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0'; return t_False;} 
[0-9][0-9]*        { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0'; return t_Integer;} 
(\$c_)[0-9][0-9]*   { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';return t_Constant;} 
(\$e_)[0-9][0-9]*   { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';return t_Variable;} 
(\$b_)[0-9][0-9]*   { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';return t_BoolVariable;} 


 [a-zA-Z0-9][a-zA-Z0-9]* { /*printf("%s \n",yytext);*/rules_parserlval.string_value=strdup(yytext); rules_parserlval.string_value[yyleng]='\0';return t_String;} 


\n  {}

%%
int yywrap(void) {
return 1;
}

