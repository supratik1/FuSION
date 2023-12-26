#include "DagManager.h"
#include "helping_functions.h"
#include "DAGSimplifier.h"
#include <map>
#include<iostream>
using namespace std;





/**
 * Synopsis: Constructor for the DAGManager
 *
 * Description: Construct the DAGManager instance. Opens the log files
 * and sets up various lists, counters, etc. The log of every instance
 * of DAGManager is written to a new file. This is useful for the
 * purpose of debugging to the client of the DAGManager.
 *
 *
 **/
extern t_DAGSimplifier *ds;
int t_DAGManager::m_dagmanager_instance_number = 0;
t_DAGManager* t_DAGManager::singleton = NULL;

#ifdef QE //#ifdef added by Ajith John on 9 June 2013
	t_DAGManager::t_DAGManager(bool enableSimplifierOnCreation, bool restricted_mode_flag, bool enable_logging_flag,bool recursion_recompute_signatures, unsigned long long int threshold_death_row_size)
	// the additional argument threshold_death_row_size sets the maximum possible size of death row.
#else
	t_DAGManager::t_DAGManager(bool enableSimplifierOnCreation, bool restricted_mode_flag, bool enable_logging_flag,bool recursion_recompute_signatures)
#endif
{

    m_dagmanager_log = new ofstream();
    m_logManager = t_Logger::getInstance();
    m_dagmanager_instance_number++;
    m_copy_prefix_counter = 0;
    m_dagnode_count = 0;
    m_node_id_counter = 0;
    if(t_ConfigurationOptions::getConfig()->m_dagNodeIdFile.empty() == false)
    {
        
        ifstream dagIdFile(t_ConfigurationOptions::getConfig()->m_dagNodeIdFile.c_str());

	if (!dagIdFile.fail()) {
	    dagIdFile>>m_node_id_counter;
	    m_node_id_counter++;
	    cerr<<"Starting Dag Node Id from "<<m_node_id_counter<<endl;
	    dagIdFile.close();
	}
    }
    
    string log_file_name = 
	t_ConfigurationOptions::getConfig()->m_sLogDir + "/DAGManager_Instance" + 
	integerToString(m_dagmanager_instance_number) + ".log";
    m_dagmanager_log->open(log_file_name.c_str());
    m_logManager->LOG("DAGManager instance " + toString(m_dagmanager_instance_number) + " created", m_dagmanager_log, c_RunLevelVerbosity);
    //cout<<"Check the log in the file "<<log_file_name<<endl;
    m_logManager->LOG("DAGManager instance " + integerToString(m_dagmanager_instance_number) + " created", m_dagmanager_log, c_RunLevelVerbosity);
    m_logManager->LOG("Check the log in the file " + log_file_name + "\n", m_dagmanager_log, c_DebugLevelVerbosity);

    m_logManager->LOG("Creating instance of DAG Manager in", m_dagmanager_log, c_DebugLevelVerbosity);
    if (restricted_mode_flag)
        m_logManager->LOG(" Restricted mode\n", m_dagmanager_log, c_DebugLevelVerbosity);
    else
        m_logManager->LOG(" Unique labels mode\n", m_dagmanager_log, c_DebugLevelVerbosity);
    m_logManager->LOG(" Simplify On Creating DAG  mode:" + toString(enableSimplifierOnCreation) + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
    m_simplifyDAGOnCreation = enableSimplifierOnCreation;
    //Initial setup for the list of root nodes
    m_head_roots.m_next_root = NULL;
    m_head_roots.m_prev_root = NULL;
    m_tail_roots = &m_head_roots;

    //Initial setup for the list of all nodes that will be generated in the DAG!
    m_head_nodes.m_next_node = NULL;
    m_head_nodes.m_prev_node = NULL;
    m_tail_nodes = &m_head_nodes;
    m_restricted_mode = restricted_mode_flag;
    m_enable_logging = enable_logging_flag;
    m_recursiveReplaceFlag = recursion_recompute_signatures;

    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
    	//Initial setup for death row
    	m_head_dead.m_next_dead = NULL;
    	m_head_dead.m_prev_dead = NULL;
    	m_tail_dead = &m_head_dead;
    	m_death_row_size = 0;
    	m_threshold_death_row_size = threshold_death_row_size;
    #endif

    m_logManager->LOG("Created DAGManager\n", m_dagmanager_log, c_DebugLevelVerbosity);
}


/**
 * Synopsis : Destructor for the DAGManager
 *
 * Description : It will dellocate all the memory that was allocated by the DAGManager
 * Check out for the error message by this guy to detect memory leaks
 */

t_DAGManager::~t_DAGManager()
{
    if(t_ConfigurationOptions::getConfig()->m_dagNodeIdFile.empty() == false) {

	ofstream dagIdFile(t_ConfigurationOptions::getConfig()->m_dagNodeIdFile.c_str());

	dagIdFile<<m_node_id_counter<<endl;
	dagIdFile.close();
    }
    return ;

    /* TODO: rakesh - Nov 24 - Need to check the code below to see if it is really needed. */
    
    //These resources(memory) are automatically freed when the program exits.
    //Free up all the resources that are allocated
    m_logManager->LOG("Invoking the destructor \n", m_dagmanager_log, c_DebugLevelVerbosity);



    //THIS IS NOT GARBAGE COLLECTION
    t_DAGNode *current_node = m_head_nodes.m_next_node;
    int counter = 0;
    m_logManager->LOG("Freeing the memory allocated \n", m_dagmanager_log, c_DebugLevelVerbosity);
    while (current_node != NULL)
    {
        t_DAGNode *node_to_delete = current_node;
        current_node = current_node->m_next_node;
        delete(node_to_delete);
        counter++;
    }
    m_logManager->LOG("Finished freeing the memory allocated \n", m_dagmanager_log, c_DebugLevelVerbosity);

    if (m_dagnode_count != counter)
    {
        m_logManager->LOG("ERROR!! Failed to reclaim memory allocated to all dag nodes \n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG(integerToString(m_dagnode_count) + " number of nodes were created\n", m_dagmanager_log, c_DebugLevelVerbosity);
        m_logManager->LOG(integerToString(counter) + " number of nodes were reclaimed\n", m_dagmanager_log, c_DebugLevelVerbosity);
        m_logManager->LOG("I could not recollect the memory allocated to " + integerToString(m_dagnode_count - counter) + " nodes\n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG("Please look for leaking memory, or there may be a bug in the DAGManager \n", m_dagmanager_log, c_RunLevelVerbosity);
    }
    else
    {
        m_logManager->LOG(integerToString(m_dagnode_count) + " number of nodes were created\n", m_dagmanager_log, c_DebugLevelVerbosity);
        m_logManager->LOG(integerToString(counter) + " number of nodes were reclaimed\n", m_dagmanager_log, c_DebugLevelVerbosity);
        m_logManager->LOG("Successfully reclaimed memory allocated to all dag nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    }

    m_logManager->LOG("Closing the log file\n", m_dagmanager_log, c_DebugLevelVerbosity);
    m_dagmanager_log->close();
    cout << "Destroyed the DAGManager\n";
    
}

/**
 * Synopsis: Create a node with a given label and no children
 *
 * Description: Creates a new DAG Node with given label and no children. 
 * In restricted mode, this function returns an old node, marking it as being shared (isShared =true) if the node with the specified label exists the DAG.
 * In user-defined labels mode, this function returns an error if the node with the same label exists in the DAG.
 *
 * 
 * Otherwise it creates a new node, marking it as unshared (isShared = false)
 * All flags have valid values on the node returned.
 *
 * Return value: DAGNode * to the node created, NULL if it fails
 *
 * See Also : CreateNode(string label, vector<DAGNode *> v_children, void *value)
 *            buildDAGFromFile
 */

t_DAGNode * t_DAGManager::createNode(
                                     const string& label,
                                     void *value //NULL by default, see .h file
                                     )
{

    /*The moment you create a new node successfully, you should add it to the list of nodes created. If you are returning an old node, then no need to do this.
      Maintaining this list is responsibility of DAGManager, not DAGNode!!
     */
    if (label == "")
        return NULL;

    if (m_restricted_mode)
    {
        m_logManager->LOG("Creating a leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        string signature = createSignature(label);
        t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
        if (search_signature.success())
        {
            // a node with this key exists in the hashtable
            //This is not a new node....
            //This node is a shared node in the DAG!!
            t_DAGNode *result_node = search_signature.getValue();
            result_node->m_isShared = true;

	    #ifndef QE //#ifndef added by Ajith John on 9 June 2013
            	result_node->m_reference_count++;
            	//When this node is returned, whoever holds it has a pointer to this node
            	//Therefore its reference count should be increased by 1
	    #endif 

            m_logManager->LOG("Found a node with label " + label + " in the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
            m_logManager->LOG("Sharing the requested node\n", m_dagmanager_log, c_DebugLevelVerbosity);
            m_logManager->LOG("Created node successfully\n", m_dagmanager_log, c_DebugLevelVerbosity);
            return result_node;
        }
        else
        {
            m_logManager->LOG("Creating a new DAGNode \n", m_dagmanager_log, c_DebugLevelVerbosity);
            t_DAGNode *result_node = new t_DAGNode(label, value);
            result_node->m_isShared = false;
            m_dagnode_count++;
            result_node->m_reference_count = 0; //somebody is holding a handle to this node!!
            result_node->m_signature = signature;
            //count memory
            countBytesUsedByANodeInMemoryUsage(result_node);

            /**
             * Modification on 9-04-2011
             */
            result_node->m_replaceBy=NULL;
            result_node -> m_node_id = ++m_node_id_counter;
            m_logManager->LOG("signature: " + signature +" ID:"+toString(result_node->getNodeID())+ "\n", m_dagmanager_log, c_DebugLevelVerbosity);
            t_HTStatusValue <t_DAGNode *> insert_result = ht.hashtable_insert(signature, result_node);
            if (insert_result.success())
            {
                result_node->m_signature = signature;
                //This is a new node and this is a root. A new node is always a root
                bool addedToRootsList = insertNodeInRootsList(result_node);
                bool addedToNodesList = insertNodeInNodesList(result_node);
                m_logManager->LOG("Created node successfully\n", m_dagmanager_log, c_DebugLevelVerbosity);
                return result_node;
            }
            else
            {
                m_logManager->LOG("ERROR!! Failed Creating a node with label " + label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
                return NULL;
            }
        }
    }
    else if (!m_restricted_mode) //user-defined labels mode
    {
        //In this case, the label of the node itself is its signature
        m_logManager->LOG("Creating a leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        string signature = createSignature(label);
        t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
        if (search_signature.success())
        {
            m_logManager->LOG("Found a node with label " + label + " in the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
            m_logManager->LOG("ERROR!! Failed creating new node with label " + label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
            return NULL;
        }
        else
        {
            m_logManager->LOG("Creating a new DAGNode \n", m_dagmanager_log, c_DebugLevelVerbosity);
            t_DAGNode *result_node = new t_DAGNode(label, value);
            m_dagnode_count++;
            result_node->m_isShared = false;
            result_node->m_reference_count = 0;
            result_node->m_signature = signature;
            //count memory
            countBytesUsedByANodeInMemoryUsage(result_node);
            result_node -> m_node_id = ++m_node_id_counter;
            /**
             * Modification on 9-04-2011
             */
            result_node->m_replaceBy=NULL;
            t_HTStatusValue <t_DAGNode *> insert_result = ht.hashtable_insert(signature, result_node);
            if (insert_result.success())
            {
                //This is a new node and this is a root
                bool addedToRootsList = insertNodeInRootsList(result_node);
                bool addedToNodesList = insertNodeInNodesList(result_node);
                m_logManager->LOG("Created node successfully\n", m_dagmanager_log, c_DebugLevelVerbosity);
                return result_node;
            }
            else
            {
                m_logManager->LOG("ERROR!! Failed Creating a node with label " + label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
                return NULL;
            }
        }


    }
    return NULL;
}

/**
 * Synopsis: Create a node with specified operands (i.e. a non-leaf node )
 *
 * Description: Creates a new DAG Node with specified operands. 
 * In restricted mode, this function returns an old node, marking it as being shared (isShared =true) if the node with the same signature exists the DAG.
 * In user-defined labels mode, it returns NULL if a node with same label (but perhaps a different set of children) exitst in the DAG
 * Otherwise it creates a new node, marking it as unshared (isShared = false)
 * All flags have valid values on the node returned.
 *
 * Return value: DAGNode * to the node created, NULL if it fails
 *
 * See Also : DAGManager::CreateNode
 */


t_DAGNode * t_DAGManager::createNode(
                                     const string& label,
                                     const vector<t_DAGNode*>& v_children,
                                     void *value, //NULL by default, see .h file
				     bool remember_simplified_dag_after_rewrite_and_return_unsimplified_dag_before_rewrite, // false by default, see .h file
				     bool recall_simplified_dag_from_last_createNode_call // false by default, see .h file
                                     )
{
    static t_DAGNode * simplified_dag_after_rewrite = NULL;
    if (m_restricted_mode)
    {
	if (recall_simplified_dag_from_last_createNode_call) {
	    t_DAGNode * ret_val = simplified_dag_after_rewrite;
	    simplified_dag_after_rewrite = NULL;
	    return ret_val;
	}
	
        if (label == "")
            return NULL;
        int num_operands = v_children.size();
        m_logManager->LOG("Creating a non-leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        string signature = createSignature(label, v_children); //This is a nonleaf
        t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
        if (search_signature.success())
        {
            // a node with this key exists in the hashtable
            //This node is a shared node in the DAG!!
            m_logManager->LOG("Sharing a non-leaf node with label\n", m_dagmanager_log, c_DebugLevelVerbosity);
            t_DAGNode *result_node = search_signature.getValue();
            result_node->m_isShared = true;

	    #ifndef QE //#ifndef added by Ajith John on 9 June 2013
            	result_node->m_reference_count++;
            	//When this node is returned, whoever holds it has a pointer to this node
            	//Therefore its reference count should be increased by 1
	    #endif 

            m_logManager->LOG("Created a non-leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
            return result_node;
        }
        else
        {
            m_logManager->LOG("Creating a new non-leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);            
            t_DAGNode *result_node = new t_DAGNode(label, v_children, value);

           m_dagnode_count++;
            result_node->m_isShared = false;
            result_node->m_isLeaf = false;
            result_node->m_reference_count = 0;
            result_node->m_signature = signature;
            /**
             * Modification on 9-04-2011
             */            
            result_node->m_replaceBy=NULL;
            //count memory
            //countBytesUsedByANodeInMemoryUsage(result_node);
            result_node -> m_node_id = ++m_node_id_counter;
            m_logManager->LOG("signature: " + signature +" ID:"+toString(result_node->getNodeID())+ "\n", m_dagmanager_log, c_DebugLevelVerbosity);
            for (int i = 0; i < num_operands; i++)
                {
                   // v_children[i]->m_in_list.push_back(result_node);

		    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
			/* We should not be just increasing the m_reference_count of children. 
		     	If m_reference_count becomes 1 and if the child is in m_death row, 
		     	then the child should be removed from m_death_row. Also, we should
		     	do the incrementing m_reference_count and pulling out from m_death row
		     	recursively on the children */
			incrementReferenceCount(v_children[i]);
		    #else
                    	v_children[i]->m_reference_count++;
		    #endif
                }

            t_HTStatusValue <t_DAGNode *> insert_result = ht.hashtable_insert(signature, result_node);
            if (insert_result.success())
            {
                result_node->m_signature = signature;
                //This is a new node and this is a root
                bool addedToRootsList = insertNodeInRootsList(result_node);
                bool addedToNodesList = insertNodeInNodesList(result_node);
                /** /for (int i = 0; i < num_operands; i++)
                {
                    v_children[i]->m_reference_count++;
                } */
                m_logManager->LOG("Created a non-leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
		
		t_DAGNode * unsimplified_dag_before_rewrite = result_node;
		    
		if(m_simplifyDAGOnCreation)
                {
                    ds->runPriorityStrategyV2(result_node);
                }
		if (remember_simplified_dag_after_rewrite_and_return_unsimplified_dag_before_rewrite) {
		    simplified_dag_after_rewrite = result_node;
		    return unsimplified_dag_before_rewrite;
		}
		else {
		    return result_node;
		}
            }
            else
            {
                m_logManager->LOG("ERROR!! Failed Creating a non-leaf node with label " + label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
                return NULL;
            }
        }
    }

    else if (!m_restricted_mode)
    {
        if (label == "")
            return NULL;
        int num_operands = v_children.size();
        //In this case, the label of the node itself is its signature
        string signature = createSignature(label);
        t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
        if (search_signature.success())
        {
            m_logManager->LOG("ERROR!! Failed Creating a non-leaf node with label " + label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
            return NULL;
        }
        else
        {
            m_logManager->LOG("Creating a non-leaf in user-defined labels  mode \n", m_dagmanager_log, c_DebugLevelVerbosity);
            t_DAGNode *result_node = new t_DAGNode(label, v_children, value);
            m_dagnode_count++;
            result_node->m_isShared = false;
            result_node->m_reference_count = 0;
            result_node->m_signature = signature;
            /**
             * Modification on 9-04-2011
             */
            result_node->m_replaceBy=NULL;
            //count memory
            countBytesUsedByANodeInMemoryUsage(result_node);
            result_node -> m_node_id = ++m_node_id_counter;

            t_HTStatusValue <t_DAGNode *> insert_result = ht.hashtable_insert(signature, result_node);
            if (insert_result.success())
            {
                //This is a new node and this is a root
                bool addedToRootsList = insertNodeInRootsList(result_node);
                bool addedToNodesList = insertNodeInNodesList(result_node);
                //increment the reference count of the children of this node
                for (int i = 0; i < num_operands; i++)
                    v_children[i]->m_reference_count++;
                m_logManager->LOG("Created a non-leaf node with label " + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
                return result_node;
            }
            else
            {
                m_logManager->LOG("ERROR!! Failed Creating a non-leaf node with label " + label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
                return NULL;
            }
        }

    }
    return NULL;
}

/**
 * Synopsis: Byte counter
 *
 * Description: This function just sums up the number of bytes of information contained in a DAGNode.
 */

inline void t_DAGManager::countBytesUsedByANodeInMemoryUsage(t_DAGNode *node)
{
    m_memory_usage += sizeof (t_DAGNode);
    m_memory_usage += node->m_label.size();
    m_memory_usage += node->m_out_list.size() * sizeof (t_DAGNode *);
    m_memory_usage += node->m_signature.size();

}

/**
 *Synopsis:  The nodes in the DAG keep adding or getting deleted. The isShared flag on a node may carry inconsistent value in such cases.
 * Use this function to Re-Identify and Re-Mark the shared nodes
 *
 * Description: This function marks the shared nodes in the DAG by performing a DFS. Every node that is visited more than once is marked as shared.
 *
 * Precondition: None!!
 * Postcondition: Ensures that visited flag of all DAGNodes is set at the end. All flags of all DAGNodes have consistent value
 * 
 * Return value: true if it succeeds, false otherwise
 *
 * See Also : markSharedNodesRecursively
 */
int t_DAGManager::markSharedNodes(t_DAGNode *node)
{

    /*-------
      If there are some delete operations on the DAG, then there is a chance that the node that was marked as shared now becomes unshared.
      There are two things that can be done on this...
      1. A node having isShared=true && reference_count >1 is indeed a shared node AND
      A node having isShared=true && reference_count <=1 is not shared.
      This logic relies heavily on the correctness of reference count code!!
      2. Rerun the algorithm to mark the shared nodes...
      This is a better way certainly....
      -------*/

    if (node == NULL)
        return -1;


    m_logManager->LOG("Marking the Shared Nodes..\n", m_dagmanager_log, c_DebugLevelVerbosity);
    //Resetting visited flag of all nodes is needed here in order to ensure correctness
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
        return -1;

    int count = 0;
    m_logManager->LOG("Marking now\n", m_dagmanager_log, c_DebugLevelVerbosity);
    bool mark_shared = markSharedNodesRecursively(node, count);
    m_logManager->LOG("Finished marking the Shared Nodes..\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return count;
}

/**
 * Synopsis: Mark the shared nodes in the DAG
 *
 * Description: When the DAG is JUST created, the shared nodes are marked correctly. However, the DAG may undergo a number of delete operations or rewrite operations, which may not be able to maintain the consistency of isShared flag of the DAG nodes. This function marks shared nodes in the DAG.
 *
 * Arguments: A non-null handle to a DAGNode, A reference to an integer that counts number of shared nodes. (Create an integer and set it to 0, pass its reference here)
 *
 * Return value: true if it succeeds, false otherwise
 *
 * Side Effects: Argument count finally contains the number of nodes that are marked as shared
 */


bool t_DAGManager::markSharedNodesRecursively(t_DAGNode *node, int &count)
{

    if (node == NULL)
        return false;

    if (node->m_visited == true)
    {
        count++;
        node->m_isShared = true;
        return true;
    }
    node->m_visited = true;
    int out_list_size = node->m_out_list.size();
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        bool marked_child = markSharedNodesRecursively(*it, count);
        if (marked_child == false)
        {
            m_logManager->LOG("ERROR!! Failed marking shared nodes recursively\n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }

    return true;
}

/**
 * Synopsis: Print the DAG
 *
 * Description: Prints the DAG to stdout..Since this function prints the DAG with an explicit indication of the shared nodes, it is a 4 step process
 * 1- Mark the shared nodes
 * 2- Assign temperory names to shared nodes
 * 3- print the root expression
 * 4- print expressions of shared nodes
 * In order to print the expressions of shared nodes, it uses shared_nodes_queue in which it temperorily queues up the shared nodes, while marking them
 *
 * Return value: false if it cannot print the DAG
 *               true on success
 *
 * See Also: printDAGRecursively, markSharedNodes, ResetVisitedFlagOfDAG, assignTempIDsAndBuildSharedNodesQ, printSharedNodesFromQueue
 */


bool t_DAGManager::printDAG(t_DAGNode *node, ofstream *outfile)
{

    m_logManager->LOG("Printing the DAG \n", m_dagmanager_log, c_DebugLevelVerbosity);
    //markSharedNodes has no precondition
    int mark_shared_nodes_count = markSharedNodes(node);
    if (mark_shared_nodes_count == -1)
    {
        m_logManager->LOG("ERROR!! Failed marking shared nodes\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }



    //Since assignTempIDsAndBuild* requires visited flag of DAG be resetted
    //After a call to markSharedNodes, we are sure about the consistency of the visited flag of DAG
    //So invoke ResetVisitedFlagOfDAG
    bool reset_visited = ResetVisitedFlagOfDAG(node);
    if (reset_visited == false)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAG\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    vector<t_DAGNode *> shared_nodes_queue;
    int temp_id_counter = 1;
    bool assign_temp_ids = assignTempIDsAndBuildSharedNodesQ(node, temp_id_counter, shared_nodes_queue);


    *outfile << "root := ";
    bool print_root_expr = printDAGRecursively(node, true, outfile);
    if (print_root_expr == false)
    {
        m_logManager->LOG("ERROR!! Failed printing the expression of the Root node of the DAG \n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    *outfile << endl;


    bool print_shared_expressions = printSharedNodesFromQueue(outfile, shared_nodes_queue);
    if (print_shared_expressions == false)
    {
        m_logManager->LOG("ERROR!! Failed printing shared nodes from queue\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    m_logManager->LOG("Successfully printed the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Assign temperory ids to shared nodes
 *
 * Description: This is a function used during the common subexpression identification process which is used to mark the shared nodes with temperory identifiers. It is recursive.
 *
 * Precondition: Requires the visited flag of DAGNodes be resetted
 *
 * Return value: false if it cannot print the DAG
 *               true on success
 * 
 *See Also: printDAG, printDAGRecursively, markSharedNodes, ResetVisitedFlagOfDAG, printSharedNodesFromQueue
 */

bool t_DAGManager::assignTempIDsAndBuildSharedNodesQ(t_DAGNode *node, int &temp_id_counter, vector<t_DAGNode *> &shared_nodes_queue)
{

    m_logManager->LOG("Assigning the temporary IDs anb building the shared nodes queue\n", m_dagmanager_log, c_DebugLevelVerbosity);
    /*--Visited flags are reset. Shared flags are on */
    if ((!(node->m_visited) && node->m_isShared))
    {
        node->m_temp_id = temp_id_counter;
        temp_id_counter++;
        node->m_visited = true;
        shared_nodes_queue.push_back(node);
    }
    else if (node->m_visited)
    {
        return true;
    }
    node->m_visited = true;
    int num_operands = node->m_out_list.size();
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        bool assign_ids = assignTempIDsAndBuildSharedNodesQ(*it, temp_id_counter, shared_nodes_queue);
        if (assign_ids == false)
        {
            m_logManager->LOG("ERROR!! Failed to assign temporary ids to DAGnodes", m_dagmanager_log, c_DebugLevelVerbosity);
            return false;
        }
    }
    m_logManager->LOG("Successfully assigned the temporary IDs anb building the shared nodes queue\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Print the expressions of shared nodes
 *
 * Description: Prints the expressions of shared nodes to stdout from shared_nodes_queue
 *
 * Return value: false if it cannot print the DAG
 *               true on success
 *
 * See Also: printDAG, printDAGRecursively, markSharedNodes, ResetVisitedFlagOfDAG, assignTempIDsAndBuildSharedNodesQ
 */

bool t_DAGManager::printSharedNodesFromQueue(ofstream *outfile, vector<t_DAGNode *> & shared_nodes_queue)
{
    m_logManager->LOG("Printing the shared nodes from the queue\n", m_dagmanager_log, c_DebugLevelVerbosity);
    int num_shared_nodes = shared_nodes_queue.size();
    for (int i = 0; i < num_shared_nodes; i++)
    {
        *outfile << "$" << shared_nodes_queue[i]->getLabel() << "_" << shared_nodes_queue[i]->m_temp_id << " :=  ";
        bool print_expression = printDAGRecursively(shared_nodes_queue[i], true, outfile);
        *outfile << endl;
        if (!print_expression)
        {
            m_logManager->LOG("ERROR!! Failed printing a node from shared nodes queue\n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }
    m_logManager->LOG("Successfully Printed the shared nodes from the queue\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Print the root expression of a DAG
 *
 * Description: Prints the root expression of a DAG to outfile, recursively
 * It is important to node that shared nodes are suffixed with their temp_ids during printing. This helps in reading back the DAG from the file.
 *
 * Return value: false if it cannot print the DAG
 *               true on success
 * 
 * See Also: printDAG, markSharedNodes, ResetVisitedFlagOfDAG, assignTempIDsAndBuildSharedNodesQ, printSharedNodesFromQueue
 */


bool t_DAGManager::printDAGRecursively(t_DAGNode *node, bool isRoot, ofstream *outfile)
{
    if (node == NULL)
        return false;

    if (node->m_isShared && !isRoot)
    {

        *outfile << "$" << node->getLabel() << "_" << node->m_temp_id;
        return true;
    }
    else
    {

        *outfile << node->getLabel();
    }

    int num_operands = node->m_out_list.size();
    if (num_operands > 0)
    {
        *outfile << "(";
        int i = 0;
        for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
        {
            bool printed_child = printDAGRecursively(*it, false, outfile);
            if (i != num_operands - 1)
                *outfile << ",";
            if (printed_child == false)
                return false;
            i++;
        }
        *outfile << ")";
    }
    return true;
}

/**
 * Synopsis : This function returns the size of the DAG, viewed as a DAG
 *
 * Arguments: Handle to a DAGNode
 *
 *Return value: 0 if it fails or a null DAG
 *              >=1 on success
 */



int t_DAGManager::getSizeOfDAG(t_DAGNode *node)
{
    //reset visited flag of all DAGNodes to be sure about the consistency
    m_logManager->LOG("Obtaining the size of DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
        return 0;

    int count = getCountOfNodesInDAGRecursively(node);
    m_logManager->LOG("Succesfully obtainedthe size of the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return count;
}

/**
 * Synopsis: This is a private function, counts number of DAG nodes viewed as a DAG
 *
 * Description: This is a recursive function. It traverses the DAG marking the nodes as visited, and keeps counting the nodes in the DAG
 *
 *Precondition: The visited flags of reachable DAGNodes should be resetted.
 *
 * Return value: Count of DAG Nodes, viewed as DAG
 *
 */
int t_DAGManager::getCountOfNodesInDAGRecursively(t_DAGNode *node)
{
    if (node == NULL)
        return 0;
    int count = 1; //Count this node
    if (node->m_visited == true)
        return 0; //this node was already counted!!

    node->m_visited = true;
    int num_operands = node->m_out_list.size();
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)

    {
        count = count + getCountOfNodesInDAGRecursively(*it);
    }
    return count;
}

/**
 * Synopsis : Find size of DAG, starting from given DAGNode viewed as a tree.
 *
 * Return value : -1, if it fails
 *                >=0, on success
 */

int t_DAGManager::getSizeOfDAGAsTree(t_DAGNode *node)
{
    m_logManager->LOG("Obtaining the size of DAG as a Tree\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node->m_out_list.size() == 0)
        return 1;
    else
    {
        int size = 1;

        for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
            size += getSizeOfDAGAsTree(*it);
        m_logManager->LOG("Obtained the size of DAG as a Tree\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return size;
    }
}

/**
 * Synopsis: this function resets the visited flags of all the DAG nodes contained in the DAG Manager by traversing the queue of DAGNodes
 *
 * Description: This function traverses the linked list of all DAGNodes created in the DAGManager starting from head_nodes and goes on resetting visited flag of every node (including the nodes that have possibly became garbage)
 *
 * Precondition: None
 *
 * Return value: true if it succeeds, false otherwise
 *
 * Complexity: O(number of nodes in the DAGManager)
 *
 * See Also: ResetVisitedFlagOfDAG
 */


bool t_DAGManager::ResetVisitedFlagOfAllDAGNodes()
{
    m_logManager->LOG("Resetting the visited flag of all DAG nodes by traversing the list of DAG Nodes\n", m_dagmanager_log, c_DebugLevelVerbosity);
    t_DAGNode *current_node = &m_head_nodes;
    int counter = 0;
    while (current_node->m_next_node != NULL)
    {
        current_node = current_node->m_next_node;
        current_node->m_visited = false;
        counter++;
    }
    if (current_node != m_tail_nodes)
    {
        m_logManager->LOG("ERROR!! Failed to reset visited flags of all dag nodes \n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    m_logManager->LOG("Successfully resetted visited flags of all dag nodes\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: this function simply prints the DAG to specified file
 *
 * Description: This function is same as printDAGAsTree except that it prints it to a file
 *
 * Return value: true if it succeeds, false otherwise
 *
 *
 */


bool t_DAGManager::printDAGAsTreeToFile(
                                        t_DAGNode *node,
                                        ofstream *outfile
                                        )
{
    bool result;
    *outfile << node->m_label;
    if (!node->m_isLeaf)
    {
        *outfile << "(";
        int out_list_size = node->m_out_list.size();
        int i = 0;
        for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
        {
            bool result = printDAGAsTreeToFile(*it, outfile);
            if (!(i == out_list_size - 1))
                *outfile << ",";

            if (result == false)
            {
                m_logManager->LOG("ERROR!! Failed printing DAGAsTreeToFile\n", m_dagmanager_log, c_RunLevelVerbosity);
                return false;
            }
            i++;
        }
        *outfile << ")";
    }

    return true;

}

/**
 * Synopsis: print DAG in graphviz DOTTY format
 *
 * Description: This function prints the DAG in DOTTY format to a file. The output file can be visually observered using dotty
 *
 * Return value: true if it succeeds , false otherwise
 *
 *
 */

bool t_DAGManager::printDAGAsDOTFile(t_DAGNode *node, ofstream *outfile)
{
    /*-------
      The nodes in the DAG that are shared are indeed marked as being shared while they are created.
      Unless there is a deleteDAG operation executed on the DAG, a node that is shared has no opportunity for becoming unshared???
      -------*/
    m_logManager->LOG("Printing DAG as DOT\n", m_dagmanager_log, c_DebugLevelVerbosity);
    *outfile << "digraph G{" << endl;

    m_logManager->LOG("Marking shared nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    int mark_shared_nodes_count = markSharedNodes(node);
    if (mark_shared_nodes_count == -1)
    {
        m_logManager->LOG("Found shared_node_count = -1 in printDAGAsDOTFile\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    m_logManager->LOG("Resetting visited flag\n", m_dagmanager_log, c_DebugLevelVerbosity);
    bool reset_visited = ResetVisitedFlagOfDAG(node);
    if (reset_visited == false)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAG\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    m_logManager->LOG("Printing the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    bool printed_dot = printDAGAsDOTFileRecursively(node, outfile);
    if (printed_dot == false)
    {
        m_logManager->LOG("ERROR!! Failed printing DAGAsDot recursively\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    *outfile << "}" << endl;
    return true;
}

bool t_DAGManager::printDAGAsDOTFileRecursively(t_DAGNode *node, ofstream *outfile)
{

    if (node == NULL)
    {
        return false;
    }
    if (node->m_visited == true)
        return true;
    node->m_visited = true;

    *outfile << "n" << node << "[label=\"" << node->getLabel() << "\\nShared: " << node->m_isShared << "\\nRoot: " << node->m_isRoot << "\\n Leaf " << node->m_isLeaf << "\\nRC:" << node->m_reference_count << "\\nCC:" << node->m_copy_count << "\"];" << endl;
    int num_operands = node->m_out_list.size();

    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        *outfile << "n" << node << "->" << "n" << *it << endl;
    }


    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        bool printed_operands = printDAGAsDOTFileRecursively(*it, outfile);
        if (printed_operands == false)
        {
            m_logManager->LOG("ERROR!! Failed call to printDAGAsDOTFileRecursively\n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }
    return true;
}

/**
 * This is a private function, friend of both DAGNode and DAGManager.
 * This function converts a pointer address to a hex string.
 * This function is used for generating the (internal) unique key associated with each node
 *
 * Return Value : 0xabcdef123 --> "abcdef123"
 *                invalid hex number --> ""
 */

string addressToString(void *ptr)
{
    //m_logManager->LOG("Address of ptr is "<<ptr<<endl;
	string result = toString(ptr);
    //m_logManager->LOG("Size of void pointer is "<<sizeof(void *)<<endl;
/*
    unsigned long int num = (unsigned long int) ptr;
    for (int i = 1; i <= sizeof (void *) *2; i++)
    {
        int nump = num & 0xf;
        num = num >> 4;
        switch (nump)
        {
        case 0:
            result = "0" + result;
            break;
        case 1:
            result = "1" + result;
            break;
        case 2:
            result = "2" + result;
            break;
        case 3:
            result = "3" + result;
            break;
        case 4:
            result = "4" + result;
            break;
        case 5:
            result = "5" + result;
            break;
        case 6:
            result = "6" + result;
            break;
        case 7:
            result = "7" + result;
            break;
        case 8:
            result = "8" + result;
            break;
        case 9:
            result = "9" + result;
            break;
        case 10:
            result = "a" + result;
            break;
        case 11:
            result = "b" + result;
            break;
        case 12:
            result = "c" + result;
            break;
        case 13:
            result = "d" + result;
            break;
        case 14:
            result = "e" + result;
            break;
        case 15:
            result = "f" + result;
            break;
        default:
            return ""; //This is really important. If it cannot convert a hex number to string, it will return empty string
            break;
        }
    }
*/
   // result=result.substr(2,result.length()-2);
    return result;

}

/**
 * Synopsis: Marks the operands of a new root node as non-root
 *
 * Description: When a new DAG is created, it will finally be
 * represented in terms of a DAGNode. This is a new root. The operands
 * of this DAG MAY or MAYNOT be roots. Since all the operands of this
 * new root are now reachable from this root, we mark the operands as
 * non root.

 * A caller may still view these operands as roots. But that is its
 * responsibility. All we care about is whether all the valid DAG is
 * still reachable from the available set of roots in the DAGManager.
 *
 * Return value: true if it succeeds, false otherwise
 *
 */


bool t_DAGManager::deleteOperandsOfANodeFromRootsList(t_DAGNode *node)
{

    m_logManager->LOG("Deleting operands of a node from roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    //Put the non-root nodes off the list...DO NOT DELETE THEM
    if (node!=NULL && !(node->m_out_list.empty()))
    {
        int size = node->m_out_list.size();

        for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
        {
            //Delete every  child of node from roots list
            t_DAGNode *node_to_delete = (*it);
            if (!(node_to_delete-> m_isRoot))
            {
                continue;
            }
            if (node_to_delete->m_next_root == NULL)
            {
		// if--elseif conditions added by Ajith John on 9 June 2013
		if (node_to_delete != m_tail_roots)
		{
			m_logManager->LOG("Error in t_DAGManager::deleteOperandsOfANodeFromRootsList!! trying to delete the last node in roots_list, but m_tail_roots is not pointing to the last node\n", m_dagmanager_log, c_RunLevelVerbosity);  
			return false;			
		}
		else if (node_to_delete->m_prev_root == NULL)
		{
			m_logManager->LOG("Error in t_DAGManager::deleteOperandsOfANodeFromRootsList!! attempt to delete head_roots\n", m_dagmanager_log, c_RunLevelVerbosity);  
			return false;			
		}
		else
		{
                	//This is the last node in the list
                	node_to_delete->m_isRoot = false;

			m_tail_roots = node_to_delete->m_prev_root;// line added by Ajith John on 9 June 2013
                	node_to_delete->m_prev_root->m_next_root = NULL;
                	node_to_delete->m_next_root = NULL;
                	node_to_delete->m_prev_root = NULL;
		}

            }
            else if (node_to_delete->m_prev_root == NULL)
            {
                m_logManager->LOG("ERROR!! Failed deleting operands of a node from roots list\n", m_dagmanager_log, c_RunLevelVerbosity);
                m_logManager->LOG("Could not find head_roots \n", m_dagmanager_log, c_RunLevelVerbosity);
                return false;
                //cannot happen, since there is a header node head_roots from which the list has originated. This node is never NULL!!
                //This is the purpose of having the head node
            }
            else
            {
                node_to_delete->m_isRoot = false;
                node_to_delete->m_prev_root->m_next_root = node_to_delete->m_next_root;
                node_to_delete->m_next_root->m_prev_root = node_to_delete->m_prev_root;
                node_to_delete->m_next_root = NULL;
                node_to_delete->m_prev_root = NULL;
            }

        }
    }

    m_logManager->LOG("Successfully Deleted operands of a node from roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Detailed deletion of a node from DAG
 *
 * Description: When deleting a node from dag, it may not be possible to delete it directly...This function takes care for details of deletion process. STRICTLY PRIVATE use.
 *
 * Return Value: True if it succeeds, false otherwise
 *
 */

bool t_DAGManager::deleteARootNodeFromDAG(t_DAGNode *root)
{

    m_logManager->LOG("Deleting a root from DAG  :", m_dagmanager_log, c_DebugLevelVerbosity);
    m_logManager->LOG(root->m_label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
    //Let this node hang around as an unreferenced node
    //garbage collector will take care for its deletion
    if (root->m_isRoot)
    {
        //This was a root node
        //remove it from the list of root nodes
        bool deleted_from_roots = deleteNodeFromRootsList(root);

        if (deleted_from_roots == false)
        {
            m_logManager->LOG("Unable to delete the root\n", m_dagmanager_log, c_DebugLevelVerbosity);
            return false;
        }
        root->m_isRoot = false;

	#ifndef QE //#ifndef added by Ajith John on 9 June 2013
	        root->m_reference_count = 0;
	#endif
    }


    m_logManager->LOG("Deleted a root from DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Get count of ROOT nodes in the DAG
 *
 * Description: This function returns the number of nodes present in the list of root nodes. Note that a caller may have more handles than the number of roots present in this list.
 * This list just contains enough root nodes from which all the VALID dag is reachable. The caller may be interpreting some more nodes as roots.
 *
 * Return value: >=0, the count of nodes on the roots list
 *
 *
 */

int t_DAGManager::getCountOfRootNodes()
{
    t_DAGNode *current_node = m_head_roots.m_next_root;
    if (current_node == NULL)
        return 0;
    int counter = 0;
    while (current_node != NULL)
    {
        counter++;
        current_node = current_node->m_next_root;
    }
    return counter;
}

/**
 * Synopsis: Removes root status of a DAGNode
 *
 * Description: Marks a node as non root by deleting it from the list
 * of root nodes maintained by the DAGManager It does not delete any
 * node, just reorganizes roots list. When a node is deleted from
 * roots list, its operands may become new roots, which is also
 * performed by this function.
 */
bool t_DAGManager::deleteNodeFromRootsList(t_DAGNode *node_to_delete)
{


    //
    m_logManager->LOG("Deleting a node from roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (!(node_to_delete-> m_isRoot))
    {
        m_logManager->LOG("Node to be deleted from roots list is not a root\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return false;
    }
    if (node_to_delete->m_next_root == NULL)
    {
	//if--elseif added by Ajith John on 9 June 2013
	if (node_to_delete != m_tail_roots)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteNodeFromRootsList!! trying to delete the last node in roots_list, but m_tail_roots is not pointing to the last node\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;			
	}
	else if (node_to_delete->m_prev_root == NULL)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteNodeFromRootsList!! attempt to delete head_roots\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;			
	}
	else
	{
	        node_to_delete->m_isRoot = false;
	
		m_tail_roots = node_to_delete->m_prev_root; //line added by Ajith John on 9 June 2013
	        node_to_delete->m_prev_root->m_next_root = NULL;

	        for (list<t_DAGNode *>::iterator it = node_to_delete->m_out_list.begin(); it != node_to_delete->m_out_list.end(); it++)
	        {
	            t_DAGNode *current_child = *it;
	            if (current_child->m_in_list.size() == 0)
	                insertNodeInRootsList(current_child);
	        }
	        return true;
	        //This is the last node in the list
	}
    }
    else if (node_to_delete->m_prev_root == NULL)
    {
        return false;
        m_logManager->LOG("ERROR!! Failed to hit head_roots when deleting a node from roots list\n", m_dagmanager_log, c_RunLevelVerbosity);
        //cannot happen
    }
    else
    {
        node_to_delete->m_isRoot = false;
        node_to_delete->m_prev_root->m_next_root = node_to_delete->m_next_root;
        node_to_delete->m_next_root->m_prev_root = node_to_delete->m_prev_root;
        for (list<t_DAGNode *>::iterator it = node_to_delete->m_out_list.begin(); it != node_to_delete->m_out_list.end(); it++)
        {
            t_DAGNode *current_child = *it;
            if (current_child->m_in_list.size() == 0)
                insertNodeInRootsList(current_child);
        }

        return true;
    }
    m_logManager->LOG("Successfully a node from roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
}

int t_DAGManager::getCollisionsInHashTable()
{
    return ht.getCountOfCollisions();
}

/**
 * Synopsis: Detach a sub-DAG from a DAG
 *
 * Description: This function detaches given sub-DAG from the main DAG. Given a sub-DAG indicated by node N, it looks at all the parents of N, and deletes N from their out_list. Then it sets reference count of N to 0 and clears its in-list. This effectively detaches the sub-DAG beneath N from the DAG.
 *
 * Return value: false if it cannot detach the DAG
 *               true on success
 */




bool t_DAGManager::deleteIncidentEdges(t_DAGNode *node)
{

    m_logManager->LOG("Deleting incident edges on a node\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
        m_logManager->LOG("Node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (node->m_in_list.size() == 0)
    {
        return true;
    }
    else
    {

        for (list<t_DAGNode*>::iterator inlist_it = node->m_in_list.begin(); inlist_it != node->m_in_list.end(); inlist_it++)
        {
            t_DAGNode *parent_i = *inlist_it;
            for (list<t_DAGNode*>::iterator it = parent_i->m_out_list.begin(); it != parent_i->m_out_list.end(); it++)
            {
                if ((*it) == node)
                {
                    parent_i->m_out_list.erase(it);
                    RecomputeSignatureOfANode(parent_i);
                    break;
                }
            }
        }
        node->m_in_list.clear();

	#ifndef QE //#ifndef added by Ajith John on 9 June 2013
        	node->m_reference_count = 0;
	#endif

        m_logManager->LOG("Successfully Deleted incident edges on a node\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }

}

/**
 * synopsis: Build signature of a DAG Node
 *
 * Description: Calculating the signature of a symbol node does not involve any operand addresses. The purpose of this function is to provide a shorthand for such nodes. This is also a PRIVATE function.
 *Arguments:
 *
 * Return value:  if the label is "", then ""
 *                else a valid signature string
 */

inline string t_DAGManager::createSignature(
                                            const string& label //label of the node to be created
                                            )
{

    vector<t_DAGNode *> dummy_vector;

    return createSignature(label, dummy_vector);
}

/**
 * synopsis: Build signature of a DAG Node
 *
 * Description: This function takes specifications of a DAGNode and builds a signature from it. This function builds signature for both leaf and non-leaf nodes. This function is PRIVATE to DAGManager
 *
 *Arguments:
 *
 * Return value:  if the label is "", then ""
 *                else a valid signature string
 */
template<typename Alloc>
inline string t_DAGManager::createSignature(
                                            const string& label, //Label of the node
                                            const vector<t_DAGNode*, Alloc> &v_operands //Operands of the node
                                            )
{
    if (label == "")
        return "";
    string signature = "";
    int num_operands = v_operands.size();
    for (int i = 0; i < num_operands; i++)
    {
        //cout<<"Creating signature for "<<i<<"of"<<num_operands<<":"<< (void*)v_operands[i]<<endl;
        signature = signature + addressToString(v_operands[i]);
    }
    signature += label;
    if (num_operands == 0) //this is a leaf node
        signature += "1";
    else //non-leaf node
        signature += "0";
    return signature;
}

template string t_DAGManager::createSignature(
                                            const string& label, //Label of the node
                                            const vector<t_DAGNode*, std::allocator<t_DAGNode*> > &v_operands //Operands of the node
                                            );

template string t_DAGManager::createSignature(
                                            const string& label, //Label of the node
                                            const vector<t_DAGNode*, MyAllocator<t_DAGNode*> > &v_operands //Operands of the node
                                            );

/**
 * Synopsis: PRIVATE function that inserts a node in the list of root nodes
 *
 * Description: The DAGManager internally maintains a list of roots of
 *DAGs. Since the DAGManager may contain more than one DAG, a list of
 *all root nodes acts as a container for all the root nodes. This
 *function inserts a node in the list of root nodes. This is a PRIVATE
 *function, and should be called on a node that is known to be
 *root. e.g. a newly created DAGNode As soon as it adds a node as a
 *root node, it will take care for making the operands of this node as
 *non-root.
 *
 * Arguments: Non-null DAGNode
 *
 * Return value: True, if it succeeds, false otherwise
 */
inline bool t_DAGManager::insertNodeInRootsList(t_DAGNode *node)
{
    m_logManager->LOG("Inserting a node in roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
        m_logManager->LOG("WARNING. Attempt to add a null node to the list of roots\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
	/* A node should be considered as root only if in_list's size is zero;
           A node's referece_count may be greater than zero due to external pointers */
	if (node->m_in_list.size() != 0)
	{
		m_logManager->LOG("ERROR!! Cannot add a node with non-zero in_list size to the list of root nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
        	return false;	
    	}
    #else
	if (node->m_reference_count != 0)
    	{
        	m_logManager->LOG("ERROR!! Cannot add a node with non-zero reference count to the list of root nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
        	return false;
    	}
    #endif
    
    m_logManager->LOG("Inserting a node in roots list \n", m_dagmanager_log, c_DebugLevelVerbosity);
    m_tail_roots->m_next_root = node;
    node->m_prev_root = m_tail_roots;
    node->m_next_root = NULL;
    m_tail_roots = node;
    node->m_isRoot = true;
    bool deleted_operands = deleteOperandsOfANodeFromRootsList(node);
    if (deleted_operands == false)
    {
        m_logManager->LOG("Could not delete the operands of a new root node from the roots list\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    m_logManager->LOG("Successfully Inserted a node in roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: PRIVATE function that inserts a node in the list of nodes
 *
 * Description: The DAGManager internally maintains a list of all
 * nodes of DAG. This list allows one to traverse the DAG in O(n),
 * allows garbage collection and prevents necessity to traverse DAG as
 * tree..
 *
 * Arguments: Non-null DAGNode
 *
 * Return value: True, if it succeeds, false otherwise
 */



inline bool t_DAGManager::insertNodeInNodesList(t_DAGNode *node)
{
    m_logManager->LOG("Inserting a node in nodes list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
        m_logManager->LOG("ERROR!! Failed Inserting a node in roots list, node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    m_tail_nodes->m_next_node = node;
    node->m_prev_node = m_tail_nodes;
    node->m_next_node = NULL;
    m_tail_nodes = node;
    m_logManager->LOG("Successfully Inserted a node in roots list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Public function that adds edge between two nodes...(Unique labels mode only)
 *
 * Description: This function allows to add edge between two nodes. Note that this function is costly, as it checks for possibility of creation of cycle when adding the edge. This check is done by testing whether the node to added as child appears in the ancestor list of parent...
 *
 * Arguments: DAGNode *parent: Node to be made parent
 *            DAGNode *child: Node to be made child
 *
 * Return value: True, if it succeeds, false otherwise
 * PostCondition: In restricted mode, adding edge between two nodes will change the signature of parent. There may exist a node in hashtable with same signature, which will replace this node, if found.
 */

bool t_DAGManager::addEdge(t_DAGNode *parent, t_DAGNode *child)
{
    m_logManager->LOG("Adding edge between two nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (parent == NULL || child == NULL)
    {
        m_logManager->LOG("ERROR!! Failed adding edge between two nodes, either parent or child is null \n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }


    set<t_DAGNode *> all_parents = getSetOfAncestorNodesByDFS(parent); //DFS or BFS does not matter
    set<t_DAGNode *>::iterator par_it;
    for (par_it = all_parents.begin(); par_it != all_parents.end(); par_it++)
    {
        if ((*par_it) == child)
        {
            m_logManager->LOG("Found child in the ancestors of parent, cannot add edge \n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }
    parent->m_out_list.push_back(child);
    child->m_in_list.push_back(parent);

    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
	/* You cannot just say ++ on reference_count;
           reference_count should be incremented using incrementReferenceCount function */
	incrementReferenceCount(child);
    #else
	child->m_reference_count++;
    #endif

    if (child->m_isRoot)
    {
        child->m_isRoot = false;
        bool del_as_root = deleteNodeFromRootsList(child);
        if (!del_as_root)
        {
            m_logManager->LOG("ERROR!! Failed to delete child node from the list of root nodes \n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }
    if (m_restricted_mode)
    {
        bool correct_sign = RecomputeSignatureOfANode(parent);
        if (!correct_sign)
        {
            m_logManager->LOG("ERROR!! Failed recomputing the signature of the parent \n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }
    m_logManager->LOG("Successfully Added edge between two nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;

}

/**
 * Synopsis: Public function that removes edge between two nodes...(User-Maintained labels mode only)
 *
 * Description: This function allows to remove edge between two nodes. It will remove FIRST edge between the two nodes.
 *
 * Arguments: DAGNode *parent: Node to be made parent
 *            DAGNode *child: Node to be made child
 *
 * Return value: -1 if it fails, integer >=0 representing the position of child in out-list of parent
 */

int t_DAGManager::removeFirstEdge(t_DAGNode *parent, t_DAGNode *child)
{
    m_logManager->LOG("Removing edge between two nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    int edge_counter = -1;
    if (parent == NULL || child == NULL)
    {
        m_logManager->LOG("ERROR!! Failed removing edge between two nodes \n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG("Either parent or child is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return edge_counter;
    }


    for (list<t_DAGNode*>::iterator it = parent->m_out_list.begin(); it != parent->m_out_list.end(); it++)
    {
        edge_counter++;
        if (*it == child)
        {
            parent->m_out_list.erase(it);
            break;
        }
    }
    for (list<t_DAGNode*>::iterator it = child->m_in_list.begin(); it != child->m_in_list.end(); it++)
    {
        if (*it == parent)
        {
            child->m_in_list.erase(it);
            break;
        }
    }

    if (m_restricted_mode)
    {
        //Now signature of the parent has changed. So delete and reinsert it in the hashtable
        bool correct_sign = RecomputeSignatureOfANode(parent);
        if (!correct_sign)
        {
            m_logManager->LOG("ERROR!! Failed recomputing the signature of the parent \n", m_dagmanager_log, c_RunLevelVerbosity);
            return -1;
        }

    }

    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
	/* You cannot just say -- on reference_count;
    	reference_count should be decremented using decrementReferenceCount function */ 
	decrementReferenceCount(child);
    #else
    	child->m_reference_count--;
    #endif

    m_logManager->LOG("Successfully removed edge between two nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    return edge_counter;

}

bool t_DAGManager::removeIthOutgoingEdgeOfANode(t_DAGNode *node, int i)
{
    m_logManager->LOG("Removing " + integerToString(i) + "th outgoing edge of a node\n", m_dagmanager_log, c_DebugLevelVerbosity);
    int edge_counter = 0;
    if (node == NULL)
    {
        m_logManager->LOG("ERROR!! Failed removing " + integerToString(i) + "th edge \n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG("Node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    t_DAGNode *child=NULL;
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        edge_counter++;
        if (edge_counter == i)
        {
            node->m_out_list.erase(it);
            child = (*it);
            break;
        }
    }

    for (list<t_DAGNode*>::iterator it = child->m_in_list.begin(); it != child->m_in_list.end(); it++)
    {
        if (*it == node)
        {
            child->m_in_list.erase(it);
            break;
        }
    }

    if (m_restricted_mode)
    {
        //Now signature of the node has changed. So delete and reinsert it in the hashtable
        bool correct_sign = RecomputeSignatureOfANode(node);
        if (!correct_sign)
        {
            m_logManager->LOG("ERROR!! Failed recomputing the signature of the node \n", m_dagmanager_log, c_RunLevelVerbosity);
            return -1;
        }

    }

    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
	/* You cannot just say -- on reference_count;
    	reference_count should be decremented using decrementReferenceCount function */ 
	decrementReferenceCount(child);
    #else
    	child->m_reference_count--;
    #endif

    m_logManager->LOG("Successfully removed " + integerToString(i) + "th edge of a node\n", m_dagmanager_log, c_DebugLevelVerbosity);

}

/**
 * Synopsis: Public function to allow resetting of visited flags of DAGNodes
 *
 * Description: This function is just an alias for ResetVisitedFlagOfAllDAGNodes, except that it is public. During external traversals, a client will need to reset the visited flag of DAG. However, since the client is also allowed to set/reset the visited flag, in order to ensure the correctness, we should reset the visited flag of all the DAGNodes by traversing the linked list of all nodes. Hence it is implemented this way....
 *
 *Return Value: true if it succeeds, false otherwise
 *
 * Precondition: None!!
 *
 * Postcondition: visited flag of all nodes is resetted
 */



bool t_DAGManager::ResetVisitedFlagOfDAGNodes()
{
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    return reset_visited;

}

/**
 * Synopsis: Private function used to reset visited flag of a DAG!
 *
 * Description: Several functions in DAGManager have a precondition that the visited flag of nodes should be resetted.
 * Use this function to reset the visited flag of nodes of a DAG.
 *
 * Arguments: Non Null DAGNode *
 *
 * Precondition: The visited flags of all nodes in the DAG must be consistent. Here, by consistent, we mean all either set or reset.
 * Note that the function operates recursively. If visited flag of a node is reset but not of its child, then this function will not work correctly. (In such cases, one should invoke the ResetVisitedFlagOfAllDAGNodes function that traverses the linked list of DAGNodes and resets their visited flag). 
 * However, when you are sure that the flags are consistent (e.g. After a call to markSharedNodes), you can bypass the overhead of resetting visited flag of all DAGNodes in the entire DAG and use this function.
 *
 * Complexity: O(number of nodes in the DAG starting from root "node")
 *
 * Return value: true if it succeeds, false otherwise
 *
 * See Also: ResetVisitedFlagOfAllDAGNodes
 */


bool t_DAGManager::ResetVisitedFlagOfDAG(t_DAGNode *node)
{
    m_logManager->LOG("Resetting visited flag of DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAG\n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG("Node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    node->m_visited = true;
    return ResetVisitedFlagOfDAGRecursively(node);
}

/**
 * Synopsis: Private function used to reset visited flag of a DAG!
 *
 * Description: This recursive function actually performs resetting of the flag!!
 *
 * Arguments: Non Null DAGNode *
 *
 * Return value: true if it succeeds, false otherwise
 */

bool t_DAGManager::ResetVisitedFlagOfDAGRecursively(t_DAGNode *node)
{
//    if( ! node->m_visited )
//        return true;
    if ((node->m_visited == false) && (node->m_isShared == true))
    {
        return true;
    }
    node->m_visited = false;
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        if (/*(*it)->m_visited &&*/ ResetVisitedFlagOfDAGRecursively(*it) == false)
            return false;
    }
    return true;
}

/**
 */





set<t_DAGNode *> t_DAGManager::getSetOfDescendentNodesByDFS(t_DAGNode *node)
{
    /* Begin Commented by Supratik: Nov 29, 2012
    vector<t_DAGNode *> nodes;
    nodes.clear();
    bool reset_visited = ResetVisitedFlagOfDAG(node);
    if (reset_visited == false)
    {
        m_logManager->LOG("Could not reset visited flag of DAG\n", m_dagmanager_log, c_RunLevelVerbosity);
        return nodes;
    }
    nodes = getChildrenInDFSRecursively(node);
    return nodes;
     * End Commented by Supratik: NOv 29, 2012 */

    set <t_DAGNode *> visitedSet;
    visitedSet.insert(node);
    
    list<t_DAGNode *> workingStack;
    t_DAGNode * workingNode;
    
    workingStack.push_back(node);
    while (workingStack.size() > 0) {
        workingNode = workingStack.back();
        workingStack.pop_back();
        for (list<t_DAGNode*>::iterator it = workingNode->m_out_list.begin(); it != workingNode->m_out_list.end(); it++)
        {
            if (visitedSet.find(*it) == visitedSet.end()) {
                visitedSet.insert((*it));
                workingStack.push_back((*it));
            }
        }              
    }
    return visitedSet;

}

/* Begin Commented by Supratik: Nov 28, 2012
vector<t_DAGNode *> t_DAGManager::getChildrenInDFSRecursively(t_DAGNode * node)
{
    vector<t_DAGNode *> nodes;
    if (node->m_visited == true)
        return nodes;
    node->m_visited = true;
    if (node->m_out_list.size() == 0)
    {
        nodes.push_back(node);
        return nodes;
    }
    nodes.push_back(node);
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        vector<t_DAGNode *> child_nodes = getChildrenInDFSRecursively(*it);
        for (int i = 0; i < child_nodes.size(); i++)
            nodes.push_back(child_nodes[i]);
    }

    return nodes;
}
* End Commented by Supratik: Nov 29, 2012 */

set<t_DAGNode *> t_DAGManager::getSetOfAncestorNodesByDFS(t_DAGNode *node)
{
   /* Begin Commented by Supratik: Nov 28, 2012
    vector<t_DAGNode *> nodes;
    nodes.clear();
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
        return nodes;
    nodes = getParentsInDFSRecursively(node);
    return nodes;
    * End Commented by Supratik: Nov 28, 2012 */
    
    set <t_DAGNode *> visitedSet;
    visitedSet.insert(node);
    
    list<t_DAGNode *> workingStack;
    t_DAGNode * workingNode;
    workingStack.push_back(node);
    while (workingStack.size() > 0) {
        workingNode = workingStack.back();
        workingStack.pop_back();
        for (list<t_DAGNode*>::iterator it = workingNode->m_in_list.begin(); it != workingNode->m_in_list.end(); it++)
        {
            if (visitedSet.find(*it) == visitedSet.end()) {
                visitedSet.insert((*it));
                workingStack.push_back((*it));
            }
        }              
    }
    return visitedSet;
}

/* Begin Commented by Supratik: Nov 29, 2012
vector<t_DAGNode *> t_DAGManager::getParentsInDFSRecursively(t_DAGNode * node)
{
    vector<t_DAGNode *> nodes;
    if (node->m_visited == true)
        return nodes;
    node->m_visited = true;

    nodes.push_back(node);
    for (list<t_DAGNode*>::iterator it = node->m_in_list.begin(); it != node->m_in_list.end(); it++)
    {
        vector<t_DAGNode *> child_nodes = getParentsInDFSRecursively(*it);
        for (int i = 0; i < child_nodes.size(); i++)
            nodes.push_back(child_nodes[i]);
    }
    return nodes;
}
* End Commented by Supratik: Nov 29, 2012*/

set<t_DAGNode *> t_DAGManager::getSetOfDescendentNodesByBFS(t_DAGNode *node)
{
    /* Begin Commented by Supratik: Nov 29, 2012
    list<t_DAGNode *> bfs_queue;
    vector<t_DAGNode *> bfs_nodes;

    bfs_queue.clear();
    if (node == NULL)
        return bfs_nodes;
    bfs_queue.push_back(node);
    bfs_nodes.push_back(node);
    bool reset_visited = ResetVisitedFlagOfDAG(node);
    if (reset_visited == false)
        return bfs_nodes;

    t_DAGNode *current_node;
    do
    {
        current_node = bfs_queue.front();
        bfs_queue.pop_front();
        if (current_node->m_visited == true)
        {
            continue;
        }
        current_node->m_visited = true;

        for (list<t_DAGNode*>::iterator it = current_node->m_out_list.begin(); it != current_node->m_out_list.end(); it++)
        {
            if ((*it)->m_visited == false)
            {
                bfs_queue.push_back(*it);
                bfs_nodes.push_back(*it);
            }
        }

    }
    while (bfs_queue.size() > 0);


    return bfs_nodes;
     * End Commented by Supratik */
    
    set <t_DAGNode *> visitedSet;
    visitedSet.insert(node);
    
    list<t_DAGNode *> workingQueue;
    t_DAGNode * workingNode;
    workingQueue.push_back(node);
    while (workingQueue.size() > 0) {
        workingNode = workingQueue.front();
        workingQueue.pop_front();
        for (list<t_DAGNode*>::iterator it = workingNode->m_out_list.begin(); it != workingNode->m_out_list.end(); it++)
        {
            if (visitedSet.find(*it) == visitedSet.end()) {
                visitedSet.insert((*it));
                workingQueue.push_back((*it));
            }
        }              
    }
    return visitedSet;
    
}

set<t_DAGNode *> t_DAGManager::getSetOfAncestorNodesByBFS(t_DAGNode *node)
{
    /*  Begin Commented by Supratik: Nov 29, 2012
    list<t_DAGNode *> bfs_queue;
    vector<t_DAGNode *> bfs_nodes;

    bfs_queue.clear();
    if (node == NULL)
        return bfs_nodes;
    bfs_queue.push_back(node);
    bfs_nodes.push_back(node);
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    ;
    if (reset_visited == false)
        return bfs_nodes;


    t_DAGNode *current_node;
    do
    {
        current_node = bfs_queue.front();
        bfs_queue.pop_front();
        if (current_node->m_visited == true)
        {
            bfs_queue.pop_front();
            continue;
        }
        current_node->m_visited = true;

        for (list<t_DAGNode*>::iterator it = current_node->m_in_list.begin(); it != current_node->m_in_list.end(); it++)
        {

            bfs_queue.push_back(*it);
            bfs_nodes.push_back(*it);
        }

    }
    while (bfs_queue.size() > 0);
    return bfs_nodes;
     * End Commented by Supratik: Nov 29, 2012 */
    
    set <t_DAGNode *> visitedSet;
    visitedSet.insert(node);
    
    list<t_DAGNode *> workingQueue;
    t_DAGNode * workingNode;
    workingQueue.push_back(node);
    while (workingQueue.size() > 0) {
        workingNode = workingQueue.front();
        workingQueue.pop_front();
        for (list<t_DAGNode*>::iterator it = workingNode->m_in_list.begin(); it != workingNode->m_in_list.end(); it++)
        {
            if (visitedSet.find(*it) == visitedSet.end()) {
                visitedSet.insert((*it));
                workingQueue.push_back((*it));
            }
        }              
    }
    return visitedSet;
    
}

/**
 * Synopsis: Public function that tells if the given DAG is a Tree
 *
 * Description: This function performs a simple test, it counts the number of shared nodes in the DAG. If they are 0, then the DAG is indeed a tree...
 *
 * Arguments: Non-null DAGNode
 *
 * Return value: True, if it succeeds, false otherwise
 */


bool t_DAGManager::isDAGATree(t_DAGNode *node)
{
    if (node == NULL)
        return false;

    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
        return false;


    int count_of_shared_nodes = markSharedNodes(node);
    //m_logManager->LOG("Value of count is "<<count<<"\n";
    if (count_of_shared_nodes == 0)
        return true;
    return false;
}

/**
 * Synopsis: Test if the node_to_test is ancestor of ancestor_of
 *
 * Description: this function obtains list of all ancestor nodes of ancestor_of and searches it if it finds the node_to_test in it
 *
 * Return value: true if it succeeds, false otherwise
 *
 */


bool t_DAGManager::isAncestor(t_DAGNode *node_to_test, t_DAGNode *ancestor_of)
{
    if ((node_to_test == NULL) || (ancestor_of == NULL))
    {
        return false;
    }

    set<t_DAGNode *> all_parents = getSetOfAncestorNodesByDFS(ancestor_of);
    if (all_parents.find(node_to_test) != all_parents.end()) {
        return true;
    }
    
    /* Begin Commented by Supratik: Nov 29, 2012
    for (int i = 0; i < all_parents.size(); i++)
    {
        if (all_parents[i] == node_to_test)
            return true;
    }
     * End Commented by Supratik: Nov 29, 2012 */
    
    return false;
}

/**
 * Synopsis: Test if the node_to_test is descendent of descendent_of
 *
 * Description: this function obtains list of all descendent nodes of ancestor_of and searches it if it finds the node_to_test in it. This implementation uses DFS order, but it is really immaterial.
 *
 * Return value: true if it succeeds, false otherwise
 *
 */


bool t_DAGManager::isDescendent(t_DAGNode *node_to_test, t_DAGNode *descendent_of)
{

    if ((node_to_test == NULL) || (descendent_of == NULL))
    {
        return false;
    }

    set<t_DAGNode *> all_descendants = getSetOfDescendentNodesByDFS(descendent_of);
    if (all_descendants.find(node_to_test) != all_descendants.end()) {
        return true;
    }
    
    /* Begin Commented by Supratik: Nov 29, 2012
    for (int i = 0; i < all_children.size(); i++)
    {
        if (all_children[i] == node_to_test)
            return true;
    }
     * End Commented by Supratik: Nov 29, 2012 */
    
    return false;

}

/**
 * Synopsis: Get the ith child of a node
 *
 * Description: this function returns handle to ith child of a node. 
 *
 * Return value: NULL if it fails, DAGNode * to the ith child if it succeeds
 *
 */


t_DAGNode * t_DAGManager::getIThChildOfNode(t_DAGNode *node, int i)
{
    if (i < 0)
        return NULL;
    if (node->m_out_list.size() == 0)
        return NULL;
    int counter = 0;
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        if (counter == i)
            return (*it);
        counter++;
    }
    return NULL;
}

/**
 * Synopsis: Obtain handle to a node with given label
 *
 * Description: This function returns a handle to a DAGNode with given label. For restricted mode, it searches for the leaf nodes. For user defined mode, it may return a non-leaf node.
 *
 *Return value: A NULL if it cannot search the node with given specs, DAGNode * to node if it finds one
 */

t_DAGNode * t_DAGManager::getNodeWithLabel(const string& label)
{


    if (label == "")
        return NULL;


    string signature = "";
    if (!m_restricted_mode)
    {
        m_logManager->LOG("Searching a DAGNode with label" + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        signature = label;
    }
    else
    {
        m_logManager->LOG("Searching a leaf DAGNode with label" + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        signature = createSignature(label);
    }

    t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
    if (search_signature.success())
    {
        t_DAGNode *result_node = search_signature.getValue();
        return result_node;
    }
    else
        return NULL;

}

/**
 * Synopsis: Test whether a node is parent of another
 *
 * Description: This function checks if a DAGNode node_to_test is parent of the DAGNode parent_of
 *
 * Return Value: True if the test succeeds, false otherwise
 *
 *
 */

bool t_DAGManager::isParent(t_DAGNode *node_to_test, t_DAGNode *parent_of)
{
    if ((node_to_test == NULL) || (parent_of == NULL))
    {
        return false;
    }
    for (list<t_DAGNode*>::iterator it = node_to_test->m_out_list.begin(); it != node_to_test->m_out_list.end(); it++)
    {
        if (parent_of == (*it))
            return true;
    }
    return false;
}

/**
 * Synopsis: Test whether a node is child of another
 *
 * Description: This function checks if a DAGNode node_to_test is child of the DAGNode child_of
 *
 * Return Value: True if the test succeeds, false otherwise
 *
 *
 */

bool t_DAGManager::isChild(t_DAGNode *node_to_test, t_DAGNode *child_of)
{
    if ((node_to_test == NULL) || (child_of == NULL))
    {
        return false;
    }

    for (list<t_DAGNode*>::iterator it = child_of->m_out_list.begin(); it != child_of->m_out_list.end(); it++)
    {
        if (node_to_test == (*it))
            return true;
    }

    return false;
}

/**
 * Synopsis: Determine the memory consumed by the DAG
 *
 * Description: This function computes the memory consumed by the DAG(s)
 *
 * Return Value: An unsigned long integer representing number of bytes of memory used..
 *
 *
 */

unsigned long int t_DAGManager::getMemoryUsage()
{
    return m_memory_usage;

}

/**
 * Synopsis : Delete a node from hashtable
 *
 * Description: This function deletes a node from the hashtable
 *
 *
 *
 */




bool t_DAGManager::deleteANodeFromHashTable(t_DAGNode *node_to_delete)
{
    string signature = node_to_delete->m_signature;
    t_HTStatusValue<t_DAGNode *> delete_result = ht.hashtable_delete(signature);
    if (delete_result.success())
    {
        m_logManager->LOG("Node deleted from hashtable successfully\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }
    else
    {
        m_logManager->LOG("Node NOT deleted from hashtable \n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    return true;
}

/**
 * Synopsis: Get the list of Root nodes
 *
 * Description: The DAGManager maintains a bunch of DAGs and not just a single DAG. It internally keeps track of all the roots of the DAGs from which the entire DAG is reachable. (These roots may have been returned by the DAGManager when corresponding DAGs were created...)
 * This function simply returns the list of root nodes in a vector maintained by it..
 *
 *
 * Return value: The set of roots maintained by the DAGManager in a STL vector
 *
 */
vector<t_DAGNode*> t_DAGManager::getVectorOfRootNodes()
{
    vector<t_DAGNode *> roots;
    t_DAGNode *current_node = m_head_roots.m_next_root;

    while (current_node != NULL)
    {
        roots.push_back(current_node);
        current_node = current_node->m_next_root;
    }
    return roots;
}

/**
 * Synopsis: Add child at position i to parent
 *
 * Description: This function adds a "child" child to "parent" parent at position i
 *
 * Return value: true if it succeeds , false otherwise
 *
 * Postcondition: shared flag of nodes may be incorrect after a call to this function. Use markSharedNodes function to obtain correct values for it.
 */

bool t_DAGManager::addNodeAsIthChild(t_DAGNode *parent, t_DAGNode *child, int i)
{

    if ((parent == NULL) || (child == NULL))
    {
        return false;
    }
    if (i < 0)
    {
        return false;
    }
    if (i > parent->m_out_list.size() + 1)
    {
        return false;
    }



    if (m_restricted_mode)
    {
        int counter = 0;
        list<t_DAGNode*>::iterator it;
        for (it = parent->m_out_list.begin(); it != parent->m_out_list.end(); it++)
        {
            if (counter == i)
                break;
            counter++;
        }
        parent->m_out_list.insert(it, child);

	#ifdef QE //#ifdef added by Ajith John on 9 June 2013
		/* You cannot just say ++ on reference_count;
        	reference_count should be incremented using incrementReferenceCount function */ 
		incrementReferenceCount(child);
	#else
        	child->m_reference_count++;
	#endif

        if (child->m_isRoot)
        {
            bool deleted_from_roots = deleteNodeFromRootsList(child);
            if (deleted_from_roots == false)
            {
                return false;
            }

        }
        //Signature of the parent node changed, Recompute it
        bool correct_sign = RecomputeSignatureOfANode(parent);
        if (!correct_sign)
            return false;
        return true;
    }
    else //!restricted mode
    {

        int counter = 0;
        list<t_DAGNode*>::iterator it;
        for (it = parent->m_out_list.begin(); it != parent->m_out_list.end(); it++)
        {
            if (counter == i)
                break;
            counter++;
        }
        parent->m_out_list.insert(it, child);

	#ifdef QE //#ifdef added by Ajith John on 9 June 2013
		/* You cannot just say ++ on reference_count;
        	reference_count should be incremented using incrementReferenceCount function */ 
		incrementReferenceCount(child);
	#else
        	child->m_reference_count++;
	#endif
        
        if (child->m_isRoot)
        {
            bool deleted_from_roots = deleteNodeFromRootsList(child);
            if (deleted_from_roots == false)
            {
                return false;
            }

        }
        //No signatures change!!
    }
}

bool t_DAGManager::RecomputeSignatureOfANode(t_DAGNode *node)
{
    int rec_cntr_recomp_fn = 0;
    int rec_cntr_replace_function = 0;

    return RecomputeSignatureOfANodeInternal(node, true, rec_cntr_recomp_fn, rec_cntr_replace_function);
}

/**
 * Synopsis: Re-Compute the signature of a node
 *
 * Description: If the signature of a node changes, it has to be recomputed. Note that after recomputing the signature, we retain the address of the node whose signature is being recomputed, and therefore signature of its parents does not change!!
 * This function is restricted mode function, and does not work in user-defined labels mode
 *
 * Return Value: True if it succeeds, false if it fails, or when called in user-defined labels mode
 */




bool t_DAGManager::RecomputeSignatureOfANodeInternal(t_DAGNode *node, bool enable_sanity_check, int rec_cntr_recomp_fn, int rec_cntr_replace_function)
{
    m_logManager->LOG("Recomputing signature of a node \n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
        m_logManager->LOG("Node is Null\n", m_dagmanager_log, c_RunLevelVerbosity);
        throw std::runtime_error("Cannot compute signature of NULL");
        //return false;
    }
    if (!m_restricted_mode)
    {
        m_logManager->LOG("ERROR!! Attempt to change signature of a node is user-defined labels mode\n", m_dagmanager_log, c_RunLevelVerbosity);
        throw std::logic_error("Attempt to change the signature in user-defined label mode");
        //return false;
    }
/*
    //Delete the hashtable entry using the old signature
    string current_signature = node->m_signature;
    t_HTStatusValue<t_DAGNode *> delete_result = ht.hashtable_delete(current_signature);
    if (delete_result.success())
    {
        m_logManager->LOG("Node deleted from hashtable successfully  " + node->m_label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
    }
    else
    {
        m_logManager->LOG("ERROR!! Node NOT deleted from hashtable  " + node->m_label + "\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
*/
    vector<t_DAGNode *> v_operands;
    m_logManager->LOG("Creating v_operands\n", m_dagmanager_log, c_DebugLevelVerbosity);
    //create v_operands considering the newly added child
    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        v_operands.push_back(*it);
    }

    if (v_operands.size() == 0)
        m_logManager->LOG("WARNING!! Changing signature of a leaf node\n", m_dagmanager_log, c_RunLevelVerbosity);

    m_logManager->LOG("Creating signature for new parent " + node->m_label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);

    string new_signature;
    new_signature = createSignature(node->m_label, v_operands);


    t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(new_signature);
    if (search_signature.success())
    {
        m_logManager->LOG("Found the node in the hashtable\n", m_dagmanager_log, c_DebugLevelVerbosity);
        //Replace the node by the node whose signature is equal to this node's new signature
        t_DAGNode *replaceBy = search_signature.getValue();
        //Modify every parent of node to point to replace_by instead of node
        rec_cntr_replace_function += 1;
        //if(m_recursiveReplaceFlag == true)
        {
            m_logManager->LOG("RECURSION COUNTER FOR ReplaceDAGByDAGInternal:= " + integerToString(rec_cntr_replace_function) + "\n", m_dagmanager_log, c_RunLevelVerbosity);
            bool repl_result = ReplaceDAGByDAGInternal(node, replaceBy, enable_sanity_check, rec_cntr_recomp_fn, rec_cntr_replace_function);

            if (!repl_result)
            {
                m_logManager->LOG("ERROR!! Failed ReplaceDAGByDAG in RecomputeSignatureOfANode \n", m_dagmanager_log, c_RunLevelVerbosity);
                throw std::runtime_error("FAILED ReplaceDAGByDAGInternal recursion counter:= " + integerToString(rec_cntr_replace_function));
                //return false;
            }
        }
        return true;
    }
    else
    {
        m_logManager->LOG("Did not find the node in the hashtable\n", m_dagmanager_log, c_RunLevelVerbosity);
        t_HTStatusValue <t_DAGNode *> insert_result = ht.hashtable_insert(new_signature, node);
        if (insert_result.success())
        {
            m_logManager->LOG("Successfully inserted node in the hashtable\n", m_dagmanager_log, c_DebugLevelVerbosity);
            node->m_signature = new_signature;
            //only Leaf can change, rest all flags will retain their values
            if (v_operands.size() == 0)
                node->m_isLeaf = true;
            return true;
        }
        else
        {
            m_logManager->LOG("ERROR!! Failed inserting the node in the hashtable\n", m_dagmanager_log, c_RunLevelVerbosity);
            throw std::runtime_error("Cannot insert "+new_signature+" in hashtable");
            //return false;
        }
    }

    m_logManager->LOG("Successfully recomputed the signature of the node\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 *Synopsis: Logging function
 *
 * Description: The purpose of this function is to log a message to log file.
 * This has been extracted out as a seperate function just in order to allow enabling/disabling of logging process.
 * Since the logging is a file operation, it is estimated to take fair amount of time. Therefore it is customary to disable it, if one is sure about the DAG operations correctness
 * Fatal messages will still be thrown to stderr/cerr
 */

void t_DAGManager::LOG(string message)
{
    if (m_enable_logging)
        *m_dagmanager_log << message;
}

/**
 * Synopsis: Obtain handle to a node with given label
 *
 * Description: This function returns a handle to a DAGNode with given label. For restricted mode, it searches for the leaf nodes. For user defined mode, it may return a non-leaf node.
 *
 *Return value: A NULL if it cannot search the node with given specs, DAGNode * to node if it finds one
 */
template<typename Alloc>
t_DAGNode * t_DAGManager::testAndReturnDAGNodeIfItExists(const string& label, const vector<t_DAGNode *, Alloc>& v_children)
{


    if (label == "")
        return NULL;


    string signature = "";
    if (!m_restricted_mode)
    {
        m_logManager->LOG("Searching a DAGNode with label" + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        signature = label;
    }
    else
    {
        m_logManager->LOG("Searching a leaf DAGNode with label" + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        signature = createSignature(label, v_children);
    }

    t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
    if (search_signature.success())
    {
        t_DAGNode *result_node = search_signature.getValue();
        return result_node;
    }
    else
        return NULL;

}

template t_DAGNode * t_DAGManager::testAndReturnDAGNodeIfItExists(const string& label, const vector<t_DAGNode *, std::allocator<t_DAGNode*> >& v_children);
template t_DAGNode * t_DAGManager::testAndReturnDAGNodeIfItExists(const string& label, const vector<t_DAGNode *, MyAllocator<t_DAGNode*> >& v_children);
/**
 * Synopsis: Obtain handle to a node with given label
 *
 * Description: This function returns a handle to a DAGNode with given label. For restricted mode, it searches for the leaf nodes. For user defined mode, it may return a non-leaf node.
 *
 *Return value: A NULL if it cannot search the node with given specs, DAGNode * to node if it finds one
 */

t_DAGNode * t_DAGManager::testAndReturnDAGNodeIfItExists(const string& label)
{


    if (label == "")
        return NULL;


    string signature = "";
    if (!m_restricted_mode)
    {
        m_logManager->LOG("Searching a DAGNode with label" + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        signature = label;
    }
    else
    {
        //m_logManager->LOG("Searching a leaf DAGNode with label" + label + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
        signature = createSignature(label);
    }

    t_HTStatusValue <t_DAGNode *> search_signature = ht.hashtable_search(signature);
    if (search_signature.success())
    {
        t_DAGNode *result_node = search_signature.getValue();
        return result_node;
    }
    else
        return NULL;

}

/**
 * Synopsis: print DAG in graphviz DOTTY format
 *
 * Description: This function prints the DAG in DOTTY format to a file. The output file can be visually observered using dotty
 *
 * Return value: true if it succeeds , false otherwise
 *
 *
 */

bool t_DAGManager::printDAGAsDOTFileWithAbstractSignatures(t_DAGNode *node, ofstream *outfile)
{
    /*-------
      The nodes in the DAG that are shared are indeed marked as being shared while they are created.
      Unless there is a deleteDAG operation executed on the DAG, a node that is shared has no opportunity for becoming unshared???
      -------*/
    m_logManager->LOG("Printing DAG as DOT\n", m_dagmanager_log, c_DebugLevelVerbosity);
    *outfile << "digraph G{" << endl;

    m_logManager->LOG("Marking shared nodes \n", m_dagmanager_log, c_DebugLevelVerbosity);
    int mark_shared_nodes_count = markSharedNodes(node);
    if (mark_shared_nodes_count == -1)
    {
        m_logManager->LOG("Found shared_node_count = -1 in printDAGAsDOTFile\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return false;
    }

    m_logManager->LOG("Resetting visited flag\n", m_dagmanager_log, c_DebugLevelVerbosity);
    bool reset_visited = ResetVisitedFlagOfDAG(node);
    if (reset_visited == false)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAG\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    m_logManager->LOG("Printing the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    bool printed_dot = printDAGAsDOTFileRecursivelyWithAbstractSignatures(node, outfile);
    if (printed_dot == false)
    {
        m_logManager->LOG("ERROR!! Failed printing DAGAsDot recursively\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    *outfile << "}" << endl;
    return true;
}

bool t_DAGManager::printDAGAsDOTFileRecursivelyWithAbstractSignatures(t_DAGNode *node, ofstream *outfile)
{

    if (node == NULL)
    {
        return false;
    }
    if (node->m_visited == true)
        return true;
    node->m_visited = true;

    *outfile << "n" << node << "[label=\"" << node->getLabel() << "\\n";
    for (set<string> ::iterator it = node->m_abstract_signatures.begin(); it != node->m_abstract_signatures.end(); it++)
        *outfile << *it << "\\n";
    *outfile << "\"];" << endl;
    int num_operands = node->m_out_list.size();

    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        *outfile << "n" << node << "->" << "n" << *it << endl;
    }


    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        bool printed_operands = printDAGAsDOTFileRecursivelyWithAbstractSignatures(*it, outfile);
        if (printed_operands == false)
        {
            m_logManager->LOG("ERROR!! Failed call to printDAGAsDOTFileRecursively\n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }
    return true;
}

bool t_DAGManager::deleteANodeFromInListOfItsChildren(t_DAGNode *node)
{
    m_logManager->LOG("Deleting outgoing edges on a node\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
        m_logManager->LOG("Node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (node->m_out_list.size() == 0)
    {
        return true;
    }
    else
    {
        for (list<t_DAGNode*>::iterator outlist_it = node->m_out_list.begin(); outlist_it != node->m_out_list.end(); outlist_it++)
        {
            t_DAGNode *child_i = *outlist_it;
            for (list<t_DAGNode*>::iterator it = child_i->m_in_list.begin(); it != child_i->m_in_list.end(); it++)
            {
                if ((*it) == node)
                {
                    child_i->m_in_list.erase(it);

		    #ifndef QE //#ifndef added by Ajith John on 9 June 2013
                    	child_i->m_reference_count -= 1;
		    #endif

                    break;
                }
            }
        }
        node->m_out_list.clear();
        RecomputeSignatureOfANode(node);
        *m_dagmanager_log << "Successfully deleted outgoing edges of a node\n";
        return true;
    }

}

bool t_DAGManager::deleteNodeFromNodesList(t_DAGNode *node_to_delete)
{
    m_logManager->LOG("Deleting a node from nodess list\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node_to_delete->m_next_node == NULL)
    {
	//if--elseif conditions added by Ajith John on 9 June 2013
	if (node_to_delete != m_tail_nodes)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteNodeFromNodesList!! trying to delete the last node in nodes_list, but m_tail_nodes is not pointing to the last node\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;			
	}
	else if (node_to_delete->m_prev_node == NULL)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteNodeFromNodesList!! attempt to delete head_nodes\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;			
	}
	else
	{
		m_tail_nodes = node_to_delete->m_prev_node; //line added by Ajith John on 9 June 2013
        	node_to_delete->m_prev_node->m_next_node = NULL;
        	return true;
	}

    }
    else if (node_to_delete->m_prev_node == NULL)
    {
        return false;
        m_logManager->LOG("ERROR!! Failed to hit head_nodes when deleting a node from nodes list\n", m_dagmanager_log, c_RunLevelVerbosity);
        //cannot happen
    }
    else
    {
        node_to_delete->m_prev_node->m_next_node = node_to_delete->m_next_node;
        node_to_delete->m_next_node->m_prev_node = node_to_delete->m_prev_node;
        return true;
    }
    m_logManager->LOG("Successfully a node deleted from nodes list\n", m_dagmanager_log, c_DebugLevelVerbosity);
}

bool t_DAGManager::deleteDAG(t_DAGNode *node_to_delete)
{
    #ifdef QE //#ifdef added by Ajith John on 9 June 2013
	// This function should not be used for deletion of dags
	m_logManager->LOG("ERROR!! t_DAGManager::deleteDAG called\n", m_dagmanager_log, c_RunLevelVerbosity);
   	return false;
    #else
    	//if a root node is to be deleted, simply eliminate it from the list of roots.......
    	if (node_to_delete->m_isRoot)
    	{
        	bool remove_from_roots_result = deleteNodeFromRootsList(node_to_delete);
        	if (!remove_from_roots_result)
        	    return false;
        	node_to_delete->m_reference_count = 0;
        	return true;
    	}
    	else
        	node_to_delete->m_reference_count = 0;
    	return 0;
   #endif
}

vector<t_DAGNode *> t_DAGManager::getVectorOfAllLeafNodesInDAGManager()
{
    t_DAGNode *current_node = m_head_nodes.m_next_node;
    int counter = 0;
    m_logManager->LOG("Preparing the vector of all leaf nodes in the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    vector<t_DAGNode *> leaf_nodes;
    while (current_node != NULL)
    {
        t_DAGNode *node_to_process = current_node;
        leaf_nodes.push_back(node_to_process);
        current_node = current_node->m_next_node;
        counter++;
    }
    m_logManager->LOG("Finished preparing the list of leaf nodes in the DAG \n", m_dagmanager_log, c_DebugLevelVerbosity);
    return leaf_nodes;
}

bool t_DAGManager::checkWhetherCreatingEdgeFormsCycle(t_DAGNode *nodeFrom, t_DAGNode *nodeTo)
{
    list<t_DAGNode *> bfs_queue;
    vector<t_DAGNode *> bfs_nodes;

    bfs_queue.clear();
    if ((nodeFrom == NULL) || (nodeTo == NULL))
        return false;
    bfs_queue.push_back(nodeTo);
    bfs_nodes.push_back(nodeTo);
    bool reset_visited = ResetVisitedFlagOfDAG(nodeTo);

    if (reset_visited == false)
        return false;

    t_DAGNode *current_node;
    do
    {
        current_node = bfs_queue.front();
        bfs_queue.pop_front();
        if (current_node->m_visited == true)
        {
            continue;
        }
        current_node->m_visited = true;

        for (list<t_DAGNode*>::iterator it = current_node->m_out_list.begin(); it != current_node->m_out_list.end(); it++)
        {
            if ((*it)->m_visited == false)
            {
                if ((*it) == nodeFrom)
                {
                    return false;
                }
                bfs_queue.push_back(*it);
                bfs_nodes.push_back(*it);
            }
        }

    }
    while (bfs_queue.size() > 0);

    return false;

}


#ifdef QE //#ifdef added by Ajith John on 9 June 2013
// The following functions (from insertToDeathRow to deleteDeadNode) are added by Ajith John

/**
 * Synopsis: Insert the given node into death row
* 
* Description: Can be done by changing the
 * m_next_dead and m_prev_dead pointers
 *
 * Return value: true if it succeeds , false otherwise
 *
 */
inline bool 
t_DAGManager::insertToDeathRow(t_DAGNode *node)
{
    m_logManager->LOG("Inserting a node in death row\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
	m_logManager->LOG("Error in t_DAGManager::insertToDeathRow(t_DAGNode *node)!! node is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);  
        return false;              
    }
    else if (node->m_reference_count > 0)
    {
        m_logManager->LOG("Error in t_DAGManager::insertToDeathRow(t_DAGNode *node)!! cannot add a node with positive reference count to death row \n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    else
    {
        // Inserting to death row
	m_tail_dead->m_next_dead = node;
	node->m_prev_dead = m_tail_dead;
    	node->m_next_dead = NULL;
    	m_tail_dead = node;

	m_death_row_size++;

        m_logManager->LOG("Successfully inserted a node into m_death_row\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }       
}


/**
 * Synopsis: Erase the given node from death row
 *
* Description: Can be done by changing the
 * m_next_dead and m_prev_dead pointers
 *
 * Return value: true if it succeeds , false otherwise
 *
 *
 */
inline bool 
t_DAGManager::eraseFromDeathRow(t_DAGNode *node)
{
    m_logManager->LOG("Deleting a node from death row\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (node == NULL)
    {
	m_logManager->LOG("Error in t_DAGManager::eraseFromDeathRow(t_DAGNode *node)!! node is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);  
        return false;              
    }
    else if(m_death_row_size == 0)
    {
	m_logManager->LOG("Error in t_DAGManager::eraseFromDeathRow(t_DAGNode *node)!! m_death_row_size = 0\n", m_dagmanager_log, c_RunLevelVerbosity);  
        return false;  
    }
    else if (node->m_next_dead == NULL) //node is the tail node in the death row
    {       
	if (node != m_tail_dead)
	{
		m_logManager->LOG("Error in t_DAGManager::eraseFromDeathRow!! trying to delete the last node in death row, but m_tail_dead is not pointing to the last node\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;			
	}
	else if (node->m_prev_dead == NULL)
	{
		m_logManager->LOG("Error in t_DAGManager::eraseFromDeathRow!! attempt to delete head_dead\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;			
	}
	else
	{
		m_tail_dead = node->m_prev_dead;	
        	node->m_prev_dead->m_next_dead = NULL; //detach the node from death row
		node->m_next_dead = NULL;
        	node->m_prev_dead = NULL;	

		m_death_row_size--;
	
       		m_logManager->LOG("Successfully deleted a node from death row\n", m_dagmanager_log, c_DebugLevelVerbosity);
       		return true;        
	}
    }
    else if (node->m_prev_dead == NULL)
    {
	 m_logManager->LOG("Error in t_DAGManager::eraseFromDeathRow(t_DAGNode *node)!! node is not in death row\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
     }
    else
    {
	//detach the node from death row
        node->m_prev_dead->m_next_dead = node->m_next_dead;
        node->m_next_dead->m_prev_dead = node->m_prev_dead;
	node->m_next_dead = NULL;
        node->m_prev_dead = NULL;	

	m_death_row_size--;

	m_logManager->LOG("Successfully deleted a node from death row\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }
}



/**
 * Synopsis: Check if the given node is in death row
 *
 * Description: node is in death row iff
 * m_next_dead and m_prev_dead pointers are NULL
 *
 * Return value: true if it is , false otherwise
 *
 *
 */
inline bool 
t_DAGManager::findInDeathRow(t_DAGNode *node)
{   
   if (node->m_next_dead == NULL && node->m_prev_dead == NULL) // is not in death row
   {
	return false;              
   }
   else // is in death row
   {
      	return true;
   }
}


/**
 * Synopsis: decrement reference count of the given node, clear
 * the death row and delete nodes in death row if its size exceeds the limit
 *
 * Description: 1) calls decrementReferenceCountInternal
 *		2) If death row's size exceeds the limit, then all nodes in 
 * 		death row are permanantly deleted and the death row is cleared. 
 * 
 * Return value: true if it succeeds , false otherwise
 *
 */
bool 
t_DAGManager::decrementReferenceCount(t_DAGNode *node)
{
    // 	decrement reference count of the given node
    bool ret_from_internal = decrementReferenceCountInternal(node);
 	
    if(!ret_from_internal)//error occures in decrementReferenceCountInternal; propogate the error up
    {
	m_logManager->LOG("Error in t_DAGManager::decrementReferenceCount(t_DAGNode *node)!! failure when _DAGManager::decrementReferenceCountInternal(t_DAGNode *node) is called\n", m_dagmanager_log, c_RunLevelVerbosity);  
	return false;
     }

    // Check if size of death row >= threshold. 
    // If yes, free nodes in death row and clear death row
    if(m_death_row_size >= m_threshold_death_row_size)
    {	
	m_logManager->LOG("death row size >= maximum possible death row size\n", m_dagmanager_log, c_DebugLevelVerbosity); 
	m_logManager->LOG("deleting nodes in death row and clearing the death row\n", m_dagmanager_log, c_DebugLevelVerbosity); 

	return deleteNodesInDeathRowAndClearDeathRow();	
    }			

    return true;
}




/**
 * Synopsis: decrement reference count of the given node.
 *
 * Description: Decrements m_reference_count of the given node.
 * If m_reference_count becomes zero, then the node is added
 * to death row and decrementReferenceCountInternal is called recursively on its 
 * children. 
 * 
 * Return value: true if it succeeds , false otherwise
 *
 */
bool 
t_DAGManager::decrementReferenceCountInternal(t_DAGNode *node)
{
    if (node == NULL)
    {
	m_logManager->LOG("Error in t_DAGManager::decrementReferenceCountInternal(t_DAGNode *node)!! node is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);  
        return false;              
    }
    else
    {
        node->m_reference_count--;

	if(node->m_reference_count == 0) //node->m_reference_count has become 0 from 1
	{
		insertToDeathRow(node); // insert the node into death row		
					
		/*call decrementReferenceCountInternal on node's children.	
		Note that there's no need of dynamic programming here. Let a node's m_reference_count
		becomes 0, and is pushed into death row. Its  m_reference_count will not be decremented
		further. Hence we will not call  decrementReferenceCountInternal recursively on its children. */

		for (list<t_DAGNode*>::iterator child_it = node->m_out_list.begin(); child_it != node->m_out_list.end(); child_it++)
		{
			bool ret_from_child = decrementReferenceCountInternal(*child_it);

			if(!ret_from_child)//error occures in child; propogate the error up
			{
				m_logManager->LOG("Error in t_DAGManager::decrementReferenceCountInternal(t_DAGNode *node)!! failure when called on child node\n", m_dagmanager_log, c_RunLevelVerbosity);  
				return false;
			}
		}//for each child
		
	}//if node->m_reference_count has become 0 from 1

	else if(node->m_reference_count < 0) //node->m_reference_count has become -1 from 0
	// This means that reference counting is incorrect.. may be user was using it, but ref. count was zero
	{
		m_logManager->LOG("Error in t_DAGManager::decrementReferenceCountInternal(t_DAGNode *node)!! reference count of node becomes negative\n",	m_dagmanager_log, c_RunLevelVerbosity);  
	        return false;  
	}
		
	m_logManager->LOG("Successfully decremented reference count of a node\n", m_dagmanager_log, c_DebugLevelVerbosity);


        return true;
    } //if node is not NULL  
}


/**
 * Synopsis: increment reference count of the given node.
 *
 * Description: Increments m_reference_count of the given node.
 * If m_reference_count becomes 1 and if the node is 
 * present in the death row, then the node is removed from the 
 * death row and incrementReferenceCount is called recursively on its 
 * children.  
 *
 *
 * Return value: true if it succeeds , false otherwise
 *
 *
 */
bool 
t_DAGManager::incrementReferenceCount(t_DAGNode *node)
{
    if (node == NULL)
    {
	m_logManager->LOG("Error in t_DAGManager::incrementReferenceCount(t_DAGNode *node)!! node is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);  
        return false;              
    }
    else
    {
        node->m_reference_count++;
	
	if(node->m_reference_count == 1) //node->m_reference_count has become 1 from 0
	{
		if(findInDeathRow(node)) //node is in m_death_row
		{
			eraseFromDeathRow(node); // remove the node from m_death_row
			
			/*call incrementReferenceCount on node's children.	
			Note that there's no need of dynamic programming here. Let a node's m_reference_count
	becomes 1, and is pulled off from death row. Suppose it is reached again. Now its m_reference_count
	becomes 2. Hence we will not call  incrementReferenceCount recursively on it */

			for (list<t_DAGNode*>::iterator child_it = node->m_out_list.begin(); child_it != node->m_out_list.end(); child_it++)
			{
				bool ret_from_child = incrementReferenceCount(*child_it);

				if(!ret_from_child)//error occurs in child; propogate the error up
				{
					m_logManager->LOG("Error in t_DAGManager::incrementReferenceCount(t_DAGNode *node)!! failure when called on child node\n", m_dagmanager_log, c_RunLevelVerbosity);  
					return false;
				}
			}//for each child
		}// if node is in m_death_row
	}//if node->m_reference_count has become 1 from 0

	m_logManager->LOG("Successfully incremented reference count of a node\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }// if node is not NULL    
}//function ends



/**
 * Synopsis: Get the vector of dead nodes in the parameter passed.
 *
 * Description: We just need to traverse the m_next_dead pointer.
 *
 * Return value: true always.
 *
 */
bool 
t_DAGManager::getVectorOfDeadNodes(vector<t_DAGNode*> &dead_nodes)
{
    dead_nodes.clear();
    t_DAGNode *current_node = m_head_dead.m_next_dead;

    while (current_node != NULL)
    {
        dead_nodes.push_back(current_node);
        current_node = current_node->m_next_dead;
    }

    return true;
}


/**
 * Synopsis: Deletes the nodes in the death row permanantly and clears the 
 * death row
 *
 * Return value: true if it succeeds , false otherwise
 *
 */
bool 
t_DAGManager::deleteNodesInDeathRowAndClearDeathRow()
{
	//get the vector of dead nodes first
	vector<t_DAGNode*> dead_nodes;
	getVectorOfDeadNodes(dead_nodes);

	//take each dead node and delete it
	for(vector<t_DAGNode*>::iterator it = dead_nodes.begin(); it != dead_nodes.end(); it++)
	{
		t_DAGNode* dead_node = *it;
		bool ret_value;		
		
		//delete dead_node
		ret_value = deleteDeadNode(dead_node);
		if(!ret_value)
		{
			m_logManager->LOG("Error in t_DAGManager::deleteNodesInDeathRowAndClearDeathRow!! failure from t_DAGManager::deleteDeadNode\n", m_dagmanager_log, c_RunLevelVerbosity);  
			return false;	
		}		
	}
	
	return true;
	
}




/**
 * Synopsis: Suppose we are deleting a node. This function removes the 
 * node from the inlists of its children, and updates the rootslist

 * Description: Find the children of the node. Then remove the node from the 
 * inlists of its children. Now delete the node from rootslist, if 
 * it exists in rootslist. Now for each child, if inlist.size()==0, 
 * then insert the child into rootslist.  
 */
bool 
t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren(t_DAGNode *node_to_delete)
{
    bool ret_value;    

    if(node_to_delete == NULL)
    {
	m_logManager->LOG("Error in t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren!!! argument is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);
	return false;
    }

    //find children of node_to_delete in "children"
    set<t_DAGNode*> children;
    for (list<t_DAGNode *>::iterator it = node_to_delete->m_out_list.begin(); it != node_to_delete->m_out_list.end(); it++)
    {
            t_DAGNode *current_child = *it;
	    children.insert(current_child);
    }

    //delete node_to_delete from the inlists of its children
    ret_value = deleteANodeFromInListOfItsChildren(node_to_delete);
    if(!ret_value)
    {
	m_logManager->LOG("Error in t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren!! failure from t_DAGManager::deleteANodeFromInListOfItsChildren\n", m_dagmanager_log, c_RunLevelVerbosity);  
	return false;	
    }

    //if node_to_delete is a root, then remove node_to_delete from rootslist
    if (node_to_delete-> m_isRoot)
    {
    	if (node_to_delete->m_next_root == NULL)//node_to_delete is the last root node, i.e. m_tail_roots is pointing to it
    	{

		if (node_to_delete != m_tail_roots)
		{
			m_logManager->LOG("Error in t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren!! trying to delete the last node in roots_list, but m_tail_roots is not pointing to the last node\n", m_dagmanager_log, c_RunLevelVerbosity);  
			return false;			
		}
		else if (node_to_delete->m_prev_root == NULL)
		{
			m_logManager->LOG("Error in t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren!! attempt to delete head_roots\n", m_dagmanager_log, c_RunLevelVerbosity);  
			return false;			
		}
		else
		{
        		node_to_delete->m_isRoot = false;
		
			m_tail_roots = 	node_to_delete->m_prev_root;	
        		node_to_delete->m_prev_root->m_next_root = NULL;
			node_to_delete->m_next_root = NULL;
        		node_to_delete->m_prev_root = NULL;
		}
	}
	else if (node_to_delete->m_prev_root == NULL)
    	{
		m_logManager->LOG("Error in t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren!! Failed to hit head_roots when deleting a node from roots list\n", m_dagmanager_log, c_RunLevelVerbosity);
		return false;
	}
	else
    	{
        	node_to_delete->m_isRoot = false;

        	node_to_delete->m_prev_root->m_next_root = node_to_delete->m_next_root;
        	node_to_delete->m_next_root->m_prev_root = node_to_delete->m_prev_root;
		node_to_delete->m_next_root = NULL;
        	node_to_delete->m_prev_root = NULL;
	}
    }

       
    //insert each child into rootslist, if inlist's size is zero
    for(set<t_DAGNode *>::iterator it = children.begin(); it != children.end(); it++)
        {
            t_DAGNode *current_child = *it;
	    
            if (current_child->m_in_list.size() == 0)
		{
			insertNodeInRootsList(current_child);
		}
        }

     return true;
}


/**
 * Synopsis: Update m_memory_usage when a node is deleted
 *
 * Description: This function just sums up the number of bytes of information contained in a DAGNode, and subtracts it from m_memory_usage
 */

inline void t_DAGManager::reduceBytesUsedByANodeInMemoryUsage(t_DAGNode *node)
{
    m_memory_usage -= sizeof (t_DAGNode);
    m_memory_usage -= node->m_label.size();
    m_memory_usage -= node->m_out_list.size() * sizeof (t_DAGNode *);
    m_memory_usage -= node->m_signature.size();

}



/**
 * Synopsis: Deletes a dead node  
 * 
 *
 * Return value: true if it succeeds , false otherwise
 *
 */

bool 
t_DAGManager::deleteDeadNode(t_DAGNode* dead_node)
{
	bool ret_value;

	// re-compute m_dagnode_count and m_memory_usage
	m_dagnode_count--; 

	// TODO: It looks like the way in which m_memory_usage is maintained is not correct; 
	// reducing m_memory_usage by reduceBytesUsedByANodeInMemoryUsage(dead_node) causes underflow

	//delete dead_node from death row
	ret_value = eraseFromDeathRow(dead_node);
	if(!ret_value)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteDeadNode!! failure from t_DAGManager::eraseFromDeathRow\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;	
	}

	//delete dead_node from nodes list
	ret_value = deleteNodeFromNodesList(dead_node);		
	if(!ret_value)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteDeadNode!! failure from t_DAGManager::deleteNodeFromNodesList\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;	
	}

	// delete dead_node from roots list (this may require putting the children of the dead node into roots list), and
	// delete the dead node from the inlists of its children
	ret_value = deleteANodeFromRootsListAndInListOfItsChildren(dead_node);
	if(!ret_value)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteDeadNode!! failure from t_DAGManager::deleteANodeFromRootsListAndInListOfItsChildren\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;	
	}   
	
		
	// delete the dead_node from the outlists of its parents
	ret_value = deleteIncidentEdges(dead_node);
	if(!ret_value)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteDeadNode!! failure from t_DAGManager::deleteIncidentEdges\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;	
	}
	

	// delete the dead node from hash table
	ret_value = deleteANodeFromHashTable(dead_node);
	if(!ret_value)
	{
		m_logManager->LOG("Error in t_DAGManager::deleteDeadNode!! failure from t_DAGManager::deleteANodeFromHashTable\n", m_dagmanager_log, c_RunLevelVerbosity);  
		return false;	
	}

	//free the memory for dead_node
	delete(dead_node);

	return true;	
}
#endif //#endif added by Ajith John on 9 June 2013

