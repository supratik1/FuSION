#include "DagManager.h"
#include "helping_functions.h"

#include<iostream>
using namespace std;



/** Constructors of the DAGNode*/

t_DAGNode:: t_DAGNode(void *value_to_set) //Default Constructor
{
  m_label = "";
  //Nothing to do with reference_count here

  //Nothing to do with signature here
  //no in-list
  //no out-list
  m_visited = false;
  //nothing to do with shared flag here
  m_isRoot = true;
  m_isLeaf = true;
  m_copy_count = 0;
  m_value = value_to_set;
  m_temp_id = 0;
}

t_DAGNode :: t_DAGNode(const string& label_to_assign, void *value_to_set)
{
  m_isLeaf = true;
  m_label = label_to_assign;
  m_isRoot = true;
  m_visited = false;
  m_copy_count = 0;
  m_value = value_to_set;
  m_temp_id = 0;
}

t_DAGNode :: t_DAGNode(const string& label_to_assign, const vector<t_DAGNode*>& v_operands, void *value_to_set)
{

  int size = v_operands.size();
  for(int i=0;i<size;i++)
    {
      m_out_list.push_back(v_operands[i]);
      v_operands[i]->m_in_list.push_back(this);
    } /**/


  //if v_operands is empty, then this is really a leaf node
  if(m_out_list.size()==0)
    m_isLeaf = true;
  else
    m_isLeaf = false;
  m_label = label_to_assign;
  m_isRoot = true;
  m_visited = false;
  m_copy_count = 0;
  m_value = value_to_set;
  m_temp_id = 0;
}


/* Destructor of the DAGNode */
t_DAGNode :: ~t_DAGNode()
{
  //This is the destructor...
    //oh really?

}

void t_DAGNode::printExpressionToFile(ostream& outfile)
{
    static const bool printNodeIdOfSymbol = true;
    t_DAGNode* const dagnode = this;
        if (dagnode == NULL)
    {
        return ;
    }

    outfile << dagnode->getLabel();

    int num_operands = dagnode->getSizeOfOutList();
    if (num_operands > 0)
    {
        outfile << "(";
        int i = 0;
        list<t_DAGNode *>::iterator it = dagnode->getOutListBeginIterator();
        list<t_DAGNode *>::iterator outlist_end_iterator = dagnode->getOutListEndIterator();
        
        for (; it != outlist_end_iterator; it++)
        {
            if ((*it)->getSizeOfOutList() > 0)
            {
                outfile << "["<< (*it)->getNodeID()<< "]"; // << hex << (long int) *it <<
            }
            else
            {
                outfile << (*it)->getLabel();
                if(printNodeIdOfSymbol)
                    outfile<<":"<<(*it)->getNodeID();
            }

            if (i != num_operands - 1)
                outfile << ",";

            i++;
        }
        outfile << ")"<<endl;
        dagnode->assignVisitedFlag(true);

        it = dagnode->getOutListBeginIterator();
        for (; it != outlist_end_iterator; it++)
        {
            if (((*it)->isNodeVisited()==false) && ((*it)->getSizeOfOutList() > 0))
            {
                outfile << "["<< (*it)->getNodeID()<< "] := ";
                (*it)->printExpressionToFile(outfile);		
            }
        }

    }
}

/**
 * Synopsis: Read the label of the DAGNode
 *
 * Description: This is a public function that reads the label of the node. Note that it will eliminate the copy prefix from the label of the node before returning. One can query the DAGNode with isCopied method to test if the node was copied..
 Also note that labels of nodes never change, once assigned at the time of creation.
 *
 *
 */



string t_DAGNode ::getLabel()
{
  if(m_copy_count==0)
    return this->m_label;
  else
    {
      string copy_prefix = integerToString(this->m_copy_count);
      int length = copy_prefix.size();
      return this->m_label.substr(this->m_copy_count, this->m_label.size()- length);
    }
}

/**
 * Synopsis: Get the size of out-list of a DAGNode
 * 
 * Description: Simply returns the size of out-list
 * 
 * Return value: integer >=0
 *
 * See Also: getOutListBeginIterator, getOutListEndIterator
 */
int t_DAGNode ::getSizeOfOutList()
{
  return this->m_out_list.size();
}




/**
 *Synopsis: Get begin iterator on the out-list
 *
 *
 * Description: The purpose of this function is to allow the client to traverse the out-list using iterator. Client may traverse the entire DAG using these iterators.
 *
 * Return value: Valid begin iterator on the out-list if the node has non-empty out-list
 *
 * See Also: getSizeOfOutList, getOutListEndIterator
 **/

list <t_DAGNode *> :: iterator t_DAGNode :: getOutListBeginIterator()
{
  list <t_DAGNode *> ::iterator it;
  if(this->m_out_list.size()!=0)
    it = this->m_out_list.begin();
  return it;
}



/**
 *Synopsis: Get end iterator on the out-list
 *
 * Description: The purpose of this function is to allow the client to traverse the out-list using iterator. Client may traverse the entire DAG using these iterators.
 *
 * Return value: Valid end iterator on the out-list if the node has non-empty out-list
 *
 * See Also: getSizeOfOutList, getOutListBeginIterator
 **/

list <t_DAGNode *> :: iterator t_DAGNode :: getOutListEndIterator()
{
  list <t_DAGNode *> ::iterator it;
  if(this->m_out_list.size()!=0)
    it = this->m_out_list.end();
  return it;
}

bool t_DAGNode::assignVisitedFlagRecursively(bool value, t_HashTable<unsigned long long, bool> &visitedSet)
{
    assignVisitedFlag(value);
    visitedSet.hashtable_insert(this->m_node_id, true);
    list<t_DAGNode*>::iterator it = getOutListBeginIterator(),
            last = getOutListEndIterator();
    for(; it!= last; ++it)
    {
        if( visitedSet.hashtable_search( (*it)->m_node_id).success() == false)
            (*it)->assignVisitedFlagRecursively(value, visitedSet);
    }
}

/**
 *Synopsis: Get The Value associated with a node
 *
 * Description: Allows the client to read the custom value associated with the node
 *
 * Return value: A void * to the value field of the DAGNode, NULL if none exists
 *
 *See Also: setValue
 */
 
void * t_DAGNode :: getValue()
{
  return m_value;
}


/**
 *Synopsis: Set The Value associated with a node
 *
 * Description: Allows the client to write the custom value associated with the node
 *
 * Return value: A bool indicating success or failure of the operation
 *
 *See Also: setValue
 */

bool t_DAGNode ::setValue(void *value_to_set)
{
  m_value = value_to_set;
  return true;
}



/**
 * Synopsis: Set the value of visited flag of a node
 *
 * Description: Use this function in external traversals to set/reset the visited flag of the node
 *
 * Return Value: true if it succeeds. false otherwise
 *
 * See Also: DAGManager::ResetVisitedFlagOfDAGNodes
 */
bool t_DAGNode :: assignVisitedFlag(bool assign_to)
{
  m_visited = assign_to;
}



/**
 * Synopsis: Test if the DAG represented by this node is copied one .
 *
 * Description: Tests the copy_count of the node and determines if the DAG represented by this node is copied or not
 *
 * Return Value: A boolean true or false.
 */
bool t_DAGNode :: isCopied()
{
  return (m_copy_count == 0)?false : true;
}




bool t_DAGNode ::isNodeVisited()
{
  return this->m_visited;
}
bool t_DAGNode ::isNodeShared()
{
  return this->m_isShared;
}


void t_DAGNode ::assignTempID(int id)
{
  m_temp_id = id;
}
int t_DAGNode ::getTempID()
{
  return this->m_temp_id;
}



int t_DAGNode :: getNodeID()
{
return this->m_node_id;
}


bool t_DAGNode ::addAbstractSignature(string label_to_add)
{
  this->m_abstract_signatures.insert(label_to_add);
}
set<string> t_DAGNode :: getAbstractSignatures()
{
  return this->m_abstract_signatures;
}

bool t_DAGNode:: isPresentAsAbstractSignature(string label_to_test)
{
  set<string> :: iterator it;
  
  for(it = m_abstract_signatures.begin();it!=m_abstract_signatures.end();it++)
    {
      if(*it == label_to_test)
	return true;
    }
  return false;
}


bool t_DAGNode :: setDataType(int type_to_set)
{
  this->m_data_type = type_to_set;
}
  int t_DAGNode :: getDataType()
  {
    return this->m_data_type;
  }
//Get iterator to the begining of inlist
list <t_DAGNode *> :: iterator t_DAGNode :: getInListBeginIterator()
  {
    list <t_DAGNode *> ::iterator it;
    if(this->m_in_list.size()!=0)
      it = this->m_in_list.begin();
    return it;
  }

//Get iterator to the end of inlist
list <t_DAGNode *> :: iterator t_DAGNode :: getInListEndIterator()
{
  list <t_DAGNode *> ::iterator it;
  if(this->m_in_list.size()!=0)
    it = this->m_in_list.end();
  return it;
}

int t_DAGNode :: getInListSize()
{
  return this->m_in_list.size();
}
