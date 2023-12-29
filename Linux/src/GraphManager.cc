#include <cassert>
#include <map>
#include <cmath>
#include <set>
#include <cstdlib>
#include <queue>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "../include/Oprs.h"
#include "../include/macros.h"
#include "../include/GraphManager.h"
#include "../include/XMLParser.h"
// #include "../include/template_datatypes.h"
#include "../include/rules_parser_datatypes.h"
// #include "../include/template_scanner.tab.h"
#include "../include/rules_parser.tab.h"
#include "../include/rule_tuple_store.h"

// #include "../ExpressionManager_Based_Implementation/ExpressionManager/ExpressionManager.h"
// #include "../ExpressionManager_Based_Implementation/HashTableManager/HashTable_WithStandardKeys.h"
// #include "ExpressionManager.h"
// #include "HashTable_WithStandardKeys.h"

// #include "NewBV.cc"

#define min(x, y) (x < y) ? x : y

using namespace std;

// these functions and vars needed for Expression Manager
extern string IntToString(int num);
extern map<string, t_Expression *> getExpressionMap;
extern map<t_Expression *, pair<string, int>> getCNFIndexMap;

extern vector<int> add_cnf(vector<int> u, vector<int> v, int &idx, int &clauses, ofstream &fout);
extern string buildRelationalExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
extern string buildArithmaticExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
extern string buildSelectExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
extern string buildConcatExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
extern string buildBracketExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
extern string buildVariableExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);

extern t_Expression *build_OR_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em);
extern t_Expression *build_AND_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em);
extern t_Expression *build_OR_constraints(t_Expression *e1, t_Expression *e2, int a1, int a2, t_ExpressionManager *em);
extern t_Expression *build_AND_constraints(t_Expression *e1, t_Expression *e2, int a1, int a2, t_ExpressionManager *em);

// for being able to print map
template <typename Key, typename Val>
extern void print_map(map<Key, Val> &map2print);

GraphManager::GraphManager()
{

	bvm = new BVManager();
	nodeTypeEncodingMap = getNodeTypeEncodingMap();
	edgeTypeEncodingMap = getEdgeTypeEncodingMap();
	edgeSubtypeEncodingMap = getEdgeSubtypeEncodingMap();
	pathwayEncodingMap = getPathwayEncodingMap();

	// t_ConfigurationOptions config_options("config.txt");
	// em = new t_ExpressionManager(config_options);
}

GraphManager::~GraphManager()
{
}

map<string, int> GraphManager::getNodeTypeEncodingMap()
{
	string node_type_encoding_file;
	//	cout << "Enter node type encoding filename: ";
	//	cin >> node_type_encoding_file;
	string supp_files_dir = SUPPORTING_FILES_DIR;
	node_type_encoding_file = supp_files_dir + "node_type.txt";
	// cout << "Node type encoding filename: " << node_type_encoding_file << endl;
	string delim = DELIMITER_STRING_FOR_TYPE_AND_SUBTYPE_ENCODINGS;
	return getMapFromFile(node_type_encoding_file, delim);
}

map<string, int> GraphManager::getEdgeTypeEncodingMap()
{
	string edge_type_encoding_file;
	//	cout << "Enter edge type encoding filename: ";
	//	cin >> edge_type_encoding_file;
	string supp_files_dir = SUPPORTING_FILES_DIR;
	edge_type_encoding_file = supp_files_dir + "edge_type.txt";
	// cout << "Edge type encoding filename: " << edge_type_encoding_file << endl;
	string delim = DELIMITER_STRING_FOR_TYPE_AND_SUBTYPE_ENCODINGS;
	return getMapFromFile(edge_type_encoding_file, delim);
}

map<string, int> GraphManager::getEdgeSubtypeEncodingMap()
{
	string edge_subtype_encoding_file;
	//	cout << "Enter edge subtype encoding filename: ";
	//	cin >> edge_subtype_encoding_file;
	string supp_files_dir = SUPPORTING_FILES_DIR;
	edge_subtype_encoding_file = supp_files_dir + "edge_subtype.txt";
	// cout << "Edge subtype encoding filename: " << edge_subtype_encoding_file << endl;
	string delim = DELIMITER_STRING_FOR_TYPE_AND_SUBTYPE_ENCODINGS;
	return getMapFromFile(edge_subtype_encoding_file, delim);
}

map<string, int> GraphManager::getPathwayEncodingMap()
{
	string pathway_encoding_file;
	//	cout << "Enter pathway encoding filename: ";
	//	cin >> pathway_encoding_file;
	string supp_files_dir = SUPPORTING_FILES_DIR;
	pathway_encoding_file = supp_files_dir + "all_paths.txt";
	// pathway_encoding_file = supp_files_dir + "all_paths.txt";
	// cout << "Pathway encoding filename: " << pathway_encoding_file << endl;
	string delim = DELIMITER_STRING_FOR_PATHWAY_ENCODINGS;
	return getMapFromFile(pathway_encoding_file, delim);
}

map<string, int> GraphManager::getMapFromFile(string filename, string delim)
{

	ifstream myfile(filename.c_str());
	string line;
	list<string> tokens, *pTokens;
	map<string, int> content;

	while (getline(myfile, line))
	{
		pTokens = this->get_list_of_names_from_string(line, delim);
		tokens = *pTokens;
		if (tokens.size() > 2)
		{
			cerr << "Invalid file format\n";
		}
		string first_elem, second_elem;
		first_elem = tokens.front();
		tokens.pop_front();
		second_elem = tokens.front();
		content.insert(pair<string, int>(first_elem, atoi(second_elem.c_str())));
		delete pTokens;
	}
	return content;
}

string GraphManager::toString(int i)
{
	stringstream ss;
	ss << i;
	string str = ss.str();
	return str;
}

bool GraphManager::invalid_string(string str)
{
	int i = 0;
	while ((i < str.length()) && (std::isalnum(str[i])))
	{
		++i;
	}
	return (i != str.length());
}

Graph *GraphManager::register_new_graph(map<string, Node *> &node_map, string graph_genesis_operation, list<unsigned int> operand_graphs, string other_params)
{
	Graph *new_graph = new Graph(node_map, graph_genesis_operation, operand_graphs, other_params);
	if (new_graph == NULL)
	{
		cerr << "Error in GraphManager::register_new_graph. Graph is empty\n";
		return NULL;
	}

	// graph_list.push_back(new_graph);
	graph_id_to_graph_map.insert(pair<unsigned int, Graph *>((*new_graph).graph_id, new_graph));

	// since the node id map might have changed
	// we need to update rep-id s in all graphs in memory
	/*
	map<unsigned int, Graph *>::iterator map_it;
	map<string, Node *>::iterator graph_it;
	vector<string> to_remove;
	vector<pair<string, Node*> > to_add;

	for (map_it = graph_list.begin(); map_it != graph_list.end(); map_it++) {
		//map<string, Node*> curr_graph = map_it->second->map_of_nodes;

		for (graph_it = map_it->second->map_of_nodes.begin(); graph_it != map_it->second->map_of_nodes.end(); graph_it++){

			string local1 = graph_it->first;
			string local2 = graph_it->second->rep_id_name;
			if (local1 != local2) {
				cout << local1 << " mismatch in register " << local2 << endl;
			}

			string rep_id = get_rep_id_name_iteratively(local2);

			if (local1 != rep_id || local2 != rep_id || local1 != local2) {
				cout << "type: " << graph_it->second->node_type << endl;
				cout << "first: " << local1 << " " << " second->rep_id: " << local2 << " repid from map for local 2 " << rep_id << endl;
				cout << "------------" << endl;


				pair<string, Node*> temp = *graph_it;
				temp.first = rep_id;
				temp.second->rep_id_name = rep_id;
				to_remove.push_back(local1);
				to_add.push_back(temp);
			}
		}
		cout << "camee out of inner loop" << endl;

		for(vector<string>::iterator itr = to_remove.begin(); itr != to_remove.end(); ++itr){
			map_it->second->map_of_nodes.erase(*itr);
		}

		for(vector<pair<string, Node*> >::iterator itr = to_add.begin(); itr != to_add.end(); ++itr){
			map_it->second->map_of_nodes.insert(*itr);
		}
	}
	*/

	return new_graph;
	// bool success = add_new_graph(node_map);
	// graph_genesis.insert(pair<string, graph_summary>(new_graph, summary));
}

int GraphManager::get_graph_list_size()
{
	return graph_id_to_graph_map.size();
}

Graph *GraphManager::get_latest_graph()
{
	return graph_id_to_graph_map.end()->second;
}

Graph *GraphManager::get_graph_from_index(int index)
{
	if (index < 0 || index > (graph_id_to_graph_map.size()))
	{
		return NULL;
	}

	return graph_id_to_graph_map[index];
}

Graph *GraphManager::get_subgraph_with_edges_removed(Graph *graph, string filename)
{

	// Graph* create_graph_edge_removed(Graph* input_graph, std::set<Edge*>& edges_to_remove);

	set<Edge *> all_edges = get_all_edges_from_node_map(graph->rep_id_to_node_map);
	set<Edge *> edges_to_remove;
	set<Edge *>::iterator it;

	ifstream myfile(filename.c_str());
	string delim = " ";
	string line;
	list<string> tokens, *pTokens;

	map<int, Node *> node_id_map;
	set<bvatom> edge_map;

	while (getline(myfile, line))
	{
		pTokens = this->get_list_of_names_from_string(line, delim);
		tokens = *pTokens;

		// Get source and target node ids

		string source_name = tokens.front();
		tokens.pop_front();
		string target_name = tokens.front();
		tokens.pop_front();
		string value = tokens.front();

		if (value != "true")
		{
			Node *source_node = get_node_by_name(graph, source_name);
			Node *target_node = get_node_by_name(graph, target_name);
			if ((source_node != NULL) && (target_node != NULL))
			{

				for (it = all_edges.begin(); it != all_edges.end(); it++)
				{
					if ((bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "source_node_id"), get_bvatom_for_node_id(source_node))) && (bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "target_node_id"), get_bvatom_for_node_id(target_node))))
					{
						edges_to_remove.insert((*it));
					}
				}
			}
		}
	}

	string graph_genesis_label = "Removed edges from Graph";
	list<unsigned int> oper_graphs_list;
	oper_graphs_list.push_back(graph->graph_id);
	string oper_parameters = "that are not True in processed Z3 output " + filename;
	map<string, Node *> new_graph_map = create_graph_edge_removed(graph, edges_to_remove)->rep_id_to_node_map;
	Graph *new_graph = register_new_graph(new_graph_map, graph_genesis_label, oper_graphs_list, oper_parameters);

	if (new_graph->rep_id_to_node_map.empty())
	{
		cout << "Error in GraphManager::get_subgraph_from_edges : Could not create graph" << endl;
	}
	return new_graph;
}

/*
// Following method retains edges in a graph based on source and target node names
Graph* GraphManager::get_subgraph_with_edges_retained(Graph * graph, string filename){

	set<Edge *> all_edges = get_all_edges_from_node_map(graph->map_of_nodes);
	set<Edge *> edges_to_retain;
	set<Edge *>::iterator it;

	ifstream myfile(filename.c_str());
	string delim = " ";
	string line;
	list<string> tokens, *pTokens;

	map<int, Node*> node_id_map;
	set<bvatom> edge_map;

	while (getline(myfile, line)) {
		pTokens = this->get_list_of_names_from_string(line, delim);
		tokens = *pTokens;

		// Get source and target node ids

		string source_name = tokens.front();
		tokens.pop_front();
		string target_name = tokens.front();
		tokens.pop_front();
		string value = tokens.front();

		if (value == "true") {
			Node * source_node = get_node_by_name(graph, source_name);
			Node * target_node = get_node_by_name(graph, target_name);
			if ( (source_node != NULL) && (target_node != NULL ) ) {
				for (it = all_edges.begin(); it != all_edges.end(); it++){
					if ( (bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "source_node_id"), get_bvatom_for_node_id(source_node)) )
							&& (bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "target_node_id"), get_bvatom_for_node_id(target_node)) ) ) {
						edges_to_retain.insert((*it));
					}
				}
			}
		}
	}

	string graph_genesis_label = "Retained edges from Graph";
	list<unsigned int> oper_graphs_list;
	oper_graphs_list.push_back(graph->graph_id);
	string oper_parameters = "that are True in processed Z3 output edge assignments " + filename;
	map<string, Node *>new_graph_map = create_graph_edge_retained(graph, edges_to_retain)->map_of_nodes;
	Graph * new_graph = register_new_graph(new_graph_map, graph_genesis_label, oper_graphs_list, oper_parameters);

	if (new_graph->map_of_nodes.empty()) {
		cout << "Error in GraphManager::get_subgraph_from_edges : Could not create graph" << endl;
	}
	return new_graph;
}*/

Graph *GraphManager::get_subgraph_with_edges_retained(Graph *graph, string filename)
{
	set<Edge *> all_edges = get_all_edges_from_node_map(graph->rep_id_to_node_map);
	set<Edge *> edges_to_retain;
	set<Edge *>::iterator it;

	ifstream myfile(filename.c_str());
	string delim = " ";
	string line;
	list<string> tokens, *pTokens;

	map<int, Node *> node_id_map;
	set<bvatom> edge_map;

	while (getline(myfile, line))
	{
		pTokens = this->get_list_of_names_from_string(line, delim);
		tokens = *pTokens;

		// Get source and target node ids

		string name = tokens.front();
		tokens.pop_front();
		string value = tokens.front();

		if (value == "true")
		{
			// --- added by sukanya on 21 Mar 2015

			// ignore if not an edge
			string head(name.c_str(), 0, 4);
			if (head == "edge")
			{
				string edge_name = name;
				// ---

				// processing info of the edge
				// cout << edge_name << endl;
				list<string> subparts, *pSubparts;
				pSubparts = this->get_list_of_names_from_string(edge_name, ".");
				subparts = *pSubparts;

				subparts.pop_front(); // remove 'edge' prefix
				string end_names = subparts.front();
				subparts.pop_front();
				string edge_info = subparts.front();
				// cout << "End nodes: " << end_names << endl;
				// cout << "Edge info: " << edge_info << endl;

				list<string> end_nodes, *pEnd_nodes;
				pEnd_nodes = this->get_list_of_names_from_string(end_names, "to");
				end_nodes = *pEnd_nodes;

				Node *source_node = get_node_by_name(graph, end_nodes.front());
				end_nodes.pop_front();
				Node *target_node = get_node_by_name(graph, end_nodes.front());

				list<string> edgeInfo, *pEdgeInfo;
				pEdgeInfo = this->get_list_of_names_from_string(edge_info, "-");
				edgeInfo = *pEdgeInfo;

				string edgeType = edgeInfo.front();

				edgeInfo.pop_front(); // Remove the 'edge type' from list of strings
				edgeInfo.pop_back();  // Remove 'present' from the list of strings

				if ((source_node != NULL) && (target_node != NULL))
				{
					for (it = all_edges.begin(); it != all_edges.end(); it++)
					{
						if ((bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "source_node_id"), get_bvatom_for_node_id(source_node))) && (bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "target_node_id"), get_bvatom_for_node_id(target_node))) && (bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "edge_type"), get_bvatom_for_edge_type(edgeType))) && (bvm->checkBVEquality(get_field_as_bv_from_edge_id((*it)->edge_id, "edge_subtype"), get_bvatom_for_edge_subtype_list(edgeInfo))))

						{
							edges_to_retain.insert((*it));
						}
					}
				}
			}
			else
			{
				// cout << name << " " << value << endl;
			}
		}
	}

	string graph_genesis_label = "Retained edges from Graph";
	list<unsigned int> oper_graphs_list;
	oper_graphs_list.push_back(graph->graph_id);
	string oper_parameters = "that are True in processed Z3 output edge assignments " + filename;
	map<string, Node *> new_graph_map = create_graph_edge_retained(graph, edges_to_retain)->rep_id_to_node_map;
	Graph *new_graph = register_new_graph(new_graph_map, graph_genesis_label, oper_graphs_list, oper_parameters);

	if (new_graph->rep_id_to_node_map.empty())
	{
		cout << "Error in GraphManager::get_subgraph_from_edges : Could not create graph" << endl;
	}
	return new_graph;
}

string GraphManager::get_rep_id_name_iteratively(string nodeName)
{

	while (id_equiv_class_map.find(nodeName) != id_equiv_class_map.end())
	{
		if ((id_equiv_class_map.find(nodeName))->second == nodeName)
			break;

		nodeName = (id_equiv_class_map.find(nodeName))->second;
	}
	return nodeName;
}

Node *GraphManager::create_node(string name)
{
	if (name == "")
		return NULL;
	Node *node = new Node(name);
	return node;
}

Node *GraphManager::create_node(string name, string display_name, vector<Node *> &comp_set, string type, string link)
{
	if (name == "")
		return NULL;
	Node *node = new Node(name, display_name, comp_set, type, link);
	return node;
}

Node *GraphManager::create_node(string name, string display_name, string type)
{
	if (name == "")
		return NULL;
	vector<Node *> empty_set_of_nodes;
	string empty_link = "";
	Node *node = new Node(name, display_name, empty_set_of_nodes, type, empty_link);
	return node;
}

Node *GraphManager::duplicate_node(Node *sourceNode)
{
	Node *copyNode = new Node(sourceNode->rep_id_name);
	copyNode->node_link = sourceNode->node_link;
	copyNode->node_type = sourceNode->node_type;
	copyNode->rep_display_name = sourceNode->rep_display_name;
	copyNode->components = sourceNode->components;

	return copyNode;
}

// Node *GraphManager::get_node_by_name(Graph * graph, string id_name) {
//	for (map<string, Node *>::iterator graph_it = graph.begin(); graph_it != graph.end(); graph_it++) {
//		if((*graph_it).second->rep_id_name == id_name)
//			return (*graph_it).second;
//		}
//	return NULL;
// }

list<string> *GraphManager::get_list_of_names_from_string(string name_str, string delim)
{
	list<string> *list_of_labels = new list<string>;
	unsigned start_pos = 0;
	while (start_pos < name_str.length())
	{
		std::size_t found = name_str.find(delim, start_pos);
		if (found != std::string::npos)
		{
			(*list_of_labels).push_back(name_str.substr(start_pos, found - start_pos));
			start_pos = found + delim.length();
		}
		else
		{
			(*list_of_labels).push_back(name_str.substr(start_pos));
			break;
		}
	}
	return list_of_labels;
}

string GraphManager::get_and_update_rep_id_name(string id_name)
{

	if (id_equiv_class_map.find(id_name) == id_equiv_class_map.end())
	{
		// return(invalid_string);
		return "#";
		// call add_to_display_name_map(id)
		// id_name_map.insert(pair<string, string>(id_name, id_name));//added now
		// add_to_id_name_map(id_name, id_name);
	}

	string curr_id_name = id_name;

	//	while ((id_name_map.find(curr_id_name))->second != curr_id_name) {
	//		curr_id_name = (id_name_map.find(curr_id_name))->second;
	//	}

	while (id_equiv_class_map.find(curr_id_name) != id_equiv_class_map.end())
	{

		if ((id_equiv_class_map.find(curr_id_name))->second == curr_id_name)
		{
			break;
		}

		curr_id_name = (id_equiv_class_map.find(curr_id_name))->second;
	}

	string rep_id_name = curr_id_name;
	curr_id_name = id_name;

	//	while ((id_name_map.find(curr_id_name))->second != curr_id_name) {
	//		string curr_id_name1 = (id_name_map.find(curr_id_name))->second;
	//		id_name_map.erase(curr_id_name);
	//		id_name_map.insert(pair<string, string>(curr_id_name, rep_id_name));
	//		curr_id_name = curr_id_name1;
	//	}

	while (id_equiv_class_map.find(curr_id_name) != id_equiv_class_map.end())
	{

		if ((id_equiv_class_map.find(curr_id_name))->second == curr_id_name)
		{
			break;
		}
		string curr_id_name1 = (id_equiv_class_map.find(curr_id_name))->second;
		id_equiv_class_map.erase(curr_id_name);
		id_equiv_class_map.insert(pair<string, string>(curr_id_name, rep_id_name));
		curr_id_name = curr_id_name1;
	}

	return rep_id_name;
}

string GraphManager::get_and_update_rep_display_name(string display_name)
{

	if (display_name_map.find(display_name) == display_name_map.end())
		// return(invalid_string);
		return "#";

	string curr_display_name = display_name;
	// while ((display_name_map.find(curr_display_name))->second != curr_display_name) {
	//    curr_display_name = (display_name_map.find(curr_display_name))->second;
	// }

	while (display_name_map.find(curr_display_name) != display_name_map.end())
	{
		if ((display_name_map.find(curr_display_name))->second == curr_display_name)
		{
			break;
		}

		curr_display_name = (display_name_map.find(curr_display_name))->second;
	}
	string rep_display_name = curr_display_name;
	curr_display_name = display_name;
	//	 while ((display_name_map.find(curr_display_name))->second != curr_display_name) {
	//		 string curr_display_name1 = (display_name_map.find(curr_display_name))->second; //curr_display_name;
	//		 display_name_map.erase(curr_display_name);
	//		 display_name_map.insert(pair<string, string>(curr_display_name, rep_display_name));
	//		 curr_display_name = curr_display_name1;
	//	 }
	while (display_name_map.find(curr_display_name) != display_name_map.end())
	{
		if ((display_name_map.find(curr_display_name))->second == curr_display_name)
		{
			break;
		}
		string curr_display_name1 = (display_name_map.find(curr_display_name))->second;
		display_name_map.erase(curr_display_name);
		display_name_map.insert(pair<string, string>(curr_display_name, rep_display_name));
		curr_display_name = curr_display_name1;
	}

	return rep_display_name;
}

string GraphManager::get_and_update_rep_id_name_from_display_name(string display_name)
{

	string curr_display_name = display_name;
	while ((display_name_map.find(curr_display_name))->second != curr_display_name)
	{
		curr_display_name = (display_name_map.find(display_name))->second;
	}

	string rep_display_name = curr_display_name;

	if (display_id_name_map.find(rep_display_name) == display_id_name_map.end())
		// return(invalid_string);
		return "#";

	string curr_id_name = display_id_name_map.find(rep_display_name)->second;
	while ((id_equiv_class_map.find(curr_id_name))->second != curr_id_name)
	{
		curr_id_name = (id_equiv_class_map.find(curr_id_name))->second;
	}

	string rep_id_name = curr_id_name;

	curr_display_name = display_name;
	while ((display_name_map.find(curr_display_name))->second != curr_display_name)
	{
		if (display_id_name_map.find(curr_display_name) == display_id_name_map.end())
		{
			display_id_name_map.insert(pair<string, string>(curr_display_name, rep_id_name));
		}
		else
		{
			if (display_id_name_map.find(curr_display_name)->second != rep_id_name)
			{
				display_id_name_map.erase(curr_display_name);
				display_id_name_map.insert(pair<string, string>(curr_display_name, rep_id_name));
			}
		}
		curr_display_name = display_name_map.find(curr_display_name)->second;
	}
	return rep_id_name;
}

void GraphManager::add_to_id_name_map(string id_name, string rep_id_name)
{

	if ((id_equiv_class_map.find(id_name) != id_equiv_class_map.end()) && (id_equiv_class_map.find(id_name)->second == rep_id_name))
	{
		return;
	}

	else
	{
		if (id_equiv_class_map.find(id_name) != id_equiv_class_map.end())
		{
			id_equiv_class_map.erase(id_name);
		}
		id_equiv_class_map.insert(pair<string, string>(id_name, rep_id_name));
		// cout << "inserted pair " << id_name << " " << rep_id_name << " onto id_name_map" << endl;
	}
}

void GraphManager::add_to_display_name_map(string display_name, string rep_display_name)
{

	if ((display_name_map.find(display_name) != display_name_map.end()) && (display_name_map.find(display_name)->second == rep_display_name))
	{
		return;
	}

	else
	{
		if (display_name_map.find(display_name) != display_name_map.end())
		{
			display_name_map.erase(display_name);
		}
		display_name_map.insert(pair<string, string>(display_name, rep_display_name));
		// cout << "inserted pair " << display_name << " " << rep_display_name << " onto display_name_map" << endl;
	}
}

void GraphManager::add_to_display_id_name_map(string rep_display_name, string rep_id_name)
{

	if ((display_id_name_map.find(rep_display_name) != display_id_name_map.end()) && (display_id_name_map.find(rep_display_name)->second == rep_id_name))
	{
		return;
	}

	else
	{
		if (display_id_name_map.find(rep_display_name) != display_id_name_map.end())
		{
			display_id_name_map.erase(rep_display_name);
		}
		display_id_name_map.insert(pair<string, string>(rep_display_name, rep_id_name));
		// cout << "inserted pair " << rep_display_name << " " << rep_id_name << " onto display_id_name_map" << endl;
	}
}

void GraphManager::add_to_local_to_hsa_node_id_map(unsigned long local_node_id, string rep_id_name)
{

	node_id_to_rep_id_map.insert(pair<unsigned long, string>(local_node_id, rep_id_name));
}

string GraphManager::get_hsa_id_from_local_node_id(unsigned long source_node_id)
{
	if (node_id_to_rep_id_map.find(source_node_id) == node_id_to_rep_id_map.end())
	{
		cerr << "Node id not found" << endl;
	}
	return node_id_to_rep_id_map.find(source_node_id)->second;
}

Node *GraphManager::get_node_by_name(Graph *graph, string name)
{
	if (graph->rep_id_to_node_map.find(name) == graph->rep_id_to_node_map.end())
	{
		cerr << "Node name not found" << endl;
		return NULL;
	}
	return graph->rep_id_to_node_map.find(name)->second;
}

// flattens the map of equivalence class such that in each pair of the equivalence class of id the second is the rep id
// returns rep id for that equivalence class
string GraphManager::flatten_id_name_map_util(string id)
{
	map<string, string>::iterator itr = id_equiv_class_map.find(id);
	if (itr == id_equiv_class_map.end())
	{
		cerr << "node id " + id + " is not present in the map!" << endl;
		return "";
	}

	if (itr->first == itr->second)
	{
		return itr->second;
	}

	itr->second = flatten_id_name_map_util(itr->second);
	return itr->second;
}

// flattens the entire id name map using func flatten_id_name_map_util
void GraphManager::flatten_id_name_map()
{
	for (map<string, string>::iterator itr = id_equiv_class_map.begin(); itr != id_equiv_class_map.end(); ++itr)
	{
		cout << "flattening map" << endl;
		if (itr->second != itr->first)
		{
			itr->second = flatten_id_name_map_util(itr->first);
		}
	}
}

string GraphManager::uint_to_bin(unsigned int number)
{
	if (number == 0)
		return "0";
	if (number == 1)
		return "1";

	if (number % 2 == 0)
		return uint_to_bin(number / 2) + "0";
	else
		return uint_to_bin(number / 2) + "1";
}

bvatom GraphManager::create_edge_id(Node *source, Node *target, string edge_type, list<string> edge_subtype, list<string> pathways)
{

	bvatom source_type_bv;
	if (nodeTypeEncodingMap.find(source->node_type) == nodeTypeEncodingMap.end())
	{
		cerr << "Node has unrecognized type: " << source->node_type << endl;
		source_type_bv = bvm->getZeroedBVatom(NODE_TYPE_LENGTH);
	}

	else
	{
		string source_type_binary = uint_to_bin((nodeTypeEncodingMap.find(source->node_type))->second);
		source_type_bv = bvm->getBVatom(NODE_TYPE_LENGTH, source_type_binary);
	}
	// cout << "source_type: " << source->node_type << endl;
	string source_id_binary = uint_to_bin(source->node_id);
	// cout << "source_id: " << source->node_id << endl;//" binary: " << source_id_binary << endl;
	bvatom source_id_bv = bvm->getBVatom(NODE_ID_LENGTH, source_id_binary);

	bvatom curr_id = bvm->concat(source_type_bv, source_id_bv);
	// cout << "target_type: " << target->node_type << endl;
	////string target_type_binary = uint_to_bin(atoi((target->node_type).c_str()));
	string target_type_binary = uint_to_bin((nodeTypeEncodingMap.find(target->node_type))->second);
	bvatom target_type_bv = bvm->getBVatom(NODE_TYPE_LENGTH, target_type_binary);

	curr_id = bvm->concat(curr_id, target_type_bv);

	string target_id_binary = uint_to_bin(target->node_id);
	// cout << "target_id: " << target->node_id << endl; // " binary: " << target_id_binary << endl;
	bvatom target_id_bv = bvm->getBVatom(NODE_ID_LENGTH, target_id_binary);

	curr_id = bvm->concat(curr_id, target_id_bv);

	// bvatom temp1 = bvm->concat(source_id_bv, target_id_bv);
	// cout << "Source and target ids: ";
	// bvm->printPrettyBVatom(temp1);
	// cout << "\n";

	bvatom edge_type_bv;
	if (edgeTypeEncodingMap.find(edge_type) == edgeTypeEncodingMap.end())
	{
		cerr << "Edge has unrecognized type: " << edge_type << endl;
		edge_type_bv = bvm->getZeroedBVatom(EDGE_TYPE_LENGTH);
	}

	else
	{
		string edge_type_value = uint_to_bin((edgeTypeEncodingMap.find(edge_type))->second);
		edge_type_bv = bvm->getBVatom(EDGE_TYPE_LENGTH, edge_type_value);
	}
	// cout << "edge type: " << bvm->printBVasBinString(edge_type_bv) << endl;
	curr_id = bvm->concat(curr_id, edge_type_bv);

	// bvatom temp2 = bvm->concat(temp1, edge_type_bv);
	// cout << "Source, target and type: ";
	// bvm->printPrettyBVatom(temp2);
	// cout << "\n";

	bvatom edge_subtype_bv = bvm->getZeroedBVatom(EDGE_SUBTYPE_LENGTH);

	list<string>::iterator it = edge_subtype.begin();
	string one_str = toString(1);
	bvatom singleton_bv = bvm->getBVatom(1, one_str);

	int edge_subtype_count = 0;
	while (it != edge_subtype.end())
	{
		edge_subtype_count++;
		if (edgeSubtypeEncodingMap.find((*it)) == edgeSubtypeEncodingMap.end())
		{
			cerr << "Edge has unrecognized subtype: " << (*it) << endl;
		}
		else
		{
			string edge_subtype_value = uint_to_bin((edgeSubtypeEncodingMap.find(*it))->second);
			// cout << "subtype value: " << (subtypeEncodingMap.find(*it))->second << endl;

			bvm->replaceBySelBVatom(singleton_bv, edge_subtype_bv, (unsigned long)0, (unsigned long)(edgeSubtypeEncodingMap.find(*it))->second, (unsigned long)1);
		}
		it++;
	}
	// cout << "edge subtype: " << bvm->printBVasBinString(edge_subtype_bv) << endl;

	//	if (subtype_count > 1)
	//		cout << "#######";

	curr_id = bvm->concat(curr_id, edge_subtype_bv);

	//	bvatom temp3 = bvm->concat(temp2, edge_subtype_bv);
	//	cout << "Source, target type and complete subtype: ";
	//	bvm->printPrettyBVatom(temp3);
	//	cout << "\n";
	//	cout << "Subtype of edge " << source->rep_id_name << " - " << target->rep_id_name << " : ";
	//	bvm->printPrettyBVatom(edge_subtype_bv);
	//	cout << " whose binary: " << bvm->printBVasBinString(edge_subtype_bv) << "\n";

	bvatom pathway_bv = bvm->getZeroedBVatom(PATHWAY_ID_LENGTH);

	it = pathways.begin();
	while (it != pathways.end())
	{
		if (pathwayEncodingMap.find(*it) == pathwayEncodingMap.end())
		{
			cerr << "Edge has unrecognized pathway information: " << (*it) << endl;
		}
		else
		{
			string pathway_value = uint_to_bin((pathwayEncodingMap.find(*it))->second);
			// cout << "pathway value: " << (pathwayEncodingMap.find(*it))->second << endl;

			bvm->replaceBySelBVatom(singleton_bv, pathway_bv, (unsigned long)0, (unsigned long)(pathwayEncodingMap.find(*it))->second, (unsigned long)1);
		}
		it++;
	}

	//	bvatom pathway_bv = bvm->getZeroedBVatom(PATHWAY_ID_LENGTH);
	//
	//	if (pathwayEncodingMap.find(pathway) == pathwayEncodingMap.end()) {
	//		cerr << "Edge has unrecognized pathway information: " << pathway << endl;
	//	}
	//	else {
	//		string pathway_value = uint_to_bin((pathwayEncodingMap.find(pathway))->second);
	//		//cout << "pathway value: " << (pathwayEncodingMap.find(*it))->second << endl;
	//
	//		bvm->replaceBySelBVatom(singleton_bv, pathway_bv, (unsigned long)0, (unsigned long)(pathwayEncodingMap.find(pathway))->second, (unsigned long)1);
	//
	//	}

	curr_id = bvm->concat(curr_id, pathway_bv);

	return curr_id;
}

Edge *GraphManager::create_edge(Node *source, Node *target)
{
	string type = "";
	list<string> subtype; // = new list<string>;
	subtype.empty();
	string pathway = "";
	list<string> pathways;
	pathways.empty();
	bvatom edge_id = create_edge_id(source, target, type, subtype, pathways);
	// string id = get_and_update_rep_id_name(source->rep_id_name) + "_" + get_and_update_rep_id_name(target->rep_id_name);
	Edge *new_edge = new Edge(edge_id, source, target);

	(source->outlist).insert(new_edge);
	(target->inlist).insert(new_edge);

	return new_edge;
}

Edge *GraphManager::create_edge(Node *source, Node *target, string type, list<string> subtype, list<string> pathways)
{
	string subtype_str;
	list<string>::iterator it = subtype.begin();
	while (it != subtype.end())
	{
		subtype_str = subtype_str + "_" + (*it);
		it++;
	}

	bvatom edge_id = create_edge_id(source, target, type, subtype, pathways);
	// cout << "Edge belongs to pathway " << pathway << endl;
	// string id = get_and_update_rep_id_name(source->rep_id_name) + "_" + get_and_update_rep_id_name(target->rep_id_name) + subtype_str;

	Edge *new_edge = new Edge(edge_id, source, target, type, subtype, pathways);
	// Edge * new_edge = new Edge(id, source, target, type, subtype, pathway);

	(source->outlist).insert(new_edge);
	(target->inlist).insert(new_edge);

	//////	if (subtype_str == "_binding/association") {
	//////		//Edge * new_edge1 = new Edge(id, source, target, type, subtype, pathway);
	//////		Edge * new_edge1 = new Edge(id, target, source, type, subtype, pathway);
	//////		(target->outlist).insert(new_edge1);
	//////		(source->inlist).insert(new_edge1);
	//////	}

	return new_edge;
}

// Edge * GraphManager::create_edge(Node * source, Node * target, string type, list<string> subtype, list<string> pathway)
//{
//	string subtype_str;
//	list<string>::iterator it = subtype.begin();
//	while (it!= subtype.end()) {
//		subtype_str= subtype_str + "_" + (*it);
//		it++;
//	}
//
//	//unsigned int edge_id = create_edge_id(source, target, type, subtype);
//	string id = get_and_update_rep_id_name(source->rep_id_name) + "_" + get_and_update_rep_id_name(target->rep_id_name) + subtype_str;
//
//
//	Edge * new_edge = new Edge(id, source, target, type, subtype, pathway);
//
//	(source->outlist).insert(new_edge);
//	(target->inlist).insert(new_edge);
//
////////	if (subtype_str == "_binding/association") {
////////		//Edge * new_edge1 = new Edge(id, source, target, type, subtype, pathway);
////////		Edge * new_edge1 = new Edge(id, target, source, type, subtype, pathway);
////////		(target->outlist).insert(new_edge1);
////////		(source->inlist).insert(new_edge1);
////////	}
//
//	return new_edge;
//}

// Graph * GraphManager::add_edge(Graph * graph1, string source_name, string target_name) {
//	Node * source;
//	Node * target;
//
//	map<string, Node *> graph = graph1->map_of_nodes;
//	if (graph.find(source_name) != graph.end()) {
//		source = graph.find(source_name)->second;
//	}
//
//	else {
//		source = create_node(source_name);
//		graph.insert(pair<string ,Node *>(source_name, source));
//	}
//
//	if (graph.find(target_name) != graph.end()) {
//		target = graph.find(target_name)->second;
//	}
//
//	else {
//		target = create_node(target_name);
//		graph.insert(pair<string ,Node *>(target_name, target));
//	}
//
//	string edge_id = source_name + "_" + target_name;
//	set<string> edge_set = get_all_edge_id_in_graph(graph);
//	if (edge_set.find(edge_id) == edge_set.end()) {
//		Edge * new_edge = create_edge(source, target);
//		((graph.find(source->get_name())->second)->outlist).insert(new_edge);
//		((graph.find(target->get_name())->second)->inlist).insert(new_edge);
//	}
//
//	Graph * new_graph = register_new_graph();
//	return graph;
//
// }

set<bvatom> &GraphManager::get_all_edge_ids_from_node_map(map<string, Node *> node_map)
{
	set<bvatom> *edge_id_set = new set<bvatom>;
	// set<unsigned int> edge_set;

	for (map<string, Node *>::iterator it = node_map.begin(); it != node_map.end(); it++)
	{
		set<Edge *>::iterator it1;

		for (it1 = (*it).second->outlist.begin(); it1 != (*it).second->outlist.end(); it1++)
		{
			edge_id_set->insert((*it1)->edge_id);
		}

		for (it1 = (*it).second->inlist.begin(); it1 != (*it).second->inlist.end(); it1++)
		{
			edge_id_set->insert((*it1)->edge_id);
		}
	}
	return *edge_id_set;
}

set<Edge *> &GraphManager::get_all_edges_from_node_map(map<string, Node *> node_map)
{
	set<Edge *> *edge_set = new set<Edge *>;
	for (map<string, Node *>::iterator it = node_map.begin(); it != node_map.end(); it++)
	{
		set<Edge *>::iterator it1;

		for (it1 = (*it).second->outlist.begin(); it1 != (*it).second->outlist.end(); it1++)
		{
			edge_set->insert(*it1);
		}

		for (it1 = (*it).second->inlist.begin(); it1 != (*it).second->inlist.end(); it1++)
		{
			edge_set->insert(*it1);
		}
	}
	return *edge_set;
}

//////
//////map<string, Node *> GraphManager::merge_two_graphs(map<string, Node *> g1, map<string, Node *> g2, int mode) {
//////
//////	set <string> set_num;
//////	map<string, Node *> g3;
//////	map<string, Node *>::iterator it1, it2, it3;
//////
//////
//////	for (it1 = g1.begin(); it1 != g1.end(); it1++) {
//////
//////		if (mode == MERGE_MODE) {
//////			// Get all nodes from first graph
//////			Node * g1Node = (*it1).second;
//////			Node * newNode = new Node(g1Node->rep_id_name, g1Node->rep_display_name, g1Node->components, g1Node->node_type, g1Node->node_link);
//////                        add_to_local_to_hsa_node_id_map(newNode->node_id, newNode->rep_id_name);
//////			g3.insert(pair<string , Node *>((*it1).first, newNode));
//////		}
//////	}
//////
//////	if (mode == MERGE_MODE) {
//////
//////		// Get remaining nodes from second graph
//////		for (it2 = g2.begin(); it2 != g2.end(); it2++) {
//////
//////			// Node not already created
//////			if (g3.count((*it2).first) == 0) {
//////				Node * g2Node = (*it2).second;
//////				Node * newNode = new Node(g2Node->rep_id_name, g2Node->rep_display_name, g2Node->components, g2Node->node_type, g2Node->node_link);
//////                                add_to_local_to_hsa_node_id_map(newNode->node_id, newNode->rep_id_name);
//////				g3.insert(pair<string ,Node *>((*it2).first, newNode));
//////			}
//////		}
//////
//////	}
//////
//////	set <Edge *>::iterator it;
//////	set<bvatom> edgeIdsInG2 = this->get_all_edge_ids_from_node_map(g2);
//////	set<bvatom> edgeIdsInG1 = this->get_all_edge_ids_from_node_map(g1);
//////
//////	for (it3 = g3.begin(); it3 != g3.end(); it3++) {
//////
//////		string id = (*it3).first;
//////
//////		set <Edge *> list1, list2;
//////
//////		// If node with id present in graph 1
//////		if (g1.find(id) != g1.end()) {
//////
//////			list1 = g1[id]->outlist;
//////			for (it=list1.begin(); it != list1.end(); it++) {
//////
//////				bvatom eid = (*it)->edge_id;
//////
//////				//For each element in outlist
//////				//g3.addEdge();
//////
//////				// Should this edge remain
//////				if (mode == MERGE_MODE) {
//////					create_edge((*it3).second, g3[(*it)->edge_target->rep_id_name],	(*it)->edge_type, (*it)->edge_subtype, (*it)->belongs_to_pathway);
//////				}
//////			}
//////
//////		}
//////
//////		if ((mode == MERGE_MODE) && (g2.find(id) != g2.end()))  {
//////			list2 = g2[id]->outlist;
//////
//////			for (it=list2.begin(); it != list2.end(); it++) {
//////
//////				bvatom eid = (*it)->edge_id;
//////
//////				if (edgeIdsInG1.find(eid) == edgeIdsInG1.end()) {
//////
//////					create_edge((*it3).second, g3[(*it)->edge_target->rep_id_name], (*it)->edge_type, (*it)->edge_subtype, (*it)->belongs_to_pathway);
//////
//////				}
//////			}
//////		}
//////
//////	}
//////
//////	return g3;
//////
//////}

/*map<string, Node *>& GraphManager::merge_two_graphs(map<string, Node *> g1, map<string, Node *> g2, int mode) {

	set<string> set_num;
	map<string, Node *> * g3 = new map<string, Node *>;
	map<string, Node *>::iterator it1, it2, it3;


	for (it1 = g1.begin(); it1 != g1.end(); it1++) {

		if (mode == MERGE_MODE) {
			// Get all nodes from first graph
			Node * g1Node = (*it1).second;
			Node * newNode = new Node(g1Node->rep_id_name, g1Node->rep_display_name, g1Node->components, g1Node->node_type, g1Node->node_link);
						add_to_local_to_hsa_node_id_map(newNode->node_id, newNode->rep_id_name);
						g3->insert(pair<string , Node *>((*it1).first, newNode));

		}
	}

	if (mode == MERGE_MODE) {

		// Get remaining nodes from second graph
		for (it2 = g2.begin(); it2 != g2.end(); it2++) {

			// Node not already created
			if (g3->count((*it2).first) == 0) {
				Node * g2Node = (*it2).second;
				Node * newNode = new Node(g2Node->rep_id_name, g2Node->rep_display_name, g2Node->components, g2Node->node_type, g2Node->node_link);
								add_to_local_to_hsa_node_id_map(newNode->node_id, newNode->rep_id_name);
				g3->insert(pair<string ,Node *>((*it2).first, newNode));
			}
		}

	}

		if (mode == INTERSECT_MODE) {
			// For each node in first graph
			for (it1 = g1.begin(); it1 != g1.end(); it1++) {
				for (it2 = g2.begin(); it2 != g2.end(); it2++){
					// Node present in both graphs
					if((*it1).first == (*it2).first) {
						// Node not already created
						if (g3->count((*it1).first) == 0) {
				Node * g2Node = (*it1).second;
				Node * newNode = new Node(g2Node->rep_id_name, g2Node->rep_display_name, g2Node->components, g2Node->node_type, g2Node->node_link);
								add_to_local_to_hsa_node_id_map(newNode->node_id, newNode->rep_id_name);
				g3->insert(pair<string ,Node *>((*it1).first, newNode));
			}
					}
				}
			}
		}

	set <Edge *>::iterator it;
	set<bvatom> edgeIdsInG2 = this->get_all_edge_ids_from_node_map(g2);
	set<bvatom> edgeIdsInG1 = this->get_all_edge_ids_from_node_map(g1);

		// Get set of all edge ids in graph with pathway masked
	set<bvatom>::iterator set_bv_iter;
		set<bvatom> edgeIdsInG1PathwayMasked;
		bvatom mask_bvatom = bvm->getZeroedBVatom(PATHWAY_ID_LENGTH);
		for(set_bv_iter = edgeIdsInG1.begin(); set_bv_iter != edgeIdsInG1.end(); set_bv_iter++) {
				bvatom changed_edge_id = (*set_bv_iter);
				bvm->replaceBySelBVatom(mask_bvatom, changed_edge_id, (unsigned long)0, (unsigned long)0, (unsigned long)PATHWAY_ID_LENGTH);
				edgeIdsInG1PathwayMasked.insert(changed_edge_id);
		}


	for (it3 = g3->begin(); it3 != g3->end(); it3++) {

		string id = (*it3).first;

		set <Edge *> list1, list2;

		// If node with id present in graph 1
		if (g1.find(id) != g1.end()) {

			list1 = g1[id]->outlist;
			for (it=list1.begin(); it != list1.end(); it++) {

				bvatom eid = (*it)->edge_id;

				//For each element in outlist
				//g3.addEdge();

				// Should this edge remain
				if (mode == MERGE_MODE) {
					create_edge((*it3).second, (*g3)[(*it)->edge_target->rep_id_name], (*it)->edge_type, (*it)->edge_subtype, (*it)->belongs_to_pathways);
				}
			}

		}

		if ((mode == MERGE_MODE) && (g2.find(id) != g2.end()))  {
			list2 = g2[id]->outlist;

			for (it=list2.begin(); it != list2.end(); it++) {

				bvatom eid = (*it)->edge_id;
								bvatom reduced_eid = eid;

								// get edge id with pathway information masked
								bvatom mask_bvatom = bvm->getZeroedBVatom(PATHWAY_ID_LENGTH);
								bvm->replaceBySelBVatom(mask_bvatom, reduced_eid, (unsigned long)0, (unsigned long)0, (unsigned long)PATHWAY_ID_LENGTH);

								// get all edge ids in graph with pathway information masked


								if (edgeIdsInG1PathwayMasked.find(reduced_eid) == edgeIdsInG1PathwayMasked.end()) {
										create_edge((*it3).second, (*g3)[(*it)->edge_target->rep_id_name], (*it)->edge_type, (*it)->edge_subtype, (*it)->belongs_to_pathways);

								}
								list<string> pathways;
								pathways= (*it)->belongs_to_pathways;


////				if (edgeIdsInG1.find(eid) == edgeIdsInG1.end()) {
////
////					create_edge((*it3).second, (*g3)[(*it)->edge_target->rep_id_name], (*it)->edge_type, (*it)->edge_subtype, (*it)->belongs_to_pathways);
////
////				}
			}
		}

				if (mode == INTERSECT_MODE) {// && (g1.find(id) != g1.end()) && (g2.find(id) != g2.end())) {

					cout << "Here" << endl;
						list1 = g1[id]->outlist;
						list2 = g2[id]->outlist;
						cout << "Id " << id << " outlist size" << list1.size() << " " << list2.size() << endl;
						set <Edge *>::iterator l1;
						set <Edge *>::iterator l2;
						// For each edge in first graph
						for (l1=list1.begin(); l1 != list1.end(); l1++) {
							for (l2=list2.begin(); l2 != list2.end(); l2++) {
								if (((*l1)->edge_source->rep_id_name == (*l2)->edge_source->rep_id_name) && ((*l1)->edge_target->rep_id_name == (*l2)->edge_target->rep_id_name)) {
										create_edge((*it3).second, (*g3)[(*l1)->edge_target->rep_id_name], (*l1)->edge_type, (*l1)->edge_subtype, (*l1)->belongs_to_pathways);

								}
							}
						}
				}

	}

	return *g3;

}*/

map<string, Node *> GraphManager::merge_two_graphs(map<string, Node *> g1, map<string, Node *> g2, int mode)
{
	set<string> set_num;
	map<string, Node *> g3;
	map<string, Node *>::iterator it1, it2, it3;

	for (it1 = g1.begin(); it1 != g1.end(); it1++)
	{

		if (mode == MERGE_MODE)
		{
			// Get all nodes from first graph
			Node *g1Node = (*it1).second;
			Node *newNode = new Node(g1Node->rep_id_name,
									 g1Node->rep_display_name, g1Node->components, g1Node->node_type,
									 g1Node->node_link);

			add_to_local_to_hsa_node_id_map(newNode->node_id, newNode->rep_id_name);
			// g3.insert(pair<string , Node *>((*it1).first, newNode));
			g3.insert(pair<string, Node *>(newNode->rep_id_name, newNode));
		}
	}

	if (mode == MERGE_MODE)
	{

		// Get remaining nodes from second graph
		for (it2 = g2.begin(); it2 != g2.end(); it2++)
		{

			// Node not already created
			if (g3.count((*it2).first) == 0)
			{
				Node *g2Node = (*it2).second;
				Node *newNode = new Node(g2Node->rep_id_name,
										 g2Node->rep_display_name, g2Node->components, g2Node->node_type,
										 g2Node->node_link);

				add_to_local_to_hsa_node_id_map(newNode->node_id,
												newNode->rep_id_name);
				// g3.insert(pair<string ,Node *>((*it2).first, newNode));
				g3.insert(pair<string, Node *>(newNode->rep_id_name, newNode));
			}
		}
	}

	set<Edge *>::iterator it;
	// set<bvatom> edgeIdsInG2 = this->get_all_edges_from_node_map(g2);
	//	set<bvatom> edgeIdsInG1 = this->get_all_edges_from_node_map(g1);
	set<bvatom> edgeIdsInG2 = this->get_all_edge_ids_from_node_map(g2);
	set<bvatom> edgeIdsInG1 = this->get_all_edge_ids_from_node_map(g1);
	for (it3 = g3.begin(); it3 != g3.end(); it3++)
	{

		string id = (*it3).first;

		set<Edge *> list1, list2;

		// If node with id present in graph 1
		if (g1.find(id) != g1.end())
		{

			list1 = g1[id]->outlist;
			for (it = list1.begin(); it != list1.end(); it++)
			{

				bvatom eid = (*it)->edge_id;

				// For each element in outlist
				// g3.addEdge();

				// Should this edge remain
				if (mode == MERGE_MODE)
				{
					create_edge((*it3).second,
								g3[(*it)->edge_target->rep_id_name], (*it)->edge_type,
								(*it)->edge_subtype, (*it)->belongs_to_pathways);
				}
			}
		}

		if ((mode == MERGE_MODE) && (g2.find(id) != g2.end()))
		{
			list2 = g2[id]->outlist;

			for (it = list2.begin(); it != list2.end(); it++)
			{

				bvatom eid = (*it)->edge_id;

				if (edgeIdsInG1.find(eid) == edgeIdsInG1.end())
				{

					create_edge((*it3).second, g3[(*it)->edge_target->rep_id_name],
								(*it)->edge_type, (*it)->edge_subtype, (*it)->belongs_to_pathways);
				}
			}
		}
	}
	return g3;
}

Graph *GraphManager::merge_graphs(list<Graph *> glist, int mode)
{

	assert(glist.size() > 0);
	string graph_genesis_label = "Merged graphs ";
	list<unsigned int> operand_graph_id_list;

	map<string, Node *> merged_graph = (glist.front())->rep_id_to_node_map;
	operand_graph_id_list.push_back(glist.front()->graph_id);
	glist.pop_front();
	// int i = 1;
	while (!glist.empty())
	{
		map<string, Node *> graph2 = (glist.front())->rep_id_to_node_map;
		operand_graph_id_list.push_back(glist.front()->graph_id);
		glist.pop_front();
		// merged_graph = merge_two_graphs(merged_graph, graph2, MERGE_MODE);
		// cout << "merging graph " << i << endl;
		merged_graph = merge_two_graphs(merged_graph, graph2, mode);
		// i++;
	}

	Graph *result_graph = register_new_graph(merged_graph, graph_genesis_label, operand_graph_id_list, "");
	return result_graph;
}

Graph *GraphManager::bounded_reach(Graph *g, set<Node *> source_nodes_set, set<Node *> excluded_nodes_set,
								   int bound, int type_of_edge, int direction)
{
	map<string, Node *> graph = g->rep_id_to_node_map;
	map<string, Node *> resultGraph;
	list<Node *> updated_nodes_queue; // it is a list
	// list<Node *> updated_nodes_list;
	int num_nodes = graph.size();

	for (set<Node *>::iterator node = source_nodes_set.begin(); node != source_nodes_set.end(); node++)
	{
		string nodeName = (*node)->rep_id_name;

		while (id_equiv_class_map.find(nodeName) != id_equiv_class_map.end())
		{

			if ((id_equiv_class_map.find(nodeName))->second == nodeName)
			{
				break;
			}

			nodeName = (id_equiv_class_map.find(nodeName))->second;
		}
		// string nodeName = get_rep_id_name_recursively((*node)->rep_id_name);

		// do we need to keep this check here? once checked in main.cc

		// nodeName is repid of a source node???
		if (graph.find(nodeName) == graph.end())
			//////cerr << "Node " << (*node)->rep_id_name << "not found" << endl;
			cerr << "Node " << nodeName << " not found" << endl;

		else if (excluded_nodes_set.find(*node) == excluded_nodes_set.end())
		{										  // the source node id not in the excluded list
			updated_nodes_queue.push_back(*node); // source node goes in queue
			cout << "Node " << (*node)->rep_id_name << endl;
			// updated_nodes_list.push_back(*node);

			set<Node *> visited;													   // visited nodes
			visited.insert(*node);													   // the source node has been visited
			resultGraph.insert(pair<string, Node *>(nodeName, duplicate_node(*node))); // source node inserted in graph
			// resultGraph.insert(pair<string, Node *>(nodeName, *node)); //// added on Oct 17

			map<Node *, int> dist_from_source; // distances of nodes from source node
			for (map<string, Node *>::iterator graph_it = graph.begin(); graph_it != graph.end(); graph_it++)
			{
				dist_from_source.insert(pair<Node *, int>((*graph_it).second, -1)); // in the begining all  distances from source node are -1
			}

			dist_from_source[*node] = 0; // its own distance is zero

			while (!updated_nodes_queue.empty())
			{											   // as long as there are elements in queue - BFSish??
				Node *node1 = updated_nodes_queue.front(); // node1 is source node in original graph
				visited.insert(node1);					   // added here on 29 Nov 2013
				updated_nodes_queue.pop_front();

				// node1 is the node dequed

				Node *newSourceNode; // source node in resulting graph
				if (resultGraph.find(node1->rep_id_name) == resultGraph.end())
				{ // if the dequed node is not already there in the graph insert it
					newSourceNode = duplicate_node(node1);
					resultGraph.insert(pair<string, Node *>(newSourceNode->rep_id_name, newSourceNode)); //// added on Oct 17
				}
				else
				{ // if in graph just retrieve it
					newSourceNode = resultGraph[node1->rep_id_name];
				}

				set<Edge *> edge_list; // get fwd or bkwd list of  node accordingly ?? should we handle undirected edges here
				if (direction == FORWARD)
				{
					edge_list = node1->outlist;
				}
				else if (direction == BACKWARD)
				{
					edge_list = node1->inlist;
				}
				else
				{
					cerr << "Invalid direction of search\n";
					assert(false);
				}

				// go over the edges
				for (set<Edge *>::iterator edge_it = edge_list.begin(); edge_it != edge_list.end(); edge_it++)
				{
					int shouldThisEdgeBeIncluded = 0; // set based on what type of edges include (shouldn't it be just any edge type - we wnat to see the entire flow?)
					Edge *currEdge = *edge_it;

					list<string> subtype_list = currEdge->edge_subtype;
					for (list<string>::iterator subtype_list_it = subtype_list.begin(); subtype_list_it != subtype_list.end(); subtype_list_it++)
					{

						if (((type_of_edge == INHIBITION_EDGE) && (*subtype_list_it == "inhibition")) ||
							((type_of_edge == ACTIVATION_EDGE) && (*subtype_list_it == "activation")) ||
							(type_of_edge == ANY_EDGE))
						{
							shouldThisEdgeBeIncluded = 1;
						}
					}

					if (shouldThisEdgeBeIncluded == 1)
					{

						/*** Added on 6 Dec 2013 ***/
						Node *targetNode; // get the appropriate tartget node for this edge
						if (direction == FORWARD)
						{
							targetNode = currEdge->edge_target;
						}
						else if (direction == BACKWARD)
						{
							targetNode = currEdge->edge_source;
						}
						else
						{
							cerr << "Invalid direction of search\n";
							assert(false);
						}

						/******/

						// Node * targetNode = currEdge->edge_target; // target node in original graph

						if (excluded_nodes_set.find(targetNode) == excluded_nodes_set.end())
						{																						   // if only the target node is not there ine excluded list
							dist_from_source[targetNode] = dist_from_source[node1] + 1;							   // added on 29 Nov 2013 --- update distances
							int distanceToTarget = min(dist_from_source[targetNode], dist_from_source[node1] + 1); // why this?

							if (distanceToTarget <= bound)
							{ // if only the distance is less than bound enque it
								if (visited.find(targetNode) == visited.end())
								{ // only if it is  not visited it be enqued
									updated_nodes_queue.push_back(targetNode);
									//	updated_nodes_list.push_back(targetNode);
									////visited.insert(targetNode);
									// resultGraph.insert(pair<string, Node *>(targetNode->rep_id_name, targetNode)); //// added on Oct 17
								}
								Node *newTargetNode; // target node in resulting graph
								if (resultGraph.find(targetNode->rep_id_name) == resultGraph.end())
								{
									////newTargetNode = duplicate_node(*node);

									newTargetNode = duplicate_node(targetNode);
									resultGraph.insert(pair<string, Node *>(newTargetNode->rep_id_name, newTargetNode)); //// added on Oct 17
								}
								else
								{
									newTargetNode = resultGraph[targetNode->rep_id_name];
								}

								// create edges here only --- so assoc dissoc should be handled here I suppose
								/*** Added on 6 Dec 2013 ***/
								if (direction == FORWARD)
								{
									create_edge(newSourceNode, newTargetNode, currEdge->edge_type, currEdge->edge_subtype, currEdge->belongs_to_pathways);
								}
								else if (direction == BACKWARD)
								{
									create_edge(newTargetNode, newSourceNode, currEdge->edge_type, currEdge->edge_subtype, currEdge->belongs_to_pathways);
								}
								else
								{
									cerr << "Invalid direction of search\n";
									assert(false);
								}
								/******/
								// create_edge(newSourceNode, newTargetNode, currEdge->edge_type, currEdge->edge_subtype, currEdge->belongs_to_pathway);
							}
						}
					}
				}
			}
		}
	}

	// rest is for genesis
	string graph_genesis_operation = "Bounded reach on ";
	list<unsigned int> operand_graphs;
	operand_graphs.push_back(g->graph_id);

	//	set<Node *> source_nodes_set, set<Node *> excluded_nodes_set,
	//					        int bound, int type_of_edge, int direction
	//

	set<Node *>::iterator set_it;
	string str_source_nodes = "Source Nodes: \n";
	for (set_it = source_nodes_set.begin(); set_it != source_nodes_set.end(); set_it++)
	{
		str_source_nodes = str_source_nodes + "\t" + (*set_it)->rep_display_name + "(" + (*set_it)->rep_id_name + ")\n";
	}

	string str_excluded_nodes = "Excluded Nodes: \n";
	for (set_it = excluded_nodes_set.begin(); set_it != excluded_nodes_set.end(); set_it++)
	{
		str_excluded_nodes = str_excluded_nodes + "\t" + (*set_it)->rep_display_name + "(" + (*set_it)->rep_id_name + ")\n";
	}

	string str_type_of_edge;

	if (type_of_edge == INHIBITION_EDGE)
		str_type_of_edge = "Inhibition";
	else if (type_of_edge == ACTIVATION_EDGE)
		str_type_of_edge = "Activation";
	else
		str_type_of_edge = "All";

	string str_direction;
	if (direction == FORWARD)
		str_direction = "Forward";
	else if (direction == BACKWARD)
		str_direction = "Backward";
	else
		str_direction = "Invalid";

	string other_params = str_source_nodes + "\n" + str_excluded_nodes + "\n" + "Bound: " + toString(bound) + "\n" + "Edge type: " + str_type_of_edge + "\n" + "Type of edge: " + str_direction + "\n";
	Graph *finalGraph = register_new_graph(resultGraph, graph_genesis_operation, operand_graphs, other_params);
	//	this->add_new_graph(resultGraph);
	// cout << "Size of result graph: " << resultGraph.size() << endl;
	// return resultGraph;
	return finalGraph;
}

/*Graph * GraphManager::bounded_reach(Graph * g, set<Node *> source_nodes_set, set<Node *> excluded_nodes_set,
		int bound, int type_of_edge, int direction) {
	map<string, Node *> graph = g->map_of_nodes;
	map<string, Node *> resultGraph;
	list<Node *> updated_nodes_queue;
	//list<Node *> updated_nodes_list;
	int num_nodes = graph.size();

	for (set<Node *>::iterator node = source_nodes_set.begin(); node != source_nodes_set.end(); node++) {
		string nodeName = (*node)->rep_id_name;

		while (id_name_map.find(nodeName) != id_name_map.end()) {

			if ((id_name_map.find(nodeName))->second == nodeName) {
				break;
			}

			nodeName = (id_name_map.find(nodeName))->second;

		}
		//string nodeName = get_rep_id_name_recursively((*node)->rep_id_name);

		// do we need to keep this check here? once checked in main.cc

		if (graph.find(nodeName) == graph.end())
			//////cerr << "Node " << (*node)->rep_id_name << "not found" << endl;
			cerr << "Node " << nodeName << " not found" << endl;

		else if (excluded_nodes_set.find(*node) == excluded_nodes_set.end())	{
			updated_nodes_queue.push_back(*node);
			cout << "Node " << (*node)->rep_id_name << endl;
			//updated_nodes_list.push_back(*node);

			set<Node *> visited;
			visited.insert(*node);
			resultGraph.insert(pair<string, Node *>(nodeName, duplicate_node(*node)));
			//resultGraph.insert(pair<string, Node *>(nodeName, *node)); //// added on Oct 17

			map<Node *, int> dist_from_source;
			for (map<string, Node *>::iterator graph_it = graph.begin(); graph_it != graph.end(); graph_it++) {
				dist_from_source.insert(pair<Node *, int>((*graph_it).second, -1));
			}

			dist_from_source[*node] = 0;

			while (!updated_nodes_queue.empty()) {
				Node * node1 = updated_nodes_queue.front(); // node1 is source node in original graph
				visited.insert(node1);// added here on 29 Nov 2013
				updated_nodes_queue.pop_front();

				Node *newSourceNode; // source node in resulting graph
				if (resultGraph.find(node1->rep_id_name) == resultGraph.end()) {
					newSourceNode = duplicate_node(node1);
					resultGraph.insert(pair<string, Node *>(newSourceNode->rep_id_name, newSourceNode)); //// added on Oct 17

				}
				else {
					newSourceNode = resultGraph[node1->rep_id_name];
				}


				set<Edge *> edge_list;
				if (direction == FORWARD) {
					edge_list = node1->outlist;
				}
				else if (direction == BACKWARD) {
					edge_list = node1->inlist;
				}
				else {
					cerr << "Invalid direction of search\n";
					assert(false);
				}


				for (set<Edge *>::iterator edge_it = edge_list.begin(); edge_it != edge_list.end(); edge_it++) {
					int shouldThisEdgeBeIncluded = 0;
					Edge * currEdge = *edge_it;

					list<string> subtype_list = currEdge->edge_subtype;
					for (list<string>::iterator subtype_list_it = subtype_list.begin(); subtype_list_it != subtype_list.end(); subtype_list_it++) {

						if (((type_of_edge == INHIBITION_EDGE)  && (*subtype_list_it == "inhibition")) ||
								((type_of_edge == ACTIVATION_EDGE)  && (*subtype_list_it == "activation")) ||
								(type_of_edge == ANY_EDGE)) {
							shouldThisEdgeBeIncluded = 1;
						}
					}

					if (shouldThisEdgeBeIncluded== 1) {

						** Added on 6 Dec 2013 **
						Node * targetNode;
						if (direction == FORWARD) {
							targetNode = currEdge->edge_target;
						}
						else if (direction == BACKWARD) {
							targetNode = currEdge->edge_source;
						}
						else {
							cerr << "Invalid direction of search\n";
							assert(false);
						}

						****

						//Node * targetNode = currEdge->edge_target; // target node in original graph

						if (excluded_nodes_set.find(targetNode) == excluded_nodes_set.end()) {
							dist_from_source[targetNode] = dist_from_source[node1] + 1; // added on 29 Nov 2013
							int distanceToTarget = min(dist_from_source[targetNode], dist_from_source[node1] + 1);

							if (distanceToTarget <= bound)  {
								if (visited.find(targetNode) == visited.end()) {
									updated_nodes_queue.push_back(targetNode);
									//	updated_nodes_list.push_back(targetNode);
									////visited.insert(targetNode);
									//resultGraph.insert(pair<string, Node *>(targetNode->rep_id_name, targetNode)); //// added on Oct 17

								}
								Node *newTargetNode; // target node in resulting graph
								if (resultGraph.find(targetNode->rep_id_name) == resultGraph.end()) {
									////newTargetNode = duplicate_node(*node);

									newTargetNode = duplicate_node(targetNode);
									resultGraph.insert(pair<string, Node *>(newTargetNode->rep_id_name, newTargetNode)); //// added on Oct 17
								}
								else {
									newTargetNode = resultGraph[targetNode->rep_id_name];
								}

								** Added on 6 Dec 2013 **
								if (direction == FORWARD) {
									create_edge(newSourceNode, newTargetNode, currEdge->edge_type, currEdge->edge_subtype, currEdge->belongs_to_pathways);
								}
								else if (direction == BACKWARD) {
									create_edge(newTargetNode, newSourceNode, currEdge->edge_type, currEdge->edge_subtype, currEdge->belongs_to_pathways);
								}
								else {
									cerr << "Invalid direction of search\n";
									assert(false);
								}
								****
								//create_edge(newSourceNode, newTargetNode, currEdge->edge_type, currEdge->edge_subtype, currEdge->belongs_to_pathway);

							}
						}
					}
				}
			}
		}
	}

	string graph_genesis_operation = "Bounded reach on ";
	list<unsigned int> operand_graphs;
	operand_graphs.push_back(g->graph_id);

	//	set<Node *> source_nodes_set, set<Node *> excluded_nodes_set,
	//					        int bound, int type_of_edge, int direction
	//

	set<Node *>::iterator set_it;
	string str_source_nodes = "Source Nodes: \n";
	for (set_it = source_nodes_set.begin(); set_it != source_nodes_set.end(); set_it++) {
		str_source_nodes = str_source_nodes + "\t" + (*set_it)->rep_display_name + "(" + (*set_it)->rep_id_name + ")\n";
	}

	string str_excluded_nodes = "Excluded Nodes: \n";
	for (set_it = excluded_nodes_set.begin(); set_it != excluded_nodes_set.end(); set_it++) {
		str_excluded_nodes = str_excluded_nodes + "\t" + (*set_it)->rep_display_name + "(" + (*set_it)->rep_id_name + ")\n";
	}

	string str_type_of_edge;

	if (type_of_edge == INHIBITION_EDGE)
		str_type_of_edge = "Inhibition";
	else if (type_of_edge == ACTIVATION_EDGE)
		str_type_of_edge = "Activation";
	else
		str_type_of_edge = "All";

	string str_direction;
	if (direction == FORWARD)
		str_direction = "Forward";
	else if (direction == BACKWARD)
		str_direction = "Backward";
	else
		str_direction = "Invalid";

	string other_params = str_source_nodes + "\n" + str_excluded_nodes + "\n" + "Bound: " + toString(bound) + "\n" + "Edge type: " + str_type_of_edge + "\n" + "Type of edge: " + str_direction + "\n";
	Graph * finalGraph = register_new_graph(resultGraph, graph_genesis_operation, operand_graphs, other_params);
	//	this->add_new_graph(resultGraph);
	//cout << "Size of result graph: " << resultGraph.size() << endl;
	//return resultGraph;
	return finalGraph;
}*/
// function
// map<string, Node *> GraphManager::freach(map<string, Node *> graph, string node_label) {
//	list<Node *> my_stack;
//	Node * next;
//	map<string, Node *> reachable;
//	map<string, Node *>::iterator graph_it = graph.begin();
//
//	if (graph.find(node_label) != graph.end()) {
//		Node * source = (*graph_it).second;
//		map<string, Node *>::iterator it = graph.find(source->rep_id_name);
//		my_stack.push_back((*it).second);
//		reachable.insert(pair<string ,Node *>((*it).first, (*it).second));
//	}
//
//	else {
//		return reachable;
//	}
//
//	while (!my_stack.empty()) {
//		next = my_stack.front();
//		my_stack.pop_front();
//
//		for (set<Edge *>::iterator next_it = next->outlist.begin(); next_it != next->outlist.end(); next_it++) {
//			my_stack.push_back((*next_it)->edge_target);
//			reachable.insert(pair<string ,Node *>((*next_it)->edge_target->rep_id_name, (*next_it)->edge_target));
//		}
//	}
//
//	return reachable;
//
// }

Graph *GraphManager::create_graph_from_xml(string xmlfile)
{

	XMLParser *my_parser = new XMLParser();
	map<string, Node *> new_graph_map = (*my_parser).createGraphFromXML(this, xmlfile);
	// map<string, Node *> new_graph_map = (*my_parser).createGraphFromXMLNew(this, xmlfile);
	if (new_graph_map.empty())
	{
		cout << "Error in GraphManager::createGraphFromXML : Could not create graph" << endl;
	}

	string graph_genesis_label = "Read " + xmlfile;
	list<unsigned int> empty_list;
	empty_list.empty();

	// considering .. harshit
	// flatten_id_name_map();
	// new_graph_map = merge_two_graphs(new_graph_map, new_graph_map, MERGE_MODE);
	// considering .. harshit
	Graph *new_graph = register_new_graph(new_graph_map, graph_genesis_label, empty_list, "");
	return new_graph;
}

// Graph * GraphManager::create_graph_from_sbml(string sbmlfile) {

// 	SBMLParser *my_sbml_parser = new SBMLParser();
// 	map<string, Node *> new_graph_map = (*my_sbml_parser).createGraphFromSBML(this, sbmlfile);
// //	if (new_graph.empty()) {
// //		cout << "Error in GraphManager::createGraphFromSBML : Could not create graph" << endl;
// //	}
// 	string graph_genesis_label = "Read " + sbmlfile;
// 	list<unsigned int> empty_list;
// 	empty_list.empty();
// 	Graph * new_graph = register_new_graph(new_graph_map, graph_genesis_label, empty_list, "");
// 	return new_graph;
// }

Graph *GraphManager::create_graph_from_txt(string filename)
{

	ifstream myfile(filename.c_str());
	string delim = "\t";
	string line;
	list<string> tokens, *pTokens;
	map<string, Node *> new_graph_map; // map_of_nodes in Graph class
	map<int, Node *> node_id_map;
	set<bvatom> edge_map;

	while (getline(myfile, line))
	{
		pTokens = this->get_list_of_names_from_string(line, delim);
		tokens = *pTokens;

		/* Create source node */
		string source_id_name = tokens.front();
		tokens.pop_front();
		string source_display_name = tokens.front();
		tokens.pop_front();

		// Erase invalid dot characters from id name
		char chars[] = ":";
		for (unsigned int i = 0; i < strlen(chars); ++i)
		{
			source_id_name.erase(std::remove(source_id_name.begin(), source_id_name.end(), chars[i]), source_id_name.end());
		}

		// Erase invalid dot characters from display name
		char chars2[] = "-.:'";
		for (unsigned int i = 0; i < strlen(chars2); ++i)
		{
			source_display_name.erase(std::remove(source_display_name.begin(), source_display_name.end(), chars2[i]), source_display_name.end());
		}

		Node *source_node;

		if (new_graph_map.find(source_id_name) == new_graph_map.end())
		{
			string type = "gene";
			// cout << "Creating node with hsa-id " << source_id_name << endl;
			source_node = create_node(source_id_name, source_display_name, type);
		}
		else
		{
			source_node = (new_graph_map.find(source_id_name))->second;
			// cout << "Found node with hsa-id " << source_id_name << endl;
		}
		new_graph_map.insert(pair<string, Node *>(source_id_name, source_node));
		add_to_local_to_hsa_node_id_map(source_node->node_id, source_node->rep_id_name);

		/* Create target node */
		string target_id_name = tokens.front();
		tokens.pop_front();
		string target_display_name = tokens.front();
		tokens.pop_front();

		// Erase invalid dot characters from id name
		// char chars[] = ":";
		for (unsigned int i = 0; i < strlen(chars); ++i)
		{
			target_id_name.erase(std::remove(target_id_name.begin(), target_id_name.end(), chars[i]), target_id_name.end());
		}

		// Erase invalid dot characters from display name
		// char chars2[] = "-.:'";
		for (unsigned int i = 0; i < strlen(chars2); ++i)
		{
			target_display_name.erase(std::remove(target_display_name.begin(), target_display_name.end(), chars2[i]), target_display_name.end());
		}
		Node *target_node;
		if (new_graph_map.find(target_id_name) == new_graph_map.end())
		{
			string type = "gene";
			// cout << "Creating node with hsa-id " << source_id_name << endl;
			target_node = create_node(target_id_name, target_display_name, type);
		}
		else
		{
			target_node = (new_graph_map.find(target_id_name))->second;
			// cout << "Found node with hsa-id " << source_id_name << endl;
		}
		new_graph_map.insert(pair<string, Node *>(target_id_name, target_node));
		add_to_local_to_hsa_node_id_map(target_node->node_id, target_node->rep_id_name);

		delete pTokens;

		/* Create edge between source and target */
		Edge *new_edge, *new_edge_back;
		/* Following values set for Human PPI data */
		string type = "PPrel";
		list<string> subtype;
		subtype.push_back("PPin");
		string p_name = "path:HumanPPI";
		list<string> pathways;
		pathways.push_back(p_name);
		bvatom to_check_edge_id = create_edge_id(source_node, target_node, type, subtype, pathways);

		// set<string> edge_set = gm->get_all_edges_from_node_map(new_graph);

		// if (edge_set.find(edge_id) == edge_set.end()) {

		if (edge_map.find(to_check_edge_id) == edge_map.end())
		{
			// cout << "Creating edge " << source_node->rep_id_name << " - " << target_node->rep_id_name << endl;
			new_edge = create_edge(source_node, target_node, type, subtype, pathways);
			// edge_map.insert(pair<unsigned int, char>(new_edge->edge_id, 'a'));
			edge_map.insert(new_edge->edge_id);

			// Modified by Harshit to make it behave like undirected
			// since the edge is an undirected edge so must be present both ways, and there is no real source and target out here
			new_edge_back = create_edge(target_node, source_node, type, subtype, pathways);
			source_node->inlist.insert(new_edge_back);
			target_node->outlist.insert(new_edge_back);
		}
	}

	if (new_graph_map.empty())
	{
		cout << "Error in GraphManager::create_graph_from_txt : Could not create graph" << endl;
	}

	string graph_genesis_label = "Read " + filename;
	list<unsigned int> empty_list;
	empty_list.empty();
	Graph *new_graph = register_new_graph(new_graph_map, graph_genesis_label, empty_list, "");
	return new_graph;
}

Graph *GraphManager::create_graph_edge_removed(Graph *input_graph, std::set<Edge *> &edges_to_remove)
{
	set<Edge *> edges_input_graph = get_all_edges_from_node_map(input_graph->rep_id_to_node_map);
	map<string, Node *> *node_map_output_graph = new map<string, Node *>;

	// create node_map with required edges excluded
	for (set<Edge *>::iterator itr = edges_input_graph.begin(); itr != edges_input_graph.end(); ++itr)
	{
		Node *source_node = NULL;
		Node *target_node = NULL;

		// creating and inserting source node and target node for the current edge into the node map
		map<string, Node *>::iterator itr_map;
		if ((itr_map = node_map_output_graph->find((*itr)->edge_source->rep_id_name)) != node_map_output_graph->end())
		{
			source_node = itr_map->second;
		}
		else
		{
			source_node = new Node((*itr)->edge_source->rep_id_name, (*itr)->edge_source->rep_display_name, (*itr)->edge_source->components, (*itr)->edge_source->node_type, (*itr)->edge_source->node_link);
			node_map_output_graph->insert(pair<string, Node *>((*itr)->edge_source->rep_id_name, source_node));
		}

		if ((itr_map = node_map_output_graph->find((*itr)->edge_target->rep_id_name)) != node_map_output_graph->end())
		{
			target_node = itr_map->second;
		}
		else
		{
			target_node = new Node((*itr)->edge_target->rep_id_name, (*itr)->edge_target->rep_display_name, (*itr)->edge_target->components, (*itr)->edge_target->node_type, (*itr)->edge_target->node_link);
			node_map_output_graph->insert(pair<string, Node *>((*itr)->edge_target->rep_id_name, target_node));
		}

		bool curr_edge_include = true;

		// case for current edge found in the list of edge to be removed
		for (set<Edge *>::iterator itr_remov = edges_to_remove.begin(); itr_remov != edges_to_remove.end(); ++itr_remov)
		{
			if (bvm->checkBVEquality((*itr)->edge_id, (*itr_remov)->edge_id))
			{
				curr_edge_include = false;
				break;
			}
		}

		if (curr_edge_include)
		{
			Edge *edge_to_insert = create_edge(source_node, target_node, (*itr)->edge_type, (*itr)->edge_subtype, (*itr)->belongs_to_pathways);
			source_node->outlist.insert(edge_to_insert);
			target_node->inlist.insert(edge_to_insert);
		}
	}

	string graph_genesis_operation = "remove edges from graph";
	list<unsigned int> operand_graphs_list(input_graph->graph_id);
	string other_params = "";
	Graph *output_graph = register_new_graph(*node_map_output_graph, graph_genesis_operation, operand_graphs_list, other_params);
	// cout << "returning graph" << endl;
	return output_graph;
}

Graph *GraphManager::create_graph_edge_retained(Graph *input_graph, std::set<Edge *> &edges_to_remove)
{
	set<Edge *> edges_input_graph = get_all_edges_from_node_map(input_graph->rep_id_to_node_map);
	map<string, Node *> *node_map_output_graph = new map<string, Node *>;

	// create node_map with only the required edges included
	for (set<Edge *>::iterator itr = edges_input_graph.begin(); itr != edges_input_graph.end(); ++itr)
	{
		bool curr_edge_include = false;

		// case for current edge found in the list of edges to be included
		for (set<Edge *>::iterator itr_remov = edges_to_remove.begin(); itr_remov != edges_to_remove.end(); ++itr_remov)
		{
			if (bvm->checkBVEquality((*itr)->edge_id, (*itr_remov)->edge_id))
			{
				curr_edge_include = true;
				break;
			}
		}

		if (curr_edge_include)
		{
			Node *source_node = NULL;
			Node *target_node = NULL;

			// creating and inserting source node and target node for the current edge into the node map
			map<string, Node *>::iterator itr_map;
			if ((itr_map = node_map_output_graph->find((*itr)->edge_source->rep_id_name)) != node_map_output_graph->end())
			{
				source_node = itr_map->second;
			}
			else
			{
				source_node = new Node((*itr)->edge_source->rep_id_name, (*itr)->edge_source->rep_display_name, (*itr)->edge_source->components, (*itr)->edge_source->node_type, (*itr)->edge_source->node_link);
				node_map_output_graph->insert(pair<string, Node *>((*itr)->edge_source->rep_id_name, source_node));
			}

			if ((itr_map = node_map_output_graph->find((*itr)->edge_target->rep_id_name)) != node_map_output_graph->end())
			{
				target_node = itr_map->second;
			}
			else
			{
				target_node = new Node((*itr)->edge_target->rep_id_name, (*itr)->edge_target->rep_display_name, (*itr)->edge_target->components, (*itr)->edge_target->node_type, (*itr)->edge_target->node_link);
				node_map_output_graph->insert(pair<string, Node *>((*itr)->edge_target->rep_id_name, target_node));
			}

			Edge *edge_to_insert = create_edge(source_node, target_node, (*itr)->edge_type, (*itr)->edge_subtype, (*itr)->belongs_to_pathways);
			source_node->outlist.insert(edge_to_insert);
			target_node->inlist.insert(edge_to_insert);
		}
	}

	string graph_genesis_operation = "retain only these edges from graph";
	list<unsigned int> operand_graphs_list(input_graph->graph_id);
	string other_params = "";
	Graph *output_graph = register_new_graph(*node_map_output_graph, graph_genesis_operation, operand_graphs_list, other_params);
	cout << "returning graph" << endl;
	return output_graph;
}

void GraphManager::print_genesis(int graphnum, set<int> &graphs_printed)
{

	Graph *g = get_graph_from_index(graphnum);

	cout << "Graph " << graphnum << ": " << g->genesis.oper << " ";

	list<unsigned int> oper_graphs = g->genesis.oper_graph_ids;
	for (list<unsigned int>::iterator gen_it = oper_graphs.begin(); gen_it != oper_graphs.end(); gen_it++)
	{
		cout << " " << *gen_it;
	}

	cout << " " << g->genesis.oper_parameters << "\n";
	graphs_printed.insert(graphnum);

	for (list<unsigned int>::iterator gen_it = oper_graphs.begin(); gen_it != oper_graphs.end(); gen_it++)
	{
		if (graphs_printed.find(*gen_it) == graphs_printed.end())
		{
			print_genesis(*gen_it, graphs_printed);
		}
	}
}

void GraphManager::write_node_in_dot(ofstream &myfile, Node *node, map<string, colorTriple> &colorMap, map<string, char> &alreadySeenNode)
{
	if (node->node_type == "compound")
	{
		myfile << node->rep_id_name << " [shape = circle, penwidth=2, label = \"" << node->rep_display_name + "\\n(" + node->rep_id_name + ")"
			   << "\"";
	}
	else if (node->node_type == "group")
	{
		myfile << node->rep_id_name << " [shape = hexagon, penwidth=2, peripheries=2, label = \"" << node->rep_display_name + "\\n(" + node->rep_id_name + ")"
			   << "\"";
	}
	else
	{
		myfile << node->rep_id_name << " [shape = box, penwidth=2, label = \"" << node->rep_display_name + "\\n(" + node->rep_id_name + ")"
			   << "\"";
	}
	if (colorMap.find(node->rep_id_name) != colorMap.end())
	{
		myfile << ", style = filled, color = \"#";
		colorTriple color = colorMap[node->rep_id_name];
		if (color.red < 16)
		{
			myfile << "0";
		}
		myfile << hex << color.red;
		if (color.green < 16)
		{
			myfile << "0";
		}
		myfile << hex << color.green;
		if (color.blue < 16)
		{
			myfile << "0";
		}
		myfile << hex << color.blue;
		myfile << "\"";
	}
	myfile << "];\n";
	alreadySeenNode.insert(pair<string, char>(node->rep_id_name, 'a'));
}

bool GraphManager::write_graph_as_dot(Graph *g, map<string, colorTriple> &colorMap, string dotFileName)
{
	map<string, Node *> graph = g->rep_id_to_node_map;
	ofstream myfile;
	myfile.open(dotFileName.c_str());
	myfile << "Digraph G  {\n";

	// myfile << "Graph" << g->graph_id << " [shape = box, label = \"" << "Graph " << g->graph_id << "\"";
	myfile << "label = \"Graph " << g->graph_id << "\";\n";
	myfile << "labelloc = top;\n";
	myfile << "labeljust = left;\n";
	myfile << "labelfontsize = 20.0;\n";

	myfile << "edge [style=\"solid\"];\n";

	map<string, char> alreadySeenNode;

	set<bvatom> alreadySeenEdge;
	for (map<string, Node *>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		Node *node = (*it).second;
		if (alreadySeenNode.find(node->rep_id_name) == alreadySeenNode.end())
		{
			write_node_in_dot(myfile, node, colorMap, alreadySeenNode);
		}

		if (!(node->outlist.empty()))
		{

			for (set<Edge *>::iterator list_it = node->outlist.begin(); list_it != node->outlist.end(); list_it++)
			{
				if (alreadySeenEdge.find((*list_it)->edge_id) == alreadySeenEdge.end())
				{

					Node *target = (*list_it)->edge_target;

					if (alreadySeenNode.find(target->rep_id_name) == alreadySeenNode.end())
					{
						write_node_in_dot(myfile, target, colorMap, alreadySeenNode);
					}

					myfile << "\t" << node->rep_id_name;
					myfile << " -> ";
					myfile << (*list_it)->edge_target->rep_id_name;

					alreadySeenEdge.insert((*list_it)->edge_id);

					myfile << "[";

					string edge_type = (*list_it)->edge_type;
					list<string> subtype_list = (*list_it)->edge_subtype;

					string arrowhead, style, label;

					for (list<string>::iterator subtype_list_it = subtype_list.begin(); subtype_list_it != subtype_list.end(); subtype_list_it++)
					{
						if (*subtype_list_it == "binding/association" || *subtype_list_it == "dissociation")
						{
							arrowhead = "none";
							style = "bold";

							if (*subtype_list_it == "binding/association")
							{
								label += "assoc\\n";
							}
							if (*subtype_list_it == "dissociation")
							{
								label += "dissoc\\n";
							}
							//
						}

						if (*subtype_list_it == "inhibition")
						{
							arrowhead = "tee";
							label += "inhb\\n";
						}

						if (*subtype_list_it == "repression")
						{
							arrowhead = "tee";
							label += "repr\\n";
						}

						if (*subtype_list_it == "activation")
						{
							label += "actv\\n";
						}

						if (*subtype_list_it == "expression")
						{
							label += "expr\\n";
						}

						if (*subtype_list_it == "phosphorylation")
						{
							std::size_t found = label.find("-p\\n");
							if (found != std::string::npos)
							{
								cerr << "Conflicting edge labels phosphorylation and dephosphorylation in edge " << node->rep_id_name << " - " << target->rep_id_name << endl;
								return false;
							}

							else
							{
								label = label + "+p\\n";
							}
						}
						if (*subtype_list_it == "dephosphorylation")
						{
							std::size_t found = label.find("+p");
							if (found != std::string::npos)
							{
								cerr << "Conflicting edge labels phosphorylation and dephosphorylation in edge " << node->rep_id_name << " - " << target->rep_id_name << endl;
								return false;
							}

							else
							{
								label = label + "-p\\n";
							}
						}

						if (*subtype_list_it == "indirect effect")
						{
							style = "dashed";
						}
					}

					if (edge_type == "PPrel")
					{
						label = label + "PPrel\\n";
					}
					if (edge_type == "GErel")
					{
						label = label + "GErel\\n";
					}
					if (edge_type == "PCrel")
					{
						label = label + "PCrel\\n";
					}
					if (edge_type == "ECrel")
					{
						label = label + "ECrel\\n";
					}

					list<string> pathway_list = (*list_it)->belongs_to_pathways;
					for (list<string>::iterator pathway_list_it = pathway_list.begin(); pathway_list_it != pathway_list.end(); pathway_list_it++)
					{
						label += " " + (*pathway_list_it) + "\\n";
					}

					myfile << "arrowhead=\"" << arrowhead << "\", style=\"" << style << "\", label=\"" << label << "\", ";

					myfile << "arrowsize=1, penwidth = 2]";

					myfile << ";"
						   << "\n";
				}
			}
		}
	}
	myfile << "}";
	myfile.close();
	return true;
}
//
//
// bvatom GraphManager::get_edge_id_with_masked_pathway(bvatom full_edge_id) {
//    bvatom masked_edge_id = bvm->
//    get_field_as_bv_from_edge_id(bvatom edge_itr, string field);
//}

bvatom GraphManager::get_field_as_bv_from_edge_id(bvatom edge_itr, string field)
{
	unsigned long offset_lo;
	unsigned long offset_hi;

	// Get offset values for each field
	// Note: Offsets are from the right of the bitvector
	if (field == "source_node_type")
	{
		offset_lo = PATHWAY_ID_LENGTH + EDGE_SUBTYPE_LENGTH + EDGE_TYPE_LENGTH + NODE_ID_LENGTH + NODE_TYPE_LENGTH + NODE_ID_LENGTH;
		offset_hi = offset_lo + NODE_TYPE_LENGTH - 1;
	}
	else if (field == "source_node_id")
	{
		offset_lo = PATHWAY_ID_LENGTH + EDGE_SUBTYPE_LENGTH + EDGE_TYPE_LENGTH + NODE_ID_LENGTH + NODE_TYPE_LENGTH;
		offset_hi = offset_lo + NODE_ID_LENGTH - 1;
	}
	else if (field == "target_node_type")
	{
		offset_lo = PATHWAY_ID_LENGTH + EDGE_SUBTYPE_LENGTH + EDGE_TYPE_LENGTH + NODE_ID_LENGTH;
		offset_hi = offset_lo + NODE_TYPE_LENGTH - 1;
	}
	else if (field == "target_node_id")
	{
		offset_lo = PATHWAY_ID_LENGTH + EDGE_SUBTYPE_LENGTH + EDGE_TYPE_LENGTH;
		offset_hi = offset_lo + NODE_ID_LENGTH - 1;
	}
	else if (field == "edge_type")
	{
		offset_lo = PATHWAY_ID_LENGTH + EDGE_SUBTYPE_LENGTH;
		offset_hi = offset_lo + EDGE_TYPE_LENGTH - 1;
	}
	else if (field == "edge_subtype")
	{
		offset_lo = PATHWAY_ID_LENGTH;
		offset_hi = offset_lo + EDGE_SUBTYPE_LENGTH - 1;
	}
	else if (field == "pathway_id")
	{
		offset_lo = (unsigned long)0;
		offset_hi = offset_lo + PATHWAY_ID_LENGTH - 1;
	}
	else
	{
		offset_lo = (unsigned long)0;
		offset_hi = (unsigned long)0;
		cout << "In GraphManager::get_field_as_bv_from_edge_id. Going to select empty bit vector" << endl;
	}
	bvatom value_bv = bvm->select_with_int(edge_itr, offset_hi, offset_lo);
	return value_bv;
}

// returns the bvatom for a particular edge subtype given as string
bvatom GraphManager::get_bvatom_for_edge_subtype(string subtype_str)
{																																																		// subtype_str should be "activation", "inhibition" etc
	bvatom edge_subtype_bv = bvm->getZeroedBVatom(EDGE_SUBTYPE_LENGTH);																																	// bvatom with all zeros
	string one_str = toString(1);																																										// string with just 1
	bvatom singleton_bv = bvm->getBVatom(1, one_str);																																					// convert string to bvatom
	bvm->replaceBySelBVatom(singleton_bv, edge_subtype_bv, static_cast<unsigned long>(0), static_cast<unsigned long>(edgeSubtypeEncodingMap.find(subtype_str)->second), static_cast<unsigned long>(1)); // converting all zeroed edge_subtype_active_bv into a bvatom that corresponds to subtype_str
	return edge_subtype_bv;
}

// returns the bvatom for multiple edge subtypes given as list of strings
bvatom GraphManager::get_bvatom_for_edge_subtype_list(const list<string> &edge_subtype_list)
{
	if (edge_subtype_list.empty())
	{
		return bvm->getZeroedBVatom(EDGE_SUBTYPE_LENGTH);
	}

	list<string>::const_iterator list_itr = edge_subtype_list.begin();

	// make bvatom for the first element in list
	bvatom edge_subtype_bv = bvm->getZeroedBVatom(EDGE_SUBTYPE_LENGTH); // bvatom with all zeros
	string one_str = toString(1);										// string with just 1
	bvatom singleton_bv = bvm->getBVatom(1, one_str);					// convert string to bvatom
	bvm->replaceBySelBVatom(singleton_bv, edge_subtype_bv, static_cast<unsigned long>(0), static_cast<unsigned long>(edgeSubtypeEncodingMap.find(*list_itr)->second), static_cast<unsigned long>(1));
	++list_itr;

	// keep oring the bvatoms from the second element in the list onwards till the end
	while (list_itr != edge_subtype_list.end())
	{
		bvatom edge_subtype_temp_bv = bvm->getZeroedBVatom(EDGE_SUBTYPE_LENGTH); // bvatom with all zeros
		string one_str = toString(1);											 // string with just 1
		bvatom singleton_bv = bvm->getBVatom(1, one_str);						 // convert string to bvatom
		bvm->replaceBySelBVatom(singleton_bv, edge_subtype_temp_bv, static_cast<unsigned long>(0), static_cast<unsigned long>(edgeSubtypeEncodingMap.find(*list_itr)->second), static_cast<unsigned long>(1));
		edge_subtype_bv = bvm->bitwise_or(edge_subtype_bv, edge_subtype_temp_bv); // oring here
		++list_itr;
	}

	return edge_subtype_bv;
}

// returns the bvatom for a particular edge type given the edge type as string --- Harshit
bvatom GraphManager::get_bvatom_for_edge_type(string type_str)
{
	string edge_type_value = uint_to_bin((edgeTypeEncodingMap.find(type_str))->second);
	bvatom edge_type_bv = bvm->getBVatom(EDGE_TYPE_LENGTH, edge_type_value);
	return edge_type_bv;
}

// returns the bvatom for a particular node type given the node type as string --- Harshit
bvatom GraphManager::get_bvatom_for_node_type(string node_type)
{
	string node_type_binary = uint_to_bin((nodeTypeEncodingMap.find(node_type))->second);
	bvatom source_type_bv = bvm->getBVatom(NODE_TYPE_LENGTH, node_type_binary);
	return source_type_bv;
}

bvatom GraphManager::get_bvatom_for_node_id(Node *node)
{
	string id = uint_to_bin(node->node_id);
	return bvm->getBVatom(NODE_ID_LENGTH, id); // convert string to bvatom
}

// bvatom returned for a particular edge type where edge_t is the edge type in rules_parser
bvatom GraphManager::get_bvatom_for_parser_edge_t(edge_t edge_type)
{
	// for edge types
	if (edge_type == PPREL_EDGE)
	{
		return get_bvatom_for_edge_type("PPrel");
	}
	else if (edge_type == ECREL_EDGE)
	{
		return get_bvatom_for_edge_type("ECrel");
	}
	else if (edge_type == GEREL_EDGE)
	{
		return get_bvatom_for_edge_type("GErel");
	}
	else if (edge_type == PCREL_EDGE)
	{
		return get_bvatom_for_edge_type("PCrel");
	}
	else if (edge_type == NONE_EDGE)
	{
		cerr << "NONE_EDGE type can't be converted to bvatom, exiting . . ." << endl;
		exit(1);
	}
	else
	{
		cerr << "Error: unknown edge type, exiting . . ." << endl;
		exit(1);
	}
}

// bvatom returned for a particular edge subtype where edge_subt is the edge type in rules_parser
bvatom GraphManager::get_bvatom_for_parser_edge_subt(edge_subt edge_subtype)
{
	// for edge subtypes
	if (edge_subtype == ACTIVE_EDGE)
	{
		return get_bvatom_for_edge_subtype("activation");
	}
	else if (edge_subtype == INHIBIT_EDGE)
	{
		return get_bvatom_for_edge_subtype("inhibition");
	}
	else if (edge_subtype == EXPRESS_EDGE)
	{
		return get_bvatom_for_edge_subtype("expression");
	}
	else if (edge_subtype == REPRESS_EDGE)
	{
		return get_bvatom_for_edge_subtype("repression");
	}
	else if (edge_subtype == INDIRECT_EDGE)
	{
		return get_bvatom_for_edge_subtype("indirect effect");
	}
	else if (edge_subtype == STATE_CHANGE_EDGE)
	{
		return get_bvatom_for_edge_subtype("state change");
	}
	else if (edge_subtype == ASSOC_EDGE)
	{
		return get_bvatom_for_edge_subtype("binding/association");
	}
	else if (edge_subtype == DISSOC_EDGE)
	{
		return get_bvatom_for_edge_subtype("dissociation");
	}
	else if (edge_subtype == MISSING_INTERACTION_EDGE)
	{
		return get_bvatom_for_edge_subtype("missing interaction");
	}
	else if (edge_subtype == NONE_EDGE_SUB)
	{
		cerr << "NONE_EDGE_SUB subtype can't be converted to bvatom, exiting . . ." << endl;
		exit(1);
	}
	else
	{
		cerr << "Error: unknown edge type, exiting . . ." << endl;
		exit(1);
	}
}

// bvatom for list of edge subtypes where list_edge_subtypes is the list of edge subtypes in rules_parser
bvatom GraphManager::get_bvatom_for_parser_edge_subtype_list(list_edge_subtypes_t *list_edge_subtypes)
{
	assert(list_edge_subtypes); // can't be null

	// get bvatom for  first element in the list
	bvatom edge_subt_list_bv = get_bvatom_for_parser_edge_subt(list_edge_subtypes->edge_subtype_data);
	list_edge_subtypes = list_edge_subtypes->next;

	// keep oring rest of the elements in the list
	while (list_edge_subtypes)
	{
		bvatom temp_bv = get_bvatom_for_parser_edge_subt(list_edge_subtypes->edge_subtype_data);
		edge_subt_list_bv = bvm->bitwise_or(edge_subt_list_bv, temp_bv);
		list_edge_subtypes = list_edge_subtypes->next;
	}
	return edge_subt_list_bv;
}

string GraphManager::get_node_type_str_from_node_type_parser(node_t node_type)
{
	if (node_type == GENE_NODE)
	{
		return string("gene");
	}
	else if (node_type == GROUP_NODE)
	{
		return string("group");
	}
	else if (node_type == COMPOUND_NODE)
	{
		return string("compound");
	}
	else if (node_type == NONE_NODE)
	{
		cerr << "Can't convert NONE_NODE to string, exiting . . ." << endl;
		exit(1);
	}
	else
	{
		cerr << "Unknown node type, exiting . . ." << endl;
		exit(1);
	}
}

// return edge subtype as string on passing bvatom (useful in edge subtype string inclusion in edge presence variable)
string GraphManager::get_edge_subtype_string_from_bvatom(bvatom edge_subtype)
{
	// iterate over each bit position in the bvatom from second most right to leftmost and if it is set to one get the corresponding
	// string from the map and append all these strings and return the final string that contains all edge subtype information
	string edge_subtype_str = "";
	// cout << bvm->printBVasBinString(edge_subtype) << endl;

	// if edge_subtype is zero then pick the string corresponding to zero from the map
	// all bits zero is undefined in the map
	if (bvm->checkBVForZero(edge_subtype))
	{
		for (map<string, int>::iterator itr = edgeSubtypeEncodingMap.begin(); itr != edgeSubtypeEncodingMap.end(); ++itr)
		{
			if (itr->second == 0)
			{
				edge_subtype_str += get_string_prefix(get_string_prefix(itr->first, ' '), '/') + "-";
				break;
			}
		}
		return edge_subtype_str;
	}

	for (int i = 1; i < EDGE_SUBTYPE_LENGTH; ++i)
	{
		bvatom mask_bv = bvm->getZeroedBVatom(EDGE_SUBTYPE_LENGTH); // create all zero bvatom of length EDGE_SUBTYPE
		string one_str = toString(1);
		bvatom singleton_bv = bvm->getBVatom(1, one_str);
		bvm->replaceBySelBVatom(singleton_bv, mask_bv, 0, i, 1); // ith index bit from right set to 1, others are zero

		// cout << bvm->printBVasBinString(mask_bv) << endl;
		if (!bvm->checkBVForZero(bvm->bitwise_and(edge_subtype, mask_bv)))
		{
			for (map<string, int>::iterator itr = edgeSubtypeEncodingMap.begin(); itr != edgeSubtypeEncodingMap.end(); ++itr)
			{
				if (itr->second == i)
				{
					edge_subtype_str += get_string_prefix(get_string_prefix(itr->first, ' '), '/') + "-";
				}
			}
		}
	}
	// cout << endl;
	if (!edge_subtype_str.empty())
		return edge_subtype_str;

	cerr << "no string edgetype exists for the bvatom edgesubtype, exiting... " << endl;
	exit(1);
}

/*
bool GraphManager::listHasString(listOfStrings* value_list, const char* attr_value){
	listOfStrings* temp_ptr = value_list;
	while(temp_ptr != NULL){
		if(strcmp(attr_value, temp_ptr->strng) == 0)
			return true;
		temp_ptr = temp_ptr->next;
	}

	return false;
}
*/

/*
bool GraphManager::rulesHaveAttribute(t_list_of_t_attr_content* list_of_attr, t_attr_name attribute, const char* attr_value){
	t_list_of_t_attr_content* temp_ptr = list_of_attr;
	while(temp_ptr != NULL){
		if(temp_ptr->attribute_content->attribute == attribute && listHasString(temp_ptr->attribute_content->value_list, attr_value))
						return true;
		temp_ptr = temp_ptr->next;
	}

	return false;
}
*/

/*
bool GraphManager::list_preconditions_has(t_list_of_t_precondition* list_of_preconds, const char* precond_member) {
	t_list_of_t_precondition* temp_ptr = list_of_preconds;

	while(temp_ptr != NULL){
		if(list_operands_has(temp_ptr->precondition->list_of_operands, precond_member))
			return true;
		temp_ptr = temp_ptr->next;
	}

	return false;
}
*/

/*
bool GraphManager::list_operands_has(t_list_of_operands* list_of_operands, const char* list_of_operands_member){
	t_list_of_operands* temp_ptr = list_of_operands;
	while(temp_ptr != NULL){
		if(strcmp(temp_ptr->operand->name, list_of_operands_member) == 0){
			return true;
	}
		temp_ptr = temp_ptr->next;
	}

	return false;
}
*/

/*
bool GraphManager::rulesHaveAction(t_list_of_t_action* list_of_actns, const char* precondition_member  ,t_oper_name opertr, const char* action_list_of_operands_member){
	t_list_of_t_action* temp_ptr = list_of_actns;
	while(temp_ptr != NULL){
		if(temp_ptr->action->operator_name == opertr && list_preconditions_has(temp_ptr->action->list_of_preconditions, precondition_member) && list_operands_has(temp_ptr->action->list_of_operands ,action_list_of_operands_member))
			return true;
		temp_ptr = temp_ptr->next;
	}

	return false;
}
*/

void GraphManager::update_activation_definitions_simple_PPrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter)
{
	// string curr_defintion = "\n(=> " + edge_smt_var + " (=> " + source_id_smt_var + " " + target_id_smt_var  + ")) \n";
	/*	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + target_id_smt_var  + ")) \n";
		definitions += curr_defintion;
		if(definition_counter > 0){
			definitions += " ) ";
			definitions.insert(0, " ( and ");
		}
		++definition_counter;*/

	// actual defn
	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + target_id_smt_var + "))\n";

	// trial defn
	// string curr_defintion = "\n( xor ( and " + target_id_smt_var + " ( xor ( and " + edge_smt_var + " " + source_id_smt_var + " ) ( and ( not  " + edge_smt_var + " ) ( not " + source_id_smt_var + " ) ) ) ) ( and ( not " + target_id_smt_var  + " ) ( not " + edge_smt_var + " ) ( not " + source_id_smt_var + " ) ) )\n";
	// string curr_defintion = "\n( xor ( and " + target_id_smt_var + " " + edge_smt_var + " " + source_id_smt_var + " ) ( and ( not " + target_id_smt_var  + " ) ( not " + edge_smt_var + " ) ( not " + source_id_smt_var + " ) ) )\n";

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_inhibition_definitions_simple_PPrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter)
{
	// string curr_defintion = "\n(=> " + edge_smt_var + " (=> " + source_id_smt_var + " " + " (not " + target_id_smt_var  + "))) \n";
	/*string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + " (not " + target_id_smt_var  + "))) \n";
	definitions += curr_defintion;
	if(definition_counter > 0){
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	++definition_counter;*/

	// actual defn
	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + " (not " + target_id_smt_var + "))) \n";

	// trial defn
	// string curr_defintion = "\n( xor ( and ( not " + target_id_smt_var + " ) ( xor ( and " + edge_smt_var + " " + source_id_smt_var + " ) ( and ( not  " + edge_smt_var + " ) ( not " + source_id_smt_var + " ) ) ) ) ( and " + target_id_smt_var  + " ( not " + edge_smt_var + " ) ( not " + source_id_smt_var + " ) ) )\n";
	// string curr_defintion = "\n( xor ( and ( not " + target_id_smt_var + " ) " + edge_smt_var + " " + source_id_smt_var + " ) ( and " + target_id_smt_var  + " ( not " + edge_smt_var + " ) ( not " + source_id_smt_var + " ) ) )\n";

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_expression_definitions(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter)
{
	// very old with vacuous edges as well
	// string curr_defintion = "\n(=> " + edge_smt_var + " (=> " + source_id_smt_var + " " + target_id_smt_var  + ")) \n";

	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + target_id_smt_var + ")) \n";

	/*definitions += curr_defintion;
	if(definition_counter > 0){
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	++definition_counter;*/

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_repression_definitions(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter)
{
	// very old with vacuous edges as well
	// string curr_defintion = "\n(=> " + edge_smt_var + " (=> " + source_id_smt_var + " " + " (not " + target_id_smt_var  + "))) \n";

	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + " (not " + target_id_smt_var + "))) \n";

	/*definitions += curr_defintion;
	if(definition_counter > 0){
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	++definition_counter;*/

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_activation_definitions_simple_PCrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter)
{
	// very old with vacuous edges as well
	// string curr_defintion = "\n(=> " + edge_smt_var + " (=> " + source_id_smt_var + " " + target_id_smt_var  + ")) \n";

	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + target_id_smt_var + ")) \n";

	/*definitions += curr_defintion;
	if(definition_counter > 0){
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	++definition_counter;*/

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_inhibition_definitions_simple_PCrel(string &definitions, const string &edge_smt_var, const string &source_id_smt_var, const string &target_id_smt_var, int &definition_counter)
{
	// very old with vacuous edges as well
	// string curr_defintion = "\n(=> " + edge_smt_var + " (=> " + source_id_smt_var + " " + " (not " + target_id_smt_var  + "))) \n";

	string curr_defintion = "\n(=> " + edge_smt_var + " (and " + source_id_smt_var + " " + " (not " + target_id_smt_var + "))) \n";

	/*definitions += curr_defintion;
	if(definition_counter > 0){
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	++definition_counter;*/

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_definitions_edgetype_neither_PPrel(string &definitions, const string &edge_z3_present, const string &edge_z3_active, const string &source_z3_active, const string &target_z3_active, int &definition_counter)
{
	string defintion_active_part = "\n(=> ( and " + edge_z3_present + " " + edge_z3_active + " )" + " (and " + source_z3_active + " " + target_z3_active + ")) ";

	// very old with vacuous edges as well
	// string defintion_active_part = "\n(=> ( and " + edge_z3_present + " " + edge_z3_active + " )" + " (=> " + source_z3_active + " " + target_z3_active  + ")) ";

	string defintion_inhibit_part = "\n(=> ( and " + edge_z3_present + "( not " + edge_z3_active + " ) )" + " (and " + source_z3_active + " " + " (not " + target_z3_active + "))) ";

	// very old with vacuous edges as well
	// string defintion_inhibit_part = "\n(=> ( and " + edge_z3_present + "( not " + edge_z3_active + " ) )" + " (=> " + source_z3_active + " " + " (not " + target_z3_active  + "))) ";

	string curr_defintion = "( or " + defintion_active_part + defintion_inhibit_part + " ) \n";

	// definitions += curr_defintion;
	/*	if(definition_counter > 0){
			definitions += " ) ";
			definitions.insert(0, " ( and ");
		}
		++definition_counter;*/

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::update_definitions_edgetype_neither_PCrel(string &definitions, const string &edge_z3_present, const string &edge_z3_active, const string &source_z3_active, const string &target_z3_active, int &definition_counter)
{
	string defintion_active_part = "\n(=> ( and " + edge_z3_present + " " + edge_z3_active + " )" + " (and " + source_z3_active + " " + target_z3_active + ")) ";
	// string defintion_active_part = "\n(=> ( and " + edge_z3_present + " " + edge_z3_active + " )" + " (=> " + source_z3_active + " " + target_z3_active  + ")) ";
	string defintion_inhibit_part = "\n(=> ( and " + edge_z3_present + "( not " + edge_z3_active + " ) )" + " (and " + source_z3_active + " " + " (not " + target_z3_active + "))) ";
	// string defintion_inhibit_part = "\n(=> ( and " + edge_z3_present + "( not " + edge_z3_active + " ) )" + " (=> " + source_z3_active + " " + " (not " + target_z3_active  + "))) ";

	string curr_defintion = "( xor " + defintion_active_part + defintion_inhibit_part + " ) \n";

	/*definitions += "( or " + defintion_active_part + defintion_inhibit_part + " ) \n";
	if(definition_counter > 0){
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	++definition_counter;*/

	if (definition_counter == 0)
	{
		definitions += curr_defintion;
	}
	if (definition_counter == 1)
	{
		definitions += curr_defintion;
		definitions += " ) ";
		definitions.insert(0, " ( and ");
	}
	if (definition_counter > 1)
	{
		definitions.insert(definitions.end() - 2, curr_defintion.begin(), curr_defintion.end());
	}
	++definition_counter;
}

void GraphManager::mark_potentially_conflicting_nodes(Graph *graph, ofstream &out_confl, const string &target_id_str, const bvatom &edge_subtype_bv, std::set<Node *> &set_potential_conflict_nodes)
{
	Node *ptr_target_node = get_node_by_name(graph, target_id_str);

	if (ptr_target_node != NULL && ptr_target_node->potential_activation_conflict == false)
	{
		for (set<Edge *>::iterator inlist_edge_itr = (ptr_target_node->inlist).begin(); inlist_edge_itr != (ptr_target_node->inlist).end(); inlist_edge_itr++)
		{
			bvatom inlist_edge_set_id = (*inlist_edge_itr)->edge_id;
			bvatom inlist_curr_edge_subtype_bv = get_field_as_bv_from_edge_id(inlist_edge_set_id, "edge_subtype");

			if (!bvm->checkBVForZero(bvm->bitwise_and(inlist_curr_edge_subtype_bv, edge_subtype_bv)))
			{
				out_confl << "conflicting node is " << ptr_target_node->rep_id_name << "\n"
						  << flush;
				ptr_target_node->potential_activation_conflict = true;
				set_potential_conflict_nodes.insert(ptr_target_node);
				break;
			}
		}
	}
}

void GraphManager::mark_potentially_conflicting_nodes_expression(Graph *graph, ofstream &out_confl, const string &target_id_str, const bvatom &edge_subtype_bv, std::set<Node *> &set_potential_conflict_nodes)
{
	Node *ptr_target_node = get_node_by_name(graph, target_id_str);

	if (ptr_target_node != NULL && ptr_target_node->potential_activation_conflict == false)
	{
		for (set<Edge *>::iterator inlist_edge_itr = (ptr_target_node->inlist).begin(); inlist_edge_itr != (ptr_target_node->inlist).end(); inlist_edge_itr++)
		{
			bvatom inlist_edge_set_id = (*inlist_edge_itr)->edge_id;
			bvatom inlist_curr_edge_subtype_bv = get_field_as_bv_from_edge_id(inlist_edge_set_id, "edge_subtype");

			if (!bvm->checkBVForZero(bvm->bitwise_and(inlist_curr_edge_subtype_bv, edge_subtype_bv)))
			{
				out_confl << "conflicting node is " << ptr_target_node->rep_id_name << "\n"
						  << flush;
				ptr_target_node->potential_expression_conflict = true;
				set_potential_conflict_nodes.insert(ptr_target_node);
				break;
			}
		}
	}
}

// handles PPrel and ECrel cases for z3 variable declarations
void GraphManager::handle_edge_active_unknown(set<Edge *>::iterator edge_itr, ofstream &out)
{
	set<Edge *> target_node_adj_edges((*edge_itr)->edge_target->inlist);
	target_node_adj_edges.insert((*edge_itr)->edge_target->outlist.begin(), (*edge_itr)->edge_target->outlist.end());

	bool target_node_has_assoc_dissoc_edge = false;
	bvatom current_edgeid = (*edge_itr)->edge_id;
	set<Edge *>::iterator edge_set_itr;
	for (edge_set_itr = target_node_adj_edges.begin(); edge_set_itr != target_node_adj_edges.end(); edge_set_itr++)
	{
		bvatom edge_consider_id = (*edge_set_itr)->edge_id;
		if (!bvm->checkBVEquality(current_edgeid, edge_consider_id))
		{
			list<string>::iterator itr_adj_edges_begin = (*edge_set_itr)->edge_subtype.begin();
			list<string>::iterator itr_adj_edges_end = (*edge_set_itr)->edge_subtype.end();
			if (find(itr_adj_edges_begin, itr_adj_edges_end, "binding/association") != itr_adj_edges_end || find(itr_adj_edges_begin, itr_adj_edges_end, "dissociation") != itr_adj_edges_end)
				target_node_has_assoc_dissoc_edge = true;
		}
	}

	list<string>::iterator itr_ls_begin = (*edge_itr)->edge_subtype.begin();
	list<string>::iterator itr_ls_end = (*edge_itr)->edge_subtype.end();

	if (((*edge_itr)->edge_type == "PPrel" || (*edge_itr)->edge_type == "ECrel") &&
		!target_node_has_assoc_dissoc_edge &&
		find(itr_ls_begin, itr_ls_end, "binding/association") == itr_ls_end &&
		find(itr_ls_begin, itr_ls_end, "dissociation") == itr_ls_end &&
		find(itr_ls_begin, itr_ls_end, "activation") == itr_ls_end &&
		find(itr_ls_begin, itr_ls_end, "inhibition") == itr_ls_end &&
		find(itr_ls_begin, itr_ls_end, "state change") == itr_ls_end &&
		(find(itr_ls_begin, itr_ls_end, "phosphorylation") != itr_ls_end ||
		 find(itr_ls_begin, itr_ls_end, "dephosphorylation") != itr_ls_end ||
		 find(itr_ls_begin, itr_ls_end, "glycosylation") != itr_ls_end ||
		 find(itr_ls_begin, itr_ls_end, "ubiquitination") != itr_ls_end ||
		 find(itr_ls_begin, itr_ls_end, "methylation") != itr_ls_end ||
		 find(itr_ls_begin, itr_ls_end, "indirect effect") != itr_ls_end))
	{

		out << "(declare-const "
			<< "edge_" << (*edge_itr)->edge_source->rep_id_name + "to" + (*edge_itr)->edge_target->rep_id_name + "_active Bool)\n"
			<< flush;
	}
}

void GraphManager::writeConstraintDeclarations(ofstream &out, Graph *graph)
{

	out << "; --- z3 SMT script for graph "
		<< "and rules read from file ---\n\n; --- declaring booleans ---\n\n"
		<< flush;
	out.flush();

	for (map<string, Node *>::iterator itr = (graph->rep_id_to_node_map).begin(); itr != (graph->rep_id_to_node_map).end(); ++itr)
	{
		// we are ignoring nodes that are maps, orthologues, enzymes, or reactions
		if ((*itr).second->node_type != "map" && (*itr).second->node_type != "ortholog" && (*itr).second->node_type != "enzyme" && (*itr).second->node_type != "reaction")
		{
			string node_id = (*itr).first;

			// declarations for nodes
			if ((*itr).second->node_type == "gene")
			{
				out << "(declare-const " << node_id + "_active Bool)\n"
					<< flush;
				out << "(declare-const " << node_id + "_expression_changes Bool)\n"
					<< flush;
				out << "(declare-const " << node_id + "_over_expressed Bool)\n"
					<< flush;
			}

			if ((*itr).second->node_type == "compound")
			{
				out << "(declare-const " << node_id + "_active Bool)\n"
					<< flush;
			}

			// declarations for edges (since every edge will be in outlist of some node so we look at outlist)
			for (set<Edge *>::iterator edge_itr = itr->second->outlist.begin(); edge_itr != itr->second->outlist.end(); ++edge_itr)
			{

				list<string>::iterator itr_ls_begin = (*edge_itr)->edge_subtype.begin();
				list<string>::iterator itr_ls_end = (*edge_itr)->edge_subtype.end();

				// we are ignoring missing interactions and maplinks
				if (find(itr_ls_begin, itr_ls_end, "missing interaction") == itr_ls_end && (*edge_itr)->edge_type != "maplink")
				{
					// edge presence variable
					out << "(declare-const "
						<< "edge_" << (*edge_itr)->edge_source->rep_id_name + "to" + (*edge_itr)->edge_target->rep_id_name + "_present Bool)\n"
						<< flush;

					// cases where the activation inhibition is not present on the edge but we believe either should be present
					// PPrel and ECrel cases
					handle_edge_active_unknown(edge_itr, out);

					// PCrel cases
					if ((*edge_itr)->edge_type == "PCrel" &&
						find(itr_ls_begin, itr_ls_end, "activation") == itr_ls_end &&
						find(itr_ls_begin, itr_ls_end, "inhibition") == itr_ls_end)
					{

						out << "(declare-const "
							<< "edge_" << (*edge_itr)->edge_source->rep_id_name + "to" + (*edge_itr)->edge_target->rep_id_name + "_active Bool)\n"
							<< flush;
					}
				}
			}
		}
	}

	out << "\n"
		<< flush;
}

/*
void GraphManager::writeConstraintDefinitions(ofstream& out, Graph* graph, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes){
	out << "; --- constraint defintions---\n\n" << flush;
	out << "(define-fun constraints () Bool\n" << flush;
	int definition_counter = 0; //for handling and operator of z3 script
	string definitions = "";

	set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->map_of_nodes); //get all the edge ids (bvatom type) of the graph

	//set various bvatoms for different cases for comparison with edge attributes

	//bvatom for activation and inhibition
	bvatom edge_subtype_active_bv = get_bvatom_for_edge_subtype("activation");
	bvatom edge_subtype_inhibit_bv = get_bvatom_for_edge_subtype("inhibition");

	for(set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr){//iterate over the edge ids
			//get current edge subtype as bvatom
			bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_subtype");

			//get source node hsa ids
			bvatom source_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "source_node_id");
			unsigned long source_id_ulong = bvm->convertBVToUnsignedInt(source_id_bv);
			string source_id_str = get_hsa_id_from_local_node_id((unsigned long) source_id_ulong);
			string source_id_smt_var = source_id_str + "_level";

			//get target node hsa ids
			bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
			unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
			string target_id_str = get_hsa_id_from_local_node_id((unsigned long) target_id_ulong);
			string target_id_smt_var = target_id_str + "_level";

			//iterate over the rules to look for whatever you want
			for(list<t_rule*>::const_iterator list_itr = global_list_of_rules.begin(); list_itr != global_list_of_rules.end(); ++list_itr){
				t_list_of_t_attr_content* list_of_attr = (*list_itr)->list_of_attributes;
				t_list_of_t_action* list_of_actns = (*list_itr)->list_of_actions;

				//case for handling activation
				if(rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel")){

					// source_active --> target_active constraint definitions
					if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active")){
						//update_activation_definitions(definitions, source_id_smt_var, target_id_smt_var, definition_counter);
					}

					//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
					if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict") && bvm->checkBVEquality(curr_edge_subtype_bv, edge_subtype_active_bv)){
						//mark_potentially_conflicting_nodes(graph, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);
					}
				}

				//case for handling inhibition
				if(rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel")){

					// source_active --> target_inhibit constraint definitions
					if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit")){
						//update_inhibition_definitions(definitions, source_id_smt_var, target_id_smt_var, definition_counter);
					}

					//to add to the set of potentially conflicting nodes
					if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict") && bvm->checkBVEquality(curr_edge_subtype_bv, edge_subtype_inhibit_bv)){
						//mark_potentially_conflicting_nodes(graph, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);
					}

				}

			}//inner for ends here
		}//outer for ends here
	out << definitions << flush;
	out << "\n)\n(assert constraints)\n(check-sat)\n(get-model)\n" << flush;
}
*/

void GraphManager::declare_protein_variables(ofstream &out_decl, const string &z3_active, const string &z3_changes, const string &z3_over_expressed)
{
	out_decl << "(declare-const " << z3_active + " Bool)\n"
			 << flush;
	out_decl << "(declare-const " << z3_changes + " Bool)\n"
			 << flush;
	out_decl << "(declare-const " << z3_over_expressed + " Bool)\n"
			 << flush;
}

/*
//writes both declarations and definitions in z3 compatible format --- Harshit
void GraphManager::write_constraints_general(ofstream& out_decl, ofstream& out_defn, ofstream& out_assert, ofstream& out_confl, Graph* graph, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes){
	int definition_counter = 0; //for handling and operator of z3 script
	string definitions = "";

	//set various bvatoms for different cases for comparison with edge attributes
	//edge types
	bvatom edge_type_PPrel = get_bvatom_for_edge_type("PPrel");
	bvatom edge_type_ECrel = get_bvatom_for_edge_type("ECrel");
	bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
	bvatom edge_type_PCrel = get_bvatom_for_edge_type("PCrel");
	bvatom edge_type_maplink = get_bvatom_for_edge_type("maplink");

	//edge subtypes
	bvatom edge_subtype_active_bv = get_bvatom_for_edge_subtype("activation");
	bvatom edge_subtype_inhibit_bv = get_bvatom_for_edge_subtype("inhibition");
	bvatom edge_subtype_express_bv = get_bvatom_for_edge_subtype("expression");
	bvatom edge_subtype_repress_bv = get_bvatom_for_edge_subtype("repression");
	bvatom edge_subtype_indirect_effect_bv = get_bvatom_for_edge_subtype("indirect effect");
	bvatom edge_subtype_state_change_bv = get_bvatom_for_edge_subtype("state change");
	bvatom edge_subtype_bind_bv = get_bvatom_for_edge_subtype("binding/association");
	bvatom edge_subtype_disso_bv = get_bvatom_for_edge_subtype("dissociation");
	bvatom edge_subtype_miss_interctn_bv = get_bvatom_for_edge_subtype("missing interaction");
	bvatom edge_subtype_phospho_bv = get_bvatom_for_edge_subtype("phosphorylation");
	bvatom edge_subtype_dephospho_bv = get_bvatom_for_edge_subtype("dephosphorylation");
	bvatom edge_subtype_glycosyl_bv = get_bvatom_for_edge_subtype("glycosylation");
	bvatom edge_subtype_ubiquit_bv = get_bvatom_for_edge_subtype("ubiquitination");
	bvatom edge_subtype_methyl_bv = get_bvatom_for_edge_subtype("methylation");

	//node types
	bvatom node_type_gene_bv = get_bvatom_for_node_type("gene");
	bvatom node_type_group_bv = get_bvatom_for_node_type("group");
	bvatom node_type_compound_bv = get_bvatom_for_node_type("compound");


	set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->map_of_nodes);//get all the edge ids (bvatom type) of the graph
	for(set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr){//iterate over the edge ids
		//get current edge subtype and type as bvatom
		bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_subtype");
		bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_type");
		bvatom curr_source_node_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "source_node_type");
		bvatom curr_target_node_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_type");

		//get source node ids (gene, compound)
		bvatom source_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "source_node_id");
		unsigned long source_id_ulong = bvm->convertBVToUnsignedInt(source_id_bv);
		string source_id_str = get_hsa_id_from_local_node_id((unsigned long) source_id_ulong);

		//z3 variables for source
		string source_z3_active;
		string source_z3_changes;
		string source_z3_over_expressed;
		map<string, Node*>::iterator itr_graph_map_source;
		itr_graph_map_source = graph->map_of_nodes.find(source_id_str);
		if(itr_graph_map_source->second->node_type == "gene"){
			source_z3_active = source_id_str+"_active";
			source_z3_changes = source_id_str+"_expression_changes";
			source_z3_over_expressed = source_id_str+"_over_expressed";
			declare_protein_variables(out_decl, source_z3_active, source_z3_changes, source_z3_over_expressed);
		}
		if(itr_graph_map_source->second->node_type == "compound"){
			source_z3_active = source_id_str+"_active";
			out_decl << "(declare-const " << source_z3_active + " Bool)\n" << flush;
		}

		//get target node hsa ids
		bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
		unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
		string target_id_str = get_hsa_id_from_local_node_id((unsigned long) target_id_ulong);

		//z3 variables for target
		string target_z3_active;
		string target_z3_changes;
		string target_z3_over_expressed;
		map<string, Node*>::iterator itr_graph_map_target;
		itr_graph_map_target = graph->map_of_nodes.find(target_id_str);
		if(itr_graph_map_target->second->node_type == "gene"){
			target_z3_active = target_id_str+"_active";
			target_z3_changes = target_id_str+"_expression_changes";
			target_z3_over_expressed = target_id_str+"_over_expressed";
			declare_protein_variables(out_decl, target_z3_active, target_z3_changes, target_z3_over_expressed);
		}
		if(itr_graph_map_target->second->node_type == "compound"){
			target_z3_active = target_id_str+"_active";
			out_decl << "(declare-const " << target_z3_active + " Bool)\n" << flush;
		}

		//z3 variables for edges

		//edge presence
		string edge_z3_present;
		if(!bvm->checkBVEquality(curr_edge_type_bv, edge_type_maplink) &&
			bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_miss_interctn_bv))  &&
				itr_graph_map_source->second->node_type != "ortholog" &&
				itr_graph_map_source->second->node_type != "enzyme" &&
				itr_graph_map_source->second->node_type != "reaction" &&
				itr_graph_map_source->second->node_type != "map" &&
				itr_graph_map_target->second->node_type != "ortholog" &&
				itr_graph_map_target->second->node_type != "enzyme" &&
				itr_graph_map_target->second->node_type != "reaction" &&
				itr_graph_map_target->second->node_type != "map"
		){
			edge_z3_present = "edge_" + source_id_str + "to" + target_id_str + "_present";
			out_decl << "(declare-const " << edge_z3_present + " Bool)\n" << flush;
		}

		//edge could be activation/inhibition but not enough info in the edge graph
		//for this case we first need a check on all the other adjacent edges of the source and target variable

		//check if source node has some other adjacent edge involved in assoc/dissoc
		set<Edge*> source_node_adj_edges(itr_graph_map_source->second->outlist);
		source_node_adj_edges.insert(itr_graph_map_source->second->inlist.begin(), itr_graph_map_source->second->inlist.end());

		bool source_node_has_assoc_dissoc_edge = false;
		bvatom current_edgeid = *edge_itr;
		set<Edge*>::iterator edge_set_itr;
		for(edge_set_itr = source_node_adj_edges.begin(); edge_set_itr != source_node_adj_edges.end(); edge_set_itr++){
			bvatom edge_consider_id = (*edge_set_itr)->edge_id;
			if(!bvm->checkBVEquality(current_edgeid, edge_consider_id)){
				list<string>::iterator itr_adj_edges_begin = (*edge_set_itr)->edge_subtype.begin();
				list<string>::iterator itr_adj_edges_end = (*edge_set_itr)->edge_subtype.end();
				if(find(itr_adj_edges_begin, itr_adj_edges_end, "binding/association") != itr_adj_edges_end || find(itr_adj_edges_begin, itr_adj_edges_end, "dissociation") != itr_adj_edges_end)
					source_node_has_assoc_dissoc_edge = true;
			}
		}

		//check if target node has some other adjacent edge involved in assoc/dissoc
		set<Edge*> target_node_adj_edges(itr_graph_map_target->second->inlist);
		target_node_adj_edges.insert(itr_graph_map_target->second->outlist.begin(), itr_graph_map_target->second->outlist.end());

		bool target_node_has_assoc_dissoc_edge = false;
		for(edge_set_itr = target_node_adj_edges.begin(); edge_set_itr != target_node_adj_edges.end(); edge_set_itr++){
			bvatom edge_consider_id = (*edge_set_itr)->edge_id;
			if(!bvm->checkBVEquality(current_edgeid, edge_consider_id)){
				list<string>::iterator itr_adj_edges_begin = (*edge_set_itr)->edge_subtype.begin();
				list<string>::iterator itr_adj_edges_end = (*edge_set_itr)->edge_subtype.end();
				if(find(itr_adj_edges_begin, itr_adj_edges_end, "binding/association") != itr_adj_edges_end || find(itr_adj_edges_begin, itr_adj_edges_end, "dissociation") != itr_adj_edges_end)
					target_node_has_assoc_dissoc_edge = true;
			}
		}

		//now we come to the actual check
		string edge_z3_active;
		bool edge_neither_activ_nor_inhibit_in_graph = false;
		//PPrel and ECrel cases
		if((bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
				bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel)) &&
				!target_node_has_assoc_dissoc_edge &&
				!source_node_has_assoc_dissoc_edge &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_bind_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_disso_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_state_change_bv))
		){
			edge_z3_active = "edge_" + source_id_str + "to" + target_id_str + "_active";
			out_decl << "(declare-const " << edge_z3_active + " Bool)\n" << flush;
			edge_neither_activ_nor_inhibit_in_graph = true;
		}

		//PCrel cases
		if(bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) &&
		   bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
		   bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)))
		{
			edge_z3_active = "edge_" + source_id_str + "to" + target_id_str + "_active";
			out_decl << "(declare-const " << edge_z3_active + " Bool)\n" << flush;
		}

		//iterate over the rules to generate constraints
		for(list<t_rule*>::const_iterator list_itr = global_list_of_rules.begin(); list_itr != global_list_of_rules.end(); ++list_itr){
			t_list_of_t_attr_content* list_of_attr = (*list_itr)->list_of_attributes;
			t_list_of_t_action* list_of_actns = (*list_itr)->list_of_actions;

			//case 1: protein activates protein (assoc dissoc not involved)
			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_active") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_active") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
					 rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel))
			){

				// source_active --> target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active"))
					update_activation_definitions_simple_PPrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);

			}

			//case 2: protein inhibits protein (assoc dissoc not involved)

			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_inhibit") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_inhibit") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
							rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel))
			){

				// source_active --> not target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit"))
					update_inhibition_definitions_simple_PPrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);

			}

			//case 3: protein expresses protein

			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_express") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_express") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "expression") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_express_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel)
			){

				// source_active --> target_over_expressed constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_express"))
					update_expression_definitions(definitions, edge_z3_present, source_z3_active , target_z3_over_expressed, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_repressed", LOGICAL_IMPLIES, "flag_target_for_potential_expression_conflict"))
					mark_potentially_conflicting_nodes_expression(graph, out_confl, target_id_str, edge_subtype_repress_bv, set_potential_conflict_nodes);

			}

			//case 4: protein represses protein

			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_repress") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_repress") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "repression") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_repress_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel)
			){

				// source_active --> not target_over_expressed constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_repress"))
					update_repression_definitions(definitions, edge_z3_present, source_z3_active , target_z3_over_expressed, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_expressed", LOGICAL_IMPLIES, "flag_target_for_potential_expression_conflict"))
					mark_potentially_conflicting_nodes_expression(graph, out_confl, target_id_str, edge_subtype_express_bv, set_potential_conflict_nodes);

			}

			//case 5: protein activates compound

			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_active") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_active") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active"))
					update_activation_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);
			}

			//case 6: protein inhibits compound

			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_inhibit") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_inhibit") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> not target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit"))
					update_inhibition_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);
			}

			//case 7: compound activates protein

			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_active") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_active") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active"))
					update_activation_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);
			}

			//case 8: compound inhibits protein
			if(rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_inhibit") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "target_inhibit") && !target_node_has_assoc_dissoc_edge &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> not target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit"))
					update_inhibition_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);
			}

			//case 9:PPrel and ECrel where edge in KEGG is neither activating nor inhibiting
			if(edge_neither_activ_nor_inhibit_in_graph &&
				rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
			   rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
			   (rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
				rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_activation") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_inhibition") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_assoc") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_bind_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_dissoc") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_disso_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_state_change") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_state_change_bv)) &&
				rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "either") && !source_node_has_assoc_dissoc_edge &&
				rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "either") && !target_node_has_assoc_dissoc_edge
			){
				// edge_active -> (source_active --> target_active) OR edge_inhibit -> (source_active --> not target_active) constraint definitions
				if(rulesHaveAction(list_of_actns, "edge_active", LOGICAL_IMPLIES, "activation") &&
				   rulesHaveAction(list_of_actns, "edge_inhibit", LOGICAL_IMPLIES, "inhibition"))
					update_definitions_edgetype_neither_PPrel(definitions, edge_z3_present, edge_z3_active, source_z3_active , target_z3_active, definition_counter);
			}

			//case 10:PCrel where edge in KEGG is neither activating nor inhibiting --- edge from gene to compound
			if(edge_neither_activ_nor_inhibit_in_graph &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_activation") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_inhibition") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "either") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "either") && !target_node_has_assoc_dissoc_edge
			){
				// edge_active -> (source_active --> target_active) OR edge_inhibit -> (source_active --> target_active) constraint definitions
				if(rulesHaveAction(list_of_actns, "edge_active", LOGICAL_IMPLIES, "activation") &&
						rulesHaveAction(list_of_actns, "edge_inhibit", LOGICAL_IMPLIES, "inhibition"))
					update_definitions_edgetype_neither_PCrel(definitions, edge_z3_present, edge_z3_active, source_z3_active , target_z3_active, definition_counter);
			}

			//case 11:PCrel where edge in KEGG is neither activating nor inhibiting --- edge from compound to gene
			if(edge_neither_activ_nor_inhibit_in_graph &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_activation") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_inhibition") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAction(list_of_actns, "source_not_in_assoc_dissoc", LOGICAL_IMPLIES, "either") && !source_node_has_assoc_dissoc_edge &&
					rulesHaveAction(list_of_actns, "target_not_in_assoc_dissoc", LOGICAL_IMPLIES, "either") && !target_node_has_assoc_dissoc_edge
			){
				// edge_active -> (source_active --> target_active) OR edge_inhibit -> (source_active --> target_active) constraint definitions
				if(rulesHaveAction(list_of_actns, "edge_active", LOGICAL_IMPLIES, "activation") &&
						rulesHaveAction(list_of_actns, "edge_inhibit", LOGICAL_IMPLIES, "inhibition"))
					update_definitions_edgetype_neither_PCrel(definitions, edge_z3_present, edge_z3_active, source_z3_active , target_z3_active, definition_counter);
			}

			//case 12: if either of the source or target nodes are involved in association or dissociation with edges other than the current one
			if((rulesHaveAction(list_of_actns, "source_in_assoc_dissoc", LOGICAL_IMPLIES, "retain_edge") && source_node_has_assoc_dissoc_edge ||
					rulesHaveAction(list_of_actns, "target_in_assoc_dissoc", LOGICAL_IMPLIES, "retain_edge") && target_node_has_assoc_dissoc_edge) &&
					(rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) ||
					 rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) ||
					 rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "group") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					(rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) ||
					 rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) ||
					 rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "group") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
					(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) ||
					 rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel) ||
					 rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
					 rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel))
			){
				//out_assert << "( assert " + edge_z3_present + " )\n" << flush;
			}

			//case 13: if current edge itself is involved in association or dissociation
			if((rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) ||
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) ||
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "group") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					(rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) ||
							rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) ||
							rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "group") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
							(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) ||
									rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel) ||
									rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
									rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel)) &&
									(rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "binding") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_bind_bv)) ||
											rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "dissociation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_disso_bv)) )
			){
				//out_assert << "( assert " + edge_z3_present + " )\n" << flush;
			}
		}
	}
	out_defn << definitions << flush;
}
*/

/*
//writes both declarations and definitions in z3 compatible format --- Harshit
void GraphManager::write_constraints_general_new(ofstream& out_decl, ofstream& out_defn, ofstream& out_assert, ofstream& out_confl, Graph* graph, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes){
	int definition_counter = 0; //for handling and operator of z3 script
	string definitions = "";

	//set various bvatoms for different cases for comparison with edge attributes
	//edge types
	bvatom edge_type_PPrel = get_bvatom_for_edge_type("PPrel");
	bvatom edge_type_ECrel = get_bvatom_for_edge_type("ECrel");
	bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
	bvatom edge_type_PCrel = get_bvatom_for_edge_type("PCrel");
	bvatom edge_type_maplink = get_bvatom_for_edge_type("maplink");

	//edge subtypes
	bvatom edge_subtype_active_bv = get_bvatom_for_edge_subtype("activation");
	bvatom edge_subtype_inhibit_bv = get_bvatom_for_edge_subtype("inhibition");
	bvatom edge_subtype_express_bv = get_bvatom_for_edge_subtype("expression");
	bvatom edge_subtype_repress_bv = get_bvatom_for_edge_subtype("repression");
	bvatom edge_subtype_indirect_effect_bv = get_bvatom_for_edge_subtype("indirect effect");
	bvatom edge_subtype_state_change_bv = get_bvatom_for_edge_subtype("state change");
	bvatom edge_subtype_bind_bv = get_bvatom_for_edge_subtype("binding/association");
	bvatom edge_subtype_disso_bv = get_bvatom_for_edge_subtype("dissociation");
	bvatom edge_subtype_miss_interctn_bv = get_bvatom_for_edge_subtype("missing interaction");
	bvatom edge_subtype_phospho_bv = get_bvatom_for_edge_subtype("phosphorylation");
	bvatom edge_subtype_dephospho_bv = get_bvatom_for_edge_subtype("dephosphorylation");
	bvatom edge_subtype_glycosyl_bv = get_bvatom_for_edge_subtype("glycosylation");
	bvatom edge_subtype_ubiquit_bv = get_bvatom_for_edge_subtype("ubiquitination");
	bvatom edge_subtype_methyl_bv = get_bvatom_for_edge_subtype("methylation");

	//node types
	bvatom node_type_gene_bv = get_bvatom_for_node_type("gene");
	bvatom node_type_group_bv = get_bvatom_for_node_type("group");
	bvatom node_type_compound_bv = get_bvatom_for_node_type("compound");


	set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->map_of_nodes);//get all the edge ids (bvatom type) of the graph
	for(set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr){//iterate over the edge ids
		//get current edge subtype and type as bvatom
		bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_subtype");
		bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_type");
		bvatom curr_source_node_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "source_node_type");
		bvatom curr_target_node_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_type");

		//get source node ids (gene, compound, complex)
		bvatom source_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "source_node_id");
		unsigned long source_id_ulong = bvm->convertBVToUnsignedInt(source_id_bv);
		string source_id_str = get_hsa_id_from_local_node_id((unsigned long) source_id_ulong);

		//z3 variables for source
		string source_z3_active;
		string source_z3_changes;
		string source_z3_over_expressed;
		map<string, Node*>::iterator itr_graph_map_source;
		itr_graph_map_source = graph->map_of_nodes.find(source_id_str);

		if(itr_graph_map_source->second->node_type == "gene"){
			source_z3_active = source_id_str+"_active";
			source_z3_changes = source_id_str+"_expression_changes";
			source_z3_over_expressed = source_id_str+"_over_expressed";
			declare_protein_variables(out_decl, source_z3_active, source_z3_changes, source_z3_over_expressed);
		}

		if(itr_graph_map_source->second->node_type == "compound" || itr_graph_map_source->second->node_type == "group"){
			source_z3_active = source_id_str+"_active";
			out_decl << "(declare-const " << source_z3_active + " Bool)\n" << flush;
		}

		//get target node ids (gene, compound, complex)
		bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
		unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
		string target_id_str = get_hsa_id_from_local_node_id((unsigned long) target_id_ulong);

		//z3 variables for target
		string target_z3_active;
		string target_z3_changes;
		string target_z3_over_expressed;
		map<string, Node*>::iterator itr_graph_map_target;
		itr_graph_map_target = graph->map_of_nodes.find(target_id_str);

		if(itr_graph_map_target->second->node_type == "gene"){
			target_z3_active = target_id_str+"_active";
			target_z3_changes = target_id_str+"_expression_changes";
			target_z3_over_expressed = target_id_str+"_over_expressed";
			declare_protein_variables(out_decl, target_z3_active, target_z3_changes, target_z3_over_expressed);
		}
		if(itr_graph_map_target->second->node_type == "compound" || itr_graph_map_target->second->node_type == "group"){
			target_z3_active = target_id_str+"_active";
			out_decl << "(declare-const " << target_z3_active + " Bool)\n" << flush;
		}

		//z3 variables for edges

		//edge presence
		string edge_z3_present;
		if(!bvm->checkBVEquality(curr_edge_type_bv, edge_type_maplink) &&
			bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_miss_interctn_bv))  &&
				itr_graph_map_source->second->node_type != "ortholog" &&
				itr_graph_map_source->second->node_type != "enzyme" &&
				itr_graph_map_source->second->node_type != "reaction" &&
				itr_graph_map_source->second->node_type != "map" &&
				itr_graph_map_target->second->node_type != "ortholog" &&
				itr_graph_map_target->second->node_type != "enzyme" &&
				itr_graph_map_target->second->node_type != "reaction" &&
				itr_graph_map_target->second->node_type != "map"
		){
			edge_z3_present = "edge_" + source_id_str + "to" + target_id_str + "_present";
			out_decl << "(declare-const " << edge_z3_present + " Bool)\n" << flush;
		}

		//edge could be activation/inhibition but not enough info in the edge graph
		string edge_z3_active;
		bool edge_neither_activ_nor_inhibit_in_graph = false;
		//PPrel and ECrel cases
		if((bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
				bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_bind_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_disso_bv)) &&
				bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_state_change_bv))
		){
			edge_z3_active = "edge_" + source_id_str + "to" + target_id_str + "_active";
			out_decl << "(declare-const " << edge_z3_active + " Bool)\n" << flush;
			edge_neither_activ_nor_inhibit_in_graph = true;
		}

		//PCrel cases
		if(bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) &&
		   bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
		   bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)))
		{
			edge_z3_active = "edge_" + source_id_str + "to" + target_id_str + "_active";
			out_decl << "(declare-const " << edge_z3_active + " Bool)\n" << flush;
		}

		//iterate over the rules to generate constraints
		for(list<t_rule*>::const_iterator list_itr = global_list_of_rules.begin(); list_itr != global_list_of_rules.end(); ++list_itr){
			t_list_of_t_attr_content* list_of_attr = (*list_itr)->list_of_attributes;
			t_list_of_t_action* list_of_actns = (*list_itr)->list_of_actions;

			//case 1: protein/protein-complexes activates protein/protein-complexes (assoc dissoc not involved)
			if(	rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
					(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
					 rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel))
			){

				// source_active --> target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active"))
					update_activation_definitions_simple_PPrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);

			}

			//case 2: protein/protein-complexes inhibits protein/protein-complexes (assoc dissoc not involved)

			if(	rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
					(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel))
			){

				// source_active --> not target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit"))
					update_inhibition_definitions_simple_PPrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);

			}

			//case 3: protein/protein-complex expresses protein

			if(
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "expression") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_express_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel)
			){

				// source_active --> target_over_expressed constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_express"))
					update_expression_definitions(definitions, edge_z3_present, source_z3_active , target_z3_over_expressed, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_repressed", LOGICAL_IMPLIES, "flag_target_for_potential_expression_conflict"))
					mark_potentially_conflicting_nodes_expression(graph, out_confl, target_id_str, edge_subtype_repress_bv, set_potential_conflict_nodes);

			}

			//case 4: protein/protein-complex represses protein

			if(
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "repression") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_repress_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel)
			){

				// source_active --> not target_over_expressed constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_repress"))
					update_repression_definitions(definitions, edge_z3_present, source_z3_active , target_z3_over_expressed, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_expressed", LOGICAL_IMPLIES, "flag_target_for_potential_expression_conflict"))
					mark_potentially_conflicting_nodes_expression(graph, out_confl, target_id_str, edge_subtype_express_bv, set_potential_conflict_nodes);

			}

			//case 5: protein/protein-complex activates compound

			if(
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active"))
					update_activation_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);
			}

			//case 6: protein/protein-complex inhibits compound

			if(
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> not target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit"))
					update_inhibition_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);
			}

			//case 7: compound activates protein/protein-complex

			if(
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "activation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_active"))
					update_activation_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_inhibited", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_inhibit_bv, set_potential_conflict_nodes);
			}

			//case 8: compound inhibits protein/protein-complex
			if(
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "inhibition") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel)
			){
				// source_active --> not target_active constraint definitions
				if(rulesHaveAction(list_of_actns, "source_active", LOGICAL_IMPLIES, "target_inhibit"))
					update_inhibition_definitions_simple_PCrel(definitions, edge_z3_present, source_z3_active , target_z3_active, definition_counter);

				//add to the set of potentially conflicting node and set corresponding bool flag in the Node class
				if(rulesHaveAction(list_of_actns, "target_activated", LOGICAL_IMPLIES, "flag_target_for_potential_activation_conflict"))
					mark_potentially_conflicting_nodes(graph, out_confl, target_id_str, edge_subtype_active_bv, set_potential_conflict_nodes);
			}

			//commenting starts for the must be inhibition/activation when none is given in KEGG
			/*
			//case 9:PPrel and ECrel where edge in KEGG is neither activating nor inhibiting
			if(edge_neither_activ_nor_inhibit_in_graph &&
				rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
				rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
			   (rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
				rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_activation") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_inhibition") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_assoc") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_bind_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_dissoc") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_disso_bv)) &&
				rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_state_change") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_state_change_bv))
			){
				// edge_active -> (source_active --> target_active) OR edge_inhibit -> (source_active --> not target_active) constraint definitions
				if(rulesHaveAction(list_of_actns, "edge_active", LOGICAL_IMPLIES, "activation") &&
				   rulesHaveAction(list_of_actns, "edge_inhibit", LOGICAL_IMPLIES, "inhibition"))
						update_definitions_edgetype_neither_PPrel(definitions, edge_z3_present, edge_z3_active, source_z3_active , target_z3_active, definition_counter);
			}

			//case 10:PCrel where edge in KEGG is neither activating nor inhibiting --- edge from gene/complex to compound
			if(edge_neither_activ_nor_inhibit_in_graph &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_activation") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_inhibition") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv))
			){
				// edge_active -> (source_active --> target_active) OR edge_inhibit -> (source_active --> target_active) constraint definitions
				if(rulesHaveAction(list_of_actns, "edge_active", LOGICAL_IMPLIES, "activation") &&
						rulesHaveAction(list_of_actns, "edge_inhibit", LOGICAL_IMPLIES, "inhibition"))
					update_definitions_edgetype_neither_PCrel(definitions, edge_z3_present, edge_z3_active, source_z3_active , target_z3_active, definition_counter);
			}

			//case 11:PCrel where edge in KEGG is neither activating nor inhibiting --- edge from compound to gene/complex
			if(edge_neither_activ_nor_inhibit_in_graph &&
					rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) &&
					rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) || bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_activation") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)) &&
					rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "not_inhibition") && bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv))
			){
				// edge_active -> (source_active --> target_active) OR edge_inhibit -> (source_active --> target_active) constraint definitions
				if(rulesHaveAction(list_of_actns, "edge_active", LOGICAL_IMPLIES, "activation") &&
						rulesHaveAction(list_of_actns, "edge_inhibit", LOGICAL_IMPLIES, "inhibition"))
					update_definitions_edgetype_neither_PCrel(definitions, edge_z3_present, edge_z3_active, source_z3_active , target_z3_active, definition_counter);
			}
			*/
// commenting ends for the must be inhibition/activation when none is given in KEGG
/*
//case 12: if current edge itself is involved in association or dissociation
if((rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) ||
		rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) ||
		rulesHaveAttribute(list_of_attr, SOURCE_NODE_TYPE, "group") && bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)) &&
		(rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "compound") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) ||
				rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "gene") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) ||
				rulesHaveAttribute(list_of_attr, TARGET_NODE_TYPE, "group") && bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
				(rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PCrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) ||
						rulesHaveAttribute(list_of_attr, EDGE_TYPE, "ECrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel) ||
						rulesHaveAttribute(list_of_attr, EDGE_TYPE, "PPrel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
						rulesHaveAttribute(list_of_attr, EDGE_TYPE, "GErel") && bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel)) &&
						(rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "binding") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_bind_bv)) ||
								rulesHaveAttribute(list_of_attr, EDGE_SUBTYPE, "dissociation") && !bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_disso_bv)) )
){
	//out_assert << "( assert " + edge_z3_present + " )\n" << flush;
}
}
}
out_defn << definitions << flush;
}
*/

void GraphManager::write_constraints_node_based(ofstream &out_defn, Graph *graph)
{
	string outer_and = "\n( and ";
	bool nothing_to_write = true;
	for (map<string, Node *>::iterator itr = graph->rep_id_to_node_map.begin(); itr != graph->rep_id_to_node_map.end(); ++itr)
	{
		Node *curr_node = itr->second;
		set<Edge *> *inlist_curr_node = &(curr_node->inlist);
		string target_node_id = curr_node->rep_id_name;

		vector<string> active_defns;
		vector<string> inactive_defns;
		vector<string> express_defns;
		vector<string> repress_defns;

		string active_constraints1 = "";
		string active_constraints2 = "";
		string inactive_constraints1 = "";
		string inactive_constraints2 = "";

		string express_constraints1 = "";
		string express_constraints2 = "";
		string repress_constraints1 = "";
		string repress_constraints2 = "";

		// set various bvatoms for different cases for comparison with edge attributes
		// edge types
		bvatom edge_type_PPrel = get_bvatom_for_edge_type("PPrel");
		bvatom edge_type_ECrel = get_bvatom_for_edge_type("ECrel");
		bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
		bvatom edge_type_PCrel = get_bvatom_for_edge_type("PCrel");
		bvatom edge_type_maplink = get_bvatom_for_edge_type("maplink");

		// edge subtypes
		bvatom edge_subtype_active_bv = get_bvatom_for_edge_subtype("activation");
		bvatom edge_subtype_inhibit_bv = get_bvatom_for_edge_subtype("inhibition");
		bvatom edge_subtype_express_bv = get_bvatom_for_edge_subtype("expression");
		bvatom edge_subtype_repress_bv = get_bvatom_for_edge_subtype("repression");
		bvatom edge_subtype_indirect_effect_bv = get_bvatom_for_edge_subtype("indirect effect");
		bvatom edge_subtype_state_change_bv = get_bvatom_for_edge_subtype("state change");
		bvatom edge_subtype_bind_bv = get_bvatom_for_edge_subtype("binding/association");
		bvatom edge_subtype_disso_bv = get_bvatom_for_edge_subtype("dissociation");
		bvatom edge_subtype_miss_interctn_bv = get_bvatom_for_edge_subtype("missing interaction");
		bvatom edge_subtype_phospho_bv = get_bvatom_for_edge_subtype("phosphorylation");
		bvatom edge_subtype_dephospho_bv = get_bvatom_for_edge_subtype("dephosphorylation");
		bvatom edge_subtype_glycosyl_bv = get_bvatom_for_edge_subtype("glycosylation");
		bvatom edge_subtype_ubiquit_bv = get_bvatom_for_edge_subtype("ubiquitination");
		bvatom edge_subtype_methyl_bv = get_bvatom_for_edge_subtype("methylation");

		// node types
		bvatom node_type_gene_bv = get_bvatom_for_node_type("gene");
		bvatom node_type_group_bv = get_bvatom_for_node_type("group");
		bvatom node_type_compound_bv = get_bvatom_for_node_type("compound");

		for (set<Edge *>::iterator itr_e = (*inlist_curr_node).begin(); itr_e != (*inlist_curr_node).end(); ++itr_e)
		{
			string source_node_id = (*itr_e)->edge_source->rep_id_name;

			bvatom edge_bv = (*itr_e)->edge_id;
			bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id(edge_bv, "edge_subtype");
			bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(edge_bv, "edge_type");
			bvatom curr_source_node_type_bv = get_field_as_bv_from_edge_id(edge_bv, "source_node_type");
			bvatom curr_target_node_type_bv = get_field_as_bv_from_edge_id(edge_bv, "target_node_type");

			// activation inhibition case
			if ((bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) ||
				 bvm->checkBVEquality(curr_target_node_type_bv, node_type_compound_bv) ||
				 bvm->checkBVEquality(curr_target_node_type_bv, node_type_group_bv)) &&
				(bvm->checkBVEquality(curr_edge_type_bv, edge_type_PPrel) ||
				 bvm->checkBVEquality(curr_edge_type_bv, edge_type_PCrel) ||
				 bvm->checkBVEquality(curr_edge_type_bv, edge_type_ECrel)) &&
				(bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) ||
				 bvm->checkBVEquality(curr_source_node_type_bv, node_type_compound_bv) ||
				 bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)))
			{

				string source_node_var = source_node_id + "_active";
				string edge_presence_var = "edge_" + source_node_id + "to" + target_node_id + "_present";
				string target_node_var = target_node_id + "_active";
				string curr_defn_part = "(and " + edge_presence_var + " " + source_node_var + ")";

				// activation case
				if (!bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_active_bv)))
				{
					active_defns.push_back(curr_defn_part);
				}
				// inhibition case
				if (!bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_inhibit_bv)))
				{
					inactive_defns.push_back(curr_defn_part);
				}
			}

			// expression repression case
			if (bvm->checkBVEquality(curr_target_node_type_bv, node_type_gene_bv) &&
				bvm->checkBVEquality(curr_edge_type_bv, edge_type_GErel) &&
				(bvm->checkBVEquality(curr_source_node_type_bv, node_type_gene_bv) ||
				 bvm->checkBVEquality(curr_source_node_type_bv, node_type_group_bv)))
			{

				string source_node_var = source_node_id + "_active";
				string edge_presence_var = "edge_" + source_node_id + "to" + target_node_id + "_present";
				string target_node_var = target_node_id + "_over_expressed";
				string curr_defn_part = "(and " + edge_presence_var + " " + source_node_var + ")";

				// expression case
				if (!bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_express_bv)))
				{
					express_defns.push_back(curr_defn_part);
				}
				// repression case
				if (!bvm->checkBVForZero(bvm->bitwise_and(curr_edge_subtype_bv, edge_subtype_repress_bv)))
				{
					repress_defns.push_back(curr_defn_part);
				}
			}
		}

		if (active_defns.size() == 1)
		{
			active_constraints1 += "\n(=> " + target_node_id + "_active" + " " + active_defns[0] + ")";
			active_constraints2 += "\n(=> " + active_defns[0] + " " + target_node_id + "_active)";
		}
		else if (active_defns.size() > 1)
		{
			string oring_all = "(or ";
			for (int i = 0; i < active_defns.size(); ++i)
			{
				oring_all += active_defns[i] + " ";
			}
			oring_all += ")";
			active_constraints1 += "\n(=> " + target_node_id + "_active" + " " + oring_all + ")";
			active_constraints2 += "\n(=> " + oring_all + " " + target_node_id + "_active)";
		}

		if (inactive_defns.size() == 1)
		{
			inactive_constraints1 += "\n(=> (not " + target_node_id + "_active)" + " " + inactive_defns[0] + ")";
			inactive_constraints2 += "\n(=> " + inactive_defns[0] + " (not " + target_node_id + "_active))";
		}
		else if (inactive_defns.size() > 1)
		{
			string oring_all = "(or ";
			for (int i = 0; i < inactive_defns.size(); ++i)
			{
				oring_all += inactive_defns[i] + " ";
			}
			oring_all += ")";
			active_constraints1 += "\n(=> (not " + target_node_id + "_active)" + " " + oring_all + ")";
			active_constraints2 += "\n(=> " + oring_all + " (not " + target_node_id + "_active))";
		}

		if (express_defns.size() == 1)
		{
			express_constraints1 += "\n(=> " + target_node_id + "_over_expressed" + " " + express_defns[0] + ")";
			express_constraints2 += "\n(=> " + express_defns[0] + " " + target_node_id + "_over_expressed)";
		}
		else if (express_defns.size() > 1)
		{
			string oring_all = "(or ";
			for (int i = 0; i < express_defns.size(); ++i)
			{
				oring_all += express_defns[i] + " ";
			}
			oring_all += ")";
			express_constraints1 += "\n(=> " + target_node_id + "_over_expressed" + " " + oring_all + ")";
			express_constraints2 += "\n(=> " + oring_all + " " + target_node_id + "_over_expressed)";
		}

		if (repress_defns.size() == 1)
		{
			repress_constraints1 += "\n(=> (not " + target_node_id + "_over_expressed)" + " " + repress_defns[0] + ")";
			repress_constraints2 += "\n(=> " + repress_defns[0] + " (not " + target_node_id + "_over_expressed))";
		}
		else if (repress_defns.size() > 1)
		{
			string oring_all = "(or ";
			for (int i = 0; i < repress_defns.size(); ++i)
			{
				oring_all += repress_defns[i] + " ";
			}
			oring_all += ")";
			repress_constraints1 += "\n(=> (not " + target_node_id + "_over_expressed)" + " " + oring_all + ")";
			repress_constraints2 += "\n(=> " + oring_all + " (not " + target_node_id + "_over_expressed))";
		}

		if (!active_defns.empty() || !inactive_defns.empty() || !express_defns.empty() || !repress_defns.empty())
		{
			nothing_to_write = false;
			out_defn << outer_and;
			outer_and = "";
			out_defn << active_constraints1 << active_constraints2 << inactive_constraints1 << inactive_constraints2 << express_constraints1 << express_constraints2 << repress_constraints1 << repress_constraints2 << endl;
		}
	}

	if (!nothing_to_write)
	{
		out_defn << ")\n";
	}
}

/*
void GraphManager::generateConstraints(const int& graphnum, const list<t_rule*>& global_list_of_rules, std::set<Node*>& set_potential_conflict_nodes){
	string constraints_declaration_file, constraints_definitions_file, constraints_assertions_file, potentially_conflicting_nodes_file;
	/*cout << "\nEnter the relative filepath for declaration of the variables in the constraints: ";
	cin >> constraints_declaration_file;


	cout << "\nEnter the relative filepath for definitions of the constraints: ";
	cin >> constraints_definitions_file;


	cout << "\nEnter the relative filepath for assertions of the constraints: ";
	cin >> constraints_assertions_file;


	cout << "\nEnter the relative filepath for storing potentially conflicting nodes: ";
	cin >> potentially_conflicting_nodes_file;
	*/

/*
	constraints_declaration_file = "dec";
	constraints_definitions_file = "def";
	constraints_assertions_file = "assert";
	potentially_conflicting_nodes_file = "confl";

	ofstream output_declarations(constraints_declaration_file.c_str());
	ofstream output_definitions(constraints_definitions_file.c_str());
	ofstream output_assertions(constraints_assertions_file.c_str());
	ofstream output_potential_conflicts(potentially_conflicting_nodes_file.c_str());

	Graph* graph = get_graph_from_index(graphnum);

	if(output_declarations.is_open() && output_definitions.is_open() && output_assertions.is_open() && output_potential_conflicts){
		//writeConstraintDeclarations(output_declarations, graph);
		//writeConstraintDefinitions(output_declarations, graph, global_list_of_rules, set_potential_conflict_nodes);
		output_definitions << "(define-fun graph-constraints () Bool\n";
		write_constraints_general_new(output_declarations, output_definitions, output_assertions, output_potential_conflicts, graph, global_list_of_rules, set_potential_conflict_nodes);
		output_definitions << ")\n";

		//the new definitions written per node by looking at target node - on modular code this will change
		output_definitions << "\n(define-fun graph-constraints2 () Bool\n";
		write_constraints_node_based(output_definitions, graph);
		output_definitions << ")\n";

		output_assertions << "\n( assert graph-constraints )\n";
		output_assertions << "\n( assert graph-constraints2 )\n";
	}
	else{
		cerr << "Unable to open declaration/definition/assertions/conflicting_nodes file(s) ";// << constraint_file << endl;
	}

	output_declarations.close();
	output_definitions.close();
	output_assertions.close();
	output_potential_conflicts.close();

	string micro_arr_decl, micro_arr_defn, micro_arr_assert, footer, final_z3_script, z3_outcome;

	/*cout << "\nEnter the relative filepath of micro array declarations: ";
	cin >> micro_arr_decl;

	cout << "\nEnter the relative filepath of micro array definitions: ";
	cin >> micro_arr_defn;

	cout << "\nEnter the relative filepath of micro array assertions: ";
	cin >> micro_arr_assert;

	cout << "\nEnter the relative filepath of z3 footer (check-sat, get-model etc stuff): ";
	cin >> footer;

	cout << "\nEnter the relative filepath of the final z3 script that z3 runs (give extension .smt2): ";
	cin >> final_z3_script;

	cout << "\nEnter the relative filepath to store z3 outcome ";
	cin >> z3_outcome;

	*/

/*
	char choice;
	cout << " Enter 'y' to include microarray data, \n or any other key to exclude microarray data ";
	cin >> choice;

	if(choice == 'y'){

		micro_arr_decl = "psmd9_decl";
		//micro_arr_defn = "psmd9_defn";
		micro_arr_defn = "empty";
		micro_arr_assert = "psmd9_assert";

		/*micro_arr_decl = "TB_S2_micro_decl";
		micro_arr_defn = "empty";
		micro_arr_assert = "TB_S2_micro_assert";*/

/*micro_arr_decl = "empty";
micro_arr_defn = "empty";
//micro_arr_assert = "samle_general_micro_array_assertions";
micro_arr_assert = "4_node_assertions";*/

/*micro_arr_decl = "empty";
micro_arr_defn = "empty";
micro_arr_assert = "4620_assert";*/

/*micro_arr_decl = "psmd9_decl_express_activ_both";
//micro_arr_defn = "psmd9_defn";
micro_arr_defn = "empty";
micro_arr_assert = "psmd9_assert_overexpressed_active_both";*/
/*
	}
	else{
		micro_arr_decl = "empty";
		micro_arr_defn = "empty";
		micro_arr_assert = "empty";
	}

	footer = "footer";
	final_z3_script = "final_z3_script.smt2";
	z3_outcome = "z3_result";


	//declarations may have been repeated so they need to be concatenated sorted and made unique
	string all_declarations = "all_declarations";
	string temp_file = "temp";
	string command_cat_declarations = "cat " + constraints_declaration_file + " " + micro_arr_decl + " > " + all_declarations;
	string command_sort_uniq = "sort " + all_declarations + " | uniq " + " > " + temp_file;
	string command_copy_back = "cat " + temp_file + " > " + all_declarations;

	try {
		system(command_cat_declarations.c_str());
		system(command_sort_uniq.c_str());
		system(command_copy_back.c_str());
	} catch (...) {
		cerr << "Exception occurred in merging declarations files"<< endl;
	}

	//merge all definitions
	string all_definitions = "all_definitions";
	string command_cat_definitions = "cat " + constraints_definitions_file + " " + micro_arr_defn + " > " + all_definitions;

	try {
		system(command_cat_definitions.c_str());
	} catch (...) {
		cerr << "Exception occurred in merging definitions files" << endl;
	}

	//merge all assertions
	string all_assertions = "all_assertions";
	string command_cat_assertions = "cat " + constraints_assertions_file + " " + micro_arr_assert + " > " + all_assertions;

	try {
		system(command_cat_assertions.c_str());
	} catch (...) {
		cerr << "Exception occurred in merging assertions files" << endl;
	}

	//generate final z3-acceptable script
	string command_final_constrains = "cat " + all_declarations + " " + all_definitions + " " + all_assertions + " " + footer + " > " + final_z3_script;

	try {
		system(command_final_constrains.c_str());
	} catch (...) {
		cerr << "Exception occurred in generating final z3 script file" << endl;
	}

	//run z3
	string command_run_z3 = "z3 " + final_z3_script + " > " + z3_outcome;

	try {
		system(command_run_z3.c_str());
	} catch (...) {
		cerr << "Exception occurred in running z3 on the constraints" << endl;
	}

	//convert the output of z3 into the format usable for if SAT

	ifstream file_in(z3_outcome.c_str());
	string first_line_z3_result;
	getline(file_in, first_line_z3_result);
	if(first_line_z3_result == "unsat"){
		cout << "Result is UNSAT" << endl;
	}
	else if(first_line_z3_result == "sat") {
		string subgraph_readable_file;
		cout << "Result is SAT. Enter the filepath to which the result is stored in the subgraph command readable format" << endl;
		cin >> subgraph_readable_file;
		//string command_z3_to_readable = "grep -A1 edge_.*_present " + z3_outcome + " | tr '\n' ' ' | sed 's/define-fun//g;s/() Bool    //g;s/(/\\n(/g;s/--//g;s/( //g;s/)//g' | sed -e '1d' | sed 's/edge_//g;s/to/ /g;s/_present//g;s/ \{1,\}/ /g;s/false /false/g;s/true /true/g'" + " > " + subgraph_readable_file;
		string command_z3_to_readable = "./z3_soln_edge_assignments.sh " + z3_outcome + " > " + subgraph_readable_file;
		try {
			system(command_z3_to_readable.c_str());
		} catch (...) {
			cerr << "Exception occurred in converting z3 output to readable form" << endl;
		}
	}
	else{
		cout << "Error during z3 run: " + first_line_z3_result << endl;
	}
}

*/

int GraphManager::count_GErel_targets_with_other_NON_GErel_incoming_edge(const int &graphnum)
{
	Graph *graph = get_graph_from_index(graphnum);
	int count = 0;
	for (map<string, Node *>::const_iterator itr = (graph->rep_id_to_node_map).begin(); itr != (graph->rep_id_to_node_map).end(); ++itr)
	{
		int ge_rel_count = 0;
		int non_ge_rel_count = 0;
		for (set<Edge *>::const_iterator edge_itr = itr->second->inlist.begin(); edge_itr != itr->second->inlist.end(); edge_itr++)
		{
			if ((*edge_itr)->edge_type == "GErel") // not complete add more to be added
				++ge_rel_count;
			else if ((*edge_itr)->edge_type != "GErel")
				++non_ge_rel_count;
		}
		if (ge_rel_count > 0 && non_ge_rel_count > 0)
		{
			count++;
			cout << "Desired GErel target" << itr->second->rep_id_name << endl;
		}
	}
	return count;
}

void GraphManager::print_GErel_edges(const int &graphnum)
{
	Graph *graph = get_graph_from_index(graphnum);

	string filename;
	cout << "Enter file path to which to write GErel edges" << endl;
	cin >> filename;

	ofstream fout(filename.c_str());

	bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
	bvatom edge_subtype_express_bv = get_bvatom_for_edge_subtype("expression");
	bvatom edge_subtype_repress_bv = get_bvatom_for_edge_subtype("repression");

	set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->rep_id_to_node_map); // get all the edge ids (bvatom type) of the graph

	for (set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr)
	{ // iterate over the edge ids
		// get current edge subtype and type as bvatom
		bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_subtype");
		bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_type");

		if (bvm->checkBVEquality(edge_type_GErel, curr_edge_type_bv))
		{

			bvatom source_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "source_node_id");
			unsigned long source_id_ulong = bvm->convertBVToUnsignedInt(source_id_bv);
			string source_id_str = get_hsa_id_from_local_node_id((unsigned long)source_id_ulong);

			bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
			unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
			string target_id_str = get_hsa_id_from_local_node_id((unsigned long)target_id_ulong);

			bvatom pathway_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "pathway_id");
			vector<string> all_pathways_of_curr_edge;

			for (int i = 0; i < PATHWAY_ID_LENGTH; ++i)
			{
				bvatom pathway_at_curr_bit_posn = bvm->select_with_int(pathway_id_bv, (unsigned long)i, (unsigned long)i);
				unsigned long pathway_at_curr_bit_long = bvm->convertBVToUnsignedInt(pathway_at_curr_bit_posn);
				if (pathway_at_curr_bit_long == 1)
				{ // that i th position from right is 1 so the corresponding patway is present
					for (map<string, int>::iterator itr = pathwayEncodingMap.begin(); itr != pathwayEncodingMap.end(); ++itr)
					{
						if (itr->second == i)
						{
							all_pathways_of_curr_edge.push_back(itr->first);
							break;
						}
					}
				}
			}

			fout << "\nPaths: ";
			for (vector<string>::iterator itr = all_pathways_of_curr_edge.begin(); itr != all_pathways_of_curr_edge.end(); ++itr)
			{
				fout << *itr << " ";
			}
			fout << "; ";

			if (bvm->checkBVEquality(edge_subtype_express_bv, curr_edge_subtype_bv))
			{
				fout << source_id_str << " over-expresses " << target_id_str << endl;
			}
			else if (bvm->checkBVEquality(edge_subtype_repress_bv, curr_edge_subtype_bv))
			{
				fout << source_id_str << " represses " << target_id_str << endl;
			}
		}
	}
	fout << endl;
	fout.close();
}

void GraphManager::print_GErel_targets(const int &graphnum)
{
	Graph *graph = get_graph_from_index(graphnum);
	bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
	string filename;
	cout << "Enter file path to which to write all GErel edge target node ids" << endl;
	cin >> filename;

	ofstream fout(filename.c_str());

	set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->rep_id_to_node_map); // get all the edge ids (bvatom type) of the graph

	if (!fout)
	{
		cerr << "Error: can't open file " + filename << endl;
	}
	else
	{
		for (set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr)
		{ // iterate over the edge ids
			// get current edge type as bvatom
			bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_type");

			if (bvm->checkBVEquality(edge_type_GErel, curr_edge_type_bv))
			{
				bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
				unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
				string target_id_str = get_hsa_id_from_local_node_id((unsigned long)target_id_ulong);
				fout << target_id_str << endl;
			}
		}
		fout.close();
		string command = "sort -u " + filename + " > temp-GE ; cat temp-GE > " + filename + " ; rm temp-GE";
		system(command.c_str());
	}
}

void GraphManager::print_all_node_names(const int &graphnum)
{
	// string file = "all_39_pathway_nodes";
	string file;
	cout << "Enter file name to store the display names of all the nodes " << endl;
	cin >> file;
	ofstream out(file.c_str());
	Graph *graph = get_graph_from_index(graphnum);
	for (map<string, Node *>::const_iterator itr = (graph->rep_id_to_node_map).begin(); itr != (graph->rep_id_to_node_map).end(); ++itr)
	{
		out << itr->second->rep_display_name << "\n"
			<< flush;
	}
}

void GraphManager::bvm_testing()
{
	cout << "in bvm_testing" << endl;
	string s = "10110100";
	bvatom s_bv = bvm->getBVatom(8, s);
	cout << bvm->printBVasBinString(s_bv) << endl;
	bvatom curr_bv = bvm->select_with_int(s_bv, (unsigned long)5, (unsigned long)2);
	cout << bvm->printBVasBinString(curr_bv) << endl;
}

void GraphManager::print_pathway_encoding()
{
	for (map<string, int>::iterator itr = pathwayEncodingMap.begin(); itr != pathwayEncodingMap.end(); ++itr)
	{
		cout << itr->first << " : " << itr->second << endl;
	}
}

void GraphManager::print_display_names_of_hsa_ids(const Graph *graph)
{
	const map<string, Node *> &map_of_nodes_ref = graph->rep_id_to_node_map;

	string filename;
	cout << "Enter file path with each line an hsa id" << endl;
	cin >> filename;

	ifstream fin(filename.c_str());
	string curr_hsa_id;
	while (getline(fin, curr_hsa_id))
	{
		for (map<string, Node *>::const_iterator itr = map_of_nodes_ref.begin(); itr != map_of_nodes_ref.end(); ++itr)
		{
			if (itr->second->rep_id_name == curr_hsa_id)
			{
				cout << itr->second->rep_display_name << endl;
			}
		}
	}
}

void GraphManager::print_list_strings(list<string> &list_str, ofstream &fout)
{
	for (list<string>::iterator str_itr = list_str.begin(); str_itr != list_str.end(); ++str_itr)
	{
		fout << *str_itr << endl;
	}
}

void GraphManager::print_all_pathways_in_graph(Graph *graph)
{
	string filename;
	cout << "Enter file path to which to write pathways" << endl;
	cin >> filename;
	ofstream fout(filename.c_str());

	for (map<string, Node *>::const_iterator itr = (graph->rep_id_to_node_map).begin(); itr != (graph->rep_id_to_node_map).end(); ++itr)
	{
		for (set<Edge *>::const_iterator edge_itr = itr->second->inlist.begin(); edge_itr != itr->second->inlist.end(); edge_itr++)
		{
			print_list_strings((*edge_itr)->belongs_to_pathways, fout);
		}
		for (set<Edge *>::const_iterator edge_itr = itr->second->outlist.begin(); edge_itr != itr->second->outlist.end(); edge_itr++)
		{
			print_list_strings((*edge_itr)->belongs_to_pathways, fout);
		}
	}
}

bool GraphManager::node_has_input_edge_of_type(Node *node, bvatom edge_type)
{
	for (set<Edge *>::iterator edge_itr = node->inlist.begin(); edge_itr != node->inlist.end(); ++edge_itr)
	{
		bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id((*edge_itr)->edge_id, "edge_type");
		if (bvm->checkBVEquality(edge_type, curr_edge_type_bv))
		{
			return true;
		}
	}
	return false;
}

bool GraphManager::node_has_output_edge_of_type(Node *node, bvatom edge_type)
{
	for (set<Edge *>::iterator edge_itr = node->outlist.begin(); edge_itr != node->outlist.end(); ++edge_itr)
	{
		bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id((*edge_itr)->edge_id, "edge_type");
		if (bvm->checkBVEquality(edge_type, curr_edge_type_bv))
		{
			return true;
		}
	}
	return false;
}

bool GraphManager::node_has_input_edge_of_subtype(Node *node, bvatom edge_subtype)
{
	for (set<Edge *>::iterator edge_itr = node->inlist.begin(); edge_itr != node->inlist.end(); ++edge_itr)
	{
		bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id((*edge_itr)->edge_id, "edge_subtype");
		if (!bvm->checkBVForZero(bvm->bitwise_and(edge_subtype, curr_edge_subtype_bv)))
		{
			return true;
		}
	}
	return false;
}

bool GraphManager::node_has_output_edge_of_subtype(Node *node, bvatom edge_subtype)
{
	for (set<Edge *>::iterator edge_itr = node->outlist.begin(); edge_itr != node->outlist.end(); ++edge_itr)
	{
		bvatom curr_edge_subtype_bv = get_field_as_bv_from_edge_id((*edge_itr)->edge_id, "edge_subtype");
		if (!bvm->checkBVForZero(bvm->bitwise_and(edge_subtype, curr_edge_subtype_bv)))
		{
			return true;
		}
	}
	return false;
}

void GraphManager::print_equivalence_class_from_rep_id(string repid)
{
	cout << "Equivalence class for " << repid << "is :\n";
	map<string, string>::iterator itr = id_equiv_class_map.find(repid);

	if (itr == id_equiv_class_map.end())
	{
		return;
	}
	else
	{
		cout << itr->second << endl;
		while (itr->first != itr->second)
		{
			itr = id_equiv_class_map.find(itr->second);
			cout << itr->second << endl;
		}
	}
	cout << endl;
}

void GraphManager::write_microarr_assert_to_file(int graphnum)
{
	string up_reg_filename, down_reg_filename, prefix;

	cout << "\nEnter file for up regulated entries: ";
	cin >> up_reg_filename;
	cout << "\nEnter file for down regulated entries: ";
	cin >> down_reg_filename;
	cout << "\nEnter output file prefix: ";
	cin >> prefix;

	string nodeId;
	ifstream ifs_up(up_reg_filename.c_str());
	ifstream ifs_down(down_reg_filename.c_str());
	set<string> up_reg_hsa_ids, down_reg_hsa_ids;
	set<string> graph_hsa_ids;
	while (ifs_up >> nodeId)
	{
		up_reg_hsa_ids.insert(get_rep_id_name_iteratively(nodeId));
	}
	cout << "No. of entities up regulated: " << up_reg_hsa_ids.size() << endl;
	while (ifs_down >> nodeId)
	{
		down_reg_hsa_ids.insert(get_rep_id_name_iteratively(nodeId));
	}
	cout << "No. of entities down regulated: " << down_reg_hsa_ids.size() << endl;

	Graph *graph = get_graph_from_index(graphnum);
	map<string, Node *> node_map = graph->rep_id_to_node_map;

	bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
	set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->rep_id_to_node_map); // get all the edge ids (bvatom type) of the graph
	for (set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr)
	{ // iterate over the edge ids
		// get current edge type as bvatom
		bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_type");

		if (bvm->checkBVEquality(edge_type_GErel, curr_edge_type_bv))
		{
			bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
			unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
			string target_id_str = get_hsa_id_from_local_node_id((unsigned long)target_id_ulong);
			graph_hsa_ids.insert(target_id_str);
		}
	}

	cout << "No. of entities in the graph: " << graph_hsa_ids.size() << endl;

	// Entities both up and down regulated
	set<string> up_and_down_reg;
	std::set_intersection(up_reg_hsa_ids.begin(), up_reg_hsa_ids.end(), down_reg_hsa_ids.begin(), down_reg_hsa_ids.end(), std::inserter(up_and_down_reg, up_and_down_reg.begin()));
	cout << "Up and down reg has " << up_and_down_reg.size() << " elements" << endl;

	set<string> up_reg_minus_common, down_reg_minus_common;
	std::set_difference(up_reg_hsa_ids.begin(), up_reg_hsa_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), std::inserter(up_reg_minus_common, up_reg_minus_common.begin()));
	std::set_difference(down_reg_hsa_ids.begin(), down_reg_hsa_ids.end(), up_and_down_reg.begin(), up_and_down_reg.end(), std::inserter(down_reg_minus_common, down_reg_minus_common.begin()));

	// Entities up regulated common with graph
	set<string> up_reg_in_graph;
	std::set_intersection(up_reg_minus_common.begin(), up_reg_minus_common.end(), graph_hsa_ids.begin(), graph_hsa_ids.end(), std::inserter(up_reg_in_graph, up_reg_in_graph.begin()));
	cout << "Up reg has " << up_reg_in_graph.size() << " elements common with graph" << endl;

	// Entities down regulated common with graph
	set<string> down_reg_in_graph;
	std::set_intersection(down_reg_minus_common.begin(), down_reg_minus_common.end(), graph_hsa_ids.begin(), graph_hsa_ids.end(), std::inserter(down_reg_in_graph, down_reg_in_graph.begin()));
	cout << "Down reg has " << down_reg_in_graph.size() << " elements common with graph" << endl;

	// Graph minus up regulated entities
	set<string> temp;
	set<string> graph_unchanged_entities;
	std::set_difference(graph_hsa_ids.begin(), graph_hsa_ids.end(), up_reg_in_graph.begin(), up_reg_in_graph.end(), std::inserter(temp, temp.begin()));
	// Graph minus up minus down regulated entities
	std::set_difference(temp.begin(), temp.end(), down_reg_in_graph.begin(), down_reg_in_graph.end(), std::inserter(graph_unchanged_entities, graph_unchanged_entities.begin()));

	string assert_filename = prefix + "_assert";
	// string color_map_filename = prefix + "_color_map";

	ofstream ofs_assert(assert_filename.c_str(), ofstream::out);
	// ofstream ofs_color(color_map_filename.c_str(),ofstream::out);

	set<string>::iterator itr_set;
	// up regulated nodes present in the graph
	for (itr_set = up_reg_in_graph.begin(); itr_set != up_reg_in_graph.end(); itr_set++)
	{
		ofs_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
		ofs_assert << "(assert " << (*itr_set) << "_over_expressed)" << endl;
	}

	// down regulated nodes present in the graph
	for (itr_set = down_reg_in_graph.begin(); itr_set != down_reg_in_graph.end(); itr_set++)
	{
		ofs_assert << "(assert " << (*itr_set) << "_expression_changes)" << endl;
		ofs_assert << "(assert (not " << (*itr_set) << "_over_expressed))" << endl;
	}

	// unchanged nodes in the graph
	for (itr_set = graph_unchanged_entities.begin(); itr_set != graph_unchanged_entities.end(); itr_set++)
	{
		ofs_assert << "(assert (not " << (*itr_set) << "_expression_changes))" << endl;
	}
	ofs_assert.close();
}

void GraphManager::write_microarr_decl_to_file(int graphnum)
{
	Graph *graph = get_graph_from_index(graphnum);
	string decl_file;
	cout << "Enter the file to which to write microarr declarations\n ";
	cin >> decl_file;

	ofstream fout(decl_file.c_str());

	set<string> all_targets;

	if (!fout)
	{
		cerr << "Can't open file " + decl_file + " for writing microarray declarations\n";
	}
	else
	{
		bvatom edge_type_GErel = get_bvatom_for_edge_type("GErel");
		set<bvatom> set_edge_ids = get_all_edge_ids_from_node_map(graph->rep_id_to_node_map); // get all the edge ids (bvatom type) of the graph
		for (set<bvatom>::iterator edge_itr = set_edge_ids.begin(); edge_itr != set_edge_ids.end(); ++edge_itr)
		{ // iterate over the edge ids
			// get current edge type as bvatom
			bvatom curr_edge_type_bv = get_field_as_bv_from_edge_id(*edge_itr, "edge_type");

			if (bvm->checkBVEquality(edge_type_GErel, curr_edge_type_bv))
			{
				bvatom target_id_bv = get_field_as_bv_from_edge_id(*edge_itr, "target_node_id");
				unsigned long target_id_ulong = bvm->convertBVToUnsignedInt(target_id_bv);
				string target_id_str = get_hsa_id_from_local_node_id((unsigned long)target_id_ulong);
				all_targets.insert(target_id_str);
			}
		}

		for (set<string>::iterator itr = all_targets.begin(); itr != all_targets.end(); ++itr)
		{
			string decl1 = "(declare-const " + *itr + "_expression_changes Bool)";
			string decl2 = "(declare-const " + *itr + "_over_expressed Bool)";
			fout << decl1 << endl;
			fout << decl2 << endl;
		}

		fout.close();
	}
}

string GraphManager::get_string_prefix(string input, char delim)
{
	int ind = input.find_first_of(delim);
	if (ind == string::npos)
		return input;
	return input.substr(0, ind);
}

// fill node_array with the the nodes that are the source nodes
// of the edges of edge_type directed to target_node
void GraphManager::get_source_nodes(set<Node *> &source_node_array, Node *target_node, edge_t edge_type)
{
	bvatom edge_type_parser_bv = get_bvatom_for_parser_edge_t(edge_type);

	for (set<Edge *>::iterator inlist_itr = target_node->inlist.begin(); inlist_itr != target_node->inlist.end(); ++inlist_itr)
	{
		bvatom inlist_curr_edge_id_bv = (*inlist_itr)->edge_id;
		bvatom curr_edge_type = get_field_as_bv_from_edge_id(inlist_curr_edge_id_bv, "edge_type");
		if (bvm->checkBVEquality(edge_type_parser_bv, curr_edge_type))
		{
			source_node_array.insert((*inlist_itr)->edge_source);
		}
	}
}

// fill node_array with the the nodes that are the source nodes
// of the edges of subtype directed to target_node
void GraphManager::get_source_nodes(set<Node *> &source_node_array, Node *target_node, edge_subt edge_subtype)
{
	bvatom edge_subtype_parser_bv = get_bvatom_for_parser_edge_subt(edge_subtype);

	for (set<Edge *>::iterator inlist_itr = target_node->inlist.begin(); inlist_itr != target_node->inlist.end(); ++inlist_itr)
	{
		bvatom inlist_curr_edge_id_bv = (*inlist_itr)->edge_id;
		bvatom curr_edge_subtype = get_field_as_bv_from_edge_id(inlist_curr_edge_id_bv, "edge_subtype");
		if (!bvm->checkBVForZero(bvm->bitwise_and(edge_subtype_parser_bv, curr_edge_subtype)))
		{
			source_node_array.insert((*inlist_itr)->edge_source);
		}
	}
}

// fill node_array with the the nodes that are the target nodes
// of the edges of type emanating from source_node
void GraphManager::get_target_nodes(set<Node *> &target_node_array, Node *source_node, edge_t edge_type)
{
	bvatom edge_type_parser_bv = get_bvatom_for_parser_edge_t(edge_type);

	for (set<Edge *>::iterator outlist_itr = source_node->outlist.begin(); outlist_itr != source_node->outlist.end(); ++outlist_itr)
	{
		bvatom outlist_curr_edge_id_bv = (*outlist_itr)->edge_id;
		bvatom curr_edge_type = get_field_as_bv_from_edge_id(outlist_curr_edge_id_bv, "edge_type");
		if (bvm->checkBVEquality(edge_type_parser_bv, curr_edge_type))
		{
			target_node_array.insert((*outlist_itr)->edge_target);
		}
	}
}

// fill node_array with the the nodes that are the target nodes
// of the edges of subtype emanating from source_node
void GraphManager::get_target_nodes(set<Node *> &target_node_array, Node *source_node, edge_subt edge_subtype)
{
	bvatom edge_subtype_parser_bv = get_bvatom_for_parser_edge_subt(edge_subtype);

	for (set<Edge *>::iterator outlist_itr = source_node->outlist.begin(); outlist_itr != source_node->outlist.end(); ++outlist_itr)
	{
		bvatom outlist_curr_edge_id_bv = (*outlist_itr)->edge_id;
		bvatom curr_edge_subtype = get_field_as_bv_from_edge_id(outlist_curr_edge_id_bv, "edge_subtype");
		if (!bvm->checkBVForZero(bvm->bitwise_and(edge_subtype_parser_bv, curr_edge_subtype)))
		{
			target_node_array.insert((*outlist_itr)->edge_target);
		}
	}
}

/*
void GraphManager::fill_node_tuple(quantifier_t quant, const string& curr_var_name, Graph* graph, set_nodes_t* set_nodes, set<map<string, Node*> * >& node_tuple){
	//base cases are with no operator
	if(set_nodes->opr == NONE_OPR){
		if(quant == FOR_ALL_QUANT && set_nodes->fun_name == ALL_NODES_FUN){
			if(node_tuple.empty()){
				//if empty add all nodes with curr_var_name as there identifier
				for(map<string, Node*>::iterator graph_itr = graph->map_of_nodes.begin(); graph_itr != graph->map_of_nodes.end(); ++graph_itr){
					map<string, Node*> tuple_as_map;
					tuple_as_map.insert(pair<string, Node*>(curr_var_name, graph_itr->second));
					node_tuple.insert(&tuple_as_map);
				}
			}
			else{
				//if not empty then for each existing tuple create new tuples with all nodes
				set<map<string, Node*> * > temp_set;
				set<map<string, Node*> * >::iterator tuple_itr;
				for(tuple_itr = node_tuple.begin(); tuple_itr != node_tuple.end(); ++tuple_itr){
					for(map<string, Node*>::iterator graph_itr = graph->map_of_nodes.begin(); graph_itr != graph->map_of_nodes.end(); ++graph_itr){
						map<string, Node*> tuple_as_map(*(*tuple_itr));
						tuple_as_map.insert(pair<string, Node*>(curr_var_name,graph_itr->second));
						temp_set.insert(&tuple_as_map);
					}
				}
				node_tuple.clear();

				for(tuple_itr = temp_set.begin(); tuple_itr != temp_set.end(); ++tuple_itr){
					node_tuple.insert(*tuple_itr);
				}
			}
		}

		else if(quant == FOR_ALL_QUANT && (set_nodes->fun_name == EDGE_TYPE_FUN || set_nodes->fun_name == EDGE_SUBTYPE_FUN)){
			string arg1(set_nodes->arg1);
			string arg2(set_nodes->arg2);
			edge_t edge_type_subtype = set_nodes->arg3_edge;
			//map<string, pair<quantifier_t, vector<Node*> > >::iterator anteced_map_itr;
			set<map<string, Node*> * > temp_set;
			set<map<string, Node*> * >::iterator tuple_itr;
			map<string, Node*>::iterator map_itr;

			//curr_var_name becomes source node
			if(arg1 == curr_var_name){
				//anteced_map_itr = antecedent_map.find(arg2);
				if(node_tuple.empty()){
					cerr << "Error: existing tuples shouldn't be empty for EDGE(x,y,type) cases, exiting . . ." << endl;
					exit(1);
				}
				for(tuple_itr = node_tuple.begin(); tuple_itr != node_tuple.end(); ++tuple_itr){
					map<string, Node*>* temp_map = *tuple_itr;
					map_itr = temp_map->find(arg2);
					if(map_itr == temp_map->end()){
						cerr << "The argument variable " << arg2 << " couldn't be found, continuing . . ." << endl;
						continue;
					}
					else{
						vector<Node*> source_nodes;
						get_source_nodes(source_nodes, map_itr->second, edge_type_subtype);
						//insert news tuples along with source nodes
						for(vector<Node*>::iterator source_node_itr = source_nodes.begin(); source_node_itr != source_nodes.end(); ++source_node_itr){
							//append the source nodes to existing tuples
							map<string, Node*>* temp_map_new = new map<string, Node*>(*temp_map);
							temp_map_new->insert(pair<string, Node*>(curr_var_name, *source_node_itr));
							temp_set.insert(temp_map_new);
						}
					}
				}
				node_tuple.clear();

				for(tuple_itr = temp_set.begin(); tuple_itr != temp_set.end(); ++tuple_itr){
					node_tuple.insert(*tuple_itr);
				}
			}

			//curr_var_name becomes target node
			else if(arg2 == curr_var_name){
				if(node_tuple.empty()){
					cerr << "Error: existing tuples shouldn't be empty for EDGE(x,y,type) cases, exiting . . ." << endl;
					exit(1);
				}
				for(tuple_itr = node_tuple.begin(); tuple_itr != node_tuple.end(); ++tuple_itr){
					map<string, Node*>* temp_map = *tuple_itr;
					map_itr = temp_map->find(arg1);
					if(map_itr == temp_map->end()){
						cerr << "The argument variable " << arg1 << " couldn't be found, continuing . . ." << endl;
						continue;
					}
					else{
						vector<Node*> target_nodes;
						get_target_nodes(target_nodes, map_itr->second, edge_type_subtype);
						//insert news tuples along with source nodes
						for(vector<Node*>::iterator target_node_itr = target_nodes.begin(); target_node_itr != target_nodes.end(); ++target_node_itr){
							//append the source nodes to existing tuples
							map<string, Node*>* temp_map_new = new map<string, Node*>(*temp_map);
							temp_map_new->insert(pair<string, Node*>(curr_var_name, *target_node_itr));
							temp_set.insert(temp_map_new);
						}
					}
				}
			}

			else{
				cerr << "Error: current variable name doesn't match one of the arguments, exiting . . ." << endl;
				exit(1);
			}
		}

		else if(quant == FOR_ALL_QUANT && set_nodes->fun_name == PRED_FUN){
			string arg1(set_nodes->arg1);
			set<map<string, Node*> * > temp_set;
			set<map<string, Node*> * >::iterator tuple_itr;
			map<string, Node*>::iterator map_itr;
			if(node_tuple.empty()){
				cerr << "Error: existing tuples shouldn't be empty for PRED(x) cases, exiting . . ." << endl;
				exit(1);
			}
			for(tuple_itr = node_tuple.begin(); tuple_itr != node_tuple.end(); ++tuple_itr){
				map<string, Node*>* temp_map = *tuple_itr;
				map_itr = temp_map->find(arg1);
				if(map_itr == temp_map->end()){
					cerr << "The argument variable " << arg1 << " couldn't be found, continuing . . ." << endl;
					continue;
				}
				else{
					//get all nodes from inlist of a node as pred
					for(set<Edge*>::iterator inlist_itr = map_itr->second->inlist.begin(); inlist_itr != map_itr->second->inlist.end(); ++inlist_itr){
						map<string, Node*>* temp_map_new = new map<string, Node*>(*temp_map);
						temp_map_new->insert(pair<string, Node*>(curr_var_name, inlist_itr->edge_source));
						temp_set.insert(temp_map_new);
					}
				}
			}
			node_tuple.clear();

			for(tuple_itr = temp_set.begin(); tuple_itr != temp_set.end(); ++tuple_itr){
				node_tuple.insert(*tuple_itr);
			}
		}

		else if(quant == FOR_ALL_QUANT && set_nodes->fun_name == SUCC_FUN){
			string arg1(set_nodes->arg1);
			set<map<string, Node*> * > temp_set;
			set<map<string, Node*> * >::iterator tuple_itr;
			map<string, Node*>::iterator map_itr;
			if(node_tuple.empty()){
				cerr << "Error: existing tuples shouldn't be empty for SUCC(x) cases, exiting . . ." << endl;
				exit(1);
			}
			for(tuple_itr = node_tuple.begin(); tuple_itr != node_tuple.end(); ++tuple_itr){
				map<string, Node*>* temp_map = *tuple_itr;
				map_itr = temp_map->find(arg1);
				if(map_itr == temp_map->end()){
					cerr << "The argument variable " << arg1 << " couldn't be found, continuing . . ." << endl;
					continue;
				}
				else{
					//get all nodes from inlist of a node as pred
					for(set<Edge*>::iterator outlist_itr = map_itr->second->outlist.begin(); outlist_itr != map_itr->second->outlist.end(); ++outlist_itr){
						map<string, Node*>* temp_map_new = new map<string, Node*>(*temp_map);
						temp_map_new->insert(pair<string, Node*>(curr_var_name, outlist_itr->edge_target));
						temp_set.insert(temp_map_new);
					}
				}
			}
			node_tuple.clear();

			for(tuple_itr = temp_set.begin(); tuple_itr != temp_set.end(); ++tuple_itr){
				node_tuple.insert(*tuple_itr);
			}
		}

		else if(quant == FOR_ALL_QUANT && set_nodes->fun_name == NODE_TYPE_FUN){
			node_t node_type = set_nodes->arg3_node;
			string node_type_str = get_node_type_str_from_node_type_parser(node_type);

			set<map<string, Node*> * > temp_set;
			set<map<string, Node*> * >::iterator tuple_itr;
			map<string, Node*>::iterator map_itr;

			//if there are no tuples
			//then all those nodes that are of the given type
			if(node_tuple.empty()){
				for(map<string, Node*>::iterator graph_itr = graph->map_of_nodes.begin(); graph_itr != graph->map_of_nodes.end(); ++graph_itr){
					map<string, Node*> tuple_as_map;
					if(graph_itr->second->node_type == node_type_str){
						tuple_as_map.insert(pair<string, Node*>(curr_var_name, graph_itr->second));
						node_tuple.insert(&tuple_as_map);
					}
				}
			}
			//if there are already tuples then append all nodes of node_type
			//with each of them
			else{
				for(tuple_itr = node_tuple.begin(); tuple_itr != node_tuple.end(); ++tuple_itr){
					for(map<string, Node*>::iterator graph_itr = graph->map_of_nodes.begin(); graph_itr != graph->map_of_nodes.end(); ++graph_itr){
						if(graph_itr->second->node_type == node_type_str){
							map<string, Node*> tuple_as_map(*(*tuple_itr));
							tuple_as_map.insert(pair<string, Node*>(curr_var_name,graph_itr->second));
							temp_set.insert(&tuple_as_map);
						}
					}
				}
			}
			node_tuple.clear();

			for(tuple_itr = temp_set.begin(); tuple_itr != temp_set.end(); ++tuple_itr){
				node_tuple.insert(*tuple_itr);
			}
		}


	}
}
*/

vector<t_Expression *> GraphManager::get_microarray_constraints(string micro_arr_decl, string micro_arr_assert, t_ExpressionManager *em)
{
	ifstream fin(micro_arr_decl.c_str());
	ifstream fin1(micro_arr_assert.c_str());
	if (fin.fail() || fin1.fail())
	{
		cerr << "Unable to open micro array declaration/definition/assertions/conflicting_nodes file(s) "; // << constraint_file << endl;
	}

	string s;

	TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};
	while (fin >> s)
	{
		if (s[0] != '(')
			continue;

		fin >> s;

		if (getExpressionMap.find(s) == getExpressionMap.end())
		{
			cerr << "The variable should have been declared in the graph constraints\n";
			getExpressionMap[s] = em->createSymbol(s, te);
			getCNFIndexMap[getExpressionMap[s]] = make_pair(s, -1);
		}

		fin >> s;
	}

	vector<t_Expression *> micro_constraints;

	while (fin1 >> s)
	{
		if (s[0] != '(')
			continue;

		fin1 >> s;

		if (s[0] != '(')
		{
			s = s.substr(0, s.length() - 1);
			micro_constraints.push_back(getExpressionMap[s]);
		}
		else
		{
			fin1 >> s;
			s = s.substr(0, s.length() - 2);
			micro_constraints.push_back(em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, getExpressionMap[s]));
		}
	}

	return micro_constraints;
}

// appends edge upper bound constraints to the file CNF_filename (w/o the upper bound value)
// returns vector of internal variables storing the actual upper bound
vector<int> GraphManager::write_edgecount_cnf(string CNF_filename, map<t_Expression *, pair<string, int>> getCNFIndexMap, int &idx, int &clauses)
{
	vector<vector<int>> add_constraints;
	ofstream fout(CNF_filename.c_str(), ofstream::out | ofstream::app);

	ofstream fout1("mapping", ofstream::out);
	for (map<t_Expression *, pair<string, int>>::iterator it = getCNFIndexMap.begin(); it != getCNFIndexMap.end(); it++)
	{
		string s = (it->second).first;
		if (s.substr(0, 4) == "edge")
		{
			if ((it->second).second == -1)
				(it->second).second = idx++;

			// fout<<(it->second).first<<" "<<(it->second).second<<endl;
			vector<int> v;
			v.push_back((it->second).second);
			add_constraints.push_back(v);
		}
		fout1 << (it->second).first << " " << (it->second).second << endl;
	}

	cout << "worked...!!\n";
	// fout<<"------------\n";
	while (add_constraints.size() > 1)
	{

		vector<vector<int>> new_cons;

		for (int i = 0; i < add_constraints.size(); i += 2)
		{
			if ((i + 1) < add_constraints.size())
			{
				new_cons.push_back(add_cnf(add_constraints[i], add_constraints[i + 1], idx, clauses, fout));
			}
			else
			{
				// should append zero here
				fout << -idx << " 0\n";
				clauses++;
				add_constraints[i].push_back(idx);
				idx++;

				new_cons.push_back(add_constraints[i]);
			}
		}

		add_constraints = new_cons;
	}

	cout << "worked...!!\n";

	vector<int> a = add_constraints[0];

	vector<int> b;

	cout << " ---- \n";
	for (int i = 0; i < a.size(); i++)
	{
		b.push_back(idx++);
		cout << b[i] << " " << a[i] << endl;
	}
	cout << " ---- \n";

	vector<int> temp;
	// checking if a < b

	for (int i = a.size() - 1; i >= 0; i--)
	{
		if (i == 0)
		{
			fout << b[i];
			for (int j = 0; j < temp.size(); j++)
				fout << " " << temp[j];
			fout << " 0\n";
			clauses++;

			fout << -a[i];
			for (int j = 0; j < temp.size(); j++)
				fout << " " << temp[j];
			fout << " 0\n";
			clauses++;
		}
		else
		{
			fout << b[i];
			for (int j = 0; j < temp.size(); j++)
				fout << " " << temp[j];
			fout << " " << -(idx) << " 0\n";
			clauses++;

			fout << -a[i];
			for (int j = 0; j < temp.size(); j++)
				fout << " " << temp[j];
			fout << " " << (-idx) << " 0\n";
			clauses++;

			temp.push_back(idx);
			idx++;

			fout << -b[i] << " " << a[i];
			for (int j = 0; j < temp.size(); j++)
				fout << " " << temp[j];
			fout << " 0\n";
			clauses++;

			fout << -a[i] << " " << b[i];
			for (int j = 0; j < temp.size(); j++)
				fout << " " << temp[j];
			fout << " 0\n";
			clauses++;
		}
	}

	cout << "works ............!!\n";

	ofstream fout4("ind_var", ofstream::out);

	map<int, int> ind_vars;

	for (map<t_Expression *, pair<string, int>>::iterator it = getCNFIndexMap.begin(); it != getCNFIndexMap.end(); it++)
	{
		if ((it->second).second != -1)
		{
			fout4 << (it->second).second << endl;
			ind_vars[(it->second).second] = 1;
		}
	}

	ofstream fout5("dep_var", ofstream::out);

	for (int i = 1; i < idx; i++)
	{
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

set<Node *> GraphManager::find_set_of_nodes_for_tuple(string &var_name, Graph *graph, set_nodes_t *set_nodes_parser, map<string, Node *> &tuple_ref)
{
	// find the set of nodes as specified by set of nodes from the parser set_nodes_parser
	set<Node *> set_nodes_to_return;
	// base cases are the ones that have noe operator in set_nodes_parser
	if (set_nodes_parser->opr == NONE_OPR)
	{
		if (set_nodes_parser->fun_name == ALL_NODES_FUN)
		{
			for (map<string, Node *>::iterator graph_itr = graph->rep_id_to_node_map.begin(); graph_itr != graph->rep_id_to_node_map.end(); ++graph_itr)
			{
				set_nodes_to_return.insert(graph_itr->second);
			}
		}

		else if (set_nodes_parser->fun_name == NODE_TYPE_FUN)
		{
			node_t node_type = set_nodes_parser->arg3_node;
			string node_type_str = get_node_type_str_from_node_type_parser(node_type);
			for (map<string, Node *>::iterator graph_itr = graph->rep_id_to_node_map.begin(); graph_itr != graph->rep_id_to_node_map.end(); ++graph_itr)
			{
				if (graph_itr->second->node_type == node_type_str)
				{
					set_nodes_to_return.insert(graph_itr->second);
				}
			}
		}

		else if (set_nodes_parser->fun_name == EDGE_TYPE_FUN || set_nodes_parser->fun_name == EDGE_SUBTYPE_FUN)
		{
			if (tuple_ref.empty())
			{
				cerr << "Error: existing tuples shouldn't be empty for EDGE_TYPE(y,x,...) AND EDGE_SUBTYPE(y,x,...) cases, exiting . . ." << endl;
				exit(1);
			}
			string arg1(set_nodes_parser->arg1);
			string arg2(set_nodes_parser->arg2);

			map<string, Node *>::iterator map_itr;

			// curr_var_name becomes source node
			if (arg1 == var_name)
			{
				map_itr = tuple_ref.find(arg2);
				if (map_itr == tuple_ref.end())
				{
					cerr << "The argument variable " << arg2 << " couldn't be found, continuing . . ." << endl;
				}
				else
				{
					if (set_nodes_parser->fun_name == EDGE_TYPE_FUN)
					{
						edge_t edge_type = set_nodes_parser->arg3_edge;
						get_source_nodes(set_nodes_to_return, map_itr->second, edge_type);
					}
					else if (set_nodes_parser->fun_name == EDGE_SUBTYPE_FUN)
					{
						edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
						get_source_nodes(set_nodes_to_return, map_itr->second, edge_subtype);
					}
				}
			}

			// curr_var_name becomes target node
			else if (arg2 == var_name)
			{
				map_itr = tuple_ref.find(arg1);
				if (map_itr == tuple_ref.end())
				{
					cerr << "The argument variable " << arg1 << " couldn't be found, continuing . . ." << endl;
				}
				else
				{
					if (set_nodes_parser->fun_name == EDGE_TYPE_FUN)
					{
						edge_t edge_type = set_nodes_parser->arg3_edge;
						get_target_nodes(set_nodes_to_return, map_itr->second, edge_type);
					}
					else if (set_nodes_parser->fun_name == EDGE_SUBTYPE_FUN)
					{
						edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
						get_target_nodes(set_nodes_to_return, map_itr->second, edge_subtype);
					}
				}
			}

			else
			{
				cerr << "Error: current variable name doesn't match one of the arguments, exiting . . ." << endl;
				exit(1);
			}
		}

		else if (set_nodes_parser->fun_name == PRED_FUN || set_nodes_parser->fun_name == SUCC_FUN)
		{
			if (tuple_ref.empty())
			{
				cerr << "Error: existing tuples shouldn't be empty for PRED(x) or SUCC(x) cases, exiting . . ." << endl;
				exit(1);
			}
			string arg1(set_nodes_parser->arg1);
			map<string, Node *>::iterator map_itr;
			map_itr = tuple_ref.find(arg1);

			if (set_nodes_parser->fun_name == SUCC_FUN)
			{
				for (set<Edge *>::iterator outlist_itr = map_itr->second->outlist.begin(); outlist_itr != map_itr->second->outlist.end(); ++outlist_itr)
				{
					set_nodes_to_return.insert((*outlist_itr)->edge_target);
				}
			}
			else if (set_nodes_parser->fun_name == PRED_FUN)
			{
				for (set<Edge *>::iterator inlist_itr = map_itr->second->inlist.begin(); inlist_itr != map_itr->second->inlist.end(); ++inlist_itr)
				{
					set_nodes_to_return.insert((*inlist_itr)->edge_source);
				}
			}
		}

		else if (set_nodes_parser->fun_name == NODE_INPUT_EDGE_TYPE_FUN || set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_TYPE_FUN || set_nodes_parser->fun_name == NODE_INPUT_EDGE_SUBTYPE_FUN || set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_SUBTYPE_FUN)
		{
			// go over each node and check if its incoming or outgoing edge as required satisfies the criteria and accordingly insert it into the set
			for (map<string, Node *>::iterator graph_itr = graph->rep_id_to_node_map.begin(); graph_itr != graph->rep_id_to_node_map.end(); ++graph_itr)
			{
				bool insert_node = false; // when this tag is true then insert the current node
				if (set_nodes_parser->fun_name == NODE_INPUT_EDGE_TYPE_FUN)
				{
					edge_t edge_type = set_nodes_parser->arg3_edge;
					bvatom edge_type_bv = get_bvatom_for_parser_edge_t(edge_type);
					insert_node = node_has_input_edge_of_type(graph_itr->second, edge_type_bv);
				}
				else if (set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_TYPE_FUN)
				{
					edge_t edge_type = set_nodes_parser->arg3_edge;
					bvatom edge_type_bv = get_bvatom_for_parser_edge_t(edge_type);
					insert_node = node_has_output_edge_of_type(graph_itr->second, edge_type_bv);
				}
				else if (set_nodes_parser->fun_name == NODE_INPUT_EDGE_SUBTYPE_FUN)
				{
					edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
					bvatom edge_subtype_bv = get_bvatom_for_parser_edge_subt(edge_subtype);
					insert_node = node_has_input_edge_of_subtype(graph_itr->second, edge_subtype_bv);
				}
				else if (set_nodes_parser->fun_name == NODE_OUTPUT_EDGE_SUBTYPE_FUN)
				{
					edge_subt edge_subtype = set_nodes_parser->arg3_edgesub;
					bvatom edge_subtype_bv = get_bvatom_for_parser_edge_subt(edge_subtype);
					insert_node = node_has_output_edge_of_subtype(graph_itr->second, edge_subtype_bv);
				}
				if (insert_node)
				{
					set_nodes_to_return.insert(graph_itr->second);
				}
			}
		}

		else if (set_nodes_parser->fun_name == NONE_FUN)
		{
			cerr << "NONE_FUN seen, exiting . . ." << endl;
			exit(1);
		}
	}

	// for other case make recursive calls//for other case make recursive calls
	else if (set_nodes_parser->opr == OR_OPR || set_nodes_parser->opr == AND_OPR)
	{
		set<Node *> left_set = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser->left, tuple_ref);
		set<Node *> right_set = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser->right, tuple_ref);
		if (set_nodes_parser->opr == OR_OPR)
		{
			set_union(left_set.begin(), left_set.end(), right_set.begin(), right_set.end(), inserter(set_nodes_to_return, set_nodes_to_return.begin()));
		}
		else if (set_nodes_parser->opr == AND_OPR)
		{
			set_intersection(left_set.begin(), left_set.end(), right_set.begin(), right_set.end(), inserter(set_nodes_to_return, set_nodes_to_return.begin()));
		}
	}
	else if (set_nodes_parser->opr == NOT_OPR)
	{
		set<Node *> to_negate_set = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser->left, tuple_ref);
		set<Node *> universe_set;
		for (map<string, Node *>::iterator graph_itr = graph->rep_id_to_node_map.begin(); graph_itr != graph->rep_id_to_node_map.end(); ++graph_itr)
		{
			universe_set.insert(graph_itr->second);
		}
		set_difference(universe_set.begin(), universe_set.end(), to_negate_set.begin(), to_negate_set.end(), inserter(set_nodes_to_return, set_nodes_to_return.begin()));
	}
	return set_nodes_to_return;
}

void GraphManager::fill_temp_tuple_set(quantifier_t quant, string &var_name, set<Node *> &set_nodes_for_current_tuple, map<string, Node *> &tuple_ref, set<map<string, Node *>> &temp_tuple_set)
{
	if (quant == FOR_ALL_QUANT)
	{
		// if the quantifier is for all then we append all the elements of set_nodes_for_current_tuple
		for (set<Node *>::iterator node_set_itr = set_nodes_for_current_tuple.begin(); node_set_itr != set_nodes_for_current_tuple.end(); node_set_itr++)
		{
			map<string, Node *> tuple_ref_new(tuple_ref);
			tuple_ref_new.insert(pair<string, Node *>(var_name, *node_set_itr));
			temp_tuple_set.insert(tuple_ref_new);
		}
	}
	else if (quant == THERE_EXISTS_QUANT)
	{
		cout << "THERE_EXISTS case - Yet to be handled, exiting" << endl;
		exit(1);
	}
	else
	{
		cerr << "quantifier missing in antecedent, exiting . . " << endl;
		exit(1);
	}
}

void GraphManager::fill_set_tuple_for_antecedent(quantifier_t quant, string &var_name, Graph *graph, set_nodes_t *set_nodes_parser, set<map<string, Node *>> &set_tuples)
{
	// iterate over each of the previous tuples to get the set of nodes for current identifier (e.g. y in FOR_ALL y)
	// for each tuple find the set of nodes that satisfy the requirement of current antecedent
	// keep adding newly created tuples with appended Nodes to a temporary set which finally replaces the contents of set_tuples

	// if the tuple set is empty then fill it first
	if (set_tuples.empty())
	{
		map<string, Node *> dummy_tuple;
		set<Node *> set_nodes_for_current_tuple = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser, dummy_tuple);
		for (set<Node *>::iterator node_set_itr = set_nodes_for_current_tuple.begin(); node_set_itr != set_nodes_for_current_tuple.end(); node_set_itr++)
		{
			// map<string, Node*>* tuple = new map<string, Node*>;
			map<string, Node *> tuple;
			tuple.insert(pair<string, Node *>(var_name, *node_set_itr));
			set_tuples.insert(tuple);
		}
		return;
	}

	// set<map<string, Node*> * > temp_tuple_set;//its contents will replace the contents of set_tuple, when we are done with the current antecedent
	set<map<string, Node *>> temp_tuple_set; // its contents will replace the contents of set_tuple, when we are done with the current antecedent
	// set<map<string, Node*> * >::iterator tuple_set_itr;
	set<map<string, Node *>>::iterator tuple_set_itr;
	for (tuple_set_itr = set_tuples.begin(); tuple_set_itr != set_tuples.end(); ++tuple_set_itr)
	{
		map<string, Node *> tuple_curr = *tuple_set_itr;
		set<Node *> set_nodes_for_current_tuple = find_set_of_nodes_for_tuple(var_name, graph, set_nodes_parser, tuple_curr);
		fill_temp_tuple_set(quant, var_name, set_nodes_for_current_tuple, tuple_curr, temp_tuple_set);
	}

	// replace old tuple set with new tuples
	set_tuples.clear();

	for (tuple_set_itr = temp_tuple_set.begin(); tuple_set_itr != temp_tuple_set.end(); ++tuple_set_itr)
	{
		set_tuples.insert(*tuple_set_itr);
	}
}

void GraphManager::print_single_tuple(map<string, Node *> &tuple_ref)
{
	map<string, Node *>::iterator tuple_itr;
	for (tuple_itr = tuple_ref.begin(); tuple_itr != tuple_ref.end(); ++tuple_itr)
	{
		cout << tuple_itr->first << ":" << tuple_itr->second->rep_id_name << " ";
	}
}

void GraphManager::print_all_tuples(set<map<string, Node *>> &set_tuples)
{
	cout << endl;
	set<map<string, Node *>>::iterator tuple_set_itr;
	int i = 1;
	for (tuple_set_itr = set_tuples.begin(); tuple_set_itr != set_tuples.end(); ++tuple_set_itr)
	{
		cout << "Tuple number: " << i << endl;
		map<string, Node *> tuple_curr = *tuple_set_itr;
		print_single_tuple(tuple_curr);
		cout << endl;
		++i;
	}
}

// antecdent list is stored in reverse order in parsing, so we need this function that
// accesses antecdents in the right order
void GraphManager::access_antecdents_for_filling_tuples(list_antecedent_t *antecedent_list, set<map<string, Node *>> &set_tuples, Graph *graph)
{
	if (antecedent_list == NULL)
	{
		cerr << "Error: no antecdents, exiting . . . " << endl;
		exit(1);
	}
	if (antecedent_list->next == NULL)
	{
		quantifier_t quant = antecedent_list->quantifier;
		string var_name(antecedent_list->var);
		fill_set_tuple_for_antecedent(quant, var_name, graph, antecedent_list->set_nodes, set_tuples);
	}
	else
	{
		access_antecdents_for_filling_tuples(antecedent_list->next, set_tuples, graph);
		quantifier_t quant = antecedent_list->quantifier;
		string var_name(antecedent_list->var);
		fill_set_tuple_for_antecedent(quant, var_name, graph, antecedent_list->set_nodes, set_tuples);
	}
}

t_Expression *GraphManager::get_constraints_for_rule(rule_t *rule_ptr, Graph *graph, set<Node *> &set_potential_conflct_nodes, t_ExpressionManager *em)
{
	// map<string, pair<quantifier_t, vector<Node*> > > antecedent_map;
	list_antecedent_t *antecedent_list = rule_ptr->antecedent;
	// set<map<string, Node*> * > set_tuples; //this set will store the final tuples obtained by going through all antecedents
	// set<map<string, Node*> > set_tuples; //this set will store the final tuples obtained by going through all antecedents (antecedents come before colon)
	// access_antecdents_for_filling_tuples(antecedent_list, set_tuples, graph);

	static int i = 1;
	RuleTupleStore tuple_store_obj(antecedent_list, graph, this);
	// cout << "\nprinting tuples for rule: " << i << endl;
	tuple_store_obj.print_all_tuples();
	++i;
	// for testing just print the tuples
	// print_all_tuples(set_tuples);

	/*consequent_t* consequent_curr_rule = rule_ptr->consequent;
	logical_stmt_t*  left_of_implication = consequent_curr_rule->left_of_implies;
	logical_stmt_t* right_of_implication = consequent_curr_rule->right_of_implies;*/

	t_Expression *constraint_for_rule = tuple_store_obj.get_constraint_em(rule_ptr->consequent, em, this);
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

	// now constraints_for_a_rule contains all constraints for a rule pointed to by rule_ptr, all these need to be ANDed
}

void GraphManager::generate_constraints_using_rules_parser(const int graph_num, list<rule_t *> &list_of_rules, set<Node *> &set_potential_conflict_nodes, t_ExpressionManager *em)
{
	Graph *graph = get_graph_from_index(graph_num);
	vector<t_Expression *> constraints_for_all_rules_vec;
	for (list<rule_t *>::iterator rule_itr = list_of_rules.begin(); rule_itr != list_of_rules.end(); rule_itr++)
	{
		rule_t *curr_rule_ptr = *rule_itr;
		t_Expression *cons_for_rule = get_constraints_for_rule(curr_rule_ptr, graph, set_potential_conflict_nodes, em);
		if (cons_for_rule)
		{
			constraints_for_all_rules_vec.push_back(cons_for_rule);
		}
	}

	vector<t_Expression *>::iterator expr_vec_itr = constraints_for_all_rules_vec.begin();
	t_Expression *expr1 = *expr_vec_itr;
	assert(expr1);
	++expr_vec_itr;
	while (expr_vec_itr != constraints_for_all_rules_vec.end())
	{
		t_Expression *expr2 = *expr_vec_itr;
		assert(expr2);
		expr1 = build_AND_constraints(expr1, expr2, 0, 0, em);
		++expr_vec_itr;
	}

	t_Expression *constraints_all_rules = expr1;

	char choice;
	cout << "\nEnter 'y' to include microarray data in the constraints, \n or any other key to exclude microarray data ";
	cin >> choice;

	if (choice == 'y')
	{
		string micro_arr_decl;
		string micro_arr_assert;

		cout << "\nEnter the relative filepath to micro array declarations: ";
		cin >> micro_arr_decl;

		cout << "\nEnter the relative filepath to micro array assertions: ";
		cin >> micro_arr_assert;

		/*string micro_arr_decl = "psmd9_decl";
		string micro_arr_assert = "psmd9_assert";*/

		expr_vec_itr = constraints_for_all_rules_vec.begin();
		vector<t_Expression *> graph_micrarr_constraints = get_microarray_constraints(micro_arr_decl, micro_arr_assert, em);
		while (expr_vec_itr != constraints_for_all_rules_vec.end())
		{
			graph_micrarr_constraints.push_back(*expr_vec_itr);
			++expr_vec_itr;
		}

		char solve;
		cout << "\nEnter 'y' to solve the total constraints, \n or any other key to not ";
		cin >> solve;

		if (solve == 'y')
		{
			solve_constraints(graph_micrarr_constraints, em);
		}
	}
	else
	{
		char solve;
		cout << "\nEnter 'y' to solve constraints without microarray data, \n or any other key to not ";
		cin >> solve;

		if (solve == 'y')
		{
			solve_constraints(constraints_for_all_rules_vec, em);
		}
	}

	// string test_constraints_out_file = "test";
	ofstream fout("test_em");

	if (fout.is_open())
	{
		// iterate over each rule and write constraints for each rule
		// em->printExpressionsToFileInSMT(constraints_all_rules, "test_smt");
		em->printExpressionToFileAsDAG("constraintsAllRules ", constraints_all_rules, fout);
		fout.close();
	}
	else
	{
		cerr << "\nUnable to open the output file stream for constraints testing " << endl;
	}
}

void GraphManager::solve_constraints(vector<t_Expression *> cons, t_ExpressionManager *em)
{

	string CNF_file = "CNF_file";
	string z3_out_file = "z3_out_file";
	string z3_error_file = "z3_error_file";
	string bin_search_report = "bin_search_report";

	ofstream fout(CNF_file.c_str());
	string tmp_file = "temp_file";
	string tmp_file2 = "temp_file2";
	int idx = 1, clauses = 0;
	em->printExpressionsToFileInCNF(cons, tmp_file, idx, clauses, getCNFIndexMap);
	vector<int> upper_bound_val;
	ofstream fout2(tmp_file2.c_str());
	string cmd = "cat " + tmp_file + " >> " + CNF_file + " ; " + "cat " + tmp_file2 + " >> " + CNF_file;
	int upper_bound_cur;
	int upper_bound_user;
	int time_out;

	ofstream fout3(bin_search_report.c_str());
	ifstream fin;

	bool loop_forver = true;

	while (loop_forver)
	{
		char choice;
		cout << "\nEnter 'b' to do the binary-search-based constraint solving with metric as upper bound on total number of edges ";
		cout << "\nEnter 'u' to do constraint solving with metric as just a fixed upper bound on total number of edges ";
		cout << "\nEnter 'n' to do constraint solving without any metric ";
		cin >> choice;

		switch (choice)
		{
		case 'b':
			loop_forver = false;
			upper_bound_val = write_edgecount_cnf(tmp_file, getCNFIndexMap, idx, clauses);

			cout << "\nEnter the time to wait for each run(in seconds) : ";
			cin >> time_out;

			upper_bound_cur = 1;

			// increasing the upper_bound by 2 times for each run in the loop
			for (int i = 0; i <= upper_bound_val.size(); i++)
			{

				if (i == upper_bound_val.size())
					upper_bound_cur--;

				fout.close();
				fout2.close();

				fout.open(CNF_file.c_str());
				fout2.open(tmp_file2.c_str());

				fout << "p cnf " << idx - 1 << " " << clauses << endl;

				upper_bound_user = upper_bound_cur;
				for (int i = 0; i < upper_bound_val.size(); i++)
				{
					if (upper_bound_user % 2 == 0)
						fout2 << -upper_bound_val[i] << " 0\n";
					else
						fout2 << upper_bound_val[i] << " 0\n";
					upper_bound_user /= 2;
				}
				fout.close();
				fout2.close();
				system((cmd + "; timeout " + IntToString(time_out) + " z3 -dimacs " + CNF_file + " > " + z3_out_file + " 2> " + z3_error_file).c_str());
				// sleep(time_out+3);

				fout3 << " Upper bound : " << upper_bound_cur << endl;

				string s;
				fin.close();
				fin.open(z3_error_file.c_str());
				fin >> s;
				if (s.length() > 0)
				{
					fout3 << "Error in cnf input file format";
					break;
				}

				fin.close();
				fin.open(z3_out_file.c_str());
				fin >> s;

				if (s == "sat")
				{
					fout3 << s << endl;
					break;
				}
				else if (s == "unsat")
					fout3 << s << endl;
				else
					fout3 << "timed out\n";

				upper_bound_cur *= 2;
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
			loop_forver = false;
			upper_bound_val = write_edgecount_cnf(tmp_file, getCNFIndexMap, idx, clauses);

			cout << "\nEnter upper bound ";
			cin >> upper_bound_user;

			fout << "p cnf " << idx - 1 << " " << clauses << endl;

			for (int i = 0; i < upper_bound_val.size(); i++)
			{
				if (upper_bound_user % 2 == 0)
					fout2 << -upper_bound_val[i] << " 0\n";
				else
					fout2 << upper_bound_val[i] << " 0\n";
				upper_bound_user /= 2;
			}
			fout.close();
			fout2.close();
			system(cmd.c_str());

			break;
		case 'n':
			loop_forver = false;
			break;
		default:
			cout << "\nError: invalid choice ";
			break;
		}
	}
}

t_Expression *GraphManager::get_node_constraint(Node *curr_node, node_state_t node_state, t_ExpressionManager *em)
{
	t_Expression *expression_to_return = NULL;
	TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};
	string node_variable;
	if (node_state == ACTIVATED_STATE)
	{
		node_variable = curr_node->rep_id_name + "_active";
	}
	else if (node_state == INHIBITED_STATE)
	{
		node_variable = curr_node->rep_id_name + "_active";
	}
	else if (node_state == OVEREXPRESSED_STATE)
	{
		node_variable = curr_node->rep_id_name + "_over_expressed";
	}
	else if (node_state == REPRESSED_STATE)
	{
		node_variable = curr_node->rep_id_name + "_over_expressed";
	}
	else
	{
		cerr << "Node state isn't set, exiting . . ." << endl;
		exit(1);
	}
	if (getExpressionMap.find(node_variable) == getExpressionMap.end())
	{
		expression_to_return = em->createSymbol(node_variable, te1);
		getExpressionMap[node_variable] = expression_to_return;
		getCNFIndexMap[expression_to_return] = make_pair(node_variable, -1);
	}
	else
	{
		expression_to_return = getExpressionMap[node_variable];
	}

	if (node_state == INHIBITED_STATE || node_state == REPRESSED_STATE)
	{
		expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
	}
	// em->printExpressionToFileAsDAG("expr_to_return", expression_to_return, cout);
	return expression_to_return;
}

t_Expression *GraphManager::get_node_constraint(Node *curr_node, node_state2_t node2_state, t_ExpressionManager *em)
{
	t_Expression *expression_to_return = NULL;
	TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};
	string node_variable;
	if (node2_state == DIFFER_EXPRESSED_STATE || NOT_DIFFER_EXPRESSED_STATE)
	{
		node_variable = curr_node->rep_id_name + "_expression_changes";
	}
	else
	{
		cerr << "Node state isn't set, exiting . . ." << endl;
		exit(1);
	}
	if (getExpressionMap.find(node_variable) == getExpressionMap.end())
	{
		expression_to_return = em->createSymbol(node_variable, te1);
		getExpressionMap[node_variable] = expression_to_return;
		getCNFIndexMap[expression_to_return] = make_pair(node_variable, -1);
	}
	else
	{
		expression_to_return = getExpressionMap[node_variable];
	}

	if (node2_state == NOT_DIFFER_EXPRESSED_STATE)
		expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
	// em->printExpressionToFileAsDAG("expr_to_return", expression_to_return, cout);
	return expression_to_return;
}

// match happens when the edge type match exactly and when the edge subtype list of parser is a subset of the edge subtype list of the graph Edge
bool GraphManager::edge_type_subtype_match(Edge *edge_ptr, edge_t edge_type, list_edge_subtypes_t *list_edge_subtypes)
{
	// get bvatoms for types
	bvatom edge_type_bv = get_bvatom_for_parser_edge_t(edge_type);
	string edge_ptr_type_str = edge_ptr->edge_type;
	bvatom edge_ptr_type_bv = get_bvatom_for_edge_type(edge_ptr_type_str);

	// get bvatoms for subtypes
	bvatom edge_subtypes_list_bv = get_bvatom_for_parser_edge_subtype_list(list_edge_subtypes);
	bvatom edge_ptr_list_subt_bv = get_bvatom_for_edge_subtype_list(edge_ptr->edge_subtype);

	// p is a bitwise-subset of q iff (NOT p OR q) has all bits set to 1
	bool edge_type_match = bvm->checkBVEquality(edge_type_bv, edge_ptr_type_bv);
	bool edge_subtype_list_subset_match = bvm->checkBVForZero(bvm->bitwise_negate(bvm->bitwise_or(bvm->bitwise_negate(edge_subtypes_list_bv), edge_ptr_list_subt_bv)));
	return edge_type_match && edge_subtype_list_subset_match;
}

t_Expression *GraphManager::get_edge_constraint(Node *node1, Node *node2, edge_state_t edge_state, edge_t edge_type, list_edge_subtypes_t *list_edge_subtypes, t_ExpressionManager *em)
{
	assert(node1 != NULL);
	assert(node2 != NULL);

	// first a check needs to be performed that the edge actually exists from node1 to node2
	bool edge_exists_from_node1_to_node2 = false;
	Edge *edge_to_consider;
	set<Edge *>::iterator edge_set_itr;
	for (edge_set_itr = node1->outlist.begin(); edge_set_itr != node1->outlist.end(); ++edge_set_itr)
	{
		if ((*edge_set_itr)->edge_target == node2 && edge_type_subtype_match(*edge_set_itr, edge_type, list_edge_subtypes))
		{
			edge_exists_from_node1_to_node2 = true;
			edge_to_consider = *edge_set_itr;
			break;
		}
	}

	if (!edge_exists_from_node1_to_node2)
	{
		// check if there is an edge from node2 to node1
		bool edge_exists_from_node2_to_node1 = false;
		for (edge_set_itr = node2->outlist.begin(); edge_set_itr != node2->outlist.end(); ++edge_set_itr)
		{
			if ((*edge_set_itr)->edge_target == node1 && edge_type_subtype_match(*edge_set_itr, edge_type, list_edge_subtypes))
			{
				edge_exists_from_node2_to_node1 = true;
				break;
			}
		}

		if (edge_exists_from_node2_to_node1)
		{
			cerr << "Error: there is no edge of desired type from " + node1->rep_id_name + " to " + node2->rep_id_name +
						", but there exists edge of desired type from " + node2->rep_id_name + " to " + node1->rep_id_name;
		}
		else
		{
			cerr << "Error: there is no edge of desired type from " + node1->rep_id_name + " to " + node2->rep_id_name +
						", also there is no edge of desired type from " + node2->rep_id_name + " to " + node1->rep_id_name;
		}

		cerr << "\nExiting . . ." << endl;
		exit(1);
	}

	t_Expression *expression_to_return = NULL;
	TypeOfExpressionTuple te1 = {TYPE_UNSIGNED_BITVECTOR, 1};

	bvatom edge_subtype_bv = get_bvatom_for_edge_subtype_list(edge_to_consider->edge_subtype);
	string edge_subtypes_str = get_edge_subtype_string_from_bvatom(edge_subtype_bv);
	string edge_type_str = edge_to_consider->edge_type;
	string edge_variable = "edge." + node1->rep_id_name + "to" + node2->rep_id_name + "." + edge_type_str + "-" + edge_subtypes_str + "present";

	if (getExpressionMap.find(edge_variable) == getExpressionMap.end())
	{
		expression_to_return = em->createSymbol(edge_variable, te1);
		getExpressionMap[edge_variable] = expression_to_return;
		getCNFIndexMap[expression_to_return] = make_pair(edge_variable, -1);
	}
	else
	{
		expression_to_return = getExpressionMap[edge_variable];
	}

	if (edge_state != PRESENT_STATE)
	{
		expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expression_to_return);
	}

	assert(expression_to_return != NULL);
	return expression_to_return;
}

t_Expression *GraphManager::get_constraint_for_tuple(map<string, Node *> &curr_tuple_ref, logical_stmt_t *left_of_implication, logical_stmt_t *right_of_implication, t_ExpressionManager *em)
{
	t_Expression *left_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, left_of_implication, em);
	t_Expression *right_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, right_of_implication, em);

	assert(left_expr != NULL);
	assert(right_expr != NULL);
	// em->printExpressionToFileAsDAG("left_constraint", left_expr, cout);
	// em->printExpressionToFileAsDAG("right_constraint", right_expr, cout);

	t_Expression *implication_expr = NULL;
	implication_expr = build_OR_constraints(left_expr, right_expr, 1, 0, em); // this is basically implication left_expr -> right_expr
	assert(implication_expr != NULL);
	// em->printExpressionToFileAsDAG("implication_constraint", implication_expr, cout);
	return implication_expr;
}

t_Expression *GraphManager::get_constraint_for_logic_stmt_and_tuple(map<string, Node *> &curr_tuple_ref, logical_stmt_t *logic_stmt, t_ExpressionManager *em)
{
	t_Expression *expression_to_return = NULL;
	map<string, Node *>::iterator tuple_itr;
	// base case
	if (logic_stmt->opr == NONE_OPR)
	{
		if (logic_stmt->fun_name2 == NODE_FUN2)
		{
			string arg1(logic_stmt->arg1);
			tuple_itr = curr_tuple_ref.find(arg1);
			if (tuple_itr == curr_tuple_ref.end())
			{
				cerr << "There is no identifier " + arg1 + " in the tuples generated, exiting . . ." << endl;
				exit(1);
			}
			else if (logic_stmt->state_node != NODE_NONE_STATE)
			{
				expression_to_return = get_node_constraint(tuple_itr->second, logic_stmt->state_node, em);
			}
			else if (logic_stmt->state_node2 != NODE_NONE2_STATE)
			{
				expression_to_return = get_node_constraint(tuple_itr->second, logic_stmt->state_node2, em);
			}
			else
			{
				cerr << "\ndebug node state issues ";
				exit(1);
			}
		}
		else if (logic_stmt->fun_name2 == EDGE_FUN2)
		{
			string arg1(logic_stmt->arg1);
			string arg2(logic_stmt->arg2);
			Node *node1 = NULL, *node2 = NULL;
			tuple_itr = curr_tuple_ref.find(arg1);
			if (tuple_itr == curr_tuple_ref.end())
			{
				cerr << "There is no identifier " + arg1 + " in the tuples generated, exiting . . ." << endl;
				exit(1);
			}
			else
			{
				node1 = tuple_itr->second;
			}
			tuple_itr = curr_tuple_ref.find(arg2);
			if (tuple_itr == curr_tuple_ref.end())
			{
				cerr << "There is no identifier " + arg2 + " in the tuples generated, exiting . . ." << endl;
				exit(1);
			}
			else
			{
				node2 = tuple_itr->second;
			}
			assert(node1 != NULL && node2 != NULL);
			expression_to_return = get_edge_constraint(node1, node2, logic_stmt->state_edge, logic_stmt->arg3, logic_stmt->arg4_list, em);
		}
		else
		{
			cerr << "Empty logical statement . . ., exiting . . ." << endl;
			exit(1);
		}
	}

	else if (logic_stmt->opr == OR_OPR || logic_stmt->opr == AND_OPR)
	{
		t_Expression *left_of_operator_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, logic_stmt->left, em);
		t_Expression *right_of_operator_expr = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, logic_stmt->right, em);

		if (logic_stmt->opr == OR_OPR)
		{
			expression_to_return = build_OR_constraints(left_of_operator_expr, right_of_operator_expr, 0, 0, em);
		}
		else if (logic_stmt->opr == AND_OPR)
		{
			expression_to_return = build_AND_constraints(left_of_operator_expr, right_of_operator_expr, 0, 0, em);
		}
	}

	else if (logic_stmt->opr == NOT_OPR)
	{
		t_Expression *expr_to_neg = get_constraint_for_logic_stmt_and_tuple(curr_tuple_ref, logic_stmt->left, em);
		expression_to_return = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expr_to_neg);
	}
	return expression_to_return;
}
