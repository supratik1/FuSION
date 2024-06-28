#ifndef EXPRESSIONASDAG_H
#define EXPRESSIONASDAG_H
#include "ExpressionValue.h"
#include "InferTypeOfExpression.h"
//#include "DagManager.h"
#include "helping_functions.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

class t_Expression;

extern   map <string,t_InferTypeOfExpression *> mapOfTypeInferenceRules;

class t_ExpressionAsDag
{
 private:
  t_DAGNode *m_dag; //Dag representation of the Expression

  // Constructor for creating dag Expression  (Symbol or Constant)
  t_ExpressionAsDag(
		  string &label,
		  TypeOfExpressionTuple &typeInfo,
		  t_TypeOfValueStoredInExpressionLabel &labelType,
		  t_Expression *expressionNode,
		  t_DAGManager *&dagmanager
		  );
 
     
  //Constructor for creating dag experssion (with sub-expressions already created)
  t_ExpressionAsDag(
		  string &label,
		  TypeOfExpressionTuple &typeInfo,
                  t_TypeOfValueStoredInExpressionLabel &labelType,
		  vector<t_ExpressionAsDag *> &operandExpressions,
		  t_Expression *expressionNode,
                  t_DAGManager *&dagmanager
		 );
      
   
  //Get type tuple(Type and width) of Expression
  TypeOfExpressionTuple *getTypeOfExpression();
    
  /**
   * TODO
   * printToFile
   * ReadFromFile
   */

  /**
   * Destructor
   */
  ~t_ExpressionAsDag();
     
  /**
   * Function     :   getSizeOfExpression
   * Returns      :   Number of nodes present in the Dag Expression
   * Description  :   Calls number of nodes in dag to get sizeq
   */
  int getSizeOfExpression(t_DAGManager *&dagmanager);

  // check two dag expressions are pointing to the same dag node (Structure of expression is same?)
  bool checkStructuralEquality(
			       t_ExpressionAsDag *expr2,
			       t_DAGManager *&dagmanger
			      );
    

  //Getting all  leaf nodes of the expression
  vector<t_Expression *> getLeafExpressions(t_DAGManager *&dagmanager);

  //Set value of a expression node (dagnode)
  bool setValue(
		t_ExpressionValue *value,
		t_DAGManager *&dagmanager
	       );

  //Remove a dag expression
  bool removeDagExpression(
			   t_DAGManager *&dagmanger
			  );




  //print Expression into a file
  bool printExpressionToFile(
			     ofstream *outfile,
			     t_DAGManager *&dagmanager
			    ); 

  //print ExpressionAsDAG recursively
  bool printExpressionToFileRecursively(
					t_DAGNode *dagnode,
					bool isRoot,
					ofstream *outfile
				       );



//print shared nodes for queue of shared nodes that already built
bool printSharedNodesFromQueue(
			       ofstream *outfile, 
			       vector<t_DAGNode *> &shared_nodes_queue
			      );

 /* 
  //read Expression from the file and build back the dag expressions
  bool readExpressionsFromFile();
  */

  //print Expression into a constraint format
  bool printExpressionAsYicesExpression(
					string &expression_of_symbol,
					ofstream *fout,t_DAGManager *&dagmanger
				       );
  
  //Build a queue for printing the shared nodes while printing in constraint format
  bool BuildSharedNodesQforPrinting(
				    t_DAGNode *node,
				    int &temp_id_counter, 
				    vector<t_DAGNode *> &shared_nodes_queue
				   );
  
  //print (def .. ) i.e., variable in constraint format  (yices format)
  bool printDefineAVariable(t_DAGNode *current_dag_node, string &label_of_node, bool isCommonSubExpression);
   
 
  string formPrintString(t_ExpressionAsDag *dagExpression,string &name);

 //if clear=true then clears all the temporary expression names it will not assign any names.
  bool setTempNames(bool clear,bool temp_prefix, int &TempIdCounter, t_ExpressionAsDag *dagExpression);
  /**
   * Replace dag expression with another dag expression
   */

  bool  replaceLeafOfADAGExpressionByDAGExpression
    (
     t_DAGManager *&dagmanager,
     t_ExpressionAsDag *dagExpressionToReplace,
     t_ExpressionAsDag *dagExpressionReplaceBy
     );

  bool printExpressionToFileAsDAG(t_DAGNode *dagnode, ostream &outfile);
    bool printExpressionToFileAsDAG(t_DAGNode *dagnode, ofstream *outfile);

  bool printExpressionToFileAsTree1(t_DAGNode *dagnode,ofstream *outfile);
  /**
   * Get methods of the expression as dag
   */

  string getLabelOfExpressionNode();
  TypeOfExpressionTuple getTypeOfExpressionTuple();
  t_TypeOfValueStoredInExpressionLabel getValueStoredInExpressionLabel();
  vector<t_Expression *> getVectorOfOperands();
  t_ExpressionValue * getValue();

  inline bool clearVisitedFlagsOfAllDagNodes(t_DAGManager* dag_manager)
  {
      return dag_manager->ResetVisitedFlagOfDAGNodes();
  }

    //Made as friend for accessing the dag expression implementation for Expression class
  friend class t_Expression;
public :
    t_DAGNode* getDag() const {
        return m_dag;
    }


};

#endif /* EXPRESSIONASDAG_H */
