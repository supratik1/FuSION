#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <vector>

using namespace std;

class c_Edge {
public:
  int src_hsa, tgt_hsa;
  string edge_type, edge_subtype;

  c_Edge(int esrc, int etgt, string etype, string esubtype) {
    src_hsa = esrc;
    tgt_hsa = etgt;
    edge_type = etype;
    edge_subtype = esubtype;
  }

  ~c_Edge() {;}
};

class c_SubGraph {
private:
  string inp_filename, out_filename;
  ifstream ifs;
  ofstream ofs;
  map<string, int> hsa_ids;
  vector<c_Edge> edges;
  int node_count;

public:
  c_SubGraph(char* in_fname, char* out_fname) {
    inp_filename = string(in_fname);
    out_filename = string(out_fname);
    node_count = 0;
  }

  ~c_SubGraph() {;}
  void readEdges();
  void writeEdges();
};
  
int main(int argc, char *argv[])
{
  if (argc != 3) {
    cerr << "Usage: " << string(argv[0]) << " input_file output_file" << endl;
  }
  else {
    c_SubGraph oG(argv[1], argv[2]);
    oG.readEdges();
    oG.writeEdges();
  }
}

void c_SubGraph::readEdges()
{
  ifs.open(inp_filename, ios::in);
  string mline;

  string src_hsa, tgt_hsa, edge_type, edge_subtype;
  
  map<string, int>::iterator it;
  while (std::getline(ifs, mline)) {
    std::istringstream nline(mline);
    nline >> src_hsa;
    nline >> tgt_hsa;
    nline >> edge_type;
    nline >> edge_subtype;

    if (edge_type != "PPRel") {
      cout << "Adding an edge of type " << edge_type << " (different from \"PPRel\")" << endl;
    }
    if ((edge_subtype != "activation") && (edge_subtype != "inhibition")) {
      cout << "Adding an edge of subtype " << edge_subtype << " (different from \"inhibition\" or \"activation\")" << endl;
    }
    
    it = hsa_ids.find(src_hsa);
    if (it == hsa_ids.end()) {
      hsa_ids[src_hsa] = node_count++;
    }

    it = hsa_ids.find(tgt_hsa);
    if (it == hsa_ids.end()) {
      hsa_ids[tgt_hsa] = node_count++;
    }

    c_Edge new_edge(hsa_ids[src_hsa], hsa_ids[tgt_hsa], edge_type, edge_subtype);
    edges.push_back(new_edge);
  }
  
  ifs.close();
}

void c_SubGraph::writeEdges()
{
  ofs.open(out_filename, ios::out);

  ofs << "<?xml version=\"1.0\"?>" << endl;
  ofs << "<!DOCTYPE pathway SYSTEM \"Additional edges added\">" << endl;
  ofs << "<pathway name=\"KEGG\">" << endl;

  map<string, int>::iterator it;
  for (it = hsa_ids.begin(); it != hsa_ids.end(); it++) {
    ofs << "\t<entry id=\"" << it->second << "\" name=\"" + it->first + "\" type=\"gene\">" << endl;
    ofs << "\t  <graphics name=\"" + it->first + "\"/>" << endl;
    ofs << "\t</entry>" << endl;
  }

  for (int i = 0; i < edges.size(); i++) {
    ofs << "\t<relation entry1=\"" << edges[i].src_hsa << "\" entry2=\"" << edges[i].tgt_hsa << "\" type=\"" + edges[i].edge_type + "\" pathways=\"KEGG\">" << endl;
    ofs << "\t <subtype name=\"" + edges[i].edge_subtype;
    ofs << "\"/>" << endl;
    ofs << "\t</relation>" << endl;
  }
  ofs << "</pathway>" << endl;

  ofs.close();
}
