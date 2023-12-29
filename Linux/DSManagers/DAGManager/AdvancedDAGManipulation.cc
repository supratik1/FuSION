#include "DagManager.h"
#include "helping_functions.h"
#include <map>
#include<iostream>
#include<algorithm>
#include<iterator>
using namespace std;


/**
 * Synopsis: Public function to copy a DAG
 *
 * Description: This function allows to copy a DAG into new DAG. Note that the way new and old DAG differentiate, is based on difference in their labels. Hence it uses a "prefix", with which it modifies the lables of the original DAG. The prefix is completely internal and not visible to client otherwise.
 *
 * Arguments: Non-null DAGNode
 *
 * Return value: Handle to copied DAG's root, if suceess
 *               NULL otherwise
 *
 * See Also : copy
 *
 */


static map<unsigned long int, unsigned long int> copy_map;

t_DAGNode * t_DAGManager::copyDAG(t_DAGNode *node)
{
    m_logManager->LOG("Copying the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    m_copy_prefix_counter++;
    string copy_prefix = integerToString(m_copy_prefix_counter);

    if (node == NULL)
    {
        m_logManager->LOG("Node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return NULL;
    }
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAG\n", m_dagmanager_log, c_RunLevelVerbosity);
        return NULL;
    }
    copy_map.clear();
    t_DAGNode * copied_node = copy(node, copy_prefix);
    copy_map.clear();
    m_logManager->LOG("Successfully copied the DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return copied_node;
}

/**
 * Synopsis: PRIVATE function used during copying of DAG
 *
 * Description: This function actually performs copy of DAG by recursively traversing the original DAG.
 *
 * Arguments:
 *
 * Return value: Handle to copied DAG's root, if suceess
 *               NULL otherwise

 * See Also: copyDAG
 *
 */

t_DAGNode * t_DAGManager::copy(
                               t_DAGNode *node, //(sub)DAG to copy
                               string copy_prefix //copy prefix
                               )
{
    if (node->m_visited == true)
    {
        return (t_DAGNode *) copy_map[(unsigned long int) node];

    }
    node->m_visited = true;
    vector<t_DAGNode *> operands;

    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        t_DAGNode *child = *it;
        operands.push_back(copy(*it, copy_prefix));
    }

    t_DAGNode *copied_node;
    if (operands.size() == 0)
    {
        copied_node = createNode(copy_prefix + node->m_label);
        copied_node->m_copy_count = m_copy_prefix_counter;
        copied_node->m_value = node->m_value; //shallow copy
    }
    else
    {
        copied_node = createNode(copy_prefix + node->m_label, operands);
        copied_node->m_copy_count = m_copy_prefix_counter;
        copied_node->m_value = node->m_value; //shallow copy
    }
    copy_map.insert(make_pair((unsigned long int) (node), (unsigned long int) (copied_node)));
    return copied_node;

}

/**
 * Synopsis: Public function to copy a DAG with values
 *
 * Description: This function allows to copy a DAG into new DAG including the values...This function requires a function pointer that defines how should the value be copied..Note that the way new and old DAG differentiate, is based on difference in their labels. Hence it uses a "prefix", with which it modifies the lables of the original DAG. The prefix is completely internal and not visible to client otherwise.
 *
 * Arguments: Non-null DAGNode
 *
 * Return value: Handle to copied DAG's root, if suceess
 *               NULL otherwise
 */

t_DAGNode * t_DAGManager::copyDAGWithValues(t_DAGNode *node, void * (*copyfunc)(void *value_to_copy))
{
    m_logManager->LOG("Copying the DAG with values\n", m_dagmanager_log, c_DebugLevelVerbosity);
    string copy_prefix = integerToString(m_copy_prefix_counter + 1);
    if (node == NULL)
    {
        m_logManager->LOG("Root node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return NULL;
    }
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAGNodes \n", m_dagmanager_log, c_RunLevelVerbosity);
        return NULL;
    }
    copy_map.clear();
    t_DAGNode *copied_root = copyWithValues(node, copy_prefix, copyfunc);
    copy_map.clear();
    m_logManager->LOG("Successfully copied DAG with values\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return copied_root;
}

/**
 * Synopsis: PRIVATE function used during copying of DAG
 *
 * Description: This function actually performs copy of DAG by recursively traversing the original DAG.
 *
 * Arguments:
 *
 * Return value: Handle to copied DAG's root, if suceess
 *               NULL otherwise
 */

t_DAGNode * t_DAGManager::copyWithValues(
                                         t_DAGNode *node, //(sub)DAG to copy
                                         string copy_prefix, //copy prefix
                                         void * (*copyfunc)(void *value_to_copy)
                                         )
{
    if (node->m_visited == true)
    {
        return (t_DAGNode *) copy_map[(unsigned long int) node];
    }
    node->m_visited = true;
    vector<t_DAGNode *> operands;

    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        t_DAGNode *child = *it;
        operands.push_back(copyWithValues(*it, copy_prefix, copyfunc));
    }

    t_DAGNode *copied_node;
    if (operands.size() == 0)
    {
        copied_node = createNode(copy_prefix + node->m_label);
        copied_node->m_copy_count++;
        copied_node->m_value = copyfunc(node->m_value); //deep copy
    }
    else
    {
        copied_node = createNode(copy_prefix + node->m_label, operands);
        copied_node->m_copy_count++;
        copied_node->m_value = copyfunc(node->m_value); //deep copy
    }
    copy_map.insert(make_pair((unsigned long int) (node), (unsigned long int) (copied_node)));
    return copied_node;

}

/**
 * Synopsis: Public function that allows to replace a leaf by another DAG
 *
 * Description: This function allows to replace a leaf by other DAG. Note carefully the preconditions
 * 1. Both node_to_replace and node_replace_by must be non-null
 * 2. out_list of node_to_replace must be 0. i.e. the reader must be a leaf
 * 3. in_list of node_replace_by must be 0. i.e. the writer must be a root
 * Then this function replaces the reader by the write such that the effect is visibe in entire DAG. i.e. reader is no more part of ANY dag.
 *
 *
 * Arguments: Non-null DAGNodes toReplace and replaceBy that satisfy above written conditions
 *
 * Return value: True, if it succeeds, false otherwise
 *
 * Postcondition: shared flag of nodes may be incorrect after a call to this function. Use markSharedNodes function to obtain correct values for it.

 */

bool t_DAGManager::ReplaceLeafByRootOfADAG(t_DAGNode *toReplace, t_DAGNode *replaceBy)
{

    m_logManager->LOG("Invoking ReplaceLeafByRootOfADAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if ((toReplace == NULL) || (replaceBy == NULL))
    {
        m_logManager->LOG("Either toReplace or replaceBy is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (toReplace == replaceBy)
    {
        m_logManager->LOG("toreplace and replaceby are equal\n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }
    if (replaceBy->m_in_list.size() != 0) //Is this requirement still needed?????
    {
        m_logManager->LOG("in-list of replaceBy has size Non-0\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (toReplace->m_out_list.size() != 0)
    {
        m_logManager->LOG("out-list of toReplace has size Non-0\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    m_logManager->LOG("All tests passed for replacement\n", m_dagmanager_log, c_DebugLevelVerbosity);
    //bool replace_result = ReplaceDAGByDAG(toReplace, replaceBy);


    m_logManager->LOG("Releasing the node to replace\n", m_dagmanager_log, c_DebugLevelVerbosity);
    for (list<t_DAGNode*>::iterator it = toReplace->m_in_list.begin(); it != toReplace->m_in_list.end(); it++)
    {
        t_DAGNode *current_parent = *it;
        for (list<t_DAGNode*>::iterator parentit = current_parent->m_out_list.begin(); parentit != current_parent->m_out_list.end(); parentit++)
        {
            if (*parentit == toReplace)
            {

                list<t_DAGNode *>::iterator to_insert_at = current_parent->m_out_list.erase(parentit);
                current_parent->m_out_list.insert(to_insert_at, replaceBy);
                replaceBy->m_in_list.push_back(*it);
                break; //only one at a time!!
            }
        }
    }

    //At this stage, signature of every parent has changed.. so delete and insert every parent from the hashtable. 
    //Note that the address of parent is to be retained. So DO not delete the actual node, or create any new node, 
    //just delete the hashtable entry. (Note that the DAGNodes are created outside the hashtable and are just inserted in it. 
    //So deleting a hashtable entry does not delete the DAGNode itself!!
    
    toReplace->m_replaceBy = replaceBy;
    
    if (m_restricted_mode)
    {
        m_logManager->LOG("Changing the signature of every parent ....\n", m_dagmanager_log, c_DebugLevelVerbosity);
        for (list<t_DAGNode*>::iterator it = replaceBy->m_in_list.begin(); it != replaceBy->m_in_list.end(); it++)
        {
            bool correct_sign = RecomputeSignatureOfANode(*it);
            if (!correct_sign)
                return false;
        }
    }

    //  deleteDAG(toReplace);
    m_logManager->LOG("Successfully replaced Leaf by DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;

    //return replace_result;
}

/**
 * Synopsis: Replace child_to_replace by child_replace_by
 *
 *
 * Postcondition: shared flag of nodes may be incorrect after a call to this function. Use markSharedNodes function to obtain correct values for it.

 **/

bool t_DAGManager::ReplaceChild(t_DAGNode *parent, t_DAGNode *child_to_replace, t_DAGNode *child_replace_by)
{

    /*          parent_node
                /   \     \
               /     \     \
     *   c_t_repl  *
                           <-------c_repl_by
 


                Replace the child c_t_repl by c_repl_by.

                In this process, signaure of only parent_node changes
                +
                There are no constraints on what child_to_replace and child_replace_by are
     */

    m_logManager->LOG("Replacing a child \n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (parent == NULL || child_to_replace == NULL || child_replace_by == NULL)
    {
        m_logManager->LOG("Either parent or child_to_replace or child_replace_by is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    //First check if child_to_replace is really child of node parent
    for (list<t_DAGNode*>::iterator parentit = parent->m_out_list.begin(); parentit != parent->m_out_list.end(); parentit++)
    {
        if (*parentit == child_to_replace)
            break;
        else
        {
            m_logManager->LOG("child_to_replace is not a child of parent\n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
    }




    if (m_restricted_mode)
    {

        for (list<t_DAGNode*>::iterator parentit = parent->m_out_list.begin(); parentit != parent->m_out_list.end(); parentit++)
        {
            if (*parentit == child_to_replace)
            {
                list<t_DAGNode*>::iterator insert_at = parent->m_out_list.erase(parentit);
                parent->m_out_list.insert(insert_at, child_replace_by);
            }
        }

        //Remove parent from the in-list of child_to_replace
        for (list<t_DAGNode*>::iterator it = child_to_replace->m_in_list.begin(); it != child_to_replace->m_in_list.end(); it++)
        {
            if (*it == parent)
            {
                child_to_replace->m_in_list.erase(it);
                break;
            }
        }


        if (child_replace_by->m_isRoot)
        {
            bool deleted_from_roots = deleteNodeFromRootsList(child_replace_by);
            if (deleted_from_roots == false)
            {
                m_logManager->LOG("Could not delete child_replace_by from list of roots\n", m_dagmanager_log, c_RunLevelVerbosity);
                return false;
            }

        }

        //Add parent to the in-list of child_replace_by
        child_replace_by->m_in_list.push_back(parent);

        //Now the signature of the parent has changed!!!

        bool correct_sign = RecomputeSignatureOfANode(parent);
        if (!correct_sign)
        {
            m_logManager->LOG("ERROR!! Failed recomputing signature of parent\n", m_dagmanager_log, c_RunLevelVerbosity);
            return false;
        }
        m_logManager->LOG("Successfully replaced child \n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }
    else //!restricted mode
    {

        for (list<t_DAGNode*>::iterator parentit = parent->m_out_list.begin(); parentit != parent->m_out_list.end(); parentit++)
        {
            if (*parentit == child_to_replace)
            {
                list<t_DAGNode*>::iterator insert_at = parent->m_out_list.erase(parentit);
                parent->m_out_list.insert(insert_at, child_replace_by);
            }
        }

        //Remove parent from the in-list of child_to_replace
        for (list<t_DAGNode*>::iterator it = child_to_replace->m_in_list.begin(); it != child_to_replace->m_in_list.end(); it++)
        {
            if (*it == parent)
            {
                child_to_replace->m_in_list.erase(it);
                break;
            }
        }
        if (child_replace_by->m_isRoot)
        {
            bool deleted_from_roots = deleteNodeFromRootsList(child_replace_by);
            if (deleted_from_roots == false)
            {
                m_logManager->LOG("Could not delete child_replace_by from list of roots\n", m_dagmanager_log, c_RunLevelVerbosity);
                return false;
            }

        }

        //Add parent to the in-list of child_replace_by
        child_replace_by->m_in_list.push_back(parent);
        m_logManager->LOG("Successfully replaced child \n", m_dagmanager_log, c_DebugLevelVerbosity);
        return true;
    }
}

/**
 * Synopsis: Public function that allows to merge node toReplace and replaceBy
 *
 * Description: This function allows merging two functionally equivalent nodes. Note carefully the preconditions
 * 1. Both node_to_replace and node_replace_by must be non-null
 * The functional equivalance of two nodes is user determined...e.g. A DAG representing expression (a+0) is functionally equivalent to a DAG representing expression a, and one would want to replace the earlier by the later. After such replacement, he would want to see every DAG referring to DAG (a+0) as (a). This function allows this kind of merging.
 * Note that this function deletes the DAG toReplace (i.e. a+0) in its operation. External handles (references created to expression a+0) are no more valid. The DAGManager simply cannot redirect external references to point to DAG (a) because it does not have back-pointers to them. (However, all internal references will be redirected, as expected). So be careful with these handles in your code.
 *
 *
 * Arguments: Non-null DAGNodes toReplace and replaceBy that satisfy above written conditions
 *
 * Return value: True, if it succeeds, false otherwise
 *
 * Postcondition: shared flag of nodes may be incorrect after a call to this function. Use markSharedNodes function to obtain correct values for it.

 */

bool t_DAGManager::MergeEquivalentNodes(t_DAGNode *toReplace, t_DAGNode *replaceBy)
{

    m_logManager->LOG("Invoking MergeEquivalentNodes\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if ((toReplace == NULL) || (replaceBy == NULL))
    {
        m_logManager->LOG("Either toReplace or replaceBy is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG("ERROR!! Merging Failed\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (toReplace == replaceBy)
    {
        m_logManager->LOG("toreplace and replaceby are equal\n", m_dagmanager_log, c_RunLevelVerbosity);
        m_logManager->LOG("Merging Successful\n", m_dagmanager_log, c_RunLevelVerbosity);
        return true;
    }

    m_logManager->LOG("All tests passed for merging\n", m_dagmanager_log, c_DebugLevelVerbosity);

    //At this stage, what we should do is that 
    //1. Detach the toReplace node and replace it by replaceBy AT EXACTLY SAME locations in the DAG


    m_logManager->LOG("Releasing the node to replace and replacing it ....\n", m_dagmanager_log, c_DebugLevelVerbosity);
    for (list<t_DAGNode*>::iterator it = toReplace->m_in_list.begin(); it != toReplace->m_in_list.end(); it++)
    {
        t_DAGNode *current_parent = *it;
        for (list<t_DAGNode*>::iterator parentit = current_parent->m_out_list.begin(); parentit != current_parent->m_out_list.end(); parentit++)
        {
            if (*parentit == toReplace)
            {
                list<t_DAGNode *>::iterator to_insert_at = current_parent->m_out_list.erase(parentit);
                current_parent->m_out_list.insert(to_insert_at, replaceBy);
                replaceBy->m_in_list.push_back(*it);
                break; //only one at a time!!
            }
        }
    }


    //At this stage, signature of every parent has changed.. so delete
    //and insert every parent from the hashtable. Note that the address
    //of parent is to be retained. So DO not delete the actual node, or
    //create any new node, just delete the hashtable entry. (Note that
    //the DAGNodes are created outside the hashtable and are just
    //inserted in it. So deleting a hashtable entry does not delete the
    //DAGNode itself!!


    if (m_restricted_mode)
    {
        m_logManager->LOG("Changing the signature of every parent ....\n", m_dagmanager_log, c_DebugLevelVerbosity);
        for (list<t_DAGNode*>::iterator it = replaceBy->m_in_list.begin(); it != replaceBy->m_in_list.end(); it++)
        {
            bool correct_sign = RecomputeSignatureOfANode(*it);
            if (!correct_sign)
                return false;
        }
    }
    m_logManager->LOG("Deleting the DAG toReplace\n", m_dagmanager_log, c_DebugLevelVerbosity);
    //  deleteDAG(toReplace);
    m_logManager->LOG("Successfully merged two nodes in a DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: Obtain height of DAG by assigning every node a height equal to length of shortest path to this node starting from given root
 *
 * Description: The height of a DAGNode is defined as the length of shortest path to this node starting from given root.
 * This function performs a BFS. It uses the fact that a node at level i+1 is reached in a BFS only when all nodes at level i are already visited.
 * Note that this is a DAG, and there are no cycles.
 *
 * Return value: An integer giving the height of DAG, with root having height 1
 * -1 if it fails
 *
 */


int t_DAGManager::getHeightOfDAGWithMinimumPaths(t_DAGNode *root)
{
    if (root == NULL)
    {
        return -1;
    }
    bool reset_visited = ResetVisitedFlagOfDAG(root);
    if (!reset_visited)
    {
        return -1;
    }

    vector<t_DAGNode *> nodes_at_current_level;
    vector<t_DAGNode *> nodes_at_next_level;
    int current_level = 1;
    nodes_at_current_level.push_back(root);
    root->m_visited = true;

    //Always, elements contained in the nodes_at_current_level have visited flag true
    while (1)
    {
        vector<t_DAGNode *>::iterator it;
        for (it = nodes_at_current_level.begin(); it != nodes_at_current_level.end(); it++)
        {
            t_DAGNode *current_node = *it;
            list<t_DAGNode *>::iterator child_it;
            for (child_it = current_node->m_out_list.begin(); child_it != current_node->m_out_list.end(); child_it++)
            {
                t_DAGNode *child_to_add_to_next_level = *child_it;
                if (child_to_add_to_next_level->m_visited == true)
                {
                    continue;
                }
                else
                {
                    nodes_at_next_level.push_back(child_to_add_to_next_level);
                    child_to_add_to_next_level->m_visited = true;
                }
            }

        }

        if (nodes_at_next_level.size() == 0)
        {
            return current_level;
        }
        else
        {
            current_level++;
            nodes_at_current_level.clear();
            for (it = nodes_at_next_level.begin(); it != nodes_at_next_level.end(); it++)
            {
                nodes_at_current_level.push_back(*it);
            }
            nodes_at_next_level.clear();
        }
    }
    return -1;
}

/**
 * Synopsis: Private function that collects leaves
 *
 * Description: This is a private function used by getVectorOfLeafNodes that recursively traverses the DAG and collects the leaves of the DAG
 *
 * Return Value: The leaf nodes of the DAG in a STL vector
 */


vector<t_DAGNode*> t_DAGManager::collectLeavesRecursively(t_DAGNode* node)
{
    vector<t_DAGNode *> leaves;
    leaves.clear();
    if (node->m_visited == true)
    {
        return leaves;
    }
    else
    {
        node->m_visited = true;
        if (node->m_isLeaf || (node->m_out_list.size() == 0))
        {
            leaves.push_back(node);
            return leaves;
        }
        else
        {
            for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
            {
                vector <t_DAGNode *> leaves_of_children = collectLeavesRecursively(*it);
                for (int i = 0; i < leaves_of_children.size(); i++)
                {
                    leaves.push_back(leaves_of_children[i]);
                }
            }
            return leaves;
        }
    }
}

/**
 * Synopsis: Collect Leaves of a DAG
 *
 * Description: This function traverses the DAG pointed by node, and collects all the leaf nodes in that DAG. It returns this list of leaves. This function resets the visited flag of the DAG and invokes collectLeavesRecursively to actually collect the leaves
 *
 * Return Value: A empty/non-empty STL vector of DAGNode * which are leaves of this DAG
 */


vector<t_DAGNode *> t_DAGManager::getVectorOfLeafNodes(t_DAGNode *node)
{
    vector <t_DAGNode *> leaves;
    bool reset_visited = ResetVisitedFlagOfDAG(node);
    if (reset_visited == false)
        return leaves;

    leaves = collectLeavesRecursively(node);
    return leaves;
}

bool t_DAGManager::ReplaceLeafByDAG(t_DAGNode *toReplace, t_DAGNode *replaceBy, bool checkForAcyclicity)
{
    m_logManager->LOG("Invoking ReplaceLeafByDAG toReplace:"+toString(toReplace->getNodeID())+" replaceBy:"+toString(replaceBy->getNodeID()),
                      m_dagmanager_log, c_DebugLevelVerbosity);
    if ((toReplace == NULL) || (replaceBy == NULL))
    {
        m_logManager->LOG("Either toReplace or replaceBy is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (toReplace == replaceBy)
    {
        m_logManager->LOG("toreplace and replaceby are equal\n", m_dagmanager_log, c_RunLevelVerbosity);
        return true;
    }
    if (toReplace->m_out_list.size() != 0)
    {
        m_logManager->LOG("out-list of toReplace has size Non-0\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }



    m_logManager->LOG("All tests passed for replacement\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if (checkForAcyclicity)
    {
        /**** /
      vector<t_DAGNode *> immediate_parents_of_to_replace;
      list<t_DAGNode *> ::iterator beginit = toReplace->getInListBeginIterator();
      list<t_DAGNode *> ::iterator endit = toReplace->getInListEndIterator();
      list<t_DAGNode *> ::iterator it = beginit;
      while(it!=endit)
        {
          immediate_parents_of_to_replace.push_back(*it);
          it++;
        }
      for(int i=0;i<immediate_parents_of_to_replace.size();i++)
        {
          bool creates_cycle = 
            checkWhetherCreatingEdgeFormsCycle(immediate_parents_of_to_replace[i] , toReplace);
	  
          if(!creates_cycle)
            {
              m_logManager->LOG("Found child in the ancestors of parent, cannot  ReplaceLeafByDAG \n",m_dagmanager_log,c_RunLevelVerbosity);
              return false;
            }
        } **/
        //Perform the check for acyclicity
        //check if the node replaceBy appears in the ancestor list of toReplace
        set<t_DAGNode *> all_parents = getSetOfAncestorNodesByDFS(toReplace); //DFS or BFS does not matter
        set<t_DAGNode *>::iterator par_it;
        for (par_it = all_parents.begin(); par_it != all_parents.end(); par_it++)
        {
            if (*par_it == replaceBy)
            {
                {
//                    cerr<<"All parents of "<<toReplace->getNodeID()<<" with Label "<<toReplace->getLabel()<<endl;
//                    transform(all_parents.begin(), all_parents.end(), ostream_iterator<int>(cerr, ", "), 
//                            mem_fun(&t_DAGNode::getNodeID));
//                    cerr<<endl;
//                    {
//                        t_HashTable<unsigned long long, bool> visited;
//                    replaceBy->assignVisitedFlagRecursively(false, visited);
//                    }
                    //cerr<<"cyclic{"<<replaceBy->getNodeID()<<"} :=";
                    //replaceBy->printExpressionToFile(cerr);
                }
                
                throw std::runtime_error("Found child in the ancestors of parent, cannot  ReplaceLeafByDAG for label "+
                        replaceBy->getLabel() + "::"+ (*par_it)->getLabel());
                //return false;
            }
        }
    }



    m_logManager->LOG("Releasing the node to replace\n", m_dagmanager_log, c_RunLevelVerbosity);
    for (list<t_DAGNode*>::iterator it = toReplace->m_in_list.begin(); it != toReplace->m_in_list.end(); it++)
    {
        t_DAGNode *current_parent = *it;
        if (current_parent->m_replaceBy == NULL)
        {
            for (list<t_DAGNode*>::iterator parentit = current_parent->m_out_list.begin(); parentit != current_parent->m_out_list.end(); parentit++)
            {
                if (*parentit == toReplace)
                {
                    list<t_DAGNode *>::iterator to_insert_at = current_parent->m_out_list.erase(parentit);
                    current_parent->m_out_list.insert(to_insert_at, replaceBy);
                    replaceBy->m_in_list.push_back(*it);
                    break; //only one at a time!!
                }
            }
        }
    }

    toReplace->m_replaceBy = replaceBy;
    //At this stage, signature of every parent has changed.. so delete and insert every parent from the hashtable.
    //Note that the address of parent is to be retained. So DO not delete the actual node, or create any new node,
    //just delete the hashtable entry. (Note that the DAGNodes are created outside the hashtable and are just inserted in it.
    //So deleting a hashtable entry does not delete the DAGNode itself!!


    if (m_restricted_mode && m_recursiveReplaceFlag)
    {
        m_logManager->LOG("Changing the signature of every parent ....\n", m_dagmanager_log, c_DebugLevelVerbosity);
        for (list<t_DAGNode*>::iterator it = toReplace->m_in_list.begin(); it != toReplace->m_in_list.end(); it++)
        {
            bool correct_sign = RecomputeSignatureOfANode(*it);
            assert(correct_sign);
//            if (!correct_sign)
//            {
//                return false;
//            }
        }
    }

    //  deleteDAG(toReplace);
    m_logManager->LOG("Successfully replaced Leaf by DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;

}

/**
 * Synopsis: Public function to copy a DAG with values
 *
 * Description: This function allows to copy a DAG into new DAG including the values...This function requires a function pointer that defines how should the value be copied..Note that the way new and old DAG differentiate, is based on difference in their labels. Hence it uses a "prefix", with which it modifies the lables of the original DAG. The prefix is completely internal and not visible to client otherwise.
 *
 * Arguments: Non-null DAGNode
 *
 * Return value: Handle to copied DAG's root, if suceess
 *               NULL otherwise
 */

t_DAGNode * t_DAGManager::copyDAGWithValuesAndUserDefinedSuffixOnLeafNodes(t_DAGNode *node, void * (*copyfunc)(void *value_to_copy), string suffix)
{
    m_logManager->LOG("Copying the DAG with values and user defined suffix\n", m_dagmanager_log, c_DebugLevelVerbosity);
    string copy_prefix = suffix;
    if (node == NULL)
    {
        m_logManager->LOG("Root node is null\n", m_dagmanager_log, c_RunLevelVerbosity);
        return NULL;
    }
    bool reset_visited = ResetVisitedFlagOfAllDAGNodes();
    if (reset_visited == false)
    {
        m_logManager->LOG("ERROR!! Failed resetting visited flag of DAGNodes \n", m_dagmanager_log, c_RunLevelVerbosity);
        return NULL;
    }
    copy_map.clear();
    t_DAGNode *copied_root = copyWithValuesAndUserDefinedSuffixOnLeafNodes(node, copy_prefix, copyfunc, suffix);
    copy_map.clear();
    m_logManager->LOG("Successfully copied DAG with values\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return copied_root;
}

/**
 * Synopsis: PRIVATE function used during copying of DAG
 *
 * Description: This function actually performs copy of DAG by recursively traversing the original DAG.
 *
 * Arguments:
 *
 * Return value: Handle to copied DAG's root, if suceess
 *               NULL otherwise
 */

t_DAGNode * t_DAGManager::copyWithValuesAndUserDefinedSuffixOnLeafNodes(
                                                                        t_DAGNode *node, //(sub)DAG to copy
                                                                        string copy_prefix, //copy prefix
                                                                        void * (*copyfunc)(void *value_to_copy),
                                                                        string suffix
                                                                        )
{
    if (node->m_visited == true)
    {
        return (t_DAGNode *) copy_map[(unsigned long int) node];
    }
    node->m_visited = true;
    vector<t_DAGNode *> operands;

    for (list<t_DAGNode*>::iterator it = node->m_out_list.begin(); it != node->m_out_list.end(); it++)
    {
        t_DAGNode *child = *it;
        operands.push_back(copyWithValuesAndUserDefinedSuffixOnLeafNodes(*it, copy_prefix, copyfunc, suffix));
    }

    t_DAGNode *copied_node;
    if (operands.size() == 0)
    {
        copied_node = createNode(node->m_label + suffix);
        copied_node->m_copy_count++;
        copied_node->m_value = copyfunc(node->m_value); //deep copy
    }
    else
    {
        copied_node = createNode(node->m_label, operands);
        copied_node->m_copy_count++;
        copied_node->m_value = copyfunc(node->m_value); //deep copy
    }
    copy_map.insert(make_pair((unsigned long int) (node), (unsigned long int) (copied_node)));
    return copied_node;

}



/**
 *
 */

//Try to use this function for all replace methods!

bool t_DAGManager::ReplaceDAGByDAG(t_DAGNode *toReplace, t_DAGNode *replaceBy)
{
    int rec_cntr_recomp_fn = 0;
    int rec_cntr_replace_function = 0;
    bool result = ReplaceDAGByDAGInternal(toReplace, replaceBy, true, rec_cntr_recomp_fn, rec_cntr_replace_function);
    return result;
}
//need to comment
bool t_DAGManager::ReplaceDAGByDAGInternal(t_DAGNode *toReplace, t_DAGNode *replaceBy, bool enable_sanity_check, int rec_cntr_recomp_fn, int rec_cntr_replace_function)
{
    
    m_logManager->LOG("Invoking ReplaceDAGByDAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    if ((toReplace == NULL) || (replaceBy == NULL))
    {
        m_logManager->LOG("Either toReplace or replaceBy is NULL\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }
    if (toReplace == replaceBy)
    {
        m_logManager->LOG("toreplace and replaceby are equal\n", m_dagmanager_log, c_RunLevelVerbosity);
        return true;
    }

    m_logManager->LOG("All tests passed for replacement\n", m_dagmanager_log, c_DebugLevelVerbosity);

    m_logManager->LOG("Releasing the node to replace\n", m_dagmanager_log, c_DebugLevelVerbosity);

    list<t_DAGNode *> list_of_parents;
    for (list<t_DAGNode*>::iterator it = toReplace->m_in_list.begin(); it != toReplace->m_in_list.end(); it++)
    {
        t_DAGNode *current_parent = *it;
        /* Update 16-04-2011*/
        /* Update parent only if it has not yet been rendered garbage by ReplaceDAGByDAGInternal */
        if (current_parent->m_replaceBy == NULL)
        {
            list_of_parents.push_back(current_parent);            
            for (list<t_DAGNode*>::iterator parentit = current_parent->m_out_list.begin(); parentit != current_parent->m_out_list.end(); parentit++)
            {
                if (*parentit == toReplace)
                {
                    current_parent->m_out_list.insert(parentit, replaceBy);
                    list<t_DAGNode *>::iterator to_insert_at = current_parent->m_out_list.erase(parentit);
                    if (replaceBy->m_isRoot)
                    {
                        deleteNodeFromRootsList(replaceBy);
                        replaceBy->m_reference_count++;
                    }
                    replaceBy->m_in_list.push_back(*it);
                    break; //only one at a time!!
                }
            } 
        }
    }



    toReplace->m_in_list.clear();
    toReplace->m_reference_count = 0;
    /**
     * Modification on 9-04-2011
     */
    toReplace->m_replaceBy = replaceBy;

    //Delete the hashtable entry using the old signature
    string current_signature = toReplace->m_signature;
    t_HTStatusValue<t_DAGNode *> delete_result = ht.hashtable_delete(current_signature);
    if (delete_result.success())
    {
        m_logManager->LOG("Node deleted from hashtable successfully  " + toReplace->m_label + toString(toReplace->m_node_id) + "\n", m_dagmanager_log, c_DebugLevelVerbosity);
    }
    else
    {
        m_logManager->LOG("ERROR!! Node NOT deleted from hashtable  " + toReplace->m_label + toString(toReplace->m_node_id) + "\n", m_dagmanager_log, c_RunLevelVerbosity);
        return false;
    }

    if (m_restricted_mode)
    {
        m_logManager->LOG("Changing the signature of every parent ....\n", m_dagmanager_log, c_DebugLevelVerbosity);
        for (list<t_DAGNode*>::iterator it = list_of_parents.begin(); it != list_of_parents.end(); it++)
        {
            rec_cntr_recomp_fn += 1;
            m_logManager->LOG("RECURSION COUNTER FOR RecomputeSignatureOfANode:= " + integerToString(rec_cntr_recomp_fn) + "\n", m_dagmanager_log, c_RunLevelVerbosity);
            bool correct_sign = RecomputeSignatureOfANodeInternal(*it, false, rec_cntr_recomp_fn, rec_cntr_replace_function);
            if (!correct_sign)
                return false;
        }
    }
    //Acyclicity checks have to be enforced if enable_sanity_check is set to true
    if (enable_sanity_check)
    {
        // collect_garbage();
    }
    m_logManager->LOG("Successfully replaced DAG by DAG\n", m_dagmanager_log, c_DebugLevelVerbosity);
    return true;

}
