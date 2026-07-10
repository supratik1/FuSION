#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <string>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <fstream>
#include "Oprs.h"
// #include "template_datatypes.h"
#include "rules_parser_datatypes.h"
// #include "template_scanner.tab.h"
#include "rules_parser.tab.h"

// #include "../ExpressionManager_Based_Implementation/ExpressionManager/ExpressionManager.h"
// #include "../ExpressionManager_Based_Implementation/HashTableManager/HashTable_WithStandardKeys.h"
#include "ExpressionManager.h"
// #include "../ExpressionManager/ExpressionManager.h"
#include "HashTable_WithStandardKeys.h"

using namespace std;

class colorTriple
{
public:
    int red, green, blue;

public:
    colorTriple()
    {
        red = green = blue = 255;
    }
    colorTriple(int redValue, int greenValue, int blueValue)
    {
        red = redValue;
        green = greenValue;
        blue = blueValue;
    }
};

class Edge;

class Node
{
private:
    static int id_counter;

public:
    bool potential_activation_conflict; // when one node has both incoming activating and inhibiting edges this bool is true ow false
    bool potential_expression_conflict; // when one node has both incoming activating and inhibiting edges this bool is true ow false
    unsigned int node_id;
    string rep_id_name; // considered as unique id
    string rep_display_name;
    string node_type;
    string node_link; //***throw away?

    vector<Node *> components; // the component proteins in the complex
    set<Edge *> inlist;
    set<Edge *> outlist;

public:
    Node(string name);
    Node(string name, string display_name, vector<Node *> &components, string type, string link);
    ~Node();

    string get_name();
};

class Edge
{
    // private:
    //***static set of bvatoms that already used
public:
    // unsigned int edge_id; // considered as unique id
    bvatom edge_id; // considered as unique id
    Node *edge_source;
    Node *edge_target;
    string edge_type;
    list<string> edge_subtype;
    ////string belongs_to_pathway;
    list<string> belongs_to_pathways;

public:
    ////Edge(bvatom id, Node * source, Node * target, string type, list<string> subtype, string pathway);
    Edge(bvatom id, Node *source, Node *target, string type, list<string> subtype, list<string> pathways);
    Edge(bvatom id, Node *source, Node *target);
    Edge();
    ~Edge();
};

struct graph_genesis
{
    string oper; // eg. read, merge, etc.
    list<unsigned int> oper_graph_ids;
    string oper_parameters; // eg. list of source nodes, bound, etc.
};

class Graph
{
private:
    static int graph_id_counter;

public:
    unsigned int graph_id;
    map<string, Node *> rep_id_to_node_map;
    graph_genesis genesis;

public:
    Graph(map<string, Node *> new_graph, string graph_genesis_operation, list<unsigned int> operand_graphs, string other_params);
    ~Graph();
};

class BVManager;

class GraphManager
{

    // private:
public:
    // map <string, struct graph_summary> graph_genesis;
    // list < map<string, Node *> > graph_list;
    map<unsigned int, Graph *> graph_id_to_graph_map;
    map<unsigned long, string> node_id_to_rep_id_map;         //***should be unsigned int
    map<string, string> id_equiv_class_map;                   // map of all hsa ids to the rep id (can get equivalence class from here - the second member is the repr element of equiv class)
    map<string, string> display_name_map;                     //****comment out???
    map<string, string> display_id_name_map;                  //****comment out?
    map<unsigned long, unsigned int> node_id_to_graph_id_map; //***fill it
    map<bvatom, unsigned int> edge_id_to_graph_id_map;        //***fill it
    map<unsigned long, string> node_id_to_display_name;       //***fill it

    BVManager *bvm;

    /*these encodings come from supporting files*/
    map<string, int> nodeTypeEncodingMap;
    map<string, int> edgeTypeEncodingMap;
    map<string, int> edgeSubtypeEncodingMap;

    map<string, int> pathwayEncodingMap;

    // expression manager related
    // t_ExpressionManager *em;

public:
    GraphManager();
    ~GraphManager();

public:
    /* Read all supporting files into maps*/
    map<string, int> getNodeTypeEncodingMap();
    map<string, int> getEdgeTypeEncodingMap();
    map<string, int> getEdgeSubtypeEncodingMap();
    map<string, int> getPathwayEncodingMap();
    map<string, int> getMapFromFile(string filename, string delim);

    /* Helping functions */
    string toString(int i);
    string uint_to_bin(unsigned int number);
    bool invalid_string(string str);
    list<int> get_ids_from_string(string name);
    list<string> *get_list_of_names_from_string(string name, string delim);

    /* Set and update methods for graph node maps */
    void add_to_id_name_map(string id_name, string rep_id_name);
    void add_to_display_name_map(string display_name, string rep_display_name);
    void add_to_display_id_name_map(string rep_display_name, string rep_id_name);
    void add_to_local_to_hsa_node_id_map(unsigned long local_node_id, string rep_id_name);
    string get_rep_id_name_iteratively(string nodeName);
    string get_and_update_rep_id_name(string id_name);
    string get_and_update_rep_display_name(string display_name);
    string get_and_update_rep_id_name_from_display_name(string display_name);
    string get_hsa_id_from_local_node_id(unsigned long local_node_id);
    // Node *get_node_from_local_node_id(unsigned int id);
    Node *get_node_by_name(Graph *graph, string name);

    /* equivalence class map related functions */
    string flatten_id_name_map_util(string id); // flattens the id name map with rep id stored as second in all entries of one equivalence class
    void flatten_id_name_map();                 // flattens the entire id name map using func flatten_id_name_map_util

    /* Graph creation methods */
    Graph *create_graph_from_xml(string xmlfile);
    Graph *create_graph_from_txt(string txtfile);
    // Graph * create_graph_from_sbml(string sbmlfile);
    Graph *create_graph_edge_removed(Graph *input_graph, std::set<Edge *> &edges_to_remove);
    Graph *create_graph_edge_retained(Graph *input_graph, std::set<Edge *> &edges_to_remove);

    Graph *register_new_graph(map<string, Node *> &node_map, string graph_genesis_operation, list<unsigned int> operand_graphs, string other_params);
    int get_graph_list_size();

    /* Methods for graph retrieval */
    Graph *get_latest_graph();
    Graph *get_graph_from_index(int index);
    Graph *get_subgraph_with_edges_removed(Graph *graph, string edges_filename);
    Graph *get_subgraph_with_edges_retained(Graph *graph, string edges_filename);

    /* Methods related to nodes and edges */
    Node *create_node(string name, string display_name, vector<Node *> &components, string type, string link);
    Node *create_node(string name, string display_name, string type);
    Node *create_node(string name);
    Node *duplicate_node(Node *node);

    ////bvatom create_edge_id(Node * source, Node * target, string type, list<string> subtype, string pathway);
    bvatom create_edge_id(Node *source, Node *target, string type, list<string> subtype, list<string> pathways);
    Edge *create_edge(Node *source, Node *target);
    Edge *create_edge(Node *source, Node *target, string type, list<string> subtype, list<string> pathways);
    // Graph *add_edge(Graph * graph, string source, string target);
    set<bvatom> &get_all_edge_ids_from_node_map(map<string, Node *> node_map);
    set<Edge *> &get_all_edges_from_node_map(map<string, Node *> node_map);

    /* Functions for bvatom related processing*/
    bvatom get_field_as_bv_from_edge_id(bvatom edge_itr, string field_name);
    // unsigned int get_source_node_id_from_edge_id(bvatom edge_id);
    bvatom get_bvatom_for_edge_subtype(string subtype_str);
    bvatom get_bvatom_for_edge_subtype_list(const list<string> &edge_subtype_list);
    bvatom get_bvatom_for_edge_type(string type_str);
    bvatom get_bvatom_for_node_type(string node_type);
    bvatom get_bvatom_for_node_id(Node *node);

    bvatom get_bvatom_for_parser_edge_t(edge_t edge_type);
    bvatom get_bvatom_for_parser_edge_subt(edge_subt edge_subtype);
    bvatom get_bvatom_for_parser_edge_subtype_list(list_edge_subtypes_t *list_edge_subtypes);

    string get_edge_subtype_string_from_bvatom(bvatom edge_subtype);

    // get node type as string
    string get_node_type_str_from_node_type_parser(node_t node_type);

    t_Expression *get_constraint_for_tuple(map<string, Node *> &curr_tuple_ref, logical_stmt_t *left_of_implication, logical_stmt_t *right_of_implication, t_ExpressionManager *em);
    t_Expression *get_constraint_for_logic_stmt_and_tuple(map<string, Node *> &curr_tuple_ptr, logical_stmt_t *logic_stmt, t_ExpressionManager *em);
    t_Expression *get_node_constraint(Node *curr_node, node_state_t node_state, t_ExpressionManager *em);
    t_Expression *get_node_constraint(Node *curr_node, node_state2_t node2_state, t_ExpressionManager *em);
    t_Expression *get_edge_constraint(Node *node1, Node *node2, edge_state_t edge_state, edge_t edge_type, list_edge_subtypes_t *list_edge_subtypes, t_ExpressionManager *em);

    // for testing etc.
    void bvm_testing();
    void print_pathway_encoding();

private:
    // Intermediate method -- works with the type map<string, Node*> -- not made into a Graph type
    // map<string, Node *>& merge_two_graphs(map<string, Node *> g1, map<string, Node *> g2, int mode);
    map<string, Node *> merge_two_graphs(map<string, Node *> g1, map<string, Node *> g2, int mode);

    vector<t_Expression *> get_microarray_constraints(string micro_arr_decl, string micro_arr_assert, t_ExpressionManager *em);
    // functions used for constraint handling
    void handle_edge_active_unknown(set<Edge *>::iterator edge_itr, ofstream &out); // function to have constraints for edge being either activating or inhibiting when it is not actually mentioned for PPrel case
    // void writeConstraintDefinitions(ofstream& out, Graph* graph, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes);//writes constraint definitions in z3 syntax to file
    // void write_constraints_general(ofstream& out_decl, ofstream& out_defn, ofstream& out_assert, ofstream& out_confl, Graph* graph, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes);//writes constraint definitions in z3 syntax to file
    // void write_constraints_general_new(ofstream& out_decl, ofstream& out_defn, ofstream& out_assert, ofstream& out_confl, Graph* graph, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes);//writes constraint definitions in z3 syntax to file
    void write_constraints_node_based(ofstream &out_defn, Graph *graph);
    void writeConstraintDeclarations(ofstream &out, Graph *graph); // writes variable delarations in z3 syntax to file
    // bool rulesHaveAttribute(t_list_of_t_attr_content* list_of_attr, t_attr_name attribute, const char* attr_value);//checks if an attribute with certain value is present in the list of attributes
    // bool listHasString(listOfStrings* value_list, const char* attr_value);//checks if a list of atribute values contains this value
    void declare_protein_variables(ofstream &out_decl, const string &z3_active, const string &z3_changes, const string &z3_over_expressed);

    // bool rulesHaveAction(t_list_of_t_action* list_of_actns, const char* precondition_member  ,t_oper_name, const char* actions_list_of_operands_member);
    // bool list_preconditions_has(t_list_of_t_precondition* list_of_preconds, const char* precond_member);
    // bool list_operands_has(t_list_of_operands* list_of_operands, const char* list_of_operands_member);

    void update_activation_definitions_simple_PPrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter);
    void update_inhibition_definitions_simple_PPrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter);
    void update_expression_definitions(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter);
    void update_repression_definitions(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter);
    void update_activation_definitions_simple_PCrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter);
    void update_inhibition_definitions_simple_PCrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter);
    void update_definitions_edgetype_neither_PPrel(string &definitions, const string &edge_z3_present, const string &edge_z3_active, const string &source_z3_active, const string &target_z3_active, int &definition_counter);
    void update_definitions_edgetype_neither_PCrel(string &definitions, const string &edge_z3_present, const string &edge_z3_active, const string &source_z3_active, const string &target_z3_active, int &definition_counter);

    /*function for marking potentially conflicting nodes
     * By conflicting node, we mean if a node as both incoming inhibiting
     * and activating edge then it is a candidate for conflict.
     * If any incoming edge to the node with name target_id_str has subtype edge_subtype_bv
     * then this node is marked for potential conflict
     */
    void mark_potentially_conflicting_nodes(Graph *graph, ofstream &out_confl, const string &target_id_str, const bvatom &edge_subtype_bv, std::set<Node *> &set_potential_conflict_nodes);            // added by Harshit
    void mark_potentially_conflicting_nodes_expression(Graph *graph, ofstream &out_confl, const string &target_id_str, const bvatom &edge_subtype_bv, std::set<Node *> &set_potential_conflict_nodes); // added by Harshit

    // string handling functions
    string get_string_prefix(string input, char delim); // return substring from begining to just before delim

    // functions associated with modular rules parser
    t_Expression *get_constraints_for_rule(rule_t *rule_ptr, Graph *graph, set<Node *> &set_potential_conflct_nodes, t_ExpressionManager *em);
    void access_antecdents_for_filling_tuples(list_antecedent_t *antecedent_list, set<map<string, Node *>> &set_tuples, Graph *graph);
    // void fill_node_tuple(quantifier_t quant, const string& curr_var_name, Graph* graph, set_nodes_t* set_nodes, set<map<string, Node*> * >& node_tuple);
    void get_source_nodes(set<Node *> &source_node_array, Node *target_node, edge_t edge_type);
    void get_source_nodes(set<Node *> &source_node_array, Node *target_node, edge_subt edge_subtype);
    void get_target_nodes(set<Node *> &target_node_array, Node *source_node, edge_t edge_type_subtype);
    void get_target_nodes(set<Node *> &target_node_array, Node *source_node, edge_subt edge_subtype);
    void fill_set_tuple_for_antecedent(quantifier_t quant, string &var_name, Graph *graph, set_nodes_t *set_nodes_parser, set<map<string, Node *>> &set_tuples);

    void fill_temp_tuple_set(quantifier_t quant, string &var_name, set<Node *> &set_nodes_for_current_tuple, map<string, Node *> &tuple_ref, set<map<string, Node *>> &temp_tuple_set);
    void print_all_tuples(set<map<string, Node *>> &set_tuples);
    void print_single_tuple(map<string, Node *> &tuple_ref);

public:
    // functions associated with modular rules parser
    set<Node *> find_set_of_nodes_for_tuple(string &var_name, Graph *graph, set_nodes_t *set_nodes_parser, map<string, Node *> &tuple_ref);

    /* Graph manipulations and querying */
    Graph *merge_graphs(list<Graph *> graph_list, int mode);
    Graph *bounded_reach(Graph *graph, set<Node *> source_nodes_list, set<Node *> excluded_nodes_list, int bound, int type_of_edge, int direction);
    void print_genesis(int graphnum, set<int> &graphs_printed);
    // void generateConstraints(const int& graphnum, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes);// added by Harshit
    int count_GErel_targets_with_other_NON_GErel_incoming_edge(const int &graphnum); // added by Harshit - returns the count of nodes under transcriptional control that have no other incoming edge
    void print_GErel_edges(const int &graphnum);
    void print_GErel_targets(const int &graphnum);
    void print_all_node_names(const int &graphnum);
    void print_display_names_of_hsa_ids(const Graph *graph);
    void print_list_strings(list<string> &list_str, ofstream &fout);
    void print_all_pathways_in_graph(Graph *graph);
    bool node_has_input_edge_of_type(Node *node, bvatom edge_type);
    bool node_has_output_edge_of_type(Node *node, bvatom edge_type);
    bool node_has_input_edge_of_subtype(Node *node, bvatom edge_subtype);
    bool node_has_output_edge_of_subtype(Node *node, bvatom edge_subtype);
    void print_equivalence_class_from_rep_id(string repid);
    void write_microarr_decl_to_file(int graphnum);
    void write_microarr_assert_to_file(int graphnum);

    // modular rules parser is associated with
    void generate_constraints_using_rules_parser(const int graph_num, list<rule_t *> &list_of_rules, std::set<Node *> &set_potential_conflict_nodes, t_ExpressionManager *em);

    // constraint solving
    void solve_constraints(vector<t_Expression *> cons, t_ExpressionManager *em);
    vector<int> write_edgecount_cnf(string CNF_filename, map<t_Expression *, pair<string, int>> getCNFIndexMap, int &idx, int &clauses);

    /* Display of graphs */
    void write_node_in_dot(ofstream &myfile, Node *node, map<string, colorTriple> &colorMap, map<string, char> &alreadySeenNode);
    bool write_graph_as_dot(Graph *graph, map<string, colorTriple> &colorMap, string dotFileName);
    bool edge_type_subtype_match(Edge *edge_ptr, edge_t edge_type, list_edge_subtypes_t *list_edge_subtypes);
};

#endif
