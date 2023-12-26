#include <string>
#include <cstring>
#include <iostream>
#include "../include/GraphManager.h"
using namespace std;


Node::Node(string name) {
	node_id = id_counter++;
	rep_id_name = name;
	rep_display_name = name;
	potential_activation_conflict = false;
	potential_expression_conflict = false;
}


Node::Node(string name, string display_name, vector<Node*>& components, string type, string link) {
	node_id = id_counter++;
	rep_id_name = name; // considered as unique id
	rep_display_name = display_name;
	node_type = type;
	node_link = link;
	for(vector<Node*>::iterator itr = components.begin(); itr != components.end(); ++itr){
		this->components.push_back(*itr);
	}
	potential_activation_conflict = false;
	potential_expression_conflict = false;
}


Node::~Node() {
}



Edge::Edge(bvatom id, Node * source, Node * target) {
	edge_id = id; // considered as unique id
	edge_source = source;
	edge_target = target;
}
////
////Edge::Edge(bvatom id, Node * source, Node * target, string type, list<string> subtype, string pathway) {
////	edge_id = id; // considered as unique id
////	edge_source = source;
////	edge_target = target;
////	edge_type = type;
////	edge_subtype = subtype;
////	belongs_to_pathway = pathway;
////}

Edge::Edge(bvatom id, Node * source, Node * target, string type, list<string> subtype, list<string> pathways) {
	edge_id = id; // considered as unique id
	edge_source = source;
	edge_target = target;
	edge_type = type;
	edge_subtype = subtype;
	belongs_to_pathways = pathways;
}



Edge::~Edge() {
}


Graph::Graph(map<string, Node *> node_map, string graph_genesis_operation, list<unsigned int> operand_graphs, string other_params) {
	graph_id = graph_id_counter++;
	rep_id_to_node_map = node_map;
	
	//graph_genesis summary;
	genesis.oper = graph_genesis_operation;
	genesis.oper_graph_ids = operand_graphs;
	genesis.oper_parameters = other_params;
}

Graph::~Graph() {
}
