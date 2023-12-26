#include "DAGSimplifier.h"
#include "DagManager.h"
#include <map>
using namespace std;

/**
 * Synopsis: []
 *
 * Description : []
 *
 * Arguments :[
 DAGNode * &node: Let node point to a DAGNode "n" before the simplification. The intended use of node is that, after simplification, it should point to a DAGNode that gives the simplified version of the DAG rooted at "n". 

]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier::RunAStrategy(t_DAGNode * & node, string Strategy)
{
    m_logManager->LOG("In RunAStrategy function\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    m_logManager->LOG("Initiating the preprocessing of the DAG before starting simplification\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    //Add the labels to DAGNodes from here!!
    bool add_labels = this->initializeAbstractRuleSignaturesOfDAG(node);
    if (!add_labels)
    {
        m_logManager->LOG("Failed adding labels to DAGNodes \n", m_dag_simplifier_log, c_RunLevelVerbosity);
        return false;
    }
    m_logManager->LOG("After initial labeling \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    ofstream *dotfile2 = new ofstream("initiallabels");
    bool printed_as_dot = m_dag_manager->printDAGAsDOTFileWithAbstractSignatures(node, dotfile2);
    dotfile2->close();

    m_logManager->LOG("Strategy requested is " + Strategy + "\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    if (Strategy == "FCFS")
    {
        return runFCFSStrategy(node);
    }
    else if (Strategy == "PRIO")
    {
        return runPriorityStrategy(node);
    }
    else if (Strategy == "RAND")
    {
        //return runRandomStrategy(root);
    }
    else
    {
        m_logManager->LOG("ERROR!! Failed running strategy " + Strategy + "\n", m_dag_simplifier_log, c_RunLevelVerbosity);
        return false;
    }

    return true;
}

/**
 * Synopsis: []
 *
 * Description : []
 *
 * Arguments :[]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier::runFCFSStrategy(t_DAGNode * &root)
{
    //Before we run the FCFS strategy, we must make sure that the labels are already added to the DAG, therefore the function RunAStrategy
    m_logManager->LOG("Starting FCFS Strategy\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    static int counter = 0;
    int RuleCounter = 0; //Counter over rules

    vector<t_RewriteRule *> ::iterator rules_iterator;
    bool dag_changed = true;
    while (dag_changed == true)
    {
        dag_changed = false;
        for (rules_iterator = this->m_setOfRewriteRules.begin(); rules_iterator != this->m_setOfRewriteRules.end(); rules_iterator++)
        {
            t_RewriteRule *rule_being_applied = *rules_iterator; //Current Rule to apply
            set<t_DAGNode *> NodesMatched; //Set of nodes on which current rule has matched
            bool clear_labels = this->clearAbstractRuleSignaturesOfDAG(root);
            bool add_labels = this->initializeAbstractRuleSignaturesOfDAG(root);
            set<t_DAGNode *> CandidateAbstractSignChangedNodes; //This is the set of nodes whose abstract signature could have potentially changed! This includes the set of nodes corresponding to LHS and nodes created/reused in RHS

            NodesMatched.clear();
            RuleCounter++;
            m_logManager->LOG("Applying Rule Number " + toString(RuleCounter) + "\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
	    rule_being_applied->printRewriteRule(m_dag_simplifier_log);
            string valid_string_of_lhs_of_current_rule = rule_being_applied->getValidStringOfLHSOfRule();
            m_logManager->LOG("ValidString of LHS of rule is " + rule_being_applied->getValidStringOfLHSOfRule() + "\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
            m_logManager->LOG("Assigning the nodes of the DAG to matched rules by traversing the DAG\n", m_dag_simplifier_log, c_DebugLevelVerbosity);

            bool add_dag_nodes_to_rules_result =
                    addDAGNodesMatchingThisRuleToSetOfMatchedNodes(root, rule_being_applied, NodesMatched, true);

            if (!add_dag_nodes_to_rules_result)
            {
                m_logManager->LOG("ERROR!! Failed adding the nodes of the DAG to matched rules \n", m_dag_simplifier_log, c_RunLevelVerbosity);
                return false;
            }

            m_logManager->LOG("Finished assigning the nodes of the DAG to matched rules by traversing the DAG\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
            m_logManager->LOG("Number of nodes matched on this rule : " + toString(NodesMatched.size()) + "\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
            for (set<t_DAGNode *> ::iterator listit = NodesMatched.begin(); listit != NodesMatched.end(); listit++)
            {
                t_DAGNode *node_to_process = (*listit);


                if (node_to_process->isGarbageNode())
                {
                    m_logManager->LOG("Found a garbage node to process \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                    //Delete this node from the list of nodes
                    //		  NodesMatched.erase( (*listit));
                    continue;
                }
                if (!checkIfNodeHasAAbstractRuleSignature(node_to_process, rule_being_applied->getValidStringOfLHSOfRule())) //This check is necessary
                {
                    m_logManager->LOG("Node has lost its simplification label \n", m_dag_simplifier_log, c_RunLevelVerbosity);
                    //Delete this node from the list of nodes
                    //NodesMatched.erase( (*listit));
                    continue;
                }


                map <string, t_DAGNode * > MapOfRuleComponentStringToDAGNode;
                set<t_DAGNode *> dagnodes_corr_to_lhs;
                bool get_corr_result =
                        getCorrespondenceBetweenRuleComponentStringsAndDAGNodes(rule_being_applied->lhs, node_to_process, MapOfRuleComponentStringToDAGNode, dagnodes_corr_to_lhs);

                if (!get_corr_result)
                {
                    //return false;
                    continue;
                }
                for (set<t_DAGNode *> ::iterator lhs_it = dagnodes_corr_to_lhs.begin(); lhs_it != dagnodes_corr_to_lhs.end(); lhs_it++)
                {
                    if ((*lhs_it) == node_to_process)
                    {
                        dagnodes_corr_to_lhs.erase(lhs_it);
                        break;
                    }
                }

                m_logManager->LOG("Applying the Rule and constructing RHS\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                set<t_DAGNode *> dagnodes_corr_to_rhs;
                t_DAGNode * changed =
                        applyRuleAtANodeAndConstructRHS(node_to_process, rule_being_applied, root, MapOfRuleComponentStringToDAGNode, dagnodes_corr_to_rhs);


                m_logManager->LOG("Successfully applied rule on the node", m_dag_simplifier_log, c_DebugLevelVerbosity);
                if (changed == NULL)
                {
                    m_logManager->LOG("ERROR!! Failed applying the rule on the given node\n", m_dag_simplifier_log, c_RunLevelVerbosity);
                }
                else
                {
                    dag_changed = true;
                    m_logManager->LOG("Clearing simplification labels of current node \n", m_dag_simplifier_log, c_DebugLevelVerbosity);

                    m_logManager->LOG("Calling for incremental update \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                    set<t_DAGNode *> nodes_to_update_abstract_sign_on;
                    m_logManager->LOG("LHS nodes " + toString(dagnodes_corr_to_lhs.size()) + "\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                    m_logManager->LOG("RHS nodes " + toString(dagnodes_corr_to_rhs.size()) + "\n", m_dag_simplifier_log, c_DebugLevelVerbosity);

                    //		  nodes_to_update_abstract_sign_on.insert(dagnodes_corr_to_lhs.begin(), dagnodes_corr_to_lhs.end());
                    nodes_to_update_abstract_sign_on.insert(dagnodes_corr_to_rhs.begin(), dagnodes_corr_to_rhs.end());
                    ++counter;
                    m_dag_manager->printASetOfDAGNodes(nodes_to_update_abstract_sign_on);
                    bool incr_update_result = increamental_update(nodes_to_update_abstract_sign_on);
                    if (!incr_update_result)
                    {
                        m_logManager->LOG("ERROR!! Failed performing incremental update \n", m_dag_simplifier_log, c_RunLevelVerbosity);
                        return false;
                    }



                }
                dagnodes_corr_to_lhs.clear();
                dagnodes_corr_to_rhs.clear();


            }

        }
    }
    m_logManager->LOG("Finished Running FCFS Strategy\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    return true;
}

/**
 * Synopsis: []
 *
 * Description : []
 *
 * Arguments :[]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier::increamental_update(set<t_DAGNode *> nodes_to_update)
{
    //  cout<<"--------------------------------------------------\n,m_dag_simplifier_log,c_DebugLevelVerbosity);
    //  cout<<"Performing incremental update "<<"\n,m_dag_simplifier_log,c_DebugLevelVerbosity);
    m_logManager->LOG("--------------------------------------------------\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    m_logManager->LOG("Performing incremental update \n", m_dag_simplifier_log, c_DebugLevelVerbosity);

    set<t_DAGNode *> new_nodes_to_update;
    if (nodes_to_update.size() == 0)
    {
        m_logManager->LOG("0 nodes for incremental update\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
        return true;
    }
    for (set <t_DAGNode *>::iterator it = nodes_to_update.begin(); it != nodes_to_update.end(); it++)
    {
        t_DAGNode *node_to_process = (*it);
        if (node_to_process->isGarbageNode())
            continue;
        set<string> old_abstract_signs = node_to_process->getAbstractSignatures();
        /*** /m_logManager->LOG("-----------------------------------------------------\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
        m_logManager->LOG("Old Signs : "+node_to_process->getLabel()+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
        for(set<string>::iterator i=old_abstract_signs.begin(); i!=old_abstract_signs.end();i++)
          {
            m_logManager->LOG((*i),m_dag_simplifier_log,c_DebugLevelVerbosity);
          } */
        node_to_process->clearAbstractSignatures();
        addAbstractRuleSignaturesToADAGNode(node_to_process);
        set<string> new_abstract_signs = node_to_process->getAbstractSignatures();
        /*** /m_logManager->LOG("New Signs :"+node_to_process->getLabel(),m_dag_simplifier_log,c_DebugLevelVerbosity);
        for(set<string>::iterator i=new_abstract_signs.begin(); i!=new_abstract_signs.end();i++)
          {
            m_logManager->LOG((*i),m_dag_simplifier_log,c_DebugLevelVerbosity);
          }
        m_logManager->LOG("-----------------------------------------------------\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
         */
        if (checkSetsOfStringsForEquality(old_abstract_signs, new_abstract_signs))
        {
            continue;
        }
        else
        {
            list <t_DAGNode *> ::iterator itbegin = node_to_process->getInListBeginIterator();
            list <t_DAGNode *> ::iterator itend = node_to_process->getInListEndIterator();
            for (list <t_DAGNode *> ::iterator itparent = itbegin; itparent != itend; itparent++)
            {
                new_nodes_to_update.insert(*itparent);
            }
        }
    }

    increamental_update(new_nodes_to_update);
    //  cout<<"--------------------------------------------------\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
    //  cout<<"Finished Performing incremental update\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
    m_logManager->LOG("--------------------------------------------------\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    m_logManager->LOG("Finished Performing incremental update\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    return true;
}

bool t_DAGSimplifier::removeEdgesCorrespondingToLHSOfRule(t_RewriteRuleExpression *lhs, t_DAGNode *node)
{
    /*  vector<PCPair *> v;
    bool remove_edges_result = removeEdgesCorrespondingToLHSOfRuleRecursively(lhs, node, v);
    for(int i=0;i<v.size(); i++)
      {
        dag_manager->removeEdgeWithoutChangingSignatures(v[i]->parent, v[i]->child);
      }

    return remove_edges_result;
     */
}

/**
 * Synopsis: []
 *
 * Description : []
 *
 * Arguments :[]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier::removeEdgesCorrespondingToLHSOfRuleRecursively(t_RewriteRuleExpression *lhs, t_DAGNode *node, vector<PCPair *> & v)
{
    t_RewriteRuleExpression *current_node = lhs;
    t_DAGNode *corresponding_node = node;
    if (lhs->m_arguments.size() == 0)
    {
        return true;
    }
    else
    {
        for (int i = 0; i < lhs->m_arguments.size(); i++)
        {
            t_DAGNode *child_node = m_dag_manager->getIThChildOfNode(node, i);
            //*dag_simplifier_log<<"Removing Edges "<<node->getLabel()<<" "<<child_node->getLabel(),m_dag_simplifier_log,c_DebugLevelVerbosity);
            if (node->getInListSize() == 0 || node->getInListSize() == 1)
            {
                PCPair *p = new PCPair();
                p->parent = node;
                p->child = child_node;
                v.push_back(p);
            }

        }
        for (int i = 0; i < lhs->m_arguments.size(); i++)
        {
            t_DAGNode *child_node = m_dag_manager->getIThChildOfNode(node, i);
            bool remove_edges_rec_result =
                    removeEdgesCorrespondingToLHSOfRuleRecursively(lhs->m_arguments[i], child_node, v);
            if (!remove_edges_rec_result)
            {
                return false;
            }
        }
    }
    return true;
}




//node is the DAGNode on which the RULE rule is to be applied

/**
 * Synopsis: []
 *
 * Description : []
 *
 * Arguments :[
 DAGNode * &node: Let node point to a DAGNode "n" before the simplification. The intended use of node is that, after simplification, it should point to a DAGNode that gives the simplified version of the DAG rooted at "n". 
 ]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */

t_DAGNode *t_DAGSimplifier::applyRuleAtANodeAndConstructRHS
(
 t_DAGNode * &current_node,
 t_RewriteRule *rule,
 t_DAGNode * &node,
 map<string, t_DAGNode *> &MapOfRuleNodeNameToDAGNode,
 set<t_DAGNode *> &dagnodes_corr_to_rhs
 )
{
    m_logManager->LOG("---------------------------------------------\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    m_logManager->LOG("Applying rule at a node \n", m_dag_simplifier_log, c_DebugLevelVerbosity);

    t_RewriteRuleExpression *rhs_of_rule = rule->rhs;
    t_DAGNode *result;


    result = constructRHSOfRuleRecursively(rhs_of_rule, MapOfRuleNodeNameToDAGNode, dagnodes_corr_to_rhs);
    //Redirect the in-list of correspondance[current_node] to result node
    //if current_node is non-leaf
    //replace the result node in its place!!!
    //from the in-list of toReplace, find out all its parents, and make them redirect to the result node
    //delete the toReplace node from the out-list of all its parents
    bool replace_result = m_dag_manager->ReplaceDAGByDAG(current_node, result);
    if (!replace_result)
    {
        m_logManager->LOG("ERROR!! Failed replacing the matched non-leaf node by the RHS\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
        return NULL;
    }


    if (m_logManager->getVerbosityLevel() == c_DebugLevelVerbosity)
    {
        if (current_node == node)
        {
            ofstream *dotfile2 = new ofstream("newroot");
            bool printed_as_dot = m_dag_manager->printDAGAsDOTFileWithAbstractSignatures(result, dotfile2);
            dotfile2->close();
            //cout<<"Modifying the root \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
            node = result;
        }
        m_logManager->LOG("---------------------------------------------\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    }
    return result;
}

/**
 * Synopsis: []
 *
 * Description : []
 *
 * Arguments :[]
 *
 * Return Value :[]
 *
 * Side Effects : []
 */
t_DAGNode * t_DAGSimplifier::constructRHSOfRuleRecursively
(
 t_RewriteRuleExpression * rhs_expression,
 map<string, t_DAGNode *> MapOfRuleNodeNameToDAGNode,
 set<t_DAGNode *> &dagnodes_corr_to_rhs
 )
{
    t_DAGNode *result;
    vector<t_DAGNode *> children;
    if (rhs_expression->m_arguments.size() == 0)
    {
        //This is a leaf in rhs
        string signature_of_rhs_node = rhs_expression->getStringOfRuleComponent();
        if (signature_of_rhs_node == "true")
        {
            result = m_dag_manager->createNode("true");
        }
        else if (signature_of_rhs_node == "false")
        {
            result = m_dag_manager->createNode("false");
        }
        else if (signature_of_rhs_node == "0")
        {
            result = m_dag_manager->createNode("0");
        }
        else if (signature_of_rhs_node == "1")
        {
            result = m_dag_manager->createNode("1");
        }
        else if (signature_of_rhs_node == "0b1")
        {
            result = m_dag_manager->createNode("0b1");
        }
        else if (signature_of_rhs_node == "0b0")
        {
            result = m_dag_manager->createNode("0b0");
        }
        else
        {
            map<string, t_DAGNode *> ::iterator map_rule_to_dag_it;
            map_rule_to_dag_it = MapOfRuleNodeNameToDAGNode.find(signature_of_rhs_node);
            if (map_rule_to_dag_it != MapOfRuleNodeNameToDAGNode.end())
            {
                result = MapOfRuleNodeNameToDAGNode[signature_of_rhs_node];
            }
            else
            {
                m_logManager->LOG("ERROR!! Could not create leaf node in RHS\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
            }
        }
        dagnodes_corr_to_rhs.insert(result);
        return result;
    }
    else
    {
        for (int i = 0; i < rhs_expression->m_arguments.size(); i++)
        {

            t_DAGNode *rhs_child = constructRHSOfRuleRecursively(rhs_expression->m_arguments[i], MapOfRuleNodeNameToDAGNode, dagnodes_corr_to_rhs);
            children.push_back(rhs_child);
        }

        //string result_label = rhs_expression->getStringOfRuleComponent();
        string result_label = rhs_expression->m_op_name;
        //This node is newly created, so there is no need to recompute its own signature
        result = m_callBack(result_label, children);//m_dag_manager->createNode(result_label, children);
        if (m_logManager->getVerbosityLevel() == c_DebugLevelVerbosity)
        {
            ofstream *dotfile2 = new ofstream("temp");
            bool printed_as_dot = m_dag_manager->printDAGAsDOTFileWithAbstractSignatures(result, dotfile2);
            dotfile2->close();
        }
        dagnodes_corr_to_rhs.insert(result);
        return result;

    }

}

bool t_DAGSimplifier::runPriorityStrategy(t_DAGNode *&root)
{
    //Before we run the Priority strategy, we must make sure that the labels are already added to the DAG, therefore the function RunAStrategy
    m_logManager->LOG("Starting PRIORITY Strategy\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    static int counter = 0;
    int RuleCounter = 0; //Counter over rules

    vector<t_RewriteRule *> ::iterator rules_iterator;
    bool dag_changed = true;
    bool sort_rules_result = this->sortTheRewriteRulesAsPerPriority();
    /**
    ofstream *o = new ofstream();
    o->open("sortedrules");
    printSortedRules(o);
     o->close();
     **/
    while (dag_changed == true)
    {
        dag_changed = false;
        for (rules_iterator = this->m_sortedSetOfRewriteRules.begin(); rules_iterator != this->m_sortedSetOfRewriteRules.end(); rules_iterator++)
        {
            t_RewriteRule *rule_being_applied = *rules_iterator; //Current Rule to apply
            set<t_DAGNode *> NodesMatched; //Set of nodes on which current rule has matched
            bool clear_labels = this->clearAbstractRuleSignaturesOfDAG(root);
            bool add_labels = this->initializeAbstractRuleSignaturesOfDAG(root);
            set<t_DAGNode *> CandidateAbstractSignChangedNodes; //This is the set of nodes whose abstract signature could have potentially changed! This includes the set of nodes corresponding to LHS and nodes created/reused in RHS

            NodesMatched.clear();
            RuleCounter++;
            m_logManager->LOG("Applying Rule Number " + toString(RuleCounter), m_dag_simplifier_log, c_DebugLevelVerbosity);
	    rule_being_applied->printRewriteRule(m_dag_simplifier_log);
            string valid_string_of_lhs_of_current_rule = rule_being_applied->getValidStringOfLHSOfRule();
            m_logManager->LOG("ValidString of LHS of rule is " + rule_being_applied->getValidStringOfLHSOfRule(), m_dag_simplifier_log, c_DebugLevelVerbosity);
            m_logManager->LOG("Assigning the nodes of the DAG to matched rules by traversing the DAG\n", m_dag_simplifier_log, c_DebugLevelVerbosity);

            bool add_dag_nodes_to_rules_result =
                    addDAGNodesMatchingThisRuleToSetOfMatchedNodes(root, rule_being_applied, NodesMatched, true);

            if (!add_dag_nodes_to_rules_result)
            {
                m_logManager->LOG("ERROR!! Failed adding the nodes of the DAG to matched rules \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                return false;
            }

            m_logManager->LOG("Finished assigning the nodes of the DAG to matched rules by traversing the DAG\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
            m_logManager->LOG("Number of nodes matched on this rule : " + toString(NodesMatched.size()), m_dag_simplifier_log, c_DebugLevelVerbosity);
            for (set<t_DAGNode *> ::iterator listit = NodesMatched.begin(); listit != NodesMatched.end(); listit++)
            {
                t_DAGNode *node_to_process = (*listit);


                if (node_to_process->isGarbageNode())
                {
                    m_logManager->LOG("Found a garbage node to process \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                    //Delete this node from the list of nodes
                    //		  NodesMatched.erase( (*listit));
                    continue;
                }
                if (!checkIfNodeHasAAbstractRuleSignature(node_to_process, rule_being_applied->getValidStringOfLHSOfRule())) //This check is necessary
                {
                    m_logManager->LOG("Node has lost its simplification label \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                    //Delete this node from the list of nodes
                    //NodesMatched.erase( (*listit));
                    continue;
                }


                map <string, t_DAGNode * > MapOfRuleComponentStringToDAGNode;
                set<t_DAGNode *> dagnodes_corr_to_lhs;
                bool get_corr_result =
                        getCorrespondenceBetweenRuleComponentStringsAndDAGNodes(rule_being_applied->lhs, node_to_process, MapOfRuleComponentStringToDAGNode, dagnodes_corr_to_lhs);

                if (!get_corr_result)
                {
                    //return false;
                    continue;
                }
                for (set<t_DAGNode *> ::iterator lhs_it = dagnodes_corr_to_lhs.begin(); lhs_it != dagnodes_corr_to_lhs.end(); lhs_it++)
                {
                    if ((*lhs_it) == node_to_process)
                    {
                        dagnodes_corr_to_lhs.erase(lhs_it);
                        break;
                    }
                }

                set<t_DAGNode *> dagnodes_corr_to_rhs;
                t_DAGNode * changed =
                        applyRuleAtANodeAndConstructRHS(node_to_process, rule_being_applied, root, MapOfRuleComponentStringToDAGNode, dagnodes_corr_to_rhs);


                m_logManager->LOG("Successfully applied rule on the node", m_dag_simplifier_log, c_DebugLevelVerbosity);
                if (changed == NULL)
                {
                    m_logManager->LOG("ERROR!! Failed applying the rule on the given node\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                }
                else
                {
                    dag_changed = true;
                    m_logManager->LOG("Clearing simplification labels of current node \n", m_dag_simplifier_log, c_DebugLevelVerbosity);

                    m_logManager->LOG("Calling for incremental update \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                    set<t_DAGNode *> nodes_to_update_abstract_sign_on;
                    m_logManager->LOG("LHS nodes " + toString(dagnodes_corr_to_lhs.size()), m_dag_simplifier_log, c_DebugLevelVerbosity);
                    m_logManager->LOG("RHS nodes " + toString(dagnodes_corr_to_rhs.size()), m_dag_simplifier_log, c_DebugLevelVerbosity);

                    //		  nodes_to_update_abstract_sign_on.insert(dagnodes_corr_to_lhs.begin(), dagnodes_corr_to_lhs.end());
                    nodes_to_update_abstract_sign_on.insert(dagnodes_corr_to_rhs.begin(), dagnodes_corr_to_rhs.end());
                    ++counter;
                    m_dag_manager->printASetOfDAGNodes(nodes_to_update_abstract_sign_on);
                    bool incr_update_result = increamental_update(nodes_to_update_abstract_sign_on);
                    if (!incr_update_result)
                    {
                        m_logManager->LOG("ERROR!! Failed performing incremental update \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
                        return false;
                    }
                    string filename = "inter_" + integerToString(counter);
                    ofstream *dotfile2 = new ofstream(filename.c_str());
                    bool printed_as_dot = m_dag_manager->printDAGAsDOTFileWithAbstractSignatures(root, dotfile2);
                    dotfile2->close();



                }
                dagnodes_corr_to_lhs.clear();
                dagnodes_corr_to_rhs.clear();


            }

        }
    }
    m_logManager->LOG("Finished Running PRIO Strategy\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
}

/**
 * @method runPriorityStrategyV2()
 * @param root
 * @return true on successful (or false  on failed) application of some rule
 */

bool t_DAGSimplifier::runPriorityStrategyV2(t_DAGNode *&root, bool increamental_update_flag)
{
    //Before we run the Priority strategy, we must make sure that the labels are already added to the DAG, therefore the function RunAStrategy
    m_logManager->LOG("Starting PRIORITY Strategy-V2\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    static int counter = 0;
    int RuleCounter = 0; //Counter over rules
    // Compute abstract signatures of each node
    bool dag_changed = true;
    if (root->getSizeOfOutList() == 0)
    {
        return false;
    }
    if (root->isGarbageNode())
    {
        m_logManager->LOG("Found a garbage node to process \n", m_dag_simplifier_log, c_DebugLevelVerbosity);
        return false;
    }

    string operatorName = root->getLabel();

    vector<t_RewriteRule* > rulesOfCurrentOperator = m_MapOfRewriteRulesBasedOnOperators[operatorName];
    if (!rulesOfCurrentOperator.empty())
    {
        vector<t_RewriteRule* >::iterator rules_iterator;
        for (rules_iterator = rulesOfCurrentOperator.begin(); rules_iterator != rulesOfCurrentOperator.end(); rules_iterator++)
        {
            t_RewriteRule *rule_being_applied = *rules_iterator; //Current Rule to apply
            set<t_DAGNode *> NodesMatched; //Set of nodes on which current rule has matched
            NodesMatched.clear();
            RuleCounter++;

            m_logManager->LOG("Trying Rule Number " + toString(RuleCounter), m_dag_simplifier_log, c_DebugLevelVerbosity);
   
            map <string, t_DAGNode * > MapOfRuleComponentStringToDAGNode;
            set<t_DAGNode *> dagnodes_corr_to_lhs;
            bool get_corr_result = getCorrespondenceBetweenRuleComponentStringsAndDAGNodes(rule_being_applied->lhs, root, MapOfRuleComponentStringToDAGNode, dagnodes_corr_to_lhs);
            if (!get_corr_result) {
		m_logManager->LOG("No match with rule #" + toString(RuleCounter), m_dag_simplifier_log, c_DebugLevelVerbosity);
                continue;
            }
            for (set<t_DAGNode *> ::iterator lhs_it = dagnodes_corr_to_lhs.begin(); lhs_it != dagnodes_corr_to_lhs.end(); lhs_it++)
            {
                if ((*lhs_it) == root)
                {
                    dagnodes_corr_to_lhs.erase(lhs_it);
                    break;
                }
            }
            set<t_DAGNode *> dagnodes_corr_to_rhs;
            t_DAGNode * changed = applyRuleAtANodeAndConstructRHS(root, rule_being_applied, root, MapOfRuleComponentStringToDAGNode, dagnodes_corr_to_rhs);
            m_logManager->LOG("Successfully applied rule on the node", m_dag_simplifier_log, c_DebugLevelVerbosity);
	    rule_being_applied->printRewriteRule(m_dag_simplifier_log);
            if (changed == NULL)
            {
                m_logManager->LOG("ERROR!! Failed applying the rule on the given node\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
            }
            else
            {
                dag_changed = true;
                m_logManager->LOG("LHS nodes " + toString(dagnodes_corr_to_lhs.size()) + "==> RHS nodes " + toString(dagnodes_corr_to_rhs.size()), m_dag_simplifier_log, c_DebugLevelVerbosity);

                ++counter;
                root = changed;
                break;
            }
            dagnodes_corr_to_lhs.clear();
            dagnodes_corr_to_rhs.clear();
        }
    }
    m_logManager->LOG("Finished Running PRIO Strategy\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    return true;
}
