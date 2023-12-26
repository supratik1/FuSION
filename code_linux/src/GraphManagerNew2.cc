#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<queue>
#include<algorithm>
#include<limits>
#include <unordered_map>

#include "../include/GraphManagerNew.h"
#include "../include/XMLParser.h"
#include "../include/macros.h"
#include "../include/rules_parser_datatypes.h"
#include "../include/rules_parser.tab.h"
#include "../include/rule_tuple_store.h"

int GraphManagerNew::graph_id_count = 0;
//int GraphManagerNew::node_id_count = 0;
//int GraphManagerNew::edge_id_count = 0;
int NodeNew::node_id_counter = 0;
int EdgeNew::edge_id_counter = 0;

extern ofstream debug_log;
extern std::string concatenate_strings(std::vector<std::string> vec_strings, std::string delim);
extern list<string> split_string_into_list(string name_str, string delim);

NodeNew::NodeNew() {
    node_id = ++node_id_counter;
}

inline int NodeNew::get_node_id() {
    return node_id;
}

inline void NodeNew::set_rep_id(std::string repid) {
    rep_id = repid;
}

inline std::string NodeNew::get_rep_id() {
    return rep_id;
}

inline std::vector<std::string> NodeNew::get_all_id() {
    return all_id;
}

inline void NodeNew::set_all_id(std::vector<std::string> all_ids) {
    all_id = all_ids;
}

inline void NodeNew::add_to_all_id(std::string id) {
    all_id.push_back(id);
}

inline std::vector<int> NodeNew::get_comp_ids() {
    return components;
}

inline void NodeNew::add_comp_id(int id) {
    components.push_back(id);
}

inline void NodeNew::update_component_id_at_index(int pos, int value) {
    components[pos] = value;
}

inline void NodeNew::set_all_display_names(std::vector<std::string> all_disp_names) {
    display_names = all_disp_names;
}

inline std::vector<std::string> NodeNew::get_disp_ids() {
    return display_names;
}

inline void NodeNew::add_disp_id(std::string id) {
    display_names.push_back(id);
}

inline std::string NodeNew::get_node_type() {
    return node_type;
}

inline void NodeNew::add_node_type(std::string type) {
    node_type = type;
}

inline void NodeNew::remove_node_type() {
    node_type = "";
}

inline std::vector<int> NodeNew::get_inlist() {
    return inlist;
}

inline void NodeNew::add_to_inlist(int eid) {
    inlist.push_back(eid);
}

inline void NodeNew::remove_from_inlist(int eid) {
    vector<int>::iterator itr = find(inlist.begin(), inlist.end(), eid);
    if (itr == inlist.end()) {
        cerr << "Error: edge with id " << eid << "is not in the inlist" << endl;
    } else {
        inlist.erase(itr);
    }
}

inline std::vector<int> NodeNew::get_outlist() {
    return outlist;
}

inline void NodeNew::add_to_outlist(int eid) {
    outlist.push_back(eid);
}

inline void NodeNew::remove_from_outlist(int eid) {
    vector<int>::iterator itr = find(outlist.begin(), outlist.end(), eid);
    if (itr == outlist.end()) {
        cerr << "Error: edge with id " << eid << "is not in the outlist" << endl;
    } else {
        outlist.erase(itr);
    }
}

inline int NodeNew::get_in_degree() {
    return inlist.size();
}

inline int NodeNew::get_out_degree() {
    return outlist.size();
}

bool NodeNew::is_target_of_GErel_edge(GraphNew* graph) {
    for (vector<int>::iterator inlist_itr = inlist.begin(); inlist_itr != inlist.end(); ++inlist_itr) {
        EdgeNew* edge = graph->get_edge_from_eid(*inlist_itr);
        if (edge) {
            if (edge->get_edge_type() == "GErel" && (edge->has_subtype("expression") || edge->has_subtype("repression"))) {
                return true;
            }
        } else {
            cerr << "Error: doesn't seem to be the right graph" << endl;
        }
    }
    return false;
}

inline void NodeNew::add_description_for_node(std::string desc){
    description = desc;
}
        
inline std::string NodeNew::get_description_for_node(){
    return description;
}
   
inline void NodeNew::add_compartment_for_node(std::string comp){
    compartment = comp;
}
        
inline std::string NodeNew::get_compartment_for_node(){
    return compartment;
}

EdgeNew::EdgeNew() {
    edge_id = ++edge_id_counter;
}

inline int EdgeNew::get_edge_id() {
    return edge_id;
}

inline std::string EdgeNew::get_edge_type() {
    return edge_type;
}

inline void EdgeNew::add_edge_type(std::string type) {
    edge_type = type;
}

inline void EdgeNew::remove_edge_type() {
    edge_type = "";
}

inline std::vector<std::string> EdgeNew::get_edge_subtypes() {
    return edge_subtypes;
}

inline void EdgeNew::add_subtype(std::string subtype) {
    edge_subtypes.push_back(subtype);
}

inline std::vector<std::string> EdgeNew::get_pathways() {
    return edge_belongs_to_pathways;
}

inline void EdgeNew::add_pathway(std::string pathway) {
    edge_belongs_to_pathways.push_back(pathway);
}

inline void EdgeNew::add_source(int nid) {
    edge_source = nid;
}

inline int EdgeNew::get_source() {
    return edge_source;
}

inline void EdgeNew::add_target(int nid) {
    edge_target = nid;
}

inline int EdgeNew::get_target() {
    return edge_target;
}

inline bool EdgeNew::has_subtype(std::string subtype) {
    for (vector<string>::iterator subt_itr = edge_subtypes.begin(); subt_itr != edge_subtypes.end(); subt_itr++) {
        if (*subt_itr == subtype) {
            return true;
        }
    }
    return false;
}

inline void EdgeNew::add_reaction_desc(std::string desc) {
    reaction_desc = desc;
}

inline std::string EdgeNew::get_reaction_desc() {
    return reaction_desc;
}

inline void EdgeNew::set_other_value1(int value) {
    other1 = value;
}

inline int EdgeNew::get_other_value1() {
    return other1;
}

inline void EdgeNew::set_other_value2(int value) {
    other2 = value;
}

inline int EdgeNew::get_other_value2() {
    return other2;
}

inline void EdgeNew::set_other_value3(int value) {
    other3 = value;
}

inline int EdgeNew::get_other_value3() {
    return other3;
}

inline void EdgeNew::set_other_value4(int value) {
    other4 = value;
}

inline int EdgeNew::get_other_value4() {
    return other4;
}

//these functions and vars needed for Expression Manager and CNF
extern int idx;
extern int clauses;
extern map<string, t_Expression *> getExpressionMap;
extern map<t_Expression*, pair<string, int> > getCNFIndexMap;

extern vector<int> add_cnf(vector<int> u, vector<int> v, int& idx, int & clauses, ofstream& fout);
extern string buildRelationalExpression(string reader, t_ExpressionManager* em, t_Expression* &expr);
extern string buildArithmaticExpression(string reader, t_ExpressionManager* em, t_Expression* &expr);
extern string buildSelectExpression(string reader, t_ExpressionManager* em, t_Expression* &expr);
extern string buildConcatExpression(string reader, t_ExpressionManager *em, t_Expression* &expr);
extern string buildBracketExpression(string reader, t_ExpressionManager *em, t_Expression* &expr);
extern string buildVariableExpression(string reader, t_ExpressionManager *em, t_Expression* &expr);

extern t_Expression* build_OR_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em);
extern t_Expression* build_AND_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em);
extern t_Expression* build_OR_constraints(t_Expression* e1, t_Expression* e2, int a1, int a2, t_ExpressionManager *em);
extern t_Expression* build_AND_constraints(t_Expression* e1, t_Expression* e2, int a1, int a2, t_ExpressionManager *em);

extern t_Expression* build_AND_constraints_from_vector(vector<t_Expression*>& vec, t_ExpressionManager *em);
extern t_Expression* build_OR_constraints_from_vector(vector<t_Expression*>& vec, t_ExpressionManager *em);
extern t_Expression* get_expression_sequence_positive_constraint(vector<t_Expression*>& nid_dist_variable_seq, t_ExpressionManager *em);
t_Expression* build_XOR_constraints(t_Expression* a, t_Expression* b, t_ExpressionManager *em);
t_Expression* build_EQUIV_constraint(t_Expression* a, t_Expression* b, t_ExpressionManager *em);


extern string IntToString(int num);

GraphNew::GraphNew() {

}

GraphNew::~GraphNew() {

}

template<typename Data>
void GraphNew::print_vector(std::vector<Data>& vec, ofstream& fout) {
    for (typename std::vector<Data>::iterator itr = vec.begin(); itr != vec.end(); ++itr) {
        fout << *itr << " ";
    }
    fout << endl;
}

template<typename Data>
void GraphNew::print_set(std::set<Data>& vec, ofstream& fout) {
    for (typename std::set<Data>::iterator itr = vec.begin(); itr != vec.end(); ++itr) {
        fout << *itr << " ";
    }
    fout << endl;
}

template<typename Key, typename Val>
void GraphNew::add_to_map(std::map<Key, Val>& generic_map, Key k, Val v) {
    generic_map.insert(pair<Key, Val>(k, v));
}

template<typename Key, typename Val>
void GraphNew::delete_from_map(std::map<Key, Val>& generic_map, Key k) {
    generic_map.erase(k);
}

template<typename Key, typename Val>
void GraphNew::print_map(std::map<Key, Val>& generic_map, ofstream& fout) {
    for (typename map<Key, Val>::iterator itr = generic_map.begin(); itr != generic_map.end(); ++itr) {
        fout << itr->first << " ==> " << itr->second << endl;
    }
    fout << endl;
}

template<typename Key, typename Val>
void GraphNew::add_to_map2(std::map<Key, std::vector<Val> >& generic_map, Key k, Val v) {
    typename std::map<Key, std::vector<Val> >::iterator itr = generic_map.find(k);
    if (itr == generic_map.end()) {
        std::vector<Val> temp;
        temp.push_back(v);
        generic_map.insert(pair<Key, std::vector<Val> >(k, temp));
    } else {
        itr->second.push_back(v);
    }
}

template<typename Key, typename Val>
std::vector<Val> GraphNew::get_val_map2(std::map<Key, std::vector<Val> >& generic_map, Key k) {
    std::vector<Val> empty_vec;
    typename std::map<Key, std::vector<Val> >::iterator itr = generic_map.find(k);
    if (itr == generic_map.end()) {
        return empty_vec;
    } else {
        return itr->second;
    }
}

template<typename Key, typename Val>
void GraphNew::delete_from_map2(std::map<Key, std::vector<Val> >& generic_map, Key k) {
    generic_map.erase(k);
}

template<typename Key, typename Val>
void GraphNew::print_map2(std::map<Key, std::vector<Val> >& generic_map, ofstream& fout) {
    for (typename map<Key, std::vector<Val> >::iterator itr = generic_map.begin(); itr != generic_map.end(); ++itr) {
        fout << itr->first << " ==> ";
        print_vector(itr->second, fout);
    }
    fout << endl;
}

template<typename Key, typename Val>
bool GraphNew::map_has_key_with_val(std::map<Key, std::vector<Val> >& generic_map, Key k, Val v) {
    typename std::map<Key, std::vector<Val> >::iterator itr = generic_map.find(k);
    if (itr == generic_map.end()) {
        return false;
    } else {
        typename std::vector<Val>::iterator itr_vec = find(itr->second.begin(), itr->second.end(), v);
        if (itr_vec == itr->second.end())
            return false;
    }
    return true;
}

inline int GraphNew::check_if_node_already_created(std::string rep_id) {
    return get_nid_from_rep_id(rep_id);
}

int GraphNew::create_new_node() {

    try {
        NodeNew * new_node = new NodeNew();
        int new_node_id = new_node->get_node_id();
        add_nid_node(new_node_id, new_node);
        return new_node_id;
    }    catch (std::bad_alloc &e) {
        cerr << "new failed in GraphNew::create_new_node()" << endl;
        return -1;
    }

    return -1;
}

//inline NodeNew * GraphNew::get_node_from_nid(int nid) {
//    std::unordered_map<int, NodeNew *>::iterator itr = nid_to_node_map.find(nid);
//    if (itr == nid_to_node_map.end()) {
//        cerr << "1. Not found node with id " << nid << endl;
//        return NULL;
//    } else {
//        NodeNew * node = (*itr).second;
//        return node;
//    }
//}
inline NodeNew * GraphNew::get_node_from_nid(int nid) {
    std::unordered_map<int, NodeNew *>::iterator itr = nid_to_node_map.find(nid);
    if (itr == nid_to_node_map.end())
        return NULL;
    return (*itr).second;
    
    
}

/* HIGHLY INEFFICIENT CODE: Supratik Aug 24, 2015  NEED TO CHANGE */
int GraphNew::check_if_edge_already_created(int new_source_nid, int new_target_nid, std::string edge_type, std::vector<std::string> edge_subtypes) {

    std::vector<int> all_outlist_eids = get_outlist(new_source_nid);
    std::vector<int>::iterator list_itr;

    // for each edge in outlist of source node
    for (list_itr = all_outlist_eids.begin(); list_itr != all_outlist_eids.end(); list_itr++) {
        int id = *list_itr;

        if (get_target_node(id) == new_target_nid && get_edge_type(id) == edge_type && get_edge_subtypes_of_edge(id) == edge_subtypes) {
            return id;
        }
    }
    return -1;
}

int GraphNew::create_new_edge() {

    try {
        EdgeNew * new_edge = new EdgeNew();
        int new_edge_id = new_edge->get_edge_id();
        add_eid_edge(new_edge_id, new_edge);
        return new_edge_id;
    }    catch (std::bad_alloc &e) {
        cerr << "new failed in GraphNew::create_new_edge()" << endl;
        return -1;
    }

    return -1;
}

void GraphNew::fill_maps_for_new_edge(int new_eid, int new_source_nid, int new_target_nid, string edge_type, vector<string> edge_subtypes, vector<string> edge_belongs_to_pathways) {
    
    add_edge_id(new_eid);

    // add edge info on new edge
    add_source_node(new_eid, new_source_nid);
    add_target_node(new_eid, new_target_nid);
    add_edge_type(new_eid, edge_type);
    std::vector<std::string>::iterator itr;
    for (itr = edge_subtypes.begin(); itr != edge_subtypes.end(); itr++) {
        add_subtype_for_edge(new_eid, (*itr));
    }
    for (itr = edge_belongs_to_pathways.begin(); itr != edge_belongs_to_pathways.end(); itr++) {
        add_pathway_for_edge(new_eid, (*itr));
    }

    //insert new edge in outlist of source and inlist of target nodes
    add_edge_to_outlist_of_node(new_source_nid, new_eid);
    add_edge_to_inlist_of_node(new_target_nid, new_eid);

}

inline EdgeNew * GraphNew::get_edge_from_eid(int eid) {
    std::unordered_map<int, EdgeNew *>::iterator itr = eid_to_edge_map.find(eid);
    if (itr == eid_to_edge_map.end())
        return NULL;
    return (*itr).second;
}

inline void GraphNew::add_nid_node(int nid, NodeNew * node) {
    nid_to_node_map.insert(pair<int, NodeNew *>(nid, node));
}

inline void GraphNew::add_eid_edge(int eid, EdgeNew * edge) {
    eid_to_edge_map.insert(pair<int, EdgeNew *>(eid, edge));
}

inline int GraphNew::get_graph_id() {
    return graph_id;
}

inline void GraphNew::set_graph_id(int gid) {
    graph_id = gid;
}

inline void GraphNew::set_first_offset(int offset) {
    first_offset = offset;
}

inline int GraphNew::get_first_offset() {
    return first_offset;
}

inline void GraphNew::add_node_id(int nid) {
    NIDSg.push_back(nid);
}

inline void GraphNew::add_edge_id(int eid) {
    EIDSg.push_back(eid);
}

inline std::vector<int> GraphNew::get_node_ids() {
    return NIDSg;
}

inline std::vector<int> GraphNew::get_edge_ids() {
    return EIDSg;
}

inline std::string GraphNew::get_rep_id_from_id(std::string id) {
    std::map<std::string, std::string>::iterator itr = id_to_rep_id_map.find(id);
    if (itr != id_to_rep_id_map.end()) {
        return itr->second;
    } else {
        return "";
    }
}
//std::string GraphNew::get_rep_id_from_id(std::string id) {
//        std::string rep_id = "";
//        std::map<int, NodeNew *>::iterator itr;
//        for (itr = nid_to_node_map.begin(); itr != nid_to_node_map.end(); itr++) {
//            std::vector<std::string> all_id = (*itr).second->get_all_id();
//            std::vector<std::string>::iterator itr1 = std::find(all_id.begin(), all_id.end(), id);
//            if (itr1 == all_id.end()) {
//                //cerr << "Not found node with id " << id << endl;
//            }
//            else {
//                NodeNew * node = (*itr).second;
//                rep_id = node->get_rep_id();
//            }
//        }
//        return rep_id;
//}

//std::string GraphNew::get_rep_id_from_nid(int nid) {
//    std::map<int, std::vector<std::string> > ::iterator itr = node_id_to_all_id_map.find(nid);
//    if (itr != node_id_to_all_id_map.end()) {
//    	assert(!((itr->second).empty()));
//        std::string temp = itr->second[0];// get first id corresponding to nid
//        std::string rid = get_rep_id_from_id(temp);
//        assert(rid != "");
//        return rid;
//    }
//    else {
//        return "";
//    }
//}

inline std::string GraphNew::get_rep_id_from_nid(int nid) {
    std::string rep_id = "";
    std::unordered_map<int, NodeNew *>::iterator itr = nid_to_node_map.find(nid);
    if (itr != nid_to_node_map.end()) {
        rep_id = (*itr).second->get_rep_id();
    }
    return rep_id;
}


//int GraphNew::get_nid_from_rep_id(std::string repid) {
//    std::map<int, std::vector<std::string> >::iterator itr;
//    vector<string> all_id_for_repid = get_all_ids_for_rep_id(repid);
//    if(all_id_for_repid.empty()){
//    	return -1;
//    }
//    for(itr = node_id_to_all_id_map.begin(); itr != node_id_to_all_id_map.end(); itr++) {
//    	if(itr->second == all_id_for_repid){
//    		return itr->first;
//    	}
//    }
//    assert(false);
//    return -1;
//}
//int GraphNew::get_nid_from_rep_id(std::string repid) {
//    std::map<int, NodeNew *>::iterator itr;
//    for (itr = nid_to_node_map.begin(); itr != nid_to_node_map.end(); itr++) {
//        NodeNew * node = (*itr).second;
//        if (node->get_rep_id() == repid){
//            return node->get_node_id();
//        }
//    }
//    return -1;
//}

inline int GraphNew::get_nid_from_rep_id(std::string repid) {
    std::map<std::string, int>::iterator itr = rep_id_to_nid_map.find(repid);
    if (itr == rep_id_to_nid_map.end()) {
        return -1;
    } else {
        return (*itr).second;
    }
}



inline void GraphNew::add_to_rep_id_nid_map(std::string rep_id, int nid) {
    rep_id_to_nid_map.insert(pair<std::string, int>(rep_id, nid));
}



inline void GraphNew::add_rep_id_for_node(int nid, std::string rep_id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "1. Not found node with id " << nid << endl;
    } else {
        node->set_rep_id(rep_id);
        add_to_rep_id_nid_map(rep_id, nid);
    }
}

inline void GraphNew::add_id_rep_id(std::string id, std::string repid) {
    add_to_map(id_to_rep_id_map, id, repid);
}
//void GraphNew::add_id_rep_id(std::string id, std::string repid){
//    std::map<int, NodeNew *>::iterator itr;
//    for (itr = nid_to_node_map.begin(); itr != nid_to_node_map.end(); itr++) {
//        NodeNew * node = (*itr).second;
//        if (node->get_rep_id() == repid) {
//            node->add_to_all_id(id);
//        }
//    }
//}


//std::vector<std::string> GraphNew::get_all_ids_for_rep_id(std::string rep_id){
//	std::vector<std::string> all_ids_for_repid;
//	std::map<std::string, std::string>::iterator itr;
//	for(itr = id_to_rep_id_map.begin(); itr != id_to_rep_id_map.end(); ++itr){
//		if(itr->second == rep_id){
//			all_ids_for_repid.push_back(itr->first);
//		}
//	}
//	return all_ids_for_repid;
//}

inline std::vector<std::string> GraphNew::get_all_ids_for_rep_id(std::string repid) {
    std::unordered_map<int, NodeNew *>::iterator itr;
    for (itr = nid_to_node_map.begin(); itr != nid_to_node_map.end(); itr++) {
        NodeNew * node = (*itr).second;
        if (node->get_rep_id() == repid) {
            return node->get_all_id();
        }
    }
    cout << "Not found node with rep-id " << repid << endl;
    return vector<string>();
//    std::vector<std::string> all_ids;
//    all_ids.empty();
//    return all_ids;
}

inline void GraphNew::set_all_ids_for_node(int nid, std::vector<std::string> all_ids) {
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->set_all_id(all_ids);
}

//std::vector<std::string> GraphNew::get_all_ids_of_node(int nid){
//	return get_val_map2(node_id_to_all_id_map, nid);
//}

inline std::vector<std::string> GraphNew::get_all_ids_of_node(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL)
        return vector<string>();
    return node->get_all_id();
}

//bool GraphNew::node_has_id(int nid, std::string id){
//	return map_has_key_with_val(node_id_to_all_id_map, nid, id);
//}

inline bool GraphNew::node_has_id(int nid, std::string id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "2. Not found node with id " << nid << endl;
        return false;
    } else {
        std::vector<std::string> all_ids = node->get_all_id();
        if (std::find(all_ids.begin(), all_ids.end(), id) == all_ids.end())
            return false;
        else
            return true;
    }
}

//void GraphNew::add_id_for_node(int nid, std::string id){
//	add_to_map2(node_id_to_all_id_map, nid, id);
//}

inline void GraphNew::add_id_for_node(int nid, std::string id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->add_to_all_id(id);
    
}

//std::vector<int> GraphNew::get_component_ids_of_node(int nid){
//	return get_val_map2(components, nid);
//}

inline std::vector<int> GraphNew::get_component_ids_of_node(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL)
        return vector<int>();
    return node->get_comp_ids();
}

//bool GraphNew::node_has_component_id(int nid, int comp_id){
//	return map_has_key_with_val(components, nid, comp_id);
//}

inline bool GraphNew::node_has_component_id(int nid, int comp_id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        return false;
    } else {
        std::vector<int> components = node->get_comp_ids();
        if (std::find(components.begin(), components.end(), comp_id) != components.end()) {
            return true;
        } else
            return false;
    }
}

//void GraphNew::add_component_id_for_node(int nid, int comp_id){
//	add_to_map2(components, nid, comp_id);
//}

inline void GraphNew::add_component_id_for_node(int nid, int comp_id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->add_comp_id(comp_id);
    
}

void GraphNew::update_component_of_node_at_index(int nid, int pos, int value) {
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->update_component_id_at_index(pos, value);
         
}
inline void GraphNew::set_all_display_id_for_node(int nid, std::vector<std::string> all_disp_names) {
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->set_all_display_names(all_disp_names);
    
}

//std::vector<std::string> GraphNew::get_all_display_ids_of_node(int nid){
//	return get_val_map2(node_id_to_display_name_map, nid);
//}

inline std::vector<std::string> GraphNew::get_all_display_ids_of_node(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL)
        return vector<string>();
    return node->get_disp_ids();
}

//bool GraphNew::node_has_display_id(int nid, std::string id){
//	return map_has_key_with_val(node_id_to_display_name_map, nid, id);
//}

bool GraphNew::node_has_display_id(int nid, std::string id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "3. Not found node with id " << nid << endl;
        return false;
    } else {
        std::vector<std::string> all_disp_ids = node->get_disp_ids();
        if (std::find(all_disp_ids.begin(), all_disp_ids.end(), id) != all_disp_ids.end()) {
            return true;
        } else
            return false;
    }
}

//void GraphNew::add_display_id_for_node(int nid, std::string id){
//	add_to_map2(node_id_to_display_name_map, nid, id);
//}

inline void GraphNew::add_display_id_for_node(int nid, std::string id) {
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->add_disp_id(id);
    
}

//std::string GraphNew::get_node_type(int nid){
//	std::map<int, std::string>::iterator itr = node_type.find(nid);
//	if(itr != node_type.end()){
//		return itr->second;
//	}
//	else{
//		return "";
//	}
//}

inline std::string GraphNew::get_node_type(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL)
        return "";
    return node->get_node_type();
    
}

//void GraphNew::add_node_type(int nid, string type){
//	add_to_map(node_type, nid, type);
//}

inline void GraphNew::add_node_type(int nid, std::string type) {
//    if (type == "gene" || type == "protein")
//        type = "gene_or_protein";
    NodeNew * node = get_node_from_nid(nid);
    if (node != NULL)
        node->add_node_type(type);
    
}

//void GraphNew::remove_node_type_info(int nid){
//	delete_from_map(node_type, nid);
//}

inline void GraphNew::remove_node_type_info(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "4 .Not found node with id " << nid << endl;
    } else {
        node->remove_node_type();
    }
}

inline void GraphNew::add_description_for_node(int nid, std::string desc){
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "5. Not found node with id " << nid << endl;
    } else {
        node->add_description_for_node(desc);
    }
    
}

inline std::string GraphNew::get_description_for_node(int nid){
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "6. Not found node with id " << nid << endl;
        return "";
    } else {
        return node->get_description_for_node();
    }
}


void GraphNew::add_compartment_for_node(int nid, std::string comp){
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "7. Not found node with id " << nid << endl;
    } else {
        node->add_compartment_for_node(comp);
    }
    
}

inline std::string GraphNew::get_compartment_for_node(int nid){
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "8. Not found node with id " << nid << endl;
        return "";
    } else {
        return node->get_compartment_for_node();
    }
}

//std::string GraphNew::get_edge_type(int eid){
//	std::map<int, std::string>::iterator itr = edge_type.find(eid);
//	if(itr != edge_type.end()){
//		return itr->second;
//	}
//	else{
//		return "";
//	}
//}

inline std::string GraphNew::get_edge_type(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "9. Not found edge with id " << eid << endl;
        return "";
    } else {
        return edge->get_edge_type();
    }
}

//void GraphNew::add_edge_type(int eid, std::string type){
//	add_to_map(edge_type, eid, type);
//}

inline void GraphNew::add_edge_type(int eid, std::string type) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "10. Not found edge with id " << eid << endl;
    } else {
        edge->add_edge_type(type);
    }
}

//void GraphNew::remove_edge_id_type(int eid){
//	delete_from_map(edge_type, eid);
//}

inline void GraphNew::remove_edge_id_type(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "11. Not found edge with id " << eid << endl;
    } else {
        edge->remove_edge_type();
    }
}

//std::vector<std::string> GraphNew::get_edge_subtypes_of_edge(int eid){
//	return get_val_map2(edge_subtypes, eid);
//}

inline std::vector<std::string> GraphNew::get_edge_subtypes_of_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "12. Not found edge with id " << eid << endl;
        std::vector<std::string> subtypes;
        subtypes.empty();
        return subtypes;
    } else {
        return edge->get_edge_subtypes();
    }
}

//bool GraphNew::edge_has_subtype(int eid, std::string subtype){
//	return map_has_key_with_val(edge_subtypes, eid, subtype);
//}

inline bool GraphNew::edge_has_subtype(int eid, std::string subtype) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "13. Not found edge with id " << eid << endl;
        return false;
    } else {
        std::vector<std::string> all_subtypes = edge->get_edge_subtypes();
        if (std::find(all_subtypes.begin(), all_subtypes.end(), subtype) != all_subtypes.end()) {
            return true;
        } else
            return false;
    }
}

//void GraphNew::add_subtype_for_edge(int eid, std::string subtype){
//	add_to_map2(edge_subtypes, eid, subtype);
//}

inline void GraphNew::add_subtype_for_edge(int eid, std::string subtype) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "14. Not found edge with id " << eid << endl;
    } else {
        edge->add_subtype(subtype);
    }
}

inline void GraphNew::add_reaction_desc_for_edge(int eid, std::string desc) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "15. Not found edge with id " << eid << endl;
    } else {
        edge->add_reaction_desc(desc);
    }
}

inline std::string GraphNew::get_reaction_desc_for_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "16. Not found edge with id " << eid << endl;
        return "";
    } else {
        return edge->get_reaction_desc();
    }
}
//std::vector<std::string> GraphNew::get_all_pathways_for_edge(int eid){
//	return get_val_map2(edge_belongs_to_pathways, eid);
//}


inline std::vector<std::string> GraphNew::get_all_pathways_for_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "17. Not found edge with id " << eid << endl;
        std::vector<std::string> pathways;
        pathways.empty();
        return pathways;
    } else {
        return edge->get_pathways();
    }
}

//bool GraphNew::edge_belongs_to_pathway(int eid, std::string pathway){
//	return map_has_key_with_val(edge_belongs_to_pathways, eid, pathway);
//}

inline bool GraphNew::edge_belongs_to_pathway(int eid, std::string pathway) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "18. Not found edge with id " << eid << endl;
        return false;
    } else {
        std::vector<std::string> all_pathways = edge->get_pathways();
        if (std::find(all_pathways.begin(), all_pathways.end(), pathway) != all_pathways.end()) {
            return true;
        } else
            return false;
    }
}

//void GraphNew::add_pathway_for_edge(int eid, std::string pathway){
//	add_to_map2(edge_belongs_to_pathways, eid, pathway);
//}

inline void GraphNew::add_pathway_for_edge(int eid, std::string pathway) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "19. Not found edge with id " << eid << endl;
    } else {
        edge->add_pathway(pathway);
    }
}

//void GraphNew::add_source_node(int eid, int nid){
//	add_to_map(edge_source, eid, nid);
//}

inline void GraphNew::add_source_node(int eid, int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "20. Not found node with id " << nid << endl;
    }
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    edge->add_source(nid);
}

//int GraphNew::get_source_node(int eid){
//	std::map<int, int>::iterator itr = edge_source.find(eid);
//	if(itr != edge_source.end()){
//		return itr->second;
//	}
//	else{
//		return -1;
//	}
//}

inline int GraphNew::get_source_node(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
        return -1;
    } else {
        return edge->get_source();
    }
}

//void GraphNew::add_target_node(int eid, int nid){
//	add_to_map(edge_target, eid, nid);
//}

inline void GraphNew::add_target_node(int eid, int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "21. Not found node with id " << nid << endl;
    }
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    edge->add_target(nid);
}

//int GraphNew::get_target_node(int eid){
//	std::map<int, int>::iterator itr = edge_target.find(eid);
//	if(itr != edge_target.end()){
//		return itr->second;
//	}
//	else{
//		return -1;
//	}
//}

inline int GraphNew::get_target_node(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
        return -1;
    } else {
        return edge->get_target();
    }
}

//std::vector<int> GraphNew::get_inlist(int nid){
//	return get_val_map2(inlist, nid);
//}

inline std::vector<int> GraphNew::get_inlist(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "22. Not found node with id " << nid << endl;
        std::vector<int> inlist;
        inlist.empty();
        return inlist;
    } else {
        return node->get_inlist();
    }
}

//bool GraphNew::node_has_edge_in_inlist(int nid, int eid){
//	return map_has_key_with_val(inlist, nid, eid);
//}

inline bool GraphNew::node_has_edge_in_inlist(int nid, int eid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "23. Not found node with id " << nid << endl;
        return false;
    } else {
        std::vector<int> edges_in_inlist = node->get_inlist();
        if (std::find(edges_in_inlist.begin(), edges_in_inlist.end(), eid) != edges_in_inlist.end()) {
            return true;
        } else
            return false;
    }
}

//void GraphNew::add_edge_to_inlist_of_node(int nid, int eid){
//	add_to_map2(inlist, nid, eid);
//}

inline void GraphNew::add_edge_to_inlist_of_node(int nid, int eid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "24. Not found node with id " << nid << endl;
    } else {
        node->add_to_inlist(eid);
    }
}

//std::vector<int> GraphNew::get_outlist(int nid){
//	return get_val_map2(outlist, nid);
//}

inline std::vector<int> GraphNew::get_outlist(int nid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL)
        return vector<int>();
    return node->get_outlist();
}

//bool GraphNew::node_has_edge_in_outlist(int nid, int eid){
//	return map_has_key_with_val(outlist, nid, eid);
//}

inline bool GraphNew::node_has_edge_in_outlist(int nid, int eid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "25. Not found node with id " << nid << endl;
        return false;
    } else {
        std::vector<int> edges_in_outlist = node->get_outlist();
        if (std::find(edges_in_outlist.begin(), edges_in_outlist.end(), eid) != edges_in_outlist.end()) {
            return true;
        } else
            return false;
    }
}

//void GraphNew::add_edge_to_outlist_of_node(int nid, int eid){
//	add_to_map2(outlist, nid, eid);
//}

inline void GraphNew::add_edge_to_outlist_of_node(int nid, int eid) {
    NodeNew * node = get_node_from_nid(nid);
    if (node == NULL) {
        cerr << "26. Not found node with id " << nid << endl;
    } else {
        node->add_to_outlist(eid);
    }
}

inline void GraphNew::set_other_value1_in_edge(int eid, int value) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        edge->set_other_value1(value);
    }
}
        
inline int GraphNew::get_other_value1_in_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        return edge->get_other_value1();
    }
}

inline void GraphNew::set_other_value2_in_edge(int eid, int value) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        edge->set_other_value2(value);
    }
}
        
inline int GraphNew::get_other_value2_in_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        return edge->get_other_value2();
    }
}

inline void GraphNew::set_other_value3_in_edge(int eid, int value) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        edge->set_other_value3(value);
    }
}
        
inline int GraphNew::get_other_value3_in_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        return edge->get_other_value3();
    }
}
    

inline void GraphNew::set_other_value4_in_edge(int eid, int value) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        edge->set_other_value4(value);
    }
}
        
inline int GraphNew::get_other_value4_in_edge(int eid) {
    EdgeNew * edge = get_edge_from_eid(eid);
    if (edge == NULL) {
        cerr << "Not found edge with id " << eid << endl;
    }
    else {
        return edge->get_other_value4();
    }
}


/****  HIGHLY INEFFICIENT CODE : Supratik, Aug 22, 2015.  NEEDS CHANGE ***/
int GraphNew::get_node_id_with_components(std::set<int>& comp_ids_set) {
    assert(!comp_ids_set.empty());
    int nid = -1;
    std::vector<int> comp_ids_vec(comp_ids_set.begin(), comp_ids_set.end());
    for (std::vector<int>::iterator node_id_itr = NIDSg.begin(); node_id_itr != NIDSg.end(); node_id_itr++) {
        if (get_node_type(*node_id_itr) == "group" && get_component_ids_of_node(*node_id_itr) == comp_ids_vec) {
            nid = *node_id_itr;
            break;
        }
    }
    return nid;
}

std::string GraphNew::concatenate_sorted_rep_ids(std::set<int>& components, std::string delim) {
    std::string to_return = "";
    std::set<std::string> comp_rep_ids;

    for (std::set<int>::iterator set_itr = components.begin(); set_itr != components.end(); set_itr++) {
        string rid = get_rep_id_from_nid(*set_itr);
        assert(rid != ""); //must be assigned rep ids to components from same graph
        comp_rep_ids.insert(rid);
    }

    if (comp_rep_ids.size() == 1) {
        return *comp_rep_ids.begin();
    }

    to_return = *comp_rep_ids.begin();

    for (std::set<std::string>::iterator set_itr = comp_rep_ids.begin(); set_itr != comp_rep_ids.end(); set_itr++) {
        if (set_itr != comp_rep_ids.begin()) {
            to_return += delim + *set_itr;
        }
    }

    return to_return;
}

void GraphNew::bfs(int source_node, unordered_map<int, int>& bfs_tree) {
    
#ifdef DEBUG_FLAG
    debug_log << endl << " -- bfs begins --" << endl;
#endif
    
    int curr_source, curr_target;
    list<int> queue;
    set<int> visited;
    
    
    queue.push_back(source_node);
    visited.insert(source_node);
    
    while(!queue.empty()) {
        curr_source = queue.front();
        
        queue.pop_front();
    
        vector<int> out_list = get_outlist(curr_source);
        
        for (vector<int>::iterator edge_itr = out_list.begin(); edge_itr != out_list.end(); edge_itr++) {
            curr_target = get_target_node(*edge_itr);
           
            if (visited.find(curr_target) == visited.end()) {
                queue.push_back(curr_target);
                bfs_tree.insert(pair<int, int>(curr_target, curr_source));
                visited.insert(curr_target);
#ifdef DEBUG_FLAG
                debug_log << curr_source << " -> " << curr_target << endl;
#endif                 
            }
        }
       
    }
    
#ifdef DEBUG_FLAG
    debug_log << " -- bfs ends --" << endl;
#endif    
}

void GraphNew::get_bfs_nodes(int i, set<int>& reachable_nodes, int direction) {
        set<int> visited;
	list<int> bfs_queue;
	
	bfs_queue.push_back(i);

	while (!bfs_queue.empty()) {

		int curr_node = bfs_queue.front();
		bfs_queue.pop_front();

		if (visited.find(curr_node) == visited.end()) {
			
			visited.insert(curr_node);
			reachable_nodes.insert(curr_node);
                        
                        vector<int> edges_list;
                        if(direction == FORWARD) {
                            edges_list = get_outlist(curr_node);
                        }
                        
                        else if(direction == BACKWARD) {
                            edges_list = get_inlist(curr_node);
                        }
                        
                        else {
                            cerr << "Invalid direction" << endl;
                        }
                        
			for (vector<int>::iterator vec_itr = edges_list.begin(); vec_itr != edges_list.end(); vec_itr++) {

                            if(direction == FORWARD) {
				int target_node = get_target_node(*vec_itr);
				bfs_queue.push_back(target_node);
                            }
                            
                            else if(direction == BACKWARD) {
                                int source_node = get_source_node(*vec_itr);
                                bfs_queue.push_back(source_node);
                            }
                            else {
                                cerr << "Invalid direction" << endl;
                            }

			}
		}

	}

}

void GraphNew::augment_bfs(int source_node, int target_node, unordered_map<int, int>& bfs_tree) {

}

list<int> GraphNew::get_path(int source, int target, unordered_map<int, int> bfs_tree) {
    
    unordered_map<int, int>::iterator map_itr;
    list<int> path;
    path.push_front(target);
    while (target != source) {
        map_itr = bfs_tree.find(target);
        target = (*map_itr).second;
        path.push_front(target);
    }
#ifdef DEBUG_FLAG
    debug_log << " -- path -- " << endl;
    for(list<int>::iterator list_itr = path.begin(); list_itr != path.end(); list_itr++) {
        debug_log << (*list_itr) << " ";
    }
    debug_log << endl;
#endif 
    
    return path;
}

    

void GraphNew::write_node_to_dot_file_richer(ofstream& myfile, int nid, set<int>& drawn_nodes, color_triple_new color) {

    std::string repid = get_rep_id_from_nid(nid);
    std::string type = get_node_type(nid);
    string delim = "\\n";
    //std::string display_names = concatenate_strings(get_all_display_ids_of_node(nid), " | ");
    std::string display_names = get_all_display_ids_of_node(nid).front();
    std::string all_ids = concatenate_strings(get_all_ids_of_node(nid), delim);
    std::vector<std::string> all_id_vec = get_all_ids_of_node(nid);
    
    // strip compartment information from display name
//    string::size_type open_brac;
//    open_brac = display_names.find("[", 0);
//    display_names = display_names.substr(0, open_brac-1);
//    
    /*
    if (type == "compound") {
        myfile << nid << " [shape = circle, penwidth=2, label = \"" << nid - 1 << "\"";
    } else if (type == "group") {
        //cout << "Do I xdot groups" << endl;
        myfile << nid << " [shape = hexagon, penwidth=2, peripheries=2, label = \"" << nid - 1  << "\"";
    } else if (type == "phenotype") {
        myfile << nid << " [shape = house, penwidth=2, peripheries=2, label = \"" << nid - 1  << "\"";
    } else if (type == "reaction") {
        std::string desc = get_description_for_node(nid);
        myfile << nid << " [ shape = rectangle, height = 0.2, width = 1, tooltip = \"" << desc << "\"," << " label = <<font color=\"grey\" point-size=\"12\">" << nid - 1  << "</font>>, URL = \"http://www.reactome.org/content/detail/" << nid - 1  << "\", style = \"rounded, filled\", color = orange ";
        //myfile << nid << " [shape = square, penwidth=2, peripheries=2, label = \"" << nid - 1  + "\\n(" + all_ids + ")" << "\"";
    } else {
        //myfile << nid << " [style = \"rounded, filled\", color = grey, margin = 0.05, shape = box, tooltip = \"" << display_names << "\"," << " label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
        myfile << nid << " [style = \"rounded, filled\", color = grey, margin = 0.05, shape = box, tooltip = \"" << nid - 1  << "\"," << " label = " <<
                "< <TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\"> " <<
                "<TR><TD>" << "[" << nid -1 << "] " << display_names  << "</TD></TR>" << endl;
                for(int i = 0; i < all_id_vec.size(); i++){
                    //myfile << "<TR><TD TOOLTIP=\"" << display_names << "\" ";
                    string curr_id = all_id_vec.at(i);
                    //indiv_ids = split_string_into_list(curr_id, '.');
                    
                    if(curr_id.substr(0,3) == "hsa"){
                        myfile << "<TR><TD TOOLTIP=\"" << "KEGG: " << curr_id << "\" " << "HREF=\"http://www.genome.jp/dbget-bin/www_bget?" << "hsa:" << curr_id.substr(3) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "KEGG: " << curr_id << "</FONT></TD></TR>" << endl;
                    }
                    else if(curr_id.substr(0,7) == "uniprot"){
                        myfile << "<TR><TD TOOLTIP=\"" << "UniProt: " << curr_id.substr(8) << "\" " << "HREF=\"http://www.uniprot.org/uniprot/" << curr_id.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "UniProt: " << curr_id.substr(8) << "</FONT></TD></TR>" << endl;
                    }
                    else if(curr_id.substr(0,8) == "reactome"){
                        myfile << "<TR><TD TOOLTIP=\"" << "Reactome: " << curr_id.substr(9) << "\" " << "HREF=\"http://www.reactome.org/content/detail/" << curr_id.substr(9) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "Reactome: " << curr_id.substr(9) << "</FONT></TD></TR>" << endl;
                    }
////                    else
////                    {
////                        myfile << "<TR><TD TOOLTIP=\"" << curr_id << "\" " << "><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << curr_id << "</FONT></TD></TR>" << endl;
////                    }
                     
                }
        myfile << "</TABLE>>" << endl;
    }*/

        if (type == "compound") {
        myfile << nid << " [shape = circle, penwidth=2, label = \"" << "[" << nid -1 << "] " << display_names + "\\n(" + all_ids + ")" << "\"";
    } else if (type == "group") {
        //cout << "Do I xdot groups" << endl;
        myfile << nid << " [shape = hexagon, penwidth=2, peripheries=2, label = \"" << "[" << nid -1 << "] " << display_names + "\\n(" + all_ids + ")" << "\"";
    } else if (type == "phenotype") {
        myfile << nid << " [shape = house, penwidth=2, peripheries=2, label = \"" << "[" << nid -1 << "] " << display_names << "\"";
    } else if (type == "reaction") {
        std::string desc = get_description_for_node(nid);
        myfile << nid << " [ shape = rectangle, height = 0.2, width = 1, tooltip = \"" << desc << "\"," << " label = <<font color=\"grey\" point-size=\"12\">" << display_names << "</font>>, URL = \"http://www.reactome.org/content/detail/" << display_names << "\", style = \"rounded, filled\", color = orange ";
        //myfile << nid << " [shape = square, penwidth=2, peripheries=2, label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
    } else {
        //myfile << nid << " [style = \"rounded, filled\", color = grey, margin = 0.05, shape = box, tooltip = \"" << display_names << "\"," << " label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
        myfile << nid << " [style = \"rounded, filled\", color = grey, margin = 0.05, shape = box, tooltip = \"" << nid << "\"," << " label = " <<
                "< <TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\"> " <<
                "<TR><TD>" << "[" << nid -1 << "] " << nid << "</TD></TR>" << endl;
                for(int i = 0; i < all_id_vec.size(); i++){
                    //myfile << "<TR><TD TOOLTIP=\"" << display_names << "\" ";
                    string curr_id = all_id_vec.at(i);
                    //indiv_ids = split_string_into_list(curr_id, '.');
                    
                    if(curr_id.substr(0,3) == "hsa"){
                        myfile << "<TR><TD TOOLTIP=\"" << "KEGG: " << curr_id << "\" " << "HREF=\"http://www.genome.jp/dbget-bin/www_bget?" << "hsa:" << curr_id.substr(3) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "KEGG: " << curr_id << "</FONT></TD></TR>" << endl;
                    }
                    else if(curr_id.substr(0,7) == "uniprot"){
                        myfile << "<TR><TD TOOLTIP=\"" << "UniProt: " << curr_id.substr(8) << "\" " << "HREF=\"http://www.uniprot.org/uniprot/" << curr_id.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "UniProt: " << curr_id.substr(8) << "</FONT></TD></TR>" << endl;
                    }
                    else if(curr_id.substr(0,8) == "reactome"){
                        myfile << "<TR><TD TOOLTIP=\"" << "Reactome: " << curr_id.substr(9) << "\" " << "HREF=\"http://www.reactome.org/content/detail/" << curr_id.substr(9) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "Reactome: " << curr_id.substr(9) << "</FONT></TD></TR>" << endl;
                    }
                    else
                    {
                        myfile << "<TR><TD TOOLTIP=\"" << curr_id << "\" " << "><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << curr_id << "</FONT></TD></TR>" << endl;
                    }
                     
                }
        myfile << "</TABLE>>" << endl;
    }

    myfile << ", fillcolor = \"#";

    if (color.red < 16) {
        myfile << "0";
    }
    //myfile << hex << color.red;
    std::stringstream stream1;
    stream1 << std::hex << color.red;
    string hex_red(stream1.str());
    myfile << hex_red;
    if (color.green < 16) {
        myfile << "0";
    }
    std::stringstream stream2;
    stream2 << std::hex << color.green;
    string hex_green(stream2.str());
    myfile << hex_green;
    //myfile << hex << color.green;
    if (color.blue < 16) {
        myfile << "0";
    }
    std::stringstream stream3;
    stream3 << std::hex << color.blue;
    string hex_blue(stream3.str());
    myfile << hex_blue;
    //myfile << hex << color.blue;
    myfile << "\"";


    myfile << "];\n";
    drawn_nodes.insert(nid);
}

void GraphNew::write_node_to_dot_file(ofstream& myfile, int nid, set<int>& drawn_nodes, color_triple_new color) {

    std::string type = get_node_type(nid);
    string delim = " ";
    std::string display_names = concatenate_strings(get_all_display_ids_of_node(nid), delim);
    std::string all_ids = concatenate_strings(get_all_ids_of_node(nid), delim);

    if (type == "compound") {
        myfile << nid << " [shape = circle, penwidth=2, label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
    } else if (type == "group") {
        //cout << "Do I xdot groups" << endl;
        myfile << nid << " [shape = hexagon, penwidth=2, peripheries=2, label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
    } else if (type == "phenotype") {
        myfile << nid << " [shape = house, penwidth=2, peripheries=2, label = \"" << display_names << "\"";
    } else if (type == "reaction") {
        myfile << nid << " [shape = square, penwidth=2, peripheries=2, label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
    } else {
        myfile << nid << " [shape = box, penwidth=2, label = \"" << display_names + "\\n(" + all_ids + ")" << "\"";
    }


    myfile << ", style = filled, color = black, fillcolor = \"#";

    if (color.red < 16) {
        myfile << "0";
    }
    //myfile << hex << color.red;
    std::stringstream stream1;
    stream1 << std::hex << color.red;
    string hex_red(stream1.str());
    myfile << hex_red;
    if (color.green < 16) {
        myfile << "0";
    }
    std::stringstream stream2;
    stream2 << std::hex << color.green;
    string hex_green(stream2.str());
    myfile << hex_green;
    //myfile << hex << color.green;
    if (color.blue < 16) {
        myfile << "0";
    }
    std::stringstream stream3;
    stream3 << std::hex << color.blue;
    string hex_blue(stream3.str());
    myfile << hex_blue;
    //myfile << hex << color.blue;
    myfile << "\"";


    myfile << "];\n";
    drawn_nodes.insert(nid);
}

void GraphNew::write_node_to_xml_file(int nid, map<int, int>& nid_to_xml_node_id_map, std::ofstream &fout) {
    int xml_id = nid_to_xml_node_id_map[nid];
    vector<string> all_ids = get_all_ids_of_node(nid);
    string type = get_node_type(nid);
    vector<string> all_dis_names = get_all_display_ids_of_node(nid);
    vector<int> components = get_component_ids_of_node(nid);

    string name = concatenate_strings(all_ids, " ");
    string display_name = concatenate_strings(all_dis_names, ", ");
    vector<int> components_xml_list;

    for (vector<int>::iterator itr = components.begin(); itr != components.end(); ++itr) {
        int xml_nid = nid_to_xml_node_id_map[*itr];
        components_xml_list.push_back(xml_nid);
    }

    fout << "\t<entry id=\"" << xml_id << "\" name=\"" + name << "\" type=\"" + type + "\">" << endl;
    fout << "\t\t<graphics name=\"" + display_name + "\"/>" << endl;
    for (vector<int>::iterator itr = components_xml_list.begin(); itr != components_xml_list.end(); ++itr) {
        fout << "\t\t<component id=\"" << *itr << "\"/>" << endl;
    }
    fout << "\t</entry>" << endl;
}

void GraphNew::write_edge_to_xml_file(int eid, map<int, int>& nid_to_xml_node_id_map, std::ofstream &fout) {
    int source_id = get_source_node(eid);
    int target_id = get_target_node(eid);

    int source_id_xml = nid_to_xml_node_id_map[source_id];
    int target_id_xml = nid_to_xml_node_id_map[target_id];

    vector<string> pathways = get_all_pathways_for_edge(eid);
    string concatenated_pathways = concatenate_strings(pathways, " ");

    string type = get_edge_type(eid);
    vector<string> subtypes = get_edge_subtypes_of_edge(eid);

    //fout << "\t<relation entry1=\"" << source_id_xml <<"\" entry2=\"" << target_id_xml << "\" type=\""+type+"\">"<< endl;
    fout << "\t<relation entry1=\"" << source_id_xml << "\" entry2=\"" << target_id_xml << "\" type=\"" + type << "\" pathways=\"" << concatenated_pathways + "\">" << endl;
    for (vector<string>::iterator itr = subtypes.begin(); itr != subtypes.end(); ++itr) {
        fout << "\t\t<subtype name=\"" + *itr + "\"/>" << endl;
    }
    fout << "\t</relation>" << endl;
}

void GraphNew::print_all_graph_info() {
    string file_name;
    cout << "Enter the file to store all graph information" << endl;
    cin >> file_name;
    ofstream fout(file_name.c_str());

    if (fout.is_open()) {
        fout << "Graph Id: " << graph_id << endl;
        fout << "All node ids\n";
        print_vector(NIDSg, fout);
        fout << endl;
        fout << "All edge ids\n";
        print_vector(EIDSg, fout);
        fout << endl;
        fout << "Nodes Info:\n";
        for (unordered_map<int, NodeNew *>::iterator node_map_itr = nid_to_node_map.begin(); node_map_itr != nid_to_node_map.end(); ++node_map_itr) {
            fout << "node id as key: " << node_map_itr->first << endl;
            fout << "node id in NodeNew data struct: " << node_map_itr->second->get_node_id() << endl;
            fout << "rep id: " << node_map_itr->second->get_rep_id() << endl;
            vector<string> all_ids = node_map_itr->second->get_all_id();
            fout << "all ids: ";
            print_vector(all_ids, fout);
            fout << "node type: " << node_map_itr->second->get_node_type() << endl;
            vector<int> all_comps = node_map_itr->second->get_comp_ids();
            fout << "component ids: ";
            print_vector(all_comps, fout);
            vector<string> all_dis_names = node_map_itr->second->get_disp_ids();
            fout << "all dis names: ";
            print_vector(all_dis_names, fout);
            vector<int> all_inlists = node_map_itr->second->get_inlist();
            fout << "inlist: ";
            print_vector(all_inlists, fout);
            vector<int> all_outlists = node_map_itr->second->get_outlist();
            fout << "outlist: ";
            print_vector(all_outlists, fout);
            fout << endl;
        }
        fout << endl << endl;
        fout << "Edges Info:\n";
        for (unordered_map<int, EdgeNew *>::iterator edge_map_itr = eid_to_edge_map.begin(); edge_map_itr != eid_to_edge_map.end(); ++edge_map_itr) {
            fout << "edge id as key: " << edge_map_itr->first << endl;
            fout << "edge id in EdgeNew data struct: " << edge_map_itr->second->get_edge_id() << endl;
            fout << "edge source: " << edge_map_itr->second->get_source() << "(" << get_rep_id_from_nid(edge_map_itr->second->get_source()) << ")" << endl;
            fout << "edge target: " << edge_map_itr->second->get_target() << "(" << get_rep_id_from_nid(edge_map_itr->second->get_target()) << ")" << endl;
            fout << "edge type: " << edge_map_itr->second->get_edge_type() << endl;
            vector<string> edge_subtypes = edge_map_itr->second->get_edge_subtypes();
            fout << "edge subtypes: ";
            print_vector(edge_subtypes, fout);
            vector<string> edge_pathways = edge_map_itr->second->get_pathways();
            fout << "edge pathways: ";
            print_vector(edge_pathways, fout);
            fout << endl;
        }
        fout << endl << endl;
        fout << "rep_id to nid map" << endl;
        print_map(rep_id_to_nid_map, fout);
        fout << endl << endl;
        fout << "id to rep_id map" << endl;
        print_map(id_to_rep_id_map, fout);
        fout << endl << endl;
    } else {
        cerr << "File " + file_name + " couldn't be opened" << endl;
    }
}

//void GraphNew::print_all_graph_info(){
//	string file_name;
//	cout << "Enter the file to store all graph information" << endl;
//	cin >> file_name;
//	ofstream fout(file_name.c_str());
//
//	if(fout.is_open()){
//		fout << "Graph Id: " << graph_id << endl;
//		fout << "All node ids\n";
//		print_vector(NIDSg, fout);
//		fout << endl;
//		fout << "All edge ids\n";
//		print_vector(EIDSg, fout);
//		fout << endl;
//		fout << "Id to rep id map\n";
//		print_map(id_to_rep_id_map, fout);
//		fout << endl;
//		fout << "node_id_to_all_id_map\n";
//		print_map2(node_id_to_all_id_map, fout);
//		fout << endl;
//		fout << "components\n";
//		print_map2(components, fout);
//		fout << endl;
//		fout << "node_id_to_display_name_map\n";
//		print_map2(node_id_to_display_name_map, fout);
//		fout << endl;
//		fout << "node type\n";
//		print_map(node_type, fout);
//		fout << endl;
//		fout << "edge type\n";
//		print_map(edge_type, fout);
//		fout << endl;
//		fout << "edge_subtypes\n";
//		print_map2(edge_subtypes, fout);
//		fout << endl;
//		fout << "edge_belongs_to_pathways\n";
//		print_map2(edge_belongs_to_pathways, fout);
//		fout << endl;
//		fout << "edge_source map\n";
//		print_map(edge_source, fout);
//		fout << endl;
//		fout << "edge_target map\n";
//		print_map(edge_target, fout);
//		fout << endl;
//		fout << "edge_inlist map\n";
//		print_map2(inlist, fout);
//		fout << endl;
//		fout << "edge_outlist map\n";
//		print_map2(outlist, fout);
//		fout << endl;
//		fout.close();
//	}
//	else{
//		cerr << "File " + file_name + " couldn't be opened" << endl;
//	}
//}

std::set<std::string> GraphNew::get_all_target_rep_ids() {
    set<string> all_targets;
    for (vector<int>::iterator itr = EIDSg.begin(); itr != EIDSg.end(); ++itr) {
        string target_node_rid = get_rep_id_from_nid(get_target_node(*itr));
        all_targets.insert(target_node_rid);

    }
    return all_targets;
}

std::set<std::string> GraphNew::get_all_gerel_target_rep_ids() {
    set<string> gerel_targets;
    for (vector<int>::iterator itr = EIDSg.begin(); itr != EIDSg.end(); ++itr) {
        //There might be buggy KEGG xml files where type is GErel but subtype is neither expression nor repression -- such buggy GErel edges are not picked
        if (get_edge_type(*itr) == "GErel" && (edge_has_subtype(*itr, "expression") || edge_has_subtype(*itr, "repression"))) {
            string target_node_rid = get_rep_id_from_nid(get_target_node(*itr));
            gerel_targets.insert(target_node_rid);
        }
    }
    return gerel_targets;
}

void GraphNew::print_GErel_edges() {
    string filename;
    cout << "Enter file path to which to write GErel edges" << endl;
    cin >> filename;

    ofstream fout(filename.c_str());

    if (fout.is_open()) {
        for (vector<int>::iterator itr = EIDSg.begin(); itr != EIDSg.end(); ++itr) {
            if (get_edge_type(*itr) == "GErel") {
                string source_node_rid = get_rep_id_from_nid(get_source_node(*itr));
                string target_node_rid = get_rep_id_from_nid(get_target_node(*itr));
                if (edge_has_subtype(*itr, "expression")) {
                    fout << source_node_rid << " over-expresses " << target_node_rid << endl;
                } else if (edge_has_subtype(*itr, "repression")) {
                    fout << source_node_rid << " represses " << target_node_rid << endl;
                } else {
                    cerr << "Error: GErel edge which is neither expression or repression is found from " + source_node_rid + " to " + target_node_rid << endl;
                    fout << "Error: GErel edge which is neither expression or repression is found from " + source_node_rid + " to " + target_node_rid << endl;
                }
            }
        }
        fout.close();
    } else {
        cerr << "File " + filename + " couldn't be opened" << endl;
    }
}

void GraphNew::print_GErel_targets() {
    string filename;
    cout << "Enter file path to which to write GErel edges" << endl;
    cin >> filename;
    ofstream fout(filename.c_str());

    if (fout.is_open()) {
        set<string> gerel_targets = get_all_gerel_target_rep_ids();
        for (set<string>::iterator itr = gerel_targets.begin(); itr != gerel_targets.end(); ++itr) {
            fout << *itr << endl;
        }
        fout.close();
    } else {
        cerr << "File " + filename + " couldn't be opened" << endl;
    }
}


//void GraphNew::write_microarr_assert_to_file(){
//	string up_reg_filename, down_reg_filename, assert_file;
//	cout << "\nEnter file for up regulated entries: ";
//	cin >> up_reg_filename;
//	cout << "\nEnter file for down regulated entries: ";
//	cin >> down_reg_filename;
//
//	string id;
//	ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
//	set<string> up_reg_rep_ids, down_reg_rep_ids;
//	set<string> gerel_target_repids = get_all_gerel_target_rep_ids();
//	if(!ifs_up.is_open()){
//		cerr << "Error: file " + up_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
//		return;
//	}
//	if(!ifs_down.is_open()){
//		cerr << "Error: file " + down_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
//		return;
//	}
//	while (ifs_up >> id) {
//		string repid = get_rep_id_from_id(id);
//		if(repid != ""){
//			up_reg_rep_ids.insert(repid);
//		}
//	}
//	ifs_up.close();
//	while (ifs_down >> id) {
//		string repid = get_rep_id_from_id(id);
//		if(repid != ""){
//			down_reg_rep_ids.insert(repid);
//		}
//	}
//	ifs_down.close();
//
//	//Entities both up and down regulated
//	set<string> up_and_down_reg;
//	set_intersection(up_reg_rep_ids.begin(), up_reg_rep_ids.end(), down_reg_rep_ids.begin(), down_reg_rep_ids.end(), inserter(up_and_down_reg, up_and_down_reg.begin()));
//
//	if(!up_and_down_reg.empty()){
//		cout << "Note: There are " << up_and_down_reg.size() << " genes that are both upregulated and downregulated in the microarray data. Ignoring such common genes in constraint solving." << endl;
//	}
//
//	//remove such common entries
//	set<string> up_reg_minus_common, down_reg_minus_common;
//	set_difference(up_reg_rep_ids.begin(), up_reg_rep_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), inserter(up_reg_minus_common, up_reg_minus_common.begin()));
//	set_difference(down_reg_rep_ids.begin(), down_reg_rep_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), inserter(down_reg_minus_common, down_reg_minus_common.begin()));
//
//	// Entities up regulated common with graph GErel targets
//	set<string> up_reg_in_graph;
//	set_intersection(up_reg_minus_common.begin(), up_reg_minus_common.end(), gerel_target_repids.begin(), gerel_target_repids.end(), inserter(up_reg_in_graph, up_reg_in_graph.begin()));
//
//	// Entities down regulated common with graph
//	set<string> down_reg_in_graph;
//	set_intersection(down_reg_minus_common.begin(), down_reg_minus_common.end(), gerel_target_repids.begin(), gerel_target_repids.end(), inserter(down_reg_in_graph, down_reg_in_graph.begin()));
//
//	//Entries not differentially expressed in the graph
//	set<string> graph_unchanged_entities, temp;
//	set_difference(gerel_target_repids.begin(), gerel_target_repids.end(), up_reg_in_graph.begin(), up_reg_in_graph.end(), inserter(temp, temp.begin()));
//	set_difference(temp.begin(), temp.end(), down_reg_in_graph.begin(), down_reg_in_graph.end(), inserter(graph_unchanged_entities, graph_unchanged_entities.end()));
//
//	cout << "\nEnter the file to which to write microarray nodes assertions: ";
//	cin >> assert_file;
//
//	ofstream fout_assert(assert_file.c_str());
//	if(fout_assert.is_open()){
//		set<string>::iterator itr_set;
//		// up regulated nodes present in the graph
//		for(itr_set = up_reg_in_graph.begin(); itr_set != up_reg_in_graph.end(); itr_set++){
//			fout_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
//			fout_assert << "(assert " << (*itr_set) << "_over_expressed)" << endl;
//		}
//
//		// down regulated nodes present in the graph
//		for(itr_set = down_reg_in_graph.begin(); itr_set != down_reg_in_graph.end(); itr_set++){
//			fout_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
//			fout_assert << "(assert (not " << (*itr_set) << "_over_expressed))" << endl;
//		}
//
//		// unchanged nodes in the graph
//		for(itr_set = graph_unchanged_entities.begin(); itr_set != graph_unchanged_entities.end(); itr_set++){
//			fout_assert << "(assert (not " << (*itr_set) << "_expression_changes))" << endl;
//		}
//
//		fout_assert.close();
//	}
//	else{
//		cerr << "Error: file " + assert_file + " can't be opened, returning without any microarray assertions..." << endl;
//		return;
//	}
//}



void GraphNew::write_microarr_assert_to_file(){
        string up_reg_filename, down_reg_filename, assert_file;
        cout << "\nEnter file for up regulated entries: ";
        cin >> up_reg_filename;
        cout << "\nEnter file for down regulated entries: ";
        cin >> down_reg_filename;

        string id;
        ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
        set<string> up_reg_rep_ids, down_reg_rep_ids;
        set<string> gerel_target_repids = get_all_gerel_target_rep_ids();
        //set<string> all_rep_ids_in_graph = gerel_target_repids;
        set<string> all_rep_ids_in_graph = get_all_target_rep_ids();
        // temporary -- 
//        gerel_target_repids.clear();
//        gerel_target_repids = all_rep_ids_in_graph;
//        cout << gerel_target_repids.size() << endl;
        // --
        if(!ifs_up.is_open()){
                cerr << "Error: file " + up_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
                return;
        }
        if(!ifs_down.is_open()){
                cerr << "Error: file " + down_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
                return;
        }
        while (ifs_up >> id) {
                string repid = get_rep_id_from_id(id);
                if(repid != ""){
                        up_reg_rep_ids.insert(repid);
                }
        }
        ifs_up.close();
        while (ifs_down >> id) {
                string repid = get_rep_id_from_id(id);
                if(repid != ""){
                        down_reg_rep_ids.insert(repid);
                }
        }
        ifs_down.close();

        //Entities both up and down regulated
        set<string> up_and_down_reg;
        set_intersection(up_reg_rep_ids.begin(), up_reg_rep_ids.end(), down_reg_rep_ids.begin(), down_reg_rep_ids.end(), inserter(up_and_down_reg, up_and_down_reg.begin()));

        if(!up_and_down_reg.empty()){
                cout << "Note: There are " << up_and_down_reg.size() << " genes that are both upregulated and downregulated in the microarray data. Ignoring such common genes in constraint solving." << endl;
        }

        //remove such common entries
        set<string> up_reg_minus_common, down_reg_minus_common;
        set_difference(up_reg_rep_ids.begin(), up_reg_rep_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), inserter(up_reg_minus_common, up_reg_minus_common.begin()));
        set_difference(down_reg_rep_ids.begin(), down_reg_rep_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), inserter(down_reg_minus_common, down_reg_minus_common.begin()));
        cout << "Up regulated genes: " << up_reg_minus_common.size() << endl;
        cout << "Down regulated genes: " << down_reg_minus_common.size() << endl;
        
        // Entities up and down regulated common with graph
        set<string> all_up_reg_in_graph, all_down_reg_in_graph;
        set_intersection(up_reg_minus_common.begin(), up_reg_minus_common.end(), all_rep_ids_in_graph.begin(), all_rep_ids_in_graph.end(), inserter(all_up_reg_in_graph, all_up_reg_in_graph.begin()));
        set_intersection(down_reg_minus_common.begin(), down_reg_minus_common.end(), all_rep_ids_in_graph.begin(), all_rep_ids_in_graph.end(), inserter(all_down_reg_in_graph, all_down_reg_in_graph.begin()));
        cout << "Up regulated as any target: " << all_up_reg_in_graph.size() << endl;
        cout << "Down regulated as any target: " << all_down_reg_in_graph.size() << endl;
        
        // Entities up regulated common with graph GErel targets
        set<string> up_reg_in_graph;
        set_intersection(up_reg_minus_common.begin(), up_reg_minus_common.end(), gerel_target_repids.begin(), gerel_target_repids.end(), inserter(up_reg_in_graph, up_reg_in_graph.begin()));

        // Entities down regulated common with graph
        set<string> down_reg_in_graph;
        set_intersection(down_reg_minus_common.begin(), down_reg_minus_common.end(), gerel_target_repids.begin(), gerel_target_repids.end(), inserter(down_reg_in_graph, down_reg_in_graph.begin()));
        cout << "Up regulated as GErel targets: " << up_reg_in_graph.size() << endl;
        set<string>::iterator temp_itr;
        for(temp_itr = up_reg_in_graph.begin(); temp_itr != up_reg_in_graph.end(); temp_itr++){
            cout << (*temp_itr) << " ";
        }
        cout << endl;
        cout << "Down regulated as GErel targets: " << down_reg_in_graph.size() << endl;
        
        //Entries not differentially expressed in the graph
        set<string> graph_unchanged_entities, temp;
        set_difference(gerel_target_repids.begin(), gerel_target_repids.end(), up_reg_in_graph.begin(), up_reg_in_graph.end(), inserter(temp, temp.begin()));
        //set_difference(all_rep_ids_in_graph.begin(), all_rep_ids_in_graph.end(), up_reg_in_graph.begin(), up_reg_in_graph.end(), inserter(temp, temp.begin()));
        set_difference(temp.begin(), temp.end(), down_reg_in_graph.begin(), down_reg_in_graph.end(), inserter(graph_unchanged_entities, graph_unchanged_entities.end()));

        cout << "\nEnter the file to which to write microarray nodes assertions: ";
        cin >> assert_file;

        ofstream fout_assert(assert_file.c_str());
        if(fout_assert.is_open()){
                set<string>::iterator itr_set;
                // up regulated nodes present in the graph
                for(itr_set = up_reg_in_graph.begin(); itr_set != up_reg_in_graph.end(); itr_set++){
                        fout_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
                        fout_assert << "(assert " << (*itr_set) << "_over_expressed)" << endl;
                }

                // down regulated nodes present in the graph
                for(itr_set = down_reg_in_graph.begin(); itr_set != down_reg_in_graph.end(); itr_set++){
                        fout_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
                        fout_assert << "(assert (not " << (*itr_set) << "_over_expressed))" << endl;
                }

                // unchanged nodes in the graph
                for(itr_set = graph_unchanged_entities.begin(); itr_set != graph_unchanged_entities.end(); itr_set++){
                        fout_assert << "(assert (not " << (*itr_set) << "_expression_changes))" << endl;
                }

                fout_assert.close();
        }
        else{
                cerr << "Error: file " + assert_file + " can't be opened, returning without any microarray assertions..." << endl;
                return;
        }
}
 

/*
void GraphNew::write_microarr_assert_to_file() {
    string up_reg_filename, down_reg_filename, assert_file;
    cout << "\nEnter file for up regulated entries: ";
    cin >> up_reg_filename;
    cout << "\nEnter file for down regulated entries: ";
    cin >> down_reg_filename;

    string id;
    ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
    set<string> up_reg_rep_ids, down_reg_rep_ids;
    set<string> gerel_target_repids = get_all_gerel_target_rep_ids();
    if (!ifs_up.is_open()) {
        cerr << "Error: file " + up_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
        return;
    }
    if (!ifs_down.is_open()) {
        cerr << "Error: file " + down_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
        return;
    }
    while (ifs_up >> id) {
        string repid = get_rep_id_from_id(id);
        if (repid != "") {
            up_reg_rep_ids.insert(repid);
        }
    }
    ifs_up.close();
    while (ifs_down >> id) {
        string repid = get_rep_id_from_id(id);
        if (repid != "") {
            down_reg_rep_ids.insert(repid);
        }
    }
    ifs_down.close();

    //Entities both up and down regulated
    set<string> up_and_down_reg;
    set_intersection(up_reg_rep_ids.begin(), up_reg_rep_ids.end(), down_reg_rep_ids.begin(), down_reg_rep_ids.end(), inserter(up_and_down_reg, up_and_down_reg.begin()));

    if (!up_and_down_reg.empty()) {
        cout << "Note: There are " << up_and_down_reg.size() << " genes that are both upregulated and downregulated in the microarray data. Ignoring such common genes in constraint solving." << endl;
    }

    //remove such common entries
    set<string> up_reg_minus_common, down_reg_minus_common;
    set_difference(up_reg_rep_ids.begin(), up_reg_rep_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), inserter(up_reg_minus_common, up_reg_minus_common.begin()));
    set_difference(down_reg_rep_ids.begin(), down_reg_rep_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), inserter(down_reg_minus_common, down_reg_minus_common.begin()));

    // Entities up regulated common with graph GErel targets
    set<string> up_reg_in_graph;
    set_intersection(up_reg_minus_common.begin(), up_reg_minus_common.end(), gerel_target_repids.begin(), gerel_target_repids.end(), inserter(up_reg_in_graph, up_reg_in_graph.begin()));
    
    // Entities down regulated common with graph
    set<string> down_reg_in_graph;
    set_intersection(down_reg_minus_common.begin(), down_reg_minus_common.end(), gerel_target_repids.begin(), gerel_target_repids.end(), inserter(down_reg_in_graph, down_reg_in_graph.begin()));

    //Entries not differentially expressed in the graph
    set<string> graph_unchanged_entities, temp;
    set_difference(gerel_target_repids.begin(), gerel_target_repids.end(), up_reg_in_graph.begin(), up_reg_in_graph.end(), inserter(temp, temp.begin()));
    set_difference(temp.begin(), temp.end(), down_reg_in_graph.begin(), down_reg_in_graph.end(), inserter(graph_unchanged_entities, graph_unchanged_entities.end()));

    cout << "\nEnter the file to which to write microarray nodes assertions: ";
    cin >> assert_file;

    ofstream fout_assert(assert_file.c_str());
    if (fout_assert.is_open()) {
        set<string>::iterator itr_set;
        // up regulated nodes present in the graph
        for (itr_set = up_reg_in_graph.begin(); itr_set != up_reg_in_graph.end(); itr_set++) {
            fout_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
            fout_assert << "(assert " << (*itr_set) << "_over_expressed)" << endl;
        }

        // down regulated nodes present in the graph
        for (itr_set = down_reg_in_graph.begin(); itr_set != down_reg_in_graph.end(); itr_set++) {
            fout_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
            fout_assert << "(assert (not " << (*itr_set) << "_over_expressed))" << endl;
        }

        // unchanged nodes in the graph
        for (itr_set = graph_unchanged_entities.begin(); itr_set != graph_unchanged_entities.end(); itr_set++) {
            fout_assert << "(assert (not " << (*itr_set) << "_expression_changes))" << endl;
        }

        fout_assert.close();
    } else {
        cerr << "Error: file " + assert_file + " can't be opened, returning without any microarray assertions..." << endl;
        return;
    }
}
*/

void GraphNew::write_microarr_decl_to_file() {
    string decl_file;
    cout << "Enter the file to which to write microarray nodes declarations: ";
    cin >> decl_file;
    ofstream fout(decl_file.c_str());

    if (fout.is_open()) {
        ////set<string> gerel_targets = get_all_gerel_target_rep_ids();
        set<string> gerel_targets = get_all_target_rep_ids();
        for (set<string>::iterator itr = gerel_targets.begin(); itr != gerel_targets.end(); ++itr) {

            //int node_id = get_nid_from_rep_id(*itr);
            //if(get_node_type(node_id) != "group"){//this check is temporary - groups can't be seen in microarray data but still we can do something about it later
            string decl1 = "(declare-const " + *itr + "_expression_changes Bool)";
            string decl2 = "(declare-const " + *itr + "_over_expressed Bool)";
            fout << decl1 << endl;
            fout << decl2 << endl;
            //}
        }
        fout.close();
    } else {
        cerr << "Error: can't open file " + decl_file + " for writing microarray declarations\n";
    }
}

//adds edges from components of a complex to the complex with edge subtype component

void GraphNew::add_component_edges_for_complexes_sbml(GraphManagerNew* gm) {
    for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
        NodeNew * node = get_node_from_nid(*itr);
        if (node->get_node_type() == "") {
            std::vector<int> components = node->get_comp_ids();
            std::vector<int>::iterator comp_itr;
            for (comp_itr = components.begin(); comp_itr != components.end(); comp_itr++) {
                int source_node_id = *comp_itr;
                int target_node_id = *itr;
                std::string type = "";
                vector<string> subtype;
                std::string curr_subtype = "component";
                subtype.push_back(curr_subtype);
                std::string pathway = "";
                int eid = check_if_edge_already_created(source_node_id, target_node_id, type, subtype);
                if (eid == -1) {
                    eid = create_new_edge();
                    add_edge_id(eid);
                    gm->add_edge_id_graph_id(eid, get_graph_id());
                    add_edge_type(eid, type);
                    add_edge_to_outlist_of_node(source_node_id, eid);
                    add_edge_to_inlist_of_node(target_node_id, eid);
                    add_source_node(eid, source_node_id);
                    add_target_node(eid, target_node_id);
                    for (vector<string>::iterator subt_itr = subtype.begin(); subt_itr != subtype.end(); subt_itr++) {
                        add_subtype_for_edge(eid, *subt_itr);
                    }
                    add_pathway_for_edge(eid, pathway);
                }
            }
        }
    }
}

//adds edges from components of a complex to the complex with edge subtype component

void GraphNew::add_component_edges_for_complexes(GraphManagerNew* gm) {
    for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
        NodeNew * node = get_node_from_nid(*itr);
        if (node->get_node_type() == "group") {
            std::vector<int> components = node->get_comp_ids();
            std::vector<int>::iterator comp_itr;
            for (comp_itr = components.begin(); comp_itr != components.end(); comp_itr++) {
                int source_node_id = *comp_itr;
                int target_node_id = *itr;
                std::string type = "PPrel";
                vector<string> subtype;
                std::string curr_subtype = "component";
                subtype.push_back(curr_subtype);
                std::vector<std::string> all_pathways = get_all_pathways_for_edge(*(get_edge_ids().begin()));
                int eid = check_if_edge_already_created(source_node_id, target_node_id, type, subtype);
                if (eid == -1) {
                    eid = create_new_edge();
                    add_edge_id(eid);
                    gm->add_edge_id_graph_id(eid, get_graph_id());
                    add_edge_type(eid, type);
                    add_edge_to_outlist_of_node(source_node_id, eid);
                    add_edge_to_inlist_of_node(target_node_id, eid);
                    add_source_node(eid, source_node_id);
                    add_target_node(eid, target_node_id);
                    for (vector<string>::iterator subt_itr = subtype.begin(); subt_itr != subtype.end(); subt_itr++) {
                        add_subtype_for_edge(eid, *subt_itr);
                    }
                    for (std::vector<std::string>::iterator path_itr = all_pathways.begin(); path_itr != all_pathways.end(); path_itr++){
                        add_pathway_for_edge(eid, *path_itr);
                    }
                }
                eid = check_if_edge_already_created(target_node_id, source_node_id, type, subtype);
                if (eid == -1) {
                    eid = create_new_edge();
                    add_edge_id(eid);
                    gm->add_edge_id_graph_id(eid, get_graph_id());
                    add_edge_type(eid, type);
                    add_edge_to_outlist_of_node(target_node_id, eid);
                    add_edge_to_inlist_of_node(source_node_id, eid);
                    add_source_node(eid, target_node_id);
                    add_target_node(eid, source_node_id);
                    for (vector<string>::iterator subt_itr = subtype.begin(); subt_itr != subtype.end(); subt_itr++) {
                        add_subtype_for_edge(eid, *subt_itr);
                    }
                    for (std::vector<std::string>::iterator path_itr = all_pathways.begin(); path_itr != all_pathways.end(); path_itr++){
                        add_pathway_for_edge(eid, *path_itr);
                    }
                    
                }
            }
        }
    }
}

//includes edges whose removal can remove cycle
//includes all assoc-dissoc edges because they exist in pairs and thus always cyclic

set<int> GraphNew::edges_to_remove_cycles(vector<string>& source_nodes_set) {
    set<int> edges_to_remove;
    for (vector<string>::iterator itr = source_nodes_set.begin(); itr != source_nodes_set.end(); itr++) {
        string rid = get_rep_id_from_id(*itr);
        if (rid == "") {
            cerr << "Error: one of the source nodes " + *itr << " has no rep id in the graph, ignoring..." << endl;
            continue;
        }
        int nid = get_nid_from_rep_id(rid);
#ifdef ASSERT_FLAG
        assert(nid != -1);
#endif
        if (nid == -1) {
            cerr << "Error: one of the source nodes " + *itr << " has a rep id but no node id for it in the graph.\n You have made a bug in the code -- fix it" << endl;
            exit(1);
        }
        set<int> grey_nodes, black_nodes; //grey_nodes can be thought of as currently in the stack of DFS, while black_nodes are the finally explored nodes
        get_edges_to_remove_cycles_DFS(nid, grey_nodes, black_nodes, edges_to_remove);
    }
    return edges_to_remove;
}

set<int> GraphNew::edges_to_remove_cycles_backwards(vector<string>& target_nodes_set) {
    set<int> edges_to_remove;
    for (vector<string>::iterator itr = target_nodes_set.begin(); itr != target_nodes_set.end(); itr++) {
        string rid = get_rep_id_from_id(*itr);
        if (rid == "") {
            cerr << "Error: one of the source nodes " + *itr << " has no rep id in the graph, ignoring..." << endl;
        }
        int nid = get_nid_from_rep_id(rid);
        if (nid == -1) {
            cerr << "Error: one of the source nodes " + *itr << " has a rep id but no node id for it in the graph.\n You have made a bug in the code -- fix it" << endl;
            exit(1);
        }
        set<int> grey_nodes, black_nodes; //grey_nodes can be thought of as currently in the stack of DFS, while black_nodes are the finally explored nodes
        get_edges_to_remove_cycles_DFS_backwards(nid, grey_nodes, black_nodes, edges_to_remove);
    }
    return edges_to_remove;
}

//grey_nodes can be thought of as currently in the stack of DFS, while black_nodes are the finally explored nodes
//includes edges whose removal can remove cycle
//includes all assoc-dissoc edges because they exist in pairs and thus always cyclic

void GraphNew::get_edges_to_remove_cycles_DFS(int nid, set<int> &grey_nodes, set<int> &black_nodes, set<int>& edges_to_remove) {
    grey_nodes.insert(nid);
    vector<int> out_list = get_outlist(nid);
    for (vector<int>::iterator out_itr = out_list.begin(); out_itr != out_list.end(); ++out_itr) {
        int target_nid = get_target_node(*out_itr);
        bool is_assoc = edge_has_subtype(*out_itr, "association");
        bool is_dissoc = edge_has_subtype(*out_itr, "dissociation");
        bool is_mapping = edge_has_subtype(*out_itr, "mapping");
        //grey node case (cycle found)
        if (grey_nodes.find(target_nid) != grey_nodes.end()) {
            edges_to_remove.insert(*out_itr);
            //if this is assoc or dissoc edge then the there must be its pair in opposite direction - that must be included as well
            if (is_assoc || is_dissoc || is_mapping) {
                vector<int> target_out_list = get_outlist(target_nid);
                for (vector<int>::iterator target_out_itr = target_out_list.begin(); target_out_itr != target_out_list.end(); ++target_out_itr) {
                    if (get_target_node(*target_out_itr) == nid) {
                        if ((is_assoc && edge_has_subtype(*target_out_itr, "association")) || (is_dissoc && edge_has_subtype(*target_out_itr, "dissociation")) || (is_dissoc && edge_has_subtype(*target_out_itr, "mapping"))) {
                            edges_to_remove.insert(*target_out_itr);
                        }
                    }
                }
            }
            //cout << "cycle complete with edge from " << get_rep_id_from_nid(nid) << "to " << get_rep_id_from_nid(target_nid) << endl;
        }            //black node case
        else if (black_nodes.find(target_nid) != black_nodes.end()) {
            //do nothing for black node
        }            //white node case
        else {
            get_edges_to_remove_cycles_DFS(target_nid, grey_nodes, black_nodes, edges_to_remove);
        }
    }
    //grey node becomes a black node
    grey_nodes.erase(nid);
    black_nodes.insert(nid);
}

void GraphNew::get_edges_to_remove_cycles_DFS_backwards(int nid, set<int> &grey_nodes, set<int> &black_nodes, set<int>& edges_to_remove) {
    grey_nodes.insert(nid);
    vector<int> in_list = get_inlist(nid);
    for (vector<int>::iterator in_itr = in_list.begin(); in_itr != in_list.end(); ++in_itr) {
        int source_nid = get_source_node(*in_itr);
        bool is_assoc = edge_has_subtype(*in_itr, "association");
        bool is_dissoc = edge_has_subtype(*in_itr, "dissociation");
        bool is_mapping = edge_has_subtype(*in_itr, "mapping");
        //grey node case (cycle found)
        if (grey_nodes.find(source_nid) != grey_nodes.end()) {
            edges_to_remove.insert(*in_itr);
            //if this is assoc or dissoc edge then the there must be its pair in opposite direction - that must be included as well
            if (is_assoc || is_dissoc || is_mapping) {
                vector<int> source_out_list = get_inlist(source_nid);
                for (vector<int>::iterator source_out_itr = source_out_list.begin(); source_out_itr != source_out_list.end(); ++source_out_itr) {
                    if (get_source_node(*source_out_itr) == nid) {
                        if ((is_assoc && edge_has_subtype(*source_out_itr, "association")) || (is_dissoc && edge_has_subtype(*source_out_itr, "dissociation")) || (is_dissoc && edge_has_subtype(*source_out_itr, "mapping"))) {
                            edges_to_remove.insert(*source_out_itr);
                        }
                    }
                }
            }
            //cout << "cycle complete with edge from " << get_rep_id_from_nid(nid) << "to " << get_rep_id_from_nid(target_nid) << endl;
        }            //black node case
        else if (black_nodes.find(source_nid) != black_nodes.end()) {
            //do nothing for black node
        }            //white node case
        else {
            get_edges_to_remove_cycles_DFS(source_nid, grey_nodes, black_nodes, edges_to_remove);
        }
    }
    //grey node becomes a black node
    grey_nodes.erase(nid);
    black_nodes.insert(nid);
}

void GraphNew::remove_edge(int eid) {
    EdgeNew* edge = get_edge_from_eid(eid);
    if(edge == NULL){
            cerr << "Error: no edge found with edge id " << eid << endl;
            return;
    }
    int source_id = get_source_node(eid);
    int target_id = get_target_node(eid);
    NodeNew* source_node = get_node_from_nid(source_id);
    NodeNew* target_node = get_node_from_nid(target_id);
    source_node->remove_from_outlist(eid);
    target_node->remove_from_inlist(eid);
    eid_to_edge_map.erase(eid);
    // EIDSg.erase(eid); -- replaced by
    EIDSg.erase(std::find(EIDSg.begin(), EIDSg.end(),eid));
    
    delete edge;
}

//returns bit sequence of expressions representing distance of a node

std::vector<t_Expression*> GraphNew::get_node_distance_expression_sequence(int nid, t_ExpressionManager* em, GraphManagerNew* gm) {
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};
    vector<t_Expression*> to_return(NODE_DISTANCE_BIT_SEQ_LENGTH, NULL); //distance variable is bit sequence of length NODE_DISTANCE_BIT_SEQ_LENGTH
    string rid = get_rep_id_from_nid(nid);
    if (rid == "") {
        cerr << "Error: No node of id " << nid << " found, exiting..." << endl;
        exit(1);
    } else {
        //give variables has distance_<rid>_<i>, with i from 0 to NODE_DISTANCE_BIT_SEQ_LENGTH-1 as bits in the distance variables
        for (int i = 0; i < NODE_DISTANCE_BIT_SEQ_LENGTH; ++i) {
            string node_distance_bit_var_indexed = "distance_" + rid + "_" + IntToString(i); //this is the variable name
            t_Expression* expr_node_distance_bit_var_indexed = em->createSymbol(node_distance_bit_var_indexed, te1);
            getExpressionMap[node_distance_bit_var_indexed] = expr_node_distance_bit_var_indexed;
            to_return[i] = expr_node_distance_bit_var_indexed;
            getCNFIndexMap[expr_node_distance_bit_var_indexed] = make_pair(node_distance_bit_var_indexed, -1);
        }
    }
    return to_return;
}

bool GraphNew::is_isolated_node(int nid) {
    bool to_return = false;
    NodeNew* node = get_node_from_nid(nid);

    if (node) {
        to_return = (node->get_in_degree() == 0) && (node->get_out_degree() == 0);
    }

    return to_return;
}

void GraphNew::print_nodes_rids_with_no_in_edges(std::ostream &fout) {
    for (vector<int>::iterator nid_itr = NIDSg.begin(); nid_itr != NIDSg.end(); nid_itr++) {
        vector<int> inlist = get_inlist(*nid_itr);
        if (inlist.empty()) {
            string rid = get_rep_id_from_nid(*nid_itr);
            fout << rid << endl;
        }
    }
}

//the vectors source_node_distance_expr_vec and target_node_distance_expr_vec must have the expressions for the bits of the distance, adn the expression_true_bit corresponds to the one bit and must already have been among the aserted constraints
//overflow will give UNSAT and cab checked by reading variables

t_Expression* GraphNew::get_constraint_node_distance_differ_by_one(vector<t_Expression*>& source_node_distance_expr_vec, vector<t_Expression*>& target_node_distance_expr_vec, t_Expression* expression_true_bit, t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    assert(source_node_distance_expr_vec.size() == NODE_DISTANCE_BIT_SEQ_LENGTH);
    assert(target_node_distance_expr_vec.size() == NODE_DISTANCE_BIT_SEQ_LENGTH);

    //asserting target_dist = source_dist + 1 at bit level
    t_Expression* current_carry = expression_true_bit;
    for (int i = 0; i < NODE_DISTANCE_BIT_SEQ_LENGTH; ++i) {
        t_Expression* current_sum = build_XOR_constraints(source_node_distance_expr_vec[i], current_carry, em);
        t_Expression* current_sum_equals_target_distance = build_EQUIV_constraint(current_sum, target_node_distance_expr_vec[i], em);
        to_AND.push_back(current_sum_equals_target_distance);
        t_Expression* temp = build_AND_constraints(source_node_distance_expr_vec[i], current_carry, 0, 0, em);
        current_carry = temp;
    }

    //overflow can be detected by asserting current_carry to false here
    t_Expression* carry_no_overflow = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, current_carry);
    string distance_no_overflow = "distance_no_over_flow";
    t_Expression* expr_distance_no_overflow = em->createSymbol(distance_no_overflow, te1);
    getCNFIndexMap[expr_distance_no_overflow] = make_pair(distance_no_overflow, -1);
    getExpressionMap[distance_no_overflow] = expr_distance_no_overflow;

    t_Expression* dist_no_over_flow_equiv_carry_no_over_flow = build_EQUIV_constraint(carry_no_overflow, expr_distance_no_overflow, em);

    to_AND.push_back(dist_no_over_flow_equiv_carry_no_over_flow);

    to_return = build_AND_constraints_from_vector(to_AND, em);
    return to_return;
}

//nid_distanc_var_map must have vectors assigned for all nodes

t_Expression* GraphNew::get_constraints_distance_source_target_for_visible_edge(int source_nid, map<int, vector<t_Expression*> >& nid_distanc_var_map, t_Expression* expression_true_bit, t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    for (vector<int>::iterator edge_itr = EIDSg.begin(); edge_itr != EIDSg.end(); ++edge_itr) {
        int source_id = get_source_node(*edge_itr);
        int target_id = get_target_node(*edge_itr);

        if (target_id != source_nid) {//the perturbed node can't be at one more distance than any other node

            string repid_source_node = get_rep_id_from_nid(source_id);
            string repid_target_node = get_rep_id_from_nid(target_id);
            string edge_type_str = get_edge_type(*edge_itr);
            vector<string> edge_subtype_vec_str = get_edge_subtypes_of_edge(*edge_itr);
            string edge_subtypes_str = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str);

            string edge_variable_visible = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "visible";

            t_Expression* expression_edge_visible = NULL;

            if (getExpressionMap.find(edge_variable_visible) == getExpressionMap.end()) {
                expression_edge_visible = em->createSymbol(edge_variable_visible, te1);
                getExpressionMap[edge_variable_visible] = expression_edge_visible;
                getCNFIndexMap[expression_edge_visible] = make_pair(edge_variable_visible, -1);
            } else {
                expression_edge_visible = getExpressionMap[edge_variable_visible];
            }

            vector<t_Expression*> source_node_distance_expr_vec = nid_distanc_var_map[source_id];
            vector<t_Expression*> target_node_distance_expr_vec = nid_distanc_var_map[target_id];

            t_Expression* expr_source_distance_one_less_target = get_constraint_node_distance_differ_by_one(source_node_distance_expr_vec, target_node_distance_expr_vec, expression_true_bit, em, gm);

            t_Expression* expr_visible_edge_implies_edge_difference = build_OR_constraints(expression_edge_visible, expr_source_distance_one_less_target, 1, 0, em);

            //testing starts
            /*string naming = "distance_source_target_for_visible_edge_" + repid_source_node + "-to-" + repid_target_node;
    ofstream test_fout(naming.c_str());
    em->printExpressionToFileAsDAG(naming, expr_visible_edge_implies_edge_difference, test_fout);
    test_fout.close();*/
            //testing ends

            to_AND.push_back(expr_visible_edge_implies_edge_difference);
            //cout << "Writing distance constraint for edge id " << *edge_itr << " between " + repid_source_node + " and " + repid_target_node << endl;
        }
    }

    to_return = build_AND_constraints_from_vector(to_AND, em);
    return to_return;
}

t_Expression* GraphNew::get_constraints_expr_visible_in_if_out_non_source_node(int source_nid, t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    for (vector<int>::iterator nid_itr = NIDSg.begin(); nid_itr != NIDSg.end(); nid_itr++) {
        vector<int> outlist = get_outlist(*nid_itr);
        vector<int> inlist = get_inlist(*nid_itr);

        if (*nid_itr != source_nid && !outlist.empty() && !inlist.empty()) {
            //get ORed for inlist edges
            vector<t_Expression*> to_OR;
            for (vector<int>::iterator inlist_itr = inlist.begin(); inlist_itr != inlist.end(); inlist_itr++) {
                int source_id = get_source_node(*inlist_itr);
                string repid_source_node = get_rep_id_from_nid(source_id);
                string repid_target_node = get_rep_id_from_nid(*nid_itr);
                string edge_type_str = get_edge_type(*inlist_itr);
                vector<string> edge_subtype_vec_str = get_edge_subtypes_of_edge(*inlist_itr);
                string edge_subtypes_str = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str);
                string edge_variable_visible = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "visible";

                t_Expression* expression_edge_visible = NULL;
                if (getExpressionMap.find(edge_variable_visible) == getExpressionMap.end()) {
                    expression_edge_visible = em->createSymbol(edge_variable_visible, te1);
                    getExpressionMap[edge_variable_visible] = expression_edge_visible;
                    getCNFIndexMap[expression_edge_visible] = make_pair(edge_variable_visible, -1);
                } else {
                    expression_edge_visible = getExpressionMap[edge_variable_visible];
                }

                if (expression_edge_visible) {
                    to_OR.push_back(expression_edge_visible);
                } else {
                    cerr << "Error: expression for edge visible is NULL!" << endl;
                }
            }


            t_Expression* expr_atleast_one_in_edge_if_out = build_OR_constraints_from_vector(to_OR, em);


            for (vector<int>::iterator outlist_itr = outlist.begin(); outlist_itr != outlist.end(); outlist_itr++) {
                //now for visibility of each of these outlist edges one of the those from inlist must be visible
                int target_id = get_target_node(*outlist_itr);
                string repid_source_node = get_rep_id_from_nid(*nid_itr);
                string repid_target_node = get_rep_id_from_nid(target_id);
                string edge_type_str = get_edge_type(*outlist_itr);
                vector<string> edge_subtype_vec_str = get_edge_subtypes_of_edge(*outlist_itr);
                string edge_subtypes_str = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str);
                string edge_variable_visible = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "visible";

                t_Expression* expression_edge_visible = NULL;
                if (getExpressionMap.find(edge_variable_visible) == getExpressionMap.end()) {
                    expression_edge_visible = em->createSymbol(edge_variable_visible, te1);
                    getExpressionMap[edge_variable_visible] = expression_edge_visible;
                    getCNFIndexMap[expression_edge_visible] = make_pair(edge_variable_visible, -1);
                } else {
                    expression_edge_visible = getExpressionMap[edge_variable_visible];
                }

                //now make the implication

                t_Expression* out_implies_at_least_one_in = build_OR_constraints(expression_edge_visible, expr_atleast_one_in_edge_if_out, 1, 0, em);
                if (out_implies_at_least_one_in) {
                    to_AND.push_back(out_implies_at_least_one_in);
                } else {
                    cerr << "Error: expression for out edge visible implies at least one in edge visible is NULL!" << endl;
                }
            }
        }
    }

    to_return = build_AND_constraints_from_vector(to_AND, em);
    return to_return;
}

//At least one GErel with expr/repr edge  incoming edge to a differentially expressed node is visible.
//this function assumes that the set of differentially expressed nodes passed as arguments are indeed there in the graph as GErel targets with expression or repression

t_Expression* GraphNew::get_constraint_expr_visible_incoming_edge_to_diff_expr_node(set<int>& differentially_expressed_nids, t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    for (set<int>::iterator diff_nodes_itr = differentially_expressed_nids.begin(); diff_nodes_itr != differentially_expressed_nids.end(); ++diff_nodes_itr) {
        vector<int> inlist_diff_node = get_inlist(*diff_nodes_itr);
        vector<t_Expression*> to_OR;

        for (vector<int>::iterator edge_itr = inlist_diff_node.begin(); edge_itr != inlist_diff_node.end(); edge_itr++) {
            if (get_edge_type(*edge_itr) == "GErel" && (edge_has_subtype(*edge_itr, "expression") || edge_has_subtype(*edge_itr, "repression"))) {
                int source_id = get_source_node(*edge_itr);
                string repid_source_node = get_rep_id_from_nid(source_id);
                string repid_target_node = get_rep_id_from_nid(*diff_nodes_itr);
                string edge_type_str = get_edge_type(*edge_itr);
                vector<string> edge_subtype_vec_str = get_edge_subtypes_of_edge(*edge_itr);
                string edge_subtypes_str = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str);
                string edge_variable_visible = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "visible";

                t_Expression* expression_edge_visible = NULL;
                if (getExpressionMap.find(edge_variable_visible) == getExpressionMap.end()) {
                    expression_edge_visible = em->createSymbol(edge_variable_visible, te1);
                    getExpressionMap[edge_variable_visible] = expression_edge_visible;
                    getCNFIndexMap[expression_edge_visible] = make_pair(edge_variable_visible, -1);
                } else {
                    expression_edge_visible = getExpressionMap[edge_variable_visible];
                }

                if (expression_edge_visible) {
                    to_OR.push_back(expression_edge_visible);
                } else {
                    cerr << "Error: expression for edge visible is NULL?" << endl;
                }
            }
        }

        t_Expression* expr_atleast_one_in_edge_for_diff_node = build_OR_constraints_from_vector(to_OR, em);
        if (expr_atleast_one_in_edge_for_diff_node) {
            to_AND.push_back(expr_atleast_one_in_edge_for_diff_node);
        } else {
            cerr << "Error: no atleast one in edge visible constraint for the diff node?" << endl;
        }
    }

    to_return = build_AND_constraints_from_vector(to_AND, em);
    return to_return;
}

t_Expression* GraphNew::get_zero_distance_constraint_for_node(int nid, map<int, vector<t_Expression*> >& nid_distanc_var_map, t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> nid_dist_variable_seq;
    //if the variables to represent the bit sequence for node is not present then make it, other wise use it
    if (nid_distanc_var_map.find(nid) == nid_distanc_var_map.end()) {
        nid_dist_variable_seq = get_node_distance_expression_sequence(nid, em, gm);
        nid_distanc_var_map[nid] = nid_dist_variable_seq;
    } else {
        nid_dist_variable_seq = nid_distanc_var_map[nid];
    }

    //for distance to to be zero, all bits must be zero , so ORing all bits must generate zero
    t_Expression* temp = build_OR_constraints_from_vector(nid_dist_variable_seq, em);
    to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, temp); //negating

    return to_return;
}

t_Expression* GraphNew::get_positive_distance_constrains_for_non_source_nodes(int source_nid, map<int, vector<t_Expression*> >& nid_distanc_var_map, t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    for (vector<int>::iterator nid_itr = NIDSg.begin(); nid_itr != NIDSg.end(); nid_itr++) {
        if (*nid_itr != source_nid) {
            vector<t_Expression*> nid_dist_variable_seq;
            //if the variables to represent the bit sequence for node is not present then make it, other wise use it
            if (nid_distanc_var_map.find(*nid_itr) == nid_distanc_var_map.end()) {
                nid_dist_variable_seq = get_node_distance_expression_sequence(*nid_itr, em, gm);
                nid_distanc_var_map[*nid_itr] = nid_dist_variable_seq;
            } else {
                nid_dist_variable_seq = nid_distanc_var_map[*nid_itr];
            }
            assert(nid_dist_variable_seq.size() == NODE_DISTANCE_BIT_SEQ_LENGTH);

            t_Expression* distance_node_positive_constraint = get_expression_sequence_positive_constraint(nid_dist_variable_seq, em);
            if (distance_node_positive_constraint) {
                to_AND.push_back(distance_node_positive_constraint);
            } else {
                cerr << "Error: node distance constraint for node id " << *nid_itr << " is NULL, ignoring..." << endl;
            }
        }
    }

    //ANding constraints for all edges
    to_return = build_AND_constraints_from_vector(to_AND, em);

    return to_return;
}

t_Expression* GraphNew::get_edge_presence_implies_visible_constraints(t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    for (vector<int>::iterator edge_itr = EIDSg.begin(); edge_itr != EIDSg.end(); ++edge_itr) {
        int source_id = get_source_node(*edge_itr);
        int target_id = get_target_node(*edge_itr);
        string repid_source_node = get_rep_id_from_nid(source_id);
        string repid_target_node = get_rep_id_from_nid(target_id);
        string edge_type_str = get_edge_type(*edge_itr);
        vector<string> edge_subtype_vec_str = get_edge_subtypes_of_edge(*edge_itr);
        string edge_subtypes_str = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str);

        string edge_variable_present = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "present";
        string edge_variable_visible = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "visible";

        t_Expression* expression_edge_present = NULL;
        t_Expression* expression_edge_visible = NULL;

        if (getExpressionMap.find(edge_variable_present) == getExpressionMap.end()) {
            expression_edge_present = em->createSymbol(edge_variable_present, te1);
            getExpressionMap[edge_variable_present] = expression_edge_present;
            getCNFIndexMap[expression_edge_present] = make_pair(edge_variable_present, -1);
        } else {
            expression_edge_present = getExpressionMap[edge_variable_present];
        }

        if (getExpressionMap.find(edge_variable_visible) == getExpressionMap.end()) {
            expression_edge_visible = em->createSymbol(edge_variable_visible, te1);
            getExpressionMap[edge_variable_visible] = expression_edge_visible;
            getCNFIndexMap[expression_edge_visible] = make_pair(edge_variable_visible, -1);
        } else {
            expression_edge_visible = getExpressionMap[edge_variable_visible];
        }

        if (expression_edge_present && expression_edge_visible) {
            t_Expression* expression_edge_present_implies_visible = build_OR_constraints(expression_edge_present, expression_edge_visible, 1, 0, em);
            if (expression_edge_present) {
                to_AND.push_back(expression_edge_present_implies_visible);
            } else {
                cerr << "Error: implication is NULL, shouldn't be, check?" << endl;
            }
        } else {
            cout << "Note: edge present or visible expression is NULL, check?" << endl;
        }
    }

    //ANding constraints for all edges
    to_return = build_AND_constraints_from_vector(to_AND, em);

    return to_return;
}

t_Expression* GraphNew::get_reach_path_constraints(t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;
    vector<t_Expression*> to_AND;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    //get constraints: if an edge is present it must be visible
    cout << "Writing constraints: Edge present implies Edge visible" << endl;
    t_Expression* edge_present_implies_visible = get_edge_presence_implies_visible_constraints(em, gm);
    if (edge_present_implies_visible) {
        to_AND.push_back(edge_present_implies_visible);
    } else {
        cerr << "Note: no constraints for edge present implies visible in the graph" << endl;
    }

    //testing starts
    /*ofstream test_fout1("constraintsPresenceImpliesVisible");
    em->printExpressionToFileAsDAG("constraintsPresenceImpliesVisible ", edge_present_implies_visible, test_fout1);
    test_fout1.close();*/
    //testing ends

    //get source nodes and differentially expressed  nodes that are actually GErel targets with expression or repression as subtype
    string source_node_id;
    cout << "\nEnter source node id, e.g. hsa100 etc: ";
    cin >> source_node_id;
    string source_rid = get_rep_id_from_id(source_node_id);
    if (source_rid == "") {
        cerr << "Error: No rep id found for source node " + source_node_id + ", returning NULL" << endl;
        return NULL;
    }
    int source_nid = get_nid_from_rep_id(source_rid);
    assert(source_nid != -1);

    set<int> differentially_expressed_nids;
    string file_diff_exp;
    string s;
    cout << "Enter the file path containing the list of ids of differentially expressed nodes: ";
    cin >> file_diff_exp;

    ifstream fin(file_diff_exp.c_str());
    if (fin.is_open()) {
        while (fin >> s) {
            string diff_rid = get_rep_id_from_id(s);
            if (diff_rid != "") {
                int diff_nid = get_nid_from_rep_id(diff_rid);

                vector<int> inlist = get_inlist(diff_nid);
                for (vector<int>::iterator in_itr = inlist.begin(); in_itr != inlist.end(); in_itr++) {
                    if (get_edge_type(*in_itr) == "GErel" && (edge_has_subtype(*in_itr, "expression") || edge_has_subtype(*in_itr, "repression"))) {
                        //check: source node shouldn't be the same as the differentially expressed node
                        if (diff_nid == source_nid) {
                            cout << "Source node is also differentialy expressed node, ignoring it as differentially expressed node" << endl;
                        } else {
                            differentially_expressed_nids.insert(diff_nid);
                        }
                        break; //one edge is enough
                    }
                }
            } else {
                cerr << "Error: no rep id for differentially expressed id " + s << endl;
            }
        }
    } else {
        cerr << "Error: Couldn't open the file " + file_diff_exp << endl;
    }

    //get constraints: All the distances other than the source are positive.
    cout << "Writing constraints: all non_source distance are positive" << endl;
    map<int, vector<t_Expression*> > nid_distanc_var_map; //maps from a node id to each of its distances represented by a variable sequence
    t_Expression* expr_non_source_positive_dists = get_positive_distance_constrains_for_non_source_nodes(source_nid, nid_distanc_var_map, em, gm);
    if (expr_non_source_positive_dists) {
        to_AND.push_back(expr_non_source_positive_dists);
    } else {
        cerr << "Error: No non-source distance positive constraints" << endl;
    }

    //testing starts
    /*ofstream test_fout2("non_source_positive");
    em->printExpressionToFileAsDAG("non_source_positive ", expr_non_source_positive_dists, test_fout2);
    test_fout2.close();*/
    //testing ends

    cout << "Writing constraints: source node distance is zero" << endl;
    //get constraint: Distance of source node is 0.
    t_Expression* expr_source_node_zero_distance = get_zero_distance_constraint_for_node(source_nid, nid_distanc_var_map, em, gm);
    if (expr_source_node_zero_distance) {
        to_AND.push_back(expr_source_node_zero_distance);
    } else {
        cerr << "Error: No source distance zero constraints" << endl;
    }

    //testing starts
    /*ofstream test_fout3("source_zero");
    em->printExpressionToFileAsDAG("source_zero ", expr_source_node_zero_distance, test_fout3);
    test_fout3.close();*/
    //testing ends

    cout << "Writing constraints: At least one incoming edge to a differentially expressed node is visible" << endl;
    //get constraints: At least one incoming edge to a differentially expressed node is visible.

    t_Expression* expr_visible_incoming_edge_to_diff_expr_node = get_constraint_expr_visible_incoming_edge_to_diff_expr_node(differentially_expressed_nids, em, gm);
    if (expr_visible_incoming_edge_to_diff_expr_node) {
        to_AND.push_back(expr_visible_incoming_edge_to_diff_expr_node);
    } else {
        cerr << "Error: No source distance zero constraints" << endl;
    }

    //testing starts
    /*ofstream test_fout4("visible_incoming_edge_to_diff_expr_node");
    em->printExpressionToFileAsDAG("visible_incoming_edge_to_diff_expr_node ", expr_visible_incoming_edge_to_diff_expr_node, test_fout4);
    test_fout4.close();*/
    //testing ends

    cout << "Writing constraints: If a non-source node has an outgoing visible edge then it must have an incoming visible edge" << endl;
    //get constraints: If a non-source node has an outgoing visible edge then it must have an incoming visible edge
    t_Expression* expr_non_source_node_visible_edges_in_if_out = get_constraints_expr_visible_in_if_out_non_source_node(source_nid, em, gm);
    if (expr_non_source_node_visible_edges_in_if_out) {
        to_AND.push_back(expr_non_source_node_visible_edges_in_if_out);
    } else {
        cerr << "Error: No source distance zero constraints" << endl;
    }

    //testing starts
    /*ofstream test_fout5("non_source_node_visible_edges_in_if_out");
    em->printExpressionToFileAsDAG("non_source_node_visible_edges_in_if_out ", expr_non_source_node_visible_edges_in_if_out, test_fout5);
    test_fout5.close();*/
    //testing ends

    //create a variable that is asserted to true and thus acts as 1 in constraint solver -- used as we need to say distane of a nodes is one more than distance of the other node
    //variable name  given "true_bit"
    string true_bit = "true_bit";
    t_Expression* expression_true_bit = em->createSymbol(true_bit, te1);
    getExpressionMap[true_bit] = expression_true_bit;
    getCNFIndexMap[expression_true_bit] = make_pair(true_bit, -1);
    to_AND.push_back(expression_true_bit); //this will assert that this bit is set to 1

    //testing starts
    /*ofstream test_fout6("true bit");
    em->printExpressionToFileAsDAG("true_bit ", expression_true_bit, test_fout6);
    test_fout6.close();*/
    //testing ends

    cout << "Writing constraints: if an edge is visible then distance[target] = distance[source]+1" << endl;
    //get constraints: if an edge is visible then distance[target] = distance[source]+1
    t_Expression* expr_distance_source_target_for_visible_edge = get_constraints_distance_source_target_for_visible_edge(source_nid, nid_distanc_var_map, expression_true_bit, em, gm);
    if (expr_distance_source_target_for_visible_edge) {
        to_AND.push_back(expr_distance_source_target_for_visible_edge);
    } else {
        cerr << "Error: No constraint for source distance one less than edge distance for visible edges" << endl;
    }

    //testing starts
    /*ofstream test_fout7("distance_source_target_for_visible_edge ");
    em->printExpressionToFileAsDAG("distance_source_target_for_visible_edge  ", expr_distance_source_target_for_visible_edge, test_fout7);
    test_fout7.close();*/
    //testing ends

    //get constraints: assoc/dissoc edge exist in pair
    //add later

    cout << "Anding reach constraints..." << endl;
    to_return = build_AND_constraints_from_vector(to_AND, em);
    return to_return;
}

t_Expression* GraphNew::get_DAG_reach_path_constraints(t_ExpressionManager* em, GraphManagerNew* gm) {
    t_Expression* to_return = NULL;

    vector<t_Expression*> to_AND;

    //go over each edge and if that edge is present then one of its incoming edges must be present
    for (vector<int>::iterator edge_itr = EIDSg.begin(); edge_itr != EIDSg.end(); ++edge_itr) {
        int source_id = get_source_node(*edge_itr);
        int target_id = get_target_node(*edge_itr);
        string repid_source_node = get_rep_id_from_nid(source_id);
        string repid_target_node = get_rep_id_from_nid(target_id);
        string edge_type_str = get_edge_type(*edge_itr);
        vector<string> edge_subtype_vec_str = get_edge_subtypes_of_edge(*edge_itr);
        string edge_subtypes_str = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str);

        string source_node_variable = repid_source_node + "_active";
        string edge_variable = "edge." + repid_source_node + "to" + repid_target_node + "." + edge_type_str + "-" + edge_subtypes_str + "present";

        vector<t_Expression*> to_OR;

        //go over all incoming edges of source
        vector<int> source_inlist = get_inlist(source_id);
        for (vector<int>::iterator source_in_itr = source_inlist.begin(); source_in_itr != source_inlist.end(); source_in_itr++) {
            //if the source node is active and source-target edge is present then it must be explained by one of the activating incoming edges and source of source being active
            //so collect all these to be ored later
            if (edge_has_subtype(*source_in_itr, "activation")) {
                int id_source_of_source = get_source_node(*source_in_itr);
                string repid_source_of_source = get_rep_id_from_nid(id_source_of_source);
                string edge_type_str_in = get_edge_type(*source_in_itr);
                vector<string> edge_subtype_vec_str_in = get_edge_subtypes_of_edge(*source_in_itr);
                string edge_subtypes_str_in = gm->get_edge_substring_for_constraint_variable(edge_subtype_vec_str_in);

                string source_of_source_node_variable = repid_source_of_source + "_active";
                string edge_variable_in = "edge." + repid_source_of_source + "to" + repid_source_node + "." + edge_type_str_in + "-" + edge_subtypes_str_in + "present";

                t_Expression* expression_to_OR = NULL;
                t_Expression* expression_node = NULL;
                t_Expression* expression_edge = NULL;
                TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

                if (getExpressionMap.find(source_of_source_node_variable) == getExpressionMap.end()) {
                    expression_node = em->createSymbol(source_of_source_node_variable, te1);
                    getExpressionMap[source_of_source_node_variable] = expression_node;
                    getCNFIndexMap[expression_node] = make_pair(source_of_source_node_variable, -1);
                } else {
                    expression_node = getExpressionMap[source_of_source_node_variable];
                }

                if (getExpressionMap.find(edge_variable_in) == getExpressionMap.end()) {
                    expression_edge = em->createSymbol(edge_variable_in, te1);
                    getExpressionMap[edge_variable_in] = expression_edge;
                    getCNFIndexMap[expression_edge] = make_pair(edge_variable_in, -1);
                } else {
                    expression_edge = getExpressionMap[edge_variable_in];
                }

                expression_to_OR = build_AND_constraints(expression_node, expression_edge, 0, 0, em);
                to_OR.push_back(expression_to_OR);
            }

            //inhibition cases to be added later -- may need to redo some missing edges

        }

        t_Expression* expression_ORred = NULL;

        //if indeed the to_OR vector is not empty then we must have found an explaining incoming edge
        if (!to_OR.empty()) {
            expression_ORred = to_OR[0];
            if (to_OR.size() > 1) {
                //OR rest of the expressions
                for (vector<t_Expression*>::iterator to_or_itr = to_OR.begin() + 1; to_or_itr != to_OR.end(); to_or_itr++) {
                    t_Expression* temp = build_OR_constraints(expression_ORred, *to_or_itr, 0, 0, em);
                    expression_ORred = temp;
                }
            }

            t_Expression* expression_to_AND = NULL;
            t_Expression* expression_edge = NULL;
            TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

            if (getExpressionMap.find(edge_variable) == getExpressionMap.end()) {
                expression_edge = em->createSymbol(edge_variable, te1);
                getExpressionMap[edge_variable] = expression_edge;
                getCNFIndexMap[expression_edge] = make_pair(edge_variable, -1);
            } else {
                expression_edge = getExpressionMap[edge_variable];
            }

            expression_to_AND = build_OR_constraints(expression_edge, expression_ORred, 1, 0, em);
            to_AND.push_back(expression_to_AND);
        }
    }

    to_return = build_AND_constraints_from_vector(to_AND, em);

    return to_return;
}

void GraphNew::perform_kegg_consistency_check() {
    std::vector<int>::iterator itr;
    int gid = get_graph_id();
    string consistency_log = "log_kegg_consistency_graph_" + IntToString(gid);
    ofstream fout(consistency_log.c_str());
    if (fout.is_open()) {
        for (itr = EIDSg.begin(); itr != EIDSg.end(); itr++) {
            EdgeNew * edge = get_edge_from_eid(*itr);
            int source_id = edge->get_source();
            int target_id = edge->get_target();
            string source_rid = get_rep_id_from_nid(source_id);
            string target_rid = get_rep_id_from_nid(target_id);
            string edge_type = edge->get_edge_type();

            string source_type = get_node_type(source_id);
            string target_type = get_node_type(target_id);

            //adjacent compound implies edge type PCrel
            if ((target_type == "compound" || source_type == "compound") && edge_type != "PCrel") {
                fout << "Edge between " + source_rid + " and " + target_rid + " is of type " + edge_type + ", changing it to PCrel" << endl;
                remove_edge_id_type(*itr);
                add_edge_type(*itr, "PCrel");
            }

            //expression or repression implies GErel
            if ((edge_has_subtype(*itr, "expression") || edge_has_subtype(*itr, "repression")) && edge_type != "GErel") {
                fout << "Edge between " + source_rid + " and " + target_rid + " is of type " + edge_type + ", changing it to GErel" << endl;
                remove_edge_id_type(*itr);
                add_edge_type(*itr, "GErel");
            }

            //remaining checks are statistically less and to be added later



        }
        fout.close();
    } else {
        cerr << "Error: Log file " + consistency_log + " couldn't be opened" << endl;
    }
}

void GraphNew::display_graph_silent(string color_map_filename, string vis_edges_filename, string dot_filename, GraphManagerNew* gm) {

    map<int, color_triple_new> node_color_map;
    map<int, color_triple_new>::iterator color_itr;

    char color_visible_edges = 'y';
    //cout << "Enter 'y' or 'Y' to color edges that are visible but not present as assigned by the constraint solver for subgraph generation, any other key to not...";
    //cin >> color_visible_edges;
    set<int> red_vis_edges;

    if (color_visible_edges == 'y' || color_visible_edges == 'Y') {
        string visi_edges_file = vis_edges_filename;
        //cout << "Enter the file containing visible edges list: ";
        //cin >> visi_edges_file;

        ifstream fin(visi_edges_file.c_str());
        string vis_edge_var;
        if (fin.is_open()) {
            while (fin >> vis_edge_var) {
                list<string> *subparts = gm->get_list_of_names_from_string(vis_edge_var, ".");
                subparts->pop_front(); // remove 'edge' prefix
                string end_names = subparts->front(); //source to target
                subparts->pop_front();
                string edge_info = subparts->front(); //type subtype

                //get source and target rep ids
                list<string> *end_nodes = gm->get_list_of_names_from_string(end_names, "to");
                string source_rep_id = end_nodes->front();
                end_nodes->pop_front();
                string target_rep_id = end_nodes->front();
                end_nodes->pop_front();

                //get type info
                list<string>* edgeInfo = gm->get_list_of_names_from_string(edge_info, "-");
                string edgeType = edgeInfo->front(); //get edge type
                edgeInfo->pop_front(); // Remove the 'edge type' from list of strings

                //edgeInfo now has only subtypes

                //go over edges of the graph and if edge matches add it to red_vis_edges to be colored red later
                for (vector<int>::iterator edge_itr = EIDSg.begin(); edge_itr != EIDSg.end(); ++edge_itr) {
                    string ingraph_edge_type = get_edge_type(*edge_itr);
                    vector<string> ingraph_edge_subtypes = get_edge_subtypes_of_edge(*edge_itr);
                    sort(ingraph_edge_subtypes.begin(), ingraph_edge_subtypes.end());
                    vector<string> edgeSubtypes(edgeInfo->begin(), edgeInfo->end());
                    sort(edgeSubtypes.begin(), edgeSubtypes.end());
                    string ingraph_source_rid = get_rep_id_from_nid(get_source_node(*edge_itr));
                    string ingraph_target_rid = get_rep_id_from_nid(get_target_node(*edge_itr));
                    if (ingraph_source_rid == source_rep_id && ingraph_target_rid == target_rep_id && ingraph_edge_type == edgeType && edgeSubtypes == ingraph_edge_subtypes) {
                        red_vis_edges.insert(*edge_itr);
                    }
                }
            }
            fin.close();
        } else {
            cerr << "Error: Couldn't open assg file " + visi_edges_file << endl;
        }
    }

    if (color_map_filename != "0") {
        ifstream ifs(color_map_filename.c_str());
        string node_id;
        int redValue, greenValue, blueValue;
        if (!ifs.is_open()) {
            cerr << "Error: the color file " + color_map_filename + " couldn't be opened. Returning without displaying..." << endl;
            return;
        }
        while (ifs >> node_id) {
            ifs >> redValue;
            ifs >> greenValue;
            ifs >> blueValue;

            color_triple_new color(redValue, greenValue, blueValue);

            string rep_id = get_rep_id_from_id(node_id);
            if (rep_id != "") {
                int nid = get_nid_from_rep_id(rep_id);
                node_color_map.insert(pair<int, color_triple_new>(nid, color));
            }
        }
    }

    std::string dis_name = dot_filename;
    //std::cout << "\nEnter a file name for the graph display with .dot extension (e.g. mygraph.dot): ";
    //std::cin >> dis_name;

    ofstream fout(dis_name.c_str());

    if (fout.is_open()) {
        fout << "Digraph G  {\n";
        fout << "label = \"Graph " << this->graph_id << "\";\n";
        fout << "labelloc = top;\n";
        fout << "labeljust = left;\n";
        fout << "labelfontsize = 20.0;\n";
        fout << "edge [style=\"solid\"];\n";

        set<int> drawn_nodes;
        //set<int> drawn_edges;

        vector<set<int> > undirected_edges;

        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            set<int>::iterator drawn_nodes_itr = drawn_nodes.find(*itr);

            if (drawn_nodes_itr == drawn_nodes.end()) {//a node not already written
                // get color of node
                color_triple_new color_to_set;
                color_itr = node_color_map.find(*itr);
                if (color_itr != node_color_map.end()) {
                    color_to_set.red = (*color_itr).second.red;
                    color_to_set.green = (*color_itr).second.green;
                    color_to_set.blue = (*color_itr).second.blue;
                }
                write_node_to_dot_file(fout, *itr, drawn_nodes, color_to_set);

            }
            vector<int> node_out_list = get_outlist(*itr);

            if (!node_out_list.empty()) {
                for (vector<int>::iterator out_itr = node_out_list.begin(); out_itr != node_out_list.end(); out_itr++) {
                    int target_node_id = get_target_node(*out_itr);
                    drawn_nodes_itr = drawn_nodes.find(target_node_id);
                    if (drawn_nodes_itr == drawn_nodes.end()) {

                        // get color of node
                        color_triple_new color_to_set2;
                        color_itr = node_color_map.find(target_node_id);
                        if (color_itr != node_color_map.end()) {
                            color_to_set2.red = (*color_itr).second.red;
                            color_to_set2.green = (*color_itr).second.green;
                            color_to_set2.blue = (*color_itr).second.blue;
                        }
                        write_node_to_dot_file(fout, target_node_id, drawn_nodes, color_to_set2);

                    }
                    set<int> edge_nodes;
                    edge_nodes.insert(*itr);
                    edge_nodes.insert(target_node_id);
                    //undirected edge - so we draw it onle once with no arrow
                    if (find(undirected_edges.begin(), undirected_edges.end(), edge_nodes) == undirected_edges.end()) {
                        fout << "\t" << *itr;
                        fout << " -> ";
                        fout << get_target_node(*out_itr);

                        string edge_type = get_edge_type(*out_itr);
                        vector<string> edge_subtypes = get_edge_subtypes_of_edge(*out_itr);
                        vector<string> edge_belongs_to_pathways = get_all_pathways_for_edge(*out_itr);

                        string arrowhead, style, label, color;
                        fout << "[";

                        vector<string>::iterator vec_str_itr;

                        for (vec_str_itr = edge_subtypes.begin(); vec_str_itr != edge_subtypes.end(); vec_str_itr++) {
                            if (*vec_str_itr == "association" || *vec_str_itr == "dissociation") {

                                arrowhead = "none";
                                style = "bold";

                                if (*vec_str_itr == "association") {
                                    label += "assoc\\n";
                                }
                                if (*vec_str_itr == "dissociation") {
                                    label += "dissoc\\n";
                                }
                                undirected_edges.push_back(edge_nodes);
                            }

                            if (*vec_str_itr == "inhibition") {
                                arrowhead = "tee";
                                label += "inhb\\n";
                            }

                            if (*vec_str_itr == "repression") {
                                arrowhead = "tee";
                                label += "repr\\n";
                            }

                            if (*vec_str_itr == "activation") {
                                label += "actv\\n";
                            }

                            if (*vec_str_itr == "expression") {
                                label += "expr\\n";
                            }

                            if (*vec_str_itr == "dephosphorylation") {
                                label += "-p\\n";
                            }

                            if (*vec_str_itr == "phosphorylation") {
                                label += "+p\\n";
                            }

                            if (*vec_str_itr == "indirect") {
                                style = "dashed";
                            }

                            if (*vec_str_itr == "component") {
                                label += "comp\\n";
                            }

                            if (*vec_str_itr == "PPI") {
                                label = "PPI";

                            }
                        }

                        if (edge_type == "PPrel") {
                            label = label + "PPrel\\n";
                        }
                        if (edge_type == "GErel") {
                            label = label + "GErel\\n";
                        }
                        if (edge_type == "PCrel") {
                            label = label + "PCrel\\n";
                        }
                        if (edge_type == "ECrel") {
                            label = label + "ECrel\\n";
                        }

                        for (vec_str_itr = edge_belongs_to_pathways.begin(); vec_str_itr != edge_belongs_to_pathways.end(); vec_str_itr++) {
                            label += *vec_str_itr + "\\n";
                        }

                        if ((color_visible_edges == 'y' || color_visible_edges == 'Y') && red_vis_edges.find(*out_itr) != red_vis_edges.end()) {
                            fout << "arrowhead=\"" << arrowhead << "\", style=\"" << style << "\", color=red, label=\"" << label << "\", ";
                        } else {
                            fout << "arrowhead=\"" << arrowhead << "\", style=\"" << style << "\", color=\"" << color << "\", label=\"" << label << "\", ";
                        }
                        fout << "arrowsize=1, penwidth = 2]";
                        fout << ";" << "\n";

                    }
                }
            }
        }
        fout << "}";
        fout.close();

        string xdot_path_name = XDOT_PATH_NAME;
        string systemCommand = xdot_path_name + dis_name + " & ";
        //string systemCommand =  xdot_path_name + dis_name;
        //system(systemCommand.c_str());
    } else {
        cerr << "\nCouldn't open file for writing: " << dis_name << endl;
    }
}

void GraphNew::display_graph(string color_map_filename, GraphManagerNew* gm) {
    map<int, color_triple_new> node_color_map;
    map<int, color_triple_new>::iterator color_itr;

    char color_visible_edges;
    cout << "Enter 'y' or 'Y' to color edges that are visible but not present as assigned by the constraint solver for subgraph generation, any other key to not...";
    cin >> color_visible_edges;
    set<int> red_vis_edges;

    if (color_visible_edges == 'y' || color_visible_edges == 'Y') {
        string visi_edges_file;
        cout << "Enter the file containing visible edges list: ";
        cin >> visi_edges_file;

        ifstream fin(visi_edges_file.c_str());
        string vis_edge_var;
        if (fin.is_open()) {
            while (fin >> vis_edge_var) {
                list<string> *subparts = gm->get_list_of_names_from_string(vis_edge_var, ".");
                subparts->pop_front(); // remove 'edge' prefix
                string end_names = subparts->front(); //source to target
                subparts->pop_front();
                string edge_info = subparts->front(); //type subtype

                //get source and target rep ids
                list<string> *end_nodes = gm->get_list_of_names_from_string(end_names, "to");
                string source_rep_id = end_nodes->front();
                end_nodes->pop_front();
                string target_rep_id = end_nodes->front();
                end_nodes->pop_front();

                //get type info
                list<string>* edgeInfo = gm->get_list_of_names_from_string(edge_info, "-");
                string edgeType = edgeInfo->front(); //get edge type
                edgeInfo->pop_front(); // Remove the 'edge type' from list of strings

                //edgeInfo now has only subtypes

                //go over edges of the graph and if edge matches add it to red_vis_edges to be colored red later
                for (vector<int>::iterator edge_itr = EIDSg.begin(); edge_itr != EIDSg.end(); ++edge_itr) {
                    string ingraph_edge_type = get_edge_type(*edge_itr);
                    vector<string> ingraph_edge_subtypes = get_edge_subtypes_of_edge(*edge_itr);
                    sort(ingraph_edge_subtypes.begin(), ingraph_edge_subtypes.end());
                    vector<string> edgeSubtypes(edgeInfo->begin(), edgeInfo->end());
                    sort(edgeSubtypes.begin(), edgeSubtypes.end());
                    string ingraph_source_rid = get_rep_id_from_nid(get_source_node(*edge_itr));
                    string ingraph_target_rid = get_rep_id_from_nid(get_target_node(*edge_itr));
                    if (ingraph_source_rid == source_rep_id && ingraph_target_rid == target_rep_id && ingraph_edge_type == edgeType && edgeSubtypes == ingraph_edge_subtypes) {
                        red_vis_edges.insert(*edge_itr);
                    }
                }
            }
            fin.close();
        } else {
            cerr << "Error: Couldn't open assg file " + visi_edges_file << endl;
        }
    }

    if (color_map_filename != "0") {
        ifstream ifs(color_map_filename.c_str());
        string node_id;
        int redValue, greenValue, blueValue;
        if (!ifs.is_open()) {
            cerr << "Error: the color file " + color_map_filename + " couldn't be opened. Returning without displaying..." << endl;
            return;
        }
        while (ifs >> node_id) {
            ifs >> redValue;
            ifs >> greenValue;
            ifs >> blueValue;

            color_triple_new color(redValue, greenValue, blueValue);

            string rep_id = get_rep_id_from_id(node_id);
            if (rep_id != "") {
                int nid = get_nid_from_rep_id(rep_id);
                node_color_map.insert(pair<int, color_triple_new>(nid, color));
            }
        }
    }

    std::string dis_name;
    std::cout << "\nEnter a file name for the graph display with .dot extension (e.g. mygraph.dot): ";
    std::cin >> dis_name;

    ofstream fout(dis_name.c_str());

    if (fout.is_open()) {
        fout << "Digraph G  {\n";
        fout << "label = \"Graph " << this->graph_id << "\";\n";
        fout << "labelloc = top;\n";
        fout << "labeljust = left;\n";
        fout << "labelfontsize = 20.0;\n";
        fout << "edge [style=\"solid\"];\n";

        set<int> drawn_nodes;
        //set<int> drawn_edges;

        vector<set<int> > undirected_edges;

        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            set<int>::iterator drawn_nodes_itr = drawn_nodes.find(*itr);

            if (drawn_nodes_itr == drawn_nodes.end()) {//a node not already written
                // get color of node
                color_triple_new color_to_set;
                color_itr = node_color_map.find(*itr);
                if (color_itr != node_color_map.end()) {
                    color_to_set.red = (*color_itr).second.red;
                    color_to_set.green = (*color_itr).second.green;
                    color_to_set.blue = (*color_itr).second.blue;
                }
                write_node_to_dot_file(fout, *itr, drawn_nodes, color_to_set);

            }
            vector<int> node_out_list = get_outlist(*itr);

            if (!node_out_list.empty()) {
                for (vector<int>::iterator out_itr = node_out_list.begin(); out_itr != node_out_list.end(); out_itr++) {
                    int target_node_id = get_target_node(*out_itr);
                    drawn_nodes_itr = drawn_nodes.find(target_node_id);
                    if (drawn_nodes_itr == drawn_nodes.end()) {

                        // get color of node
                        color_triple_new color_to_set2;
                        color_itr = node_color_map.find(target_node_id);
                        if (color_itr != node_color_map.end()) {
                            color_to_set2.red = (*color_itr).second.red;
                            color_to_set2.green = (*color_itr).second.green;
                            color_to_set2.blue = (*color_itr).second.blue;
                        }
                        write_node_to_dot_file(fout, target_node_id, drawn_nodes, color_to_set2);

                    }
                    set<int> edge_nodes;
                    edge_nodes.insert(*itr);
                    edge_nodes.insert(target_node_id);
                    //undirected edge - so we draw it onle once with no arrow
                    if (find(undirected_edges.begin(), undirected_edges.end(), edge_nodes) == undirected_edges.end()) {
                        fout << "\t" << *itr;
                        fout << " -> ";
                        fout << get_target_node(*out_itr);

                        string edge_type = get_edge_type(*out_itr);
                        vector<string> edge_subtypes = get_edge_subtypes_of_edge(*out_itr);
                        vector<string> edge_belongs_to_pathways = get_all_pathways_for_edge(*out_itr);

                        string arrowhead, style, label, color;
                        fout << "[";

                        vector<string>::iterator vec_str_itr;

                        for (vec_str_itr = edge_subtypes.begin(); vec_str_itr != edge_subtypes.end(); vec_str_itr++) {
                            if (*vec_str_itr == "association" || *vec_str_itr == "dissociation") {

                                arrowhead = "none";
                                style = "bold";

                                if (*vec_str_itr == "association") {
                                    label += "assoc\\n";
                                }
                                if (*vec_str_itr == "dissociation") {
                                    label += "dissoc\\n";
                                }
                                undirected_edges.push_back(edge_nodes);
                            }

                            if (*vec_str_itr == "inhibition") {
                                arrowhead = "tee";
                                label += "inhb\\n";
                            }

                            if (*vec_str_itr == "repression") {
                                arrowhead = "tee";
                                label += "repr\\n";
                            }

                            if (*vec_str_itr == "activation") {
                                label += "actv\\n";
                            }

                            if (*vec_str_itr == "expression") {
                                label += "expr\\n";
                            }

                            if (*vec_str_itr == "dephosphorylation") {
                                label += "-p\\n";
                            }

                            if (*vec_str_itr == "phosphorylation") {
                                label += "+p\\n";
                            }

                            if (*vec_str_itr == "indirect") {
                                style = "dashed";
                            }

                            if (*vec_str_itr == "component") {
                                label += "comp\\n";
                            }

                            if (*vec_str_itr == "PPI") {
                                label = "PPI";

                            }
                        }

                        if (edge_type == "PPrel") {
                            label = label + "PPrel\\n";
                        }
                        if (edge_type == "GErel") {
                            label = label + "GErel\\n";
                        }
                        if (edge_type == "PCrel") {
                            label = label + "PCrel\\n";
                        }
                        if (edge_type == "ECrel") {
                            label = label + "ECrel\\n";
                        }

                        for (vec_str_itr = edge_belongs_to_pathways.begin(); vec_str_itr != edge_belongs_to_pathways.end(); vec_str_itr++) {
                            label += *vec_str_itr + "\\n";
                        }

                        if ((color_visible_edges == 'y' || color_visible_edges == 'Y') && red_vis_edges.find(*out_itr) != red_vis_edges.end()) {
                            fout << "arrowhead=\"" << arrowhead << "\", style=\"" << style << "\", color=red, label=\"" << label << "\", ";
                        } else {
                            fout << "arrowhead=\"" << arrowhead << "\", style=\"" << style << "\", color=\"" << color << "\", label=\"" << label << "\", ";
                        }
                        fout << "arrowsize=1, penwidth = 2]";
                        fout << ";" << "\n";

                    }
                }
            }
        }
        fout << "}";
        fout.close();

        string xdot_path_name = XDOT_PATH_NAME;
        string systemCommand = xdot_path_name + dis_name + " & ";
        //string systemCommand =  xdot_path_name + dis_name;
        system(systemCommand.c_str());
    } else {
        cerr << "\nCouldn't open file for writing: " << dis_name << endl;
    }
}

void GraphNew::display_graph_richer_ordering_nodes(string color_map_filename, GraphManagerNew* gm) {
    map<int, color_triple_new> node_color_map;
    map<int, color_triple_new>::iterator color_itr;

    map<string, int> disp_name_nid_map;
    
    if (color_map_filename != "0") {
        ifstream ifs(color_map_filename.c_str());
        string node_id;
        int redValue, greenValue, blueValue;
        if (!ifs.is_open()) {
            cerr << "Error: the color file " + color_map_filename + " couldn't be opened. Returning without displaying..." << endl;
            return;
        }
        while (ifs >> node_id) {
            ifs >> redValue;
            ifs >> greenValue;
            ifs >> blueValue;

            color_triple_new color(redValue, greenValue, blueValue);

            string rep_id = get_rep_id_from_id(node_id);
            if (rep_id != "") {
                int nid = get_nid_from_rep_id(rep_id);
                node_color_map.insert(pair<int, color_triple_new>(nid, color));
            }
        }
    }

    std::string dis_name;
    std::cout << "\nEnter a file name for the graph display without .dot extension (e.g. mygraph): ";
    std::cin >> dis_name;

    string dis_name_dot = dis_name + ".dot";

    ofstream fout(dis_name_dot.c_str());

    if (fout.is_open()) {
        fout << "Digraph G  {\n";
        fout << "label = \"Graph " << this->graph_id << "\";\n";
        fout << "labelloc = top;\n";
        fout << "labeljust = left;\n";
        fout << "labelfontsize = 20.0;\n";
        fout << "edge [style=\"solid\"];\n";

        set<int> drawn_nodes;
        //set<int> drawn_edges;

        vector<set<int> > undirected_edges;

        // put the nodes in a map with the first display id as the key
        // this will sort the nodes (nids) based on the display names
        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            int curr_nid = (*itr);
            string curr_disp_name = get_all_display_ids_of_node(curr_nid).front();
            disp_name_nid_map.insert(pair<string, int>(curr_disp_name, curr_nid));
        }
        
        // draw the nodes in the order of the display names
        map<string, int>::iterator map_iter;
        for (map_iter = disp_name_nid_map.begin(); map_iter != disp_name_nid_map.end(); map_iter++){
            int curr_nid = (*map_iter).second;
            // get color of node
            color_triple_new color_to_set;
            color_itr = node_color_map.find(curr_nid);
            if (color_itr != node_color_map.end()) {
                color_to_set.red = (*color_itr).second.red;
                color_to_set.green = (*color_itr).second.green;
                color_to_set.blue = (*color_itr).second.blue;
            }
            write_node_to_dot_file_richer(fout, curr_nid, drawn_nodes, color_to_set);
        }
        
        // draw the edges
        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            
            vector<int> node_out_list = get_outlist(*itr);

            if (!node_out_list.empty()) {
                for (vector<int>::iterator out_itr = node_out_list.begin(); out_itr != node_out_list.end(); out_itr++) {
                    int target_node_id = get_target_node(*out_itr);
                    
                    set<int> edge_nodes;
                    edge_nodes.insert(*itr);
                    edge_nodes.insert(target_node_id);
                    //undirected edge - so we draw it onle once with no arrow
                    if (find(undirected_edges.begin(), undirected_edges.end(), edge_nodes) == undirected_edges.end()) {
                        fout << "\t" << *itr;
                        fout << " -> ";
                        fout << get_target_node(*out_itr);

                        string edge_type = get_edge_type(*out_itr);
                        vector<string> edge_subtypes = get_edge_subtypes_of_edge(*out_itr);
                        vector<string> edge_belongs_to_pathways = get_all_pathways_for_edge(*out_itr);

                        string label = " ";
                        string arrowhead, style, color;
                        fout << "[";

                        vector<string>::iterator vec_str_itr;

                        for (vec_str_itr = edge_subtypes.begin(); vec_str_itr != edge_subtypes.end(); vec_str_itr++) {
                            if (*vec_str_itr == "association" || *vec_str_itr == "dissociation" || *vec_str_itr == "reaction") {

                                arrowhead = "none";
                                
                                if (*vec_str_itr == "association") {
                                    label += " assoc";
                                }
                                if (*vec_str_itr == "dissociation") {
                                    label += " dissoc";
                                }
                                if (*vec_str_itr == "component") {
                                    label += " comp";
                                }
                                undirected_edges.push_back(edge_nodes);
                            }

                            if (*vec_str_itr == "inhibition") {
                                arrowhead = "tee";
                                label += "inhb ";
                            }

                            if (*vec_str_itr == "repression") {
                                arrowhead = "tee";
                                label += "repr ";
                            }

                            if (*vec_str_itr == "activation") {
                                label += "actv ";
                            }

                            if (*vec_str_itr == "expression") {
                                label += "expr ";
                            }

                            if (*vec_str_itr == "dephosphorylation") {
                                label += "-p ";
                            }

                            if (*vec_str_itr == "phosphorylation") {
                                label += "+p ";
                            }

                            if (*vec_str_itr == "indirect") {
                                style = "dashed";
                            }
                            
                            if (*vec_str_itr == "PPI") {
                                label = "PPI ";
                            }
                            
                            if (*vec_str_itr == "name_map") {
                                style = "dotted";
                                color = "blue";
                            }
                        }

//                        if (edge_type == "PPrel") {
//                            label = label + "PPrel\\n";
//                        }
//                        if (edge_type == "GErel") {
//                            label = label + "GErel\\n";
//                        }
//                        if (edge_type == "PCrel") {
//                            label = label + "PCrel\\n";
//                        }
//                        if (edge_type == "ECrel") {
//                            label = label + "ECrel\\n";
//                        }

//                        for (vec_str_itr = edge_belongs_to_pathways.begin(); vec_str_itr != edge_belongs_to_pathways.end(); vec_str_itr++) {
//                            label += *vec_str_itr + "\\n";
//                        }
//                        for (vec_str_itr = edge_belongs_to_pathways.begin(); vec_str_itr != edge_belongs_to_pathways.end(); vec_str_itr++) {
//                            label += *vec_str_itr + "\\n";
//                        }

                        fout << "arrowhead=\"" << arrowhead << "\", arrowsize=0.7, style=\"" << style << "\", color=\"" << color << "\", label=< <TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n ";
                        EdgeNew * edge = get_edge_from_eid(*out_itr);
                        vector<string> all_path_vec = get_all_pathways_for_edge(*out_itr);
                        //fout << "<TR><TD TOOLTIP=\"" << get_all_display_ids_of_node(get_source_node(edge->get_edge_id())).front() << "-" << get_all_display_ids_of_node(get_target_node(edge->get_edge_id())).front() << "\">" << label << "</TD></TR>\n";
                        fout << "<TR><TD>" << label << "</TD></TR>\n";
                        for(int i = 0; i < all_path_vec.size(); i++){
                            
                            string curr_path = all_path_vec.at(i);
                            string pathway_name = gm->get_pathway_names_from_path_id(curr_path);
                            if(curr_path.substr(0,8) == "path:hsa"){
                                fout << "<TR><TD TOOLTIP=\"KEGG:" << pathway_name << "\" HREF=\"http://www.genome.jp/kegg-bin/show_pathway?" << curr_path.substr(5) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">KEGG:" << curr_path.substr(5) << "</FONT></TD></TR>" << endl;
                            }
                            
                            else if(curr_path.substr(0,8) == "pathway_"){
                                fout << "<TR><TD TOOLTIP=\"Reactome:" << pathway_name << "\" HREF=\"http://www.reactome.org/content/detail/R-HSA-" << curr_path.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">Reactome:" << curr_path << "</FONT></TD></TR>";
                                
                            }
                            
                            else{
                                fout << "<TR><TD TOOLTIP=\"" << curr_path << "\" ><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << curr_path << "</FONT></TD></TR>" << endl;
                            }

//                            else if(curr_id.substr(0,7) == "uniprot"){
//                        myfile << "<TR><TD TOOLTIP=\"" << "UniProt: " << curr_id.substr(8) << "\" " << "HREF=\"http://www.uniprot.org/uniprot/" << curr_id.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "UniProt: " << curr_id.substr(8) << "</FONT></TD></TR>" << endl;
//                    }
                        }
                        fout << "</TABLE>>" << endl;
                        
                        
                        fout << "]";
                        fout << ";" << "\n";

                    }
                }
            }
        }
        fout << "}";
        fout.close();

        string dot_path_name = DOT_PATH_NAME;
        string systemCommand1 = dot_path_name + "-Tcmapx " + dis_name + ".dot -o " + dis_name + ".cmapx & ";
        system(systemCommand1.c_str());

        string systemCommand2 = dot_path_name + "-Tsvg " + dis_name + ".dot -o " + dis_name + ".svg & ";
        system(systemCommand2.c_str());

        string systemCommand3 = "firefox " + dis_name + ".svg &";
        system(systemCommand3.c_str());

    } else {
        cerr << "\nCouldn't open file for writing: " << dis_name << endl;
    }
}



void GraphNew::display_graph_richer_clustering_nodes(string color_map_filename, GraphManagerNew* gm) {
    map<int, color_triple_new> node_color_map;
    map<int, color_triple_new>::iterator color_itr;

//    map<string, set<int> > disp_name_nid_map;
//    map<string, set<int> >::iterator umap_itr;
    unordered_map<string, set<int> > disp_name_nid_map;
    unordered_map<string, set<int> >::iterator umap_itr;
    
    if (color_map_filename != "0") {
        ifstream ifs(color_map_filename.c_str());
        string node_id;
        int redValue, greenValue, blueValue;
        if (!ifs.is_open()) {
            cerr << "Error: the color file " + color_map_filename + " couldn't be opened. Returning without displaying..." << endl;
            return;
        }
        while (ifs >> node_id) {
            ifs >> redValue;
            ifs >> greenValue;
            ifs >> blueValue;

            color_triple_new color(redValue, greenValue, blueValue);

            string rep_id = get_rep_id_from_id(node_id);
            if (rep_id != "") {
                int nid = get_nid_from_rep_id(rep_id);
                node_color_map.insert(pair<int, color_triple_new>(nid, color));
            }
        }
    }

    std::string dis_name;
    std::cout << "\nEnter a file name for the graph display without .dot extension (e.g. mygraph): ";
    std::cin >> dis_name;

    string dis_name_dot = dis_name + ".dot";

    ofstream fout(dis_name_dot.c_str());

    if (fout.is_open()) {
        fout << "Digraph G  {\n";
        fout << "label = \"Graph " << this->graph_id << "\";\n";
        fout << "labelloc = top;\n";
        fout << "labeljust = left;\n";
        fout << "labelfontsize = 20.0;\n";
        fout << "edge [style=\"solid\"];\n";

        set<int> drawn_nodes;
        set<int> reaction_nodes;
        //set<int> drawn_edges;

        vector<set<int> > undirected_edges;

        // put the nodes in an unordered map with the first 3 characters of display id as the key
        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            
            int curr_nid = (*itr);
            if (get_node_type(curr_nid) == "reaction"){
                reaction_nodes.insert(curr_nid);
//                vector<int> in, out;
//                in = get_inlist(curr_nid);
//                out = get_outlist(curr_nid);
//                string in_node_name = get_all_display_ids_of_node(in.front()).front();
//                string out_node_name = get_all_display_ids_of_node(out.front()).front();
//                if (in.size() == 1 && out.size() == 1 && (in_node_name.substr(0,3) == out_node_name.substr(0,3))){
//                    string substr = in_node_name.substr(0,3);
//                    
//                    umap_itr = disp_name_nid_map.find(substr);
//                    if (umap_itr == disp_name_nid_map.end()) {
//                        set<int> curr_ids;
//                        curr_ids.insert(curr_nid);
//                        disp_name_nid_map.insert(pair<string, set<int> >(substr, curr_ids));
//                    }
//                    else {
//                        (*umap_itr).second.insert(curr_nid);
//                    }
//                }
            }
            else{
                string curr_disp_name = get_all_display_ids_of_node(curr_nid).front();

                // remove heading 'active' and 'activated' terms - remove upto first whitespace
                if (curr_disp_name.substr(0,5) == "activ" || curr_disp_name.substr(0,5) == "Activ"){

                    string::size_type spac;
                    spac = curr_disp_name.find(" ", 0);
                    curr_disp_name = curr_disp_name.substr(spac+1);

                }

                string substr = curr_disp_name.substr(0,3);
                std::transform(substr.begin(), substr.end(), substr.begin(), ::toupper); //converts substr (future key of the unordered map) to uppercase - handles mixed cases

                umap_itr = disp_name_nid_map.find(substr);
                if (umap_itr == disp_name_nid_map.end()) {
                    set<int> curr_ids;
                    curr_ids.insert(curr_nid);
                    disp_name_nid_map.insert(pair<string, set<int> >(substr, curr_ids));
                }
                else {
                    (*umap_itr).second.insert(curr_nid);
                }
            }
        }
        // write the clusters in the dot files
        int cluster_num = 0;
        
        for(umap_itr = disp_name_nid_map.begin(); umap_itr != disp_name_nid_map.end(); umap_itr++){
            set<int> cluster_content = (*umap_itr).second;
            cout << (*umap_itr).first << " - " << (*umap_itr).second.size() << endl;
            
            //fout << "subgraph cluster_" << cluster_num << " {\n";
            fout << "subgraph {\n";
            fout << "style=filled;\n";
	    fout << "color=lightgrey;\n";
            string invis_path = "";
            for (set<int>::iterator set_itr = cluster_content.begin(); set_itr != cluster_content.end(); set_itr++){
                // get color of node
                color_triple_new color_to_set;
                color_itr = node_color_map.find(*set_itr);
                if (color_itr != node_color_map.end()) {
                    color_to_set.red = (*color_itr).second.red;
                    color_to_set.green = (*color_itr).second.green;
                    color_to_set.blue = (*color_itr).second.blue;
                }
                write_node_to_dot_file_richer(fout, *set_itr, drawn_nodes, color_to_set);
                invis_path = invis_path + (*set_itr) + " -> ";
            }
            
            invis_path = invis_path.substr(0, invis_path.size()-4);
            if(cluster_content.size() > 1)
                fout << invis_path << " [arrowhead=\"none\", style = invisible];\n";
            fout << " label = \"\";\n";
            fout <<"}\n";
            cluster_num++;
        }
            
        // write the reaction nodes separately
        for (set<int>::iterator itr = reaction_nodes.begin(); itr != reaction_nodes.end(); itr++) {
            // get color of node
            color_triple_new color_to_set;
            color_itr = node_color_map.find(*itr);
            if (color_itr != node_color_map.end()) {
                color_to_set.red = (*color_itr).second.red;
                color_to_set.green = (*color_itr).second.green;
                color_to_set.blue = (*color_itr).second.blue;
            }
            write_node_to_dot_file_richer(fout, *itr, drawn_nodes, color_to_set);
        }
        
        

        // draw the edges
        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            
            vector<int> node_out_list = get_outlist(*itr);

            if (!node_out_list.empty()) {
                for (vector<int>::iterator out_itr = node_out_list.begin(); out_itr != node_out_list.end(); out_itr++) {
                    int target_node_id = get_target_node(*out_itr);
                    
                    set<int> edge_nodes;
                    edge_nodes.insert(*itr);
                    edge_nodes.insert(target_node_id);
                    //undirected edge - so we draw it onle once with no arrow
                    if (find(undirected_edges.begin(), undirected_edges.end(), edge_nodes) == undirected_edges.end()) {
                        fout << "\t" << *itr;
                        fout << " -> ";
                        fout << get_target_node(*out_itr);

                        string edge_type = get_edge_type(*out_itr);
                        vector<string> edge_subtypes = get_edge_subtypes_of_edge(*out_itr);
                        vector<string> edge_belongs_to_pathways = get_all_pathways_for_edge(*out_itr);

                        string label = " ";
                        string arrowhead, style, color;
                        fout << "[";

                        vector<string>::iterator vec_str_itr;

                        for (vec_str_itr = edge_subtypes.begin(); vec_str_itr != edge_subtypes.end(); vec_str_itr++) {
                            if (*vec_str_itr == "association" || *vec_str_itr == "dissociation" || *vec_str_itr == "reaction") {

                                arrowhead = "none";
                                
                                if (*vec_str_itr == "association") {
                                    label += " assoc";
                                }
                                if (*vec_str_itr == "dissociation") {
                                    label += " dissoc";
                                }
                                if (*vec_str_itr == "reaction") {
                                    label += " reaction";
                                }
                                undirected_edges.push_back(edge_nodes);
                            }

                            if (*vec_str_itr == "inhibition") {
                                arrowhead = "tee";
                                label += "inhb ";
                            }

                            if (*vec_str_itr == "repression") {
                                arrowhead = "tee";
                                label += "repr ";
                            }

                            if (*vec_str_itr == "activation") {
                                label += "actv ";
                            }

                            if (*vec_str_itr == "expression") {
                                label += "expr ";
                            }

                            if (*vec_str_itr == "dephosphorylation") {
                                label += "-p ";
                            }

                            if (*vec_str_itr == "phosphorylation") {
                                label += "+p ";
                            }

                            if (*vec_str_itr == "indirect") {
                                style = "dashed";
                            }

                            if (*vec_str_itr == "component") {
                                label += "comp ";
                            }

                            if (*vec_str_itr == "PPI") {
                                label = "PPI ";

                            }
                        }

                        fout << "arrowhead=\"" << arrowhead << "\", arrowsize=0.7, style=\"" << style << "\", color=\"" << color << "\", label=< <TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n ";
                        EdgeNew * edge = get_edge_from_eid(*out_itr);
                        vector<string> all_path_vec = get_all_pathways_for_edge(*out_itr);
                        //fout << "<TR><TD TOOLTIP=\"" << get_all_display_ids_of_node(get_source_node(edge->get_edge_id())).front() << "-" << get_all_display_ids_of_node(get_target_node(edge->get_edge_id())).front() << "\">" << label << "</TD></TR>\n";
                        fout << "<TR><TD>" << label << "</TD></TR>\n";
                        for(int i = 0; i < all_path_vec.size(); i++){
                            
                            string curr_path = all_path_vec.at(i);
                            string pathway_name = gm->get_pathway_names_from_path_id(curr_path);
                            if(curr_path.substr(0,8) == "path:hsa"){
                                fout << "<TR><TD TOOLTIP=\"KEGG:" << pathway_name << "\" HREF=\"http://www.genome.jp/kegg-bin/show_pathway?" << curr_path.substr(5) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">KEGG:" << curr_path.substr(5) << "</FONT></TD></TR>" << endl;
                            }
                            
                            else if(curr_path.substr(0,8) == "pathway_"){
                                fout << "<TR><TD TOOLTIP=\"Reactome:" << pathway_name << "\" HREF=\"http://www.reactome.org/content/detail/R-HSA-" << curr_path.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">Reactome:" << curr_path << "</FONT></TD></TR>";
                                
                            }
                            
                            else{
                                fout << "<TR><TD TOOLTIP=\"" << curr_path << "\" ><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << curr_path << "</FONT></TD></TR>" << endl;
                            }

//                            else if(curr_id.substr(0,7) == "uniprot"){
//                        myfile << "<TR><TD TOOLTIP=\"" << "UniProt: " << curr_id.substr(8) << "\" " << "HREF=\"http://www.uniprot.org/uniprot/" << curr_id.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "UniProt: " << curr_id.substr(8) << "</FONT></TD></TR>" << endl;
//                    }
                        }
                        fout << "</TABLE>>" << endl;
                        
                        
                        fout << "]";
                        fout << ";" << "\n";

                    }
                }
            }
        }
        fout << "}";
        fout.close();

        string dot_path_name = DOT_PATH_NAME;
        string systemCommand1 = dot_path_name + "-Tcmapx " + dis_name + ".dot -o " + dis_name + ".cmapx & ";
        system(systemCommand1.c_str());

        string systemCommand2 = dot_path_name + "-Tsvg " + dis_name + ".dot -o " + dis_name + ".svg & ";
        system(systemCommand2.c_str());

        string systemCommand3 = "firefox " + dis_name + ".svg &";
        system(systemCommand3.c_str());

    } else {
        cerr << "\nCouldn't open file for writing: " << dis_name << endl;
    }
}


void GraphNew::display_graph_richer(string color_map_filename, GraphManagerNew* gm) {
    map<int, color_triple_new> node_color_map;
    map<int, color_triple_new>::iterator color_itr;

    if (color_map_filename != "0") {
        ifstream ifs(color_map_filename.c_str());
        string node_id;
        int redValue, greenValue, blueValue;
        if (!ifs.is_open()) {
            cerr << "Error: the color file " + color_map_filename + " couldn't be opened. Returning without displaying..." << endl;
            return;
        }
        while (ifs >> node_id) {
            ifs >> redValue;
            ifs >> greenValue;
            ifs >> blueValue;

            color_triple_new color(redValue, greenValue, blueValue);

            string rep_id = get_rep_id_from_id(node_id);
            if (rep_id != "") {
                int nid = get_nid_from_rep_id(rep_id);
                node_color_map.insert(pair<int, color_triple_new>(nid, color));
            }
        }
    }

    std::string dis_name;
    std::cout << "\nEnter a file name for the graph display without .dot extension (e.g. mygraph): ";
    std::cin >> dis_name;

    string dis_name_dot = dis_name + ".dot";

    ofstream fout(dis_name_dot.c_str());

    if (fout.is_open()) {
        fout << "Digraph G  {\n";
        fout << "label = \"Graph " << this->graph_id << "\";\n";
        fout << "labelloc = top;\n";
        fout << "labeljust = left;\n";
        fout << "labelfontsize = 20.0;\n";
        fout << "edge [style=\"solid\"];\n";

        set<int> drawn_nodes;
        //set<int> drawn_edges;

        vector<set<int> > undirected_edges;

        for (vector<int>::iterator itr = NIDSg.begin(); itr != NIDSg.end(); itr++) {
            set<int>::iterator drawn_nodes_itr = drawn_nodes.find(*itr);
            
            if (drawn_nodes_itr == drawn_nodes.end()) {//a node not already written
                // get color of node
                color_triple_new color_to_set;
                color_itr = node_color_map.find(*itr);
                if (color_itr != node_color_map.end()) {
                    color_to_set.red = (*color_itr).second.red;
                    color_to_set.green = (*color_itr).second.green;
                    color_to_set.blue = (*color_itr).second.blue;
                }
                write_node_to_dot_file_richer(fout, *itr, drawn_nodes, color_to_set);
                
            }
            vector<int> node_out_list = get_outlist(*itr);

            if (!node_out_list.empty()) {
                for (vector<int>::iterator out_itr = node_out_list.begin(); out_itr != node_out_list.end(); out_itr++) {
                    int target_node_id = get_target_node(*out_itr);
                    drawn_nodes_itr = drawn_nodes.find(target_node_id);
                    if (drawn_nodes_itr == drawn_nodes.end()) {

                        // get color of node
                        color_triple_new color_to_set2;
                        color_itr = node_color_map.find(target_node_id);
                        if (color_itr != node_color_map.end()) {
                            color_to_set2.red = (*color_itr).second.red;
                            color_to_set2.green = (*color_itr).second.green;
                            color_to_set2.blue = (*color_itr).second.blue;
                        }
                        write_node_to_dot_file_richer(fout, target_node_id, drawn_nodes, color_to_set2);
                        
                    }
                    set<int> edge_nodes;
                    edge_nodes.insert(*itr);
                    edge_nodes.insert(target_node_id);
                    //undirected edge - so we draw it onle once with no arrow
                    if (find(undirected_edges.begin(), undirected_edges.end(), edge_nodes) == undirected_edges.end()) {
                        fout << "\t" << *itr;
                        fout << " -> ";
                        fout << get_target_node(*out_itr);

                        string edge_type = get_edge_type(*out_itr);
                        vector<string> edge_subtypes = get_edge_subtypes_of_edge(*out_itr);
                        vector<string> edge_belongs_to_pathways = get_all_pathways_for_edge(*out_itr);

                        string label = " ";
                        string arrowhead, style, color;
                        fout << "[";

                        vector<string>::iterator vec_str_itr;

                        for (vec_str_itr = edge_subtypes.begin(); vec_str_itr != edge_subtypes.end(); vec_str_itr++) {
                            if (*vec_str_itr == "association" || *vec_str_itr == "dissociation" || *vec_str_itr == "mapping" || *vec_str_itr == "component" ) {

                                arrowhead = "none";
                                
                                if (*vec_str_itr == "association") {
                                    label += " assoc";
                                }
                                if (*vec_str_itr == "dissociation") {
                                    label += " dissoc";
                                }
                                if (*vec_str_itr == "mapping") {
                                    label += "map";
                                }
                                if (*vec_str_itr == "component") {
                                    label += " comp";
                                }

                                undirected_edges.push_back(edge_nodes);
                            }

                            if (*vec_str_itr == "inhibition") {
                                arrowhead = "tee";
                                label += "inhb ";
                            }

                            if (*vec_str_itr == "repression") {
                                arrowhead = "tee";
                                label += "repr ";
                            }

                            if (*vec_str_itr == "activation") {
                                label += "actv ";
                            }

                            if (*vec_str_itr == "expression") {
                                label += "expr ";
                            }

                            if (*vec_str_itr == "dephosphorylation") {
                                label += "-p ";
                            }

                            if (*vec_str_itr == "phosphorylation") {
                                label += "+p ";
                            }

                            if (*vec_str_itr == "indirect") {
                                label = "ind";
                                style = "dashed";
                            }
                            
                            if (*vec_str_itr == "PPI") {
                                label += "PPI ";

                            }
                            if (*vec_str_itr == "reaction") {
                                label = "reaction";
                            }                           
                            if (*vec_str_itr == "mapping") {
                                style = "dashed";
                                color = "blue";
                                
                            }
                        }

//                        if (edge_type == "PPrel") {
//                            label = label + "PPrel\\n";
//                        }
//                        if (edge_type == "GErel") {
//                            label = label + "GErel\\n";
//                        }
//                        if (edge_type == "PCrel") {
//                            label = label + "PCrel\\n";
//                        }
//                        if (edge_type == "ECrel") {
//                            label = label + "ECrel\\n";
//                        }

//                        for (vec_str_itr = edge_belongs_to_pathways.begin(); vec_str_itr != edge_belongs_to_pathways.end(); vec_str_itr++) {
//                            label += *vec_str_itr + "\\n";
//                        }
//                        for (vec_str_itr = edge_belongs_to_pathways.begin(); vec_str_itr != edge_belongs_to_pathways.end(); vec_str_itr++) {
//                            label += *vec_str_itr + "\\n";
//                        }

                        fout << "arrowhead=\"" << arrowhead << "\", arrowsize=0.7, style=\"" << style << "\", color=\"" << color <<  "\", label=< <TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n ";
                        EdgeNew * edge = get_edge_from_eid(*out_itr);
                        vector<string> all_path_vec = get_all_pathways_for_edge(*out_itr);
                        //fout << "<TR><TD TOOLTIP=\"" << get_all_display_ids_of_node(get_source_node(edge->get_edge_id())).front() << "-" << get_all_display_ids_of_node(get_target_node(edge->get_edge_id())).front() << "\">" << label << "</TD></TR>\n";
                        fout << "<TR><TD TOOLTIP=\"EDGE:" << (*out_itr) << "\"><FONT POINT-SIZE=\"10\" COLOR=\"black\">EDGE:" << (*out_itr) << "</FONT></TD></TR>" << endl;
                        fout << "<TR><TD><FONT POINT-SIZE=\"10\" COLOR=\"black\">" << label << "</FONT></TD></TR>\n";
                        for(int i = 0; i < all_path_vec.size(); i++){
                            
                            string curr_path = all_path_vec.at(i);
                            string pathway_name = gm->get_pathway_names_from_path_id(curr_path);
                            if(curr_path.substr(0,8) == "path:hsa"){
                                fout << "<TR><TD TOOLTIP=\"KEGG:" << pathway_name << "\" HREF=\"http://www.genome.jp/kegg-bin/show_pathway?" << curr_path.substr(5) << "\"><FONT POINT-SIZE=\"10\" COLOR=\"grey\">KEGG:" << curr_path.substr(5) << "</FONT></TD></TR>" << endl;
                            }
                            
                            else if(curr_path.substr(0,8) == "pathway_"){
                                fout << "<TR><TD TOOLTIP=\"Reactome:" << pathway_name << "\" HREF=\"http://www.reactome.org/content/detail/R-HSA-" << curr_path.substr(8) << "\"><FONT POINT-SIZE=\"10\" COLOR=\"grey\">Reactome:" << curr_path << "</FONT></TD></TR>";
                                
                            }
                            
//                            else{
//                                fout << "<TR><TD TOOLTIP=\"" << (*out_itr) << "\" ><FONT POINT-SIZE=\"10\" COLOR=\"grey\">" << curr_path << "</FONT></TD></TR>" << endl;
//                            }

//                            else if(curr_id.substr(0,7) == "uniprot"){
//                        myfile << "<TR><TD TOOLTIP=\"" << "UniProt: " << curr_id.substr(8) << "\" " << "HREF=\"http://www.uniprot.org/uniprot/" << curr_id.substr(8) << "\"><FONT POINT-SIZE=\"12\" COLOR=\"grey\">" << "UniProt: " << curr_id.substr(8) << "</FONT></TD></TR>" << endl;
//                    }
                        }
                        fout << "</TABLE>>" << endl;
                        
                        
                        fout << "]";
                        fout << ";" << "\n";

                    }
                }
            }
        }
        fout << "}";
        fout.close();

        string dot_path_name = DOT_PATH_NAME;
        string systemCommand1 = dot_path_name + "-Tcmapx " + dis_name + ".dot -o " + dis_name + ".cmapx & ";
        system(systemCommand1.c_str());

        string systemCommand2 = dot_path_name + "-Tsvg " + dis_name + ".dot -o " + dis_name + ".svg & ";
        system(systemCommand2.c_str());

        string systemCommand3 = "firefox " + dis_name + ".svg &";
        system(systemCommand3.c_str());

    } else {
        cerr << "\nCouldn't open file for writing: " << dis_name << endl;
    }
}

void GraphNew::write_to_xml_file(std::ofstream &fout) {
    string pathway;
    cout << "Enter the pathway name: " << endl;
    cin >> pathway;
    fout << "<?xml version=\"1.0\"?>" << endl;
    fout << "<!DOCTYPE pathway SYSTEM \"http://www.kegg.jp/kegg/xml/KGML_v0.7.1_.dtd\">" << endl;
    fout << "<pathway name=\"" + pathway + "\">" << endl;

    map<int, int> nid_to_xml_node_id_map;

    int xml_node_id = 1;
    for (vector<int>::iterator nid_itr = NIDSg.begin(); nid_itr != NIDSg.end(); ++nid_itr) {
        nid_to_xml_node_id_map[*nid_itr] = xml_node_id;
        ++xml_node_id;
    }

    //write nodes to the xml file
    int count = 0;
    for (vector<int>::iterator nid_itr = NIDSg.begin(); nid_itr != NIDSg.end(); ++nid_itr) {
        write_node_to_xml_file(*nid_itr, nid_to_xml_node_id_map, fout);
        count++;
    }
    cout << count << " nodes written to xml file" << endl;
    //write edges to the xml file
    count = 0;
    for (vector<int>::iterator eid_itr = EIDSg.begin(); eid_itr != EIDSg.end(); ++eid_itr) {
        write_edge_to_xml_file(*eid_itr, nid_to_xml_node_id_map, fout);
        count++;
    }
    cout << count << " edges written to xml file" << endl;
    //close pathway tag
    fout << "</pathway>" << endl;
}

GraphManagerNew::GraphManagerNew() {
    while (true) {
        std::string input_file, line;
//        std::cout << "\nEnter the file path for name mapping file: ";
//        std::cin >> input_file;
        input_file = "empty";
                
        std::ifstream fin;
        fin.open(input_file.c_str(), std::ifstream::in);
        if (fin.is_open()) {
            while (getline(fin, line)) {
                std::stringstream s(line);
                std::string id, repid;
                s >> id >> repid;
                all_id_map[id] = repid;
            }
            fin.close();


        }
        else{
            std::cerr << "\nUnable to open the file " + input_file << std::endl;
        }


        std::string kegg_map_file, entry;
//        std::cout << "Enter file path of id to display name map: ";
//        std::cin >> kegg_map_file;
        kegg_map_file = "all_names.txt";
        //kegg_map_file = "empty";

        std::ifstream kegg_in;
        kegg_in.open(kegg_map_file.c_str(), std::ifstream::in);
        if (kegg_in.is_open()) {
            while (getline(kegg_in, entry)) {
                std::stringstream k(entry);
                std::string hsa_id, name;
                k >> hsa_id >> name;
                kegg_hsa_id_to_display_name_map[hsa_id] = name;
            }
            kegg_in.close();
            std::cout << kegg_hsa_id_to_display_name_map.size() << std::endl;
            
        } 
        else {
            std::cerr << "\nUnable to open file " + kegg_map_file << std::endl;
        }
        
        std::string pathway_file, entry1;
//        std::cout << "Enter file path for pathway name map: ";
//        std::cin >> pathway_file;
        pathway_file = "paths.txt";
        
        
        std::ifstream path_in;
        path_in.open(pathway_file.c_str(), std::ifstream::in);
        if (path_in.is_open()) {
            while (getline(path_in, entry1)) {
                std::stringstream l(entry1);
                std::string path_id, path_name;
                l >> path_id >> path_name;
                pathway_name_map[path_id] = path_name;
            }
            path_in.close();
            std::cout << pathway_name_map.size() << std::endl;
            break;
        } else
            std::cerr << "\nUnable to open file " + pathway_file << std::endl;

    }
}

GraphManagerNew::~GraphManagerNew() {

}

template<typename Key, typename Val>
void GraphManagerNew::add_to_map(std::map<Key, Val>& generic_map, Key k, Val v) {
    generic_map.insert(pair<Key, Val>(k, v));
}

template<typename Key, typename Val>
void GraphManagerNew::delete_from_map(std::map<Key, Val>& generic_map, Key k) {
    generic_map.erase(k);
}

template<typename Key, typename Val>
void GraphManagerNew::print_map(std::map<Key, Val>& generic_map, ofstream& fout) {
    for (typename map<Key, Val>::iterator itr = generic_map.begin(); itr != generic_map.end(); ++itr) {
        fout << itr->first << " ==> " << itr->second << endl;
    }
    fout << endl;
}

template<typename Data>
bool GraphManagerNew::is_subset(std::vector<Data> smaller_set, std::vector<Data> bigger_set) {
    sort(smaller_set.begin(), smaller_set.end());
    sort(bigger_set.begin(), bigger_set.end());
    vector<string> diff_set;
    set_difference(smaller_set.begin(), smaller_set.end(), bigger_set.begin(), bigger_set.end(), inserter(diff_set, diff_set.begin()));
    return diff_set.empty();
}

template<typename Data>
bool GraphManagerNew::intersection_empty(std::vector<Data> left_set, std::vector<Data> right_set) {
    sort(left_set.begin(), left_set.end());
    sort(right_set.begin(), right_set.end());
    vector<string> intersect_set;
    set_intersection(left_set.begin(), left_set.end(), right_set.begin(), right_set.end(), inserter(intersect_set, intersect_set.begin()));
    return intersect_set.empty();
}

void GraphManagerNew::read_edge_type_subtype_map() {
    string mapping_filename;
    cout << "Enter file for edge type/subtype mapping: ";
    cin >> mapping_filename;

    string line, delim, type;
    delim = " ";
    ifstream ifs(mapping_filename.c_str());
    cout << edge_type_subtype_map.size() << endl;
    if (ifs.is_open()) {
        while (getline(ifs, line)) {

            list<string> * tokens = get_list_of_names_from_string(line, delim);
            type = tokens->front();
            tokens->pop_front();
            list<string> subtype_list;
            while (!tokens->empty()) {
                subtype_list.push_back(tokens->front());
                tokens->pop_front();
            }
            edge_type_subtype_map[type] = subtype_list;
        }
        ifs.close();
        cout << edge_type_subtype_map.size() << endl;
    } else
        cerr << "Could not open file " << mapping_filename << endl;

}

inline map<string, list<string> > GraphManagerNew::get_edge_type_subtype_map() {
    return edge_type_subtype_map;
}

void GraphManagerNew::read_naming_map(string filename, unordered_map<string, set<string> > &nmap) {
    std::ifstream fin;
    string line;
    fin.open(filename.c_str(), std::ifstream::in);
    if (fin.is_open()) {
        while (getline(fin, line)) {
            stringstream l(line);
            string entry1, entry2;
            l >> entry1 >> entry2;
            set<string>& value_set1 = nmap[entry1];
            value_set1.insert(entry2);
            
            set<string>& value_set2 = nmap[entry2];
            value_set2.insert(entry1);
            
            //nmap[entry1].insert(entry2);
            //nmap[entry2].insert(entry1);
        }
        fin.close();
//        for (unordered_map<string, set<string> >::iterator itr = nmap.begin(); itr != nmap.end(); itr++) {
//            cout << (*itr).first << " :";
//            set<string> value_set = (*itr).second;
//            for (set<string>::iterator set_itr = value_set.begin(); set_itr != value_set.end(); set_itr++) {
//                cout << " " << (*set_itr);
//            }
//            cout << endl;
//        }
        cout << filename << " has " << nmap.size() << " entries" << endl;
    }
    else
        cerr << "\nUnable to open file " << filename << endl;
   
}
/*std::ifstream fin;
        fin.open(input_file.c_str(), std::ifstream::in);
        if (fin.is_open()) {
            while (getline(fin, line)) {
                std::stringstream s(line);
                std::string id, repid;
                s >> id >> repid;
                all_id_map[id] = repid;
            }
            fin.close();


        }
        else{
            std::cerr << "\nUnable to open the file " + input_file << std::endl;
        }*/
        
inline GraphNew* GraphManagerNew::get_graph(int gid) {
    std::map<int, GraphNew*>::iterator itr = graph_id_to_graph_map.find(gid);
    if (itr != graph_id_to_graph_map.end()) {
        return itr->second;
    } else {
        return NULL;
    }
}

inline int GraphManagerNew::get_latest_graph_id() {
    return graph_id_to_graph_map.end()->first;
}

inline void GraphManagerNew::add_graph(int gid, GraphNew* g) {
    graph_id_to_graph_map.insert(pair<int, GraphNew*>(gid, g));
}

GraphNew * GraphManagerNew::duplicate_graph(int old_gid) {
    GraphNew * old_graph = get_graph(old_gid);

    GraphNew * new_graph = new GraphNew();
    // creating and adding graph_id
    GraphManagerNew::graph_id_count++;
    int gid = GraphManagerNew::graph_id_count;
    new_graph->set_graph_id(gid);
    graph_id_to_graph_map[gid] = new_graph;

    // get all node ids from the old_graph
    std::vector<int> NIDSg1 = old_graph->get_node_ids();
    std::vector<int>::iterator itr;

    // copy all nodes from old_graph
    //cout << "start duplicating nodes of graph " << graph1->get_graph_id() << endl;
    for (itr = NIDSg1.begin(); itr != NIDSg1.end(); itr++) {
        int new_node_id = duplicate_node(new_graph, old_graph, (*itr));
    }
    // get all edge ids from the old_graph
    std::vector<int> EIDSg1 = old_graph->get_edge_ids();

    // copy all edges from old_graph
    for (itr = EIDSg1.begin(); itr != EIDSg1.end(); itr++) {
        int new_edge_id = duplicate_edge(new_graph, old_graph, (*itr));
    }


    // adding genesis information
    std::list<int> glist;
    glist.push_back(old_graph->get_graph_id());
    int result_gid = new_graph->get_graph_id();
    add_graph_genesis(result_gid, "Duplicated graph", glist);

    return new_graph;

}


int GraphManagerNew::convert_to_undirected_graph(int gid) {
    int new_gid = -1;

    GraphNew * old_graph_ptr = get_graph(gid);
    if (old_graph_ptr == NULL) {
        cerr << "Error: there is no graph for id " << gid << endl;
        return -1;
    }
    
    //new id for the new graph
    GraphNew* new_graph_ptr = new GraphNew;
    new_gid = ++GraphManagerNew::graph_id_count;
    new_graph_ptr->set_graph_id(new_gid);
    add_graph(new_gid, new_graph_ptr);
    
    
    // iterate and duplicate edges from original graph
    vector<int> eids = old_graph_ptr->get_edge_ids();
    vector<int>::iterator vec_itr;
    for (vec_itr = eids.begin(); vec_itr != eids.end(); vec_itr++) {
        int curr_eid = *vec_itr;
        int new_eid = -1;
        EdgeNew * curr_edge = old_graph_ptr->get_edge_from_eid(curr_eid);
        int curr_edge_src = old_graph_ptr->get_source_node(curr_eid);
        
        // if it is an undirected edge - no need to create the reverse edge, it will already exist
        if ((curr_edge->has_subtype("association")) || (curr_edge->has_subtype("dissociation")) || (curr_edge->has_subtype("mapping"))) {
            new_eid = duplicate_edge(new_graph_ptr, old_graph_ptr, curr_eid);
        }
        
        // if not an undirected edge
        else {
            // duplicate this edge
            new_eid = duplicate_edge(new_graph_ptr, old_graph_ptr, curr_eid);
            
            // check if reverse edge is present in original graph
            int curr_edge_tgt = curr_edge->get_target();
            vector<int> outlist_of_tgt = old_graph_ptr->get_outlist(curr_edge_tgt);
            vector<int>::iterator vec_itr2;
            
            bool rev_edge_exists = false;
            for (vec_itr2 = outlist_of_tgt.begin(); vec_itr2 != outlist_of_tgt.end(); vec_itr2++) {
                if (old_graph_ptr->get_target_node(*vec_itr2) == curr_edge_src) {
                    rev_edge_exists = true;
                }
            }
            
            // if not present, create rev edge
            if (rev_edge_exists == false) {
                int rev_eid = new_graph_ptr->create_new_edge();
                if (rev_eid == -1) {
                    cerr << "Problem creating reverse edge" << endl;
                }
                new_graph_ptr->add_edge_id(rev_eid);
                add_edge_id_graph_id(rev_eid, new_graph_ptr->get_graph_id());
                new_graph_ptr->add_edge_type(rev_eid, new_graph_ptr->get_edge_type(new_eid));
                new_graph_ptr->add_edge_to_outlist_of_node(new_graph_ptr->get_target_node(new_eid), rev_eid);
                new_graph_ptr->add_edge_to_inlist_of_node(new_graph_ptr->get_source_node(new_eid), rev_eid);
                new_graph_ptr->add_source_node(rev_eid, new_graph_ptr->get_target_node(new_eid));
                new_graph_ptr->add_target_node(rev_eid, new_graph_ptr->get_source_node(new_eid));
                new_graph_ptr->add_subtype_for_edge(rev_eid, "dummy");
                new_graph_ptr->add_pathway_for_edge(rev_eid, "dummy");
            }
            
        }
    }
    
    
    
    //for genesis
    string oper = "create undirected graph from ";
    list<int> gid_list;
    gid_list.push_back(new_gid);
    add_graph_genesis(new_gid, oper, gid_list);
    return new_gid;
}



void GraphManagerNew::add_graph_genesis(int gid, std::string oper, std::list<int> oper_graph_ids) {
    graph_genesis_new *gen = new graph_genesis_new(oper, oper_graph_ids);
    std::map<int, graph_genesis_new>::iterator itr = graph_id_to_genesis_map.find(gid);
    if (itr != graph_id_to_genesis_map.end()) {
        graph_id_to_genesis_map.erase(itr);
    }
    graph_id_to_genesis_map.insert(pair<int, graph_genesis_new>(gid, *gen));

}

void GraphManagerNew::print_genesis_new(int gid, std::ostream &fout) {
    fout << "Graph " << gid << ": ";
    std::map<int, graph_genesis_new>::iterator gen_itr = graph_id_to_genesis_map.find(gid);
    if (gen_itr != graph_id_to_genesis_map.end()) {
        fout << (*gen_itr).second.oper << " ";
        list<int> oper_graph_ids = (*gen_itr).second.operand_graph_ids;
        list<int>::iterator itr;
        for (itr = oper_graph_ids.begin(); itr != oper_graph_ids.end(); itr++) {
            fout << " " << (*itr);
        }
        fout << endl;
    } else
        fout << "No genesis info for graph " << gid << endl;
}

inline void GraphManagerNew::delete_graph_id_graph(int gid) {
    GraphNew* graph_to_delete = get_graph(gid);
    if (graph_to_delete == NULL) {
        cerr << "Error: graph with id " << gid << " doesn't exist ..., so the question of its deletion doesn't arise" << endl;
        return;
    }
    delete_from_map(graph_id_to_graph_map, gid);
    delete graph_to_delete;

}

inline int GraphManagerNew::get_graph_id_for_node(int nid) {
    std::map<int, int>::iterator itr = node_id_to_graph_id.find(nid);
    if (itr != node_id_to_graph_id.end()) {
        return itr->second;
    } else {
        return -1;
    }
}

inline int GraphManagerNew::get_graph_id_for_edge(int eid) {
    std::map<int, int>::iterator itr = edge_id_to_graph_id.find(eid);
    if (itr != edge_id_to_graph_id.end()) {
        return itr->second;
    } else {
        return -1;
    }
}

inline void GraphManagerNew::add_node_id_graph_id(int nid, int gid) {
    node_id_to_graph_id.insert(pair<int, int>(nid, gid));
}

inline void GraphManagerNew::delete_node_id_graph_id(int nid) {
    delete_from_map(node_id_to_graph_id, nid);
}

inline void GraphManagerNew::add_edge_id_graph_id(int eid, int gid) {
    edge_id_to_graph_id.insert(pair<int, int>(eid, gid));
}

inline void GraphManagerNew::delete_edge_id_graph_id(int eid) {
    delete_from_map(edge_id_to_graph_id, eid);
}

inline std::string GraphManagerNew::get_rep_id_for_id(std::string id) {
    std::map<std::string, std::string>::iterator itr = all_id_map.find(id);
    if (itr != all_id_map.end()) {
        return itr->second;
    } else {
        return "";
    }
}

void GraphManagerNew::add_rep_id_for_id(std::string id, std::string repid) {
    add_to_map(all_id_map, id, repid);
}


std::vector<std::string> GraphManagerNew::get_all_ids_for_rep_id(std::string rep_id) {
    std::vector<std::string> all_ids_for_repid;
    std::map<std::string, std::string>::iterator itr;
    for (itr = all_id_map.begin(); itr != all_id_map.end(); ++itr) {
        if (itr->second == rep_id) {
            all_ids_for_repid.push_back(itr->first);
        }
    }
    return all_ids_for_repid;
}

std::string GraphManagerNew::get_identifier(std::string key) {
    std::map<std::string, std::string>::iterator map_iter = identifier_map.find(key);
    if (map_iter == identifier_map.end()) {
        return "";
    }
    else {
        return (*map_iter).second;
    }

}

inline std::string GraphManagerNew::get_display_names_from_rep_id(std::string rep_id) {
    std::map<std::string, std::string>::iterator map_iter = kegg_hsa_id_to_display_name_map.find(rep_id);
    if (map_iter == kegg_hsa_id_to_display_name_map.end()) {
        //cout << "Rep id " << rep_id << " not found in KEGG map" << endl;
        return rep_id;
    } else {
        return (*map_iter).second;
    }

}

inline std::string GraphManagerNew::get_pathway_names_from_path_id(std::string path_id) {
    std::map<std::string, std::string>::iterator map_iter = pathway_name_map.find(path_id);
    if (map_iter == pathway_name_map.end()) {
        //cout << "Rep id " << rep_id << " not found in KEGG map" << endl;
        return path_id;
    } else {
        return (*map_iter).second;
    }

}
//int GraphManagerNew::check_if_node_already_created(GraphNew * new_graph, std::string rep_id) {
//        // get all node ids from new graph
//        std::vector<int> all_nids = new_graph->get_node_ids();
//        std::vector<int>::iterator all_nid_itr;
//        
//        // for each node in new graph
//        for (all_nid_itr = all_nids.begin(); all_nid_itr != all_nids.end(); all_nid_itr++) {
//            // if node with same rep_id present in new graph, return that node
//            if (new_graph->get_rep_id_from_nid(*all_nid_itr) == rep_id) {
//            
//                 return (*all_nid_itr);
//            }
//        }
//        
//        return -1;
//        
//}

//int GraphManagerNew::check_if_edge_already_created(GraphNew * new_graph, int new_source_nid, int new_target_nid, std::string edge_type, std::vector<std::string> edge_subtypes) {
//        // get all edge ids from new graph
//        std::vector<int> all_eids = new_graph->get_edge_ids();
//        std::vector<int>::iterator all_eid_itr;
//        
//        // for each edge in new graph
//        for (all_eid_itr = all_eids.begin(); all_eid_itr != all_eids.end(); all_eid_itr++) {
//            // if source, target and edge type and subtypes match return that edge id -- not considering belongs to pathways
//            if(new_graph->get_source_node(*all_eid_itr) == new_source_nid &&
//               new_graph->get_target_node(*all_eid_itr) == new_target_nid &&
//               new_graph->get_edge_type(*all_eid_itr) == edge_type &&
//               new_graph->get_edge_subtypes_of_edge(*all_eid_itr) == edge_subtypes) {
//                        
//                return (*all_eid_itr);
//            }
//        }
//        return -1;
//}

int GraphManagerNew::duplicate_node(GraphNew * new_graph, GraphNew * old_graph, int nid, bool duplicate_check_flag) {

    if (new_graph == NULL || old_graph == NULL) {
        cerr << "Error in node duplication: the graphs are NULL" << endl;
        return -1;
    }
    if (get_graph_id_for_node(nid) != old_graph->get_graph_id()) {
        cerr << "Error in node duplication: nid vs graph mismatch:" << get_graph_id_for_node(nid) << " vs " << old_graph->get_graph_id() << endl;
        return -1;
    }

    // get node info from old graph
    std::string node_type = old_graph->get_node_type(nid);
    std::string rep_id = old_graph->get_rep_id_from_nid(nid); //
    std::vector<std::string> all_ids = old_graph->get_all_ids_of_node(nid);
    std::vector<std::string> disp_names = old_graph->get_all_display_ids_of_node(nid);
    std::vector<int> comp_ids = old_graph->get_component_ids_of_node(nid);

    // check if node with same rep_id already created
    int nid_to_check = (!duplicate_check_flag) ? -1 : new_graph->check_if_node_already_created(rep_id);
    if (nid_to_check != -1) {
        // return existing node id
        return nid_to_check;

    }

    // create new node
    //GraphManagerNew::node_id_count++;
    //int new_nid = GraphManagerNew::node_id_count;   // created id of new node
    int new_nid = new_graph->create_new_node();
    if (new_nid == -1) {
        cerr << "Error in creating new node" << endl;
    }

    int new_graph_id = new_graph->get_graph_id();
    add_node_id_graph_id(new_nid, new_graph_id);
    new_graph->add_node_id(new_nid);
    new_graph->add_rep_id_for_node(new_nid, rep_id); // added by sukanya

    // add node info on new node
    new_graph->add_node_type(new_nid, node_type);

    //re-added by harshit*** - to fill the map id to rep id
    std::vector<std::string>::iterator itr;
    for (itr = all_ids.begin(); itr != all_ids.end(); itr++) {
        new_graph->add_id_rep_id((*itr), rep_id); //
    }
    //***
    new_graph->set_all_ids_for_node(new_nid, all_ids);

    	for (itr = disp_names.begin(); itr != disp_names.end(); itr++) {
    		new_graph->add_display_id_for_node(new_nid, (*itr));
    	}
    new_graph->set_all_display_id_for_node(new_nid, disp_names);
    string desc = old_graph->get_description_for_node(nid);
    new_graph->add_description_for_node(new_nid, desc);
    // components
    if (node_type == "group") {
        std::vector<int>::iterator comp_it;
        for (comp_it = comp_ids.begin(); comp_it != comp_ids.end(); comp_it++) {

#ifdef DUP_CHECK_FLAG_SET
            int new_comp_node_id = duplicate_node(new_graph, old_graph, (*comp_it), duplicate_check_flag);
#else
            int new_comp_node_id = duplicate_node(new_graph, old_graph, (*comp_it));
#endif                    

            new_graph->add_component_id_for_node(new_nid, new_comp_node_id);
        }
    }

    return new_nid;
}

int GraphManagerNew::duplicate_edge(GraphNew * new_graph, GraphNew * old_graph, int eid, bool duplicate_check_flag) {
    if (new_graph == NULL || old_graph == NULL) {
        cerr << "Error in edge duplication: the graphs are NULL" << endl;
        return -1;
    }
    if (get_graph_id_for_edge(eid) != old_graph->get_graph_id()) {
        cerr << "Error in edge duplication: eid vs graph mismatch" << endl;
        return -1;
    }

    // get corresponding source and target nodes in the new graph
    int old_source_nid = old_graph->get_source_node(eid);
    int old_target_nid = old_graph->get_target_node(eid);
    int new_source_nid = new_graph->get_nid_from_rep_id(old_graph->get_rep_id_from_nid(old_source_nid));
    if (new_source_nid == -1) {//the node is not there in the new graph

#ifdef DUP_CHECK_FLAG_SET
        new_source_nid = duplicate_node(new_graph, old_graph, old_source_nid, duplicate_check_flag);
#else
        new_source_nid = duplicate_node(new_graph, old_graph, old_source_nid);
#endif            

    }
    int new_target_nid = new_graph->get_nid_from_rep_id(old_graph->get_rep_id_from_nid(old_target_nid));
    if (new_target_nid == -1) {//the node is not there in the new graph
#ifdef DUP_CHECK_FLAG_SET
        new_target_nid = duplicate_node(new_graph, old_graph, old_target_nid, duplicate_check_flag);
#else
        new_target_nid = duplicate_node(new_graph, old_graph, old_target_nid);
#endif            

    }
    // get edge info from old graph
    //cout << "getting type of edge from old graph" << endl;
    std::string edge_type = old_graph->get_edge_type(eid);
    //cout << "getting subtypes from old graph" << endl;
    std::vector<std::string> edge_subtypes = old_graph->get_edge_subtypes_of_edge(eid);
    //cout << "getting belong pathways from old graph" << endl;
    std::vector<std::string> edge_belongs_to_pathways = old_graph->get_all_pathways_for_edge(eid);

    // check if edge between source and target exist with same labels
    int eid_to_check = (!duplicate_check_flag) ? -1 : new_graph->check_if_edge_already_created(new_source_nid, new_target_nid, edge_type, edge_subtypes);
    if (eid_to_check != -1) {
        // add pathway info for existing edge
        std::vector<std::string>::iterator path_itr;
        for (path_itr = edge_belongs_to_pathways.begin(); path_itr != edge_belongs_to_pathways.end(); path_itr++) {
            std::string path = *path_itr;
            if (!new_graph->edge_belongs_to_pathway(eid_to_check, path)) {
                new_graph->add_pathway_for_edge(eid_to_check, path);
            }
        }
        // return existing edge id
        return eid_to_check;
    }

    // create new edge
    //GraphManagerNew::edge_id_count++;
    //int new_eid = GraphManagerNew::edge_id_count;
    int new_eid = new_graph->create_new_edge();
    if (new_eid == -1) {
        cerr << "Error in creating new edge" << endl;
    }

    int new_graph_id = new_graph->get_graph_id();
    add_edge_id_graph_id(new_eid, new_graph_id);
    new_graph->add_edge_id(new_eid);

    // add edge info on new edge
    new_graph->add_source_node(new_eid, new_source_nid);
    new_graph->add_target_node(new_eid, new_target_nid);
    new_graph->add_edge_type(new_eid, edge_type);
    std::vector<std::string>::iterator itr;
    for (itr = edge_subtypes.begin(); itr != edge_subtypes.end(); itr++) {
        new_graph->add_subtype_for_edge(new_eid, (*itr));
    }
    for (itr = edge_belongs_to_pathways.begin(); itr != edge_belongs_to_pathways.end(); itr++) {
        new_graph->add_pathway_for_edge(new_eid, (*itr));
    }

    //insert new edge in outlist of source and inlist of target nodes
    new_graph->add_edge_to_outlist_of_node(new_source_nid, new_eid);
    new_graph->add_edge_to_inlist_of_node(new_target_nid, new_eid);

    return new_eid;
}



//
//GraphNew * GraphManagerNew::merge_two_graphs(GraphNew * graph1, GraphNew * graph2) {
//    map<string, string> submap;
//    map<string, string>::iterator submap_iter;
//    string id_key, id_value;
//        GraphNew * new_graph = new GraphNew();
//        // creating and adding graph_id
//        GraphManagerNew::graph_id_count++;
//        int gid = GraphManagerNew::graph_id_count;
//        new_graph->set_graph_id(gid);
//        graph_id_to_graph_map[gid] = new_graph;
//    
//        // get all node ids from the two graphs to merge
//        std::set<int> NIDSg1 = graph1->get_node_ids();
//        std::set<int> NIDSg2 = graph2->get_node_ids();
//        std::set<int>::iterator itr;
//        
//        // copy all nodes from graph1
//        //cout << "start duplicating nodes of graph " << graph1->get_graph_id() << endl;
//        id_value = "";
//        for(itr = NIDSg1.begin(); itr != NIDSg1.end(); itr++) {
//            int new_node_id = duplicate_node(new_graph, graph1, (*itr));
//            id_key = new_graph->get_rep_id_from_nid(new_node_id);
//            id_value = get_identifier(id_key);
//            while (id_value != "") {  
//                submap.insert(pair<string, string>(id_key, id_value));
//                id_key = id_value;
//                id_value = get_identifier(id_key);
//            }
//        }
//        
//        // copy all nodes from graph2
//        for(itr = NIDSg2.begin(); itr != NIDSg2.end(); itr++) {
//            
//            // check for conflicting types
//            string type_in_graph1 = "";
//            int nid_in_graph1 = graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr));
//            if (nid_in_graph1 != -1){ // found node in graph1
//                type_in_graph1 = graph1->get_node_type(nid_in_graph1);
//            }
//            //string type_in_graph1 = graph1->get_node_type(graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr)));
//            if(type_in_graph1 != "" && type_in_graph1 != graph2->get_node_type(*itr)) {
//                //cerr << "Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << " which is " << graph2->get_node_type(*itr) <<  " in graph " << graph2->get_graph_id() << " and " << type_if_graph1 << " in prev graphs. ";
//                cerr << "Warning: Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << ". Not merging graph " << graph2->get_graph_id() << "." << endl;
//                GraphManagerNew::graph_id_count--;
//                destroy_graph(new_graph);
//                return graph1;
//                
//            }
//            
//            int new_node_id = duplicate_node(new_graph, graph2, (*itr));
//            id_key = new_graph->get_rep_id_from_nid(new_node_id);
//            id_value = get_identifier(id_key);
//            while (id_value != "") {  
//                submap.insert(pair<string, string>(id_key, id_value));
//                id_key = id_value;
//                id_value = get_identifier(id_key);
//            }
//        }
//        cout << new_graph->get_node_ids().size() << endl;
//        
//        // iterate over submap and create new intermediate nodes and corresponding edges
//        set<string> intermed_nodes_created;
//        
//        for (submap_iter = submap.begin(); submap_iter != submap.end(); submap_iter++){
//            string src_repid = (*submap_iter).first;
//            if (new_graph->check_if_node_already_created(src_repid) != -1){
//                string intermed_repid = get_identifier(src_repid);
//                
//                if(intermed_repid == "")
//                    continue;
//                string tgt_repid = get_identifier(intermed_repid);
//                if (new_graph->check_if_node_already_created(tgt_repid) != -1){
//                    int nid = -1;
//                    // create new intermediate node
//                    //if (new_graph->check_if_node_already_created(intermed_repid) == -1){
//                    if(intermed_nodes_created.find(intermed_repid) == intermed_nodes_created.end()) {
//                        nid = new_graph->create_new_node(); // creates a new node -- added by sukanya
//                        new_graph->add_node_id(nid);
//                        new_graph->add_rep_id_for_node(nid, intermed_repid); // added by sukanya
//                        add_node_id_graph_id(nid, new_graph->get_graph_id());
//                        vector<string> all_id_for_rid = get_all_ids_for_rep_id(intermed_repid);
//                        all_id_for_rid.push_back(intermed_repid); // specially for this node
//                        for(vector<string>::iterator all_id_for_rid_itr = all_id_for_rid.begin(); all_id_for_rid_itr != all_id_for_rid.end(); all_id_for_rid_itr++){
//                                new_graph->add_id_for_node(nid, *all_id_for_rid_itr);
//                                new_graph->add_id_rep_id(*all_id_for_rid_itr, intermed_repid); // commented out by sukanya
//                        }
//                        new_graph->add_node_type(nid, "protein");
//                        new_graph->add_display_id_for_node(nid, get_display_names_from_rep_id(intermed_repid));
//                        
//                        intermed_nodes_created.insert(intermed_repid);
//                    }
//                    else {
//                        nid = new_graph->get_nid_from_rep_id(intermed_repid);
//                    }
//                    
//                    // create new edge to/from intermediate node
//                    int source_node_id = new_graph->get_nid_from_rep_id(src_repid);
//                    int target_node_id = new_graph->get_nid_from_rep_id(tgt_repid);
//                    int eid = new_graph->create_new_edge(); // creates a new edge -- added by sukanya
//                    new_graph->add_edge_id(eid); // added by sukanya
//                    add_edge_id_graph_id(eid, new_graph->get_graph_id());
//                    new_graph->add_edge_type(eid, "mapping");
//                    new_graph->add_edge_to_outlist_of_node(source_node_id, eid);
//                    new_graph->add_edge_to_inlist_of_node(nid, eid);
//                    new_graph->add_source_node(eid, source_node_id);
//                    new_graph->add_target_node(eid, nid);
//                    new_graph->add_subtype_for_edge(eid, "mapping");
//                    new_graph->add_pathway_for_edge(eid, " ");
//
//                    eid = new_graph->create_new_edge(); // creates a new edge -- added by sukanya
//                    new_graph->add_edge_id(eid); // added by sukanya
//                    add_edge_id_graph_id(eid, new_graph->get_graph_id());
//                    new_graph->add_edge_type(eid, "mapping");
//                    new_graph->add_edge_to_outlist_of_node(nid, eid);
//                    new_graph->add_edge_to_inlist_of_node(source_node_id, eid);
//                    new_graph->add_source_node(eid, nid);
//                    new_graph->add_target_node(eid, source_node_id);
//                    new_graph->add_subtype_for_edge(eid, "mapping");
//                    new_graph->add_pathway_for_edge(eid, " ");
//                    
//                    eid = new_graph->create_new_edge(); // creates a new edge -- added by sukanya
//                    new_graph->add_edge_id(eid); // added by sukanya
//                    add_edge_id_graph_id(eid, new_graph->get_graph_id());
//                    new_graph->add_edge_type(eid, "mapping");
//                    new_graph->add_edge_to_outlist_of_node(target_node_id, eid);
//                    new_graph->add_edge_to_inlist_of_node(nid, eid);
//                    new_graph->add_source_node(eid, target_node_id);
//                    new_graph->add_target_node(eid, nid);
//                    new_graph->add_subtype_for_edge(eid, "mapping");
//                    new_graph->add_pathway_for_edge(eid, " ");
//
//                    eid = new_graph->create_new_edge(); // creates a new edge -- added by sukanya
//                    new_graph->add_edge_id(eid); // added by sukanya
//                    add_edge_id_graph_id(eid, new_graph->get_graph_id());
//                    new_graph->add_edge_type(eid, "mapping");
//                    new_graph->add_edge_to_outlist_of_node(nid, eid);
//                    new_graph->add_edge_to_inlist_of_node(target_node_id, eid);
//                    new_graph->add_source_node(eid, nid);
//                    new_graph->add_target_node(eid, target_node_id);
//                    new_graph->add_subtype_for_edge(eid, "mapping");
//                    new_graph->add_pathway_for_edge(eid, " ");
//
//                    
//                }
//            }
//        }
//        
//        // get all edge ids from the two graphs to merge
//        std::set<int> EIDSg1 = graph1->get_edge_ids();
//        std::set<int> EIDSg2 = graph2->get_edge_ids();
//        
//        // copy all edges from graph1
//        for(itr = EIDSg1.begin(); itr != EIDSg1.end(); itr++) {
//            int new_edge_id = duplicate_edge(new_graph, graph1, (*itr));
//        }
//        
//        // copy all edges from graph2
//        for(itr = EIDSg2.begin(); itr != EIDSg2.end(); itr++) {
//            int new_edge_id = duplicate_edge(new_graph, graph2, (*itr));
//        }
//    
//        // adding genesis information
//        std::list<int> glist;
//        glist.push_back(graph1->get_graph_id());
//        glist.push_back(graph2->get_graph_id());
//        int result_gid = new_graph->get_graph_id();
//        add_graph_genesis(result_gid, "Merged graphs", glist);
//        
//        return new_graph;
//}


GraphNew * GraphManagerNew::merge_two_graphs(GraphNew * graph1, GraphNew * graph2, list<unordered_map<string, set<string> > >& map_list) {
    
    set<string> all_ids_in_graph;
    set<string>::iterator all_ids_in_graph_iter;
    
    GraphNew * new_graph = new GraphNew();
        // creating and adding graph_id
        GraphManagerNew::graph_id_count++;
        int gid = GraphManagerNew::graph_id_count;
        new_graph->set_graph_id(gid);
        graph_id_to_graph_map[gid] = new_graph;
    
        // get all node ids from the two graphs to merge
        int nid;
        std::vector<int> NIDSg1 = graph1->get_node_ids();
        std::vector<int> NIDSg2 = graph2->get_node_ids();
        std::vector<int>::iterator itr;
        
        // copy all nodes from graph1
        //cout << "start duplicating nodes of graph " << graph1->get_graph_id() << endl;
        
        for(itr = NIDSg1.begin(); itr != NIDSg1.end(); itr++) {
            int new_node_id = duplicate_node(new_graph, graph1, (*itr));
            vector<string> all_id_of_node = new_graph->get_all_ids_of_node(new_node_id);
            vector<string>::iterator all_id_node_iter;
            for(all_id_node_iter = all_id_of_node.begin(); all_id_node_iter != all_id_of_node.end(); all_id_node_iter++) {
                all_ids_in_graph.insert(*all_id_node_iter);
            }
        }
        
        // copy all nodes from graph2
        for(itr = NIDSg2.begin(); itr != NIDSg2.end(); itr++) {
            
            // check for conflicting types
            string type_in_graph1 = "";
            int nid_in_graph1 = graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr));
            if (nid_in_graph1 != -1){ // found node in graph1
                type_in_graph1 = graph1->get_node_type(nid_in_graph1);
            }
            //string type_in_graph1 = graph1->get_node_type(graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr)));
            if(type_in_graph1 != "" && type_in_graph1 != graph2->get_node_type(*itr)) {
                //cerr << "Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << " which is " << graph2->get_node_type(*itr) <<  " in graph " << graph2->get_graph_id() << " and " << type_if_graph1 << " in prev graphs. ";
                cerr << "Warning: Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << ". Not merging graph " << graph2->get_graph_id() << "." << endl;
                GraphManagerNew::graph_id_count--;
                destroy_graph(new_graph);
                return graph1;
                
            }
            
            int new_node_id = duplicate_node(new_graph, graph2, (*itr));
            vector<string> all_id_of_node = new_graph->get_all_ids_of_node(new_node_id);
            vector<string>::iterator all_id_node_iter;
            for(all_id_node_iter = all_id_of_node.begin(); all_id_node_iter != all_id_of_node.end(); all_id_node_iter++) {
                all_ids_in_graph.insert(*all_id_node_iter);
            }
        }
        
        
        // get all edge ids from the two graphs to merge
        std::vector<int> EIDSg1 = graph1->get_edge_ids();
        std::vector<int> EIDSg2 = graph2->get_edge_ids();
        
        // copy all edges from graph1
        for(itr = EIDSg1.begin(); itr != EIDSg1.end(); itr++) {
            int new_edge_id = duplicate_edge(new_graph, graph1, (*itr));
        }
        
        // copy all edges from graph2
        for(itr = EIDSg2.begin(); itr != EIDSg2.end(); itr++) {
            int new_edge_id = duplicate_edge(new_graph, graph2, (*itr));
        }
        

        

    
        // adding genesis information
        std::list<int> glist;
        glist.push_back(graph1->get_graph_id());
        glist.push_back(graph2->get_graph_id());
        int result_gid = new_graph->get_graph_id();
        add_graph_genesis(result_gid, "Merged graphs", glist);
        
        return new_graph;
}



/*
GraphNew * GraphManagerNew::merge_two_graphs(GraphNew * graph1, GraphNew * graph2) {

    map<string, string> all_id_map_subset;
    map<string, string>::iterator all_id_map_itr;
    string curr_rep_id;
    GraphNew * new_graph = new GraphNew();
    // creating and adding graph_id
    GraphManagerNew::graph_id_count++;
    int gid = GraphManagerNew::graph_id_count;
    new_graph->set_graph_id(gid);
    graph_id_to_graph_map[gid] = new_graph;

    // get all node ids from the two graphs to merge
    std::set<int> NIDSg1 = graph1->get_node_ids();
    std::set<int> NIDSg2 = graph2->get_node_ids();
    std::set<int>::iterator itr;

    // copy all nodes from graph1
    //cout << "start duplicating nodes of graph " << graph1->get_graph_id() << endl;
    for (itr = NIDSg1.begin(); itr != NIDSg1.end(); itr++) {
        int new_node_id = duplicate_node(new_graph, graph1, (*itr));
        
        // find node in all_id_map
        curr_rep_id = graph1->get_rep_id_from_nid(*itr);
        all_id_map_itr = all_id_map.find(curr_rep_id);
        if (all_id_map_itr != all_id_map.end()){
            all_id_map_subset.insert(pair<string,string>(curr_rep_id, (*all_id_map_itr).second));
        }
        
    }

    
    // copy all nodes from graph2
    for (itr = NIDSg2.begin(); itr != NIDSg2.end(); itr++) {

        // check for conflicting types
        string type_in_graph1 = "";
        //////int nid_in_graph1 = graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr));
        int nid_in_graph1 = graph1->get_nid_from_rep_id(get_rep_id_for_id(graph2->get_rep_id_from_nid(*itr)));
        if (nid_in_graph1 != -1) { // found node in graph1
            type_in_graph1 = graph1->get_node_type(nid_in_graph1);
            
        }
        //string type_in_graph1 = graph1->get_node_type(graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr)));
        if (type_in_graph1 != "" && type_in_graph1 != graph2->get_node_type(*itr)) {
            //cerr << "Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << " which is " << graph2->get_node_type(*itr) <<  " in graph " << graph2->get_graph_id() << " and " << type_if_graph1 << " in prev graphs. ";
            cerr << "Warning: Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << ". Not merging graph " << graph2->get_graph_id() << "." << endl;
            GraphManagerNew::graph_id_count--;
            destroy_graph(new_graph);
            return graph1;

        }

        // code added on 20 Jan 2016
        
        vector<string> ids_in_graph1 = graph1->get_all_ids_of_node(nid_in_graph1);
        vector<string> ids_in_graph2 = graph2->get_all_ids_of_node(*itr);
        
        vector<string>::iterator id_in_graph2;
        int new_node_id;
        // check if node with overlapping set of id present in graph 1
        for(id_in_graph2 = ids_in_graph2.begin(); id_in_graph2 != ids_in_graph2.end(); id_in_graph2++){
            if (graph1->get_rep_id_from_id(*id_in_graph2) != ""){
                
                    new_node_id = new_graph->get_nid_from_rep_id(new_graph->get_rep_id_from_id(graph1->get_rep_id_from_id(*id_in_graph2)));
                    new_graph->add_rep_id_for_node(new_node_id, graph1->get_rep_id_from_id(*id_in_graph2));

                    vector<string> ids_union;

                    sort(ids_in_graph1.begin(), ids_in_graph1.end());
                    sort(ids_in_graph2.begin(), ids_in_graph2.end());

                    set_union(ids_in_graph1.begin(),ids_in_graph1.end(),ids_in_graph2.begin(),ids_in_graph2.end(),back_inserter(ids_union));

                    new_graph->set_all_ids_for_node(new_node_id, ids_union);
                    
            }
            else{
                    new_node_id = duplicate_node(new_graph, graph2, (*itr));
            }
        
        }
        
        
        new_node_id = duplicate_node(new_graph, graph2, (*itr));

    }
    for(itr = NIDSg2.begin(); itr != NIDSg2.end(); itr++) {
            
            // check for conflicting types
            string type_in_graph1 = "";
            int nid_in_graph1 = graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr));
            if (nid_in_graph1 != -1){ // found node in graph1
                type_in_graph1 = graph1->get_node_type(nid_in_graph1);
                
                // find node in all_id_map
                curr_rep_id = graph1->get_rep_id_from_nid(*itr);
                all_id_map_itr = all_id_map.find(curr_rep_id);
                if (all_id_map_itr != all_id_map.end()){
                        all_id_map_subset.insert(pair<string,string>(curr_rep_id, (*all_id_map_itr).second));
                }
                
            }
            //string type_in_graph1 = graph1->get_node_type(graph1->get_nid_from_rep_id(graph2->get_rep_id_from_nid(*itr)));
            if(type_in_graph1 != "" && type_in_graph1 != graph2->get_node_type(*itr)) {
                //cerr << "Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << " which is " << graph2->get_node_type(*itr) <<  " in graph " << graph2->get_graph_id() << " and " << type_if_graph1 << " in prev graphs. ";
                cerr << "Warning: Node types do not match for node " << graph2->get_rep_id_from_nid(*itr) << ". Not merging graph " << graph2->get_graph_id() << "." << endl;
                GraphManagerNew::graph_id_count--;
                destroy_graph(new_graph);
                return graph1;
                
            }
            
            int new_node_id = duplicate_node(new_graph, graph2, (*itr));
            
        }
        
    // create edges from map
    int eid;
    for (all_id_map_itr = all_id_map_subset.begin(); all_id_map_itr != all_id_map_subset.end(); all_id_map_itr++) {
        string source_rep_id = (*all_id_map_itr).first;
        string target_rep_id = (*all_id_map_itr).second;
        int target_node_id = new_graph->check_if_node_already_created(target_rep_id);
        int source_node_id = -1;
        if (target_node_id != -1){
            source_node_id = new_graph->get_nid_from_rep_id(source_rep_id);
        }
        assert(source_node_id != -1);
        	
        string edge_type = "name_map";
        eid = new_graph->create_new_edge();
        new_graph->add_edge_id(eid);
        add_edge_id_graph_id(eid, new_graph->get_graph_id());
        new_graph->add_edge_type(eid, edge_type);
        new_graph->add_edge_to_outlist_of_node(source_node_id, eid);
        new_graph->add_edge_to_inlist_of_node(target_node_id, eid);
        new_graph->add_source_node(eid, source_node_id);
        new_graph->add_target_node(eid, target_node_id);
        new_graph->add_subtype_for_edge(eid, "name_map");
        
               
        // reverse edge
        eid = new_graph->create_new_edge();
        new_graph->add_edge_id(eid);
        add_edge_id_graph_id(eid, new_graph->get_graph_id());
        new_graph->add_edge_type(eid, edge_type);
        new_graph->add_edge_to_outlist_of_node(target_node_id, eid);
        new_graph->add_edge_to_inlist_of_node(source_node_id, eid);
        new_graph->add_source_node(eid, target_node_id);
        new_graph->add_target_node(eid, source_node_id);
        new_graph->add_subtype_for_edge(eid, "name_map");
        

    }

    // get all edge ids from the two graphs to merge
    std::set<int> EIDSg1 = graph1->get_edge_ids();
    std::set<int> EIDSg2 = graph2->get_edge_ids();

    // copy all edges from graph1
    for (itr = EIDSg1.begin(); itr != EIDSg1.end(); itr++) {
        eid = duplicate_edge(new_graph, graph1, (*itr));
    }

    // copy all edges from graph2
    for (itr = EIDSg2.begin(); itr != EIDSg2.end(); itr++) {
        eid = duplicate_edge(new_graph, graph2, (*itr));
    }

    // adding genesis information
    std::list<int> glist;
    glist.push_back(graph1->get_graph_id());
    glist.push_back(graph2->get_graph_id());
    int result_gid = new_graph->get_graph_id();
    add_graph_genesis(result_gid, "Merged graphs", glist);

    return new_graph;
}*/


//returns -1 if the graph couldn't be merged
/*
int GraphManagerNew::merge_graphs_divide_and_conquer(std::vector<int> gid_vec) {
    int n = gid_vec.size();

    //if list is empty report error and return -1
    if (n == 0) {
        cerr << "Error: No graphs to merge" << endl;
        return -1;
    }

    //if the list is of size 1 return the same graph if it is a valid graph or return -1 if it is not a valid graph
    if (n == 1) {
        int gid = *gid_vec.begin();
        GraphNew* graph_new = get_graph(gid);
        if (graph_new == NULL) {
            return -1;
        }
        return gid;
    }

    //if size is more than one then split it into two almost equal non-empty parts and call merge_two graphs on the two returned graphs
    int mid = (n - 1) / 2;

    std::vector<int>gid_list_left(gid_vec.begin(), gid_vec.begin() + mid + 1);
    std::vector<int>gid_list_right(gid_vec.begin() + mid + 1, gid_vec.end());

    int gid_left_merged = merge_graphs_divide_and_conquer(gid_list_left);
    int gid_right_merged = merge_graphs_divide_and_conquer(gid_list_right);

    if (gid_left_merged == -1 && gid_right_merged == -1) {
        return -1;
    }
    if (gid_left_merged == -1 && gid_right_merged != -1) {
        return gid_right_merged;
    }
    if (gid_left_merged != -1 && gid_right_merged == -1) {
        return gid_left_merged;
    }

    GraphNew* graph_left = get_graph(gid_left_merged);
    GraphNew* graph_right = get_graph(gid_right_merged);

    if (graph_left == NULL || graph_right == NULL) {
        cerr << "Bug: a graph must be there for a graph id obtained by merging graphs" << endl;
    }
    cout << " merging graphs: " << gid_left_merged << " and " << gid_right_merged << endl;
    GraphNew* merged_graph = merge_two_graphs(graph_left, graph_right);
    cout << "got merged graph: " << merged_graph->get_graph_id() << endl;

    return merged_graph->get_graph_id();

}*/

int GraphManagerNew::merge_graphs(std::list<int> gid_list, list<unordered_map<string, set<string> > >& map_list) {
    GraphNew * merged_graph;
    //time_t time1, time2;
    std::list<int>::iterator itr;
    std::list<GraphNew *> graph_list;
    std::list<int> glist_merged;
    for (itr = gid_list.begin(); itr != gid_list.end(); itr++) {
        GraphNew * graph = get_graph(*itr);
        if (graph != NULL) {
            graph_list.push_back(graph);
        } else {
            cerr << "Warning: Invalid graph with id " << (*itr) << endl;
        }
    }
    if (graph_list.empty()) {
        cerr << "No valid graphs to merge" << endl;
        return -1;
    }

    GraphNew * graph1 = graph_list.front();
    glist_merged.push_back(graph1->get_graph_id());
    
    graph_list.pop_front();
    while (!graph_list.empty()) {
        GraphNew * graph2 = graph_list.front();
        graph_list.pop_front();
        cout << " merging graphs: " << graph1->get_graph_id() << " and " << graph2->get_graph_id() << endl;
        //time(&time1);
        merged_graph = merge_two_graphs(graph1, graph2, map_list);
        //time(&time2);

        //cout << "got merged graph: " << merged_graph->get_graph_id() << ". Time taken (in sec): " << difftime(time2, time1) << endl;

        if (graph1->get_graph_id() != merged_graph->get_graph_id()) {
            glist_merged.push_back(graph2->get_graph_id());
        }

        graph1 = merged_graph;

    }
    
    set<string> all_ids_in_graph;
    vector<int> all_nids_in_graph = merged_graph->get_node_ids();
    set<string>::iterator all_ids_in_graph_iter;
    for(vector<int>::iterator all_nids_in_graph_iter = all_nids_in_graph.begin(); all_nids_in_graph_iter != all_nids_in_graph.end(); all_nids_in_graph_iter++) {
        vector<string> all_id_of_node = merged_graph->get_all_ids_of_node(*all_nids_in_graph_iter);
        vector<string>::iterator all_id_node_iter;
        for(all_id_node_iter = all_id_of_node.begin(); all_id_node_iter != all_id_of_node.end(); all_id_node_iter++) {
            all_ids_in_graph.insert(*all_id_node_iter);
        }
    }
    int nid;
    
    
        set<string> intermed_nodes_created;
        // iterate over the maps
        list<unordered_map<string, set<string> > >::iterator map_list_iter;
        int edges_before = merged_graph->get_edge_ids().size();
        cout << "Edges before: " << edges_before << endl;
        int edges_after = 0;
while(edges_before != edges_after) {        
        
        for(map_list_iter = map_list.begin(); map_list_iter != map_list.end(); map_list_iter++) {
            //cout << "Reading map with " << (*map_list_iter).size() << " entries" << endl;
                                    
            unordered_map<string, set<string> > current_map = (*map_list_iter);
            
            for(all_ids_in_graph_iter = all_ids_in_graph.begin(); all_ids_in_graph_iter != all_ids_in_graph.end(); all_ids_in_graph_iter++) {
                
                unordered_map<string, set<string> >::iterator found_in_map = current_map.find(*all_ids_in_graph_iter);
                if (found_in_map == current_map.end()) {
                    continue;
                }
                // create key node if not created
                string key = (*found_in_map).first;
                string repid_for_map_entry = get_rep_id_for_id(key);
                if (repid_for_map_entry == "")
                    repid_for_map_entry = key;
                                
                if (intermed_nodes_created.find(repid_for_map_entry) != intermed_nodes_created.end())
                    continue;
                if (merged_graph->check_if_node_already_created(repid_for_map_entry) == -1) {
                    nid = merged_graph->create_new_node();
                    merged_graph->add_node_id(nid);
                    merged_graph->add_rep_id_for_node(nid, repid_for_map_entry);
                    add_node_id_graph_id(nid, merged_graph->get_graph_id());
                    vector<string> all_id_for_rid = get_all_ids_for_rep_id(repid_for_map_entry);
                    all_id_for_rid.push_back(repid_for_map_entry); // specially for this node
                    for(vector<string>::iterator all_id_for_rid_itr = all_id_for_rid.begin(); all_id_for_rid_itr != all_id_for_rid.end(); all_id_for_rid_itr++){
                            merged_graph->add_id_for_node(nid, *all_id_for_rid_itr);
                            merged_graph->add_id_rep_id(*all_id_for_rid_itr, repid_for_map_entry);
   
                    }
                    merged_graph->add_node_type(nid, "protein"); // will this be a protein?
                    merged_graph->add_display_id_for_node(nid, get_display_names_from_rep_id(repid_for_map_entry));

                    intermed_nodes_created.insert(repid_for_map_entry);
                }
                else {
                    nid = merged_graph->get_nid_from_rep_id(repid_for_map_entry);
                }
                
                // create value nodes if not already created
                set<string> value_list = (*found_in_map).second;
                set<string>::iterator value_list_iter;
                for(value_list_iter = value_list.begin(); value_list_iter != value_list.end(); value_list_iter++) {
                    string repid_for_map_entry = get_rep_id_for_id(*value_list_iter);
                    if (repid_for_map_entry == "")
                        repid_for_map_entry = *value_list_iter;
                    if (intermed_nodes_created.find(repid_for_map_entry) != intermed_nodes_created.end())
                        continue;
                    if (merged_graph->check_if_node_already_created(repid_for_map_entry) == -1) {
                        nid = merged_graph->create_new_node();
                        merged_graph->add_node_id(nid);
                        merged_graph->add_rep_id_for_node(nid, repid_for_map_entry);
                        add_node_id_graph_id(nid, merged_graph->get_graph_id());
                        vector<string> all_id_for_rid = get_all_ids_for_rep_id(repid_for_map_entry);
                        all_id_for_rid.push_back(repid_for_map_entry); // specially for this node
                        for(vector<string>::iterator all_id_for_rid_itr = all_id_for_rid.begin(); all_id_for_rid_itr != all_id_for_rid.end(); all_id_for_rid_itr++){
                                merged_graph->add_id_for_node(nid, *all_id_for_rid_itr);
                                merged_graph->add_id_rep_id(*all_id_for_rid_itr, repid_for_map_entry); // commented out by sukanya
                        }
                        merged_graph->add_node_type(nid, "protein"); // will this be a protein?
                        merged_graph->add_display_id_for_node(nid, get_display_names_from_rep_id(repid_for_map_entry));

                        intermed_nodes_created.insert(repid_for_map_entry);
                    }
                    else {
                        nid = merged_graph->get_nid_from_rep_id(repid_for_map_entry);
                    }
                    
                    // create new edge to/from intermediate node
                    string src_repid = get_rep_id_for_id(key);
                    if (src_repid == "")
                        src_repid = key;
                    string tgt_repid = repid_for_map_entry;
                    int source_node_id = merged_graph->get_nid_from_rep_id(src_repid);
                    int target_node_id = merged_graph->get_nid_from_rep_id(tgt_repid);
                    vector<string> map_subtype;
                    map_subtype.push_back("mapping");
                    int eid = merged_graph->check_if_edge_already_created(source_node_id, target_node_id, "mapping", map_subtype);
                    if (eid == -1) {
                    eid = merged_graph->create_new_edge(); // creates a new edge -- added by sukanya
                    merged_graph->add_edge_id(eid); // added by sukanya
                    add_edge_id_graph_id(eid, merged_graph->get_graph_id());
                    merged_graph->add_edge_type(eid, "mapping");
                    merged_graph->add_edge_to_outlist_of_node(source_node_id, eid);
                    merged_graph->add_edge_to_inlist_of_node(target_node_id, eid);
                    merged_graph->add_source_node(eid, source_node_id);
                    merged_graph->add_target_node(eid, target_node_id);
                    merged_graph->add_subtype_for_edge(eid, "mapping");
                    merged_graph->add_pathway_for_edge(eid, " ");
                    }
                    
                    
                    eid = merged_graph->check_if_edge_already_created(target_node_id, source_node_id, "mapping", map_subtype);
                    if (eid == -1){
                    eid = merged_graph->create_new_edge(); // creates a new edge -- added by sukanya
                    merged_graph->add_edge_id(eid); // added by sukanya
                    add_edge_id_graph_id(eid, merged_graph->get_graph_id());
                    merged_graph->add_edge_type(eid, "mapping");
                    merged_graph->add_edge_to_outlist_of_node(target_node_id, eid);
                    merged_graph->add_edge_to_inlist_of_node(source_node_id, eid);
                    merged_graph->add_source_node(eid, target_node_id);
                    merged_graph->add_target_node(eid, source_node_id);
                    merged_graph->add_subtype_for_edge(eid, "mapping");
                    merged_graph->add_pathway_for_edge(eid, " ");
                    }
                    
                }
            }
            
        }
        
        all_ids_in_graph = intermed_nodes_created;
        intermed_nodes_created.clear();
        edges_before = edges_after;
        edges_after = merged_graph->get_edge_ids().size();
        cout << "Edges after: " << edges_after << endl;
}
    
    
    //int result_gid = get_latest_graph_id();
    int result_gid = graph1->get_graph_id();
    add_graph_genesis(result_gid, "Merged graphs", glist_merged);
    return result_gid;
}

void GraphManagerNew::get_nodes_on_path_from_i_to_j(GraphNew * original_graph, int i, int j, set<int>& result_nodes, 
                                                        map<int, set<int> >& frwd_bfs_results, map<int, set<int> >& bkwd_bfs_results){

	set<int> set_nodes_frwd_reachable_from_i;
        set<int> set_nodes_frwd_reachable_from_j;
	set<int> set_nodes_bkwd_reachable_from_i;
	set<int> set_nodes_bkwd_reachable_from_j;
        set<int> set_nodes_frwd_reachable_from_i_until_j;
        set<int> set_nodes_bkwd_reachable_from_j_until_i;
        
        // forward bfs from i
        if(frwd_bfs_results.find(i) == frwd_bfs_results.end()) {
            original_graph->get_bfs_nodes(i, set_nodes_frwd_reachable_from_i, FORWARD);
            frwd_bfs_results.insert(pair<int, set<int> >(i, set_nodes_frwd_reachable_from_i));
        }
        else {
            set_nodes_frwd_reachable_from_i = frwd_bfs_results[i];
            
        }
        
        
        // forward bfs from j
        if(frwd_bfs_results.find(j) == frwd_bfs_results.end()) {
            original_graph->get_bfs_nodes(j, set_nodes_frwd_reachable_from_j, FORWARD);
            frwd_bfs_results.insert(pair<int, set<int> >(j, set_nodes_frwd_reachable_from_j));
        }
        else {
            set_nodes_frwd_reachable_from_j = frwd_bfs_results[j];
        }
        
        
        // backward bfs from i
        if(bkwd_bfs_results.find(i) == bkwd_bfs_results.end()) {
            original_graph->get_bfs_nodes(i, set_nodes_bkwd_reachable_from_i, BACKWARD);
            bkwd_bfs_results.insert(pair<int, set<int> >(i, set_nodes_bkwd_reachable_from_i));
        }
        else {
            set_nodes_bkwd_reachable_from_i = bkwd_bfs_results[i];
        }
        
        
        // backward bfs from j
        if(bkwd_bfs_results.find(j) == bkwd_bfs_results.end()) {
            original_graph->get_bfs_nodes(j, set_nodes_bkwd_reachable_from_j, BACKWARD);
            bkwd_bfs_results.insert(pair<int, set<int> >(j, set_nodes_bkwd_reachable_from_j));
        }
        else {
            set_nodes_bkwd_reachable_from_j = bkwd_bfs_results[j];
        }
        
        
        
        // forward bfs from i until j
        set_intersection(set_nodes_frwd_reachable_from_i.begin(), set_nodes_frwd_reachable_from_i.end(),
                set_nodes_bkwd_reachable_from_j.begin(), set_nodes_bkwd_reachable_from_j.end(),
                inserter(set_nodes_frwd_reachable_from_i_until_j, set_nodes_frwd_reachable_from_i_until_j.begin()));
        
        
        
        
        
        // backward bfs from j until i
        set_intersection(set_nodes_frwd_reachable_from_j.begin(), set_nodes_frwd_reachable_from_j.end(),
                set_nodes_bkwd_reachable_from_i.begin(), set_nodes_bkwd_reachable_from_i.end(),
                inserter(set_nodes_bkwd_reachable_from_j_until_i, set_nodes_bkwd_reachable_from_j_until_i.begin()));
        
        
        // nodes_on_path_from_i_to_j
	set_union(set_nodes_frwd_reachable_from_i_until_j.begin(), set_nodes_frwd_reachable_from_i_until_j.end(),
                set_nodes_bkwd_reachable_from_j_until_i.begin(), set_nodes_bkwd_reachable_from_j_until_i.end(), inserter(result_nodes, result_nodes.begin()));

}

//kaka
// Implements the push-relabel algorithm
void GraphManagerNew::compute_min_cut_for_ghtree(GraphNew * original_graph, int i, int j, set<int>& cut_edges, set<int>& set_s,
                                                        map<int, set<int> >& frwd_bfs_results, map<int, set<int> >& bkwd_bfs_results) {
    
    set<int> nodes_on_path_from_i_to_j; // contains nids in original graph
    get_nodes_on_path_from_i_to_j(original_graph, GET_NODE_ID(original_graph, i), GET_NODE_ID(original_graph, j), nodes_on_path_from_i_to_j, 
                                                                                                        frwd_bfs_results, bkwd_bfs_results);
    
    cout << "Mincut iteration " << i << endl;

    
#ifdef DEBUG_FLAG
    debug_log << "Mincut iteration " << i << endl;
    debug_log << "-------------------------------" << endl;
    
    debug_log << "Computing min-cut between " << i << " and " << j << endl;

#endif
            
    static int num_of_nodes, new_gid;
    static map<int, int> map_nseq_to_res_graph_nid;
    
    static GraphNew * residual_graph;
    
    int curr_height, curr_excess;
    
    static set<int> set_seq_with_outgoing_edges_explored;
    set<int>::iterator set_itr;
    
    vector<int> outlist;
    vector<int>::iterator vec_itr;
      
    int new_nid;
    
    if (i == 1) { // creates residual graph only if this is the first iteration of the mincut code ,ie., i == 1
            residual_graph = new GraphNew;
            num_of_nodes = original_graph->get_node_ids().size();
            new_gid = ++GraphManagerNew::graph_id_count;
            residual_graph->set_graph_id(new_gid);
            add_graph(new_gid, residual_graph);
#ifdef DEBUG_FLAG
            debug_log << "Created residual graph once. Graph id is " << new_gid << endl;
            debug_log << endl;
#endif
                 
    }
    
    // creating the node for i      
    if (map_nseq_to_res_graph_nid.find(i) == map_nseq_to_res_graph_nid.end()) {
            new_nid = residual_graph->create_new_node();
            residual_graph->add_component_id_for_node(new_nid, i); // nseq in original graph
            residual_graph->add_component_id_for_node(new_nid, i); // identifier of mincut call
            residual_graph->add_component_id_for_node(new_nid, num_of_nodes); // height
            residual_graph->add_component_id_for_node(new_nid, original_graph->get_outlist(GET_NODE_ID(original_graph, i)).size()); // excess
            
            map_nseq_to_res_graph_nid.insert(pair<int, int>(i, new_nid));
             
    }
    else {
            new_nid = map_nseq_to_res_graph_nid[i];
            residual_graph->update_component_of_node_at_index(new_nid, 1, i);         // updating identifier of mincut call
            residual_graph->update_component_of_node_at_index(new_nid, 2, num_of_nodes); // updating height
            residual_graph->update_component_of_node_at_index(new_nid, 3, original_graph->get_outlist(GET_NODE_ID(original_graph, i)).size()); // updating excess
           
    }
    
    // creating the node for j -- added by sukanya on 24 May 2016
    // this is required because j will not be present in the set nodes_on_path_from_i_to_j
    // hence it may not be created later on
    int new_nid_for_target;
    if (map_nseq_to_res_graph_nid.find(j) == map_nseq_to_res_graph_nid.end()) {
            new_nid_for_target = residual_graph->create_new_node();
            residual_graph->add_component_id_for_node(new_nid_for_target, j); // nseq in original graph
            residual_graph->add_component_id_for_node(new_nid_for_target, i); // identifier of mincut call
            residual_graph->add_component_id_for_node(new_nid_for_target, 0); // height
            residual_graph->add_component_id_for_node(new_nid_for_target, 0); // excess
            
            map_nseq_to_res_graph_nid.insert(pair<int, int>(j, new_nid_for_target));
             
    }
    else {
            new_nid_for_target = map_nseq_to_res_graph_nid[j];
            residual_graph->update_component_of_node_at_index(new_nid_for_target, 1, i);         // updating identifier of mincut call
            residual_graph->update_component_of_node_at_index(new_nid_for_target, 2, 0); // updating height
            residual_graph->update_component_of_node_at_index(new_nid_for_target, 3, 0); // updating excess
            
    }
    
    
    list<int> nodes_queue;
    nodes_queue.push_back(new_nid);

    nodes_on_path_from_i_to_j.insert(GET_NODE_ID(original_graph, i));
    nodes_on_path_from_i_to_j.insert(GET_NODE_ID(original_graph, j));
    
    while (!nodes_queue.empty()) {
        
        int curr_nid_in_res_graph = nodes_queue.front();
        int curr_nseq_in_orig_graph = residual_graph->get_component_ids_of_node(curr_nid_in_res_graph)[0];
        
        nodes_queue.pop_front();
        
        // if node does not lie on path from i to j
        int curr_nid_in_orig_graph = GET_NODE_ID(original_graph, curr_nseq_in_orig_graph);
        if(nodes_on_path_from_i_to_j.find(curr_nid_in_orig_graph) == nodes_on_path_from_i_to_j.end())
            continue;
        
        
        int curr_excess = residual_graph->get_component_ids_of_node(curr_nid_in_res_graph)[3];
        if(curr_excess == 0)
            continue;
        
        int curr_height = residual_graph->get_component_ids_of_node(curr_nid_in_res_graph)[2];
        
        // if outlist is not already explored add out-edges to residual graph
        if (set_seq_with_outgoing_edges_explored.find(curr_nseq_in_orig_graph) == set_seq_with_outgoing_edges_explored.end()) {
            int nid = GET_NODE_ID(original_graph, curr_nseq_in_orig_graph);
            
            outlist = original_graph->get_outlist(nid);
            
            for(vec_itr = outlist.begin(); vec_itr != outlist.end(); vec_itr++) {
                
                int succ_node_id = original_graph->get_target_node(*vec_itr);
                
                // if node does not lie on path from i to j
                if(nodes_on_path_from_i_to_j.find(succ_node_id) == nodes_on_path_from_i_to_j.end())
                        continue;
                
                if(succ_node_id == nid)
                        continue;
                
                int succ_node_seq = GET_NODE_SEQ_NUM(original_graph, succ_node_id);
                
                bool create_new_edge = false;
                
                // create target node if not already created -- set height and excess to zero
                if (map_nseq_to_res_graph_nid.find(succ_node_seq) == map_nseq_to_res_graph_nid.end()) {
                    int new_nid_in_res_graph = residual_graph->create_new_node();
                    residual_graph->add_component_id_for_node(new_nid_in_res_graph, succ_node_seq); // nseq in original graph
                    residual_graph->add_component_id_for_node(new_nid_in_res_graph, i); // identifier for mincut call
                    residual_graph->add_component_id_for_node(new_nid_in_res_graph, 0); // height
                    residual_graph->add_component_id_for_node(new_nid_in_res_graph, 0); // excess
                    
                    map_nseq_to_res_graph_nid.insert(pair<int, int>(succ_node_seq, new_nid_in_res_graph));
                    
#ifdef DEBUG_FLAG
                    debug_log << "Created new node " << new_nid_in_res_graph << endl;
#endif                    
                        
                    int new_eid_in_res_graph = residual_graph->create_new_edge();
                    residual_graph->add_edge_id(new_eid_in_res_graph);
                    add_edge_id_graph_id(new_eid_in_res_graph, residual_graph->get_graph_id());
                    residual_graph->add_source_node(new_eid_in_res_graph, curr_nid_in_res_graph);
                    residual_graph->add_target_node(new_eid_in_res_graph, new_nid_in_res_graph);
                    residual_graph->add_edge_to_outlist_of_node(curr_nid_in_res_graph, new_eid_in_res_graph);
                    residual_graph->add_edge_to_inlist_of_node(new_nid_in_res_graph, new_eid_in_res_graph);
                    residual_graph->set_other_value1_in_edge(new_eid_in_res_graph, i); // identifier of mincut call
                    residual_graph->set_other_value2_in_edge(new_eid_in_res_graph, 1); // capacity
                    residual_graph->set_other_value4_in_edge(new_eid_in_res_graph, *vec_itr); // eid in original graph
                    residual_graph->add_edge_type(new_eid_in_res_graph, "original");
                    
                    // reverse edge
                    int rev_eid_in_res_graph = residual_graph->create_new_edge();
                    residual_graph->add_edge_id(rev_eid_in_res_graph);
                    add_edge_id_graph_id(rev_eid_in_res_graph, residual_graph->get_graph_id());
                    residual_graph->add_target_node(rev_eid_in_res_graph, curr_nid_in_res_graph);
                    residual_graph->add_source_node(rev_eid_in_res_graph, new_nid_in_res_graph);
                    residual_graph->add_edge_to_inlist_of_node(curr_nid_in_res_graph, rev_eid_in_res_graph);
                    residual_graph->add_edge_to_outlist_of_node(new_nid_in_res_graph, rev_eid_in_res_graph);
                    residual_graph->set_other_value1_in_edge(rev_eid_in_res_graph, i); // identifier of mincut call
                    
                    int rev_eid_in_orig_graph = -1;
                    bool orig_edge = false;
                    vector<int> outlist2 = original_graph->get_outlist(succ_node_id);
                    for (vector<int>::iterator out_itr = outlist2.begin(); out_itr != outlist2.end(); out_itr++) {
                            if (original_graph->get_target_node(*out_itr) == nid) {
                                    orig_edge = true;
                                    rev_eid_in_orig_graph = *out_itr;
                                    break;
                            }
                            
                    }
                    
                    if(orig_edge) {
                            residual_graph->set_other_value2_in_edge(rev_eid_in_res_graph, 1); // capacity
                            residual_graph->add_edge_type(rev_eid_in_res_graph, "original");
                            
                    }
                    else {
                            residual_graph->set_other_value2_in_edge(rev_eid_in_res_graph, 0); // capacity
                            residual_graph->add_edge_type(rev_eid_in_res_graph, "not_original");
                            
                    }
                    residual_graph->set_other_value3_in_edge(rev_eid_in_res_graph, new_eid_in_res_graph); // eid of paired reverse edge
                    residual_graph->set_other_value3_in_edge(new_eid_in_res_graph, rev_eid_in_res_graph); // eid of paired reverse edge
                    residual_graph->set_other_value4_in_edge(rev_eid_in_res_graph, rev_eid_in_orig_graph); // eid in original graph
                }
                
                // target node is already created -- update height and excess to zero for i-th iteration of mincut
                else {
                        int succ_nid_in_res_graph = map_nseq_to_res_graph_nid[succ_node_seq];
                        assert(succ_nid_in_res_graph != curr_nid_in_res_graph);
                        
                        if(residual_graph->get_component_ids_of_node(succ_nid_in_res_graph)[1]!=i) {
                        
                            residual_graph->update_component_of_node_at_index(succ_nid_in_res_graph, 1, i); // updating identifier of mincut call
                            residual_graph->update_component_of_node_at_index(succ_nid_in_res_graph, 2, 0); // updating height
                            residual_graph->update_component_of_node_at_index(succ_nid_in_res_graph, 3, 0); // updating excess
                        }
                        
#ifdef DEBUG_FLAG
                        debug_log << "Found node " << succ_nid_in_res_graph << endl;
#endif                        
                        int res_eid;
                        bool res_edge = false;
                        vector<int> outlist3 = residual_graph->get_outlist(curr_nid_in_res_graph);
                        for (vector<int>::iterator out_itr = outlist3.begin(); out_itr != outlist3.end(); out_itr++) {
                            res_eid = *out_itr;    
                            
                            if (residual_graph->get_target_node(res_eid) == succ_nid_in_res_graph) {
                                    res_edge = true;
                                    break;
                            }
                            
                        }
                        if (res_edge) {
                            
			   if(residual_graph->get_other_value1_in_edge(res_eid)!=i){
                                residual_graph->set_other_value1_in_edge(res_eid, i); // identifier of mincut call
                                assert(residual_graph->get_edge_type(res_eid) == "original");
                                residual_graph->set_other_value2_in_edge(res_eid, 1); // capacity
			    }
                                int rev_res_eid = residual_graph->get_other_value3_in_edge(res_eid); // eid of paired reverse edge
                            if(residual_graph->get_other_value1_in_edge(rev_res_eid)!=i){    
                                residual_graph->set_other_value1_in_edge(rev_res_eid, i); // identifier of mincut call
                                if (residual_graph->get_edge_type(rev_res_eid) == "original") {
                                        residual_graph->set_other_value2_in_edge(rev_res_eid, 1); // capacity
                                }
                                else {
                                        residual_graph->set_other_value2_in_edge(rev_res_eid, 0); // capacity
                                }
			    }
                        }
                        else {
                                    int new_eid_in_res_graph = residual_graph->create_new_edge();
                                    residual_graph->add_edge_id(new_eid_in_res_graph);
                                    add_edge_id_graph_id(new_eid_in_res_graph, residual_graph->get_graph_id());
                                    residual_graph->add_source_node(new_eid_in_res_graph, curr_nid_in_res_graph);
                                    residual_graph->add_target_node(new_eid_in_res_graph, succ_nid_in_res_graph);
                                    residual_graph->add_edge_to_outlist_of_node(curr_nid_in_res_graph, new_eid_in_res_graph);
                                    residual_graph->add_edge_to_inlist_of_node(succ_nid_in_res_graph, new_eid_in_res_graph);
                                    residual_graph->set_other_value1_in_edge(new_eid_in_res_graph, i); // identifier of mincut call
                                    residual_graph->set_other_value2_in_edge(new_eid_in_res_graph, 1); // capacity
                                    residual_graph->set_other_value4_in_edge(new_eid_in_res_graph, *vec_itr);  // eid in original graph
                                    residual_graph->add_edge_type(new_eid_in_res_graph, "original");

                                    // reverse edge
                                    int rev_eid_in_res_graph = residual_graph->create_new_edge();
                                    residual_graph->add_edge_id(rev_eid_in_res_graph);
                                    add_edge_id_graph_id(rev_eid_in_res_graph, residual_graph->get_graph_id());
                                    residual_graph->add_target_node(rev_eid_in_res_graph, curr_nid_in_res_graph);
                                    residual_graph->add_source_node(rev_eid_in_res_graph, succ_nid_in_res_graph);
                                    residual_graph->add_edge_to_inlist_of_node(curr_nid_in_res_graph, rev_eid_in_res_graph);
                                    residual_graph->add_edge_to_outlist_of_node(succ_nid_in_res_graph, rev_eid_in_res_graph);
                                    residual_graph->set_other_value1_in_edge(rev_eid_in_res_graph, i); // identifier of mincut call

                                    int rev_eid_in_orig_graph = -1;
                                    bool orig_edge = false;
                                    vector<int> outlist2 = original_graph->get_outlist(succ_node_id);
                                    for (vector<int>::iterator out_itr = outlist2.begin(); out_itr != outlist2.end(); out_itr++) {
                                            if (original_graph->get_target_node(*out_itr) == nid) {
                                                    rev_eid_in_orig_graph = *out_itr;
                                                    orig_edge = true;
                                                    break;
                                            }

                                    }

                                    if(orig_edge) {
                                            residual_graph->set_other_value2_in_edge(rev_eid_in_res_graph, 1); // capacity
                                            residual_graph->add_edge_type(rev_eid_in_res_graph, "original");
                                    }
                                    else {
                                            residual_graph->set_other_value2_in_edge(rev_eid_in_res_graph, 0); // capacity
                                            residual_graph->add_edge_type(rev_eid_in_res_graph, "not_original");
                                    }
                                    residual_graph->set_other_value3_in_edge(rev_eid_in_res_graph, new_eid_in_res_graph); // eid of paired reverse edge
                                    residual_graph->set_other_value3_in_edge(new_eid_in_res_graph, rev_eid_in_res_graph); // eid of paired reverse edge
                                    residual_graph->set_other_value4_in_edge(rev_eid_in_res_graph, rev_eid_in_orig_graph);  // eid in original graph
                        }
                }
            }
              
            set_seq_with_outgoing_edges_explored.insert(curr_nseq_in_orig_graph);
  
        }
        
        set<int> set_eids_to_tgt_nodes_with_min_height;
        int min_height_tgt_node_in_res_graph;
        int cum_flow_thru_edges_to_min_height_neighbr;
        
        
        do {
            cum_flow_thru_edges_to_min_height_neighbr = 0;
            
            // Get set of neighboring nodes with minimum height
            set_eids_to_tgt_nodes_with_min_height.clear();
            min_height_tgt_node_in_res_graph = inf;

            vector<int> outlist5 = residual_graph->get_outlist(curr_nid_in_res_graph);
            for (vector<int>::iterator out_itr = outlist5.begin(); out_itr != outlist5.end(); out_itr++) {
                
                int res_eid = *out_itr;
                int res_eid_tgt = residual_graph->get_target_node(res_eid);

                if (residual_graph->get_other_value1_in_edge(res_eid) != i) { //edge created in a previous mincut call

                        //if (residual_graph->get_component_ids_of_node(res_eid_tgt)[1] != i) {

                            residual_graph->update_component_of_node_at_index(res_eid_tgt, 1, i); // updating identifier of mincut call
                            residual_graph->update_component_of_node_at_index(res_eid_tgt, 2, 0); // updating height
                            residual_graph->update_component_of_node_at_index(res_eid_tgt, 3, 0); // updating excess

   
                        //}
                        residual_graph->set_other_value1_in_edge(res_eid, i);
                        if (residual_graph->get_edge_type(res_eid) == "original") {
                                residual_graph->set_other_value2_in_edge(res_eid, 1); // capacity
                        }
                        else {
                                residual_graph->set_other_value2_in_edge(res_eid, 0); // capacity
                        }

                        // since edge with id res_eid was created in a prev mincut call,
                        // then the corresponding rev edge with id rev_res_eid 
                        // should also have been created in the prev mincut call
                        int rev_res_eid = residual_graph->get_other_value3_in_edge(res_eid);
                        assert(residual_graph->get_other_value1_in_edge(rev_res_eid) != i);

                        residual_graph->set_other_value1_in_edge(rev_res_eid, i);
                        if (residual_graph->get_edge_type(rev_res_eid) == "original") {
                                residual_graph->set_other_value2_in_edge(rev_res_eid, 1); // capacity
                        }
                        else {
                                residual_graph->set_other_value2_in_edge(rev_res_eid, 0); // capacity
                        }
                }




                int curr_edge_capacity = residual_graph->get_other_value2_in_edge(res_eid);
                if (curr_edge_capacity > 0) { // capacity of this edge is greater than 0
                    int height_tgt_in_res_graph = residual_graph->get_component_ids_of_node(res_eid_tgt)[2];

                    if(height_tgt_in_res_graph < min_height_tgt_node_in_res_graph) {
                        cum_flow_thru_edges_to_min_height_neighbr = curr_edge_capacity;
                        set_eids_to_tgt_nodes_with_min_height.clear();
                        set_eids_to_tgt_nodes_with_min_height.insert(res_eid);
                        min_height_tgt_node_in_res_graph = height_tgt_in_res_graph;

                    }
                    else if (height_tgt_in_res_graph == min_height_tgt_node_in_res_graph && cum_flow_thru_edges_to_min_height_neighbr < curr_excess) {
                        set_eids_to_tgt_nodes_with_min_height.insert(res_eid);
                        cum_flow_thru_edges_to_min_height_neighbr += curr_edge_capacity;
                    }

                }

            }

            
            //assert(min_height_tgt_node_in_res_graph != inf);// -- violated


            if ((curr_height <= min_height_tgt_node_in_res_graph) && 
                    (curr_nseq_in_orig_graph != j) && (curr_nseq_in_orig_graph != i)) {
                
                residual_graph->update_component_of_node_at_index(curr_nid_in_res_graph, 2, min_height_tgt_node_in_res_graph+1);
                curr_height = min_height_tgt_node_in_res_graph+1;
            }
            
            // assert(curr_height <= num_of_nodes + 1); // may fail?
            assert(curr_height <= 2*num_of_nodes - 1);
            
            if (curr_height > min_height_tgt_node_in_res_graph) {
                
//                if (i != 1) {
//                    assert(curr_height == min_height_tgt_node_in_res_graph + 1);
//                }
                
                // push flow to neighboring nodes with lower height
                for (set_itr = set_eids_to_tgt_nodes_with_min_height.begin(); (set_itr != set_eids_to_tgt_nodes_with_min_height.end()) && (curr_excess > 0); set_itr++) {
                    
                    int curr_eid = *set_itr;
                    int tgt_node_in_res_graph = residual_graph->get_target_node(curr_eid);
		    int tgt_nseq_in_orig_graph = residual_graph->get_component_ids_of_node(tgt_node_in_res_graph)[0];

                    int curr_edge_capacity = residual_graph->get_other_value2_in_edge(curr_eid);
                    // update flow
                    int flow = (curr_excess < curr_edge_capacity) ? curr_excess : curr_edge_capacity;
                    residual_graph->update_component_of_node_at_index(curr_nid_in_res_graph, 3, curr_excess-flow);
                    int tgt_excess = residual_graph->get_component_ids_of_node(tgt_node_in_res_graph)[3];
                    residual_graph->update_component_of_node_at_index(tgt_node_in_res_graph, 3, tgt_excess+flow);
                    
                    //if (tgt_nseq_in_orig_graph != j) {
                    if ((tgt_nseq_in_orig_graph != i) && (tgt_nseq_in_orig_graph != j)) {
                        nodes_queue.push_back(tgt_node_in_res_graph);
                    }

                    // update capacity
                    residual_graph->set_other_value2_in_edge(curr_eid, curr_edge_capacity-flow);
                    int rev_edge_id = residual_graph->get_other_value3_in_edge(curr_eid);
                    int rev_edge_capacity = residual_graph->get_other_value2_in_edge(rev_edge_id);
                    residual_graph->set_other_value2_in_edge(rev_edge_id, rev_edge_capacity+flow);

                    curr_excess = curr_excess - flow;
                    
//                    char wait;
//                    cin >> wait;
                }
            }
            else {
                
	      break;
            }
            
        } while (curr_excess > 0);
             
    }
    
    set<int> set_nodes_frwd_reachable_from_i;
    set<int> set_nodes_bkwd_reachable_from_j;
    set<int> set_frwd_edges_from_i_with_zero_capacity;
    set<int> set_bkwd_edges_from_j_with_zero_capacity;
    
    set<int> visited;
    list<int> bfs_queue;
    
    // find forward reachable nodes and edges from i
    bfs_queue.push_back(i);
    //bfs_queue.push_back(map_nseq_to_res_graph_nid[i]);

    while (!bfs_queue.empty()) {
        
        int curr_node = bfs_queue.front();
        
        bfs_queue.pop_front();
        
        if (visited.find(curr_node) == visited.end()) {
            set_nodes_frwd_reachable_from_i.insert(curr_node); // takes node seq num
            if (curr_node > i){
                set_s.insert(curr_node);  // takes node seq num
            }
            visited.insert(curr_node);
            // traverse outlist
            vector<int> outlist_of_i_side_node = residual_graph->get_outlist(map_nseq_to_res_graph_nid[curr_node]);
            for (vector<int>::iterator outlist_of_i_itr = outlist_of_i_side_node.begin(); outlist_of_i_itr != outlist_of_i_side_node.end(); outlist_of_i_itr++) {
                int curr_edge = (*outlist_of_i_itr);
                
                
                if (residual_graph->get_other_value2_in_edge(curr_edge) == 0) { // stop at zero capacity edge
                    set_frwd_edges_from_i_with_zero_capacity.insert(curr_edge);

                    
                }
                
                else {
                    int target_nid = residual_graph->get_target_node(curr_edge);
                    bfs_queue.push_back(residual_graph->get_component_ids_of_node(target_nid)[0]);
                    
                }
                
            }
        }
        
    }
   
    assert(bfs_queue.empty());
    visited.clear();
    
    // find backward reachable nodes and edges from j
    bfs_queue.push_back(j);
    //bfs_queue.push_back(map_nseq_to_res_graph_nid[j]);
    
    while (!bfs_queue.empty()) {
        int curr_node = bfs_queue.front();
        //int source_nid;
        bfs_queue.pop_front();
        
        if (visited.find(curr_node) == visited.end()) {
            set_nodes_bkwd_reachable_from_j.insert(curr_node); // takes node seq num
            visited.insert(curr_node);
            // traverse inlist
            
            vector<int> inlist_of_j_side_node = residual_graph->get_inlist(map_nseq_to_res_graph_nid[curr_node]);
            for (vector<int>::iterator inlist_of_j_itr = inlist_of_j_side_node.begin(); inlist_of_j_itr != inlist_of_j_side_node.end(); inlist_of_j_itr++) {
                int curr_edge = (*inlist_of_j_itr);
                
                //anjan
                    if(residual_graph->get_edge_type(curr_edge) == "original"){
                        if ((residual_graph->get_other_value2_in_edge(curr_edge) == 0)&&(set_frwd_edges_from_i_with_zero_capacity.find(curr_edge)!=set_frwd_edges_from_i_with_zero_capacity.end())) { // stop at zero capacity edge
//anjan
                            set_bkwd_edges_from_j_with_zero_capacity.insert(curr_edge);
                            //cut_edges.insert(curr_edge); // commented out by sukanya
                        }

                        else {//anjan
                            if(residual_graph->get_other_value2_in_edge(curr_edge) == 0)
                                    set_bkwd_edges_from_j_with_zero_capacity.insert(curr_edge);
                            //bfs_queue.push_back(residual_graph->get_source_node(curr_edge)); // changed by sukanya
                            int source_nid = residual_graph->get_source_node(curr_edge);
                            bfs_queue.push_back(residual_graph->get_component_ids_of_node(source_nid)[0]);
                        }
                }
                
                
//                if (residual_graph->get_edge_type(curr_edge) == "original") {
//                    int source_nid = residual_graph->get_source_node(curr_edge);
//                    if (set_nodes_frwd_reachable_from_i.find(residual_graph->get_component_ids_of_node(source_nid)[0]) == set_nodes_frwd_reachable_from_i.end()) {
//                            set_bkwd_edges_from_j_with_zero_capacity.insert(curr_edge);
//                            bfs_queue.push_back(residual_graph->get_component_ids_of_node(source_nid)[0]);
//                    }
//                   
//                }
                
                
                
            }
        }
        
    }
    set<int> undirected_cut_edges;

    for (set<int>::iterator set_itr2 = set_frwd_edges_from_i_with_zero_capacity.begin(); set_itr2 != set_frwd_edges_from_i_with_zero_capacity.end(); set_itr2++) {
        int curr_tgt = residual_graph->get_component_ids_of_node(residual_graph->get_target_node(*set_itr2))[0];
            if (set_nodes_bkwd_reachable_from_j.find(curr_tgt) != set_nodes_bkwd_reachable_from_j.end()) {
                undirected_cut_edges.insert(residual_graph->get_other_value4_in_edge(*set_itr2));
                int rev_eid_in_res_graph = residual_graph->get_other_value3_in_edge(*set_itr2);
                undirected_cut_edges.insert(residual_graph->get_other_value4_in_edge(rev_eid_in_res_graph));
            }
        
        }
    
    
    
#ifdef DEBUG_FLAG
    debug_log << "undirected_cut_edges contains: ";
        for (set<int>::iterator set_itr2 = undirected_cut_edges.begin(); set_itr2 != undirected_cut_edges.end(); set_itr2++) {
            debug_log << *set_itr2 << " ";
        }
        debug_log << endl;
        
#endif    
    
    
    
//    set_intersection(set_frwd_edges_from_i_with_zero_capacity.begin(), set_frwd_edges_from_i_with_zero_capacity.end(), 
//            set_bkwd_edges_from_j_with_zero_capacity.begin(), set_bkwd_edges_from_j_with_zero_capacity.end(), inserter(undirected_cut_edges, undirected_cut_edges.begin()));
    
    set<int>::iterator cut_edges_itr;
    for (cut_edges_itr = undirected_cut_edges.begin(); cut_edges_itr != undirected_cut_edges.end(); cut_edges_itr++) {
        //int eid_in_orig_graph = residual_graph->get_other_value4_in_edge(*cut_edges_itr);
        int eid_in_orig_graph = *cut_edges_itr;
        if (eid_in_orig_graph != -1) {
            cut_edges.insert(eid_in_orig_graph);
         }
    }
    
    
            
#ifdef DEBUG_FLAG    
    debug_log << endl;
#endif    
}

//haha
//anjan
int GraphManagerNew::create_node(int itr_no, int v, GraphNew *residual_graph, map<int, int>& map_nseq_to_res_graph_nid, int node_height, int node_excess){

    int new_nid;
   // creating the node for i      
    if (map_nseq_to_res_graph_nid.find(v) == map_nseq_to_res_graph_nid.end()) {
            new_nid = residual_graph->create_new_node();

            residual_graph->add_node_id(new_nid);
            add_node_id_graph_id(new_nid, residual_graph->get_graph_id());

            residual_graph->add_component_id_for_node(new_nid, v); // nseq in original graph
            residual_graph->add_component_id_for_node(new_nid, itr_no); // identifier of mincut call
            residual_graph->add_component_id_for_node(new_nid, node_height); // height
            residual_graph->add_component_id_for_node(new_nid, node_excess); // excess

            map_nseq_to_res_graph_nid.insert(pair<int, int>(v, new_nid));            
    }
    else {
            new_nid = map_nseq_to_res_graph_nid[v];
	    //the node may or may not be the same for present iteration
	    if(residual_graph->get_component_ids_of_node(new_nid)[1]!=itr_no){
            	residual_graph->update_component_of_node_at_index(new_nid, 1, itr_no);      // updating identifier of mincut call
           	residual_graph->update_component_of_node_at_index(new_nid, 2, node_height); // updating height
           	residual_graph->update_component_of_node_at_index(new_nid, 3, node_excess); // updating excess
	    }
           
    }

return new_nid;

}

int GraphManagerNew::get_node_from_src_tgt(int src, int tgt,  map<string,long int>& map_src_tgt_to_eid){
	stringstream ss;
	ss<<src<<tgt;
	return map_src_tgt_to_eid[ss.str()];
}

int GraphManagerNew::create_edge(int itr_no, int curr_node_id, int succ_node_id, int eid_in_org_graph,  map<string,long int>& map_src_tgt_to_eid, GraphNew * residual_graph){
	long int new_eid_in_res_graph = residual_graph->create_new_edge();
	residual_graph->add_edge_id(new_eid_in_res_graph);
	add_edge_id_graph_id(new_eid_in_res_graph, residual_graph->get_graph_id());
	residual_graph->add_source_node(new_eid_in_res_graph, curr_node_id);
        residual_graph->add_target_node(new_eid_in_res_graph, succ_node_id);
        residual_graph->add_edge_to_outlist_of_node(curr_node_id, new_eid_in_res_graph);
        residual_graph->add_edge_to_inlist_of_node(succ_node_id, new_eid_in_res_graph);
        residual_graph->set_other_value1_in_edge(new_eid_in_res_graph, itr_no); // identifier of mincut call
        residual_graph->set_other_value2_in_edge(new_eid_in_res_graph, 1); // capacity
        residual_graph->set_other_value4_in_edge(new_eid_in_res_graph, eid_in_org_graph); // eid in original graph

//cout<<curr_node_id<<"["<<residual_graph->get_component_ids_of_node(curr_node_id)[0]<<"]"<<"->"<<new_eid_in_res_graph<<"["<<residual_graph->get_other_value4_in_edge(new_eid_in_res_graph)<<"]"<<"->"<<succ_node_id<<"["<<residual_graph->get_component_ids_of_node(succ_node_id)[0]<<"]"<<endl;

	stringstream ss;
	ss<<curr_node_id<<succ_node_id;
	map_src_tgt_to_eid.insert(pair<string,long int>(ss.str(), new_eid_in_res_graph));

}


void GraphManagerNew::push(int src, int sink, int eid, map<string, long int>& map_src_tgt_to_eid,  GraphNew * residual_graph){
    int height_src = residual_graph->get_component_ids_of_node(src)[2];
    int height_sink = residual_graph->get_component_ids_of_node(sink)[2];
    int curr_excess = residual_graph->get_component_ids_of_node(src)[3];
    int curr_edge_capacity = residual_graph->get_other_value2_in_edge(eid); 
    int flow = min(curr_excess, curr_edge_capacity);

    if (height_src <= height_sink || flow == 0) 
	return;

	//cout<<"before: "<<src<<"["<<residual_graph->get_component_ids_of_node(src)[0]<<"]"<<"->"<<eid<<"["<<residual_graph->get_other_value4_in_edge(eid)<<"]"<<"("<<residual_graph->get_other_value2_in_edge(eid)<<")"<<"->"<<sink<<"["<<residual_graph->get_component_ids_of_node(sink)[0]<<"]"<<endl;
    // update flow
    residual_graph->update_component_of_node_at_index(src, 3, curr_excess-flow);
    int sink_excess = residual_graph->get_component_ids_of_node(sink)[3];
    residual_graph->update_component_of_node_at_index(sink, 3, sink_excess+flow);

    // update capacity
    residual_graph->set_other_value2_in_edge(eid, curr_edge_capacity-flow);
    int rev_edge_id = get_node_from_src_tgt(sink, src, map_src_tgt_to_eid);
    int rev_edge_capacity = residual_graph->get_other_value2_in_edge(rev_edge_id);
    residual_graph->set_other_value2_in_edge(rev_edge_id, rev_edge_capacity+flow);
	//cout<<"After: "<<src<<"["<<residual_graph->get_component_ids_of_node(src)[0]<<"]"<<"->"<<eid<<"["<<residual_graph->get_other_value4_in_edge(eid)<<"]"<<"("<<residual_graph->get_other_value2_in_edge(eid)<<")"<<"->"<<sink<<"["<<residual_graph->get_component_ids_of_node(sink)[0]<<"]"<<endl;

     	
}

void GraphManagerNew::Relabel(int src,  vector<int>& count, int N,  GraphNew * residual_graph){	
	long int src_height = residual_graph->get_component_ids_of_node(src)[2];
	count[src_height]--;
	src_height = 2*N;
	vector<int> outlist;
	outlist = residual_graph->get_outlist(src);	
	for(vector<int>::iterator vec_itr = outlist.begin(); vec_itr != outlist.end(); vec_itr++) {
		int eid = *vec_itr;
		int trgt_node = residual_graph->get_target_node(eid); 
		int curr_edge_capacity = residual_graph->get_other_value2_in_edge(eid); 
   		int height_trgt = residual_graph->get_component_ids_of_node(trgt_node)[2];

		if(curr_edge_capacity > 0){
			if(src_height > height_trgt +1)
				src_height = height_trgt +1;
		}

	}
	//now set the height of the src node in residual graph
	residual_graph->update_component_of_node_at_index(src, 2, src_height);
	count[src_height]++;
	int curr_nseq_in_orig_graph = residual_graph->get_component_ids_of_node(src)[0];

}

void GraphManagerNew::Gap(int itr_no, map<int, int>& map_nseq_to_res_graph_nid, int k, int N,  vector<int>& count, GraphNew * residual_graph){
for (int v = 1; v <= N; v++) {
	if ((map_nseq_to_res_graph_nid.find(v) != map_nseq_to_res_graph_nid.end())){
	     int v_in_res  = map_nseq_to_res_graph_nid[v];
      	     int height_v = residual_graph->get_component_ids_of_node(v_in_res)[2];
	
 	     if (height_v < k || residual_graph->get_component_ids_of_node(v_in_res)[1] != itr_no)
		 continue;
  	     count[height_v]--;
 	     height_v = max(height_v, N+1);
	     residual_graph->update_component_of_node_at_index(v_in_res, 2, height_v); //update height
	     count[height_v]++;

      }
    }
}

void GraphManagerNew::Discharge(int itr_no, int src, map<string,long int>& map_src_tgt_to_eid, vector<int>& count, int N, GraphNew * residual_graph, map<int, int>& map_nseq_to_res_graph_nid, map<int, int>& nid_to_outlist_index){
	vector<int> outlist;
	outlist = residual_graph->get_outlist(src);
	int out_size = outlist.size();
	int curr_nseq_in_orig_graph, i;
	int curr_outlist_index = nid_to_outlist_index[src];	

	while( residual_graph->get_component_ids_of_node(src)[3]>0){
		for(i = curr_outlist_index; i< out_size; i++){
			int eid = outlist[i];
			int trgt_node = residual_graph->get_target_node(eid); 
			curr_nseq_in_orig_graph = residual_graph->get_component_ids_of_node(trgt_node)[0];
			if(residual_graph->get_component_ids_of_node(src)[3] == 0){ //if no such excess, 
				nid_to_outlist_index[src] = i;
				break;
			}
			else{	
				push(src, trgt_node, eid, map_src_tgt_to_eid, residual_graph);
	
			}
			
		}

		if(i==out_size && residual_graph->get_component_ids_of_node(src)[3] == 0)
			nid_to_outlist_index[src] = 0;

		if(residual_graph->get_component_ids_of_node(src)[3] > 0){
			int src_height = residual_graph->get_component_ids_of_node(src)[2];
			curr_nseq_in_orig_graph = residual_graph->get_component_ids_of_node(src)[0];
			if(count[src_height] == 1){
				Gap(itr_no, map_nseq_to_res_graph_nid, src_height, N, count, residual_graph); 
			}
			else
				Relabel(src,  count, N, residual_graph);		
			}
		nid_to_outlist_index[src] = 0;
		}

}


void GraphManagerNew::reset_edge(map<string,long int>& map_src_tgt_to_eid, int src, int tgt, int itr_no, GraphNew * residual_graph){
	int edge_id = get_node_from_src_tgt(src, tgt, map_src_tgt_to_eid);

	residual_graph->set_other_value1_in_edge(edge_id, itr_no); // identifier of mincut call
        residual_graph->set_other_value2_in_edge(edge_id, 1); // capacity

}


void GraphManagerNew::moveToFront(int i, vector<int>& A) {
  int temp = A[i];
  int n;
  for (n = i; n > 0; n--){
    A[n] = A[n-1];
  }
  A[0] = temp;
}


int GraphManagerNew::get_outgoing_edges_explored(GraphNew * original_graph, GraphNew * residual_graph, map<int, int>& map_nseq_to_res_graph_nid, map<string, long int>& map_src_tgt_to_eid, vector<vector<bool>>& edge_exist, int curr_node_id, int itr_no, int src_in_res, vector<int>& L, vector<bool>& isInL, int *nodeCount, int i, int j, map<int, int>& nid_to_outlist_index){
	vector<int>::iterator vec_itr;
	int succ_node_id, succ_node_seq, curr_nid_in_res, height_of_target, tar_nid_in_res, org_rev_eid;;
	vector<int> outlist;

 	outlist = original_graph->get_outlist(curr_node_id); //fetch outgoing edges of a node in ORIGINAL graph
	for(vec_itr = outlist.begin(); vec_itr != outlist.end(); vec_itr++) {
		 succ_node_id = original_graph->get_target_node(*vec_itr);
		 if(succ_node_id == curr_node_id) //self loop handling and checking already in the List
                        continue;
		
		if(succ_node_id!=i && succ_node_id!=j && !isInL[succ_node_id])
		 L[(*nodeCount)++] = succ_node_id;
		 isInL[succ_node_id] = true;

		//to know the reverse edge 
		 vector<int> outlist2 = original_graph->get_outlist(succ_node_id);
     		 for (vector<int>::iterator out_itr = outlist2.begin(); out_itr != outlist2.end(); out_itr++) {
                      if (original_graph->get_target_node(*out_itr) == curr_node_id) {
                           org_rev_eid = *out_itr;
                           break;
                   }

                 }
                 curr_nid_in_res = map_nseq_to_res_graph_nid[curr_node_id];

 		 if(map_nseq_to_res_graph_nid.find(succ_node_id) == map_nseq_to_res_graph_nid.end()) {//if target node is not created before - so no edge exist also
		 	tar_nid_in_res = create_node(itr_no, succ_node_id, residual_graph,  map_nseq_to_res_graph_nid, 0,0); 
			nid_to_outlist_index.insert(pair<int,int>(tar_nid_in_res, 0));// needed for relabel-to-front algo
			create_edge(itr_no, curr_nid_in_res, tar_nid_in_res, *vec_itr, map_src_tgt_to_eid, residual_graph);//forward edge
			
			create_edge(itr_no, tar_nid_in_res, curr_nid_in_res, org_rev_eid, map_src_tgt_to_eid, residual_graph);//backward edge
			
			edge_exist[curr_node_id][succ_node_id] = true;
			edge_exist[succ_node_id][curr_node_id] = true;
			 
			
		 }else{ // target node is already created -- but edge may or may not exist. if an edge exist(due to creation of reverse edge), then donot create that edge else do create
			tar_nid_in_res = map_nseq_to_res_graph_nid[succ_node_id];			

			if(residual_graph->get_component_ids_of_node(tar_nid_in_res)[1]!=itr_no)  // if the node is created in previouslyupdate height and excess to zero for i-th iteration of mincut
				create_node(itr_no, succ_node_id, residual_graph,  map_nseq_to_res_graph_nid, 0,0); 
							
			//edge may or may not exist	
			if(edge_exist[curr_node_id][succ_node_id] == true){
				if(residual_graph->get_other_value1_in_edge(get_node_from_src_tgt(curr_nid_in_res, tar_nid_in_res, map_src_tgt_to_eid))!=itr_no)
					reset_edge(map_src_tgt_to_eid, curr_nid_in_res, tar_nid_in_res, itr_no, residual_graph);
			}else {
				create_edge(itr_no, curr_nid_in_res, tar_nid_in_res, *vec_itr, map_src_tgt_to_eid, residual_graph);
				edge_exist[curr_node_id][succ_node_id] = true;
			}

			if(edge_exist[succ_node_id][curr_node_id] == true){
				if(residual_graph->get_other_value1_in_edge(get_node_from_src_tgt(tar_nid_in_res,  curr_nid_in_res, map_src_tgt_to_eid))!=itr_no)
					reset_edge(map_src_tgt_to_eid, tar_nid_in_res, curr_nid_in_res, itr_no, residual_graph);

			}else {
				create_edge(itr_no, tar_nid_in_res, curr_nid_in_res, org_rev_eid, map_src_tgt_to_eid, residual_graph);
				edge_exist[succ_node_id][curr_node_id] = true;
			}

			
		 }

	}
}

//anjan
void GraphManagerNew::MyMinCut(GraphNew * original_graph, int i, int j, int N, set<int>& cut_edges, set<int>& set_s){// N is the number of nodes, i is the curr node SEQ, j is the parent node SEQ.
	static GraphNew * residual_graph;
	static vector<vector<bool>> edge_exist(N+1,vector<bool>(N+1));
	static map<int, int> map_nseq_to_res_graph_nid;
	static map<string, long int> map_src_tgt_to_eid;
	static int num_of_nodes, new_gid, itr_no;
        vector<bool> is_outgoing_edges_explored(N+1), isInL(N+2); //this is only for current iteration
        vector<int> count(3*N), outlist, L(N+2); 
	int curr_nseq_in_orig_graph, nodeCount = 0;
   	map<int, int> nid_to_outlist_index; // this maps store the index of the neighboring node under consideration -- used for discharge

        count[0] = N-1;//Needed for GAP heuristic
        count[N] = 1;

	itr_no++; //node 0 has no out-list in original graph. due to mismatch in the original graph, we need to take this.
	//create the gra[h at the first iteration
    	if(itr_no==1){
            residual_graph = new GraphNew;
            num_of_nodes = original_graph->get_node_ids().size();
            new_gid = ++GraphManagerNew::graph_id_count;
            residual_graph->set_graph_id(new_gid);
            add_graph(new_gid, residual_graph);

    	}
	
	//create node i and j
   	int new_nid = create_node(itr_no, i, residual_graph,  map_nseq_to_res_graph_nid, N, original_graph->get_outlist(i).size()); 
 	create_node(itr_no, j, residual_graph,  map_nseq_to_res_graph_nid, 0,0);
	 
	
	//before pushing anything- check whether a node is explored in the residual graph
  	if(is_outgoing_edges_explored[i] == false){
		get_outgoing_edges_explored(original_graph, residual_graph, map_nseq_to_res_graph_nid, map_src_tgt_to_eid, edge_exist, i,itr_no, new_nid, L, isInL, &nodeCount, i, j, nid_to_outlist_index);
		is_outgoing_edges_explored[i] = true;
	}

	//compute preflow
	outlist = residual_graph->get_outlist(new_nid);	
	for(vector<int>::iterator vec_itr = outlist.begin(); vec_itr != outlist.end(); vec_itr++) {
		int eid = *vec_itr;
		int trgt_node = residual_graph->get_target_node(eid);
		curr_nseq_in_orig_graph = residual_graph->get_component_ids_of_node(trgt_node)[0];
		push(new_nid, trgt_node, eid, map_src_tgt_to_eid, residual_graph);
		
	}

	int nodeTracker = 0;
	while(nodeTracker != nodeCount){
		curr_nseq_in_orig_graph = L[nodeTracker];
		//cout<<curr_nseq_in_orig_graph<<" ";		
		int v =map_nseq_to_res_graph_nid[curr_nseq_in_orig_graph];
		int oldHeight = residual_graph->get_component_ids_of_node(v)[2];		
		if(is_outgoing_edges_explored[curr_nseq_in_orig_graph] == false){
			get_outgoing_edges_explored(original_graph, residual_graph, map_nseq_to_res_graph_nid, map_src_tgt_to_eid, edge_exist, curr_nseq_in_orig_graph,itr_no,new_nid, L, isInL, &nodeCount, i, j, nid_to_outlist_index);
			is_outgoing_edges_explored[curr_nseq_in_orig_graph] = true;
		}
		if(residual_graph->get_component_ids_of_node(v)[3] != 0)
			Discharge(itr_no, v, map_src_tgt_to_eid, count, N, residual_graph, map_nseq_to_res_graph_nid, nid_to_outlist_index);
		if(residual_graph->get_component_ids_of_node(v)[2] > oldHeight){
			moveToFront(nodeTracker, L);
			nodeTracker = 0;
		}
		else
			nodeTracker++;
	}


	// find the mincut edges
	set<int> visited;
    	list<int> bfs_queue;
	// find forward reachable nodes and edges from i
	set<int> set_nodes_frwd_reachable_from_i;
	set<int> set_frwd_edges_from_i_with_zero_capacity;

	bfs_queue.push_back(map_nseq_to_res_graph_nid[i]);
	while (!bfs_queue.empty()) {
		int curr_node = bfs_queue.front();
       		bfs_queue.pop_front();
		if (visited.find(curr_node) == visited.end()) {// can be make more efficient by taking a boolean vector work as a flag
			visited.insert(curr_node);
			set_nodes_frwd_reachable_from_i.insert(curr_node);
			int org_node = residual_graph->get_component_ids_of_node(curr_node)[0];
			if (org_node > i)
				set_s.insert(org_node); 
			// traverse outlist
			vector<int> outlist_of_i_side_node = residual_graph->get_outlist(curr_node);
			for (vector<int>::iterator outlist_of_i_itr = outlist_of_i_side_node.begin(); outlist_of_i_itr != outlist_of_i_side_node.end(); outlist_of_i_itr++) {
				int curr_edge = (*outlist_of_i_itr);
				if (residual_graph->get_other_value2_in_edge(curr_edge) == 0) { // stop at zero capacity edge
					set_frwd_edges_from_i_with_zero_capacity.insert(curr_edge);
				}else {
                    			bfs_queue.push_back(residual_graph->get_target_node(curr_edge));
                		}
			}
		}
	}

	// find backward reachable nodes and edges from j
	set<int> set_nodes_bkwd_reachable_from_j;
	set<int> set_bkwd_edges_from_j_with_zero_capacity;

	bfs_queue.push_back(map_nseq_to_res_graph_nid[j]);
	while (!bfs_queue.empty()) {
		int curr_node = bfs_queue.front();
       		bfs_queue.pop_front();
		if (visited.find(curr_node) == visited.end()) {// can be make more efficient by taking a boolean vector work as a flag
			visited.insert(curr_node);
			set_nodes_bkwd_reachable_from_j.insert(curr_node);
			// traverse inlist
			vector<int> inlist_of_j_side_node = residual_graph->get_inlist(curr_node);
			for (vector<int>::iterator inlist_of_j_itr = inlist_of_j_side_node.begin(); inlist_of_j_itr != inlist_of_j_side_node.end(); inlist_of_j_itr++) {
				int curr_edge = (*inlist_of_j_itr);
				if ((residual_graph->get_other_value2_in_edge(curr_edge) == 0)&&(set_frwd_edges_from_i_with_zero_capacity.find(curr_edge)!=set_frwd_edges_from_i_with_zero_capacity.end())) {
					set_bkwd_edges_from_j_with_zero_capacity.insert(curr_edge);
					cut_edges.insert(curr_edge);
				}else {
					if(residual_graph->get_other_value2_in_edge(curr_edge) == 0)
						set_bkwd_edges_from_j_with_zero_capacity.insert(curr_edge);
					bfs_queue.push_back(residual_graph->get_source_node(curr_edge));
				}
			}
	
		}
	}

/*	std::set<int>::iterator it;
	cout<<"\ncut edges are:";

	for (it = cut_edges.begin(); it != cut_edges.end(); ++it)
	{
		 int res_eid = *it;
		 int org_eid = residual_graph->get_other_value4_in_edge(res_eid);
		 int org_src = original_graph->get_source_node(org_eid);
		 int org_target = original_graph->get_target_node(org_eid);
   		 cout<<org_src<<"->"<<res_eid<<"("<< residual_graph->get_other_value2_in_edge(res_eid)<<")"<<"->"<<org_target<<endl; // Note the "*" here
	}


*/


}
// takes union of inlist and outlist of parent_i as a cut
void GraphManagerNew::compute_min_cut_dummy(GraphNew * graph, int i, int parent_i, set<int>& cut_edges, set<int>& set_s) {
    
    
    vector<int> inlist, outlist;
    inlist = graph->get_inlist(parent_i);
    outlist = graph->get_outlist(parent_i);
    
    vector<int>::iterator vec_itr;
    
    set_s.insert(parent_i);
    for (vec_itr = inlist.begin(); vec_itr != inlist.end(); vec_itr++) {
        cut_edges.insert(*vec_itr);
        set_s.insert(graph->get_source_node(*vec_itr));
    }
   
    for (vec_itr = outlist.begin(); vec_itr != outlist.end(); vec_itr++) {
        cut_edges.insert(*vec_itr);
        set_s.insert(graph->get_target_node(*vec_itr));
    }

#ifdef DEBUG_FLAG
    debug_log << " -- mincut between " << parent_i << " and " << i << " -- " << endl;
    
    for(set<int>::iterator set_itr = cut_edges.begin(); set_itr != cut_edges.end(); set_itr++) {
        debug_log << (*set_itr) << " ";
    }
    debug_log << endl;
#endif
    

}


void GraphManagerNew::compute_gh_tree(int gid, vector<int>& gomoryhu_parents, vector<set<int> >& cut_edges){
    GraphNew* graph = get_graph(gid);
    if (graph == NULL) {
        cerr << "Error: there is no graph for id " << gid << endl;
    }
    
    map<int, set<int> > frwd_bfs_results;
    map<int, set<int> > bkwd_bfs_results;
    
    int n = gomoryhu_parents.size();
cout<<"no of nodes = "<<n;
    for (int i = 2; i < n; i++) {
        
        set<int> set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut;
        set<int> set_edge_ids;
        set<int>::iterator set_itr;

	cout<<"\niteration"<<i-1<<"node id = "<<i<<" parent_id = "<<gomoryhu_parents[i]<<endl;
      //  compute_min_cut_for_ghtree(graph, i, gomoryhu_parents[i], set_edge_ids, set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut, frwd_bfs_results, bkwd_bfs_results);
        MyMinCut(graph, i, gomoryhu_parents[i], n, set_edge_ids, set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut);

    //    cut_edges[i] = set_edge_ids;
        
        for (set_itr = set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut.begin(); set_itr != set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut.end(); set_itr++) {
	    if(*set_itr>i && gomoryhu_parents[*set_itr]==gomoryhu_parents[i])
            	gomoryhu_parents[*set_itr] = i;

        }
        
        
    }
 //setting all the G-parent to 1.  
/*for (int i = 2; i < n; i++) { 
	cout<<"\niteration"<<i-1<<endl;
	MyMinCut(graph, i, gomoryhu_parents[i], n);
}*/
/*       set<int> set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut;
        set<int> set_edge_ids;
        set<int>::iterator set_itr;
MyMinCut(graph, 2230, 1167, n, set_edge_ids, set_nd_seq_nums_gt_i_with_same_parent_as_i_on_i_side_of_cut);*/
   // MyMinCut(graph, 1674, gomoryhu_parents[1674], n);
}

//void GraphManagerNew::connect(GraphNew * graph, set<int>& present_edges, vector< vector <set<int> > >& minCutMatrix, int source_nid, int target_nid){
//    //vector<int> curr_min_cut = minCutMatrix[source_nid-1][target_nid-1];
//    set<int> curr_min_cut = minCutMatrix[source_nid-1][target_nid-1];
//    set<int>::iterator set_itr;
//    cout << "Current min cut contains edges:" << endl;
//
//    for(set_itr = curr_min_cut.begin(); set_itr != curr_min_cut.end(); set_itr++){
//        cout << (*set_itr) << endl;
//    }
//    cout << endl;
//
//    // display graph drawn so far and edges in min-cut between current pair of nodes in red
//    cout << "Choose an edge:" << endl;
//    int chosen_edge_id;
//    cin >> chosen_edge_id;
//    EdgeNew * chosen_edge = graph->get_edge_from_eid(chosen_edge_id);
//    present_edges.insert(chosen_edge->get_edge_id());
//    
//    if (source_nid != chosen_edge->get_source())
//        connect(graph, present_edges, minCutMatrix, source_nid, chosen_edge->get_source());
//    
//    if(target_nid != chosen_edge->get_target())
//        connect(graph, present_edges, minCutMatrix, chosen_edge->get_target(), target_nid);
//}

void GraphManagerNew::connect(GraphNew * graph, set<int>& present_edges, vector <set<int> >& cut_edges, map<pair<int,int>, int>& cut_edge_index, int source_nid, int target_nid){
    int index = cut_edge_index[make_pair(source_nid,target_nid)];
    
    //vector<int> curr_min_cut = minCutMatrix[source_nid-1][target_nid-1];
    set<int> curr_min_cut = cut_edges[index];
    set<int>::iterator set_itr;
    cout << "Current min cut contains edges:" << endl;

    for(set_itr = curr_min_cut.begin(); set_itr != curr_min_cut.end(); set_itr++){
        cout << (*set_itr) << endl;
    }
    cout << endl;

    // display graph drawn so far and edges in min-cut between current pair of nodes in red
    cout << "Choose an edge:" << endl;
    int chosen_edge_id;
    cin >> chosen_edge_id;
    EdgeNew * chosen_edge = graph->get_edge_from_eid(chosen_edge_id);
    if (!graph->edge_has_subtype(chosen_edge_id, "dummy")) {
        present_edges.insert(chosen_edge->get_edge_id());
    }
    if (source_nid != chosen_edge->get_source())
        connect(graph, present_edges, cut_edges, cut_edge_index, source_nid, chosen_edge->get_source());
    
    if(target_nid != chosen_edge->get_target())
        connect(graph, present_edges, cut_edges, cut_edge_index, chosen_edge->get_target(), target_nid);
}

//returns id of the newly constructed reachability graph otherwise returns -1

int GraphManagerNew::bounded_reach(int gid, int direction, vector<string>& source_nodes_set, vector<string>& excluded_nodes_set, int bound) {
    int new_gid = -1;

    GraphNew* graph = get_graph(gid);
    if (graph == NULL) {
        cerr << "Error: there is no graph for id " << gid << endl;
        return -1;
    }
    if (bound < 0) {
        cerr << "Error: bound is " << bound << "(negative value), can't do reachability with negative bounds" << endl;
        return -1;
    }

    if (direction != FORWARD && direction != BACKWARD && direction != FORWARD_BACKWARD) {
        cerr << "Error: incorrect direction entry" << endl;
        return -1;
    }

    std::vector<int> source_node_ids, excluded_node_ids;

    std::vector<std::string>::iterator vec_str_itr;

    for (vec_str_itr = source_nodes_set.begin(); vec_str_itr != source_nodes_set.end(); vec_str_itr++) {
        string rid = graph->get_rep_id_from_id(*vec_str_itr);
        if (rid == "") {
            cerr << "Error: one of the source nodes " + *vec_str_itr << " has no rep id in the graph" << endl;
            continue;
        }
        int nid = graph->get_nid_from_rep_id(rid);
        if (nid == -1) {
            cerr << "Error: one of the source nodes " + *vec_str_itr << " has a rep id but no node id for it in the graph.\n";// You have left a bug in the code -- fix it" << endl;
            continue;
        }
        source_node_ids.push_back(nid);
    }

    for (vec_str_itr = excluded_nodes_set.begin(); vec_str_itr != excluded_nodes_set.end(); vec_str_itr++) {
        string rid = graph->get_rep_id_from_id(*vec_str_itr);
        if (rid == "") {
            cerr << "Error: one of the excluded nodes " + *vec_str_itr << " has no rep id in the graph" << endl;
            continue;
        }
        int nid = graph->get_nid_from_rep_id(rid);
        if (nid == -1) {
            cerr << "Error: one of the excluded nodes " + *vec_str_itr << " has a rep id but no node id for it in the graph.\n";// You have made a bug in the code -- fix it" << endl;
            continue;
        }
        excluded_node_ids.push_back(nid);
    }

    //new id for the new graph
    GraphNew* graph_ptr = new GraphNew;
    new_gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(new_gid);
    add_graph(new_gid, graph_ptr);

    //maintain a BFSish queue of nodes
    std::list<int> BFS_queue;

    //testing remove later:
    ofstream fout("rch_test");

    //maintain a set of visited nodes for reachability from source nodes
    set<int> visited_nodes;
    //maintain min distances of all nodes in the graph from source nodes (all initialized to max value of int but source nodes themselves set to 0)
    map<int, int> distances_from_source;
    vector<int> NIDSg = graph->get_node_ids();

    for (vector<int>::iterator node_itr = NIDSg.begin(); node_itr != NIDSg.end(); node_itr++) {
        distances_from_source[*node_itr] = std::numeric_limits<int>::max();
        //distances_from_source[*node_itr] = -1;
    }

    for (vector<int>::iterator source_node_itr = source_node_ids.begin(); source_node_itr != source_node_ids.end(); source_node_itr++) {
        //reach begins from source node if source node is not in the exclude list
        if (find(excluded_node_ids.begin(), excluded_node_ids.end(), *source_node_itr) == excluded_node_ids.end()) {


            distances_from_source[*source_node_itr] = 0;
            BFS_queue.push_back(*source_node_itr);
        }
    }



    //the BFSish loop based on queue emptiness
    while (!BFS_queue.empty()) {
#ifdef DEBUG_FLAG
        fout << "Queue size: " << BFS_queue.size() << endl;
#endif
        int nid = BFS_queue.front();
        visited_nodes.insert(nid);
        BFS_queue.pop_front();

        string rid = graph->get_rep_id_from_nid(nid);
        int nid_new = graph_ptr->get_nid_from_rep_id(rid);
        
        //if the reachability graph already has a node corresponding to this rid then there is no need to create such a node
        //o/w duplicate this node into the new graph
        if (nid_new == -1) {
//#ifdef DUP_CHECK_FLAG_SET
//            nid_new = duplicate_node(graph_ptr, graph, nid, false);
//#else
//            nid_new = duplicate_node(graph_ptr, graph, nid);
//#endif                                     
            nid_new = duplicate_node(graph_ptr, graph, nid, false);

        }

        vector<int> edge_list_in, edge_list_out;
        edge_list_in = graph->get_inlist(nid);

        edge_list_out = graph->get_outlist(nid);

        if (direction == FORWARD || direction == FORWARD_BACKWARD) {
            for (vector<int>::iterator edge_itr = edge_list_out.begin(); edge_itr != edge_list_out.end(); ++edge_itr) {

                int target_node = graph->get_target_node(*edge_itr);
                //if the target node in the outlist edges is not to be excluded and is not already visisted
                if (find(excluded_node_ids.begin(), excluded_node_ids.end(), target_node) == excluded_node_ids.end()) {
                    /*int temp_distance = distances_from_source[nid]+1;
                    int old_distance = distances_from_source[target_node];
                    int new_distance = min(temp_distance, old_distance);
                    distances_from_source[target_node] = new_distance;*/


                    int new_distance = distances_from_source[nid] + 1;
                    int old_distance = distances_from_source[target_node];
                    distances_from_source[target_node] = min(new_distance, old_distance);
#ifdef DEBUG_FLAG
                    fout << "Distance: " << new_distance << ", node: " << graph->get_rep_id_from_nid(target_node) << endl;
#endif

                    //if(new_distance <= bound){
                    if (old_distance == std::numeric_limits<int>::max() && new_distance <= bound) {
                        //if(visited_nodes.find(target_node) == visited_nodes.end() && find(BFS_queue.begin(), BFS_queue.end(), target_node) == BFS_queue.end()){//if not visited and not already in the queue push it
                        BFS_queue.push_back(target_node);
                        //}
                    }

                    /*if(visited_nodes.find(target_node) == visited_nodes.end()){//if not visited
                            BFS_queue.push_back(target_node);
                    }*/
                    if (distances_from_source[target_node] <= bound) {

//#ifdef DUP_CHECK_FLAG_SET
//                        duplicate_edge(graph_ptr, graph, *edge_itr, false);
//#else
//                        duplicate_edge(graph_ptr, graph, *edge_itr);
//#endif                                                                 
                        duplicate_edge(graph_ptr, graph, *edge_itr);

                        
                        
                        
                        
//#ifdef ASSOC_DISSOC_EDGE_COPY_FLAG_SET                                                                 
//                        //handle association/dissociation edges
//                        bool is_assoc = false, is_dissoc = false;
//                        /*if(graph_ptr->edge_has_subtype(*edge_itr, "association")){
//                                is_assoc = true;
//                        }
//                        if(graph_ptr->edge_has_subtype(*edge_itr, "dissociation")){
//                                is_dissoc = true;
//                        }*/
//                        if (graph->edge_has_subtype(*edge_itr, "association")) {
//                            is_assoc = true;
//                        }
//                        if (graph->edge_has_subtype(*edge_itr, "dissociation")) {
//                            is_dissoc = true;
//                        }
//                        if (is_assoc || is_dissoc) {
//
//                            //get the edge id for the opposite edge (undirected edge modeled as two directed edges in opposite directions)
//                            //vector<int> outlist_target_node = graph_ptr->get_outlist(target_node);
//                            vector<int> outlist_target_node = graph->get_outlist(target_node);
//                            int call_count = 0;
//                            for (vector<int>::iterator out_itr = outlist_target_node.begin(); out_itr != outlist_target_node.end(); ++out_itr) {
//                                /*if(graph_ptr->get_target_node(*out_itr) == nid && (graph_ptr->edge_has_subtype(*out_itr, "association")||graph_ptr->edge_has_subtype(*out_itr, "dissociation"))){
//                                        duplicate_edge(graph_ptr, graph, *out_itr);
//                                }*/
//                                if (graph->get_target_node(*out_itr) == nid && (graph->edge_has_subtype(*out_itr, "association") || graph->edge_has_subtype(*out_itr, "dissociation"))) {
////#ifdef DUP_CHECK_FLAG_SET
////                                    duplicate_edge(graph_ptr, graph, *out_itr, false);
////#else
////                                    duplicate_edge(graph_ptr, graph, *out_itr);
////#endif
//                                        duplicate_edge(graph_ptr, graph, *out_itr);
//                                        call_count++;
//                                }
//                            }
//#ifdef ASSERT_FLAG 
//                            cout << "Calls to duplicate edge: " << call_count << endl;
//                            assert (call_count == 1);
//#endif
//                        }
//
//                            
//#endif                                                                 
                    }
                }
            }
        }

        if (direction == BACKWARD || direction == FORWARD_BACKWARD) {
            for (vector<int>::iterator edge_itr = edge_list_in.begin(); edge_itr != edge_list_in.end(); ++edge_itr) {

                int local_source_node = graph->get_source_node(*edge_itr);
                if (find(excluded_node_ids.begin(), excluded_node_ids.end(), local_source_node) == excluded_node_ids.end()) {
                    /*int temp_distance = distances_from_source[nid]+1;
                    int old_distance = distances_from_source[local_source_node];
                    int new_distance = min(temp_distance, old_distance);
                    distances_from_source[local_source_node] = new_distance;*/

                    int new_distance = distances_from_source[nid] + 1;
                    int old_distance = distances_from_source[local_source_node];
                    distances_from_source[local_source_node] = min(new_distance, old_distance);

                    //if(new_distance <= bound){
                    if (old_distance == std::numeric_limits<int>::max() && new_distance <= bound) {
                        //if(visited_nodes.find(local_source_node) == visited_nodes.end() && find(BFS_queue.begin(), BFS_queue.end(), local_source_node) == BFS_queue.end()){//if not visited and not already in the queue push it
                        BFS_queue.push_back(local_source_node);
                        //}
                    }
                    /*if(visited_nodes.find(local_source_node) == visited_nodes.end()){//if not visited
                            BFS_queue.push_back(local_source_node);
                    }*/

                    if (distances_from_source[local_source_node] <= bound) {
//#ifdef DUP_CHECK_FLAG_SET
//                        duplicate_edge(graph_ptr, graph, *edge_itr, false);
//#else
//                        duplicate_edge(graph_ptr, graph, *edge_itr);
//#endif								 
                        duplicate_edge(graph_ptr, graph, *edge_itr);
//#ifdef ASSOC_DISSOC_EDGE_COPY_FLAG_SET
//                        //handle undirected edges
//                        bool is_assoc, is_dissoc;
//                        /*if(graph_ptr->edge_has_subtype(*edge_itr, "association")){
//                                is_assoc = true;
//                        }
//                        if(graph_ptr->edge_has_subtype(*edge_itr, "dissociation")){
//                                is_dissoc = true;
//                        }*/
//                        if (graph->edge_has_subtype(*edge_itr, "association")) {
//                            is_assoc = true;
//                        }
//                        if (graph->edge_has_subtype(*edge_itr, "dissociation")) {
//                            is_dissoc = true;
//                        }
//                        if (is_assoc || is_dissoc) {
//                            //get the edge id for the opposite edge (undirected edge modeled as two directed edges in posite directions)
//                            //vector<int> inlist_local_source_node = graph_ptr->get_inlist(local_source_node);
//                            vector<int> inlist_local_source_node = graph->get_inlist(local_source_node);
//                            for (vector<int>::iterator in_itr = inlist_local_source_node.begin(); in_itr != inlist_local_source_node.end(); ++in_itr) {
//                                /*if(graph_ptr->get_source_node(*in_itr) == nid && (graph_ptr->edge_has_subtype(*in_itr, "association")||graph_ptr->edge_has_subtype(*in_itr, "dissociation"))){
//                                        duplicate_edge(graph_ptr, graph, *in_itr);
//                                }*/
//                                if (graph->get_source_node(*in_itr) == nid && (graph->edge_has_subtype(*in_itr, "association") || graph->edge_has_subtype(*in_itr, "dissociation"))) {
//
////#ifdef DUP_CHECK_FLAG_SET
////                                    duplicate_edge(graph_ptr, graph, *in_itr, false);
////#else
////                                    duplicate_edge(graph_ptr, graph, *in_itr);
////#endif                                                                                     
//                                        duplicate_edge(graph_ptr, graph, *in_itr);
//
//                                }
//                            }
//                        }
//#endif
                    }
                }
            }
        }
    }

#ifdef REMOVE_CYCLE_FLAG
    if (direction == FORWARD) {
        char input_rm_cycles;
        cout << "Do you want to remove cycles in the reachable graph?" << endl;
        cout << "Enter 'y' or 'Y' for yes, any other key for no: ";
        cin >> input_rm_cycles;
        if (input_rm_cycles == 'y' || input_rm_cycles == 'Y') {
            set<int> edges_to_remove = graph_ptr->edges_to_remove_cycles(source_nodes_set);

            for (set<int>::iterator rm_edge_itr = edges_to_remove.begin(); rm_edge_itr != edges_to_remove.end(); ++rm_edge_itr) {
                //cout << graph_ptr->get_rep_id_from_nid(graph_ptr->get_source_node(*rm_edge_itr)) << "==>" << graph_ptr->get_rep_id_from_nid(graph_ptr->get_target_node(*rm_edge_itr)) << endl;
                graph_ptr->remove_edge(*rm_edge_itr);
                edge_id_to_graph_id.erase(*rm_edge_itr);
            }
        }
    }
    else if (direction == BACKWARD) {
        char output_rm_cycles;
        cout << "Do you want to remove cycles in the reachable graph?" << endl;
        cout << "Enter 'y' or 'Y' for yes, any other key for no: ";
        cin >> output_rm_cycles;
        if (output_rm_cycles == 'y' || output_rm_cycles == 'Y') {
            set<int> edges_to_remove = graph_ptr->edges_to_remove_cycles_backwards(source_nodes_set);

            for (set<int>::iterator rm_edge_itr = edges_to_remove.begin(); rm_edge_itr != edges_to_remove.end(); ++rm_edge_itr) {
                //cout << graph_ptr->get_rep_id_from_nid(graph_ptr->get_source_node(*rm_edge_itr)) << "==>" << graph_ptr->get_rep_id_from_nid(graph_ptr->get_target_node(*rm_edge_itr)) << endl;
                graph_ptr->remove_edge(*rm_edge_itr);
                edge_id_to_graph_id.erase(*rm_edge_itr);
            }
        }
    }
#endif
    // add component edges from components to complexes
    // adding these edges again these should not depend on reachability bound
    graph_ptr->add_component_edges_for_complexes(this);
    
    //for genesis:
    stringstream ss;
    ss << bound;
    string direction_str;
    if (direction == FORWARD) {
        direction_str = "FORWARD";
    } else if (direction == BACKWARD) {
        direction_str = "BACKWARD";
    }
    if (direction == FORWARD_BACKWARD) {
        direction_str = "UNDIRECTED";
    }

    string source_nodes_str = ", source nodes :";
    for (vector<string>::iterator itr = source_nodes_set.begin(); itr != source_nodes_set.end(); ++itr) {
        source_nodes_str += " " + *itr;
    }

    string excluded_nodes_str = ", excluded nodes :";
    for (vector<string>::iterator itr = excluded_nodes_set.begin(); itr != excluded_nodes_set.end(); ++itr) {
        excluded_nodes_str += " " + *itr;
    }

    string oper = "bounded reach: bound = " + ss.str() + ", direction = " + direction_str + source_nodes_str + excluded_nodes_str;
    list<int> empty_list;
    empty_list.push_back(gid);
    add_graph_genesis(new_gid, oper, empty_list);
    return new_gid;
}



int GraphManagerNew::select_subgraph(int gid, set<string>& nodes_set) {
    int new_gid = -1;
    
    GraphNew* graph = get_graph(gid);
    if (graph == NULL) {
        cerr << "Error: there is no graph with id " << gid << endl;
        return -1;
    }

    // get node nids from ids
    std::set<int> node_nid_set;
    std::set<std::string>::iterator set_itr;

    for(set_itr = nodes_set.begin(); set_itr != nodes_set.end(); set_itr++){
        string rep_id = graph->get_rep_id_from_id(*set_itr);
        if (rep_id != "") {
            int nid = graph->get_nid_from_rep_id(rep_id);
            node_nid_set.insert(nid);
        }
        
    }
    cout << "Number of entries in select file: " << node_nid_set.size() << endl;

    //new id for the new graph
    GraphNew* graph_ptr = new GraphNew;
    new_gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(new_gid);
    add_graph(new_gid, graph_ptr);

    //maintain a BFS queue of nodes
    std::list<int> BFS_queue;
    set<int> visited_nodes;
    vector<int> NIDSg = graph->get_node_ids();

    for (set<int>::iterator node_itr = node_nid_set.begin(); node_itr != node_nid_set.end(); node_itr++) {
        //reach begins from source node if source node is not in the exclude list

            BFS_queue.push_back(*node_itr);
        
    }
    
    //the BFS loop based on queue emptiness
    while (!BFS_queue.empty()) {
        int nid = BFS_queue.front();
        visited_nodes.insert(nid);
        BFS_queue.pop_front();

        string rid = graph->get_rep_id_from_nid(nid);
        int nid_new = graph_ptr->get_nid_from_rep_id(rid);

        //if the reachability graph already has a node corresponding to this rid then there is no need to create such a node
        //o/w duplicate this node into the new graph
        if (nid_new == -1) {
#ifdef DUP_CHECK_FLAG_SET
            nid_new = duplicate_node(graph_ptr, graph, nid, false);
#else
            nid_new = duplicate_node(graph_ptr, graph, nid);
#endif                                     


        }

        vector<int> edge_list_in, edge_list_out;
        edge_list_in = graph->get_inlist(nid);

        edge_list_out = graph->get_outlist(nid);


            for (vector<int>::iterator edge_itr = edge_list_out.begin(); edge_itr != edge_list_out.end(); ++edge_itr) {

                int target_node = graph->get_target_node(*edge_itr);
                
                if(node_nid_set.find(target_node) != node_nid_set.end()){
                    if(visited_nodes.find(target_node) == visited_nodes.end()){
                        BFS_queue.push_back(target_node);
                        
                    }
                    duplicate_edge(graph_ptr, graph, *edge_itr);

                }   
                
            }
        


    }


    string oper = "select";
    list<int> empty_list;
    empty_list.push_back(gid);
    add_graph_genesis(new_gid, oper, empty_list);
    return new_gid;
}


int GraphManagerNew::get_subgraph_with_edge_ids(GraphNew * graph, set<int> present_edges){
    int new_gid = -1;
    int gid = graph->get_graph_id();
    
    if (graph == NULL) {
        cerr << "Error: there is no graph with id " << gid << endl;
        return -1;
    }
    
    //new id for the new graph
    GraphNew* graph_ptr = new GraphNew;
    new_gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(new_gid);
    add_graph(new_gid, graph_ptr);

    
    for(set<int>::iterator itr = present_edges.begin(); itr != present_edges.end(); itr++){
        duplicate_edge(graph_ptr, graph, *itr);
    }
    
    return new_gid;
}



void GraphManagerNew::print_all_graph_manager_info() {
    string file_name;
    cout << "Enter the file to store all graph information" << endl;
    cin >> file_name;
    ofstream fout(file_name.c_str());

    if (fout.is_open()) {
        fout << "Global id to rep id map\n";
        print_map(all_id_map, fout);
        fout << endl;
        fout << "graph id to graph pointer map\n";
        print_map(graph_id_to_graph_map, fout);
        fout << endl;
        fout << "node_id_to_graph_id\n";
        print_map(node_id_to_graph_id, fout);
        fout << endl;
        fout << "edge_id_to_graph_id\n";
        print_map(edge_id_to_graph_id, fout);
        fout << endl;
        for (std::map<int, graph_genesis_new>::iterator itr = graph_id_to_genesis_map.begin(); itr != graph_id_to_genesis_map.end(); ++itr) {
            print_genesis_new(itr->first, fout);
        }
        fout.close();
    } else {
        cerr << "File " + file_name + " couldn't be opened" << endl;
    }
}

/*
void GraphManagerNew::read_graph_xml_file_as_it_is(std::string input_file){
        GraphNew* graph_ptr = new GraphNew;
        int gid = ++GraphManagerNew::graph_id_count;
        graph_ptr->set_graph_id(gid);
        add_graph(gid, graph_ptr);
        XMLParser *my_parser = new XMLParser();//for parsing input xml file from KEGG
        my_parser->fill_graph_from_xml_file_as_it_is(graph_ptr, input_file, this);
}
 */

int GraphManagerNew::read_graph_xml_file(std::string input_file, bool tool_written) {
    GraphNew* graph_ptr = new GraphNew;
    int gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(gid);
    add_graph(gid, graph_ptr);
    XMLParser *my_parser = new XMLParser(); //for parsing input xml file from KEGG
    if (my_parser->fill_graph_from_xml_file(graph_ptr, input_file, this, tool_written)) {
        //add component edges from components to complexes
        graph_ptr->add_component_edges_for_complexes(this);

        // genesis
        list<int> empty_list;
        string oper = "read_graph from file: " + input_file;
        add_graph_genesis(gid, oper, empty_list);
        cout << "Read graph: " << gid << " from file " << input_file << endl;
        return gid;
    } else {
        --GraphManagerNew::graph_id_count;
        return -1;
    }
}

void GraphManagerNew::create_map_from_sbml(string filename) {
    SBMLParser *my_sbml_parser = new SBMLParser();
    string map_file = "rmap.txt";
    ofstream fout(map_file.c_str());
    
    ifstream ifs(filename.c_str());
    string file;
    
    if (fout.is_open()) {
        while (getline(ifs, file)) {
            (*my_sbml_parser).create_map_from_sbml_file(fout, file);
        }
              
    }
    
    else {
        cout << "Could not open file for writing" << endl;
    }
    


}

int GraphManagerNew::read_graph_from_sbml(string sbmlfile) {
    
    GraphNew* graph_ptr = new GraphNew;
    int gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(gid);
    add_graph(gid, graph_ptr);
    SBMLParser *my_sbml_parser = new SBMLParser();
    
    bool success = (*my_sbml_parser).fill_graph_from_sbml_file(graph_ptr, sbmlfile, this);
            
    if (success) {
        //add component edges from components to complexes
        //graph_ptr->add_component_edges_for_complexes(this);
        graph_ptr->add_component_edges_for_complexes_sbml(this);
        // genesis
        list<int> empty_list;
        string oper = "read_graph from file: " + sbmlfile;
        add_graph_genesis(gid, oper, empty_list);
        cout << "Read graph: " << gid << " from file " << sbmlfile << endl;
        return gid;
    } else {
        --GraphManagerNew::graph_id_count;
        return -1;
    }


}

bool GraphManagerNew::fill_ppin_from_file(GraphNew* graph_ptr, string filename, GraphManagerNew* graph_man) {
    ifstream input_file(filename.c_str());
    string delim = "\t";
    string line;
    while (getline(input_file, line)) {
        // remove invalid characters
        char chars[] = ":";
        for (unsigned int i = 0; i < strlen(chars); ++i) {
            line.erase(std::remove(line.begin(), line.end(), chars[i]), line.end());
        }
        list<string> tokens = split_string_into_list(line, delim);
        string source_rep_id = tokens.front();
        tokens.pop_front();
        string source_display_name = tokens.front();
        tokens.pop_front();
        string target_rep_id = tokens.front();
        tokens.pop_front();
        string target_display_name = tokens.front();

        string node_type = "gene";
        int source_node_id = graph_ptr->check_if_node_already_created(source_rep_id);

        if (source_node_id == -1) {
            source_node_id = graph_ptr->create_new_node();

            //if failed to create node
            if (source_node_id == -1) {
                cerr << "Problem creating node in PPIN" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_node_id(source_node_id);
            graph_ptr->add_rep_id_for_node(source_node_id, source_rep_id);
            graph_man->add_node_id_graph_id(source_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(source_node_id, source_rep_id);
            graph_ptr->add_id_rep_id(source_rep_id, source_rep_id);
            graph_ptr->add_node_type(source_node_id, node_type);
            graph_ptr->add_display_id_for_node(source_node_id, source_display_name);
        }

        int target_node_id = graph_ptr->check_if_node_already_created(target_rep_id);
        if (target_node_id == -1) {
            target_node_id = graph_ptr->create_new_node();
            //if failed to create node
            if (target_node_id == -1) {
                cerr << "Problem creating node in PPIN" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_node_id(target_node_id);
            graph_ptr->add_rep_id_for_node(target_node_id, target_rep_id);
            graph_man->add_node_id_graph_id(target_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(target_node_id, target_rep_id);
            graph_ptr->add_id_rep_id(target_rep_id, target_rep_id);
            graph_ptr->add_node_type(target_node_id, node_type);
            graph_ptr->add_display_id_for_node(target_node_id, target_display_name);
        }

        string type = "PPrel";
        string subtype_ppin = "PPI";
        string pathway = "PPIN";
        vector<string> subtypes;
        subtypes.push_back(subtype_ppin);
        int eid = graph_ptr->check_if_edge_already_created(source_node_id, target_node_id, type, subtypes);
        if (eid == -1) {
            eid = graph_ptr->create_new_edge();
            if (eid == -1) {
                cerr << "Problem creating edge in PPIN" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(eid);
            graph_man->add_edge_id_graph_id(eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(eid, type);
            graph_ptr->add_edge_to_outlist_of_node(source_node_id, eid);
            graph_ptr->add_edge_to_inlist_of_node(target_node_id, eid);
            graph_ptr->add_source_node(eid, source_node_id);
            graph_ptr->add_target_node(eid, target_node_id);
            graph_ptr->add_subtype_for_edge(eid, subtype_ppin);
            graph_ptr->add_pathway_for_edge(eid, pathway);
        }
        // creating reverse edge
        int rev_eid = graph_ptr->check_if_edge_already_created(target_node_id, source_node_id, type, subtypes);

        if (rev_eid == -1) {
            rev_eid = graph_ptr->create_new_edge();
            if (rev_eid == -1) {
                cerr << "Problem creating reverse edge in PPIN" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(rev_eid);
            graph_man->add_edge_id_graph_id(rev_eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(rev_eid, type);
            graph_ptr->add_edge_to_outlist_of_node(target_node_id, rev_eid);
            graph_ptr->add_edge_to_inlist_of_node(source_node_id, rev_eid);
            graph_ptr->add_source_node(rev_eid, target_node_id);
            graph_ptr->add_target_node(rev_eid, source_node_id);
            graph_ptr->add_subtype_for_edge(rev_eid, subtype_ppin);
            graph_ptr->add_pathway_for_edge(rev_eid, pathway);
        }

    }

    return true;

}

int GraphManagerNew::create_graph_PPIN(string input_file) {
    GraphNew * graph_ptr = new GraphNew();
    int gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(gid);
    add_graph(gid, graph_ptr);
    bool success = fill_ppin_from_file(graph_ptr, input_file, this);
    if (success) {
        list<int> empty_list;
        string oper = "read_ppin from file: " + input_file;
        add_graph_genesis(gid, oper, empty_list);
        return gid;
    } else {
        --GraphManagerNew::graph_id_count;
        return -1;
    }
}

bool GraphManagerNew::fill_htri_from_file(GraphNew* graph_ptr, string filename, GraphManagerNew* graph_man) {
    ifstream input_file(filename.c_str());
    string delim = "\t";
    string line;
    while (getline(input_file, line)) {
        // remove invalid characters
        char chars[] = ":";
        for (unsigned int i = 0; i < strlen(chars); ++i) {
            line.erase(std::remove(line.begin(), line.end(), chars[i]), line.end());
        }
        list<string> tokens = split_string_into_list(line, delim);
        string source_rep_id = tokens.front();
        tokens.pop_front();
        string source_display_name = tokens.front();
        tokens.pop_front();
        string target_rep_id = tokens.front();
        tokens.pop_front();
        string target_display_name = tokens.front();

        string node_type = "gene";
        int source_node_id = graph_ptr->check_if_node_already_created(source_rep_id);

        if (source_node_id == -1) {
            source_node_id = graph_ptr->create_new_node();

            //if failed to create node
            if (source_node_id == -1) {
                cerr << "Problem creating node in HTRI" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_node_id(source_node_id);
            graph_ptr->add_rep_id_for_node(source_node_id, source_rep_id);
            graph_man->add_node_id_graph_id(source_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(source_node_id, source_rep_id);
            graph_ptr->add_id_rep_id(source_rep_id, source_rep_id);
            graph_ptr->add_node_type(source_node_id, node_type);
            graph_ptr->add_display_id_for_node(source_node_id, source_display_name);
        }

        int target_node_id = graph_ptr->check_if_node_already_created(target_rep_id);
        if (target_node_id == -1) {
            target_node_id = graph_ptr->create_new_node();
            //if failed to create node
            if (target_node_id == -1) {
                cerr << "Problem creating node in HTRI" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_node_id(target_node_id);
            graph_ptr->add_rep_id_for_node(target_node_id, target_rep_id);
            graph_man->add_node_id_graph_id(target_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(target_node_id, target_rep_id);
            graph_ptr->add_id_rep_id(target_rep_id, target_rep_id);
            graph_ptr->add_node_type(target_node_id, node_type);
            graph_ptr->add_display_id_for_node(target_node_id, target_display_name);
        }

        string type = "GErel";
        string subtype_expr = "expression";
        string subtype_repr = "repression";
        string pathway = "HTRI";
        vector<string> subtypes1;
        vector<string> subtypes2;
        subtypes1.push_back(subtype_expr);
        subtypes2.push_back(subtype_repr);
        int eid = graph_ptr->check_if_edge_already_created(source_node_id, target_node_id, type, subtypes1);
        if (eid == -1) {
            eid = graph_ptr->create_new_edge();
            if (eid == -1) {
                cerr << "Problem creating expression edge in HTRI" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(eid);
            graph_man->add_edge_id_graph_id(eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(eid, type);
            graph_ptr->add_edge_to_outlist_of_node(source_node_id, eid);
            graph_ptr->add_edge_to_inlist_of_node(target_node_id, eid);
            graph_ptr->add_source_node(eid, source_node_id);
            graph_ptr->add_target_node(eid, target_node_id);
            graph_ptr->add_subtype_for_edge(eid, subtype_expr);
            graph_ptr->add_pathway_for_edge(eid, pathway);
        }
        eid = graph_ptr->check_if_edge_already_created(source_node_id, target_node_id, type, subtypes2);
        if (eid == -1) {
            eid = graph_ptr->create_new_edge();
            if (eid == -1) {
                cerr << "Problem creating repression edge in HTRI" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(eid);
            graph_man->add_edge_id_graph_id(eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(eid, type);
            graph_ptr->add_edge_to_outlist_of_node(source_node_id, eid);
            graph_ptr->add_edge_to_inlist_of_node(target_node_id, eid);
            graph_ptr->add_source_node(eid, source_node_id);
            graph_ptr->add_target_node(eid, target_node_id);
            graph_ptr->add_subtype_for_edge(eid, subtype_repr);
            graph_ptr->add_pathway_for_edge(eid, pathway);
        }

    }

    return true;

}

int GraphManagerNew::create_graph_HTRI(string input_file) {
    GraphNew * graph_ptr = new GraphNew();
    int gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(gid);
    add_graph(gid, graph_ptr);
    bool success = fill_htri_from_file(graph_ptr, input_file, this);
    if (success) {
        list<int> empty_list;
        string oper = "read htri database from file: " + input_file;
        add_graph_genesis(gid, oper, empty_list);
        return gid;
    } else {
        --GraphManagerNew::graph_id_count;
        return -1;
    }
}

bool GraphManagerNew::fill_reactome_all_from_file(GraphNew* graph_ptr, string filename, GraphManagerNew* graph_man) {
    ifstream input_file(filename.c_str());
    string delim = "\t";
    string line;
    while (getline(input_file, line)) {
        // remove invalid characters
        char chars[] = ":";
        for (unsigned int i = 0; i < strlen(chars); ++i) {
            line.erase(std::remove(line.begin(), line.end(), chars[i]), line.end());
        }
        list<string> tokens = split_string_into_list(line, delim);
        string source_id = tokens.front();
        tokens.pop_front();
        //string source_display_name = tokens.front();
        tokens.pop_front();
        string target_id = tokens.front();
        tokens.pop_front();
        //string target_display_name = tokens.front();
        tokens.pop_front();
        string edge_annotation = tokens.front();
        tokens.pop_front();
        string edge_rep_id = tokens.front();
        //
        string edge_display_id = edge_rep_id;
        edge_rep_id.erase (edge_rep_id.begin(), edge_rep_id.begin()+6); 
        edge_rep_id = "reaction-" + edge_rep_id;
        
        //


        string node_type = "gene"; //"protein"
        string source_rep_id = source_id;
        map<string, string>::iterator all_id_iter = all_id_map.find(source_id);
        if (all_id_iter != all_id_map.end()) {
            source_rep_id = all_id_iter->second;
        }

        string target_rep_id = target_id;
        all_id_iter = all_id_map.find(target_id);
        if (all_id_iter != all_id_map.end()) {
            target_rep_id = all_id_iter->second;
        }

        string source_display_name = kegg_hsa_id_to_display_name_map[source_rep_id];
        string target_display_name = kegg_hsa_id_to_display_name_map[target_rep_id];
        //create edge only for reaction type
        if (edge_annotation == "reaction") {

            
            int source_node_id = graph_ptr->check_if_node_already_created(source_rep_id);

            if (source_node_id == -1) {
                source_node_id = graph_ptr->create_new_node();

                //if failed to create node
                if (source_node_id == -1) {
                    cerr << "Problem creating node in Reactome full" << endl;
                    destroy_graph(graph_ptr);
                    return false;
                }
                graph_ptr->add_node_id(source_node_id);
                graph_ptr->add_rep_id_for_node(source_node_id, source_rep_id);
                graph_man->add_node_id_graph_id(source_node_id, graph_ptr->get_graph_id());
                graph_ptr->add_id_for_node(source_node_id, source_rep_id);
                graph_ptr->add_id_rep_id(source_rep_id, source_rep_id);
                graph_ptr->add_node_type(source_node_id, node_type);
                graph_ptr->add_display_id_for_node(source_node_id, source_display_name);
            }

            int target_node_id = graph_ptr->check_if_node_already_created(target_rep_id);
            if (target_node_id == -1) {
                target_node_id = graph_ptr->create_new_node();
                //if failed to create node
                if (target_node_id == -1) {
                    cerr << "Problem creating node in Reactome full" << endl;
                    destroy_graph(graph_ptr);
                    return false;
                }
                graph_ptr->add_node_id(target_node_id);
                graph_ptr->add_rep_id_for_node(target_node_id, target_rep_id);
                graph_man->add_node_id_graph_id(target_node_id, graph_ptr->get_graph_id());
                graph_ptr->add_id_for_node(target_node_id, target_rep_id);
                graph_ptr->add_id_rep_id(target_rep_id, target_rep_id);
                graph_ptr->add_node_type(target_node_id, node_type);
                graph_ptr->add_display_id_for_node(target_node_id, target_display_name);
            }

            //int reaction_node_id = graph_ptr->check_if_node_already_created(edge_rep_id);
            //if(reaction_node_id == -1){
            int reaction_node_id = graph_ptr->create_new_node();
            //if failed to create node
            if (reaction_node_id == -1) {
                cerr << "Problem creating reaction node in Reactome full" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            string edge_type = "reaction";
            
            graph_ptr->add_node_id(reaction_node_id);
            graph_ptr->add_rep_id_for_node(reaction_node_id, edge_rep_id);
            graph_man->add_node_id_graph_id(reaction_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(reaction_node_id, edge_rep_id);
            graph_ptr->add_id_rep_id(edge_rep_id, edge_rep_id);
            graph_ptr->add_node_type(reaction_node_id, edge_type);
            graph_ptr->add_display_id_for_node(reaction_node_id, edge_display_id);
            graph_ptr->add_description_for_node(reaction_node_id, edge_display_id);
            //}

            

            string pathway = "Reactome_all";
            vector<string> subtypes;
            subtypes.push_back(edge_annotation);

            //int eid = graph_ptr->check_if_edge_already_created(source_node_id, target_node_id, edge_type, subtypes); // no need for this check for this file

            //if (eid == -1){
            //eid = graph_ptr->create_new_edge();
            int eid = graph_ptr->create_new_edge();
            if (eid == -1) {
                cerr << "Problem creating edge in Reactome full" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(eid);
            graph_man->add_edge_id_graph_id(eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(eid, edge_type);
            graph_ptr->add_edge_to_outlist_of_node(source_node_id, eid);
            graph_ptr->add_edge_to_inlist_of_node(reaction_node_id, eid);
            graph_ptr->add_source_node(eid, source_node_id);
            graph_ptr->add_target_node(eid, reaction_node_id);
            graph_ptr->add_subtype_for_edge(eid, edge_annotation);
            graph_ptr->add_pathway_for_edge(eid, pathway);
            //}

            int rev_eid = graph_ptr->create_new_edge();
            if (rev_eid == -1) {
                cerr << "Problem creating reverse edge in Reactome full" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(rev_eid);
            graph_man->add_edge_id_graph_id(rev_eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(rev_eid, edge_type);
            graph_ptr->add_edge_to_outlist_of_node(reaction_node_id, rev_eid);
            graph_ptr->add_edge_to_inlist_of_node(source_node_id, rev_eid);
            graph_ptr->add_source_node(rev_eid, reaction_node_id);
            graph_ptr->add_target_node(rev_eid, source_node_id);
            graph_ptr->add_subtype_for_edge(rev_eid, edge_annotation);
            graph_ptr->add_pathway_for_edge(rev_eid, pathway);




            eid = graph_ptr->create_new_edge();
            if (eid == -1) {
                cerr << "Problem creating edge in Reactome full" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(eid);
            graph_man->add_edge_id_graph_id(eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(eid, edge_type);
            graph_ptr->add_edge_to_outlist_of_node(reaction_node_id, eid);
            graph_ptr->add_edge_to_inlist_of_node(target_node_id, eid);
            graph_ptr->add_source_node(eid, reaction_node_id);
            graph_ptr->add_target_node(eid, target_node_id);
            graph_ptr->add_subtype_for_edge(eid, edge_annotation);
            graph_ptr->add_pathway_for_edge(eid, pathway);
            //}

            rev_eid = graph_ptr->create_new_edge();
            if (rev_eid == -1) {
                cerr << "Problem creating reverse edge in Reactome full" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(rev_eid);
            graph_man->add_edge_id_graph_id(rev_eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(rev_eid, edge_type);
            graph_ptr->add_edge_to_outlist_of_node(target_node_id, rev_eid);
            graph_ptr->add_edge_to_inlist_of_node(reaction_node_id, rev_eid);
            graph_ptr->add_source_node(rev_eid, target_node_id);
            graph_ptr->add_target_node(rev_eid, reaction_node_id);
            graph_ptr->add_subtype_for_edge(rev_eid, edge_annotation);
            graph_ptr->add_pathway_for_edge(rev_eid, pathway);

        }
    }

    return true;

}

int GraphManagerNew::create_graph_reactome_all(string input_file) {
    GraphNew * graph_ptr = new GraphNew();
    int gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(gid);
    add_graph(gid, graph_ptr);
    bool success = fill_reactome_all_from_file(graph_ptr, input_file, this);
    if (success) {
        list<int> empty_list;
        string oper = "read Reactome all interactions network from file: " + input_file;
        add_graph_genesis(gid, oper, empty_list);
        return gid;
    } else {
        --GraphManagerNew::graph_id_count;
        return -1;
    }
}


int GraphManagerNew::add_phenotype_edges(string filename) {
    GraphNew * graph_ptr = new GraphNew();
    int gid = ++GraphManagerNew::graph_id_count;
    graph_ptr->set_graph_id(gid);
    add_graph(gid, graph_ptr);

    ifstream input_file(filename.c_str());
    string delim = "\t";
    string line;
    while (getline(input_file, line)) {
        // remove invalid characters
        char chars[] = ":";
        for (unsigned int i = 0; i < strlen(chars); ++i) {
            line.erase(std::remove(line.begin(), line.end(), chars[i]), line.end());
        }
        list<string> tokens = split_string_into_list(line, delim);
        string source_rep_id = tokens.front();
        tokens.pop_front();
        string source_node_type = tokens.front();
        tokens.pop_front();
        string target_rep_id = tokens.front();
        tokens.pop_front();
        string target_node_type = tokens.front();
        tokens.pop_front();
        string pathway = tokens.front();


        string source_display_name = this->get_display_names_from_rep_id(source_rep_id);
        string target_display_name = this->get_display_names_from_rep_id(target_rep_id);
        int source_node_id = graph_ptr->check_if_node_already_created(source_rep_id);

        if (source_node_id == -1) {
            source_node_id = graph_ptr->create_new_node();

            //if failed to create node
            if (source_node_id == -1) {
                cerr << "Problem creating phenotype node" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_node_id(source_node_id);
            graph_ptr->add_rep_id_for_node(source_node_id, source_rep_id);
            this->add_node_id_graph_id(source_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(source_node_id, source_rep_id);
            graph_ptr->add_id_rep_id(source_rep_id, source_rep_id);
            graph_ptr->add_node_type(source_node_id, source_node_type);
            graph_ptr->add_display_id_for_node(source_node_id, source_display_name);
        }

        int target_node_id = graph_ptr->check_if_node_already_created(target_rep_id);
        if (target_node_id == -1) {
            target_node_id = graph_ptr->create_new_node();
            //if failed to create node
            if (target_node_id == -1) {
                cerr << "Problem creating phenotype node" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_node_id(target_node_id);
            graph_ptr->add_rep_id_for_node(target_node_id, target_rep_id);
            this->add_node_id_graph_id(target_node_id, graph_ptr->get_graph_id());
            graph_ptr->add_id_for_node(target_node_id, target_rep_id);
            graph_ptr->add_id_rep_id(target_rep_id, target_rep_id);
            graph_ptr->add_node_type(target_node_id, target_node_type);
            graph_ptr->add_display_id_for_node(target_node_id, target_display_name);
        }

        string type = "Phenotype";
        string subtype = "indirect";

        vector<string> subtypes;
        subtypes.push_back(subtype);
        int eid = graph_ptr->check_if_edge_already_created(source_node_id, target_node_id, type, subtypes);
        if (eid == -1) {
            eid = graph_ptr->create_new_edge();
            if (eid == -1) {
                cerr << "Problem creating phenotype edge" << endl;
                destroy_graph(graph_ptr);
                return false;
            }
            graph_ptr->add_edge_id(eid);
            this->add_edge_id_graph_id(eid, graph_ptr->get_graph_id());
            graph_ptr->add_edge_type(eid, type);
            graph_ptr->add_edge_to_outlist_of_node(source_node_id, eid);
            graph_ptr->add_edge_to_inlist_of_node(target_node_id, eid);
            graph_ptr->add_source_node(eid, source_node_id);
            graph_ptr->add_target_node(eid, target_node_id);
            graph_ptr->add_subtype_for_edge(eid, subtype);
            graph_ptr->add_pathway_for_edge(eid, pathway);
        }

    }
    return graph_ptr->get_graph_id();

}

void GraphManagerNew::destroy_graph(GraphNew* graph_ptr) {
    int gid = graph_ptr->get_graph_id();
    //cout << "destroying graph: " << gid << endl;

    //remove all edge-level information of this graph if any
    std::vector<int> edges_in_graph = graph_ptr->get_edge_ids();

    for (std::vector<int>::iterator edge_itr = edges_in_graph.begin(); edge_itr != edges_in_graph.end(); ++edge_itr) {
        delete_edge_id_graph_id(*edge_itr);
        EdgeNew* to_delete = graph_ptr->get_edge_from_eid(*edge_itr);
        if (to_delete) {
            delete to_delete;
        }
    }

    std::vector<int> nodes_in_graph = graph_ptr->get_node_ids();
    //remove all node-level information of this graph
    for (std::vector<int>::iterator node_itr = nodes_in_graph.begin(); node_itr != nodes_in_graph.end(); node_itr++) {
        delete_node_id_graph_id(*node_itr);
        NodeNew* to_delete = graph_ptr->get_node_from_nid(*node_itr);
        if (to_delete) {
            delete to_delete;
        }
    }
    //remove all graph-level information of this graph
    delete_graph_id_graph(gid);
    delete_from_map(graph_id_to_genesis_map, gid);
}

void GraphManagerNew::generate_constraints_using_rules_parser(int gid, list<rule_t* >& list_of_rules, t_ExpressionManager *em) {
    GraphNew* graph_ptr = get_graph(gid);
    if (graph_ptr == NULL) {
        cerr << "Error: no graph for graph id: " << gid << endl;
        return;
    }
    vector<t_Expression*> constraints_for_all_rules_vec; //stores constraints for all the rules
    int rule_num = 1;
    for (list<rule_t*>::iterator rule_itr = list_of_rules.begin(); rule_itr != list_of_rules.end(); rule_itr++) {
        rule_t* curr_rule_ptr = *rule_itr;
        t_Expression* cons_for_rule = get_constraints_for_rule(curr_rule_ptr, graph_ptr, em);
        if (cons_for_rule) {
            constraints_for_all_rules_vec.push_back(cons_for_rule);
        } else {
            cerr << "No constraints for rule: " << rule_num << endl;
        }
        rule_num++;
    }

    char include_DAG_reach_path_constraints;
    cout << "\nEnter 'y' or 'Y' to DAG reachable path constraints as well, or any other key to not: ";
    cin >> include_DAG_reach_path_constraints;

    if (include_DAG_reach_path_constraints == 'y' || include_DAG_reach_path_constraints == 'Y') {
        t_Expression* reach_path_constraints = graph_ptr->get_DAG_reach_path_constraints(em, this);
        if (reach_path_constraints == NULL) {
            cout << "Note: there are no path reach constraints" << endl;
        } else {
            constraints_for_all_rules_vec.push_back(reach_path_constraints);
        }
    }

    char reachability_at_constraint_solver_level;
    cout << "\nEnter 'y' or 'Y' to do reachability from perturbed node to differentially expressed nodes via constraint solving, or any other key to not: ";
    cin >> reachability_at_constraint_solver_level;

    if (reachability_at_constraint_solver_level == 'y' || reachability_at_constraint_solver_level == 'Y') {
        t_Expression* reach_constraint_solver_constraints = graph_ptr->get_reach_path_constraints(em, this);
        if (reach_constraint_solver_constraints == NULL) {
            cout << "Note: there are no constraints for doing constraint solving for reachability" << endl;
        } else {
            constraints_for_all_rules_vec.push_back(reach_constraint_solver_constraints);
        }
    }

    vector<t_Expression*>::iterator expr_vec_itr;
    /*assert(!constraints_for_all_rules_vec.empty());
    vector<t_Expression*>::iterator expr_vec_itr = constraints_for_all_rules_vec.begin();
    t_Expression* expr1 = *expr_vec_itr;
    assert(expr1);
    ++expr_vec_itr;
    while(expr_vec_itr != constraints_for_all_rules_vec.end()){
            t_Expression* expr2 = *expr_vec_itr;
            assert(expr2);
            expr1 = build_AND_constraints(expr1, expr2, 0, 0, em);
            ++expr_vec_itr;
    }*/

    /*char include_reach_path_constraints;
    cout << "\nEnter 'y' or 'Y' to reachable path constraints as well, or any other key to not: ";
    cin >> include_reach_path_constraints;

    if(include_reach_path_constraints == 'y' || include_reach_path_constraints == 'Y'){
            t_Expression* reach_path_constraints = graph_ptr->get_reach_path_constraints(em, this);
            if(reach_path_constraints == NULL){
                    cout << "Note: there are no path reach constraints" << endl;
            }
            else{
                    if(expr1 == NULL){
                            expr1 = reach_path_constraints;
                    }
                    else{
                            t_Expression* temp = build_AND_constraints(expr1, reach_path_constraints, 0, 0, em);
                            expr1 = temp;
                    }

            }
    }*/

    char choice;
    cout << "\nEnter 'y' to include microarray data in the constraints, \n or any other key to exclude microarray data ";
    cin >> choice;

    if (choice == 'y') {
        string micro_arr_decl;
        string micro_arr_assert;

        cout << "\nEnter the relative filepath to micro array declarations: ";
        cin >> micro_arr_decl;

        cout << "\nEnter the relative filepath to micro array assertions: ";
        cin >> micro_arr_assert;

        /*string micro_arr_decl = "psmd9_decl";
                string micro_arr_assert = "psmd9_assert";*/

        expr_vec_itr = constraints_for_all_rules_vec.begin();
        vector<t_Expression*> graph_micrarr_constraints = get_microarray_constraints(micro_arr_decl, micro_arr_assert, em);
        while (expr_vec_itr != constraints_for_all_rules_vec.end()) {
            graph_micrarr_constraints.push_back(*expr_vec_itr);
            ++expr_vec_itr;
        }

        char solve;
        cout << "\nEnter 'y' to solve the total constraints, \n or any other key to not ";
        cin >> solve;

        if (solve == 'y') {
            solve_constraints(graph_micrarr_constraints, em);
        }
    } else {
        char solve;
        cout << "\nEnter 'y' to solve constraints without microarray data, \n or any other key to not ";
        cin >> solve;

        if (solve == 'y') {
            solve_constraints(constraints_for_all_rules_vec, em);
        }
    }

    //following is just for writing to test_em file for inspection if needed
    t_Expression* expr1 = NULL;
    expr_vec_itr = constraints_for_all_rules_vec.begin();
    if (constraints_for_all_rules_vec.empty()) {
        cout << "Note: no constraints found for the given rules" << endl;
    } else {
        expr1 = *expr_vec_itr;
        assert(expr1);
        ++expr_vec_itr;
        while (expr_vec_itr != constraints_for_all_rules_vec.end()) {
            t_Expression* expr2 = *expr_vec_itr;
            assert(expr2);
            expr1 = build_AND_constraints(expr1, expr2, 0, 0, em);
            ++expr_vec_itr;
        }
    }
    t_Expression* constraints_all_rules = expr1; //got ANDing of all rules

    if (constraints_all_rules == NULL) {
        cout << "Note: no constraints generated, returning..." << endl;
    }

    //string test_constraints_out_file = "test";
    ofstream fout("test_em");

    if (fout.is_open()) {
        //em->printExpressionsToFileInSMT(constraints_all_rules, "test_smt");
        em->printExpressionToFileAsDAG("constraintsAllRules ", constraints_all_rules, fout);
        fout.close();
    } else {
        cerr << "\nUnable to open the output file test_em for constraints testing " << endl;
    }
}

t_Expression* GraphManagerNew::get_constraints_for_rule(rule_t* rule_ptr, GraphNew* graph_ptr, t_ExpressionManager* em) {
    //map<string, pair<quantifier_t, vector<Node*> > > antecedent_map;
    list_antecedent_t* antecedent_list = rule_ptr->antecedent;
    //set<map<string, Node*> * > set_tuples; //this set will store the final tuples obtained by going through all antecedents
    //set<map<string, Node*> > set_tuples; //this set will store the final tuples obtained by going through all antecedents (antecedents come before colon)
    //access_antecdents_for_filling_tuples(antecedent_list, set_tuples, graph);

    //static int i = 1;
    RuleTupleStoreNew tuple_store_obj(antecedent_list, graph_ptr, this); //tuples constructed in tuplle_store_obj
    //cout << "\nprinting tuples for rule: " << i << endl;
    //tuple_store_obj.print_all_tuples(graph_ptr);
    //++i;
    //for testing just print the tuples
    //print_all_tuples(set_tuples);

    /*consequent_t* consequent_curr_rule = rule_ptr->consequent;
            logical_stmt_t*  left_of_implication = consequent_curr_rule->left_of_implies;
            logical_stmt_t* right_of_implication = consequent_curr_rule->right_of_implies;*/
    //cout << "getting constraints for current rule" << endl;
    t_Expression* constraint_for_rule = tuple_store_obj.get_constraint_em(rule_ptr->consequent, em, this, graph_ptr);
    return constraint_for_rule;
    /*
            vector<t_Expression* > constraints_for_a_rule;//one element is the constraint for one tuple

            set<map<string, Node*> >::iterator set_tuples_itr;

            //go over each tuple and apply constraints on each tuple and fill it in the vector constraints_for_a_rule
            for(set_tuples_itr = set_tuples.begin(); set_tuples_itr != set_tuples.end(); ++set_tuples_itr){
                    map<string, Node*> curr_tuple = *set_tuples_itr;
                    t_Expression* constraint_for_current_tuple = get_constraint_for_tuple(curr_tuple, left_of_implication, right_of_implication, em);
                    //em->printExpressionToFileAsDAG("curr_tuple", constraint_for_current_tuple, cout);
                    constraints_for_a_rule.push_back(constraint_for_current_tuple);
            }*/

    //now constraints_for_a_rule contains all constraints for a rule pointed to by rule_ptr, all these need to be ANDed
}

set<int> GraphManagerNew::find_set_of_nodes_for_tuple(string& var_name, GraphNew* graph, set_nodes_t* set_nodes_parser, map<string, int>& tuple_ref) {
    //find the set of nodes as specified by set of nodes from the parser set_nodes_parser
    set<int> set_nodes_to_return;

    //base cases are the ones that have no operator in set_nodes_parser
    if (set_nodes_parser->opr == NONE_OPR) {
        //the case of returning all nodes in the graph
        if (set_nodes_parser->fun_name == ALL_NODES_FUN) {
            vector<int> NIDSg = graph->get_node_ids();
            set_nodes_to_return.insert(NIDSg.begin(), NIDSg.end());
        }
        else if (set_nodes_parser->fun_name == NODE_TYPE_FUN) {
            node_t node_type = set_nodes_parser->arg3_node;
            string node_type_str = get_node_type_str_from_node_type_parser(node_type);
            vector<int> NIDSg = graph->get_node_ids();
            for (vector<int>::iterator node_itr = NIDSg.begin(); node_itr != NIDSg.end(); ++node_itr) {
                if (graph->get_node_type(*node_itr) == node_type_str) {
                    set_nodes_to_return.insert(*node_itr);
                }
            }
        }
        else if (set_nodes_parser->fun_name == EDGE_TYPE_FUN || set_nodes_parser->fun_name == EDGE_SUBTYPE_FUN || set_nodes_parser->fun_name == EDGE_FUN) {
            if (tuple_ref.empty()) {
                cerr << "Error: existing tuples shouldn't be empty for EDGE(y,x,...), EDGE_TYPE(y,x,...) AND EDGE_SUBTYPE(y,x,...) cases, exiting . . ." << endl;
                exit(1);
            }
            string arg1(set_nodes_parser->arg1);
            string arg2(set_nodes_parser->arg2);

            map<string, int>::iterator map_itr;

            //curr_var_name becomes source node
            if (arg1 == var_name) {
                map_itr = tuple_ref.find(arg2);
                if (map_itr == tuple_ref.end()) {
                    cerr << "The argument variable " << arg2 << " couldn't be found, continuing . . ." << endl;
                } else {
                    if (set_nodes_parser->fun_name == EDGE_TYPE_FUN) {
                        edge_t edge_type = set_nodes_parser->arg3_edge;
                        get_source_nodes(set_nodes_to_return, map_itr->second, edge_type, graph);
                    } else if (set_nodes_parser->fun_name == EDGE_SUBTYPE_FUN) {
                        edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
                        get_source_nodes(set_nodes_to_return, map_itr->second, edge_subtype, graph);
                    } else if (set_nodes_parser->fun_name == EDGE_FUN) {
                        edge_t edge_type = set_nodes_parser->arg3_edge;
                        list_edge_subtypes_t* edge_sub_list = set_nodes_parser->arg4_edgesub_list;
                        get_source_nodes(set_nodes_to_return, map_itr->second, edge_type, edge_sub_list, graph);
                    }
                }
            }
                //curr_var_name becomes target node
            else if (arg2 == var_name) {
                map_itr = tuple_ref.find(arg1);
                if (map_itr == tuple_ref.end()) {
                    cerr << "The argument variable " << arg1 << " couldn't be found, continuing . . ." << endl;
                } else {
                    if (set_nodes_parser->fun_name == EDGE_TYPE_FUN) {
                        edge_t edge_type = set_nodes_parser->arg3_edge;
                        get_target_nodes(set_nodes_to_return, map_itr->second, edge_type, graph);
                    } else if (set_nodes_parser->fun_name == EDGE_SUBTYPE_FUN) {
                        edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
                        get_target_nodes(set_nodes_to_return, map_itr->second, edge_subtype, graph);
                    } else if (set_nodes_parser->fun_name == EDGE_FUN) {
                        edge_t edge_type = set_nodes_parser->arg3_edge;
                        list_edge_subtypes_t* edge_sub_list = set_nodes_parser->arg4_edgesub_list;
                        get_target_nodes(set_nodes_to_return, map_itr->second, edge_type, edge_sub_list, graph);
                    }
                }
            }
            else {
                cerr << "Error: current variable name doesn't match one of the arguments, exiting . . ." << endl;
                exit(1);
            }
        }
        else if (set_nodes_parser->fun_name == PRED_FUN || set_nodes_parser->fun_name == SUCC_FUN) {
            if (tuple_ref.empty()) {
                cerr << "Error: existing tuples shouldn't be empty for PRED(x) or SUCC(x) cases, exiting . . ." << endl;
                exit(1);
            }
            string arg1(set_nodes_parser->arg1);
            map<string, int>::iterator map_itr;
            map_itr = tuple_ref.find(arg1);

            if (set_nodes_parser->fun_name == SUCC_FUN) {
                vector<int> out_list = graph->get_outlist(map_itr->second);
                for (vector<int>::iterator out_itr = out_list.begin(); out_itr != out_list.end(); ++out_itr) {
                    set_nodes_to_return.insert(graph->get_target_node(*out_itr));
                }
            } else if (set_nodes_parser->fun_name == PRED_FUN) {
                vector<int> in_list = graph->get_inlist(map_itr->second);
                for (vector<int>::iterator in_itr = in_list.begin(); in_itr != in_list.end(); ++in_itr) {
                    set_nodes_to_return.insert(graph->get_source_node(*in_itr));
                }
            }
        }
        else if (set_nodes_parser->fun_name == NODE_INPUT_EDGE_TYPE_FUN || set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_TYPE_FUN || set_nodes_parser->fun_name == NODE_INPUT_EDGE_SUBTYPE_FUN || set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_SUBTYPE_FUN) {
            //go over each node and check if its incoming or outgoing edge as required satisfies the criteria and accordingly insert it into the set
            vector<int> NIDSg = graph->get_node_ids();
            for (vector<int>::iterator node_itr = NIDSg.begin(); node_itr != NIDSg.end(); ++node_itr) {
                bool insert_node = false; //when this tag is true then insert the current node
                if (set_nodes_parser->fun_name == NODE_INPUT_EDGE_TYPE_FUN) {
                    edge_t edge_type = set_nodes_parser->arg3_edge;
                    insert_node = node_has_input_edge_of_type(*node_itr, edge_type, graph);
                } else if (set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_TYPE_FUN) {
                    edge_t edge_type = set_nodes_parser->arg3_edge;
                    insert_node = node_has_output_edge_of_type(*node_itr, edge_type, graph);
                } else if (set_nodes_parser->fun_name == NODE_INPUT_EDGE_SUBTYPE_FUN) {
                    edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
                    insert_node = node_has_input_edge_of_subtype(*node_itr, edge_subtype, graph);
                } else if (set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_SUBTYPE_FUN) {
                    edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
                    insert_node = node_has_output_edge_of_subtype(*node_itr, edge_subtype, graph);
                }
                if (insert_node) {
                    set_nodes_to_return.insert(*node_itr);
                }
            }
        }
        else if (set_nodes_parser->fun_name == NONE_FUN) {
            cerr << "NONE_FUN seen, exiting . . ." << endl;
            exit(1);
        }
    }
        //for other case make recursive calls//for other case make recursive calls
    else if (set_nodes_parser->opr == OR_OPR || set_nodes_parser->opr == AND_OPR) {
        set<int> left_set = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser->left, tuple_ref);
        set<int> right_set = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser->right, tuple_ref);
        if (set_nodes_parser->opr == OR_OPR) {
            set_union(left_set.begin(), left_set.end(), right_set.begin(), right_set.end(), inserter(set_nodes_to_return, set_nodes_to_return.begin()));
        } else if (set_nodes_parser->opr == AND_OPR) {
            set_intersection(left_set.begin(), left_set.end(), right_set.begin(), right_set.end(), inserter(set_nodes_to_return, set_nodes_to_return.begin()));
        }
    } else if (set_nodes_parser->opr == NOT_OPR) {
        set<int> to_negate_set = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser->left, tuple_ref);
        vector<int> NIDSg = graph->get_node_ids();
        set<int> universe_set(NIDSg.begin(), NIDSg.end());
        set_difference(universe_set.begin(), universe_set.end(), to_negate_set.begin(), to_negate_set.end(), inserter(set_nodes_to_return, set_nodes_to_return.begin()));
    }
    return set_nodes_to_return;
}

string GraphManagerNew::get_node_type_str_from_node_type_parser(node_t node_type) {
    if (node_type == GENE_NODE) {
        return string("gene");
    } else if (node_type == GROUP_NODE) {
        return string("group");
    } else if (node_type == COMPOUND_NODE) {
        return string("compound");
    } else if (node_type == NONE_NODE) {
        cerr << "Can't convert NONE_NODE to string, exiting . . ." << endl;
        exit(1);
    } else {
        cerr << "Unknown node type, bug???, exiting . . ." << endl;
        exit(1);
    }
}

string GraphManagerNew::get_edge_type_str_from_edge_type_parser(edge_t edge_type) {
    if (edge_type == PPREL_EDGE) {
        return string("PPrel");
    } else if (edge_type == ECREL_EDGE) {
        return string("ECrel");
    } else if (edge_type == GEREL_EDGE) {
        return string("GErel");
    } else if (edge_type == PCREL_EDGE) {
        return string("PCrel");
    } else if (edge_type == NONE_EDGE) {
        cerr << "Can't convert NONE_EDGE to string, bug??? exiting . . ." << endl;
        exit(1);
    } else {
        cerr << "Unknown edge type, bug???, exiting . . ." << endl;
        exit(1);
    }
}

string GraphManagerNew::get_edge_subtype_str_from_edge_subtype_parser(edge_subt edge_subtype) {
    if (edge_subtype == ACTIVE_EDGE) {
        return string("activation");
    } else if (edge_subtype == INHIBIT_EDGE) {
        return string("inhibition");
    } else if (edge_subtype == EXPRESS_EDGE) {
        return string("expression");
    } else if (edge_subtype == REPRESS_EDGE) {
        return string("repression");
    } else if (edge_subtype == INDIRECT_EDGE) {
        return string("indirect");
    } else if (edge_subtype == STATE_CHANGE_EDGE) {
        return string("state");
    } else if (edge_subtype == ASSOC_EDGE) {
        return string("association");
    } else if (edge_subtype == DISSOC_EDGE) {
        return string("dissociation");
    } else if (edge_subtype == MISSING_INTERACTION_EDGE) {
        return string("missing");
    } else if (edge_subtype == STATE_CHANGE_EDGE) {
        return string("state");
    } else if (edge_subtype == COMPONENT_EDGE) {
        return string("component");
    } else if (edge_subtype == NONE_EDGE_SUB) {
        cerr << "Can't convert NONE_EDGE_SUB to string, bug??? exiting . . ." << endl;
        exit(1);
    } else {
        cerr << "Unknown edge subtype, bug???, exiting . . ." << endl;
        exit(1);
    }
}

std::vector<std::string> GraphManagerNew::get_vector_edge_subtype_str_from_edge_subtype_parser_list(list_edge_subtypes_t* list_edge_subtypes) {
    vector<string> to_return;
    while (list_edge_subtypes) {
        string curr_subtype = get_edge_subtype_str_from_edge_subtype_parser(list_edge_subtypes->edge_subtype_data);
        to_return.push_back(curr_subtype);
        list_edge_subtypes = list_edge_subtypes->next;
    }
    return to_return;
}

std::vector<std::string> GraphManagerNew::get_non_negated_edge_subtypes(ANDed_edgesub_literals_t* ANDed_clause) {
    vector<string> to_return;
    while (ANDed_clause) {
        edge_subt_literal_t* curr_subtype_literal = ANDed_clause->curr_literal;
        if (curr_subtype_literal->opr == NONE_OPR) {
            string curr_subtype = get_edge_subtype_str_from_edge_subtype_parser(curr_subtype_literal->edge_subtype_data);
            to_return.push_back(curr_subtype);
        } else if (curr_subtype_literal->opr == AND_OPR || curr_subtype_literal->opr == OR_OPR) {
            cerr << "Error: can't have AND_OPR, OR_OPR for a liters, either NONE_OPR or NOT_OPR allowed --- bug!!!, exiting..." << endl;
            exit(1);
        }
        ANDed_clause = ANDed_clause->next;
    }
    return to_return;
}

std::vector<std::string> GraphManagerNew::get_negated_edge_subtypes(ANDed_edgesub_literals_t* ANDed_clause) {
    vector<string> to_return;
    while (ANDed_clause) {
        edge_subt_literal_t* curr_subtype_literal = ANDed_clause->curr_literal;
        if (curr_subtype_literal->opr == NOT_OPR) {
            string curr_subtype = get_edge_subtype_str_from_edge_subtype_parser(curr_subtype_literal->edge_subtype_data);
            to_return.push_back(curr_subtype);
        } else if (curr_subtype_literal->opr == AND_OPR || curr_subtype_literal->opr == OR_OPR) {
            cerr << "Error: can't have AND_OPR, OR_OPR for a liters, either NONE_OPR or NOT_OPR allowed --- bug!!!, exiting..." << endl;
            exit(1);
        }
        ANDed_clause = ANDed_clause->next;
    }
    return to_return;
}

//fill node_array with the the nodes that are the source nodes
//of the edges of edge_type directed to target_node_id

void GraphManagerNew::get_source_nodes(set<int>& source_node_array, int target_node_id, edge_t edge_type, GraphNew* graph_ptr) {
    vector<int> target_inlist = graph_ptr->get_inlist(target_node_id);
    string edge_type_str = get_edge_type_str_from_edge_type_parser(edge_type);
    for (vector<int>::iterator in_itr = target_inlist.begin(); in_itr != target_inlist.end(); ++in_itr) {
        if (graph_ptr->get_edge_type(*in_itr) == edge_type_str) {
            source_node_array.insert(graph_ptr->get_source_node(*in_itr));
        }
    }
}

//fill node_array with the the nodes that are the source nodes
//of the edges of subtype directed to target_node_id

void GraphManagerNew::get_source_nodes(set<int>& source_node_array, int target_node_id, edge_subt edge_subtype, GraphNew* graph_ptr) {
    vector<int> target_inlist = graph_ptr->get_inlist(target_node_id);
    string edge_sub_type_str = get_edge_subtype_str_from_edge_subtype_parser(edge_subtype);
    for (vector<int>::iterator in_itr = target_inlist.begin(); in_itr != target_inlist.end(); ++in_itr) {
        if (graph_ptr->edge_has_subtype(*in_itr, edge_sub_type_str)) {
            source_node_array.insert(graph_ptr->get_source_node(*in_itr));
        }
    }
}

//fill node_array with the the nodes that are the source nodes
//of the edges of type edge_type, directed to target_node_id and edge_sub_list is a subset of the subtypes in those edges

void GraphManagerNew::get_source_nodes(set<int>& source_node_array, int target_node_id, edge_t edge_type, list_edge_subtypes_t* edge_sub_list, GraphNew* graph_ptr) {
    vector<int> target_inlist = graph_ptr->get_inlist(target_node_id);
    for (vector<int>::iterator in_itr = target_inlist.begin(); in_itr != target_inlist.end(); ++in_itr) {
        if (edge_type_subtype_subset_match(*in_itr, edge_type, edge_sub_list, graph_ptr)) {
            source_node_array.insert(graph_ptr->get_source_node(*in_itr));
        }
    }
}

//fill node_array with the the nodes that are the target nodes
//of the edges of type emanating from source_node_id

void GraphManagerNew::get_target_nodes(set<int>& target_node_array, int source_node_id, edge_t edge_type, GraphNew* graph_ptr) {
    vector<int> source_outlist = graph_ptr->get_outlist(source_node_id);
    string edge_type_str = get_edge_type_str_from_edge_type_parser(edge_type);
    for (vector<int>::iterator out_itr = source_outlist.begin(); out_itr != source_outlist.end(); ++out_itr) {
        if (graph_ptr->get_edge_type(*out_itr) == edge_type_str) {
            target_node_array.insert(graph_ptr->get_target_node(*out_itr));
        }
    }
}

//fill node_array with the the nodes that are the target nodes
//of the edges of subtype emanating from source_node_id

void GraphManagerNew::get_target_nodes(set<int>& target_node_array, int source_node_id, edge_subt edge_subtype, GraphNew* graph_ptr) {
    vector<int> source_outlist = graph_ptr->get_outlist(source_node_id);
    string edge_sub_type_str = get_edge_subtype_str_from_edge_subtype_parser(edge_subtype);
    for (vector<int>::iterator out_itr = source_outlist.begin(); out_itr != source_outlist.end(); ++out_itr) {
        if (graph_ptr->edge_has_subtype(*out_itr, edge_sub_type_str)) {
            target_node_array.insert(graph_ptr->get_target_node(*out_itr));
        }
    }
}

//fill node_array with the the nodes that are the target nodes
//of the edges of type edge_type, emanating from source_node_id and edge_sub_list is a subset of the subtypes in those edges

void GraphManagerNew::get_target_nodes(set<int>& target_node_array, int source_node_id, edge_t edge_type, list_edge_subtypes_t* edge_sub_list, GraphNew* graph_ptr) {
    vector<int> source_outlist = graph_ptr->get_outlist(source_node_id);
    for (vector<int>::iterator out_itr = source_outlist.begin(); out_itr != source_outlist.end(); ++out_itr) {
        if (edge_type_subtype_subset_match(*out_itr, edge_type, edge_sub_list, graph_ptr)) {
            target_node_array.insert(graph_ptr->get_target_node(*out_itr));
        }
    }
}

bool GraphManagerNew::node_has_input_edge_of_type(int nid, edge_t edge_type, GraphNew* graph) {
    vector<int> inlist = graph->get_inlist(nid);
    string edge_type_str = get_edge_type_str_from_edge_type_parser(edge_type);

    for (vector<int>::iterator itr = inlist.begin(); itr != inlist.end(); ++itr) {
        if (graph->get_edge_type(*itr) == edge_type_str) {
            return true;
        }
    }
    return false;
}

bool GraphManagerNew::node_has_output_edge_of_type(int nid, edge_t edge_type, GraphNew* graph) {
    vector<int> outlist = graph->get_outlist(nid);
    string edge_type_str = get_edge_type_str_from_edge_type_parser(edge_type);

    for (vector<int>::iterator itr = outlist.begin(); itr != outlist.end(); ++itr) {
        if (graph->get_edge_type(*itr) == edge_type_str) {
            return true;
        }
    }
    return false;
}

bool GraphManagerNew::node_has_input_edge_of_subtype(int nid, edge_subt edge_subtype, GraphNew* graph) {
    vector<int> inlist = graph->get_inlist(nid);
    string edge_sub_type_str = get_edge_subtype_str_from_edge_subtype_parser(edge_subtype);
    for (vector<int>::iterator itr = inlist.begin(); itr != inlist.end(); ++itr) {
        if (graph->edge_has_subtype(*itr, edge_sub_type_str)) {
            return true;
        }
    }
    return false;
}

bool GraphManagerNew::node_has_output_edge_of_subtype(int nid, edge_subt edge_subtype, GraphNew* graph) {
    vector<int> outlist = graph->get_outlist(nid);
    string edge_sub_type_str = get_edge_subtype_str_from_edge_subtype_parser(edge_subtype);
    for (vector<int>::iterator itr = outlist.begin(); itr != outlist.end(); ++itr) {
        if (graph->edge_has_subtype(*itr, edge_sub_type_str)) {
            return true;
        }
    }
    return false;
}

t_Expression* GraphManagerNew::get_constraint_for_tuple(map<string, int>& curr_tuple_ref, logical_stmt_t* left_of_implication, logical_stmt_t* right_of_implication, t_ExpressionManager* em, GraphNew* graph) {
    t_Expression* left_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, left_of_implication, em, graph);
    t_Expression* right_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, right_of_implication, em, graph);

    assert(left_expr != NULL);
    assert(right_expr != NULL);
    //em->printExpressionToFileAsDAG("left_constraint", left_expr, cout);
    //em->printExpressionToFileAsDAG("right_constraint", right_expr, cout);

    t_Expression* implication_expr = NULL;
    implication_expr = build_OR_constraints(left_expr, right_expr, 1, 0, em); //this is basically implication left_expr -> right_expr
    assert(implication_expr != NULL);
    //em->printExpressionToFileAsDAG("implication_constraint", implication_expr, cout);
    return implication_expr;
}

t_Expression* GraphManagerNew::get_constraint_for_logic_stmt_and_tuple(map<string, int>& curr_tuple_ref, logical_stmt_t* logic_stmt, t_ExpressionManager* em, GraphNew* graph) {
    t_Expression* expression_to_return = NULL;
    map<string, int>::iterator tuple_itr;
    //base case
    if (logic_stmt->opr == NONE_OPR) {
        if (logic_stmt->fun_name2 == NODE_FUN2) {
            string arg1(logic_stmt->arg1);
            tuple_itr = curr_tuple_ref.find(arg1);
            if (tuple_itr == curr_tuple_ref.end()) {
                cerr << "There is no identifier " + arg1 + " in the tuples generated, exiting . . ." << endl;
                exit(1);
            } else if (logic_stmt->state_node == ACTIVATED_STATE || logic_stmt->state_node == INHIBITED_STATE || logic_stmt->state_node == REPRESSED_STATE || logic_stmt->state_node == OVEREXPRESSED_STATE) {// ACTIVATED_STATE, INHIBITED_STATE, REPRESSED_STATE, OVEREXPRESSED_STATE
                expression_to_return = get_node_constraint(tuple_itr->second, logic_stmt->state_node, em, graph);
            } else if (logic_stmt->state_node2 == DIFFER_EXPRESSED_STATE || logic_stmt->state_node2 == NOT_DIFFER_EXPRESSED_STATE) {//DIFFER_EXPRESSED_STATE, NOT_DIFFER_EXPRESSED_STATE
                expression_to_return = get_node_constraint(tuple_itr->second, logic_stmt->state_node2, em, graph);
            } else {
                cerr << "\nBUG: debug node state issues ";
                exit(1);
            }
        } else if (logic_stmt->fun_name2 == EDGE_FUN2 || logic_stmt->fun_name2 == EDGE_SUBTYPE_DNF_FUN2) {
            string arg1(logic_stmt->arg1);
            string arg2(logic_stmt->arg2);
            int node1, node2;
            tuple_itr = curr_tuple_ref.find(arg1);
            if (tuple_itr == curr_tuple_ref.end()) {
                cerr << "There is no identifier " + arg1 + " in the tuples generated, exiting . . ." << endl;
                exit(1);
            } else {
                node1 = tuple_itr->second;
            }
            tuple_itr = curr_tuple_ref.find(arg2);
            if (tuple_itr == curr_tuple_ref.end()) {
                cerr << "There is no identifier " + arg2 + " in the tuples generated, exiting . . ." << endl;
                exit(1);
            } else {
                node2 = tuple_itr->second;
            }
            if (logic_stmt->fun_name2 == EDGE_FUN2) {
                expression_to_return = get_edge_constraint(node1, node2, logic_stmt->state_edge, logic_stmt->arg3, logic_stmt->arg4_list, em, graph);
            } else if (logic_stmt->fun_name2 == EDGE_SUBTYPE_DNF_FUN2) {
                expression_to_return = get_edge_constraint(node1, node2, logic_stmt->state_edge, logic_stmt->arg3, logic_stmt->arg_DNF_edgesub, em, graph);
            }
        } else {
            cerr << "Empty logical statement . . ., exiting . . ." << endl;
            exit(1);
        }
    }
    else if (logic_stmt->opr == OR_OPR || logic_stmt->opr == AND_OPR) {
        t_Expression* left_of_operator_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, logic_stmt->left, em, graph);
        t_Expression* right_of_operator_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, logic_stmt->right, em, graph);

        if (logic_stmt->opr == OR_OPR) {
            expression_to_return = build_OR_constraints(left_of_operator_expr, right_of_operator_expr, 0, 0, em);
        } else if (logic_stmt->opr == AND_OPR) {
            expression_to_return = build_AND_constraints(left_of_operator_expr, right_of_operator_expr, 0, 0, em);
        }
    }
    else if (logic_stmt->opr == NOT_OPR) {
        t_Expression* expr_to_neg = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, logic_stmt->left, em, graph);
        expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expr_to_neg);
    }
    return expression_to_return;
}

t_Expression* GraphManagerNew::get_node_constraint(int nid, node_state_t node_state, t_ExpressionManager* em, GraphNew* graph) {
    t_Expression* expression_to_return = NULL;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};
    string node_variable;
    string repid = graph->get_rep_id_from_nid(nid);
    if (node_state == ACTIVATED_STATE) {
        node_variable = repid + "_active";
    } else if (node_state == INHIBITED_STATE) {
        node_variable = repid + "_active";
    } else if (node_state == OVEREXPRESSED_STATE) {
        node_variable = repid + "_over_expressed";
    } else if (node_state == REPRESSED_STATE) {
        node_variable = repid + "_over_expressed";
    } else {
        cerr << "Node state isn't set, exiting . . ." << endl;
        exit(1);
    }
    if (getExpressionMap.find(node_variable) == getExpressionMap.end()) {
        expression_to_return = em->createSymbol(node_variable, te1);
        getExpressionMap[node_variable] = expression_to_return;
        getCNFIndexMap[expression_to_return] = make_pair(node_variable, -1);
    } else {
        expression_to_return = getExpressionMap[node_variable];
    }

    if (node_state == INHIBITED_STATE || node_state == REPRESSED_STATE) {
        expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
    }
    //em->printExpressionToFileAsDAG("expr_to_return", expression_to_return, cout);
    return expression_to_return;
}

t_Expression* GraphManagerNew::get_node_constraint(int nid, node_state2_t node2_state, t_ExpressionManager* em, GraphNew* graph) {
    t_Expression* expression_to_return = NULL;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};
    string node_variable;
    string repid = graph->get_rep_id_from_nid(nid);
    if (node2_state == DIFFER_EXPRESSED_STATE || NOT_DIFFER_EXPRESSED_STATE) {
        node_variable = repid + "_expression_changes";
    } else {
        cerr << "Node state isn't set, exiting . . ." << endl;
        exit(1);
    }
    if (getExpressionMap.find(node_variable) == getExpressionMap.end()) {
        expression_to_return = em->createSymbol(node_variable, te1);
        getExpressionMap[node_variable] = expression_to_return;
        getCNFIndexMap[expression_to_return] = make_pair(node_variable, -1);
    } else {
        expression_to_return = getExpressionMap[node_variable];
    }

    if (node2_state == NOT_DIFFER_EXPRESSED_STATE)
        expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
    //em->printExpressionToFileAsDAG("expr_to_return", expression_to_return, cout);
    return expression_to_return;
}

t_Expression* GraphManagerNew::get_edge_constraint(int node1, int node2, edge_state_t edge_state, edge_t edge_type, list_edge_subtypes_t* list_edge_subtypes, t_ExpressionManager* em, GraphNew* graph) {
    string repid_node1 = graph->get_rep_id_from_nid(node1);
    string repid_node2 = graph->get_rep_id_from_nid(node2);
    //first a check needs to be performed that the edge actually exists from node1 to node2
    bool edge_exists_from_node1_to_node2 = false;
    int edge_to_consider;
    vector<int> node1_outlist = graph->get_outlist(node1);
    vector<int>::iterator edge_set_itr;
    for (edge_set_itr = node1_outlist.begin(); edge_set_itr != node1_outlist.end(); ++edge_set_itr) {
        if (graph->get_target_node(*edge_set_itr) == node2 && edge_type_subtype_subset_match(*edge_set_itr, edge_type, list_edge_subtypes, graph)) {
            edge_exists_from_node1_to_node2 = true;
            edge_to_consider = *edge_set_itr;
            break;
        }
    }

    if (!edge_exists_from_node1_to_node2) {
        //check if there is an edge from node2 to node1
        bool edge_exists_from_node2_to_node1 = false;
        vector<int> node2_outlist = graph->get_outlist(node2);
        for (edge_set_itr = node2_outlist.begin(); edge_set_itr != node2_outlist.end(); ++edge_set_itr) {
            if (graph->get_target_node(*edge_set_itr) == node1 && edge_type_subtype_subset_match(*edge_set_itr, edge_type, list_edge_subtypes, graph)) {
                edge_exists_from_node2_to_node1 = true;
                break;
            }
        }

        if (edge_exists_from_node2_to_node1) {
            cerr << "Error: there is no edge of desired type from " + repid_node1 + " to " + repid_node2 +
                    ", but there exists edge of desired type from " + repid_node2 + " to " + repid_node1;
        } else {
            cerr << "Error: there is no edge of desired type from " + repid_node1 + " to " + repid_node2 +
                    ", also there is no edge of desired type from " + repid_node2 + " to " + repid_node1;
        }

        //this was earlier exit case
        cerr << "\nExiting. . ." << endl;
        exit(1);
    }



    t_Expression* expression_to_return = NULL;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

    string edge_type_str = graph->get_edge_type(edge_to_consider);
    vector<string> edge_subtype_vec_str = graph->get_edge_subtypes_of_edge(edge_to_consider);
    string edge_subtypes_str = get_edge_substring_for_constraint_variable(edge_subtype_vec_str);

    string edge_variable = "edge." + repid_node1 + "to" + repid_node2 + "." + edge_type_str + "-" + edge_subtypes_str + "present";

    if (getExpressionMap.find(edge_variable) == getExpressionMap.end()) {
        expression_to_return = em->createSymbol(edge_variable, te1);
        getExpressionMap[edge_variable] = expression_to_return;
        getCNFIndexMap[expression_to_return] = make_pair(edge_variable, -1);
    } else {
        expression_to_return = getExpressionMap[edge_variable];
    }

    if (edge_state != PRESENT_STATE) {
        expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
    }

    assert(expression_to_return != NULL);
    return expression_to_return;
}

t_Expression* GraphManagerNew::get_edge_constraint(int node1, int node2, edge_state_t edge_state, edge_t edge_type, DNF_edgesub_t* DNF_edgesub, t_ExpressionManager* em, GraphNew* graph) {
    string repid_node1 = graph->get_rep_id_from_nid(node1);
    string repid_node2 = graph->get_rep_id_from_nid(node2);
    bool report_error = true;
    while (DNF_edgesub) {
        ANDed_edgesub_literals_t* current_ANDed_clause = DNF_edgesub->current_ANDed_clause;
        //first a check needs to be performed that the edge actually exists from node1 to node2
        bool edge_exists_from_node1_to_node2 = false;
        int edge_to_consider;
        vector<int> node1_outlist = graph->get_outlist(node1);
        vector<int>::iterator edge_set_itr;
        for (edge_set_itr = node1_outlist.begin(); edge_set_itr != node1_outlist.end(); ++edge_set_itr) {
            if (graph->get_target_node(*edge_set_itr) == node2 && edge_subtype_ANDed_clause_match(*edge_set_itr, DNF_edgesub->current_ANDed_clause, graph)) {
                edge_exists_from_node1_to_node2 = true;
                edge_to_consider = *edge_set_itr;
                break;
            }
        }
        if (edge_exists_from_node1_to_node2) {//thus the constraint is written for the first macthed clause in the DNF
            report_error = false;
            t_Expression* expression_to_return = NULL;
            TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

            string edge_type_str = graph->get_edge_type(edge_to_consider);
            vector<string> edge_subtype_vec_str = graph->get_edge_subtypes_of_edge(edge_to_consider);
            string edge_subtypes_str = get_edge_substring_for_constraint_variable(edge_subtype_vec_str);

            string edge_variable = "edge." + repid_node1 + "to" + repid_node2 + "." + edge_type_str + "-" + edge_subtypes_str + "present";

            if (getExpressionMap.find(edge_variable) == getExpressionMap.end()) {
                expression_to_return = em->createSymbol(edge_variable, te1);
                getExpressionMap[edge_variable] = expression_to_return;
                getCNFIndexMap[expression_to_return] = make_pair(edge_variable, -1);
            } else {
                expression_to_return = getExpressionMap[edge_variable];
            }

            if (edge_state != PRESENT_STATE) {
                expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
            }

            assert(expression_to_return != NULL);
            return expression_to_return;
        }
        DNF_edgesub = DNF_edgesub->next;
    }
    if (report_error) {
        cerr << "\nNone of the clauses of edgesubtype in DNF macthed, exiting. . ." << endl;
        exit(1);
    }
    return NULL;
}

//match happens when the edge type match exactly and when the edge subtype list of parser is a subset of the edge subtype list of the edge with id eid

bool GraphManagerNew::edge_type_subtype_subset_match(int eid, edge_t edge_type, list_edge_subtypes_t* list_edge_subtypes, GraphNew* graph) {
    string edge_type_str = get_edge_type_str_from_edge_type_parser(edge_type);
    vector<string> edge_subtype_vec_str = get_vector_edge_subtype_str_from_edge_subtype_parser_list(list_edge_subtypes);
    vector<string> eid_all_subtypes_vec_str = graph->get_edge_subtypes_of_edge(eid);
    bool edge_type_match = (edge_type_str == graph->get_edge_type(eid));
    bool edge_subtype_list_subset_match = is_subset(edge_subtype_vec_str, eid_all_subtypes_vec_str);
    return edge_type_match && edge_subtype_list_subset_match;
}

//match happens when all the non-negated literals in the ANDed_clause exist as subtype in edge with id eid and none of the negated literals exist as subtype in edge with id eid

bool GraphManagerNew::edge_subtype_ANDed_clause_match(int eid, ANDed_edgesub_literals_t* ANDed_clause, GraphNew* graph) {
    vector<string> eid_all_subtypes_vec_str = graph->get_edge_subtypes_of_edge(eid);
    vector<string> non_negated_subtypes = get_non_negated_edge_subtypes(ANDed_clause);
    vector<string> negated_subtypes = get_negated_edge_subtypes(ANDed_clause);
    bool non_negated_are_subset = is_subset(non_negated_subtypes, eid_all_subtypes_vec_str);
    bool negated_dont_intersect = intersection_empty(negated_subtypes, eid_all_subtypes_vec_str);
    return non_negated_are_subset && negated_dont_intersect;
}

//for edge subtype string inclusion in edge presence variable

std::string GraphManagerNew::get_edge_substring_for_constraint_variable(std::vector<std::string>& edge_subtype_vec_str) {
    string edge_subtype_str = "";
    for (vector<string>::iterator itr = edge_subtype_vec_str.begin(); itr != edge_subtype_vec_str.end(); ++itr) {
        edge_subtype_str += get_string_prefix(get_string_prefix(*itr, ' '), '/') + "-";
    }
    return edge_subtype_str;
}

string GraphManagerNew::get_string_prefix(string input, char delim) {
    int ind = input.find_first_of(delim);
    if (ind == string::npos)
        return input;
    return input.substr(0, ind);
}

vector<t_Expression *> GraphManagerNew::get_microarray_constraints(string micro_arr_decl, string micro_arr_assert, t_ExpressionManager* em) {
    ifstream fin(micro_arr_decl.c_str());
    ifstream fin1(micro_arr_assert.c_str());
    if (fin.fail() || fin1.fail()) {
        cerr << "Unable to open micro array declaration/definition/assertions/conflicting_nodes file(s) "; // << constraint_file << endl;
    }


    string s;

    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};
    while (fin >> s) {
        if (s[0] != '(')
            continue;

        fin >> s;

        if (getExpressionMap.find(s) == getExpressionMap.end()) {
            cerr << "The variable " + s + " was not declared in the graph constraints\n";
            getExpressionMap[s] = em->createSymbol(s, te);
            getCNFIndexMap[getExpressionMap[s]] = make_pair(s, -1);
        }

        fin >> s;
    }

    vector<t_Expression *> micro_constraints;

    while (fin1 >> s) {
        if (s[0] != '(')
            continue;

        fin1 >> s;

        if (s[0] != '(') {
            s = s.substr(0, s.length() - 1);
            micro_constraints.push_back(getExpressionMap[s]);
        } else {
            fin1 >> s;
            s = s.substr(0, s.length() - 2);
            micro_constraints.push_back(em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, getExpressionMap[s]));
        }
    }

    return micro_constraints;
}

void GraphManagerNew::solve_constraints(vector<t_Expression*> cons, t_ExpressionManager* em) {
    string CNF_file = "CNF_file";
    string z3_out_file = "z3_out_file"; //z3 std output goes here
    string z3_error_file = "z3_error_file"; //z3 std error goes here
    string bin_search_report = "bin_search_report"; //binary search procedure goes here

    ofstream fout_CNF_file(CNF_file.c_str());
    //CNF_file that constraint solver is partitioned into upper part and lower part - upper part is stored in tmp_file, lower part is stored in tmp_file2
    string tmp_file = "temp_file"; //this stores all the constraints but the value of the upper bound
    string tmp_file2 = "temp_file2"; //this stores the upper bound in CNF format
    //int idx = 1, clauses = 0; //clauses is num of clauses, idx stores the number that is to be given to the next variable, thus idx-1 is the total variables


    //em->printExpressionsToFileInCNF(cons, tmp_file, idx, clauses, getCNFIndexMap);//constraints in cons written to the file tmp_file
    t_Expression* AND_cons = build_AND_constraints_from_vector(cons, em);

    ofstream em_log("em_log");
    if (em_log.is_open()) {
        em->printExpressionInCNFefficient(AND_cons, tmp_file, idx, clauses, getCNFIndexMap, em_log);
        em_log.close();
    } else {
        cerr << "Error: em_log file couldn't be opened" << endl;
    }



    vector<int> upper_bound_val; //vector of CNF variables that acts as a bit sequence that stores the upper bound - note the left most element of vector corresponds to the right most bit of the bit sequence
    ofstream fout2(tmp_file2.c_str());
    string cmd = "cat " + tmp_file + " >> " + CNF_file + " ; " + "cat " + tmp_file2 + " >> " + CNF_file; //this command puts everything in the CNF_file to make it ready for the constraint solver
    int upper_bound_cur;
    int upper_bound_user;
    int upper_bound_prev;
    int time_out; //the time in seconds beyond which z3 run is considered timed out
    bool strict_sat_boundaries;
    char choose_strict;
    int highest_unsat_bound_seen;
    sat_state_t sat_state = UNDEFINED;
    sat_state_t sat_state_prev = UNDEFINED;

    ofstream fout3(bin_search_report.c_str());
    ifstream fin;

    bool loop_forver = true;

    while (loop_forver) {
        char choice;
        cout << "\nEnter 'b' to do the binary-search-based constraint solving with metric as upper bound on total number of edges ";
        cout << "\nEnter 'u' to do constraint solving with metric as just a fixed upper bound on total number of edges ";
        cout << "\nEnter 'n' to do constraint solving without any metric ";
        cin >> choice;

        switch (choice) {
                //the binary search case
            case 'b':

                cout << "\nDo you want to find strict SAT boundaries? 'y' or 'Y' for yes, otherwise any other key: ";
                cin >> choose_strict;
                if (choose_strict == 'y' || choose_strict == 'Y') {
                    strict_sat_boundaries = true;
                }



                loop_forver = false;
                upper_bound_val = write_edgecount_cnf(tmp_file, getCNFIndexMap, idx, clauses);

                cout << "\nEnter the time to wait for each run(in seconds) : ";
                cin >> time_out;

                upper_bound_cur = 1;
                upper_bound_prev = 0;
                highest_unsat_bound_seen = 0;

                // increasing the upper_bound by 2 times for each run in the loop
                while (true) {
                    //for(int i=0; i <= upper_bound_val.size(); i++) {

                    /*if(i==upper_bound_val.size())
                            upper_bound_cur--;*/

                    if (fout_CNF_file.is_open()) {
                        fout_CNF_file.close();
                    }
                    if (fout2.is_open()) {
                        fout2.close();
                    }

                    fout_CNF_file.open(CNF_file.c_str());
                    fout2.open(tmp_file2.c_str());
                    fout2 << "c edge var sum upper bound starts" << endl;

                    fout_CNF_file << "p cnf " << idx - 1 << " " << clauses << endl;

                    upper_bound_user = upper_bound_cur;
                    for (int i = 0; i < upper_bound_val.size(); i++) {
                        if (upper_bound_user % 2 == 0)
                            fout2 << -upper_bound_val[i] << " 0\n";
                        else
                            fout2 << upper_bound_val[i] << " 0\n";
                        upper_bound_user /= 2;
                    }
                    fout_CNF_file.close();
                    fout2.close();
                    system((cmd + "; timeout " + IntToString(time_out) + " z3 -dimacs " + CNF_file + " > " + z3_out_file + " 2> " + z3_error_file).c_str());
                    //sleep(time_out+3);

                    fout3 << "\nUpper bound : " << upper_bound_cur << endl;

                    string s;
                    if (fin.is_open()) {
                        fin.close();
                    }
                    fin.open(z3_error_file.c_str());

                    if (fin.is_open()) {
                        fin >> s;
                        if (s.length() > 0) {
                            fout3 << "Error in cnf input file format";
                            fin.close();
                            break;
                        }
                    }

                    if (fin.is_open()) {
                        fin.close();
                    }
                    fin.open(z3_out_file.c_str());

                    if (fin.is_open()) {
                        fin >> s;
                        fin.close();
                    }

                    if (s == "sat") {
                        sat_state = SAT;
                        fout3 << s << endl;
                        //break;
                    } else if (s == "unsat") {
                        sat_state = UNSAT;
                        fout3 << s << endl;
                    } else {
                        sat_state = TIMEOUT;
                        fout3 << "timed out\n";
                    }

                    //testing starts
                    //system(("cp " + tmp_file + " " + tmp_file + "_" + IntToString(upper_bound_cur) + " ; " + "cp " + tmp_file2 + " " + tmp_file2 + "_" + IntToString(upper_bound_cur)).c_str());
                    //testing ends


                    //testing
                    fout3 << "current upper bound " << upper_bound_cur << endl;
                    fout3 << "previous upper bound " << upper_bound_prev << endl;
                    fout3 << "highest unsat bound " << highest_unsat_bound_seen << endl;


                    //there is an assumption here -
                    //bw UNSAT and TIMEOUT there is a continuous UNSAT followed by continuous TIMOUT
                    //bw UNSAT and SAT there is a continuous UNSAT followed by continuous SAT
                    //other cases must be handled later to this
                    if (strict_sat_boundaries) {
                        if (sat_state == SAT || sat_state == TIMEOUT) {
                            if ((upper_bound_cur - highest_unsat_bound_seen) == 1) {
                                break;
                            } else {
                                sat_state_prev = sat_state;
                                sat_state = UNDEFINED;
                                int temp = upper_bound_cur;
                                upper_bound_cur = (highest_unsat_bound_seen + upper_bound_cur) / 2;
                                upper_bound_prev = temp;
                            }
                        } else if (sat_state == UNSAT) {
                            if (sat_state_prev == UNSAT || sat_state_prev == UNDEFINED) {
                                sat_state_prev = UNSAT;
                                sat_state = UNDEFINED;
                                highest_unsat_bound_seen = upper_bound_cur;
                                upper_bound_prev = upper_bound_cur;
                                upper_bound_cur = 2 * upper_bound_prev;
                            } else if (sat_state_prev == TIMEOUT || sat_state_prev == SAT) {
                                sat_state_prev = UNSAT;
                                sat_state = UNDEFINED;
                                if (upper_bound_prev - upper_bound_cur == 1) {
                                    highest_unsat_bound_seen = upper_bound_cur;
                                    upper_bound_prev = upper_bound_cur;
                                    upper_bound_cur++;
                                } else {
                                    if (highest_unsat_bound_seen < upper_bound_cur) {
                                        highest_unsat_bound_seen = upper_bound_cur;
                                    }
                                    int temp = upper_bound_cur;
                                    upper_bound_cur = (upper_bound_prev + upper_bound_cur) / 2;
                                    upper_bound_prev = temp;
                                }
                            } else {
                                assert(false);
                            }
                        } else {
                            assert(false);
                        }
                    } else {
                        if (sat_state == SAT) {
                            break;
                        } else if (sat_state == UNSAT || sat_state == TIMEOUT) {
                            sat_state_prev = sat_state;
                            sat_state = UNDEFINED;
                            upper_bound_prev = upper_bound_cur;
                            highest_unsat_bound_seen = upper_bound_cur;
                            upper_bound_cur *= 2;
                        } else {
                            cerr << "Error: sat state must be one of SAT, UNSAT, TIMEOUT at this stage" << endl;
                            assert(false);
                        }
                    }
                }

                /*
                fout << "p cnf " <<idx-1<<" "<<clauses<<endl;

                for(int i=0;i<upper_bound_val.size();i++) {
                        if(upper_bound_user%2==0)
                                fout2<<-upper_bound_val[i]<<" 0\n";
                        else
                                fout2<<upper_bound_val[i]<<" 0\n";
                        upper_bound_user/=2;
                }
                 */

                break;
            case 'u':
                //a given upperbound case
                loop_forver = false;
                upper_bound_val = write_edgecount_cnf(tmp_file, getCNFIndexMap, idx, clauses);

                cout << "\nEnter upper bound ";
                cin >> upper_bound_user;

                fout_CNF_file << "p cnf " << idx - 1 << " " << clauses << endl; //the format is p cnf <number of variables in CNF> <number of clauses in CNF>

                for (int i = 0; i < upper_bound_val.size(); i++) {
                    if (upper_bound_user % 2 == 0)
                        fout2 << -upper_bound_val[i] << " 0\n";
                    else
                        fout2 << upper_bound_val[i] << " 0\n";
                    upper_bound_user /= 2;
                }
                fout_CNF_file.close();
                fout2.close();
                system(cmd.c_str());

                break;
                //no metric case
            case 'n':
                loop_forver = false;
                fout_CNF_file << "p cnf " << idx - 1 << " " << clauses << endl; //the format is p cnf <number of variables in CNF> <number of clauses in CNF>
                fout_CNF_file.close();
                system(cmd.c_str());
                //cout << "Not implemented yet" << endl;
                break;
            default:
                cout << "\nError: invalid choice ";
                break;
        }
    }
}

//appends edge upper bound constraints to the file CNF_filename (w/o the upper bound value)
//returns vector of internal variables storing the actual upper bound

vector<int> GraphManagerNew::write_edgecount_cnf(string CNF_filename, map<t_Expression *, pair<string, int> > getCNFIndexMap, int& idx, int& clauses) {
    vector<vector<int> > add_constraints; //stores all internal variables that must be added
    ofstream fout_CNF_file(CNF_filename.c_str(), ofstream::out | ofstream::app);
    fout_CNF_file << "c edge variables sum constraints start" << endl;

    ofstream fout_mapping("mapping", ofstream::out); //string variable name to internal CNF variable number written to file mapping

    for (map<t_Expression *, pair<string, int> >::iterator it = getCNFIndexMap.begin(); it != getCNFIndexMap.end(); it++) {
        string var_name = (it->second).first;
        if (var_name.substr(0, 4) == "edge") {
            if ((it->second).second == -1)//if the internal variables hasn't been assigned so far assign it
                (it->second).second = idx++;

            vector<int> v;
            v.push_back((it->second).second);
            add_constraints.push_back(v);
        }
        fout_mapping << (it->second).first << " " << (it->second).second << endl;
    }

    if (add_constraints.empty()) {
        cerr << "Error: there are no edge variables to add - exiting..." << endl;
        exit(1);
    }

    while (add_constraints.size() > 1) {
        vector<vector<int> > new_cons;

        for (int i = 0; i < add_constraints.size(); i += 2) {
            if ((i + 1) < add_constraints.size()) {
                new_cons.push_back(add_cnf(add_constraints[i], add_constraints[i + 1], idx, clauses, fout_CNF_file));
            } else {
                // should append zero here
                fout_CNF_file << -idx << " 0\n";
                clauses++;
                add_constraints[i].push_back(idx);
                idx++;

                new_cons.push_back(add_constraints[i]);
            }
        }
        add_constraints = new_cons;
    }

    vector<int> a = add_constraints[0];
    vector<int> b;

    for (int i = 0; i < a.size(); i++) {
        b.push_back(idx++);
        //cout<<b[i]<<" "<<a[i]<<endl;
    }

    vector<int> temp;
    // checking if a < b

    for (int i = a.size() - 1; i >= 0; i--) {
        if (i == 0) {
            fout_CNF_file << b[i];
            for (int j = 0; j < temp.size(); j++)
                fout_CNF_file << " " << temp[j];
            fout_CNF_file << " 0\n";
            clauses++;

            fout_CNF_file << -a[i];
            for (int j = 0; j < temp.size(); j++)
                fout_CNF_file << " " << temp[j];
            fout_CNF_file << " 0\n";
            clauses++;
        } else {
            fout_CNF_file << b[i];
            for (int j = 0; j < temp.size(); j++)
                fout_CNF_file << " " << temp[j];
            fout_CNF_file << " " << -(idx) << " 0\n";
            clauses++;

            fout_CNF_file << -a[i];
            for (int j = 0; j < temp.size(); j++)
                fout_CNF_file << " " << temp[j];
            fout_CNF_file << " " << (-idx) << " 0\n";
            clauses++;

            temp.push_back(idx);
            idx++;

            fout_CNF_file << -b[i] << " " << a[i];
            for (int j = 0; j < temp.size(); j++)
                fout_CNF_file << " " << temp[j];
            fout_CNF_file << " 0\n";
            clauses++;

            fout_CNF_file << -a[i] << " " << b[i];
            for (int j = 0; j < temp.size(); j++)
                fout_CNF_file << " " << temp[j];
            fout_CNF_file << " 0\n";
            clauses++;
        }

    }


    ofstream fout4("ind_var", ofstream::out);

    map<int, int> ind_vars;

    for (map<t_Expression *, pair<string, int> >::iterator it = getCNFIndexMap.begin(); it != getCNFIndexMap.end(); it++) {
        if ((it->second).second != -1) {
            fout4 << (it->second).second << endl;
            ind_vars[(it->second).second] = 1;
        }
    }

    ofstream fout5("dep_var", ofstream::out);

    for (int i = 1; i < idx; i++) {
        if (ind_vars.find(i) == ind_vars.end())
            fout5 << i << endl;
    }


    clauses += b.size();
    /*	for(int i=0;i<b.size();i++) {
            if(const_num%2==1)
                    fout<<b[i]<<" 0\n";
            else
                    fout<<-b[i]<<" 0\n";
            clauses++;
            const_num/=2;
    }
     */
    // we are just returning the constant number now which can be later set to any value
    return b;
}

GraphNew* GraphManagerNew::get_subgraph_with_edges_retained(GraphNew* graph, std::string filename) {
    if (graph == NULL) {
        cerr << "Error: null parent graph" << endl;
        return NULL;
    }
    GraphNew* subgraph = NULL;
    ifstream fin(filename.c_str());
    if (fin.is_open()) {
        subgraph = new GraphNew;
        int new_gid = ++GraphManagerNew::graph_id_count;
        subgraph->set_graph_id(new_gid);
        add_graph(new_gid, subgraph);
        string line;

        while (getline(fin, line)) {
            string name, value;
            stringstream ss(line);
            ss >> name >> value;
            if (value == "true") {
                string head(name.c_str(), 0, 4);
                if (head == "edge") {
                    list<string> *subparts = this->get_list_of_names_from_string(name, ".");
                    subparts->pop_front(); // remove 'edge' prefix
                    string end_names = subparts->front(); //source to target
                    subparts->pop_front();
                    string edge_info = subparts->front(); //type subtype

                    //get source and target rep ids
                    list<string> *end_nodes = this->get_list_of_names_from_string(end_names, "to");
                    string source_rep_id = end_nodes->front();
                    end_nodes->pop_front();
                    string target_rep_id = end_nodes->front();
                    end_nodes->pop_front();

                    //get type info
                    list<string>* edgeInfo = this->get_list_of_names_from_string(edge_info, "-");
                    string edgeType = edgeInfo->front(); //get edge type
                    edgeInfo->pop_front(); // Remove the 'edge type' from list of strings
                    edgeInfo->pop_back(); // Remove 'present' from the list of strings

                    //edgeInfo now has only subtypes

                    //go over the edges of the old graph and if edge matches then include edge in the subgraph
                    vector<int> parent_graph_EIDS = graph->get_edge_ids();
                    for (vector<int>::iterator edge_itr = parent_graph_EIDS.begin(); edge_itr != parent_graph_EIDS.end(); ++edge_itr) {
                        string parent_edge_type = graph->get_edge_type(*edge_itr);
                        vector<string> parent_edge_subtypes = graph->get_edge_subtypes_of_edge(*edge_itr);
                        sort(parent_edge_subtypes.begin(), parent_edge_subtypes.end());
                        vector<string> edgeSubtypes(edgeInfo->begin(), edgeInfo->end());
                        sort(edgeSubtypes.begin(), edgeSubtypes.end());
                        int parent_source_nid = graph->get_source_node(*edge_itr);
                        int parent_target_nid = graph->get_target_node(*edge_itr);
                        string parent_source_rid = graph->get_rep_id_from_nid(parent_source_nid);
                        string parent_target_rid = graph->get_rep_id_from_nid(parent_target_nid);
                        if (parent_source_rid == source_rep_id && parent_target_rid == target_rep_id && parent_edge_type == edgeType && edgeSubtypes == parent_edge_subtypes) {
                            duplicate_edge(subgraph, graph, *edge_itr);
                            //if edge is assoc/dissoc type then make an edge in oppoiste direction also
                            if (graph->edge_has_subtype(*edge_itr, "association") || graph->edge_has_subtype(*edge_itr, "dissociation")) {
                                vector<int> parent_target_out_list = graph->get_outlist(parent_target_nid);

                                for (vector<int>::iterator parent_target_out_list_itr = parent_target_out_list.begin(); parent_target_out_list_itr != parent_target_out_list.end(); parent_target_out_list_itr++) {
                                    int target_of_target_id = graph->get_target_node(*parent_target_out_list_itr);
                                    string target_out_edge_type = graph->get_edge_type(*parent_target_out_list_itr);
                                    vector<string> target_out_edge_subtypes = graph->get_edge_subtypes_of_edge(*parent_target_out_list_itr);
                                    sort(target_out_edge_subtypes.begin(), target_out_edge_subtypes.end());

                                    if (parent_source_nid == target_of_target_id && parent_edge_type == target_out_edge_type && parent_edge_subtypes == target_out_edge_subtypes) {
                                        duplicate_edge(subgraph, graph, *parent_target_out_list_itr);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        //store genesis
        list<int> empty_list;
        empty_list.push_back(graph->get_graph_id());
        string oper = "subgraph from assignment file: " + filename;
        add_graph_genesis(new_gid, oper, empty_list);
        fin.close();
    } else {
        cerr << "Error: couldn't open file: " + filename << endl;
    }
    return subgraph;
}



list<string>* GraphManagerNew::get_list_of_names_from_string(string name_str, string delim) {
    list<string> *list_of_labels = new list<string>;
    unsigned start_pos = 0;
    while (start_pos < name_str.length()) {
        std::size_t found = name_str.find(delim, start_pos);
        if (found != std::string::npos) {
            (*list_of_labels).push_back(name_str.substr(start_pos, found - start_pos));
            start_pos = found + delim.length();
        } else {
            (*list_of_labels).push_back(name_str.substr(start_pos));
            break;
        }
    }
    return list_of_labels;
}

void GraphManagerNew::merge_vis_files(string file_assign1, string file_assign2, std::ofstream& fout) {
    ifstream fin1(file_assign1.c_str());
    ifstream fin2(file_assign2.c_str());

    set<string> visible_edges1;
    set<string> present_edges1;
    set<string> visible_edges2;
    set<string> present_edges2;
    set<string> visible_minus_present_edges1;
    set<string> visible_minus_present_edges2;

    set<string> present_minus_visible_edges1;
    set<string> present_minus_visible_edges2;
    string visible_str = "-visible";
    string present_str = "-present";
    if (fin1.is_open()) {
        string var, boolval;
        while (fin1 >> var) {
            fin1 >> boolval;
            if (boolval == "true") {
                string::size_type visible_index = var.find(visible_str);
                string::size_type present_index = var.find(present_str);
                if (visible_index != string::npos) {
                    var.erase(visible_index, visible_str.length());
                    visible_edges1.insert(var);
                }
                if (present_index != string::npos) {
                    var.erase(present_index, present_str.length());
                    present_edges1.insert(var);
                }
            }
        }
        fin1.close();
        //find visible edges that are not present
        set_difference(visible_edges1.begin(), visible_edges1.end(), present_edges1.begin(), present_edges1.end(), inserter(visible_minus_present_edges1, visible_minus_present_edges1.begin()));
        set_difference(present_edges1.begin(), present_edges1.end(), visible_edges1.begin(), visible_edges1.end(), inserter(present_minus_visible_edges1, present_minus_visible_edges1.begin()));


    } else {
        cerr << "Error: file " + file_assign1 + " could not be opened for reading assignments" << endl;
    }

    if (fin2.is_open()) {
        string var, boolval;
        while (fin2 >> var) {
            fin2 >> boolval;
            if (boolval == "true") {
                string::size_type visible_index = var.find(visible_str);
                string::size_type present_index = var.find(present_str);
                if (visible_index != string::npos) {
                    var.erase(visible_index, visible_str.length());
                    visible_edges2.insert(var);
                }
                if (present_index != string::npos) {
                    var.erase(present_index, present_str.length());
                    present_edges2.insert(var);
                }
            }
        }
        fin2.close();
        //find visible edges that are not present
        set_difference(visible_edges2.begin(), visible_edges2.end(), present_edges2.begin(), present_edges2.end(), inserter(visible_minus_present_edges2, visible_minus_present_edges2.begin()));
        set_difference(present_edges2.begin(), present_edges2.end(), visible_edges2.begin(), visible_edges2.end(), inserter(present_minus_visible_edges2, present_minus_visible_edges2.begin()));


    } else {
        cerr << "Error: file " + file_assign2 + " could not be opened for reading assignments" << endl;
    }

    static set<string> visible_edges_merged;
    //set_union(visible_edges1.begin(), visible_edges1.begin(), visible_edges2.begin(), visible_edges2.end(), inserter(visible_edges_merged, visible_edges_merged.begin()));
    set_union(visible_minus_present_edges1.begin(), visible_minus_present_edges1.begin(), visible_minus_present_edges2.begin(), visible_minus_present_edges2.end(), inserter(visible_edges_merged, visible_edges_merged.begin()));
    static set<string> present_edges_merged;
    set_union(present_edges1.begin(), present_edges1.end(), present_edges2.begin(), present_edges2.end(), inserter(present_edges_merged, present_edges_merged.begin()));
    //set_union(present_minus_visible_edges1.begin(), present_minus_visible_edges1.end(), present_minus_visible_edges2.begin(), present_minus_visible_edges2.end(), inserter(present_edges_merged, present_edges_merged.begin()));

    set<string> visible_minus_present_merged; // = visible_edges_merged;
    set_difference(visible_edges_merged.begin(), visible_edges_merged.end(), present_edges_merged.begin(), present_edges_merged.end(), inserter(visible_minus_present_merged, visible_minus_present_merged.begin()));

    for (set<string>::iterator set_itr = visible_minus_present_merged.begin(); set_itr != visible_minus_present_merged.end(); set_itr++) {
        fout << *set_itr << endl;
    }
}

void GraphManagerNew::print_visible_but_not_present_edges_from_z3_assignment(string file_assgn, std::ofstream& fout) {
    ifstream fin(file_assgn.c_str());
    set<string> visible_edges;
    set<string> present_edges;
    set<string> visible_minus_present_edges;
    string visible_str = "-visible";
    string present_str = "-present";
    if (fin.is_open()) {
        string var, boolval;
        while (fin >> var) {
            fin >> boolval;
            if (boolval == "true") {
                string::size_type visible_index = var.find(visible_str);
                string::size_type present_index = var.find(present_str);
                if (visible_index != string::npos) {
                    var.erase(visible_index, visible_str.length());
                    visible_edges.insert(var);
                }
                if (present_index != string::npos) {
                    var.erase(present_index, present_str.length());
                    present_edges.insert(var);
                }
            }
        }
        fin.close();
        //find visible edges that are not present
        set_difference(visible_edges.begin(), visible_edges.end(), present_edges.begin(), present_edges.end(), inserter(visible_minus_present_edges, visible_minus_present_edges.begin()));

        for (set<string>::iterator set_itr = visible_minus_present_edges.begin(); set_itr != visible_minus_present_edges.end(); set_itr++) {
            fout << *set_itr << endl;
        }
    } else {
        cerr << "Error: file " + file_assgn + " couldn't be opened for reading assignments" << endl;
    }
}

void GraphManagerNew::testing_printexpr_iter(t_ExpressionManager* em) {
    int idx = 1, clauses = 0;
    map<t_Expression*, pair<string, int> > getCNFIndexMap2;
    TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};
    t_Expression* expr;
    string left = "left", right = "right", center = "center";

    t_Expression* left_expr = em->createSymbol(left, te1);
    getCNFIndexMap2[left_expr] = make_pair(left, -1);

    t_Expression* right_expr = em->createSymbol(right, te1);
    getCNFIndexMap2[right_expr] = make_pair(right, -1);

    t_Expression* center_expr = em->createSymbol(center, te1);
    getCNFIndexMap2[center_expr] = make_pair(center, -1);

    t_Expression* expr1 = build_AND_constraints(left_expr, center_expr, 0, 0, em);
    t_Expression* expr2 = build_AND_constraints(right_expr, center_expr, 0, 0, em);

    expr = build_AND_constraints(expr1, expr2, 0, 0, em);

    //expr = left_expr;

    string cnf_test = "print_cnf_test";
    ofstream em_log_test("em_log_test");

    em->printExpressionInCNFefficient(expr, cnf_test, idx, clauses, getCNFIndexMap2, em_log_test);
    em->print_expr_var_cnf_id(getCNFIndexMap2);
    em_log_test << " idx is " << idx << endl;
    em_log_test << "clauses is " << clauses << endl;
    em_log_test.close();
}
