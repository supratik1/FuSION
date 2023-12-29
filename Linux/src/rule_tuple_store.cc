#include "../include/rule_tuple_store.h"
#include <stack>

//these functions and vars needed for Expression Manager

extern map<string, t_Expression *> getExpressionMap;

extern string buildRelationalExpression(string reader, t_ExpressionManager* em, t_Expression* &expr);
extern string buildArithmaticExpression(string reader, t_ExpressionManager* em, t_Expression* &expr);
extern string buildSelectExpression(string reader,t_ExpressionManager* em,t_Expression* &expr);
extern string buildConcatExpression(string reader, t_ExpressionManager *em, t_Expression* &expr);
extern string buildBracketExpression(string reader, t_ExpressionManager *em, t_Expression* &expr);
extern string buildVariableExpression(string reader, t_ExpressionManager *em, t_Expression* &expr);

extern t_Expression* build_OR_constraints(string s1,string s2,int a1,int a2, t_ExpressionManager *em);
extern t_Expression* build_AND_constraints(string s1,string s2,int a1,int a2, t_ExpressionManager *em);
extern t_Expression* build_OR_constraints(t_Expression* e1,t_Expression* e2,int a1,int a2, t_ExpressionManager *em);
extern t_Expression* build_AND_constraints(t_Expression* e1,t_Expression* e2,int a1,int a2, t_ExpressionManager *em);

void RuleTupleStore::print_single_tuple(map<string, Node*>* tuple_ptr){
	if(tuple_ptr == NULL){
		cout << "nothing to print for NULL tuple\n";
	}

	map<string, Node*>::iterator tuple_itr;
	for(tuple_itr = tuple_ptr->begin(); tuple_itr != tuple_ptr->end(); ++tuple_itr){
		cout << tuple_itr->first << ":" << tuple_itr->second->rep_id_name << " ";
	}
}

void RuleTupleStoreNew::print_single_tuple(map<string, int>* tuple_ptr, GraphNew* graph){
	if(tuple_ptr == NULL){
		cout << "nothing to print for NULL tuple\n";
	}

	map<string, int>::iterator tuple_itr;
	for(tuple_itr = tuple_ptr->begin(); tuple_itr != tuple_ptr->end(); ++tuple_itr){
		cout << tuple_itr->first << ":" << graph->get_rep_id_from_nid(tuple_itr->second) << " ";
	}
}

void RuleTupleStore::print_all_tuples_under_node(TupleStoreNode* tuple_store_node){
	if(tuple_store_node == NULL){
		return;
	}
	else if(tuple_store_node->leaf){
		return;
	}
	else{
		//base case

		bool any_child_points_to_tuples = false;
		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if((*child_itr)->tuple_ptr){
				any_child_points_to_tuples = true;
				break;
			}
		}

		if(any_child_points_to_tuples){
			for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
				if((*child_itr)->tuple_ptr){
					print_single_tuple((*child_itr)->tuple_ptr);
					cout << endl;
				}
			}
		}
		//recursive case: if we haven't reached the tuple level then we need to make recursive calls to subtrees
		else{
			for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
				if(*child_itr)
					print_all_tuples_under_node(*child_itr);
			}
		}
	}
}

void RuleTupleStoreNew::print_all_tuples_under_node(TupleStoreNodeNew* tuple_store_node, GraphNew* graph){
	if(tuple_store_node == NULL){
		return;
	}
	else if(tuple_store_node->leaf){
		return;
	}
	else{
		//base case

		bool any_child_points_to_tuples = false;
		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if((*child_itr)->tuple_ptr){
				any_child_points_to_tuples = true;
				break;
			}
		}

		if(any_child_points_to_tuples){
			for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
				if((*child_itr)->tuple_ptr){
					print_single_tuple((*child_itr)->tuple_ptr, graph);
					cout << endl;
				}
			}
		}
		//recursive case: if we haven't reached the tuple level then we need to make recursive calls to subtrees
		else{
			for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
				if(*child_itr)
					print_all_tuples_under_node(*child_itr, graph);
			}
		}
	}
}

void RuleTupleStore::print_all_tuples(){
	print_all_tuples_under_node(this->root);
}


void RuleTupleStoreNew::print_all_tuples(GraphNew* graph){
	print_all_tuples_under_node(this->root, graph);
}


RuleTupleStore::RuleTupleStore(){
	this->root = NULL;
}

RuleTupleStore::~RuleTupleStore(){

}

RuleTupleStoreNew::~RuleTupleStoreNew(){

}

void RuleTupleStore::fill_first_level_tuples(list_antecedent_t* antecedent_list, Graph* graph, GraphManager* gm){

	map<string, Node*> dummy_tuple;
	set<map<string, Node*> > set_tuples;
	string var = antecedent_list->var;
	set<Node*>	set_nodes_for_current_tuple = gm->find_set_of_nodes_for_tuple(var, graph, antecedent_list->set_nodes, dummy_tuple);
	for(set<Node*>::iterator node_set_itr = set_nodes_for_current_tuple.begin(); node_set_itr != set_nodes_for_current_tuple.end(); node_set_itr++){
		//map<string, Node*>* tuple = new map<string, Node*>;
		map<string, Node*> tuple;
		tuple.insert(pair<string, Node*>(var, *node_set_itr));
		set_tuples.insert(tuple);
	}
	//make children
	set<map<string, Node*> >::iterator tuple_set_itr;
	for(tuple_set_itr = set_tuples.begin(); tuple_set_itr != set_tuples.end(); ++tuple_set_itr){
		TupleStoreNode* child_ptr = new TupleStoreNode;
		assert(child_ptr);
		child_ptr->leaf = true;
		child_ptr->quant = NONE_QUANT;
		child_ptr->tuple_ptr = new map<string, Node*>((*tuple_set_itr));
		assert(child_ptr->tuple_ptr);
		this->root->children.push_back(child_ptr);
	}
	return;
}

void RuleTupleStoreNew::fill_first_level_tuples(list_antecedent_t* antecedent_list, GraphNew* graph, GraphManagerNew* gm){
	map<string, int> dummy_tuple;//string to node id
	set<map<string, int> > set_tuples;
	string var = antecedent_list->var;
	set<int> set_nodes_for_current_tuple = gm->find_set_of_nodes_for_tuple(var, graph, antecedent_list->set_nodes, dummy_tuple);
	for(set<int>::iterator node_set_itr = set_nodes_for_current_tuple.begin(); node_set_itr != set_nodes_for_current_tuple.end(); node_set_itr++){
		//map<string, Node*>* tuple = new map<string, Node*>;
		map<string, int> tuple;
		tuple.insert(pair<string, int>(var, *node_set_itr));
		set_tuples.insert(tuple);
	}
	//make children
	set<map<string, int> >::iterator tuple_set_itr;
	for(tuple_set_itr = set_tuples.begin(); tuple_set_itr != set_tuples.end(); ++tuple_set_itr){
		TupleStoreNodeNew* child_ptr = new TupleStoreNodeNew;
		assert(child_ptr);
		child_ptr->leaf = true;
		child_ptr->quant = NONE_QUANT;
		child_ptr->tuple_ptr = new map<string, int>((*tuple_set_itr));
		assert(child_ptr->tuple_ptr);
		this->root->children.push_back(child_ptr);
	}
	return;
}

void RuleTupleStore::fill_next_level_tuples(TupleStoreNode* tuple_store_node, list_antecedent_t* antecedent_list, Graph* graph, GraphManager* gm){
	//this function works on a tree filled up to certain level

	if(antecedent_list == NULL){//there is nothing in the next quantification level, so we return
		return;
	}
	if(tuple_store_node == NULL){
		return;
	}
	//base case when any child points to a tuple
	bool any_child_points_to_tuples = false;
	for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
		if((*child_itr)->tuple_ptr){
			any_child_points_to_tuples = true;
			break;
		}
	}
	if(any_child_points_to_tuples){
		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if((*child_itr)->tuple_ptr){
				map<string, Node*> old_tuple = *((*child_itr)->tuple_ptr);
				assert(!old_tuple.empty());
				string var = antecedent_list->var;
				set<Node*>	set_nodes_for_current_tuple = gm->find_set_of_nodes_for_tuple(var, graph, antecedent_list->set_nodes, old_tuple);

				delete (*child_itr)->tuple_ptr;
				(*child_itr)->tuple_ptr = NULL;
				if(set_nodes_for_current_tuple.empty()){
					(*child_itr)->leaf = true;
					(*child_itr)->quant = NONE_QUANT;
				}

				else{
					(*child_itr)->leaf = false;
					(*child_itr)->quant = antecedent_list->quantifier;
					set<map<string, Node*> > set_tuples;
					set<map<string, Node*> >::iterator tuple_set_itr;
					for(set<Node*>::iterator node_set_itr = set_nodes_for_current_tuple.begin(); node_set_itr != set_nodes_for_current_tuple.end(); node_set_itr++){
						map<string, Node*> tuple(old_tuple);
						tuple.insert(pair<string, Node*>(var, *node_set_itr));
						set_tuples.insert(tuple);
					}
					for(tuple_set_itr = set_tuples.begin(); tuple_set_itr != set_tuples.end(); ++tuple_set_itr){
						TupleStoreNode* child_ptr = new TupleStoreNode;
						assert(child_ptr);
						child_ptr->leaf = true;
						child_ptr->quant = NONE_QUANT;
						child_ptr->tuple_ptr = new map<string, Node*>((*tuple_set_itr));
						assert(child_ptr->tuple_ptr);
						(*child_itr)->children.push_back(child_ptr);
					}
				}
			}
		}
	}

	//recursive case
	else{
		//this mean none of the child points to any tuple
		//if none of the children has any further children then just return

		bool some_child_has_children = false;
		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if(!((*child_itr)->children.empty())){
				some_child_has_children = true;
				break;
			}
		}

		if(!some_child_has_children)
			return;

		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			fill_next_level_tuples(*child_itr, antecedent_list, graph, gm);
		}
	}
}

void RuleTupleStoreNew::fill_next_level_tuples(TupleStoreNodeNew* tuple_store_node, list_antecedent_t* antecedent_list, GraphNew* graph, GraphManagerNew* gm){
	//this function works on a tree filled up to certain level

	if(antecedent_list == NULL){//there is nothing in the next quantification level, so we return
		return;
	}
	if(tuple_store_node == NULL){
		return;
	}
	//base case when any child points to a tuple
	bool any_child_points_to_tuples = false;
	for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
		if((*child_itr)->tuple_ptr){
			any_child_points_to_tuples = true;
			break;
		}
	}
	if(any_child_points_to_tuples){
		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if((*child_itr)->tuple_ptr){
				map<string, int> old_tuple = *((*child_itr)->tuple_ptr);
				assert(!old_tuple.empty());
				string var = antecedent_list->var;
				set<int> set_nodes_for_current_tuple = gm->find_set_of_nodes_for_tuple(var, graph, antecedent_list->set_nodes, old_tuple);

				delete (*child_itr)->tuple_ptr;
				(*child_itr)->tuple_ptr = NULL;
				if(set_nodes_for_current_tuple.empty()){
					(*child_itr)->leaf = true;
					(*child_itr)->quant = NONE_QUANT;
				}

				else{
					(*child_itr)->leaf = false;
					(*child_itr)->quant = antecedent_list->quantifier;
					set<map<string, int> > set_tuples;
					set<map<string, int> >::iterator tuple_set_itr;
					for(set<int>::iterator node_set_itr = set_nodes_for_current_tuple.begin(); node_set_itr != set_nodes_for_current_tuple.end(); node_set_itr++){
						map<string, int> tuple(old_tuple);
						tuple.insert(pair<string, int>(var, *node_set_itr));
						set_tuples.insert(tuple);
					}
					for(tuple_set_itr = set_tuples.begin(); tuple_set_itr != set_tuples.end(); ++tuple_set_itr){
						TupleStoreNodeNew* child_ptr = new TupleStoreNodeNew;
						assert(child_ptr);
						child_ptr->leaf = true;
						child_ptr->quant = NONE_QUANT;
						child_ptr->tuple_ptr = new map<string, int>((*tuple_set_itr));
						assert(child_ptr->tuple_ptr);
						(*child_itr)->children.push_back(child_ptr);
					}
				}
			}
		}
	}

	//recursive case
	else{
		//this mean none of the child points to any tuple
		//if none of the children has any further children then just return

		bool some_child_has_children = false;
		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if(!((*child_itr)->children.empty())){
				some_child_has_children = true;
				break;
			}
		}

		if(!some_child_has_children)
			return;

		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			fill_next_level_tuples(*child_itr, antecedent_list, graph, gm);
		}
	}
}

RuleTupleStore::RuleTupleStore(list_antecedent_t* antecedent_list, Graph* graph, GraphManager* gm){
	if(antecedent_list == NULL){
		cerr << "Error: no antecdents, exiting . . . " << endl;
		exit(1);
	}
	else{

		//since antecedent list stored is in reverse of the order in the rules file a stack is used
		stack<list_antecedent_t*> antecedent_stack;
		while(antecedent_list){
			antecedent_stack.push(antecedent_list);
			antecedent_list = antecedent_list->next;
		}
		list_antecedent_t* curr_antecdent = antecedent_stack.top();
		antecedent_stack.pop();

		//create root with the topmost quantifier (treat it as nonleaf for our purpose)
		this->root = new TupleStoreNode;
		this->root->leaf = false;
		this->root->tuple_ptr = NULL;
		this->root->quant = curr_antecdent->quantifier;

		//fill the first level of tuples
		fill_first_level_tuples(curr_antecdent, graph, gm);

		while(!antecedent_stack.empty()){
			list_antecedent_t* curr_antecdent = antecedent_stack.top();
			antecedent_stack.pop();
			fill_next_level_tuples(root, curr_antecdent, graph, gm);
		}
	}
}

RuleTupleStoreNew::RuleTupleStoreNew(list_antecedent_t* antecedent_list, GraphNew* graph, GraphManagerNew* gm){
	if(antecedent_list == NULL){
		cerr << "Error: no antecdents, exiting . . . " << endl;
		exit(1);
	}
	else{

		//since antecedent list stored is in reverse of the order in the rules file a stack is used
		stack<list_antecedent_t*> antecedent_stack;
		while(antecedent_list){
			antecedent_stack.push(antecedent_list);
			antecedent_list = antecedent_list->next;
		}
		list_antecedent_t* curr_antecdent = antecedent_stack.top();
		antecedent_stack.pop();

		//create root with the topmost quantifier (treat it as nonleaf for our purpose)
		this->root = new TupleStoreNodeNew;
		this->root->leaf = false;
		this->root->tuple_ptr = NULL;
		this->root->quant = curr_antecdent->quantifier;

		//fill the first level of tuples
		fill_first_level_tuples(curr_antecdent, graph, gm);

		while(!antecedent_stack.empty()){
			list_antecedent_t* curr_antecdent = antecedent_stack.top();
			antecedent_stack.pop();
			fill_next_level_tuples(root, curr_antecdent, graph, gm);
		}
	}
}

t_Expression* RuleTupleStore::get_constraint_internal(TupleStoreNode* tuple_store_node, consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManager* gm){
	logical_stmt_t* left_of_implication = consequent_curr_rule->left_of_implies;
	logical_stmt_t* right_of_implication = consequent_curr_rule->right_of_implies;
	vector<t_Expression*> expressions_to_operate_on;
	vector<t_Expression*>::iterator expr_vec_itr;

	//base case when we are at the tuple levels
	bool any_child_points_to_tuples = false;
	for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
		if((*child_itr)->tuple_ptr){
			any_child_points_to_tuples = true;
			break;
		}
	}
	if(any_child_points_to_tuples){
		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if((*child_itr)->tuple_ptr){
				map<string, Node*> curr_tuple(*(*child_itr)->tuple_ptr);
				t_Expression* cons_for_tuple = gm->get_constraint_for_tuple(curr_tuple, left_of_implication, right_of_implication, em);
				expressions_to_operate_on.push_back(cons_for_tuple);
			}
		}
	}

	//recursive case
	else{
		//this mean none of the child points to any tuple
		//if none of the children has any further children then return NULL

		bool some_child_has_children = false;
		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if(!((*child_itr)->children.empty())){
				some_child_has_children = true;
				break;
			}
		}

		if(!some_child_has_children)
			return NULL;


		for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			//for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
				if(*child_itr && (*child_itr)->quant != NONE_QUANT ){
					t_Expression* cons_for_children = get_constraint_internal(*child_itr, consequent_curr_rule, em, gm);
					expressions_to_operate_on.push_back(cons_for_children);
				}
		//	}
		}
	}

	if(expressions_to_operate_on.empty())
		return NULL;

	//constraints ready in the vector expressions_to_operate_on, now operate the quantifier on them
	if(tuple_store_node->quant == FOR_ALL_QUANT){
		expr_vec_itr = expressions_to_operate_on.begin();
		t_Expression* expr1 = *expr_vec_itr;
		++expr_vec_itr;
		while(expr_vec_itr != expressions_to_operate_on.end()){
			t_Expression* expr2 = *expr_vec_itr;
			expr1 = build_AND_constraints(expr1, expr2, 0, 0, em);
			 ++expr_vec_itr;
		}
		return expr1;
	}
	else if(tuple_store_node->quant == THERE_EXISTS_QUANT){
		expr_vec_itr = expressions_to_operate_on.begin();
		t_Expression* expr1 = *expr_vec_itr;
		++expr_vec_itr;
		while(expr_vec_itr != expressions_to_operate_on.end()){
			t_Expression* expr2 = *expr_vec_itr;
			expr1 = build_OR_constraints(expr1, expr2, 0, 0, em);
			++expr_vec_itr;
		}
		return expr1;
	}
	else{
		cerr << "Error: none quantifier\n";
		exit(1);
	}
}

t_Expression* RuleTupleStoreNew::get_constraint_internal(TupleStoreNodeNew* tuple_store_node, consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManagerNew* gm, GraphNew* graph){
	logical_stmt_t* left_of_implication = consequent_curr_rule->left_of_implies;
	logical_stmt_t* right_of_implication = consequent_curr_rule->right_of_implies;
	vector<t_Expression*> expressions_to_operate_on;
	vector<t_Expression*>::iterator expr_vec_itr;

	//base case when we are at the tuple levels
	bool any_child_points_to_tuples = false;
	for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
		if((*child_itr)->tuple_ptr){//works because all tuples stored at same depth of the tree
			any_child_points_to_tuples = true;
			break;
		}
	}
	if(any_child_points_to_tuples){
		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if((*child_itr)->tuple_ptr){
				map<string, int> curr_tuple(*(*child_itr)->tuple_ptr);
				t_Expression* cons_for_tuple = gm->get_constraint_for_tuple(curr_tuple, left_of_implication, right_of_implication, em, graph);
				expressions_to_operate_on.push_back(cons_for_tuple);
			}
		}
	}

	//recursive case
	else{
		//this mean none of the children points to any tuple
		//if none of the children has any further children then return NULL

		bool some_child_has_children = false;
		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if(!((*child_itr)->children.empty())){
				some_child_has_children = true;
				break;
			}
		}

		if(!some_child_has_children)
			return NULL;


		for(vector<TupleStoreNodeNew*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			//for(vector<TupleStoreNode*>::iterator child_itr = tuple_store_node->children.begin(); child_itr != tuple_store_node->children.end(); ++child_itr){
			if(*child_itr && (*child_itr)->quant != NONE_QUANT ){
				t_Expression* cons_for_children = get_constraint_internal(*child_itr, consequent_curr_rule, em, gm, graph);
				//???shouldn't a NULL check be done and only non-NULL constraints be pushed???
				if(cons_for_children){
					expressions_to_operate_on.push_back(cons_for_children);
				}
			}
			//	}
		}
	}

	if(expressions_to_operate_on.empty())
		return NULL;

	//constraints ready in the vector expressions_to_operate_on, now operate the quantifier on them
	if(tuple_store_node->quant == FOR_ALL_QUANT){
		expr_vec_itr = expressions_to_operate_on.begin();
		t_Expression* expr1 = *expr_vec_itr;
		++expr_vec_itr;
		while(expr_vec_itr != expressions_to_operate_on.end()){
			t_Expression* expr2 = *expr_vec_itr;
			expr1 = build_AND_constraints(expr1, expr2, 0, 0, em);
			++expr_vec_itr;
		}
		return expr1;
	}
	else if(tuple_store_node->quant == THERE_EXISTS_QUANT){
		expr_vec_itr = expressions_to_operate_on.begin();
		t_Expression* expr1 = *expr_vec_itr;
		++expr_vec_itr;
		while(expr_vec_itr != expressions_to_operate_on.end()){
			t_Expression* expr2 = *expr_vec_itr;
			expr1 = build_OR_constraints(expr1, expr2, 0, 0, em);
			++expr_vec_itr;
		}
		return expr1;
	}
	else{
		cerr << "Error: none quantifier\n";
		exit(1);
	}
}

t_Expression* RuleTupleStore::get_constraint_em(consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManager* gm){
	/*logical_stmt_t*  left_of_implication = consequent_curr_rule->left_of_implies;
	logical_stmt_t* right_of_implication = consequent_curr_rule->right_of_implies;*/
	if(root == NULL)
		return NULL;
	return get_constraint_internal(root, consequent_curr_rule, em, gm);
}

t_Expression* RuleTupleStoreNew::get_constraint_em(consequent_t* consequent_curr_rule, t_ExpressionManager* em, GraphManagerNew* gm, GraphNew* graph){
	if(root == NULL)
		return NULL;
	return get_constraint_internal(root, consequent_curr_rule, em, gm, graph);
}
