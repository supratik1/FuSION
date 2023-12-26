#ifndef _GC_H_
#define _GC_H_
#include <set>
#include <algorithm>
using namespace std;

/*
class GarbageCollector
{
  friend class DAGManager;
 private: 
  set<DAGNode *> global_set_of_garbage_nodes; //"Global" set of garbage nodes 
  set<DAGNode *> global_set_of_non_garbage_nodes; //"Global" set of non-garbage nodes


  ofstream *gc_log;
  bool enable_logging;

  DAGManager *dag_manager;
  GarbageCollector(ofstream *log_file, int enable_logging, DAGManager *dag_manager_to_use);
  ~GarbageCollector();
 private:

  bool initializeGarbageCollector();
  bool initializeGlobalSetOfGarbageNodes();
  bool initializeCurrentSetOfGarbageNodes(set<DAGNode *> &current_set_of_garbage_nodes);
  bool resetGCDataStructures();
  bool deleteGarbageNodesFromGlobalSetOfGarbageNodes();
  bool computeDifferenceOfGlobalSetOfGarbageNodesAndNonGarbageNodes();
  bool removeIncidentEdgesOfNonGarbageNodesThatPointToGarbage();

  void LOG(string message);
 public:
  int collect_garbage();
    

};
*/
#endif
