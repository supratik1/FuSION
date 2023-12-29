%{
#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <vector>
#include "../include/rules_parser_datatypes.h"

using namespace std;


//comment this out in the integrated version
int yylex(void);
void yyerror(const char *);
//to be commented ends

list<rule_t*> list_of_rules;
%}

//add code requires code here when when integration has to be done

%error-verbose
%start list_rules

%union{
  char* str;
  opr_t opr;
  fun_name_t fun_name;
  fun_name2_t fun_name2;
  quantifier_t quantifier;
  node_t node_type;
  edge_t edge_type;
  edge_subt edge_subtype;
  node_state_t node_state;
  node_state2_t node_state2;
  edge_state_t edge_state;
  set_nodes_t* set_nodes;
  list_edge_subtypes_t* list_edge_subtypes;
  //logical_edge_subt_t* logical_edge_subtypes;
  edge_subt_literal_t* edge_subtype_literal;
  ANDed_edgesub_literals_t* ANDed_edge_subtype_clause;
  DNF_edgesub_t* DNF_edge_subtype;
  list_antecedent_t* list_antecedent;
  logical_stmt_t* logical_stmt;
  consequent_t* consequent;
  rule_t* rule;
  list_rules_t* list_rules;
}

%token BEGIN_RULE
%token END_RULE
%token COLON

%token FOR_ALL
%token THERE_EXISTS
%token IN
%token IS

%token AND
%token OR
%token NOT
%token IMPLIES

%token ALL_NODES	
%token LEFT_PAREN
%token RIGHT_PAREN
%token PRED
%token SUCC
%token NODE_TYPE
%token EDGE_TYPE
%token EDGE_SUBTYPE
%token EDGE_SUBTYPE_DNF
%token NODE_INPUT_EDGE_TYPE
%token NODE_OUTPUT_EDGE_TYPE
%token NODE_INPUT_EDGE_SUBTYPE
%token NODE_OUTPUT_EDGE_SUBTYPE
%token GENE
%token GROUP
%token COMPOUND
%token PPREL
%token ECREL
%token GEREL
%token PCREL
%token ACTIVE
%token INHIBIT
%token EXPRESS
%token REPRESS
%token INDIRECT
%token STATE_CHANGE
%token ASSOC
%token DISSOC
%token MISSING_INTERACTION
%token COMPONENT
%token COMMA
%token SEMICOLON

%token EDGE
%token NODE
%token ACTIVATED
%token INHIBITED
%token OVEREXPRESSED
%token REPRESSED
%token DIFFER_EXPRESSED
%token NOT_DIFFER_EXPRESSED
%token PRESENT
%token ABSENT

%token<str> VARIABLE

%left OR
%left AND
%right NOT 

%type <list_rules> list_rules 
%type <rule> rule 
%type <list_antecedent> antecedent 
%type <consequent> consequent 
%type <set_nodes> set_nodes
%type <node_type> node_t
%type <edge_type> edge_t 
%type <edge_subtype> edge_subt 
%type <list_edge_subtypes> list_edge_subtypes
%type <logical_stmt> logical_statement
//%type <logical_edge_subtypes> logical_edge_subtypes
%type <edge_subtype_literal> edge_subtype_literal
%type <ANDed_edge_subtype_clause> ANDed_edge_subtype_clause
%type <DNF_edge_subtype> DNF_edge_subtype
%type <node_state> node_state 
%type <node_state2> node_state2 
%type <edge_state> edge_state
%type <str> var 
%%

list_rules	:	rule
				{
					list_rules_t* temp = new list_rules_t;
					assert(temp != NULL);
					temp->rule = $1;
					temp->next = NULL;
					if(list_of_rules.empty())
						list_of_rules.push_back(temp->rule);
					$$ = temp;
				}
				|
				list_rules rule
				{
					list_rules_t* temp = new list_rules_t;
					assert(temp != NULL);
					temp->rule = $2;
					temp->next = $1;
					if(!list_of_rules.empty())
						list_of_rules.push_back(temp->rule);
					$$ = temp;
				}
				;

rule	:	BEGIN_RULE antecedent COLON consequent END_RULE
			{
				rule_t* temp = new rule_t;
				assert(temp != NULL);
				temp->antecedent = $2; 
				temp->consequent = $4;
				$$ = temp;
			} 
			;
			
antecedent	:	FOR_ALL var IN set_nodes
				{
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = FOR_ALL_QUANT;
					temp->var = new char[strlen($2)+1];
					assert(temp->var != NULL);
					strcpy(temp->var, $2);
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = $4; 
					temp->next = NULL;
					$$ = temp;
				}
				|
				THERE_EXISTS var IN set_nodes
				{
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = THERE_EXISTS_QUANT;
					temp->var = new char[strlen($2)+1];
					assert(temp->var != NULL);
					strcpy(temp->var, $2);
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = $4; 
					temp->next = NULL;
					$$ = temp;
				}
				|
				antecedent FOR_ALL var IN set_nodes
				{
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = FOR_ALL_QUANT;
					temp->var = new char[strlen($3)+1];
					assert(temp->var != NULL);
					strcpy(temp->var, $3);
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = $5; 
					temp->next = $1;
					$$ = temp;
				}
				|
				antecedent THERE_EXISTS var IN set_nodes
				{
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = THERE_EXISTS_QUANT;
					temp->var = new char[strlen($3)+1];
					assert(temp->var != NULL);
					strcpy(temp->var, $3);
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = $5; 
					temp->next = $1;
					$$ = temp;
				}
				;
				
set_nodes	:	NOT set_nodes
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NOT_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = $2;
					temp->right = NULL;
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				set_nodes AND set_nodes
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = AND_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = $1;
					temp->right = new set_nodes_t;
					assert(temp->right != NULL);
					temp->right = $3;
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				set_nodes OR set_nodes
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = OR_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = $1;
					temp->right = new set_nodes_t;
					assert(temp->right != NULL);
					temp->right = $3;
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				PRED LEFT_PAREN var RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = PRED_FUN;
					temp->arg1 = new char[strlen($3)+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, $3);
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				SUCC LEFT_PAREN var RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = SUCC_FUN;
					temp->arg1 = new char[strlen($3)+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, $3);
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				NODE_TYPE LEFT_PAREN node_t RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = $3;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				NODE_INPUT_EDGE_TYPE LEFT_PAREN edge_t RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_INPUT_EDGE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = $3;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				NODE_OUTPUT_EDGE_TYPE LEFT_PAREN edge_t RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_OUTPUT_EDGE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = $3;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				NODE_INPUT_EDGE_SUBTYPE LEFT_PAREN edge_subt RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_INPUT_EDGE_SUBTYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = $3;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				NODE_OUTPUT_EDGE_SUBTYPE LEFT_PAREN edge_subt RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_OUTPUT_EDGE_SUBTYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = $3;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				EDGE_TYPE LEFT_PAREN var COMMA var COMMA edge_t RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_TYPE_FUN;
					temp->arg1 = new char[strlen($3)+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, $3);
					temp->arg2 = new char[strlen($5)+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, $5);
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = $7;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				EDGE_SUBTYPE LEFT_PAREN var COMMA var COMMA edge_subt RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_SUBTYPE_FUN;
					temp->arg1 = new char[strlen($3)+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, $3);
					temp->arg2 = new char[strlen($5)+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, $5);
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = $7;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				|
				EDGE LEFT_PAREN var COMMA var COMMA edge_t COMMA list_edge_subtypes RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_FUN;
					temp->arg1 = new char[strlen($3)+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, $3);
					temp->arg2 = new char[strlen($5)+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, $5);
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = $7;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = $9;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}
				/*
				|
				EDGE LEFT_PAREN var COMMA var COMMA edge_t COMMA logical_edge_subtypes RIGHT_PAREN
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_FUN;
					temp->arg1 = new char[strlen($3)+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, $3);
					temp->arg2 = new char[strlen($5)+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, $5);
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = $7;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = $9;
					$$ = temp;
				}
				*/
				|
				ALL_NODES
				{
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = ALL_NODES_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					$$ = temp;
				}	
				;

var 	: 	VARIABLE
			{
				char* temp = new char[strlen($1)+1];
				assert(temp != NULL);
				strcpy(temp, $1);
				$$ = temp;
			}
			;
			
node_t	:	GENE {$$ = GENE_NODE;}
			|
			GROUP {$$ = GROUP_NODE;}
			|
			COMPOUND {$$ = COMPOUND_NODE;}
			;

edge_t	:	PPREL {$$ = PPREL_EDGE;}
			|
			ECREL {$$ = ECREL_EDGE;}
			|
			GEREL {$$ = GEREL_EDGE;}
			|
			PCREL {$$ = PCREL_EDGE;}
			;

edge_subt	:	ACTIVE {$$ = ACTIVE_EDGE;}
				|
				INHIBIT {$$ = INHIBIT_EDGE;}
				|
				EXPRESS {$$ = EXPRESS_EDGE;}
				|
				REPRESS {$$ = REPRESS_EDGE;}
				|
				INDIRECT {$$ = INDIRECT_EDGE;}
				|
				STATE_CHANGE {$$ = STATE_CHANGE_EDGE;}
				|
				ASSOC {$$ = ASSOC_EDGE;}
				|
				DISSOC {$$ = DISSOC_EDGE;}
				|
				MISSING_INTERACTION {$$ = MISSING_INTERACTION_EDGE;}
				|
				COMPONENT {$$ = COMPONENT_EDGE;}
				;

list_edge_subtypes	:	edge_subt
				{
					list_edge_subtypes_t* temp = new list_edge_subtypes_t;
					assert(temp != NULL);
					temp->edge_subtype_data = $1;
					temp->next = NULL;
					$$ = temp;
				}
				|
				list_edge_subtypes SEMICOLON edge_subt
				{
					list_edge_subtypes_t* temp = new list_edge_subtypes_t;
					assert(temp != NULL);
					temp->edge_subtype_data = $3;
					temp->next = $1;
					$$ = temp;
				}
				;
				
edge_subtype_literal	:	edge_subt
							{
								edge_subt_literal_t* temp = new edge_subt_literal_t;
								assert(temp != NULL);
								temp->opr = NONE_OPR;
								temp->edge_subtype_data = $1;
								$$ = temp;
							}
							|
							NOT edge_subt
							{
								edge_subt_literal_t* temp = new edge_subt_literal_t;
								assert(temp != NULL);
								temp->opr = NOT_OPR;
								temp->edge_subtype_data = $2;
								$$ = temp;
							}
							;

ANDed_edge_subtype_clause	:	edge_subtype_literal
								{
									ANDed_edgesub_literals_t* temp = new ANDed_edgesub_literals_t;
									assert(temp != NULL);
									temp->curr_literal= $1;
									temp->next = NULL;									
									$$ = temp;
								}
								|
								ANDed_edge_subtype_clause AND edge_subtype_literal
								{
									ANDed_edgesub_literals_t* temp = new ANDed_edgesub_literals_t;
									assert(temp != NULL);
									temp->curr_literal= $3;
									temp->next = $1;									
									$$ = temp;
								}
								;

DNF_edge_subtype	:	ANDed_edge_subtype_clause
						{
							DNF_edgesub_t* temp = new DNF_edgesub_t;
							assert(temp != NULL);
							temp->current_ANDed_clause = $1;
							temp->next = NULL;
							$$= temp;
						}
						|
						DNF_edge_subtype OR ANDed_edge_subtype_clause
						{
							DNF_edgesub_t* temp = new DNF_edgesub_t;
							assert(temp != NULL);
							temp->current_ANDed_clause = $3;
							temp->next = $1;
							$$= temp;
						}
						;

/*
logical_edge_subtypes	: edge_subt
							{
								logical_edge_subt_t* temp = new logical_edge_subt_t;
								assert(temp != NULL);
								temp->opr = NONE_OPR;
								temp->left = NULL;
								temp->right = NULL;
								temp->arg_edgesub = $1;
								$$ = temp;
							}
							|
							NOT logical_edge_subtypes
							{
								logical_edge_subt_t* temp = new logical_edge_subt_t;
								assert(temp != NULL);
								temp->opr = NOT_OPR;
								temp->left = $2;
								temp->right = NULL;
								temp->arg_edgesub = NONE_EDGE_SUB;
								$$ = temp;
							}
							|
							logical_edge_subtypes AND logical_edge_subtypes
							{
								logical_edge_subt_t* temp = new logical_edge_subt_t;
								assert(temp != NULL);
								temp->opr = AND_OPR;
								temp->left = $1;
								temp->right = $3;
								temp->arg_edgesub = NONE_EDGE_SUB;
								$$ = temp;
							}
							|
							logical_edge_subtypes OR logical_edge_subtypes
							{
								logical_edge_subt_t* temp = new logical_edge_subt_t;
								assert(temp != NULL);
								temp->opr = OR_OPR;
								temp->left = $1;
								temp->right = $3;
								temp->arg_edgesub = NONE_EDGE_SUB;
								$$ = temp;
							}
							;
*/
consequent	:	logical_statement IMPLIES logical_statement
				{
					consequent_t* temp = new consequent_t;
					temp->left_of_implies = $1;
					temp->right_of_implies = $3;
					$$ = temp;
				}
				;
				
logical_statement	:	NODE LEFT_PAREN var RIGHT_PAREN IS node_state
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = NODE_FUN2;
							temp->arg1 = new char[strlen($3)+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, $3);
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = $6;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						|
						NODE LEFT_PAREN var RIGHT_PAREN IS node_state2
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = NODE_FUN2;
							temp->arg1 = new char[strlen($3)+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, $3);
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = $6;
							temp->state_edge = EDGE_NONE_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						|
						EDGE LEFT_PAREN var COMMA var COMMA edge_t COMMA list_edge_subtypes RIGHT_PAREN IS edge_state
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_FUN2;
							temp->arg1 = new char[strlen($3)+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, $3);
							temp->arg2 = new char[strlen($5)+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, $5);
							temp->arg3 = $7;
							temp->arg4_list = $9;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = $12;
							$$ = temp;
						}
						|
						EDGE_SUBTYPE_DNF LEFT_PAREN var COMMA var COMMA DNF_edge_subtype RIGHT_PAREN IS edge_state
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_SUBTYPE_DNF_FUN2;
							temp->arg1 = new char[strlen($3)+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, $3);
							temp->arg2 = new char[strlen($5)+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, $5);
							temp->arg3 = NONE_EDGE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = $7;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = $10;
							$$ = temp;
						}
						/*
						|
						EDGE LEFT_PAREN var COMMA var COMMA edge_t COMMA logical_edge_subtypes RIGHT_PAREN IS edge_state
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_FUN2;
							temp->arg1 = new char[strlen($3)+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, $3);
							temp->arg2 = new char[strlen($5)+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, $5);
							temp->arg3 = $7;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = $9;
							temp->arg_DNF_edgesub = NULL;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = $12;
							$$ = temp;
						}
						*/
						|
						NOT logical_statement
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NOT_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = $2;
							temp->right = NULL;
							temp->fun_name2 = NONE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						|
						logical_statement OR logical_statement
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = OR_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = $1;
							temp->right = new logical_stmt_t;
							assert(temp->right != NULL);
							temp->right = $3;
							temp->fun_name2 = NONE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						|
						logical_statement AND logical_statement
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = AND_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = $1;
							temp->right = new logical_stmt_t;
							assert(temp->right != NULL);
							temp->right = $3;
							temp->fun_name2 = NONE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						/*
						|
						TRUE
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = TRUE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						|
						FALSE
						{
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = FALSE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							$$ = temp;
						}
						*/
						;

node_state	:	ACTIVATED {$$ = ACTIVATED_STATE;}	
				|	
				INHIBITED {$$ = INHIBITED_STATE;}
				|
				REPRESSED {$$ = REPRESSED_STATE;}
				|
				OVEREXPRESSED {$$ = OVEREXPRESSED_STATE;}
				;

node_state2 :	DIFFER_EXPRESSED {$$ = DIFFER_EXPRESSED_STATE;}	
				|	
				NOT_DIFFER_EXPRESSED {$$ = NOT_DIFFER_EXPRESSED_STATE;}
				;

edge_state	:	PRESENT {$$ = PRESENT_STATE;}
				|
				ABSENT {$$ = ABSENT_STATE;}
				;	
			
%%

void yyerror(char const* message)
{
    cerr << message;
}

int init (void) {
    //yyparse();
    return (1);
}