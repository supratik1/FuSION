#ifndef DAG_MANAGER_H
#define DAG_MANAGER_H

#ifdef QE                              // #ifdef added by Ajith John on 9 June 2013
#define THRESHOLD_SIZE_DEATH_ROW 10000 // default maximum size of death row
#endif

#include "HashTable_WithStandardKeys.h"
#include "LogManager.h"
#include <vector>
#include <string>
#include <fstream>
#include <list>
#include <queue>
#include <map>
#include <set>
using namespace std;

/*---Forward declaration of FRIEND DAGManager ---*/
class t_DAGManager;
class GarbageCollector;

class t_DAGNode
{

  /*---------------Friends------------------*/
  /**
   * Note here that the class DAGManager has been made friend to DAGNode. This allows the class DAGManager to access private members of DAGNode.
   * Thus we get rid of a number of getty and setty methods for the DAGNode class, since it is highely unlikely that anybody else other than the DAGManager will ever need to have access to thsese fields.
   *
   **/
  friend class t_DAGManager;
  friend class GarbageCollector;
  /**
   * This is a friend function, friend from both DAGNode and DAGManager classes.
   * The purpose of this function is to convert a pointer to a hex "string" object.
   */
  friend string addressToString(void *ptr);

  /*-------------Private Fields.
    Note that these are accessible to friends in the above section------------*/

private:
  string m_label;               // the label, in restricted mode, the label may not be unique
  int m_reference_count;        // Reference Count of the nodes
  void *m_value;                // Allow the user to store a arbitrary value
                                // If the user is not storing any value, then the DAGManager stores a null in it..
  string m_signature;           // internal signature, to be used in restricted mode in DAGManager
  list<t_DAGNode *> m_in_list;  // list of incident edges
  list<t_DAGNode *> m_out_list; // list of outgoing edges
  bool m_visited;               // is this node visited?
  bool m_isShared;              // is this node shared?
  bool m_isRoot;                // is this node a root node?
  bool m_isLeaf;                // is this node a leaf node?
  int m_temp_id;                // identifier assigned to a shared node for naming..
  int m_copy_count;             // Count how many times a DAGNode was copied
  int m_node_id;
  /**
   *  Modification on 9-04-2011
   */
  t_DAGNode *m_replaceBy; // If current DAGNode is replaced by another DAGNode as a result of ReplaceDAGByDAG
                          // this field stores the node which replaces the current DAGNode
                          // this should be non null only when current DAGNode is rendered garbage by ReplaceDAGByDAG and current DAGNode
                          // is removed from HashTable

  //    next_root and prev_root are used for maintaining a list of root nodes of DAG
  // For Non-Root Nodes, these values must be NULL
  t_DAGNode *m_next_root;
  t_DAGNode *m_prev_root;

  set<string> m_abstract_signatures;
  int m_data_type;
  //    next_node is used for maintaining a list of all nodes of DAG

  // The last node created by DAGManager will have this pointer NULL
  t_DAGNode *m_next_node;
  t_DAGNode *m_prev_node;

#ifdef QE // #ifdef added by Ajith John on 9 June 2013
  // next_dead and prev_dead are used for maintaining the death row
  // For alive nodes, these values must be NULL
  t_DAGNode *m_next_dead;
  t_DAGNode *m_prev_dead;
#endif

public:
  ~t_DAGNode();
  string getLabel();
  bool addAbstractSignature(string label_to_add);
  set<string> getAbstractSignatures();
  bool isPresentAsAbstractSignature(string label_to_test);
  int getTypeOfDAGNode();
  // this method should return expression, constant or boolean
  //  a symbol is expression
  //  this method requires some expression level information from DAG
  // therefore it should be left unimplemented
  // for us, this method is going to be expression manager specific

public:
  // Constructors for the DAGNode. These differ on the number parameters only
  t_DAGNode(void *value_to_set = NULL);

private:
  t_DAGNode(const string &label_to_assign, void *value_to_set = NULL);
  t_DAGNode(const string &label_to_assign, const vector<t_DAGNode *> &v_operands, void *value_to_set = NULL);

public:
  // Allow the user to traverse the out_list of the DAGNode
  int getSizeOfOutList();
  void printExpressionToFile(ostream &out);

  // Get iterator to the begining of outlist
  list<t_DAGNode *>::iterator getOutListBeginIterator();
  // Get iterator to the end of outlist
  list<t_DAGNode *>::iterator getOutListEndIterator();

  // Read the value stored in the DAGNode
  void *getValue();

  // Write the value in the DAGNode
  bool setValue(void *value_to_set);

  // set the visited flag -- only for the external traversals

  bool assignVisitedFlag(bool assign_to);
  bool assignVisitedFlagRecursively(bool value, t_HashTable<unsigned long long, bool> &);
  bool isNodeVisited();
  bool isNodeShared();
  bool isCopied();
  void assignTempID(int id);
  int getTempID();

  int getNodeID();
  bool setDataType(int type_to_set);
  int getDataType();
  // Get iterator to the begining of inlist
  list<t_DAGNode *>::iterator getInListBeginIterator();
  // Get iterator to the end of inlist
  list<t_DAGNode *>::iterator getInListEndIterator();
  int getInListSize();
  // bool clearAbstractSignatures()
  void clearAbstractSignatures()
  {
    this->m_abstract_signatures.clear(); // pakka void.-> used in 3 places always no return required.
  }

  bool isGarbageNode()
  {
    return (this->m_reference_count == 0 && !(this->m_isRoot)) ? true : false;
  }
};

class t_DAGManager
{

  /*
    DAGManager takes care for all the private fields of the DAGNodes.
    Setting right value of reference count is responsibility of DAGManager.
    DAGNodes are always created using pointers, except some header nodes.
    Make sure that correct reference count is set in *DAGManager*
  */
private:
  static t_DAGManager *singleton;
  t_Logger *m_logManager;
  ofstream *m_dagmanager_log;
  t_HashTable<string, t_DAGNode *> ht;
  bool m_restricted_mode;
  bool m_simplifyDAGOnCreation;
  bool m_recursiveReplaceFlag;
  static int m_dagmanager_instance_number;
  bool m_enable_logging;
  int m_copy_prefix_counter;
  /*
    Statically generated head node and tail pointer for maintaining list of root nodes in the DAG
  */

  t_DAGNode m_head_roots; // This is a special node. This is never deleted. It does not appear on any list. No reference count for it.
  t_DAGNode *m_tail_roots;

  /*
    Maintain a list of all the nodes that were ever generated. This list is useful for traversing the DAG in O(n)
  */
  t_DAGNode m_head_nodes; // This is a special node. This is never deleted. It does not appear on any list. No reference count for it.
  t_DAGNode *m_tail_nodes;

  /**
   * dagnode_count is the count of all DAGNodes that were ever generated
   * This function simply returns this count to caller
   * This count may also include garbage nodes
   *
   * Minimum dagnode_count is 2, corresponding to the two header nodes of linked lists
   * of root nodes and dag nodes
   */
  int m_dagnode_count;
  int m_node_id_counter;
  // just count how many bytes were used!!
  unsigned long int m_memory_usage;

#ifdef QE // #ifdef added by Ajith John on 9 June 2013
  /* Statically generated head node and tail pointer
      for maintaining the death row */
  t_DAGNode m_head_dead; // This is a special node. This is never deleted. It does not appear on any list. No reference count for it.
  t_DAGNode *m_tail_dead;
  unsigned long long int m_death_row_size;           // Stores the size of death row
  unsigned long long int m_threshold_death_row_size; // Stores the maximum size of death row, beyond which deletion will be done
#endif

  // Friends
  friend string addressToString(void *ptr);

  /**
   * The public API of the DAG Manager. Most of the functions in this
   * API have a recursive helper, which is private.  The functions are
   * listed in this file in a order such that the set of associated
   * functions are grouped together.  See the documentation for more
   * details.
   *
   */

  /*Section 1 : Constructor and destructor
    The restricted mode flag stands for

    If the flag is set, the DAGManager uses internal mechanisms for
    generating signatures and takes care for sharing of similar node using
    it..
    If not set, the lable of every node should be unique. This is
    responsibility of the client. The DAGManager does not implement the
    mechanisms of signauture generation, etc. and it uses the lable of the
    DAGNode itself as the signature*/

public:
#ifdef QE // #ifdef added by Ajith John on 9 June 2013
  t_DAGManager(bool enableSimplifierOnCreation = false, bool restricted_mode_flag = true, bool enable_logging_flag = true, bool recursion_recompute_signatures = true, unsigned long long int threshold_death_row_size = THRESHOLD_SIZE_DEATH_ROW);
// the additional argument threshold_death_row_size sets the maximum possible size of death row.
#else
  t_DAGManager(bool enableSimplifierOnCreation = false, bool restricted_mode_flag = true, bool enable_logging_flag = true, bool recursion_recompute_signatures = true);
#endif

  ~t_DAGManager();
  static void setSingleton(t_DAGManager *dagm) { singleton = dagm; }
  static t_DAGManager *getSingleton() { return singleton; }

private:
  void LOG(string message);

  /* Section 2: Create The DAG*/
public:
  // create DAGNode
  t_DAGNode *createNode(const string &label, void *value = NULL);
  t_DAGNode *createNode(const string &label, const vector<t_DAGNode *> &v_children, void *value = NULL,
                        bool remember_simplified_dag_after_rewrite_and_return_unsimplified_dag_before_rewrite = false,
                        bool recall_simplified_dag_from_last_createNode_call = false);

  // add and remove edges between nodes
  bool addEdge(t_DAGNode *parent, t_DAGNode *child);
  int removeFirstEdge(t_DAGNode *parent, t_DAGNode *child);

  // add a child at Ith position
  bool addNodeAsIthChild(t_DAGNode *parent, t_DAGNode *child, int i);

  // remove Ith outgoing edge of a node
  bool removeIthOutgoingEdgeOfANode(t_DAGNode *node, int i);

  // create the DAG by reading a file..
  bool buildDAGFromFile(ifstream *infile);

  int removeEdgeWithoutChangingSignatures(t_DAGNode *parent, t_DAGNode *child);

private:
  // create signature of a node, restricted mode only
  template <typename Alloc>
  inline string createSignature(const string &label, const vector<t_DAGNode *, Alloc> &v_operands);
  inline string createSignature(const string &label);

  // recompute signature of a node when the signature of the node changes..

  bool RecomputeSignatureOfANodeInternal(t_DAGNode *node, bool enable_sanity_check, int rec_cntr_recomp_fn, int rec_cntr_replace_function);

public:
  bool RecomputeSignatureOfANode(t_DAGNode *node);

private:
  // delete a non-root node from the list of root nodes, also called by deleteOperandsOfANodeFromRootsList
  bool deleteNodeFromRootsList(t_DAGNode *node_to_delete);
  bool deleteNodeFromNodesList(t_DAGNode *node_to_delete);
  // a new root node has been created, add it to the list of roots
  inline bool insertNodeInRootsList(t_DAGNode *node);

  // every node that is created is to be inserted in this list
  inline bool insertNodeInNodesList(t_DAGNode *node);

  // count the number of bytes used!!
  inline void countBytesUsedByANodeInMemoryUsage(t_DAGNode *node);

  // Recompute the signatures of all parent nodes recursively
  bool RecomputeSignaturesRecursively(t_DAGNode *node);

  /* Section 4: Copy the DAG*/
public:
  // copy DAG without values
  t_DAGNode *copyDAG(t_DAGNode *node);
  // copy DAG with values
  t_DAGNode *copyDAGWithValues(t_DAGNode *node, void *(*copyfunc)(void *value_to_copy));

  // this function is different from the above one. Here you can operate on two copied DAGs together.
  // Explicit suffix is taken from the user
  t_DAGNode *copyDAGWithValuesAndUserDefinedSuffixOnLeafNodes(t_DAGNode *node, void *(*copyfunc)(void *value_to_copy), string suffix);

private:
  // Recursively copy the DAG without values
  t_DAGNode *copy(t_DAGNode *node, string copy_prefix);

  // Recursively copy the DAG with values
  t_DAGNode *copyWithValues(t_DAGNode *node, string copy_prefix, void *(*copyfunc)(void *value_to_copy));

  t_DAGNode *copyWithValuesAndUserDefinedSuffixOnLeafNodes(
      t_DAGNode *node,    //(sub)DAG to copy
      string copy_prefix, // copy prefix
      void *(*copyfunc)(void *value_to_copy),
      string suffix);

  /*Section 5 : Query the DAG */
public:
  set<t_DAGNode *> getSetOfDescendentNodesByDFS(t_DAGNode *node);
  set<t_DAGNode *> getSetOfDescendentNodesByBFS(t_DAGNode *node);
  set<t_DAGNode *> getSetOfAncestorNodesByDFS(t_DAGNode *node);
  set<t_DAGNode *> getSetOfAncestorNodesByBFS(t_DAGNode *node);

  bool checkWhetherCreatingEdgeFormsCycle(t_DAGNode *nodeFrom, t_DAGNode *nodeTo);

  // get the list of all leaf nodes starting from a root
  vector<t_DAGNode *> getVectorOfLeafNodes(t_DAGNode *node);
  vector<t_DAGNode *> getVectorOfAllLeafNodesInDAGManager();
  // Get the list of all root nodes maintained by the DAGManager
  vector<t_DAGNode *> getVectorOfRootNodes();

  // Get i th child of a node
  // Same effect can be achieved by traversing the outlist of the node using iterators obtained by getOutListBeginIterator() and getOutListEndIterator();
  t_DAGNode *getIThChildOfNode(t_DAGNode *node, int i);

  // just obtain a handle to a leaf node with given label, useful when you know that the node was already created and you don't want to create it again
  t_DAGNode *getNodeWithLabel(const string &label);
  template <typename Alloc>
  t_DAGNode *testAndReturnDAGNodeIfItExists(const string &label, const vector<t_DAGNode *, Alloc> &v_children);
  t_DAGNode *testAndReturnDAGNodeIfItExists(const string &label);
  vector<t_DAGNode *> getNodesAtIthLevel(t_DAGNode *root, int i);
  int getHeightOfDAGWithMinimumPaths(t_DAGNode *root);

private:
  vector<t_DAGNode *> getChildrenInDFSRecursively(t_DAGNode *node);
  vector<t_DAGNode *> getParentsInDFSRecursively(t_DAGNode *node);
  // Recursively collect the leaf nodes of the DAG
  vector<t_DAGNode *> collectLeavesRecursively(t_DAGNode *node);

  /*Section 6: Some more Query operations. */
public:
  // Test if the DAG is a Tree (has 0 shared nodes)
  bool isDAGATree(t_DAGNode *root);

  // Test if node_to_test is parent of parent_of
  bool isParent(t_DAGNode *node_to_test, t_DAGNode *parent_of);

  // Test if node_to_test is child of child_of
  bool isChild(t_DAGNode *node_to_test, t_DAGNode *child_of);

  // Test if node_to_test is ancestor of ancestor_of
  bool isAncestor(t_DAGNode *node_to_test, t_DAGNode *ancestor_of);

  // Test if node_to_test is descendent of descendent_of
  bool isDescendent(t_DAGNode *node_to_test, t_DAGNode *descendent_of);

  /*Section 7A: DAG Manipulation*/
public:
  bool ResetVisitedFlagOfDAGNodes();
  // mark the shared nodes in DAG (although the DAG maintains the shared flag, redo it!!)
  int markSharedNodes(t_DAGNode *node);

private:
  bool ResetVisitedFlagOfDAG(t_DAGNode *node);
  bool ResetVisitedFlagOfDAGRecursively(t_DAGNode *node);

  // traverse the list of all DAG nodes and clear the visited flag of entire DAG. Do this when you are not sure about the operation of ResetVisitedFlagOfDAG
  bool ResetVisitedFlagOfAllDAGNodes();

  // delete entry of a node from hashtable. does not delete the node itself, just deletes its entry from hashtable
  bool deleteANodeFromHashTable(t_DAGNode *node);

  // mark the shared nodes in DAG (although the DAG maintains the shared flag, redo it!!)
  //   int markSharedNodes(DAGNode *node);
  bool markSharedNodesRecursively(t_DAGNode *node, int &counter);

  // when a node becomes root, its children are no more root, so mark them as non-root
  bool deleteOperandsOfANodeFromRootsList(t_DAGNode *node);

  // we cannot delete a node from the DAG directly. Need some steps to do that depending upon the values of flags in that node..
  bool deleteARootNodeFromDAG(t_DAGNode *node);

public:
  // delete the incident edges of the node = Clear the in-list of this node and delete corresponding entry from the out-list of its parents
  bool deleteIncidentEdges(t_DAGNode *node);

  bool deleteANodeFromInListOfItsChildren(t_DAGNode *node);

  bool deleteDAG(t_DAGNode *node);

private:
  bool ReplaceDAGByDAGInternal(t_DAGNode *toReplace, t_DAGNode *replaceBy, bool enable_sanity_check, int rec_cntr_recomp_fn, int rec_cntr_replace_function);
  /*Section 7B: Advanced DAG Manipulation*/
public:
  /**
   * Modification on 9-04-2011
   */
  inline t_DAGNode *getReplaceByInfo(t_DAGNode *node)
  {
    return node->m_replaceBy;
  }
  bool ReplaceLeafByRootOfADAG(t_DAGNode *toReplace, t_DAGNode *replaceBy);
  bool ReplaceChild(t_DAGNode *parent, t_DAGNode *child_to_replace, t_DAGNode *child_replace_by);
  bool MergeEquivalentNodes(t_DAGNode *toReplace, t_DAGNode *replaceBy);

  bool ReplaceLeafByDAG(t_DAGNode *toReplace, t_DAGNode *replaceBy, bool checkForAcyclicity);

  bool ReplaceDAGByDAG(t_DAGNode *toReplace, t_DAGNode *replaceBy);

  /*Section 8: This is the garbage collector. Call it on the DAGManager object*/
private:
  bool initializeGarbageCollector(set<t_DAGNode *> &global_set_of_garbage_nodes);
  bool initializeGlobalSetOfGarbageNodes(set<t_DAGNode *> &global_set_of_garbage_nodes);
  bool initializeCurrentSetOfGarbageNodes(set<t_DAGNode *> &current_set_of_garbage_nodes, set<t_DAGNode *> &global_set_of_garbage_nodes);
  bool resetGCDataStructures(set<t_DAGNode *> &global_set_of_garbage_nodes);
  bool computeDifferenceOfGlobalSetOfGarbageNodesAndNonGarbageNodesAndDeleteInLists(set<t_DAGNode *> &global_set_of_garbage_nodes, set<t_DAGNode *> global_set_of_nodes_with_edges_to_garbage);
  bool removeEdgesFromNonGarbageNodesThatPointToGarbage(t_DAGNode *node, set<t_DAGNode *> &global_set_of_garbage_nodes);
  bool checkIfDAGNodeIsGarbage(t_DAGNode *candidate_garbage_node, set<t_DAGNode *> &global_set_of_garbage_nodes);
  bool deleteGarbageNodesAndFreeMemory(set<t_DAGNode *> &global_set_of_garbage_nodes);

public:
  int collect_garbage();

  /*Section 9: Get Statistics from the DAGManager */

public:
  // Count the number of nodes in a DAG  viewed as Tree
  int getSizeOfDAGAsTree(t_DAGNode *node);

  // Count number of nodes in a DAG viewed as DAG
  int getSizeOfDAG(t_DAGNode *node);

  // Get the count of root nodes internally maintained by the DAGManager
  int getCountOfRootNodes();

  // get the number of collisions that happened in the hashtable
  int getCollisionsInHashTable();

  // Count the number of memory bytes used by DAGManager
  unsigned long int getMemoryUsage();

  // Count all DAGNodes that were ever generated in the DAGManager
  int getCountOfAllDAGNodesInDAGManager()
  {
    return m_dagnode_count;
  }

private:
  // Recursively count the nodes in the DAG viewed as DAG
  int getCountOfNodesInDAGRecursively(t_DAGNode *node);

  /*section 10: Print the DAG*/

  // Useful for printing the DAG

  /*--General printing routines.. Prints DAG to file*/
public:
  bool printDAG(t_DAGNode *node, ofstream *outfile);
  bool printDAGAsTreeToFile(t_DAGNode *node, ofstream *outfile);

private:
  bool printDAGRecursively(t_DAGNode *node, bool isRoot, ofstream *outfile);
  bool assignTempIDsAndBuildSharedNodesQ(t_DAGNode *node, int &temp_id_counter, vector<t_DAGNode *> &shared_nodes_queue);
  bool printSharedNodesFromQueue(ofstream *outfile, vector<t_DAGNode *> &shared_nodes_queue);

public:
  bool printDAGAsDOTFile(t_DAGNode *node, ofstream *outfile);
  bool printDAGAsDOTFileWithAbstractSignatures(t_DAGNode *node, ofstream *outfile);

private:
  bool printDAGAsDOTFileRecursively(t_DAGNode *node, ofstream *outfile);
  bool printDAGAsDOTFileRecursivelyWithAbstractSignatures(t_DAGNode *node, ofstream *outfile);

public:
  void printASetOfDAGNodes(set<t_DAGNode *> set_to_print);

#ifdef QE // #ifdef added by Ajith John on 9 June 2013
  /*section 11: New Garbage Collection functions added by Ajith John*/
private:
  inline bool insertToDeathRow(t_DAGNode *node);
  inline bool eraseFromDeathRow(t_DAGNode *node);
  inline bool findInDeathRow(t_DAGNode *node);
  bool decrementReferenceCountInternal(t_DAGNode *node);
  void reduceBytesUsedByANodeInMemoryUsage(t_DAGNode *node);
  bool deleteNodesInDeathRowAndClearDeathRow();
  bool deleteDeadNode(t_DAGNode *dead_node);
  bool deleteANodeFromRootsListAndInListOfItsChildren(t_DAGNode *node_to_delete);

public:
  bool decrementReferenceCount(t_DAGNode *node);
  bool incrementReferenceCount(t_DAGNode *node);
  bool getVectorOfDeadNodes(vector<t_DAGNode *> &dead_nodes);
#endif
};

#endif
