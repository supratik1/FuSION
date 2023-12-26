#include "DAGSimplifier.h"
#include "DagManager.h"
#include <map>
#include <ostream>
using namespace std;

t_DAGNode* t_DAGSimplifier::defaultCallBack(const std::string& label, const vector<t_DAGNode*>& children)
{
    t_DAGManager::getSingleton()->createNode(label, children, NULL, false, false);
}
    
/**
 * Synopsis: [constructor for expression]
 *
 * Description : [This is constructor for the RewriteRuleExpression class, used for reading lhs/rhs of a rewrite rule]
 *
 * Arguments :[See inline]
 *
 * Return Value :[] 
 *
 * Side Effects : [None]
 */
t_RewriteRuleExpression ::  t_RewriteRuleExpression(
			  string op_name_to_set, //This is possibly e_1, e_2, b_1, ... or ite, bvneg, etc
			  int type_of_expression_to_set, 
			  vector<t_RewriteRuleExpression *> arguments_to_set, //Arguments of non-leaf expression
			  int expression_index_to_set, // 1 of e_1 ...
			  string valid_substring_to_set // valid substring corresponding to a node in expression, for each of leaf and non-leaf node
			  )
{
  this->m_expression_index = expression_index_to_set;
  this->m_valid_substring = valid_substring_to_set;
  this->m_op_name = op_name_to_set;
  this->m_type_of_expr = type_of_expression_to_set;
  for(int i=0;i<arguments_to_set.size();i++)
    {
      this->m_arguments.push_back(arguments_to_set[i]);
    }
}

/**
 * Synopsis: [constructor for expression]
 *
 * Description : [This is constructor for the RewriteRuleExpression class, used for reading lhs/rhs of a rewrite rule]
 *
 * Arguments :[See inline]
 *
 * Return Value :[] 
 *
 * Side Effects : [None]
 */
t_RewriteRuleExpression :: t_RewriteRuleExpression(
			 string op_name_to_set,//This is possibly e_1, e_2, b_1, ... or ite, bvneg, etc
			 int type_of_expression_to_set, 
			 int expression_index_to_set, // 1 of e_1 ...
			 string valid_substring_to_set  // valid substring corresponding to a node in expression, for each of leaf and non-leaf node
			 )
{
  this->m_expression_index = expression_index_to_set;
  this->m_valid_substring = valid_substring_to_set;
  this->m_op_name = op_name_to_set;
  this->m_type_of_expr = type_of_expression_to_set;

}
/**
 * Synopsis: [Returns valid-substring corresponding to a expression node]
 *
 * Description : [The valid substring corresponding to each node (leaf or non-leaf) is returned by this function. This string is generated at the time of construction of the expression]
 *
 * Arguments :[None]
 *
 * Return Value :[String] 
 *
 * Side Effects : [None]
 */
string t_RewriteRuleExpression :: getValidSubstringOfRewriteRuleExpression()
{
  return this->m_valid_substring;
}

/**
 * Synopsis: [Checks if RewriteRuleExpression is a leaf ]
 *
 * Description : [Checks if RewriteRuleExpression is a leaf]
 *
 * Arguments :[None]
 *
 * Return Value :[True/False if the expression is leaf/Non-leaf] 
 *
 * Side Effects : [None]
 */
bool t_RewriteRuleExpression :: isLeafRewriteRuleExpression()
{
  if(this->m_arguments.size()==0)
    return true;
  return false;
}
  
/**
 * Synopsis: [Print expression]
 *
 * Description : [This function simply prints the RewriteRuleExpression by recursively traversing it.]
 *
 * Arguments :[Ofstream * to the file to which the expression is to be printed]
 *
 * Return Value :[None!] 
 *
 * Side Effects : [None!]
 */
void t_RewriteRuleExpression :: printRewriteRuleExpression(ofstream *outfile)
{
  *outfile<<this->m_op_name;
  if(m_arguments.size()!=0)
    *outfile<<"(";
  for(int i=0;i<m_arguments.size();i++)
    {
      this->m_arguments[i]->printRewriteRuleExpression(outfile);
      if(i!= m_arguments.size()-1)
	*outfile<<",";
    }
  if(m_arguments.size()!=0)
    *outfile<<")";
}


/**
 * Synopsis: [Print a rewrite rule]
 *
 * Description : [Prints a rewrite rule to a file. This means it will call printRewriteRuleExpression on both lhs and rhs of the rule.]
 *
 * Arguments :[ofstream * to the file to which the rule is to be printed]
 *
 * Return Value :[None] 
 *
 * Side Effects : [None]
 */
void t_RewriteRule ::  printRewriteRule(ofstream *outfile)
{
  *outfile<<endl<<"-----------------------------------"<<endl;
  *outfile<<"LHS IS : ";
  lhs->printRewriteRuleExpression(outfile);
  *outfile<<endl;
  *outfile<<"RHS IS : ";
  rhs->printRewriteRuleExpression(outfile);
  *outfile<<endl<<"-----------------------------------"<<endl;
}

/**
 * Synopsis: [Destructor for the DAGSimplifier]
 *
 * Description : [Releases the resources held by DAGSimlifier, and destructs the object]
 *
 * Arguments :[None]
 *
 * Return Value :[None] 
 *
 * Side Effects : [None]
 */
t_DAGSimplifier :: ~t_DAGSimplifier()
{
  m_dag_simplifier_log->close();
}

/**
 * Synopsis: [Constructor for the DAGSimplifier]
 *
 * Description : [Initializes the resources associated with the DAGSimplifier. Opens log files. Constructs DAGSimplifier object ]
 *
 * Arguments :[DAGManager * to the DAGManager containing the DAG to operate on]
 *
 * Return Value :[None] 
 *
 * Side Effects : [None]
 */

t_DAGSimplifier :: t_DAGSimplifier(t_DAGManager *dag_manager_to_use)
{
  this->m_dag_manager = dag_manager_to_use;
  m_dag_simplifier_log = new ofstream();

  const string sDagSimplifierLogFileName = 
      	t_ConfigurationOptions::getConfig()->m_sLogDir + "/DAGSimplifier.log";
  m_dag_simplifier_log->open(sDagSimplifierLogFileName.c_str());
  m_logManager = t_Logger::getInstance();
  m_callBack = &t_DAGSimplifier::defaultCallBack;
}



/**
 * Synopsis: [Adds a valid substring to ValidSubstrings set]
 *
 * Description : [This function adds a string to the set of ValidSubstrings maintained by the DAGSimplifier. Note that ValidSubstrings belong only to LHS of rule
 and add them only when a RewriteRuleExpression node is created when reading the LHS of the rewrite rule]
 *
 * Arguments :[string valid_substring_to_add]
 *
 * Return Value :[true/false indicating success or failure] 
 *
 * Side Effects : [None]
 */
bool t_DAGSimplifier :: addValidSubstring(string valid_substring_to_add)
{
  m_logManager->LOG("Adding validsubstring "+valid_substring_to_add+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  this->m_ValidSubstrings.insert(valid_substring_to_add);
  return true;
}
/**
 * Synopsis: [Adds a valid string corresponding to LHS of a rule to ValidStringsOfRules set]
 *
 * Description : [This function adds a string to the set of ValidStringsOfRules maintained by the DAGSimplifier. Note that ValidStringsOfRules belong only to LHS of rule and add them only when a Entire LHS is created when reading the LHS of the rewrite rule]
 *
 * Arguments :[string valid_substring_to_add]
 *
 * Return Value :[true/false indicating success or failure] 
 *
 * Side Effects : [None]
 */
bool t_DAGSimplifier :: addValidStringOfLHSOfARule(string valid_string_to_add)
{
  m_logManager->LOG("Adding ValidString of LHS "+valid_string_to_add+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  this->m_ValidStringsOfRules.insert(valid_string_to_add);
  return true;
}

/**
 * Synopsis: [Adds a rewrite rule to DAGSimplifier]
 *
 * Description : [Add rewrite rule to DAGSimplifier's setOfRewriteRules]
 *
 * Arguments :[RewriteRule * to rule to be added]
 *
 * Return Value :[true/false indicating success or failure] 
 *
 * Side Effects : [None]
 */
bool t_DAGSimplifier :: addARewriteRule(t_RewriteRule *rule_to_add)
{
  m_logManager->LOG("Adding a rewrite rule \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  //rule_to_add->printRewriteRule(dag_simplifier_log);
  m_logManager->LOG("Adding rule to setOfRewriteRules\n",m_dag_simplifier_log,c_DebugLevelVerbosity);  
  this->m_setOfRewriteRules.push_back(rule_to_add);
  m_logManager->LOG("Adding rule to ValidStringsOfRules with ValidString= " +rule_to_add->getValidStringOfLHSOfRule()+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);

  /** Update on 27-10-2010@rajkumar */
  string operatorName = rule_to_add->getLHSRewriteRuleOperator();
  vector<t_RewriteRule*> current_operator_rules = this->m_MapOfRewriteRulesBasedOnOperators[operatorName];
  if( current_operator_rules.empty())   // No rules were present for this operator
  {
      vector<t_RewriteRule* > newRulesVector;
      newRulesVector.push_back(rule_to_add);
      this->m_MapOfRewriteRulesBasedOnOperators[operatorName]=newRulesVector;
  }
  else  // some rules are present, add new rule in the order of priority.
  {
      bool inserted=false;
      vector<t_RewriteRule*>::iterator rewriteRuleIterator = current_operator_rules.begin();
      while( !inserted )
      {
          if( rewriteRuleIterator == current_operator_rules.end() )
          {
              current_operator_rules.push_back(rule_to_add);
              inserted=true;
              break;
          }
          else if( (*rewriteRuleIterator)->m_priority <= rule_to_add->m_priority )
          {
              current_operator_rules.insert(rewriteRuleIterator,rule_to_add);
              inserted=true;
              break;
          }
          else
          {
              rewriteRuleIterator++;
          }
      }
      this->m_MapOfRewriteRulesBasedOnOperators[operatorName]=current_operator_rules;
  }
  m_logManager->LOG("New Rule added into the map",m_dag_simplifier_log,c_DebugLevelVerbosity);
  
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
bool t_DAGSimplifier :: checkIfValidSubstring(string substring_to_test_for_validity)
{
  set<string> :: iterator it;
  for (it =this->m_ValidSubstrings.begin();it!=this->m_ValidSubstrings.end();it++)
    {
      if(*it==substring_to_test_for_validity)
	return true;
    }
  return false;
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
bool t_DAGSimplifier :: checkIfValidStringOfLHSOfARule(string string_to_test_for_validity)
{
  m_logManager->LOG("Checking if string is ValiStringOfLHSOfARule for : "+string_to_test_for_validity+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  set<string> :: iterator it;
  for (it =this->m_ValidStringsOfRules.begin();it!=this->m_ValidStringsOfRules.end();it++)
    {
      if(*it==string_to_test_for_validity)
	{
	  m_logManager->LOG(string_to_test_for_validity+" is a valid LHS of a rule \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
	  return true;
	}
    }
  m_logManager->LOG(string_to_test_for_validity+" is NOT a valid LHS of a rule \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  return false;
}




/**
 * Synopsis: [Prints all Valid Substrings in DAGSimplifier]
 *
 * Description : []
 *
 * Arguments :[]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier :: printAllValidSubStrings(ofstream *outfile)
{
  set<string> :: iterator it;
  *outfile<<"Set of valid Substrings is ..\n";
  for (it =this->m_ValidSubstrings.begin();it!=this->m_ValidSubstrings.end();it++)
    {
      *outfile<<*it;
      *outfile<<endl;
    }
  *outfile<<endl;
  *outfile<<"END Set of Valid Substrings\n";
    
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
bool t_DAGSimplifier :: printAllValidStringsOfLHSOfRules(ofstream *outfile)
{
  set<string> :: iterator it;
  *outfile<<"Set of strings of all rules is ..\n";
  for (it =this->m_ValidStringsOfRules.begin();it!=this->m_ValidStringsOfRules.end();it++)
    {
      *outfile<<*it;
      *outfile<<endl;
    }
  *outfile<<endl;
  *outfile<<"END Set of strings of rules\n";

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
bool t_DAGSimplifier :: printAllRewriteRules(ofstream *outfile)
{
  vector<t_RewriteRule *>::iterator it;
  *outfile<<"Printing the rewrite rules \n";
  for(it= this->m_setOfRewriteRules.begin();it!=this->m_setOfRewriteRules.end();it++)
    {
      t_RewriteRule *rule_to_print = *it;
      rule_to_print->printRewriteRule(outfile);
    }
  *outfile<<"Finished printing all rewrite rules \n";
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
bool t_DAGSimplifier :: printSortedRules(ofstream *outfile)
{
  vector<t_RewriteRule *>::iterator it;
  *outfile<<"Printing the rewrite rules \n";
  for(it= this->m_sortedSetOfRewriteRules.begin();it!=this->m_sortedSetOfRewriteRules.end();it++)
    {
      t_RewriteRule *rule_to_print = *it;
      rule_to_print->printRewriteRule(outfile);
    }
  *outfile<<"Finished printing all rewrite rules \n";
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
vector<t_RewriteRuleExpression *> assign_arguments_of_simplification_expressions(
								    class t_VectorOfRewriteRuleExpressions *v_exprs_in
								    )
{
  vector<t_RewriteRuleExpression *>v_exprs;
  for(int i=0;i<v_exprs_in->m_v_arguments.size();i++)
    {
      v_exprs.push_back(v_exprs_in->m_v_arguments[i]);
    }

  return v_exprs;
}



//this function does a postorder traversal of the DAG to add the labels.
//note that the postorder traversal is mandatory!
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
bool t_DAGSimplifier :: initializeAbstractRuleSignaturesOfDAG(t_DAGNode *root)
{
  m_logManager->LOG("Initializing DAG Labels\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  set<string> current_labels = root->getAbstractSignatures();
  if(current_labels.size() != 0)
    {
      m_logManager->LOG("DAGNode has non-zero number of labels\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
      return true;
    }
  
  list <t_DAGNode *> :: iterator itbegin = root->getOutListBeginIterator();
  list <t_DAGNode *> :: iterator itend = root->getOutListEndIterator();
  for(list <t_DAGNode *> :: iterator it=itbegin; it!=itend; it++)
    {
      t_DAGNode *child_i = (*it);
      bool init_labels = initializeAbstractRuleSignaturesOfDAG(*it);
      if(!init_labels)
	{
	  m_logManager->LOG("Failed initializing the labels\n",m_dag_simplifier_log,c_RunLevelVerbosity);
	  return false;
	}
    }
  bool add_lables_to_this_node = addAbstractRuleSignaturesToADAGNode(root);
  if(!add_lables_to_this_node)
    {
      m_logManager->LOG("Failed initializing the label of this node\n",m_dag_simplifier_log,c_RunLevelVerbosity);
      return false;
    }
  return true;
}


//This function correctly adds labels to all nodes in the DAG
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
bool t_DAGSimplifier :: addAbstractRuleSignaturesToADAGNode(t_DAGNode *node)
{
  set<string> labels;
  if(node->getSizeOfOutList() == 0)
    {
      
      string label_of_node = node->getLabel();
      labels.insert(label_of_node);
      m_logManager->LOG("Adding label e to DAGNode \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
      labels.insert("e");
    
    }
  else
    {
   
      vector<set<string> > v;
      node->addAbstractSignature("e");
      labels.insert(node->getLabel());
      m_logManager->LOG("Adding labels to non-leaf node\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
      list <t_DAGNode *> :: iterator itbegin = node->getOutListBeginIterator();
      list <t_DAGNode *> :: iterator itend = node->getOutListEndIterator();
      m_logManager->LOG("Size of out list is "+toString(node->getSizeOfOutList())+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
      for(list <t_DAGNode *> :: iterator it=itbegin; it!=itend; it++)
	{
	  set<string> simplification_labels = (*it)->getAbstractSignatures();	
	  v.push_back(simplification_labels);
	}
      int i=0;


      while(i<v.size())
	{
	  set<string> current_set = v[i];
	  set<string> new_labels;
	  for(set<string> :: iterator it= labels.begin(); it!= labels.end();it++)
	    {
	      for(set<string> :: iterator it2= current_set.begin(); it2!= current_set.end();it2++)
		{
		  new_labels.insert(*it + "," +*it2);
		}
	    }
	  labels.clear();
	  labels = new_labels;
	  i++;
	}
    }

  /*** /
  m_logManager->LOG("Labels generated are \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  for(set <string> :: iterator it=labels.begin(); it!=labels.end(); it++)
    {
      m_logManager->LOG(*it+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
    }
  m_logManager->LOG("***\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  **/
  for(set <string> :: iterator it=labels.begin(); it!=labels.end(); it++)
    {
      //if the string that is generated is a VSS, then add it to the set of labels on that node
      if(checkIfValidSubstring(*it))
      	{
	  m_logManager->LOG("Adding ValidSUBString for simplification "+*it+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
	  node->addAbstractSignature(*it);
	}
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
bool t_DAGSimplifier :: checkIfNodeHasAAbstractRuleSignature(t_DAGNode *node, string simpl_label_to_test)
{
  set<string> simplification_labels_of_node =  node->getAbstractSignatures();
  set<string> :: iterator it;
  for(it = simplification_labels_of_node.begin(); it!= simplification_labels_of_node.end(); it++)
    {
      if((*it) == simpl_label_to_test)
	{
	  return true;
	}
    }
  return false;
}


/**
 * Synopsis: []
 *
 * Description : [
 * This is a 3 step function
 * 1. traverse the DAG in DFS
 * 2. See if the simplification_labels on this node contain any string that belongs to lhs of a rule
 * 3. if the string that is generated is a Valid LHS of a rule, then add this node to list of matched nodes on the corresponding rule

]
 *
 * Arguments :[]
 *
 * Return Value :[] 
 *
 * Side Effects : []
 */
bool t_DAGSimplifier ::addDAGNodesMatchingThisRuleToSetOfMatchedNodes(t_DAGNode * node, t_RewriteRule *rule, set<t_DAGNode *> &NodesMatched,bool recursionOnChildren)
{
  m_logManager->LOG("---------------------------------------------------------------\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  set<string> abstract_signatures_of_current_node =
    node->getAbstractSignatures();

  string valid_string_of_lhs_of_current_rule = 
    rule->getValidStringOfLHSOfRule();
  bool simplificationRuleFound = false;
  for(set <string> :: iterator it=abstract_signatures_of_current_node.begin(); it!=abstract_signatures_of_current_node.end(); it++)
    {
      if( (*it) == valid_string_of_lhs_of_current_rule)
	{ 
	  t_RewriteRuleExpression *lhs_of_rule = rule->lhs;
	  m_logManager->LOG("Adding DAGNode to SET of matched nodes \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
	  NodesMatched.insert(node);
          simplificationRuleFound =true;
          break;
	}
      else
	{
	  //this label does not belong to lhs of any rule in the system
	  //Do nothing!!
	}
    }
    if (recursionOnChildren)
    {
        //Go recursively
        list <t_DAGNode *> ::iterator itbegin = node->getOutListBeginIterator();
        list <t_DAGNode *> ::iterator itend = node->getOutListEndIterator();
        for (list <t_DAGNode *> ::iterator it = itbegin; it != itend; it++)
        {
            bool repeat_rec = addDAGNodesMatchingThisRuleToSetOfMatchedNodes((*it), rule, NodesMatched,true);
            if (!repeat_rec)
            {
                m_logManager->LOG("ERROR!! Could not traverse the DAG recursively for adding nodes to rules list\n", m_dag_simplifier_log, c_RunLevelVerbosity);
                return false;
            }
        }
    }
    //m_logManager->LOG("---------------------------------------------------------------\n", m_dag_simplifier_log, c_DebugLevelVerbosity);
    return simplificationRuleFound;
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
bool t_DAGSimplifier :: sortTheRewriteRulesAsPerPriority()
{
  int maxpriority = 0;
  for(int i=0;i<this->m_setOfRewriteRules.size(); i++)
    {
      t_RewriteRule *rule = this->m_setOfRewriteRules[i];
      if(rule->m_priority > maxpriority)
	maxpriority = rule->m_priority;
    }
  m_logManager->LOG("Max Priority is "+toString(maxpriority)+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  //this finds the rule with the maximum priority
  int current_prio = 0;
  while(current_prio!=maxpriority)
    {
      current_prio ++;
      m_logManager->LOG("Current Priority "+toString(current_prio)+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
      for(int j=0;j<this->m_setOfRewriteRules.size();j++)
	{
	  if(m_setOfRewriteRules[j]->m_priority == current_prio)
	    {
	      m_logManager->LOG("Adding Rule \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
	      this->m_sortedSetOfRewriteRules.push_back(m_setOfRewriteRules[j]);
	    }
	}
    }
  return true;
}

/*** /
bool t_DAGSimplifier :: sortTheRewriteRulesMapAsPerPriority()
{
    map<string, vector<t_RewriteRule*> >::iterator rewriteMapIterator = this->m_MapOfRewriteRulesBasedOnOperators.begin();
    while( rewriteMapIterator != m_MapOfRewriteRulesBasedOnOperators.end())
    {
        m_logManager->LOG(" Sorting rewrite rules of operator:"+ (*rewriteMapIterator).first ,m_dag_simplifier_log ,c_DebugLevelVerbosity);
        vector<t_RewriteRule*> currentOperatorRewriteRules = (*rewriteMapIterator).second;
        // Getting max priority
        int maxpriority = 0;
        vector<t_RewriteRule*>::iterator rewriteRuleIteratorOuter = currentOperatorRewriteRules.begin();
        while(rewriteRuleIteratorOuter != currentOperatorRewriteRules.end())
        {
            vector<t_RewriteRule*>::iterator rewriteRuleIteratorInner = currentOperatorRewriteRules.begin();
            while( rewriteRuleIteratorInner != currentOperatorRewriteRules.end())
            {
                t_RewriteRule *ruleI = (*rewriteRuleIteratorOuter);
                t_RewriteRule *ruleJ = (*rewriteRuleIteratorInner);
                if(ruleI->m_priority > ruleJ->m_priority)
                {
                    maxpriority = ruleI->m_priority;
                }
                rewriteRuleIteratorInner++;
            }
            rewriteRuleIteratorOuter++;
        }

        
        rewriteMapIterator++;
    }
  
  m_logManager->LOG("Max Priority is "+toString(maxpriority)+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
  //this finds the rule with the maximum priority
  int current_prio = 0;
  while(current_prio!=maxpriority)
    {
      current_prio ++;
      m_logManager->LOG("Current Priority "+toString(current_prio)+"\n",m_dag_simplifier_log,c_DebugLevelVerbosity);
      for(int j=0;j<this->m_setOfRewriteRules.size();j++)
	{
	  if(m_setOfRewriteRules[j]->m_priority == current_prio)
	    {
	      m_logManager->LOG("Adding Rule \n",m_dag_simplifier_log,c_DebugLevelVerbosity);
	      this->m_sortedSetOfRewriteRules.push_back(m_setOfRewriteRules[j]);
	    }
	}
    }
  return true;
}
 * **/
//bool t_DAGSimplifier::sortRewriteRulesOfOneOperator()


bool t_DAGSimplifier :: checkSetsOfStringsForEquality(set<string> set1, set<string> set2)
{
  set<string> :: iterator it1;
  set<string> :: iterator it2;
  if(set1.size() != set2.size())
    return false;
  for(it1= set1.begin(), it2=set2.begin(); it1!=set1.end() && it2!=set2.end(); it1++, it2++)
    {
      if((*it1) != (*it2))
	return false;
    }
  return true;
}


bool t_DAGSimplifier :: clearAbstractRuleSignaturesOfDAG(t_DAGNode *root)
{
  root->clearAbstractSignatures();
  list<t_DAGNode *> :: iterator beginit = root->getOutListBeginIterator();
  list<t_DAGNode *> :: iterator endit = root->getOutListEndIterator();
  for(list<t_DAGNode *> :: iterator it = beginit; it!=endit; it++)
    {
      this->clearAbstractRuleSignaturesOfDAG(*it);
    }
} /***
bool t_DAGSimplifier::m_logManager->LOG(string message)
{
  if(m_dag_simplifier_log == NULL)
    {
      return false;
    }
  #ifdef DEBUG_MODE
  *m_dag_simplifier_log<<message;
  #endif
  return true;
}
**/
