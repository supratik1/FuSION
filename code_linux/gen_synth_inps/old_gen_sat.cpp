#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cassert>
#include <list>

using namespace std;

#define ACTIVATION -100 // Simple codes
#define INHIBITION -200 // Simple codes
#define UPREG_VAL 5.1
#define DOWNREG_VAL -5.1

#define SRC 1
#define TGT 2
#define VarIndex(i) (i+2)
#define VARGADGET_START(i) (2 + ((unsat_option == 0) ? 6:4) * (i-1) + 1)
#define VARGADGET_END(i) (2 + ((unsat_option == 0) ? 6:4) * i)
#define VARGADGET_A(i) VARGADGET_START(i)
#define VARGADGET_NA(i) VARGADGET_START(i) + 1
#define VARGADGET_B(i) VARGADGET_START(i) + 2
#define VARGADGET_NB(i) VARGADGET_START(i) + 3
#define VARGADGET_D(i) VARGADGET_START(i) + 4
#define VARGADGET_ND(i) VARGADGET_START(i) + 5

#define CLSGADGET_START(j,n) (2 + ((unsat_option == 0) ? 6:4)*n + 4*(j-1) + 1) 
#define CLSGADGET_END(j,n) (2 + ((unsat_option == 0) ? 6:4)*n + 4*j)
#define CLSGADGET_L1C(j,n) CLSGADGET_START(j,n)
#define CLSGADGET_L2C(j,n) CLSGADGET_START(j,n) + 1
#define CLSGADGET_L3C(j,n) CLSGADGET_START(j,n) + 2
#define CLSGADGET_LCC(j,n) CLSGADGET_START(j,n) + 3

#define NodeName(i) "hsa"+to_string(i)+"00"
#define NUM_CLAUSES(n) (8+(n-3))
#define FUNC_SIG_NODE(n, r, s) ((r+1)*(((s == 0) ? 6:4)*n + 4*NUM_CLAUSES(n)) + 3)
#define problemDirName(n, r, s) "unsat_v"+to_string(n)+"_r"+to_string(r)+"_s"+to_string(s);
#define graphXMLFileName(n, r, s) "graph_v"+to_string(n)+"_r"+to_string(r)+"_s"+to_string(s)+".xml"
#define foldchgFileName(n, r, s) "foldchg_v"+to_string(n)+"_r"+to_string(r)+"_s"+to_string(s)+".txt"
#define inpscriptFileName(n, r, s) "inpscript_v"+to_string(n)+"_r"+to_string(r)+"_s"+to_string(s)+".txt"
#define dimacsFileName(n, r, s) "unsat_v"+to_string(n)+"_r"+to_string(r)+"_s"+to_string(s)+".dimacs"

class c_GenSynthGraphs {
private:
  int unsat_option;
  int nvars, nclauses, node_relax_bound, edge_relax_bound, relax_bound;
  int total_vars, total_nodes, total_clauses, fsig_node;
  string work_dir, subdir;
  ofstream xf, lf, sf, df; // XMLGraph, LogFoldChange, InputScript, DIMACS files

public:
  c_GenSynthGraphs(int s, int n, int nrb, int erb, string wd) {
    unsat_option = s;
    nvars = n;
    node_relax_bound = nrb;
    edge_relax_bound = erb;
    work_dir = wd;
    relax_bound = node_relax_bound + edge_relax_bound;
    nclauses = NUM_CLAUSES(nvars);
    total_vars = (relax_bound + 1)*nvars;
    total_clauses = (relax_bound + 1)*nclauses;
    total_nodes = (relax_bound + 1)*(((unsat_option == 0) ? 6:4)*nvars + 4*nclauses) + 3;
    // extra 3 nodes are source, target and functionally significant node
    fsig_node = FUNC_SIG_NODE(nvars, relax_bound, unsat_option);
    subdir = work_dir + "/" + problemDirName(nvars, relax_bound, unsat_option);
    string create_dir_command = "mkdir -p " + subdir;
    system(create_dir_command.c_str());
  }
  ~c_GenSynthGraphs() {;}

  void genXMLGraph();
  void genLogFoldChangeFile();
  void genInputScriptFile();
  bool allParamsOK();
  
private:
  void genXMLPrelude();
  void addXMLNode(int i);
  void genXMLGadgetsForVars();
  void addXMLEdge(int edge_src, int edge_tgt, int edge_type);
  void genXMLGadgetsForClauses();
  void genXMLGadgetForFuncSigNode();
  void genXMLPostlude();
};
  
class c_threeCNFClause{
public:
  int l1, l2, l3;

  c_threeCNFClause() {l1 = 0; l2 = 0; l3 = 0;}
  ~c_threeCNFClause() {;}
  
};

int main(int argc, char *argv[])
{
  if (argc < 6) {
    cerr << "Incorrect number of arguments!" << endl;
    cerr << "Usage: " << string(argv[0]) << " unsat_option[0/1] num_vars node_relax_bound edge_relax_bound working_directory" << endl;
  }
  else {
    c_GenSynthGraphs sG(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), string(argv[5]));
    
    if (sG.allParamsOK()) {
      sG.genXMLGraph();
      sG.genLogFoldChangeFile();
      sG.genInputScriptFile();
    }
  }
}

bool c_GenSynthGraphs::allParamsOK()
{
  bool ok = true;
  if (nvars < 3) {
    cerr << "Need at least 3 variables (currently " << nvars << ")" << endl;
    ok = false;
  }
  if (node_relax_bound < 0) {
    cerr << "Need non-negative node relax bound (currently " << node_relax_bound << ")" << endl;
    ok = false;
  }
  if (edge_relax_bound < 0) {
    cerr << "Need non-negative edge relax bound (currently " << edge_relax_bound << ")" << endl;
    ok = false;
  }
  if ((unsat_option != 0) && (unsat_option != 1)) {
    cerr << "Need 0 or 1 for unsat formula option (currently " << unsat_option << ")" << endl;
    ok = false;
  }

  return ok;
}

void c_GenSynthGraphs::genXMLGraph()
{
  // Using an idea to generate unsat formulas from 
  // https://cstheory.stackexchange.com/questions/8117/minimum-unsatisfiable-3-cnf-formulae
  // Starting from an unsat formula of 8 clauses on 3 variables, the technique
  // can be used to generate unsat formulas with 8+(n-3) 3-CNF clauses on n
  // variables where n >= 3.  The #clauses grows linearly in n using this
  // technique as opposed to listing out all 2^n 3-CNF clauses on n variables.
  
  xf.open(subdir + "/" + graphXMLFileName(nvars, relax_bound, unsat_option), ios::out);
  df.open(subdir + "/" + dimacsFileName(nvars, relax_bound, unsat_option), ios::out);
  genXMLPrelude();
  genXMLGadgetsForVars();
  genXMLGadgetsForClauses();
  genXMLGadgetForFuncSigNode();
  genXMLPostlude();
  xf.close();
  df.close();
}

void c_GenSynthGraphs::genXMLPrelude()
{
  xf << "<?xml version=\"1.0\"?>" << endl;
  xf << "<!DOCTYPE pathway SYSTEM \"http://www.kegg.jp/kegg/xml/KGML_v0.7.1_.dtd\">" << endl;
  xf << "<pathway name=\"KEGG\">" << endl << endl;

  // Using our encoding, we need 6n + 4m + 2 nodes in the graph for a
  // 3-CNF formula with n variables and m clauses.

  // Convention: node 1 is source, node 2 is target, nodes 2+6*(i-1)+1 to
  // 2+6*i are used in the gadget for variable i, and nodes 2+6*n+4*(j-1)+1 to
  // 2+6*n+4*j are used in the gadget for clause j
  // Add an extra node -- the functionally significant node
  
  for (int i = 1; i <= total_nodes; i++) {
    addXMLNode(i);
  }

  // Generating DIMACS prelude
  df << "c" << endl;
  df << "c Unsat formula with " << nvars << " vars and sum of relax bounds " << relax_bound << endl;
  df << "c" << endl;
  df << "p cnf " << total_vars << " " << total_clauses << endl;
}

void c_GenSynthGraphs::addXMLNode(int i)
{
  string nodeName = NodeName(i);
  xf << "\t<entry id=\"" << i << "\" name=\"" + nodeName + "\" type=\"gene\">" << endl;
  xf << "\t  <graphics name=\"" + nodeName + "\"/>" << endl;
  xf << "\t</entry>" << endl;
}

void c_GenSynthGraphs::genXMLGadgetsForVars()
{
  for(int i = 1; i <= total_vars; i++) {
    // Using notation in Fig 2 of CP2019 paper
    int a = VARGADGET_A(i);
    int na = VARGADGET_NA(i);
    int b = VARGADGET_B(i);
    int nb = VARGADGET_NB(i);
    
    addXMLEdge(SRC, a, ACTIVATION);
    addXMLEdge(SRC, na, ACTIVATION);
    addXMLEdge(a, b, ACTIVATION);
    addXMLEdge(a, nb, INHIBITION);
    addXMLEdge(na, b, INHIBITION);
    addXMLEdge(na, nb, ACTIVATION);

    if (unsat_option == 0) {
      int d = VARGADGET_D(i);
      int nd = VARGADGET_ND(i);

      addXMLEdge(b, d, ACTIVATION);
      addXMLEdge(b, nd, INHIBITION);
      addXMLEdge(nb, d, ACTIVATION);
      addXMLEdge(nb, nd, INHIBITION);
      addXMLEdge(d, TGT, ACTIVATION);
      addXMLEdge(nd, TGT, ACTIVATION);
    }
  }
}

void c_GenSynthGraphs::addXMLEdge(int edge_src, int edge_tgt, int edge_type)
{
  xf << "\t<relation entry1=\"" << edge_src << "\" entry2=\"" << edge_tgt << "\" type=\"PPrel\" pathways=\"KEGG\">" << endl;
  xf << "\t <subtype name=\"";
  if (edge_type == ACTIVATION) {
    xf << "activation";
  }
  else if (edge_type == INHIBITION) {
    xf << "inhibition";
  }
  else {
    cerr << "Trying to print non activation/inhibition edge" << endl;
    xf << "ERROR!!! ERROR!!! ERROR!!!";
  }
  xf << "\"/>" << endl;
  xf << "\t</relation>" << endl;
}

void c_GenSynthGraphs::genXMLGadgetsForClauses()
{
  // First generate the basic 8 clauses on the first three variables
  list<c_threeCNFClause> queue;

  c_threeCNFClause cls;
  for (int v1 = 0; v1 <=1; v1++) {
    cls.l1 = (v1 == 0) ? -1:1;
    for (int v2 = 0; v2 <=1; v2++) {
      cls.l2 = (v2 == 0) ? -2:2;
      for (int v3 = 0; v3 <=1; v3++) {
	cls.l3 = (v3 == 0) ? -3:3;
	queue.push_back(cls);
      }
    }
  }

  // Now iterate over the list and augment clauses and variables

  int cls_count = 8;
  for (int v = 4; v <= nvars; v++) {
    cls = queue.front();
    queue.pop_front();
    c_threeCNFClause repl_cls1, repl_cls2;
    // Replace (l1 OR l2 OR l3) with (l2 OR l3 OR l4) AND (~l4 OR l2 OR l1)
    repl_cls1.l1 = cls.l2;
    repl_cls1.l2 = cls.l3;
    repl_cls1.l3 = v;
    repl_cls2.l1 = -v;
    repl_cls2.l2 = cls.l2;
    repl_cls2.l3 = cls.l1;
    queue.push_back(repl_cls1);
    queue.push_back(repl_cls2);
    cls_count++; // Removed one, added two clauses
  }

  // One copy of the clauses are ready now.  The clause gadgets have
  // to be added to the XML file for (relax_bound + 1) copies

  assert(cls_count == nclauses);

  int total_cls_gadget_count = 0;

  for (int r = 0; r <= relax_bound; r++) {
    // r-th copy of the graph -- total (r+1) copies
    int curr_cls_gadget_count = 0;
    while (curr_cls_gadget_count < nclauses) {
      cls = queue.front();
      queue.pop_front();
      queue.push_back(cls);
      
      int src_l1c = (cls.l1 > 0) ? VARGADGET_B(r*nvars + cls.l1):VARGADGET_NB(r*nvars - cls.l1);
      int src_l2c = (cls.l2 > 0) ? VARGADGET_B(r*nvars + cls.l2):VARGADGET_NB(r*nvars - cls.l2);
      int src_l3c = (cls.l3 > 0) ? VARGADGET_B(r*nvars + cls.l3):VARGADGET_NB(r*nvars - cls.l3);

      df << ((cls.l1 > 0) ? r*nvars + cls.l1 : -(r*nvars - cls.l1)) << " ";
      df << ((cls.l2 > 0) ? r*nvars + cls.l2 : -(r*nvars - cls.l2)) << " ";
      df << ((cls.l3 > 0) ? r*nvars + cls.l3 : -(r*nvars - cls.l3)) << " 0" << endl;
      
      addXMLEdge(src_l1c, CLSGADGET_L1C(total_cls_gadget_count+1, total_vars), ACTIVATION);
      addXMLEdge(src_l2c, CLSGADGET_L2C(total_cls_gadget_count+1, total_vars), ACTIVATION);
      addXMLEdge(src_l3c, CLSGADGET_L3C(total_cls_gadget_count+1, total_vars), ACTIVATION);
      addXMLEdge(CLSGADGET_L1C(total_cls_gadget_count+1, total_vars), CLSGADGET_LCC(total_cls_gadget_count+1, total_vars), ACTIVATION);
      addXMLEdge(CLSGADGET_L2C(total_cls_gadget_count+1, total_vars), CLSGADGET_LCC(total_cls_gadget_count+1, total_vars), ACTIVATION);
      addXMLEdge(CLSGADGET_L3C(total_cls_gadget_count+1, total_vars), CLSGADGET_LCC(total_cls_gadget_count+1, total_vars), ACTIVATION);
      addXMLEdge(CLSGADGET_LCC(total_cls_gadget_count+1, total_vars), TGT, ACTIVATION);
      curr_cls_gadget_count++;
      total_cls_gadget_count++;
    }
  }
    
  assert(total_cls_gadget_count == total_clauses);
}

void c_GenSynthGraphs::genXMLGadgetForFuncSigNode()
{
  addXMLEdge(SRC, fsig_node, ACTIVATION);

  for (int r = 0; r <= relax_bound; r++) {
    for (int i=1; i <= nclauses; i++) {
      addXMLEdge(fsig_node, CLSGADGET_LCC(r*nclauses + i, total_vars), ACTIVATION);
    }

    if (unsat_option == 0) {
      // Having to add these extra edges to make the synthetic benchmarks work
      // with the quirk that is implemented in network_tool for counting relaxed
      // edges:  an edge (u, v) is considered relaxed *regardless of* whether (u, v)
      // is chosen to be present in the explanation subgraph if the labels of u
      // and v are not compatible with that of the edge (u, v).  The notion of
      // a relaxed edge in the paper requires (u, v) to also be present in the
      // explanation subgraph.
      // The following for loop is not needed if we make the notion of relaxed
      // edges dependent on whether the edge is chosen to be present in the
      // explanation subgraph.
      
      for (int i=1; i <= nvars; i++) {
	addXMLEdge(fsig_node, VARGADGET_D(r*nvars + i), ACTIVATION);
	addXMLEdge(fsig_node, VARGADGET_ND(r*nvars + i), ACTIVATION);
      }
    }
  }
}

void c_GenSynthGraphs::genXMLPostlude()
{
  xf << "</pathway>" << endl;
}

void c_GenSynthGraphs::genLogFoldChangeFile()
{
  lf.open(subdir + "/" + foldchgFileName(nvars, relax_bound, unsat_option), ios::out);

  float upreg_val = UPREG_VAL;
  
  lf << NodeName(SRC) << "\t" << upreg_val << endl;
  lf << NodeName(TGT) << "\t" << upreg_val << endl;

  for (int r = 0; r <= relax_bound; r++) {
    if (unsat_option == 0) {
      for (int i = 1; i <= nvars; i++) {
	lf << NodeName(VARGADGET_D(r*nvars + i)) << "\t" << upreg_val << endl;
	lf << NodeName(VARGADGET_ND(r*nvars + i)) << "\t" << upreg_val << endl;
      }
    }

    for (int i = 1; i <= nclauses; i++) {
      lf << NodeName(CLSGADGET_LCC(r*nclauses + i, total_vars)) << "\t" << upreg_val << endl;
    }
  }

  lf.close();
}

void c_GenSynthGraphs::genInputScriptFile()
{
  sf.open(subdir + "/" + inpscriptFileName(nvars, relax_bound, unsat_option), ios::out);
  
  sf << ";; Global parameters" << endl << endl;
  sf << ";; Change only if you are sure of what you are doing." << endl;
  sf << ";; Split a node if it has more than these many ids" << endl << endl;
  sf << "let $NODE_SPLIT_THRESHOLD 1" << endl << endl;
  sf << ";; ******************************" << endl;
  sf << ";; DO NOT CHANGE THE NEXT 3 LINES" << endl;
  sf << ";; ******************************" << endl << endl;
  sf << "let $ONLY_ACT_EDGES_TO_TARGET 1" << endl;
  sf << "let $ONLY_EXPR_EDGES_TO_TARGET 2" << endl;
  sf << "let $BOTH_ACT_EXPR_EDGES_TO_TARGET 0" << endl << endl;
  sf << ";; Timeouts in millisecs" << endl;
  sf << ";; timeout1: for incremental solver" << endl;
  sf << ";; timeout2: overall timeout" << endl << endl;
  sf << "let $CONSTR_SOLVER_TIMEOUT1 30000" << endl;
  sf << "let $CONSTR_SOLVER_TIMEOUT2 150000" << endl << endl;
  sf << ";; Coefficients and exponents for edge weights in min-cut based analysis" << endl;
  sf << ";; Currently not used." << endl << endl;
  sf << "let $COEFF1 1.0" << endl;
  sf << "let $EXP1 -1.0" << endl;
  sf << "let $COEFF2 1.0" << endl;
  sf << "let $EXP2 -1.0" << endl;
  sf << "let $COEFF3 1.0" << endl;
  sf << "let $EXP3 -1.0" << endl;
  sf << "let $COEFF4 1.0" << endl;
  sf << "let $EXP4 -1.0" << endl << endl;
  sf << ";;" << endl;
  sf << ";; These files need to be updated for a new pathway database" << endl;
  sf << ";;" << endl;
  sf << "let $COMMON_FILES_DIR /home/intern/experiments/counting/common_files" << endl;
  sf << "let $INTER_DB_MAP_FILE $COMMON_FILES_DIR/inter_db_mapping_file" << endl;
  sf << "let $HSA_TO_GENE_SYMBOL_MAP_FILE $COMMON_FILES_DIR/hsa_to_gene_symbol_map_6334.txt" << endl;
  sf << "let $HSA_PATH_TO_PATH_NAME_MAP_FILE $COMMON_FILES_DIR/hsa_path_to_path_name_map_2822.txt" << endl;
  sf << "let $LIST_OF_MERGED_XML_FILE $COMMON_FILES_DIR/list_of_kegg_xml_files.txt" << endl << endl;
  sf << ";;" << endl;
  sf << ";; Start of part that potentially needs to be changed for each experiment" << endl;
  sf << ";;" << endl;
  sf << "let $WORK_DIR " << subdir << endl;
  sf << "let $MERGED_PATHWAYS_XML_FILE  $WORK_DIR/" << graphXMLFileName(nvars, relax_bound, unsat_option) << endl;
  sf << "let $LOG_FOLD_CHANGE_FILE $WORK_DIR/" << foldchgFileName(nvars, relax_bound, unsat_option) << endl;
  sf << "let $EDGES_TO_TARGET $BOTH_ACT_EXPR_EDGES_TO_TARGET" << endl;
  sf << "let $SRC_NODE " << NodeName(SRC) << endl;
  sf << "let $TGT_NODE " << NodeName(TGT) << endl;
  sf << "let $NODE_TO_TEST_FOR_SIGNIFICANCE " << NodeName(fsig_node) << endl;
  sf << "let $REACH_PATH_BOUND 5" << endl;
  sf << "let $EDGE_RELAX_LB 0" << endl;
  sf << "let $EDGE_RELAX_UB " << to_string(edge_relax_bound) << endl;
  sf << "let $NODE_RELAX_LB 0" << endl;
  sf << "let $NODE_RELAX_UB " << to_string(node_relax_bound) << endl << endl;
  sf << "let $UP_REG_THRESH 1.0" << endl;
  sf << "let $DOWN_REG_THRESH -1.0" << endl;
  sf << ";;" << endl;
  sf << ";; End of part that potentially needs to be changed for each experiment" << endl;
  sf << ";;" << endl << endl;
  sf << ";; Various file names" << endl;
  sf << ";; Change only if you are sure of what you are doing." << endl << endl;
  sf << "let $EXCEPTION_TO_NODE_MERGE_FILE $WORK_DIR/exception_file.txt" << endl;
  sf << "let $UP_REG_FILE $WORK_DIR/up_reg" << endl;
  sf << "let $DOWN_REG_FILE $WORK_DIR/down_reg" << endl;
  sf << "let $FILENAME_PREFIX $WORK_DIR/synth_" << endl << endl;
  sf << ";; The following are empty files by default." << endl;
  sf << ";; Change them only if you are sure of what you are doing." << endl << endl;
  sf << "let $ESSENTIAL_NODES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $AVOID_NODES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $ESSENTIAL_EDGES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $AVOID_EDGES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $ACTIVE_NODES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $INACTIVE_NODES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $CONFIRMED_UP_REG_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $CONFIRMED_DOWN_REG_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $RELAXED_NODES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $NONRELAXED_NODES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $RELAXED_EDGES_FILE $WORK_DIR/empty_file" << endl;
  sf << "let $NONRELAXED_EDGES_FILE $WORK_DIR/empty_file" << endl << endl;

  sf << ";; create up-reg and down-reg files from fold change file and thresholds" << endl;
  sf << "cudf $LOG_FOLD_CHANGE_FILE $UP_REG_THRESH $DOWN_REG_THRESH $UP_REG_FILE $DOWN_REG_FILE" << endl;
  sf << ";;" << endl;
  sf << "exec cd $WORK_DIR; echo $SRC_NODE > src_file; echo $TGT_NODE > tgt_file" << endl;
  sf << "exec cd $WORK_DIR; cat $UP_REG_FILE $DOWN_REG_FILE src_file tgt_file > $EXCEPTION_TO_NODE_MERGE_FILE" << endl;
  sf << "exec cd $WORK_DIR; touch empty_file" << endl;
  sf << ";;" << endl;
  sf << ";; Read the following global files before starting execution of the pipeline" << endl;
  sf << ";;" << endl;
  sf << "start $INTER_DB_MAP_FILE $EXCEPTION_TO_NODE_MERGE_FILE $HSA_TO_GENE_SYMBOL_MAP_FILE $HSA_PATH_TO_PATH_NAME_MAP_FILE" << endl << endl;
  sf << ";;" << endl;
  sf << ";; Read file written in xml format" << endl;
  sf << "rgx $MERGED_PATHWAYS_XML_FILE y $NODE_SPLIT_THRESHOLD" << endl;
  sf << "size 0" << endl;
  sf << "let $MERGED_PATHWAYS_GRAPH_NUM #LASTGRAPHNUM" << endl;
  sf << "fb_rch 0 $SRC_NODE -1 $TGT_NODE -1 -1 $REACH_PATH_BOUND $EDGES_TO_TARGET" << endl;
  sf << "size 0" << endl;
  sf << "exec cd $WORK_DIR; echo $SRC_NODE $TGT_NODE > connect_pairs" << endl;
  sf << "pathz3 0 0 connect_pairs $EDGE_RELAX_LB $NODE_RELAX_LB $EDGE_RELAX_UB $NODE_RELAX_UB $REACH_PATH_BOUND $FILENAME_PREFIX $UP_REG_FILE $DOWN_REG_FILE $ESSENTIAL_NODES_FILE $AVOID_NODES_FILE $ESSENTIAL_EDGES_FILE $AVOID_EDGES_FILE $ACTIVE_NODES_FILE $INACTIVE_NODES_FILE $CONFIRMED_UP_REG_FILE $CONFIRMED_DOWN_REG_FILE $RELAXED_NODES_FILE $NONRELAXED_NODES_FILE $RELAXED_EDGES_FILE $NONRELAXED_EDGES_FILE $LOG_FOLD_CHANGE_FILE 1 1 $CONSTR_SOLVER_TIMEOUT1 $CONSTR_SOLVER_TIMEOUT2" << endl;

  sf << "let $PO_POINTS_WITH_GENE PO_with_$NODE_TO_TEST_FOR_SIGNIFICANCE" << endl;
  sf << "exec mv limits_timefile.txt $PO_POINTS_WITH_GENE_limits_timefile.txt" << endl;
  sf << "exec mv PO.dat $PO_POINTS_WITH_GENE.dat" << endl;
  sf << ";; exec gnuplot -p -e \"set xlabel 'Node relaxn'; set ylabel 'Edge relaxn'; set xrange [-0.5:$NODE_RELAX_UB+0.5]; set yrange [-0.5:$EDGE_RELAX_UB+0.5]; plot '$PO_POINTS_WITH_GENE.dat' with lp linecolor 'blue';exit\"" << endl << endl;
  sf << ";; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << endl;
  sf << ";; Starting analysis with " << NodeName(fsig_node) << " excluded" << endl;
  sf << ";;" << endl;
  
  sf << "fb_rch $MERGED_PATHWAYS_GRAPH_NUM $SRC_NODE -1 $TGT_NODE -1 $NODE_TO_TEST_FOR_SIGNIFICANCE -1 $REACH_PATH_BOUND $EDGES_TO_TARGET" << endl;
  sf << "size 0" << endl;

  sf << ";; wg 0 $SRC_NODE -1 $TGT_NODE -1 $LOG_FOLD_CHANGE_FILE $COEFF1 $EXP1 $COEFF2 $EXP2 $COEFF3 $EXP3 $COEFF4 $EXP4" << endl;
  
  sf << "pathz3 0 0 connect_pairs $EDGE_RELAX_LB $NODE_RELAX_LB $EDGE_RELAX_UB $NODE_RELAX_UB $REACH_PATH_BOUND $FILENAME_PREFIX $UP_REG_FILE $DOWN_REG_FILE $ESSENTIAL_NODES_FILE $AVOID_NODES_FILE $ESSENTIAL_EDGES_FILE $AVOID_EDGES_FILE $ACTIVE_NODES_FILE $INACTIVE_NODES_FILE $CONFIRMED_UP_REG_FILE $CONFIRMED_DOWN_REG_FILE $RELAXED_NODES_FILE $NONRELAXED_NODES_FILE $RELAXED_EDGES_FILE $NONRELAXED_EDGES_FILE $LOG_FOLD_CHANGE_FILE 1 1 $CONSTR_SOLVER_TIMEOUT1 $CONSTR_SOLVER_TIMEOUT2 $PO_POINTS_WITH_GENE" << endl;

  sf << "let $PO_POINTS_WITHOUT_GENE PO_without_$NODE_TO_TEST_FOR_SIGNIFICANCE" << endl;
  sf << "exec mv limits_timefile.txt $PO_POINTS_WITHOUT_GENE_limits_timefile.txt" << endl;
  sf << "exec mv PO.dat $PO_POINTS_WITHOUT_GENE.dat" << endl;
  sf << "exec gnuplot -p -e \"set xlabel 'Node relaxn'; set ylabel 'Edge relaxn'; set xrange [-0.5:$NODE_RELAX_UB+0.5]; set yrange [-0.5:$EDGE_RELAX_UB+0.5]; plot '$PO_POINTS_WITH_GENE.dat' with lp linecolor 'blue'; replot '$PO_POINTS_WITHOUT_GENE.dat' with lp linecolor 'red'; exit\"" << endl << endl;
  sf << "exit" << endl;

  sf.close();
}

