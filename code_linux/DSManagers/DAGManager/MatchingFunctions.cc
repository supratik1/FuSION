#include "DAGSimplifier.h"
#include "DagManager.h"
#include <map>
#include <list>
using namespace std;

/**
 * Synopsis: [Obtains correspondance between lhs of rewrite rule and DAGNodes]
 *
 * Description : [
 The correspondance map is map of a RewriteRuleExpression Node --> DAGNode
 The MapOfRuleComponentStringToDAGNode map is map of abstract string on RewriteRuleExpression node --> list of DAGNodes on which this label is assigned.
 ]
 *
 * Arguments :[]
 *
 * Return Value :[
 false if it fails obtaining correspondance. The values in correspondance and same_nodes are invalid if it fails.
 true if it succeeds. ] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier::getCorrespondenceBetweenRuleComponentStringsAndDAGNodes
(
 t_RewriteRuleExpression* lhs,
 t_DAGNode *node,
 map <string, t_DAGNode * >& MapOfRuleComponentStringToDAGNode,
 set<t_DAGNode *> & dagnodes_visited
 )

{
    if (node == NULL)
    {
        return false;
    }
    //*m_dag_simplifier_log << "Obtaining correspondence between LHS of a rule and DAGNode \n";
    vector<t_RewriteRuleExpression *>::iterator ia = lhs->m_arguments.begin();
    list<t_DAGNode*>::iterator iout = node->getOutListBeginIterator();
    
    // LHS and nodes are leaf and same label return true
    if(lhs->m_op_name == node->getLabel() && lhs->m_arguments.size()== 0 && node->getSizeOfOutList()==0)
    {
        return true;
    }
    // LHS and node have not same number of arguments
    else if(lhs->m_arguments.size() != node->getSizeOfOutList())
    {
        return false;
    }
    
    // LHS operator and node operator are not the same
    else if(lhs->m_op_name != node->getLabel() )
    {
        return false;
    }
    
    // Otherwise the operator on node is same as operator on LHS and same number of operands are present
    //    for both the node & LHS.
        /**
    if(lhs->m_op_name != node->getLabel() && lhs->m_arguments.size() != 0)
    {
        return false;
    }
    else if(lhs->m_arguments.size() != node->getSizeOfOutList())
    {
        return false;
    } */
    dagnodes_visited.insert(node);
    while (ia != lhs->m_arguments.end())
    {
        if (iout == node->getOutListEndIterator())
        {
            return false;
        }
        if ((*ia)->m_arguments.size() != 0) //Recurse if a nonleaf node
        {
            bool get_corr_result =
                    getCorrespondenceBetweenRuleComponentStringsAndDAGNodes(*ia, (*iout), MapOfRuleComponentStringToDAGNode, dagnodes_visited);

            if (!get_corr_result)
            {
                //*m_dag_simplifier_log << "WARNING!! Failed obtaining correspondance between rule nodes and DAG Nodes \n";
                return false;
            }
        }
        else
        {
            string temp = (*ia)->getStringOfRuleComponent();
            //*m_dag_simplifier_log << "Generated TEMP = " << temp << endl;
            string tempLabel = (*iout)->getLabel();
            if ((temp == "0b0") || (temp == "0b1") || (temp=="0bx"))
            {
                if (tempLabel == temp)
                {
                    MapOfRuleComponentStringToDAGNode[temp] = (*iout);
                }
                else
                {
                    return false;
                }
            }
            else
            {
                map<string, t_DAGNode *> ::iterator it = MapOfRuleComponentStringToDAGNode.find(temp);

                if (it != MapOfRuleComponentStringToDAGNode.end())
                {
                    t_DAGNode *node_corresponding_to_string = it->second;
                    if (!checkFunctionalEquivalanceOfDAGNodes(node_corresponding_to_string, (*iout)))
                    {
                       // *m_dag_simplifier_log << "WARNING!! Failed obtaining correspondance between rule nodes and DAG Nodes \n";
                        return false;
                    }
                }
                else
                {
                    MapOfRuleComponentStringToDAGNode.insert(make_pair(temp, (*iout)));
                    dagnodes_visited.insert((*iout));
                }
            }
        }
        ia++;
        iout++;
    }
    //*m_dag_simplifier_log << "Successfully obtained correspondance between lhs and DAG\n";
    return true;
}

inline bool t_DAGSimplifier::checkFunctionalEquivalanceOfDAGNodes(t_DAGNode *node1, t_DAGNode *node2)
{
    return (node1 == node2);
}
