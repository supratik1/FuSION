%{
#include <iostream>
#include <string>
#include <cstring>
#include "../include/rules_parser_datatypes.h"
#include "../include/rules_parser.tab.h"

using namespace std;

extern int init();
extern int yyparse();
extern FILE *yyin;
extern list<rule_t* > list_of_rules;
%}

%%

:			{return COLON;}
;			{return SEMICOLON;}
"BEGIN_RULE"		{return BEGIN_RULE;}
"END_RULE"		{return END_RULE;}
"FOR_ALL"		{return FOR_ALL;}
"THERE_EXISTS"		{return THERE_EXISTS;}
"IN"		{return IN;}
"IS"		{return IS;}
"AND"		{return AND;}
"OR"		{return OR;}
"NOT"		{return NOT;}
"IMPLIES"		{return IMPLIES;}
"ALL_NODES"		{return ALL_NODES;}
"("		{return LEFT_PAREN;}
")"		{return RIGHT_PAREN;}
"PRED"		{return PRED;}
"SUCC"		{return SUCC;}
"NODE_TYPE"		{return NODE_TYPE;}
"EDGE_TYPE"		{return EDGE_TYPE;}
"EDGE_SUBTYPE"		{return EDGE_SUBTYPE;}
"EDGE_SUBTYPE_DNF"	{return EDGE_SUBTYPE_DNF;}
"NODE_INPUT_EDGE_TYPE"		{return NODE_INPUT_EDGE_TYPE;}
"NODE_OUTPUT_EDGE_TYPE"		{return NODE_OUTPUT_EDGE_TYPE;}
"NODE_INPUT_EDGE_SUBTYPE"		{return NODE_INPUT_EDGE_SUBTYPE;}
"NODE_OUTPUT_EDGE_SUBTYPE"		{return NODE_OUTPUT_EDGE_SUBTYPE;}
"GENE"		{return GENE;}
"GROUP"		{return GROUP;}
"COMPOUND"		{return COMPOUND;}
"PPREL"		{return PPREL;}
"PCREL"		{return PCREL;}
"ECREL"		{return ECREL;}
"GEREL"		{return GEREL;}
"ACTIVE"		{return ACTIVE;}
"INHIBIT"		{return INHIBIT;}
"EXPRESS"		{return EXPRESS;}
"REPRESS"		{return REPRESS;}
"INDIRECT"		{return INDIRECT;}
"STATE_CHANGE"		{return STATE_CHANGE;}
"ASSOC"		{return ASSOC;}
"DISSOC"		{return DISSOC;}
"MISSING_INTERACTION"		{return MISSING_INTERACTION;}
"COMPONENT"	{return COMPONENT;}
","		{return COMMA;}
"EDGE"		{return EDGE;}
"NODE"		{return NODE;}
"ACTIVATED"		{return ACTIVATED;}
"INHIBITED"		{return INHIBITED;}
"OVEREXPRESSED"		{return OVEREXPRESSED;}
"REPRESSED"		{return REPRESSED;}
"DIFFER_EXPRESSED"		{return DIFFER_EXPRESSED;}
"NOT_DIFFER_EXPRESSED"		{return NOT_DIFFER_EXPRESSED;}
"PRESENT"		{return PRESENT;}
"ABSENT"		{return ABSENT;}

[a-zA-Z][_a-zA-Z0-9]*	{
						yylval.str = new char[yyleng+1];
						strcpy(yylval.str, yytext);
                        return VARIABLE;
						}
						
[ \t\n]		{;}
.			{cout << "unexpected character, exiting\n";exit(1);}

%%

int yywrap(void) {
    return 1;
}
