#define SUBTRACT_MODE 0
#define INTERSECT_MODE 1
#define MERGE_MODE 2
#define INHIBITION_EDGE 3
#define ACTIVATION_EDGE 4
#define ANY_EDGE 5
#define FORWARD 6
#define BACKWARD 7
#define FORWARD_BACKWARD 10
#define LOG_ACTIVATE 8
#define LOG_DEACTIVATE 9
#define BIN_SEARCH_ON_NODE_BOUND 10
#define BIN_SEARCH_ON_EDGE_BOUND 11
#define BIN_SEARCH_ON_PATH_BOUND 12
#define BIN_SEARCH_ON_NODE_RELAX_BOUND 13
#define BIN_SEARCH_ON_EDGE_RELAX_BOUND 14
#define NO_BIN_SEARCH 15
#define EXPERIMENTAL 16

#define DELIMITER_STRING_FOR_TYPE_AND_SUBTYPE_ENCODINGS " => "
#define DELIMITER_STRING_FOR_PATHWAY_ENCODINGS " => "

#define NODE_TYPE_LENGTH 4
#define NODE_ID_LENGTH (((unsigned int) sizeof(unsigned int))*8)
#define EDGE_TYPE_LENGTH 4
#define EDGE_SUBTYPE_LENGTH 32
//#define PATHWAY_ID_LENGTH 64
#define PATHWAY_ID_LENGTH 514

#define SUPPORTING_FILES_DIR "/home/sukanya/sukanya-cse-git-desktop/network_tool/supporting_files"
#define OUTPUT_FILES_DIR_PREFIX ""
//#define Z3_PATH_NAME "/home/sysbio/bin/newZ3/z3-master/build/z3"
#define Z3_PATH_NAME "/usr/bin/z3"
#define DOT_PATH_NAME "/usr/bin/dot "
//#define DOT_PATH_NAME "/usr/local/bin/sfdp "
#define PDF_VIEWER_PATH_NAME "/usr/bin/evince "
#define XDOT_PATH_NAME "/usr/bin/xdot "
//#define APPROXMC_PATH_NAME "/home/sysbio/ApproxMC_modified/ApproxMC.py"
#define APPROXMC_PATH_NAME "ApproxMC.py"
#define NODE_DISTANCE_BIT_SEQ_LENGTH 8 // harshit
//#define NODE_DISTANCE_VALUE_BIT_LENGTH 12 //16 // sukanya



//#define NODE_BOUND_BIT_LENGTH 10
#define NODE_BOUND_BIT_LENGTH(graph) ( (int) ceil(log2 ((double) graph->get_node_ids().size())) )
//#define EDGE_BOUND_BIT_LENGTH 12
#define EDGE_BOUND_BIT_LENGTH(graph) ( (int) ceil(log2 ((double) graph->get_edge_ids().size())) )
//#define PATH_BOUND_BIT_LENGTH 4//8
#define PATH_BOUND_BIT_LENGTH(path_bound)  ( (int) ceil(log2 ((double) path_bound)) )
//#define NODE_RELAX_BOUND_BIT_LENGTH NODE_BOUND_BIT_LENGTH
#define NODE_RELAX_BOUND_BIT_LENGTH(up_down_size) ( (int) ceil(log2 ((double) up_down_size))+1 )
#define EDGE_RELAX_BOUND_BIT_LENGTH(graph) EDGE_BOUND_BIT_LENGTH(graph)

#define inf INT_MAX
#define inff FLT_MAX
#define BIGNUM 200000

#define NSEQ_OFFSET 50



//#define GET_NODE_SEQ_NUM(graph, nid) ((nid+NSEQ_OFFSET - (graph->first_offset))% graph->get_node_ids().size())
//#define GET_NODE_ID(graph, nseq) ((nseq >= NSEQ_OFFSET && nseq < graph->get_node_ids().size()) ? nseq - NSEQ_OFFSET + graph->first_offset :  nseq - NSEQ_OFFSET + graph->first_offset + graph->get_node_ids().size())

#define GET_NODE_SEQ_NUM(graph, nid) (graph->map_nid_to_nseq[nid])
#define GET_NODE_ID(graph, nseq) (graph->map_nseq_to_nid[nseq])

#define GET_EDGE_SEQ_NUM(graph, eid) (graph->map_eid_to_eseq[eid])
#define GET_EDGE_ID(graph, eseq) (graph->map_eseq_to_eid[eseq])

#ifdef ONLY_INTERAC_EDGES_FOR_MINCUT
#define NOT_INTERAC_EDGE(graph, eid) ((graph->edge_has_subtype(eid, "component")) || (graph->edge_has_subtype(eid, "association")) || (graph->edge_has_subtype(eid, "dissociation")) || (graph->edge_has_subtype(eid, "mapping")))// || (graph->edge_has_subtype(eid, "indirect")))
//#define NOT_INTERAC_EDGE(graph, eid) ((graph->edge_has_subtype(eid, "component")) || (graph->edge_has_subtype(eid, "mapping")))
#else
#define NOT_INTERAC_EDGE(graph, eid) 0
#endif

#define EDGE_IS_EXPR_REPR(graph, eid) ((graph->edge_has_subtype(eid, "expression")) || (graph->edge_has_subtype(eid, "repression")))
#define EDGE_IS_ACT_INH_PHOS(graph, eid) ((graph->edge_has_subtype(eid, "activation")) || (graph->edge_has_subtype(eid, "inhibition")) || (graph->edge_has_subtype(eid, "phosphorylation")))

#define prev_along_diagonal(curr_value, gen_lbound) ((curr_value - 1 >= gen_lbound)? curr_value - 1 : gen_lbound);
#define next_along_diagonal(curr_value, gen_ubound) ((curr_value + 1 <= gen_ubound)? curr_value + 1 : gen_ubound);

#define FLUX_THRESHOLD 2.0

#define PRECISION 2
#define NORMALIZE(value, min_val, max_val) ( (value - min_val + 1) / (max_val - min_val + 1) )



//#define NORMALIZE(value, min_val, max_val) (value)
//#define NORMALIZE(value, min_val, max_val) ( ((value - min_val) / (max_val - min_val) ) < (1.0 / pow(10, PRECISION)) ? (1.0 / pow(10, PRECISION)) : ((value - min_val) / (max_val - min_val) ) )







enum SMT_solver{
    Z3, CVC4, BOOLECTOR
};


enum sat_state_t{
    UNDEFINED, SAT, UNSAT, TIMEOUT
};


#define TIMEOUT_IN_SEC 18000


#define NUM_OF_COLORS 5
#define COLOR_STR(idx) (palette_table[idx])

