#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <libxml++/libxml++.h>
//#include <libxml++.h>
#include <libxml++/parsers/domparser.h>
#include "GraphManager.h"
#include "GraphManagerNew.h"

class XMLParser : public xmlpp::DomParser
{
public:
		
	XMLParser();
	~XMLParser();

	//functions for the mergeable graphs
	void cartestian_product(vector<set<int> >& components_set, set<int>& comp_member_node_set);
	vector<int> get_component_list_for_xml_node(xmlpp::Node* node);
	bool cycle_in_components_for_node(int node_id, std::map<int, xmlpp::Node*>& file_id_node_map, set<int>& visited_nodes);
	bool cycle_in_components_relations(std::map<int, xmlpp::Node*>& file_id_node_map);
	bool fill_graph_from_xml_file(GraphNew* graph_ptr, string input_file, GraphManagerNew* graph_man, int thres, vector<string>& MAPK_merged_ids, map<string, set<string> >& MAPK_node_to_pathway_id_map, bool tool_written = false);
        //std::set<int> process_node(GraphNew* graph_ptr, xmlpp::Node* node, GraphManagerNew* graph_man, std::map<int, xmlpp::Node*>& file_id_node_map);
	std::set<int> process_node(GraphNew* graph_ptr, xmlpp::Node* node, GraphManagerNew* graph_man, std::map<int, xmlpp::Node*>& file_id_node_map, int thres);
        std::set<int> process_node_with_merge_exception(GraphNew* graph_ptr, xmlpp::Node* node, GraphManagerNew* graph_man, std::map<int, xmlpp::Node*>& file_id_node_map, int thres, string pathway, vector<string>& MAPK_merged_ids, map<string, set<string> >& MAPK_node_to_pathway_id_map);
	void process_edge(GraphNew* graph_ptr, int source_node_id, int target_node_id, xmlpp::Node* edge, GraphManagerNew* graph_man, string pathway, bool tool_written = false);

	//functions for the graph as it is(unmergeable)
	/*
	void fill_graph_from_xml_file_as_it_is(GraphNew* graph_ptr, string input_file, GraphManagerNew* graph_man);
	int process_node_as_it_is(GraphNew* graph_ptr, int node_id_in_file, GraphManagerNew* graph_man, std::map<int, xmlpp::Node*>& file_id_node_map);
	*/

	//old functions
	map<string, Node *> createGraphFromXML(GraphManager *gm, string inputfilename);
	map<string, Node *> createGraphFromXMLNew(GraphManager *gm, string inputfilename);



};

class Species;
class Reaction;

class SBMLParser
{
public:
	SBMLParser();
	~SBMLParser();
	
        void create_map_from_sbml_file(ofstream& mymap, string sbmlfile);
	bool fill_graph_from_sbml_file(GraphNew* graph_ptr, string input_file, GraphManagerNew* graph_man);
	void process_edge_sbml(GraphNew* graph_ptr, int source_node_id, int target_node_id, const Reaction * species, GraphManagerNew* graph_man, string pathway, bool tool_written);
        
        int process_node_sbml(GraphNew* graph_ptr, const Species * annotation, GraphManagerNew* graph_man);
        int process_node_sbml(GraphNew* graph_ptr, const Reaction * reaction, GraphManagerNew* graph_man);
        vector<int> get_component_list_for_sbml_node(Species * species);
        void cartestian_product_sbml(vector<set<int> >& components_set, set<int>& comp_member_node_set);
};


#endif
