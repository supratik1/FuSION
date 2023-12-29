#ifndef RULES_PARSER_DATATYPES_H
#define RULES_PARSER_DATATYPES_H

#include <list>
#include <string>
#include <map>

using namespace std;

typedef enum opr_type{
	NOT_OPR, AND_OPR, OR_OPR, NONE_OPR
} opr_t;

typedef enum fun_name_type{
	PRED_FUN, SUCC_FUN, NODE_TYPE_FUN, EDGE_FUN, EDGE_TYPE_FUN, EDGE_SUBTYPE_FUN, ALL_NODES_FUN, NODE_INPUT_EDGE_TYPE_FUN, NODE_OUTPUT_EDGE_TYPE_FUN, NODE_INPUT_EDGE_SUBTYPE_FUN, NODE_OUTPUT_EDGE_SUBTYPE_FUN, NONE_FUN
} fun_name_t;

typedef enum fun_name2_type{
	NODE_FUN2, EDGE_FUN2, EDGE_SUBTYPE_DNF_FUN2, NONE_FUN2
} fun_name2_t;

typedef enum quantifier_type{
	FOR_ALL_QUANT, THERE_EXISTS_QUANT, NONE_QUANT
} quantifier_t;

typedef enum node_type{
	GENE_NODE, GROUP_NODE, COMPOUND_NODE, NONE_NODE
} node_t;

typedef enum edge_type{
	PPREL_EDGE, ECREL_EDGE, GEREL_EDGE, PCREL_EDGE, NONE_EDGE
} edge_t;

typedef enum edge_subtype{
	ACTIVE_EDGE, INHIBIT_EDGE, EXPRESS_EDGE, REPRESS_EDGE, INDIRECT_EDGE, STATE_CHANGE_EDGE,
	ASSOC_EDGE, DISSOC_EDGE, MISSING_INTERACTION_EDGE, ANY_EDGE_SUB, COMPONENT_EDGE ,NONE_EDGE_SUB
} edge_subt;

typedef enum node_state_type{
	ACTIVATED_STATE, INHIBITED_STATE, REPRESSED_STATE, OVEREXPRESSED_STATE, NODE_NONE_STATE
} node_state_t;

typedef enum node_state2_type{
	DIFFER_EXPRESSED_STATE, NOT_DIFFER_EXPRESSED_STATE, NODE_NONE2_STATE
} node_state2_t;

typedef enum edge_state_type{
	PRESENT_STATE, ABSENT_STATE, EDGE_NONE_STATE
} edge_state_t;

typedef struct list_edge_subtypes_type{
	edge_subt edge_subtype_data;
	struct list_edge_subtypes_type* next;
} list_edge_subtypes_t;

/*
typedef struct logical_edge_subtype_type{
	opr_t opr;
	struct logical_edge_subtype_type* left;
	struct logical_edge_subtype_type* right;
	edge_subt arg_edgesub;
} logical_edge_subt_t;
*/

typedef struct edge_subtype_literal_type{
	opr_t opr;//can either be none or not (note: OR, AND not possible)
	edge_subt edge_subtype_data;
} edge_subt_literal_t;

typedef struct ANDed_edgesub_literals_type{//one of the clauses of the DNF
	edge_subt_literal_t* curr_literal;
	ANDed_edgesub_literals_type* next;
} ANDed_edgesub_literals_t;


typedef struct DNF_edgesub_type{//the DNF
	ANDed_edgesub_literals_t* current_ANDed_clause;
	DNF_edgesub_type* next;
} DNF_edgesub_t;

typedef struct set_nodes_type{
	opr_t opr;
	struct set_nodes_type* left;
	struct set_nodes_type* right;
	fun_name_t fun_name;
	char* arg1;
	char* arg2;
	node_t arg3_node;
	edge_t arg3_edge;
	edge_subt arg3_edgesub;
	list_edge_subtypes_t* arg4_edgesub_list;
	//logical_edge_subt_t* arg5_edgesub_logical;
} set_nodes_t;

typedef struct list_antecedent_type{
	quantifier_t quantifier;
	char* var;
	set_nodes_t* set_nodes;
	struct list_antecedent_type* next;
} list_antecedent_t;

typedef struct logical_stmt_type{
	opr_t opr;
	struct logical_stmt_type* left;
	struct logical_stmt_type* right;
	fun_name2_t fun_name2;
	char* arg1;
	char* arg2;
	edge_t arg3;
	list_edge_subtypes_t* arg4_list;
	//logical_edge_subt_t* arg5_edgesub_logical;
	DNF_edgesub_t* arg_DNF_edgesub;
	node_state_t state_node;
	node_state2_t state_node2;
	edge_state_t state_edge;
} logical_stmt_t;

typedef struct consequent_type{
	logical_stmt_t* left_of_implies;
	logical_stmt_t* right_of_implies;
} consequent_t;

typedef struct rule_type{
	list_antecedent_t* antecedent;
	consequent_t* consequent;
} rule_t;

typedef struct list_rules_type{
	rule_t* rule;
	struct list_rules_type* next;
} list_rules_t;

#endif
