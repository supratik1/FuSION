#ifndef RULES_TUPLE_STORE_H
#define RULES_TUPLE_STORE_H

#include "rules_parser_datatypes.h"
#include "rules_parser.tab.h"
#include "GraphManager.h"
#include "GraphManagerNew.h"

struct TupleStoreNode{
		bool leaf;//true if the node is a leaf
		map<string, Node*>* tuple_ptr;//only leaf can point to a tuple - otherwise it is NULL
		vector<TupleStoreNode*> children;//only non-leaf nodes have children, for leaf its size is zero
		quantifier_t quant;//only non leaf nodes have quantifiers
};

class RuleTupleStore{
private:
	TupleStoreNode* root;

	void fill_first_level_tuples(list_antecedent_t* antecedent_list, Graph* graph, GraphManager* gm);
	void fill_next_level_tuples(TupleStoreNode* tuple_store_node, list_antecedent_t* antecedent_list, Graph* graph, GraphManager* gm);
	void print_all_tuples_under_node(TupleStoreNode* tuple_store_node);
	void print_single_tuple(map<string, Node*>* tuple_ptr);
	t_Expression* get_constraint_internal(TupleStoreNode* tuple_store_node, consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManager* gm);

public:
	RuleTupleStore();
	RuleTupleStore(list_antecedent_t* antecedent_list, Graph* graph, GraphManager* gm);
	~RuleTupleStore();
	void print_all_tuples();
	t_Expression* get_constraint_em(consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManager* gm);
};

struct TupleStoreNodeNew{
		bool leaf;//true if the node is a leaf
		map<string, int>* tuple_ptr;//only leaf can point to a tuple - otherwise it is NULL
		vector<TupleStoreNodeNew*> children;//only non-leaf nodes have children, for leaf its size is zero
		quantifier_t quant;//only non leaf nodes have quantifiers
};

class RuleTupleStoreNew{
private:
	TupleStoreNodeNew* root;

	void fill_first_level_tuples(list_antecedent_t* antecedent_list, GraphNew* graph, GraphManagerNew* gm);
	void fill_next_level_tuples(TupleStoreNodeNew* tuple_store_node, list_antecedent_t* antecedent_list, GraphNew* graph, GraphManagerNew* gm);
	void print_all_tuples_under_node(TupleStoreNodeNew* tuple_store_node, GraphNew* graph);
	void print_single_tuple(map<string, int>* tuple_ptr, GraphNew* graph);
	t_Expression* get_constraint_internal(TupleStoreNodeNew* tuple_store_node, consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManagerNew* gm, GraphNew* graph);

public:
	RuleTupleStoreNew();
	RuleTupleStoreNew(list_antecedent_t* antecedent_list, GraphNew* graph, GraphManagerNew* gm);
	~RuleTupleStoreNew();
	void print_all_tuples(GraphNew* graph);
	t_Expression* get_constraint_em(consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManagerNew* gm, GraphNew* graph);
};

#endif
