#ifndef GRAPH_MANAGER_NEW_H
#define GRAPH_MANAGER_NEW_H

#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <set>
#include "rules_parser_datatypes.h"
#include "rules_parser.tab.h"
#include "ExpressionManager.h"
#include "HashTable_WithStandardKeys.h"
#include "GraphManager.h"

#include "ModifiedExpressionManager.h"
#include "Z3Solver.h"

#include "z3++.h"
// #include "macros.h"

using namespace std;

class GraphManagerNew;
class GraphNew;

class color_triple_new
{
public:
        int red, green, blue;

public:
        color_triple_new()
        {
                red = green = blue = 255;
        }
        color_triple_new(int redValue, int greenValue, int blueValue)
        {
                red = redValue;
                green = greenValue;
                blue = blueValue;
        }
};

struct graph_genesis_new
{

        graph_genesis_new(std::string oper_str, std::list<int> oper_ids)
            : oper(oper_str), operand_graph_ids(oper_ids) {}

        std::string oper;
        std::list<int> operand_graph_ids;
};

struct PO_point_result_tuple
{

        PO_point_result_tuple(int points, int edge_relax1, int node_relax1, int edge_relax2, int node_relax2)
            : num_of_points(points), e1(edge_relax1), n1(node_relax1), e2(edge_relax2), n2(node_relax2) {}

        int num_of_points;
        int e1;
        int n1;
        int e2;
        int n2;
};

class NodeNew
{
private:
        static int node_id_counter;

        int node_id;
        std::string rep_id;
        std::vector<std::string> all_id;
        std::string node_type;
        std::vector<int> components;
        std::vector<std::string> display_names;
        std::string description;
        std::string compartment;
        vector<string> pathways;

        std::vector<int> inlist;
        std::vector<int> outlist;

        int other_value1;
        float excess;

public:
        NodeNew();
        int get_node_id();
        void set_rep_id(std::string rep_id);
        std::string get_rep_id();
        std::vector<std::string> get_all_id();
        void set_all_id(std::vector<std::string> all_id);
        void add_to_all_id(std::string id);
        std::vector<int> get_comp_ids();
        void add_comp_id(int id);
        void update_component_id_at_index(int pos, int value);
        void set_all_display_names(std::vector<std::string> all_disp_names);
        std::vector<std::string> get_disp_ids();
        void add_disp_id(std::string id);
        std::string get_node_type();
        void add_node_type(std::string type);
        void remove_node_type();
        std::vector<int> get_inlist();
        void add_to_inlist(int eid);
        void remove_from_inlist(int eid);
        std::vector<int> get_outlist();
        void add_to_outlist(int eid);
        void remove_from_outlist(int eid);
        int get_in_degree();
        int get_out_degree();
        bool is_target_of_GErel_edge(GraphNew *graph);
        void add_description_for_node(std::string desc);
        std::string get_description_for_node();
        void add_compartment_for_node(std::string comp);
        std::string get_compartment_for_node();
        void add_pathwys_for_node(string id);
        set<string> get_all_pathways_for_node(string id);
        void add_other_value1(int value);
        int get_other_value1();
        void set_node_excess(float value);
        float get_node_excess();
};

class EdgeNew
{
private:
        static int edge_id_counter;

        int edge_id;
        int edge_source;
        int edge_target;
        std::string edge_type;
        std::vector<std::string> edge_subtypes;
        std::vector<std::string> edge_belongs_to_pathways;
        std::string reaction_desc;
        int other1, other2, other3, other4;
        std::vector<int> other;
        bool deleted; // added by anjan - 21 march 2017

        int weight;
        std::vector<float> weight_tuple;

        float weight_f;
        float capacity, flow;

public:
        EdgeNew();
        int get_edge_id();
        std::string get_edge_type();
        void add_edge_type(std::string type);
        void remove_edge_type();
        std::vector<std::string> get_edge_subtypes();
        void add_subtype(std::string subtype);
        std::vector<std::string> get_pathways();
        void add_pathway(std::string pathway);
        void add_source(int nid);
        int get_source();
        void add_target(int nid);
        int get_target();
        bool has_subtype(std::string subtype);
        void add_reaction_desc(std::string desc);
        std::string get_reaction_desc();
        void add_other_values(int value);
        std::vector<int> get_other_values();
        void set_other_value1(int value);
        int get_other_value1();
        void set_other_value2(int value);
        int get_other_value2();
        void set_other_value3(int value);
        int get_other_value3();
        void set_other_value4(int value);
        int get_other_value4();
        void set_edge_deleted_status(bool status);
        bool get_edge_deleted_status();

        void set_edge_weight(int weight);
        int get_edge_weight();

        void add_to_weight_tuple(float weight);
        void set_weight_factor_at_tuple_index(int index, float value);
        float get_weight_factor_at_tuple_index(int index);
        void set_weight_tuple(vector<float> tuple);
        std::vector<float> get_weight_tuple();

        void set_edge_weight_float(float weight_value);
        float get_edge_weight_float();
        void set_edge_capacity(float value);
        float get_edge_capacity();
        void set_edge_flow(float value);
        float get_edge_flow();
};

class GraphNew
{
private:
        int graph_id;
        std::vector<int> NIDSg; // node ids of the nodes from this graph
        std::vector<int> EIDSg; // edge ids of the edges from this graph

public:
        int first_offset;
        map<int, int> map_nid_to_nseq;
        map<int, int> map_nseq_to_nid;

        map<int, int> map_eid_to_eseq;
        map<int, int> map_eseq_to_eid;

        vector<vector<bool>> adj_matrix;
        map<int, set<int>> merged_eid_to_construction_eids_map;

private:
        std::unordered_map<int, NodeNew *> nid_to_node_map;
        std::unordered_map<int, EdgeNew *> eid_to_edge_map;

        std::map<std::string, int> rep_id_to_nid_map;        // reverse map from rep-id to nid
        std::map<std::string, std::string> id_to_rep_id_map; // map from all-ids to rep-id

        template <typename Data>
        void print_vector(std::vector<Data> &vec, std::ofstream &fout);
        template <typename Data>
        void print_set(std::set<Data> &vec, std::ofstream &fout);

        template <typename Key, typename Val>
        void add_to_map(std::map<Key, Val> &generic_map, Key k, Val v);

        template <typename Key, typename Val>
        void delete_from_map(std::map<Key, Val> &generic_map, Key k);

        template <typename Key, typename Val>
        void print_map(std::map<Key, Val> &generic_map, std::ofstream &fout);

        template <typename Key, typename Val>
        void add_to_map2(std::map<Key, std::vector<Val>> &generic_map, Key k, Val v);

        template <typename Key, typename Val>
        std::vector<Val> get_val_map2(std::map<Key, std::vector<Val>> &generic_map, Key k);

        template <typename Key, typename Val>
        void delete_from_map2(std::map<Key, std::vector<Val>> &generic_map, Key k);

        template <typename Key, typename Val>
        void print_map2(std::map<Key, std::vector<Val>> &generic_map, std::ofstream &fout);

        template <typename Key, typename Val>
        bool map_has_key_with_val(std::map<Key, std::vector<Val>> &generic_map, Key k, Val v);

        void write_node_to_dot_file(std::ofstream &myfile, int nid, std::set<int> &drawn_nodes, color_triple_new color);
        void write_node_to_dot_file_richer(GraphManagerNew *graph_man, std::ofstream &myfile, int nid, std::set<int> &drawn_nodes, color_triple_new color, string node_style, string border_color, int penwidth);

        void write_node_to_xml_file(int nid, std::map<int, int> &nid_to_xml_node_id_map, std::ofstream &myfile);
        void write_edge_to_xml_file(int eid, std::map<int, int> &nid_to_xml_node_id_map, std::ofstream &myfile);

        void write_file_to_sif(std::ofstream &myfile, int nid);
        void write_node_to_grml_file(int nid, std::map<int, int> &nid_to_ml_node_id_map, std::ofstream &myfile, string color_map_filename, int fold_change);
        void write_edge_to_grml_file(int eid, std::map<int, int> &eid_to_xml_edge_id_map, std::ofstream &myfile, std::string nme);
        void write_node_to_gml_file(int nid, std::map<int, int> &nid_to_ml_node_id_map, std::ofstream &myfile);
        void write_edge_to_gml_file(int eid, std::map<int, int> &eid_to_xml_edge_id_map, std::ofstream &myfile);
        void write_edge_to_html(int eid, map<int, int> &eid_to_ml_edge_id_map, std::ofstream &hout);
        // void write_edge_to_html_new(int eid, map<int, int>& eid_to_ml_edge_id_map, std::ofstream &hout, set<string>& up_reg_rep_ids, set<string>& down_reg_rep_ids);
        void write_edge_to_html_new(int eid, map<int, int> &eid_to_ml_edge_id_map, std::ofstream &hout, set<string> &up_reg_rep_ids, set<string> &down_reg_rep_id, set<string> &up_reg_in_all_mincut, set<string> &down_reg_in_all_mincut);

public:
        GraphNew();
        ~GraphNew();

        void write_mincut_edge_to_html(int eid, std::ofstream &hout, set<string> &up_reg_rep_ids, set<string> &down_reg_rep_ids, map<string, int> &nodes_in_mincut, map<string, int> &nodes_in_all_mincuts, set<string> &written_edges);

        int check_if_node_already_created(std::string rep_id);
        int check_if_edge_already_created(int new_source_nid, int new_target_nid, std::string edge_type, std::vector<std::string> edge_subtypes);
        int create_new_node();
        NodeNew *get_node_from_nid(int nid);
        int create_new_edge();
        void fill_maps_for_new_edge(int new_eid, int new_source_nid, int new_target_nid, string edge_type, vector<string> edge_subtypes, vector<string> edge_belongs_to_pathways);
        EdgeNew *get_edge_from_eid(int eid);
        void add_nid_node(int nid, NodeNew *node);
        void add_eid_edge(int eid, EdgeNew *edge);

        int get_graph_id();
        void set_graph_id(int gid);

        void set_first_offset(int offset);
        int get_first_offset();

        void add_to_rep_id_nid_map(std::string rep_id, int nid);
        void add_rep_id_for_node(int nid, std::string rep_id);
        void add_node_id(int nid);
        std::vector<int> get_node_ids();

        void add_edge_id(int eid);
        std::vector<int> get_edge_ids();

        std::string get_rep_id_from_id(std::string id);        // returns "" if absent
        std::string get_rep_id_from_nid(int nid);              // returns "" if absent //
        int get_nid_from_rep_id(std::string repid);            // returns -1 if absent //
        void add_id_rep_id(std::string id, std::string repid); // insert in id_to_rep_id_map
        vector<string> get_all_ids_for_rep_id(string rep_id);

        void set_all_ids_for_node(int new_nid, std::vector<std::string> all_ids);
        std::vector<std::string> get_all_ids_of_node(int nid); // gives all ids of a node

        bool node_has_id(int nid, std::string id);     // checks if node nid has id as part of its all ids
        void add_id_for_node(int nid, std::string id); // adds id to all ids of node nid

        std::vector<int> get_component_ids_of_node(int nid);  // gives all component ids of a node
        bool node_has_component_id(int nid, int comp_id);     // checks if node nid has comp_id as its component
        void add_component_id_for_node(int nid, int comp_id); // adds comp_id to components of node nid
        void update_component_of_node_at_index(int nid, int pos, int value);

        std::vector<std::string> get_all_display_ids_of_node(int nid); // gives all display ids of a node
        bool node_has_display_id(int nid, std::string id);             // checks if node nid has id as part of its display ids
        void add_display_id_for_node(int nid, std::string id);         // adds id to all display ids of node nid
        void set_all_display_id_for_node(int nid, std::vector<std::string> all_disp_names);

        std::string get_node_type(int nid);
        void add_node_type(int nid, std::string type);
        void remove_node_type_info(int nid);

        void get_all_eids_between_two_nodes(int src_nid, int tgt_nid, set<int> &all_eids);

        std::string get_edge_type(int eid);
        void add_edge_type(int eid, std::string type);
        void remove_edge_id_type(int eid);

        void add_edge_to_adj_matrix(int i, int j);
        bool get_edge_presence_status_from_adj_matrix(int i, int j);

        std::vector<std::string> get_edge_subtypes_of_edge(int eid);
        bool edge_has_subtype(int eid, std::string subtype);     // checks if edge eid has a subtype
        void add_subtype_for_edge(int eid, std::string subtype); // adds subtype for edge eid
        void set_delete_status_for_edge(int eid, bool status);   // anjan 21-03-17
        bool get_delete_status_for_edge(int eid);                // anjan 21-03-17

        void add_reaction_desc_for_edge(int eid, std::string desc);
        std::string get_reaction_desc_for_edge(int eid);

        void add_description_for_node(int nid, std::string desc);
        std::string get_description_for_node(int nid);

        void add_compartment_for_node(int nid, std::string comp);
        std::string get_compartment_for_node(int nid);

        void add_other_value1_to_node(int nid, int value);
        int get_other_value1_from_node(int nid);

        void set_node_excess(int nid, float value);
        float get_node_excess(int nid);

        void sort_nids_by_degree(vector<int> &nids);
        void get_nids_sorted_by_degree(map<int, vector<string>> &path_id_to_path_map, map<int, vector<int>> &path_id_to_eids_map, vector<int> &sorted_path_ids, map<int, int> &nid_to_degree_map_sorted, int num_bot_nodes, ofstream &fout);
        void get_eids_sorted_by_degree(map<int, vector<string>> &path_id_to_path_map, map<int, vector<int>> &path_id_to_eids_map, vector<int> &sorted_path_ids, map<int, int> &eid_to_degree_map_sorted, int num_bot_nodes, map<int, int> &eid_to_color_map);

        void get_nids_sorted_by_betweenness(map<int, float> &nid_to_betw_map);
        int get_betweenness_of_node(int nid, map<int, int> &nid_to_p_map, map<int, float> &nid_to_b_map, map<int, float> &nid_to_betw_map);
        void bfs_for_betweenness(int nid, map<int, int> &nid_to_p_map, map<int, float> &nid_to_betw_map);

        std::vector<std::string> get_all_pathways_for_edge(int eid);
        bool edge_belongs_to_pathway(int eid, std::string pathway); // checks if edge eid has a pathway
        void add_pathway_for_edge(int eid, std::string pathway);    // adds pathway for edge eid

        int get_source_node(int eid); // returns -1 if edge eid not present
        void add_source_node(int eid, int nid);

        int get_target_node(int eid); // returns -1 if edge eid not present
        void add_target_node(int eid, int nid);

        std::vector<int> get_inlist(int nid);
        bool node_has_edge_in_inlist(int nid, int eid);
        void add_edge_to_inlist_of_node(int nid, int eid);

        std::vector<int> get_outlist(int nid);
        bool node_has_edge_in_outlist(int nid, int eid);
        void add_edge_to_outlist_of_node(int nid, int eid);

        void add_other_values_in_edge(int eid, int value);
        std::vector<int> get_other_values_in_edge(int eid);
        void set_other_value1_in_edge(int eid, int value);
        int get_other_value1_in_edge(int eid);
        void set_other_value2_in_edge(int eid, int value);
        int get_other_value2_in_edge(int eid);
        void set_other_value3_in_edge(int eid, int value);
        int get_other_value3_in_edge(int eid);
        void set_other_value4_in_edge(int eid, int value);
        int get_other_value4_in_edge(int eid);
        void set_float_value1_in_edge(int eid, float value);
        float get_float_value1_in_edge(int eid);

        void set_edge_weight(int eid, int weight);
        int get_edge_weight(int eid);

        void add_to_weight_tuple_for_edge(int eid, float weight);
        void set_weight_tuple_for_edge(int eid, vector<float> tuple);
        std::vector<float> get_weight_tuple_for_edge(int eid);
        void set_weight_factor_at_tuple_index_for_edge(int eid, int index, float weight);
        float get_weight_factor_at_tuple_index_for_edge(int eid, int index);

        void set_edge_weight_float(int eid, float value);
        float get_edge_weight_float(int eid);
        void set_edge_capacity(int eid, float value);
        float get_edge_capacity(int eid);
        void set_edge_flow(int eid, float value);
        float get_edge_flow(int eid);

        int get_node_id_with_components(std::set<int> &comp_ids_set);
        std::string concatenate_sorted_rep_ids(std::set<int> &components, std::string delim);

        void preprocess_graph_for_mincut(vector<int> &node_ids, vector<int> &gomoryhu_parents);
        void preprocess_graph_for_node_edge_seq_num();

        void bfs(int curr_source, unordered_map<int, int> &bfs_tree);
        void get_bfs_nodes(int i, set<int> &reachable_nodes, int direction);
        void augment_bfs(int source_node, int target_node, unordered_map<int, int> &bfs_tree);
        list<int> get_path(int source, int target, unordered_map<int, int> bfs_tree);
        void mincut(int s, int t, unordered_map<int, int> &bfs_tree);

        void display_graph_silent(string color_map_filename, string vis_edges_filename, string dot_filename, GraphManagerNew *gm);
        void display_graph(string color_map_filename, GraphManagerNew *gm);
        void display_graph_richer(string color_map_filename, GraphManagerNew *gm, string dis_name);
        void display_graph_richer_with_edge_color(string node_color_map_filename, string edge_color_map_filename, map<int, int> &eid_to_color_map, string active_nodes_filename, string inactive_nodes_filename, string relaxed_nodes_filename, string relaxed_edges_filename, GraphManagerNew *gm, string dis_name);
        void write_graph_as_dot_richer_with_edge_color(string node_color_map_filename, string edge_color_map_filename, map<int, int> &eid_to_color_map, string active_nodes_filename, string inactive_nodes_filename, string relaxed_nodes_filename, string relaxed_edges_repids_filename, GraphManagerNew *gm, string dis_name);
        void display_graph_with_node_edge_colorfiles(GraphManagerNew *gm, string nodes_color_map_filename, string edges_color_map_filename, string file_prefix);
        void display_gomoryhu_tree(string color_map_filename, GraphManagerNew *gm);
        void display_graph_richer_ordering_nodes(string color_map_filename, GraphManagerNew *gm);
        void display_graph_richer_clustering_nodes(string color_map_filename, GraphManagerNew *gm);

        void group_paths_by_similarity(map<int, vector<string>> &path_id_to_path_map, vector<int> &sorted_path_ids, int similarity_pc, int num_of_colors, map<int, vector<int>> &group_id_to_path_ids_map);

        void perform_kegg_consistency_check();

        void write_node_to_richer_dot_file(ofstream &myfile, int nid, set<int> &drawn_nodes, color_triple_new color);
        // void write_to_xml_file(std::ofstream &myfile);
        void write_to_xml_file(std::ofstream &fout, bool b_mode, string p_name);

        void print_all_graph_info();
        std::set<std::string> get_all_target_rep_ids();
        std::set<std::string> get_all_gerel_target_rep_ids();
        void print_GErel_edges();
        void print_GErel_targets();
        void write_microarr_assert_to_file();
        void write_microarr_decl_to_file();
        void add_component_edges_for_complexes(GraphManagerNew *gm);
        void add_component_edges_for_complexes_sbml(GraphManagerNew *gm);
        // void remove_cycles_DFS(vector<string>& source_nodes_set);
        std::set<int> edges_to_remove_cycles(vector<string> &source_nodes_set);
        std::set<int> edges_to_remove_cycles_backwards(vector<string> &target_nodes_set);
        void get_edges_to_remove_cycles_DFS(int nid, set<int> &grey_nodes, set<int> &black_nodes, set<int> &edges_to_remove);
        void get_edges_to_remove_cycles_DFS_backwards(int nid, set<int> &grey_nodes, set<int> &black_nodes, set<int> &edges_to_remove);
        void remove_edge(int eid);
        t_Expression *get_DAG_reach_path_constraints(t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_reach_path_constraints(t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_edge_presence_implies_visible_constraints(t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_positive_distance_constrains_for_non_source_nodes(int source_nid, map<int, vector<t_Expression *>> &nid_distanc_var_map, t_ExpressionManager *em, GraphManagerNew *gm);
        std::vector<t_Expression *> get_node_distance_expression_sequence(int nid, t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_zero_distance_constraint_for_node(int source_nid, map<int, vector<t_Expression *>> &nid_distanc_var_map, t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_constraint_expr_visible_incoming_edge_to_diff_expr_node(set<int> &differentially_expressed_nids, t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_constraints_expr_visible_in_if_out_non_source_node(int source_nid, t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_constraints_distance_source_target_for_visible_edge(int source_nid, map<int, vector<t_Expression *>> &nid_distanc_var_map, t_Expression *expression_true_bit, t_ExpressionManager *em, GraphManagerNew *gm);
        t_Expression *get_constraint_node_distance_differ_by_one(vector<t_Expression *> &source_node_distance_expr_vec, vector<t_Expression *> &target_node_distance_expr_vec, t_Expression *expression_true_bit, t_ExpressionManager *em, GraphManagerNew *gm);
        void print_nodes_rids_with_no_in_edges(std::ostream &fout);
        bool is_isolated_node(int nid);

        void convert_to_graphml(std::ofstream &fout, bool b_mode, vector<string> &token_cmd, int &cmd_index);
        void convert_to_graphml_new(GraphManagerNew *graph_man, std::ofstream &fout, string log_fold_change_filename, bool b_mode, vector<string> &token_cmd, int &cmd_index);

        void convert_to_gml(std::ofstream &myfile);

        void convert_to_sif(GraphManagerNew *gm, bool b_mode, vector<string> &token_cmd, int &cmd_index);

        void get_nids_ranked_by_degree(vector<pair<int, int>> &vec_degree_to_nid);
        void get_nids_ranked_by_betweenness(vector<pair<int, float>> &vec_nid_to_betweenness);
};

class GraphManagerNew
{

public:
        std::map<std::string, std::string> all_id_map; // map of what all kegg hsaids map to the same rep id(Alok from ACTREC gives this input)
        std::map<std::string, std::string> kegg_hsa_id_to_display_name_map;
        std::map<std::string, std::string> pathway_name_map;
        set<string> merge_exception_set;
        std::string unique_prefix;
        void set_unique_prefix(std::string u_prefix);
        std::string get_unique_prefix();

private:
        // unique_prefix and get_unique_prefix need to be public probably.
        // std::string unique_prefix; // Added by Supratik to prevent race between
        // files with certain hard-coded names in the earlier version.
        // The race condition shows up in concurrent runs of the tool.
        std::map<std::string, std::string> identifier_map;

        std::map<std::string, std::list<std::string>> edge_type_subtype_map; // maps what all edge subtypes are possible for an edge type

        std::map<int, GraphNew *> graph_id_to_graph_map; // a bijection
        std::map<int, graph_genesis_new> graph_id_to_genesis_map;
        std::map<int, int> node_id_to_graph_id;
        std::map<int, int> edge_id_to_graph_id;

        // void set_unique_prefix(std::string u_prefix); // Added by Supratik
        // string get_unique_prefix();

        template <typename Key, typename Val>
        void add_to_map(std::map<Key, Val> &generic_map, Key k, Val v);

        template <typename Key, typename Val>
        void delete_from_map(std::map<Key, Val> &generic_map, Key k);

        template <typename Key, typename Val>
        void print_map(std::map<Key, Val> &generic_map, std::ofstream &fout);

        bool fill_ppin_from_file(GraphNew *graph_ptr, string input_file, GraphManagerNew *graph_man);
        bool fill_htri_from_file(GraphNew *graph_ptr, string input_file, GraphManagerNew *graph_man);
        bool fill_reactome_all_from_file(GraphNew *graph_ptr, string filename, GraphManagerNew *graph_man);

        GraphNew *merge_two_graphs(GraphNew *graph1, GraphNew *graph2, list<unordered_map<string, set<string>>> &map_list);
        GraphNew *merge_two_graphs_without_splitting_nodes(GraphNew *graph1, GraphNew *graph2, list<unordered_map<string, set<string>>> &map_list);

        template <typename Data>
        bool is_subset(std::vector<Data> smaller_set, std::vector<Data> bigger_set);
        template <typename Data>
        bool intersection_empty(std::vector<Data> left_set, std::vector<Data> right_set);

public:
        static int graph_id_count; //, node_id_count, edge_id_count;//initialize to zero
        GraphManagerNew();
        ~GraphManagerNew();

        GraphManagerNew(vector<string> &ip_file, int &cnt, bool b_mode);

        void read_edge_type_subtype_map();
        map<string, list<string>> get_edge_type_subtype_map();
        // void read_graph_xml_file_as_it_is(std::string input_file);
        void read_naming_map(string filename, unordered_map<string, set<string>> &mapname);
        void create_map_from_sbml(string sbmlfile);
        int read_graph_xml_file(std::string input_file, int thres, vector<string> &MAPK_merged_ids, map<string, set<string>> &MAPK_node_to_pathway_id_map, bool tool_written = false); // returns -1 if can't create graph, o/w graph id is returned
        int read_graph_from_sbml(string sbmlfile);
        int create_graph_PPIN(string input_file);
        int create_graph_HTRI(string input_file);
        int create_graph_reactome_all(string filename);
        int add_phenotype_edges(string filename);
        void destroy_graph(GraphNew *graph_ptr);

        GraphNew *get_graph(int gid); // returns null if not found
        void add_graph(int gid, GraphNew *g);
        GraphNew *duplicate_graph(int gid);
        int convert_to_undirected_graph(int gid);

        void add_graph_genesis(int gid, std::string oper, std::list<int> oper_graph_ids);
        void print_genesis_new(int gid, std::ostream &fout);

        void delete_graph_id_graph(int gid);
        int get_latest_graph_id();
        void fill_adj_matrix_for_graph(GraphNew *graph_ptr);
        bool get_edge_presence_status_from_adj_matrix(GraphNew *graph_ptr, int i, int j);

        int get_graph_id_for_node(int nid); // returns -1 if not found
        int get_graph_id_for_edge(int eid); // returns -1 if not found
        void add_node_id_graph_id(int nid, int gid);
        void delete_node_id_graph_id(int nid);
        void add_edge_id_graph_id(int eid, int gid);
        void delete_edge_id_graph_id(int eid);

        std::string get_rep_id_for_id(std::string id); // returns "" if id absent
        void add_rep_id_for_id(std::string id, std::string rep_id);
        std::vector<std::string> get_all_ids_for_rep_id(std::string rep_id);
        string get_display_names_of_all_ids_of_node(GraphNew *graph, int nid);

        std::string get_identifier(std::string key);
        std::string get_display_names_from_rep_id(std::string rep_id);
        std::string get_pathway_names_from_path_id(std::string path_id);
        // int check_if_node_already_created(GraphNew * new_graph, std::string rep_id);
        // int check_if_edge_already_created(GraphNew * new_graph, int new_source_nid, int new_target_nid, std::string edge_type, std::vector<std::string> edge_subtypes);//returne eid in new_graph if found, -1 if not found

        int duplicate_node(GraphNew *new_graph, GraphNew *old_graph, int nid, bool duplicate_check_flag = true);
        int duplicate_edge(GraphNew *new_graph, GraphNew *old_graph, int eid, bool duplicate_check_flag = true);
        int duplicate_node_without_splitting(GraphNew *new_graph, GraphNew *old_graph, int nid, bool duplicate_check_flag = true);
        int duplicate_edge_without_splitting_nodes(GraphNew *new_graph, GraphNew *old_graph, int eid, bool duplicate_check_flag = true);

        int merge_graphs(std::list<int> glist, list<unordered_map<string, set<string>>> &map_list, vector<string> &MAPK_merged_ids, map<string, set<string>> &MAPK_node_to_pathway_id_map);
        int merge_graphs_divide_and_conquer(std::vector<int> gid_vec); //

        void compute_min_cut_dummy(GraphNew *graph, int i, int parent_i, set<int> &cut_edges, set<int> &set_s);
        void compute_min_cut(GraphNew *graph, int i, int parent_i, set<int> &cut_edges, set<int> &set_s);

        int bcc_internal(GraphNew *graph, int source, vector<int> &discovery, vector<int> &low_values, list<int> &edges_list, set<int> &node_set, vector<int> &parent, int &count);
        int bcc(GraphNew *graph);
        void discharge(GraphNew *residual_graph, int N, list<int> &nodes_list, int i, int j, int curr_nid_in_res_graph, int curr_height, int curr_excess,
                       map<int, int> map_nseq_to_res_graph_nid, map<int, int> &nid_to_outlist_index, vector<int> &count_height, int &relabel_count,
                       int &sink_sat, int &sink_in, int &src_out, bool &need_exploration);
        int compute_gh_tree(int gid, vector<int> &non_isolated_node_ids, vector<int> &gomoryhu_parents, vector<set<int>> &cut_edges);
        void compute_min_cut_for_ghtree(GraphNew *original_graph, int i, int j, set<int> &cut_edges, set<int> &set_s, map<int, set<int>> &frwd_bfs_results, map<int, set<int>> &bkwd_bfs_results);
        void compute_min_cut_for_ghtree_relabel_to_front(GraphNew *original_graph, vector<int> &non_isolated_node_ids, int i, int j, set<int> &cut_edges, set<int> &set_s);
        void compute_weighted_min_cut_for_ghtree_relabel_to_front(GraphNew *original_graph, vector<int> &non_isolated_node_ids, int i, int j, set<int> &cut_edges, set<int> &set_s);
        void compute_weighted_min_cut_for_ghtree_relabel_to_front2(GraphNew *original_graph, vector<int> &non_isolated_node_ids, int i, int j, set<int> &cut_edges, set<int> &set_s);

        void get_nodes_on_simple_path(GraphNew *graph, int i, int j, set<int> &set_nodes_on_simple_path);
        void get_nodes_on_simple_path_internal(GraphNew *graph, int s, int t, int source, vector<int> &discovery, vector<int> &low_values, list<int> &edge_list, set<int> &node_set, set<int> &set_nodes_on_simple_path, vector<int> &parent);
        void get_nodes_on_path_from_i_to_j(GraphNew *original_graph, int i, int j, set<int> &result_nodes, map<int, set<int>> &frwd_bfs_results, map<int, set<int>> &bkwd_bfs_results);
        void get_nodes_on_path_from_i_to_j_dummy(GraphNew *original_graph, int i, int j, set<int> &result_nodes, map<int, set<int>> &frwd_bfs_results, map<int, set<int>> &bkwd_bfs_results);
        // void connect(GraphNew * graph, set<int>& present_edges, vector<vector< set <int> > >& minCutMatrix, int source_nid, int target_nid);
        void connect(GraphNew *graph, set<int> &present_edges, vector<set<int>> &cut_edges, map<pair<int, int>, int> &cut_edge_index, int source_nid, int target_nid);

        // void get_min_cut_edges_from_ghtree(GraphNew * graph, GraphNew * ugraph, vector<set<int> >& cut_edges, vector<int>& gomoryhu_parents, vector<bool>& selected_mincut_vec, int src_seq_num, int target_seq_num, set<int>& mincut_eids, map<int, int>& map_nid_to_bfs_index_frwd, map<int, int>& map_nid_to_bfs_index_bkwd);
        // void get_min_cut_edges_from_ghtree(GraphNew * graph, GraphNew * ugraph, vector<set<int> >& cut_edges, vector<int>& gomoryhu_parents, vector<bool>& selected_mincut_vec, int source_useq_num, int target_useq_num, set<int>& directed_cut_eids, map<int, bool>& map_nid_to_bfs_index_frwd, map<int, bool>& map_nid_to_bfs_index_bkwd);
        void get_min_cut_edges_from_ghtree(GraphNew *graph, GraphNew *ugraph, vector<vector<bool>> &closure_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, vector<bool> &selected_mincut_vec, int source_useq_num, int target_useq_num, set<int> &directed_cut_eids);
        void get_directed_cut(GraphNew *graph, int source_nid, int target_nid, set<int> &mincut_eids, set<int> &directed_cut_eids);
        void get_directed_cut2(GraphNew *graph, GraphNew *ugraph, int source_nid, int target_nid, set<int> &mincut_eids, set<int> &directed_cut_eids);
        void get_directed_cut3(GraphNew *graph, GraphNew *ugraph, int source_nid, int target_nid, set<int> &mincut_eids, set<int> &directed_cut_eids);
        void get_directed_cut4(GraphNew *graph, GraphNew *ugraph, int source_nid, int target_nid, vector<vector<bool>> &closure_matrix, set<int> &mincut_eids, set<int> &directed_cut_eids);

        void compute_transitive_closure(GraphNew *graph, vector<vector<bool>> &closure_matrix);

        void get_highest_weights_cuts(GraphNew *graph, GraphNew *ugraph, set<int> &mincut_eids, map<int, int> &nid_to_weight_map, set<int> &filtered_directed_cut_eids);
        void get_above_threshold_weights_cuts(GraphNew *graph, GraphNew *ugraph, set<int> &mincut_eids, map<int, float> &nid_to_expr_value_map, set<int> &filtered_directed_cut_eids, float threshold);
        void get_mincut_edges_with_up_reg_source(GraphNew *wgraph, GraphNew *ugraph, set<int> &directed_cut_eids, set<int> &up_reg_nids, set<int> &filtered_directed_cut_eids);
        bool bfs_excluding_edges(GraphNew *graph, int source_nid, int target_nid, set<int> &excluded_eids);
        void bfs_numbering(GraphNew *graph, int source_nid, map<int, int> &map_nid_to_bfs_index);
        void bfs_numbering2(GraphNew *graph, int source_nid, int direction, map<int, int> &map_nid_to_bfs_index);
        void bfs_numbering3(GraphNew *graph, int source_nid, int direction, map<int, int> &map_nid_to_bfs_index);
        void bfs_numbering4(GraphNew *graph, int source_nid, int direction, map<int, bool> &map_nid_to_reachable_status, map<int, set<int>> &nid_reach_thru_eids);
        void dfs_numbering(GraphNew *graph, int source_nid, map<int, int> &map_nid_to_dfs_index);
        void dfs_numbering4(GraphNew *graph, int source_nid, int direction, map<int, bool> &map_nid_to_reachable_status, map<int, set<int>> &map_nid_reachable_thru_eids_set);

        void connect_with_dfs_numbering(GraphNew *graph, set<int> &present_edges, vector<set<int>> &fwd_cut_edges, vector<set<int>> &bkd_cut_edges, vector<int> &gomoryhu_parents, int source_nid, int target_nid, map<int, int> &map_nid_to_dfs_index, ofstream &outfile);
        void disconnect(GraphNew *graph, GraphNew *ghtree, int src_nid, int tgt_nid, set<int> &set_S, set<int> &set_T, set<int> &set_W,
                        set<int> &present_edges, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents);
        void disconnect2(GraphNew *graph, GraphNew *ghtree, set<int> &set_S, set<int> &set_T, set<int> &present_edges, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents);

        // anjan
        int select_for_push_relabel(GraphNew *original_graph, GraphNew *residual_graph, int res_node_u, int delta, vector<list<int>> &nodeHeightLists, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, vector<bool> &is_outgoing_edges_explored, vector<vector<bool>> &edge_exist, int src, int target, int itr_no, int &push_node_count);
        int relabel_node(GraphNew *residual_graph, int res_node_u, vector<list<int>> &nodeHeightLists);
        int push_node(GraphNew *residual_graph, int res_node_u, int res_eid, int delta, vector<list<int>> &nodeHeightLists, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, int src, int target, int &push_node_count);
        int Initialize_preflow(GraphNew *residual_graph, int src_org_id, int tgt_org_id, map<int, int> &map_nseq_to_res_graph_nid);
        int mincut_excess_scaling(GraphNew *original_graph, int i, int j, int N, set<int> &cut_edges, set<int> &set_s, int max_cap_edge);
        void populate_set_s(GraphNew *graph, int start_node, set<int> &set_s);
        int mincut_ford_cbp(GraphNew *original_graph, int i, int j, vector<int> node_ids, set<int> &cut_edges, set<int> &set_s);
        void populate_map_src_tgt_to_eid(GraphNew *graph, map<string, long int> &map_src_tgt_to_eid, int start_node);
        void compute_Betweenness_centrality(GraphNew *original_graph, map<pair<int, int>, double> &CB_edge);
        int mincut_ford_extended(GraphNew *original_graph, GraphNew **res_graph, int i, int j, int N, set<int> &set_s, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, vector<bool> &path_flag, map<int, int> &eid_to_path_map);
        int mincut_ford(GraphNew *original_graph, int i, int j, int N, set<int> &cut_edges, set<int> &set_s);
        bool ford_BFS(GraphNew *original_graph, GraphNew *residual_graph, vector<bool> &is_outgoing_edges_explored, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, vector<vector<bool>> &edge_exist, vector<int> &q, int src, int dest, int itr_no, unordered_map<int, int> &pred, unordered_map<int, int> &pred_edge);
        void new_connect(GraphNew *graph, set<int> &present_edges, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, int source_nid, int target_nid, ofstream &outfile);

        int mincut_Dinics(GraphNew *original_graph, int i, int j, int N, set<int> &cut_edges, set<int> &set_s);
        bool Dinics_BFS(GraphNew *original_graph, GraphNew *residual_graph, vector<bool> &is_outgoing_edges_explored, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, vector<vector<bool>> &edge_exist, vector<int> &dist, vector<int> &q, int src, int dest, int itr_no);
        int Dinics_DFS(GraphNew *original_graph, GraphNew *residual_graph, vector<bool> &is_outgoing_edges_explored, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, vector<vector<bool>> &edge_exist, vector<int> &dist, vector<int> &work, int src, int target, int f, int itr_no);
        int create_node(int itr_no, int v, GraphNew *residual_graph, map<int, int> &map_nseq_to_res_graph_nid, int node_height, int node_excess);
        int get_node_from_src_tgt(int src, int tgt, map<string, long int> &map_src_tgt_to_eid);
        int get_outgoing_edges_explored(GraphNew *original_graph, GraphNew *residual_graph, map<int, int> &map_nseq_to_res_graph_nid, map<string, long int> &map_src_tgt_to_eid, vector<vector<bool>> &edge_exist, int org_node_id, int itr_no);
        long int create_edge(int itr_no, int curr_node_id, int succ_node_id, int eid_in_org_graph, map<string, long int> &map_src_tgt_to_eid, GraphNew *residual_graph, int capacity, string type);
        void reset_edge(map<string, long int> &map_src_tgt_to_eid, int src, int tgt, int itr_no, GraphNew *residual_graph);
        set<int> get_min_cut_edges(GraphNew *original_graph, GraphNew *residual_graph, map<int, int> &map_nseq_to_res_graph_nid, int i, int j, set<int> &cut_edges, set<int> &set_s, map<string, long int> &map_src_tgt_to_eid);
        bool bfs_without_excluded_edges(GraphNew *graph, int src_seqno, int dest_seqno, set<int> excluded_edge_set);
        set<int> getFilteredCutEdges(GraphNew *graph, set<int> &mincut_eids, int src_seqno, int dest_seqno);
        vector<string> tokenize_cmd(string cmd);
        // set<int> getMinCutEdges(GraphNew * graph, vector <set<int> >& cut_edges, vector<int>& gomoryhu_parents, int source_nid, int target_nid);
        void compute_min_cut_for_ghtree_relabel_to_front_2(GraphNew *original_graph, vector<int> &non_isolated_node_ids, int i, int j, set<int> &cut_edges, set<int> &fwd_cut_edges, set<int> &bkd_cut_edges, set<int> &set_s);
        set<int> getMinCutEdges_directed(GraphNew *graph, vector<set<int>> &fwd_cut_edges, vector<set<int>> &bkd_cut_edges, vector<int> &gomoryhu_parents, int source_nid, int target_nid, int &node_u_in_tree, int &is_src_side, ofstream &outfile);
        int compute_gh_tree_2(int gid, vector<int> &non_isolated_node_ids, vector<int> &gomoryhu_parents, vector<set<int>> &cut_edges, vector<set<int>> &fwd_cut_edges, vector<set<int>> &bkd_cut_edges);
        int compute_gh_tree_dinics(int gid, vector<int> &node_ids, vector<int> &gomoryhu_parents, vector<set<int>> &cut_edges, vector<set<int>> &fwd_cut_edges, vector<set<int>> &bkd_cut_edges);
        set<int> my_getMinCutEdges(GraphNew *graph, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, int source_nid, int target_nid);
        void print_all_mincuts_src_to_tgt(GraphNew *graph, int count, map<int, int> &map_count, int nid, vector<set<int>> &cum_min_cut_edges, int node_index, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, ofstream &outfile);

        set<int> getMinCutEdges(GraphNew *graph, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, int source_nid, int target_nid, int &node_u_in_tree, int &is_src_side, ofstream &outfile);
        int is_in_src_side(int chosen_edge_src, int node_u_in_tree, vector<int> &gomoryhu_parents);

        void get_hsa_from_name(string filename);

        void read_pairs_file(GraphNew *graph, string pairs_filename, vector<pair<int, int>> &pairs, set<int> &nids_as_source, set<int> &nids_as_target);
        void read_data_file_into_map(GraphNew *graph, string up_reg_filename, string down_reg_filename, unordered_map<string, bool> &umap_data);
        void read_data_file_into_set_of_ints(GraphNew *graph, string filename, set<int> &nids);
        void read_data_file_into_set_of_strings(GraphNew *graph, string filename, set<string> &repids);
        void read_edge_data_file_into_set_of_ints(GraphNew *graph, string edges_filename, set<int> &eids_matched);

        void get_fold_change_from_log_values(string log_value_filename, string output_filename);
        void read_fold_change_values_into_map(GraphNew *graph, map<int, float> &nid_to_weight_map, string fold_change_filename, int offset);

        int get_data_consistent_subgraph(GraphNew *graph, string up_reg_filename, string down_reg_filename, bool create_subgraph_flag = true);
        void get_data_overlap(GraphNew *graph, string up_reg_filename, string down_reg_filename);

        int put_weight_on_graph_edges(GraphNew *graph, vector<string> &source_nodes_set, vector<string> &target_nodes_set, string fold_change_filename, vector<float> &vec_coeff, vector<float> &vec_order);
        void assign_weight_by_inverse_of_distance(int index, GraphNew *graph, GraphNew *wgraph, vector<string> &source_nodes_set, int direction);
        void assign_weight_by_expression_values(int index_src, int index_tgt, GraphNew *graph, GraphNew *wgraph, string fold_change_filename);

        int bounded_reach(int gid, int direction, std::vector<std::string> &source_nodes_set, std::vector<std::string> &excluded_nodes_set, int bound);

        int bounded_forward_backward_reach(int gid, set<string> &source_nodes_set, set<string> &target_nodes_set, set<string> &excluded_nodes_set, int bound, int target_act_expr_flag);

        void print_all_paths(int gid, set<string> &source_nodes_set, set<string> &target_nodes_set, map<int, vector<string>> &path_id_to_path_map, map<int, vector<int>> &path_id_to_eids_map, string out_filename);
        void print_all_paths_from_source(GraphNew *graph, int curr_id, set<int> &target_node_nids, map<int, vector<string>> &path_id_to_path_map, map<int, vector<int>> &path_id_to_eids_map, ofstream &fout);
        void print_all_paths_from_source_recursively(GraphNew *graph, int source_nid, string types, set<int> &target_node_nids, map<int, bool> &visited, vector<string> path, vector<string> path_with_repids, map<int, vector<string>> &path_id_to_path_map, vector<int> &path_with_eids, map<int, vector<int>> &path_id_to_eids_map, ofstream &fout);
        void print_curr_path(GraphNew *graph, vector<string> &path, vector<int> &eids, map<int, bool> &visited, set<int> &target_node_nids, ofstream &fout);
        void print_all_paths_rank_ordered(GraphNew *graph, set<int> &nodes_nids_for_rank_ordering, map<int, float> &nid_to_expr_value_map, map<int, vector<string>> &path_id_to_path_map, vector<int> &path_ids_sorted_by_size, vector<int> &path_ids_sorted_by_enrchmnt, map<int, vector<int>> &path_id_to_eids_map, string ranked_paths_out_filename);

        int select_operation_on_graph(int gid, set<string> &nodes_set);
        int select_subgraph(int gid, set<string> &nodes_set);
        int get_subgraph_with_edge_ids(GraphNew *graph, set<int> present_edges);
        int get_subgraph_edges_removed(int gid, set<int> &eids_to_remove);
        void get_common_nodes_from_graphs(set<int> &graph_set, map<string, set<int>> &rep_id_to_gid_set_map);
        void get_common_edges_from_graphs(set<int> &graph_set, map<string, set<int>> &edge_to_gid_set_map);
        void get_common_nodes_from_merged_graph(GraphNew *merged_graph, set<string> &source_nodes_set, set<string> &target_nodes_set, map<string, set<int>> &node_to_gid_set_map, int relax_cutoff);
        void get_subgraph_from_common_edges(GraphNew *merged_graph, set<string> &source_nodes_set, set<string> &target_nodes_set, map<string, set<int>> &edge_to_gid_set_map, int relax_cutoff);

        void print_all_graph_manager_info();
        void generate_constraints_using_rules_parser(int gid, std::list<rule_t *> &list_of_rules, t_ExpressionManager *em);
        t_Expression *get_constraints_for_rule(rule_t *rule_ptr, GraphNew *graph_ptr, t_ExpressionManager *em);
        set<int> find_set_of_nodes_for_tuple(std::string &var_name, GraphNew *graph, set_nodes_t *set_nodes_parser, std::map<std::string, int> &tuple_ref);
        std::string get_node_type_str_from_node_type_parser(node_t node_type);
        std::string get_edge_type_str_from_edge_type_parser(edge_t edge_type);
        std::string get_edge_subtype_str_from_edge_subtype_parser(edge_subt edge_subtype);
        std::vector<std::string> get_vector_edge_subtype_str_from_edge_subtype_parser_list(list_edge_subtypes_t *list_edge_subtypes);
        std::vector<std::string> get_non_negated_edge_subtypes(ANDed_edgesub_literals_t *ANDed_clause);
        std::vector<std::string> get_negated_edge_subtypes(ANDed_edgesub_literals_t *ANDed_clause);
        void get_source_nodes(std::set<int> &source_node_array, int target_node_id, edge_t edge_type, GraphNew *graph_ptr);
        void get_source_nodes(std::set<int> &source_node_array, int target_node_id, edge_subt edge_subtype, GraphNew *graph_ptr);
        void get_source_nodes(set<int> &set_nodes_to_return, int target_node_id, edge_t edge_type, list_edge_subtypes_t *edge_sub_list, GraphNew *graph_ptr);
        void get_target_nodes(std::set<int> &target_node_array, int source_node_id, edge_t edge_type, GraphNew *graph_ptr);
        void get_target_nodes(std::set<int> &target_node_array, int source_node_id, edge_subt edge_subtype, GraphNew *graph_ptr);
        void get_target_nodes(set<int> &target_node_array, int source_node_id, edge_t edge_type, list_edge_subtypes_t *edge_sub_list, GraphNew *graph_ptr);
        bool node_has_input_edge_of_type(int nid, edge_t edge_type, GraphNew *graph);
        bool node_has_output_edge_of_type(int nid, edge_t edge_type, GraphNew *graph);
        bool node_has_input_edge_of_subtype(int nid, edge_subt edge_subtype, GraphNew *graph);
        bool node_has_output_edge_of_subtype(int nid, edge_subt edge_subtype, GraphNew *graph);
        t_Expression *get_constraint_for_tuple(std::map<std::string, int> &curr_tuple_ref, logical_stmt_t *left_of_implication, logical_stmt_t *right_of_implication, t_ExpressionManager *em, GraphNew *graph);
        t_Expression *get_constraint_for_logic_stmt_and_tuple(map<string, int> &curr_tuple_ref, logical_stmt_t *logic_stmt, t_ExpressionManager *em, GraphNew *graph);
        t_Expression *get_node_constraint(int nid, node_state_t node_state, t_ExpressionManager *em, GraphNew *graph);
        t_Expression *get_node_constraint(int nid, node_state2_t node2_state, t_ExpressionManager *em, GraphNew *graph);
        t_Expression *get_edge_constraint(int node1, int node2, edge_state_t edge_state, edge_t edge_type, list_edge_subtypes_t *list_edge_subtypes, t_ExpressionManager *em, GraphNew *graph);
        t_Expression *get_edge_constraint(int node1, int node2, edge_state_t edge_state, edge_t edge_type, DNF_edgesub_t *DNF_edgesub, t_ExpressionManager *em, GraphNew *graph);
        bool edge_type_subtype_subset_match(int eid, edge_t edge_type, list_edge_subtypes_t *list_edge_subtypes, GraphNew *graph);
        bool edge_subtype_ANDed_clause_match(int eid, ANDed_edgesub_literals_t *ANDed_clause, GraphNew *graph);
        std::string get_edge_substring_for_constraint_variable(std::vector<std::string> &edge_subtype_vec_str);
        std::string get_string_prefix(std::string input, char delim);
        vector<t_Expression *> get_microarray_constraints(std::string micro_arr_decl, std::string micro_arr_assert, t_ExpressionManager *em);
        void solve_constraints(vector<t_Expression *> cons, t_ExpressionManager *em);
        vector<int> write_edgecount_cnf(std::string CNF_filename, std::map<t_Expression *, std::pair<std::string, int>> getCNFIndexMap, int &idx, int &clauses);
        GraphNew *get_subgraph_with_edges_retained(GraphNew *graph, std::string filename);
        list<string> *get_list_of_names_from_string(string name_str, string delim);
        void merge_vis_files(string file_assign1, string file_assgn2, std::ofstream &fout);
        void print_visible_but_not_present_edges_from_z3_assignment(string filename, std::ofstream &fout);
        void testing_printexpr_iter(t_ExpressionManager *em);

        // constraints for connectivity using mincut
        void generate_connectivity_constraints(int graph_gid, int ghtree_gid, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, vector<pair<int, int>> &connect_pairs_to_consider, vector<pair<int, int>> &disconnect_pairs, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &essential_nids, set<int> &nids_as_source, set<int> &nids_as_target);
        void get_connection_constraints(GraphNew *graph, GraphNew *ugraph, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, list<pair<int, int>> &connect_pairs_to_consider, set<int> &eids_to_consider, set<string> &all_vars, ofstream &smt, ofstream &smt1);
        // void get_connection_implies_end_nodes_constraints(GraphNew * graph, list<pair<int, int> >& connect_pairs_to_consider, set<int>& nids_to_consider, set<string>& all_vars, ofstream& smt);
        void get_node_implies_inedge_outedge_constraints(GraphNew *graph, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &nids_as_source, set<int> &nids_as_target, set<string> &all_vars, ofstream &smt);
        void get_edge_implies_connection_constraints(GraphNew *graph, set<int> &eids_to_consider, list<pair<int, int>> &connect_pairs_to_consider, set<string> &all_vars, ofstream &smt, set<string> &all_int_vars, ofstream &order);
        void get_node_ordering_constraints(GraphNew *graph, set<int> &nids_as_source, set<string> &all_vars, set<string> &all_int_vars, ofstream &smt);
        void get_node_positive_distance_constraints(GraphNew *graph, set<int> &nids_to_consider, set<int> &nids_as_source, set<string> &all_vars, set<string> &all_int_vars, ofstream &smt);
        void write_var_declarations(set<string> &all_vars, set<string> &all_int_vars, set<int> &up_reg_nids, set<int> &down_reg_nids, ofstream &decl_fout);
        void write_data_assertions(set<int> &up_reg_nids, set<int> &down_reg_nids, set<string> &all_vars, ofstream &data_fout);
        void write_query_assertions(vector<pair<int, int>> &connect_pairs, vector<pair<int, int>> &disconnect_pairs, set<int> &essential_nids, ofstream &assert_fout);
        string process_smt_answer(GraphNew *graph, string smt_out_filename);
        void process_smt_output(GraphNew *graph, string file_prefix, string smt_out_filename, set<int> &up_reg_nids, map<int, bool> &nid_to_value_pairs, map<int, bool> &eid_to_value_pairs, string solver, bool counting_solns = false);

        // constraints for connectivity using mincut with ExpressionManager
        void get_connectivity(int graph_gid, int ugraph_gid, vector<vector<bool>> &closure_matrix, vector<vector<int>> &call_level_matrix, vector<vector<int>> &edge_level_matrix, vector<vector<int>> &call_count_matrix, vector<vector<int>> &edge_count_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, vector<pair<int, int>> &connect_pairs, vector<pair<int, int>> &disconnect_pairs, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &essential_nids, set<int> &avoid_nids, set<int> &nids_as_source, set<int> &nids_as_target, int path_length_bound, string fold_change_filename, string file_prefix, t_ExpressionManager *em);
        void generate_connectivity_constraints(GraphNew *graph, GraphNew *ugraph, vector<vector<bool>> &closure_matrix, vector<vector<int>> &call_level_matrix, vector<vector<int>> &edge_level_matrix, vector<vector<int>> &call_count_matrix, vector<vector<int>> &edge_count_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, vector<pair<int, int>> &connect_pairs, vector<pair<int, int>> &disconnect_pairs, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &essential_nids, set<int> &avoid_nids, set<int> &nids_as_source, set<int> &nids_as_target, int path_length_bound, string fold_change_filename, string file_prefix, t_ExpressionManager *em);
        void get_connection_constraints_using_mincut(t_ExpressionManager *em, vector<t_Expression *> &connection_constraints_vec, vector<t_Expression *> &connection_implies_end_nodes, GraphNew *graph, GraphNew *ugraph, vector<vector<bool>> &closure_matrix, vector<vector<int>> &call_level_matrix, vector<vector<int>> &edge_level_matrix, vector<vector<int>> &call_count_matrix, vector<vector<int>> &edge_count_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, list<pair<int, int>> &connect_pairs_to_consider, set<int> &eids_to_consider, set<int> &nids_to_consider, set<int> &up_reg_nids, set<int> &down_reg_nids, set<string> &up_reg_rep_ids, set<string> &down_reg_rep_ids, string fold_change_filename, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints, map<string, int> &nodes_in_all_mincuts, ofstream &hout);
        void get_connection_implies_end_nodes_constraints(t_ExpressionManager *em, vector<t_Expression *> &connection_implies_end_nodes, string connect_var, string src_node_var, string tgt_node_var);
        void get_edge_implies_connection_constraints(t_ExpressionManager *em, int path_length_bound, vector<t_Expression *> &edge_implies_connection_constraints_vec, vector<t_Expression *> &src_dist_lt_tgt_dist_constraints_vec, GraphNew *graph, set<int> &nids_to_consider, set<int> &eids_to_consider, list<pair<int, int>> &connect_pairs_to_consider, set<int> &all_nids_in_constraints);
        void get_node_implies_inedge_outedge_constraints(t_ExpressionManager *em, vector<t_Expression *> &node_implies_inedge_outedge_constraints_vec, GraphNew *graph, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &nids_as_source, set<int> &nids_as_target, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_source_has_outedge_constraints(t_ExpressionManager *em, vector<t_Expression *> &node_implies_inedge_outedge_constraints_vec, GraphNew *graph, set<int> &nids_as_source, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_target_has_inedge_constraints(t_ExpressionManager *em, vector<t_Expression *> &node_implies_inedge_outedge_constraints_vec, GraphNew *graph, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_as_target, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_node_has_inedge_outedge_constraints(t_ExpressionManager *em, vector<t_Expression *> &node_implies_inedge_outedge_constraints_vec, GraphNew *graph, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &remaining_nodes_to_consider, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_all_nodes_dist_lt_bound_constraints(t_ExpressionManager *em, vector<t_Expression *> &node_implies_inedge_outedge_constraints_vec, GraphNew *graph, set<int> &nids_to_consider);
        void get_src_dist_lt_tgt_dist_constraints(t_ExpressionManager *em, int path_length_bound, vector<t_Expression *> &src_dist_lt_tgt_dist_constraints_vec, GraphNew *graph, int curr_eid);
        void get_targets_dist_lt_bound_constraints(t_ExpressionManager *em, int path_length_bound, vector<t_Expression *> &targets_dist_lt_bound_constraints_vec, GraphNew *graph, set<int> &nids_as_target);
        void get_microarray_data_as_constraints(t_ExpressionManager *em, GraphNew *graph, vector<t_Expression *> &microarray_data_assertions_vec, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_to_consider, set<int> &nids_as_target, set<int> &all_nids_in_constraints);
        void get_query_assertions_as_constraints(t_ExpressionManager *em, vector<t_Expression *> &query_assertions_as_constraints_vec, vector<pair<int, int>> &connect_pairs, vector<pair<int, int>> &disconnect_pairs, set<int> &essential_nids, set<int> &avoid_nids, set<int> &nids_to_consider, set<int> &all_nids_in_constraints);
        void get_total_nodes_lt_bound_constraints(t_ExpressionManager *em, vector<t_Expression *> &total_nodes_lt_bound_constraints_vec, GraphNew *graph, set<int> &all_nids_in_constraints);
        void get_total_edges_lt_bound_constraints(t_ExpressionManager *em, vector<t_Expression *> &total_edges_lt_bound_constraints_vec, GraphNew *graph, set<int> &all_eids_in_constraints);
        void get_assertion_expressions(t_ExpressionManager *em, vector<pair<string, int>> &var_value_pairs, vector<t_Expression *> &assertion_expression_vec, ofstream &fout);
        void get_expression_edges_assertion_constraints(t_ExpressionManager *em, vector<int> &akt_expr_inedges, vector<t_Expression *> &expression_edges_assert_vec);
        void get_negated_solution_expression(t_ExpressionManager *em, map<string, bool> &var_value_pairs, t_Expression *negated_soln_expression, string neg_soln_prefix);
        void get_negated_solution_expression1(t_ExpressionManager *em, map<int, bool> &nid_to_value_pairs, t_Expression *negated_soln_expression, string file_prefix, ofstream &neg_fout);
        void get_edge_type_consistency_constraints(t_ExpressionManager *em, vector<t_Expression *> &negated_edges_contraints_vec, GraphNew *graph, set<int> &all_eids_in_constraints, set<int> &up_reg_nids, set<int> &down_reg_nids);
        void get_node_activation_status_relaxation_constraints(t_ExpressionManager *em, vector<t_Expression *> &activation_edges_constraints_vec, vector<t_Expression *> &all_nodes_relax_expr_vec, GraphNew *graph, set<int> &all_nids_in_constraints, set<int> &up_reg_nids, set<int> &down_reg_nids);
        void get_edge_activation_status_relaxation_constraints(t_ExpressionManager *em, vector<t_Expression *> &edge_activation_status_relaxation_constraints_vec, vector<t_Expression *> &all_edges_relax_expr_vec, GraphNew *graph, set<int> &all_eids_in_constraints, set<int> &up_reg_nids, set<int> &down_reg_nids);

        void get_every_path_has_diff_exp_node_constraints(t_ExpressionManager *em, vector<t_Expression *> &every_path_has_diff_exp_node_constraints_vec, GraphNew *graph, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_as_source, set<int> &nids_as_target);

        t_Expression *get_expr_var_lt_const(t_ExpressionManager *em, string var_name, int var_value);

        t_Expression *get_AND_expr_from_vector(t_ExpressionManager *em, vector<t_Expression *> &vec);
        t_Expression *get_OR_expr_from_vector(t_ExpressionManager *em, vector<t_Expression *> &vec);

        void count_smt_solutions(t_ExpressionManager *em, GraphNew *graph, string file_prefix, int soln_bound);

        void solve_mincut_constraints(t_ExpressionManager *em, GraphNew *graph, string file_prefix, int num_of_nodes_bound, int num_of_edges_bound, int path_length_bound, int node_status_relax_bound, int edge_status_relax_bound, int choice, int up_reg_down_reg_size, string smt_solver);
        void binary_search_for_exact_bound(t_ExpressionManager *em, GraphNew *graph, string file_prefix, int num_of_nodes_bound, int num_of_edges_bound, int path_length_bound, int node_status_relax_bound, int edge_status_relax_bound, int choice, int up_reg_down_reg_size, string smt_solver);

        // constraints for connectivity using mincut with Z3 C++ API
        void create_bool_expressions_one_id_with_z3(z3::context &c, vector<int> &ids, string var_prefix, map<string, z3::expr> &var_to_expr_map);
        void create_bool_expressions_two_ids_with_z3(z3::context &c, vector<pair<int, int>> &id_pairs, string var_prefix, string delimiter, map<string, z3::expr> &var_to_expr_map);
        void create_bv_expressions_one_id_with_z3(z3::context &c, vector<int> &ids, string var_prefix, int width, map<string, z3::expr> &var_to_expr_map);
        void create_bv_expression_single_var_with_z3(z3::context &c, string var, int width, map<string, z3::expr> &var_to_expr_map);
        void get_connectivity_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, int graph_gid, int ugraph_gid, vector<vector<bool>> &closure_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, vector<vector<int>> &call_level_matrix, vector<vector<int>> &edge_level_matrix, vector<vector<int>> &call_count_matrix, vector<vector<int>> &edge_count_matrix, vector<pair<int, int>> &connect_pairs, int path_bound, set<int> &nids_as_source, set<int> &nids_as_target, set<int> &up_reg_nids_to_use, set<int> &down_reg_nids_to_use, set<int> &essential_nids, set<int> &avoid_nids, set<int> &essential_eids, set<int> &avoid_eids, set<int> &active_nids, set<int> &inactive_nids, set<int> &confirmed_up_reg_nids, set<int> &confirmed_down_reg_nids, set<int> &relaxed_nids, set<int> &nonrelaxed_nids, set<int> &relaxed_eids, set<int> &nonrelaxed_eids, string fold_change_filename, string file_prefix);
        void generate_connectivity_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, GraphNew *ugraph, vector<vector<bool>> &closure_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, vector<vector<int>> &call_level_matrix, vector<vector<int>> &edge_level_matrix, vector<vector<int>> &call_count_matrix, vector<vector<int>> &edge_count_matrix, vector<pair<int, int>> &connect_pairs, int path_bound, set<int> &nids_as_source, set<int> &nids_as_target, set<int> &up_reg_nids_to_use, set<int> &down_reg_nids_to_use, set<int> &essential_nids, set<int> &avoid_nids, set<int> &essential_eids, set<int> &avoid_eids, set<int> &active_nids, set<int> &inactive_nids, set<int> &confirmed_up_reg_nids, set<int> &confirmed_down_reg_nids, set<int> &relaxed_nids, set<int> &nonrelaxed_nids, set<int> &relaxed_eids, set<int> &nonrelaxed_eids, string fold_change_filename, string file_prefix);
        void get_query_assertions_as_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, vector<pair<int, int>> &connect_pairs, set<int> &up_reg_nids_to_use, set<int> &down_reg_nids_to_use, set<int> &essential_nids, set<int> &avoid_nids, set<int> &essential_eids, set<int> &avoid_eids, set<int> &active_nids, set<int> &inactive_nids, set<int> &confirmed_up_reg_nids, set<int> &confirmed_down_reg_nids, set<int> &relaxed_nids, set<int> &nonrelaxed_nids, set<int> &relaxed_eids, set<int> &nonrelaxed_eids, set<int> &nids_to_consider, set<int> &all_nids_in_constraints);
        void get_connection_constraints_using_mincut_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, GraphNew *ugraph, vector<vector<bool>> &closure_matrix, vector<vector<int>> &call_level_matrix, vector<vector<int>> &edge_level_matrix, vector<vector<int>> &call_count_matrix, vector<vector<int>> &edge_count_matrix, vector<set<int>> &cut_edges, vector<int> &gomoryhu_parents, list<pair<int, int>> &connect_pairs_to_consider, set<int> &up_reg_nids_to_use, set<int> &down_reg_nids_to_use, set<int> &essential_nids, set<int> &avoid_nids, set<int> &active_nids, set<int> &inactive_nids, set<int> &confirmed_up_reg_nids, set<int> &confirmed_down_reg_nids, set<int> &relaxed_nids, set<int> &nonrelaxed_nids, set<int> &relaxed_eids, set<int> &nonrelaxed_eids, set<int> &nids_to_consider, set<int> &eids_to_consider, set<string> &up_reg_rep_ids, set<string> &down_reg_rep_ids, string fold_change_filename, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints, map<string, int> &nodes_in_all_mincuts, ofstream &hout);
        void get_edge_implies_connection_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &nids_to_consider, set<int> &eids_to_consider, list<pair<int, int>> &connect_pairs_to_consider, set<int> &all_nids_in_constraints);
        void get_targets_dist_lt_bound_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &nids_as_target, int path_bound);
        void get_node_implies_inedge_outedge_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &nids_as_source, set<int> &nids_as_target, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_total_nodes_lt_bound_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &nids_to_consider);
        void get_total_edges_lt_bound_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &eids_to_consider);
        void get_node_activation_status_relaxation_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &nids_to_consider, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_as_source, set<int> &nids_as_target);
        void get_every_path_has_diff_exp_node_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_as_source, set<int> &nids_as_target);
        void get_edge_activation_status_relaxation_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &eids_to_consider, set<int> &up_reg_nids, set<int> &down_reg_nids);
        void get_connection_implies_end_nodes_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, string connect_var, string src_node_var, string tgt_node_var);
        void get_src_dist_lt_tgt_dist_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, int curr_eid);
        void get_source_has_outedge_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &nids_as_source, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_target_has_inedge_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &nids_as_target, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        void get_node_has_inedge_outedge_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &up_reg_nids, set<int> &down_reg_nids, set<int> &remaining_nodes_to_consider, set<int> &nids_to_consider, set<int> &eids_to_consider, set<int> &all_nids_in_constraints, set<int> &all_eids_in_constraints);
        ////////        void get_all_nodes_dist_lt_bound_constraints_with_z3(t_ExpressionManager * em, vector<t_Expression *>& node_implies_inedge_outedge_constraints_vec, GraphNew * graph, set<int>& nids_to_consider);

        //////
        //////        void get_microarray_data_as_constraints_with_z3(Z3Solver * zs, z3::context& c, z3::solver& s, map<string, z3::expr>& all_z3_var_to_expr_map, GraphNew * graph, set<int>& up_reg_nids, set<int>& down_reg_nids, set<int>& nids_to_consider, set<int>& nids_as_target, set<int>& all_nids_in_constraints);
        //////
        //////
        //////
        //////        void get_assertion_expressions_with_z3(Z3Solver * zs, z3::context& c, z3::solver& s, map<string, z3::expr>& all_z3_var_to_expr_map, vector<pair<string, int> >& var_value_pairs);
        void get_edge_type_consistency_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, set<int> &all_eids_in_constraints);
        //////
        //////        void count_smt_solutions_with_z3(bool b_mode, z3::context& c, z3::solver& s, map<string, z3::expr>& all_z3_var_to_expr_map, string file_prefix, GraphNew * graph, int soln_bound);
        void solve_mincut_constraints_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, string file_prefix, int binary_search_nodes_bound);
        int binary_search_for_exact_bound_with_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, ofstream &fout, int edge_relax_bound, int edge_relax_bound_lower_limit, int edge_relax_bound_upper_limit, bool obtained_first_PO_point);
        void get_PO_curve(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, int node_relax_bound_lower_limit, int node_relax_bound_upper_limit, int edge_relax_bound_lower_limit, int edge_relax_bound_upper_limit, list<pair<int, int>> &node_edge_pairs, int up_reg_down_reg_size);
        void get_solution_z3(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, list<pair<int, int>> &node_edge_pairs, int up_reg_down_reg_size);

        void get_PO_curve_within_limits(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, int edge_bound_upper_limit, int node_bound_lower_limit, int edge_bound_lower_limit, int node_bound_upper_limit, list<pair<int, int>> &PO_value_pairs, ofstream &fout);
        void get_PO_points_within_limits(z3::context &c, z3::solver &s, map<string, z3::expr> &all_z3_var_to_expr_map, GraphNew *graph, int y, int z, int edge_bound_upper_limit, int node_bound_lower_limit, int edge_bound_lower_limit, int node_bound_upper_limit, vector<pair<int, int>> &PO_value_pairs, ofstream &fout, int call_type, int up_reg_down_reg_size); //, vector<vector <sat_state_t> >& sat_state_matrix);
        bool is_sat_at_existing_PO_points(z3::context &c, z3::solver &s, map<string, z3::expr> &expr_map, GraphNew *graph, vector<pair<int, int>> &PO_value_pairs, int up_reg_down_reg_size);

        bool recursive_DFS_to_detect_backedges(GraphNew *graph, int s, set<int> &visited_nids, map<int, bool> &recursion_stack, set<int> &back_edges);
        void DFS_to_detect_backedges(GraphNew *graph, int s, set<int> &back_edges);
        int compute_unrolled_DAG(int graphnum, int source_nid, int target_nid, int unrolling_levels, std::map<int, std::vector<int>> &map_nid_to_unrolled_graph_nids, std::map<int, std::vector<int>> &map_eid_to_unrolled_graph_eids);
        void count_paths_through_vertex(GraphNew *graph, int source_nid, int target_nid, int path_length, vector<vector<int>> &PathsVertices, vector<vector<int>> &PathsEdges);
        void recursive_topological_sort(GraphNew *graph, int curr_nid, set<int> &visited, list<int> &stack, int direction);
        void topologically_sort_nids_using_dfs(GraphNew *graph, int source_nid, vector<int> &topologically_sorted_nids, int direction);
        void topologically_sort_nids_using_bfs(GraphNew *graph, int source_nid, vector<int> &topologically_sorted_nids, int direction);
        void print_nodes_ranked_by_fraction_of_paths(string file_prefix, GraphNew *original_graph, GraphNew *unrolled_graph, int source_nid, int target_nid, int path_length, int &total_paths, vector<vector<int>> &PathsVertices, vector<vector<int>> &PathsEdges, std::map<int, std::vector<int>> &map_nid_to_unrolled_graph_nids);
        void print_edges_ranked_by_fraction_of_paths(string file_prefix, GraphNew *original_graph, GraphNew *unrolled_graph, int source_nid, int target_nid, int path_length, int &total_paths, vector<vector<int>> &PathsVertices, vector<vector<int>> &PathsEdges, std::map<int, std::vector<int>> &map_eid_to_unrolled_graph_eids);

        void print_nodes_ranked_by_betweenness(GraphNew *graph, string betweenness_filename);
        void print_nodes_ranked_by_degree(GraphNew *graph, string degree_filename);

        void process_z3_model(GraphNew *graph, map<string, z3::expr> &var_to_expr_map, z3::model &m, vector<int> &present_node_nids, vector<int> &present_edge_eids, vector<int> &active_node_nids, vector<int> &relaxed_node_nids, vector<int> &relaxed_edge_eids, z3::expr &soln_clause);
        void print_variables_from_z3_model(GraphNew *graph, set<int> &up_reg_nids_to_use, set<int> &down_reg_nids_to_use, vector<int> &present_node_nids, vector<int> &present_edge_eids, vector<int> &active_node_nids, vector<int> &relaxed_node_nids, vector<int> &relaxed_edge_eids, ofstream &soln_out, ofstream &relax_out);
};

#endif
