%{
#include "inferencerule_parser.tab.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>
using namespace std;
%}

A [/]
B [*]
C [^/*]
L [a-zA-Z_]

Operators add|sub|multiply|divide|mod|gt|ge|lt|le|lognot|logand|logor|logeq|diseq|wildeq|zeq|case|bitand|bitor|bitxor|bitxnor|bitneg|select|concat|leftshift|rightshift|redand|redor|rednor|redxor|redxnor|ite|replic|posedge|negedge|anyedge|orevent|edgecond|default|typeref|assignpattern|defaultvalue|labelvalue|exp|cast|tri|zeroextn|signed|unsigned|oneextn|signextn|countones
Type    "SIGNED_INTEGER"|"UNSIGNED_INTEGER"|"SIGNED_BITVECTOR"|"UNSIGNED_BITVECTOR"|"REAL"|"BOOL"

%%
{A}({A}.*|{B}({A}|{C}|{B}+{C})*{B}+{A})        {/*cout<<"Comments in Lex\n";*/
                                                return t_Comments;}
"$operator:"      {/*cout<<"Operator string\n"; */
                   return t_OperatorString; }

{Operators}         {/* cout<<"Operator:"<<yytext<<"\n";*/
                               inferencerule_parserlval.string_value=strdup(yytext);
                               inferencerule_parserlval.string_value[yyleng]='\0'; 
                               /*cout<<"Operator:"<<yytext;*/
                               return t_String;}

"$minargs:"      {/*cout<<"Minargs\n"; */
                  return t_MinArgs;}
"$maxargs:"      {/*cout<<"Maxargs\n"; */
                  return t_MaxArgs;}

\[               { return t_OSquareBrack;}
\]               { return t_CSquareBrack;}

[0-9]+           { /*cout<<yytext<<endl;*/
                    inferencerule_parserlval.int_value=atoi(yytext); return INT_NUMBER; }
[ \t\n]+         {/* Spaces nothing to do */}

"$operand1:"      { /*cout<<"operand1"<<endl;*/
                    return t_Operand1; }
"$operand2:"      { /*cout<<"operand2"<<endl;*/
                    return t_Operand2; }
"$operand3:"      { /*cout<<"operand3"<<endl;*/
                    return t_Operand3; }
"$other:"         { /*cout<<"OtherOperands"<<endl;*/
                    return t_OtherOperandsString;}
"$infinity"       { /*cout<<"Infinity"<<endl;*/
                    return t_InfinityString;}
"$infer:"         { /*cout<<"InferringType"<<endl; */
                    return t_InferString;}

"$cast:"         { return t_CastString; }

{Type}            { /* cout<<"Type recieved\n"<<yytext<<endl; */
                  inferencerule_parserlval.string_value=strdup(yytext);
                   inferencerule_parserlval.string_value[yyleng]='\0'; 
                     /*                   cout<<"Type:"<<yytext;*/
                   return t_TypeValue; }

"%max"         { /*cout<<"MaxComp"<<endl;*/
                 return t_Max;}
"%min"         { /*cout<<"MinComp"<<endl;*/
                 return t_Min;}
"%add"         { /*cout<<"AddComp"<<endl;*/
                 return t_Add;}
"%addall"      { /*cout<<"Addall Comp"<<endl;*/
                 return t_Addall;}

\#             { return t_Hash; }
\,             { return t_Comma;}
\(             { return t_OBrack; }
\)             { return t_CBrack; }
\;             { return t_SemiColon; }



"$print:"      { return t_print; }
"$printall"    { return t_printAll; }
"$once"        { return t_Once; }
"$every"       { return t_Every; }
\{             { return t_FOBrack; }
\}             { return t_FCBrack; }

\"(\\.|[^\\"])*\" {inferencerule_parserlval.string_value=strdup(yytext);
                   inferencerule_parserlval.string_value[yyleng]='\0'; 
                   /*cout<<"\nString:"<<yytext;*/
                   return t_StringLiteral; }



\$(([a-zA-Z]+)([0-9]*)) {inferencerule_parserlval.string_value=strdup(yytext);
                   inferencerule_parserlval.string_value[yyleng]='\0'; 
                  /* cout<<"\nKeyword:"<<yytext;*/
                   return t_KeyWordsOfPrint;}



%%

int yywrap(void) {
 return 1;
}
/*
int main()
{
yylex();
return 0;
}
*/
