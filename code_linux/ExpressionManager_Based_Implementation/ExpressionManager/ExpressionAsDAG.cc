#include <list>
#include "ExpressionAsDAG.h"
#include "ExpressionValue.h"
#include "Expression.h"
#include "helping_functions.h"
#include "InferTypeOfExpression.h"

#define TEMP_NAME_PREFIX "$_"

using namespace std;

/**
 * Constructor - ExpressionAsDag for Symbol or Constant
 * Parameters - label (string): Label of the symbol or constant
 *              typeInfo (TypeOfExpressionTuple) : type of teh expression and width 
 *              labelType - the value type that stored in expression label
 *              expressionNode - back pointer to Expression to get the expression from the DagNode
 * Description - The new object to which the resources are allocated and the value field of the object is created according to the parameters passed
 *                       and dag node of the current expression is created and assigned the creaed value
 */
t_ExpressionAsDag::t_ExpressionAsDag
(
 string &label,
 TypeOfExpressionTuple &typeInfo,
 t_TypeOfValueStoredInExpressionLabel &labelType,
 t_Expression *expressionNode,
 t_DAGManager *&dagmanager
 )
{
    m_dag = dagmanager->createNode(label);
    t_ExpressionValue *valueInExpression = new t_ExpressionValue();
    valueInExpression->m_labelType = labelType;
    valueInExpression->m_typeInfo = typeInfo;
    valueInExpression->m_backPointerToExpression = expressionNode;
    m_dag->setValue(valueInExpression);
}

/**
 * Constructor - ExpressionAsDag for Operator
 * Parameters - label (string): Label of the symbol or constant
 *              typeInfo (TypeOfExpressionTuple) : type of teh expression and width 
 *              labelType - the value type that stored in expression label
 *              expressionNode - back pointer to Expression to get the expression from the DagNode
 * Description - The new object to which the resources are allocated and the value field of the object is created according to the parameters passed
 *                       and dag node of the current expression is created and assigned the creaed value
 *                       the type of the value field which is inferred in the expression manager is assigned
 */
t_ExpressionAsDag::t_ExpressionAsDag
(
 string &label,
 TypeOfExpressionTuple &typeInfo,
 t_TypeOfValueStoredInExpressionLabel &labelType,
 vector<t_ExpressionAsDag *>& operandExpressions,
 t_Expression *expressionNode,
 t_DAGManager *&dagmanager)
{
    int i;
    vector<t_DAGNode *> operandDags;
    for (vector<t_ExpressionAsDag *>::iterator it = operandExpressions.begin(); it != operandExpressions.end(); it++)
    {
        operandDags.push_back((*it)->m_dag);
    }

    // Fourth argument to createNode is remember_simplified_dag_after_rewrite_and_return_unsimplified_dag_before_rewrite
    // Fifth argument to createNode is recall_simplified_dag_from_last_createNode_call
    t_DAGNode * dag_before_rewrite = dagmanager->createNode(label, operandDags, NULL, true, false);
    t_DAGNode * dag_after_rewrite = dagmanager->createNode(label, operandDags, NULL, false, true);

    m_dag = dag_after_rewrite;
    t_ExpressionValue* value =(t_ExpressionValue*) m_dag->getValue();
    if(value == NULL)
    {
        t_ExpressionValue *valueInExpression = new t_ExpressionValue();
        valueInExpression->m_backPointerToExpression = expressionNode;
        valueInExpression->m_typeInfo = typeInfo;
        valueInExpression->m_labelType = labelType;
        if(typeInfo.m_type==TYPE_MEMORY_ARRAY)
        {
            valueInExpression->m_actualExpressionValueStored.exprvalueMemory=NULL;
        }
        m_dag->setValue(valueInExpression);
    }

    if (dag_before_rewrite != dag_after_rewrite) {
	dag_before_rewrite->setValue((t_ExpressionValue *) (dag_after_rewrite->getValue()));
    }
}

/**
 * Method: getLeafExpressions()
 * Arguments: dagmanager(DAGManager) - Common dagmanger under which the expressions are created
 * Returns : Vector of the leaf expression
 * Description : Call the and get leaf dag nodes and get the corresponding expression and push into vector
 */
vector<t_Expression *> t_ExpressionAsDag::getLeafExpressions(t_DAGManager *&dagmanager)
{
    vector <t_DAGNode *> leafDAGNodes = dagmanager->getVectorOfLeafNodes(m_dag);
    vector <t_Expression *> leafExpressions;
    for (vector<t_DAGNode *>::iterator it=leafDAGNodes.begin(); it != leafDAGNodes.end(); it++)
    {
        t_ExpressionValue *exprValue = (t_ExpressionValue *)(*it)->getValue();
        leafExpressions.push_back(exprValue->m_backPointerToExpression);
    }
    return leafExpressions;
}

/**
 * Method - getTypeOfExpression()
 * Arguments: No
 * Returns : A reference to an Object of TypeOfExpressionTuple with same value of the current expression objects TypeOfExpressionTuple values
 * Description : Get Type Tuple of the current expression
 */
TypeOfExpressionTuple *t_ExpressionAsDag::getTypeOfExpression()
{
    t_ExpressionValue *valueInDag = (t_ExpressionValue *) m_dag->getValue();
    TypeOfExpressionTuple *typeTuple = new TypeOfExpressionTuple();
    typeTuple->m_type = (valueInDag->m_typeInfo).m_type;
    typeTuple->m_width = (valueInDag->m_typeInfo).m_width;
    return typeTuple;
}

/**
 * Method : setValue()
 * Arguemnts : value (ExpressionValue *)   The value which is to be assigned to the current object
 *                      dagmanager(DAGManager) The common dagmanger which is used while creating the current Expression
 * Returns : true - if value  to the dag  node is set  successfully
 *                false- otherwise
 * Side Effects: It makes the valid flag of the expressions which are above it in the dag to false
 *                         i.e.,Invalidates the values of the dags above the current dag node
 * Descritpion : Set the current dag node the passed value and make the valid flag of dagnodes above it in the dag by traversing the dag
 */
bool t_ExpressionAsDag::setValue(t_ExpressionValue *value, t_DAGManager *&dagmanager)
{
    if (!m_dag->setValue(value))
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * Method: getValue
 * Description : To get the value stored in the current expression node, get the value in the dag node and return it
 * Returns : ExpressionValue*
 */
t_ExpressionValue *t_ExpressionAsDag::getValue()
{
    return (t_ExpressionValue *)this->m_dag->getValue();
}

/**
 * Method : removeDagExpression
 * Arguments: dagmanager(DAGManager ) Common dag manager object with which the expressions are created
 * Description: Calls dagmanagers delete dag so that that current dag expression's dag will be deleted.
 * Returns : true - if currents expressions dag is deleted
 *                false - otherwise
 */
bool t_ExpressionAsDag::removeDagExpression(t_DAGManager *&dagmanager)
{
    if (dagmanager->deleteDAG(m_dag))
    {
        this->~t_ExpressionAsDag();
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Destructor : ~ExpressionAsDag()
 */
t_ExpressionAsDag::~t_ExpressionAsDag()
{
    // cout << "DagExpression is deleted" << endl;
}

/**
 * Function     :   getSizeOfExpression
 * Returns      :   Number of nodes present in the Dag Expression
 * Description  :   Calls number of nodes in dag to get sizeq
 */
int t_ExpressionAsDag::getSizeOfExpression(t_DAGManager *&dagmanager)
{
    return dagmanager->getSizeOfDAG(m_dag);
}

/**
 * Method : checkStructuralEquality
 * Arguments : expr2(ExpressionAsDag) The expressionAsDag object that is to be compared 
 * Description : Checking the structural equality is checking for the dag representation of the current dag and the passed dag (expr2) 
 */
bool t_ExpressionAsDag::checkStructuralEquality(t_ExpressionAsDag *expr2, t_DAGManager *&dagmanager)
{
    if (m_dag == expr2->m_dag) //Based on structure
        return true;
    return false;
}
/**
 *Method : setTempNames()
 *@param  clear (bool ) For clearing the temporary names already assigned to the expressions set this to true other wise make it to false
 *@param  temp_prefix (bool) Whether to use temporary name internal prefix or not?
 *@param  TempIdCounter(int &) A reference to the temporary name counter (if clear flag is true then it is not used so can be anything)
 *@param                       dagExpression (ExpressionAsDag *) The expression for which the temporary names are to be cleared
 * Description : Recursively set the temporary names of the current dag node and the childs under it in the dag according to the clear flag
 */
bool t_ExpressionAsDag::setTempNames(bool clear, bool temp_prefix, int &TempIdCounter, t_ExpressionAsDag *dagExpression)
{
    t_ExpressionValue *eValue = dagExpression->getValue();
    //cout << clear << "\t" << temp_prefix << "\t" << toString(eValue->m_labelType) << endl;
    if (eValue->m_labelType == SYMBOL || eValue->m_labelType == CONSTANT)
    {
        string newName = "";
        if (clear)
        {
            (eValue->m_backPointerToExpression)->setTempName(newName);
        }
        else if (!temp_prefix && ((eValue->m_backPointerToExpression)->getTempName() == ""))
        {
            newName = toString(TempIdCounter++);
            (eValue->m_backPointerToExpression)->setTempName(newName);
        }
        return true;
    }
    else
    {
        string empty_str = "";
        string newName = (temp_prefix) ? TEMP_NAME_PREFIX : empty_str;
        newName = newName + toString(TempIdCounter++);
        (eValue->m_backPointerToExpression)->setTempName(newName);
        list<t_DAGNode *>::iterator beginOutList = (dagExpression->m_dag)->getOutListBeginIterator();
        list<t_DAGNode *>::iterator endOutList = (dagExpression->m_dag)->getOutListEndIterator();
        list<t_DAGNode *>::iterator iter;
        for (iter = beginOutList; iter != endOutList; iter++)
        {
            t_ExpressionValue *value = (t_ExpressionValue *) ((*iter)->getValue());
            setTempNames(clear, temp_prefix, TempIdCounter, (value->m_backPointerToExpression)->m_dagExpression);
            /** * /
            //if (value->m_labelType == OPERATOR)
            {
                if (clear)
                {
                    //(value->m_backPointerToExpression)->setTempName(empty_str);
                    setTempNames(clear, temp_prefix, TempIdCounter, (value->m_backPointerToExpression)->m_dagExpression);
                }
                else if ((value->m_backPointerToExpression)->getTempName() == "")
                {
                    setTempNames(clear, temp_prefix, TempIdCounter, (value->m_backPointerToExpression)->m_dagExpression);
                }
                else
                {
                    //Name is already set for the non leaf expression
                    // Nothing to do
                }
            } /***/
        }

    }
    return true;
}

/**
 *Method -  printExpressionAsYicesExpression
 * Arguments - expression_of_symbol (string) The name of the current expression that is to be printed
 *                       dagmanger (DAGManager) The common dagmanager object with which the current dag expression is created
 *                       fout (ofstream *) File into which the output has to be printed
 * Returns - true  : if successfully printed the expression into file
 *                 false : otherwise
 * Description- Calls teh setTempNames() with clear flag in the 
 */
bool t_ExpressionAsDag::printExpressionAsYicesExpression(string &expression_of_symbol, ofstream *fout, t_DAGManager *&dagmanager)
{
    int tempId = 1;
    //Clearing all the temporary names assigned for nonleaf nodes in the dag
    setTempNames(true,true, tempId, this);
    tempId = 1;
    //start temporary names with $_1 i.e.,"$_"+tmpId
    setTempNames(false,true, tempId, this);
    //Start printing here
    string buffer = formPrintString(this, expression_of_symbol);
    t_InferTypeOfExpression *inferPrint = new t_InferTypeOfExpression();
    string result = inferPrint->makePrintStringIntoActualFormat(buffer);
    (*fout) << result;
    *fout << endl;
    // Printing into constraint format finished successfully
    return true;
}

/**
 * Method - BuildSharedNodesQforPrinting()
 * Arguments - node (DAGNode *) the node for which the shared nodes
 *                     temp_id_counter( int &) the temp id counter for giving a name to the shared node
 *                     shared_nodes_queue (vector<DAGNode *> &)  the nodes that are shared are pushed into the queue
 * Description - If the node is visited donot push into the vector and return
 *                       else increment the temporary name counter and assign visited flag as true
 *                               Check for the dagnodes beneath that dagnode are shared or not and call recursively for each dagnode in the outlist of the current dag
 * Returns - true  if the shared nodes queue is created successfully
 *                false otherwise
 */
bool t_ExpressionAsDag::BuildSharedNodesQforPrinting(t_DAGNode *node, int &temp_id_counter, vector<t_DAGNode *> &shared_nodes_queue)
{
    if ((!(node->isNodeVisited()) && node->isNodeShared()))
    {
        temp_id_counter++;
        node->assignVisitedFlag(true);
        node->assignTempID(temp_id_counter);
        shared_nodes_queue.push_back(node);
    }
    else if (node->isNodeVisited())
    {
        return true;
    }
    node->assignVisitedFlag(true);
    list <t_DAGNode *> ::iterator ol_begin_it = node->getOutListBeginIterator();
    list <t_DAGNode *> ::iterator ol_end_it = node->getOutListEndIterator();

    for (list<t_DAGNode*>::iterator it = ol_begin_it; it != ol_end_it; it++)
    {
        bool assign_ids = BuildSharedNodesQforPrinting(*it, temp_id_counter, shared_nodes_queue);
        if (assign_ids == false)
        {
            return false;
        }
    }
    return true;
}

/**
 * Method - printExpressionToFile()
 * Arguments - outfile (ofstream *) the file into which the expression is to be printed
 *                      dagmanger (DAGManager *) Common dagmanager object which is used while creating of the current expression
 * Description - First mark the shared nodes of the current dag expression and  reset the visited flag of all dagnodes,
                         Build the shared nodes queue and print each expression under this node recursively except those in shared nodes queue for printing the shared nodes only once
                         and then print the shared nodes from the queue.
 *Returns - true if successfull printed the current expression into the file
 *                 false otherwise
 */
bool t_ExpressionAsDag::printExpressionToFile(ofstream *outfile, t_DAGManager *&dagmanager)
{
    //cout << "Printing the Expression (ExpressionAsDAG) \n";
    //markSharedNodes has no precondition

    int mark_shared_nodes_count = dagmanager->markSharedNodes(this->m_dag);
    if (mark_shared_nodes_count == -1)
    {
        //cout << "ERROR!! Failed marking shared nodes in ExpressionAsDAG\n";
        return false;
    }

    // invoke ResetVisitedFlagOfDAG
    bool reset_visited = dagmanager->ResetVisitedFlagOfDAGNodes();
    if (reset_visited == false)
    {
        //cout << "ERROR!! Failed resetting visited flag of DAG Expression\n";
        return false;
    }

    vector<t_DAGNode *> shared_nodes_queue;
    int temp_id_counter = 1;
    bool assign_temp_ids = BuildSharedNodesQforPrinting(this->m_dag, temp_id_counter, shared_nodes_queue);


    *outfile << "root := ";
    bool print_root_expr = printExpressionToFileRecursively(m_dag, true, outfile);

    if (print_root_expr == false)
    {
        //cout << "ERROR!! Failed printing the expression of the Root node of the DAG \n";
        return false;
    }
    *outfile << endl;


    bool print_shared_expressions = printSharedNodesFromQueue(outfile, shared_nodes_queue);
    if (print_shared_expressions == false)
    {
        // cout << "ERROR!! Failed printing shared nodes from queue\n";
        return false;
    }
    //cout << "Successfully printed the DAG\n";
    return true;
}

/**
 * Method - printExpressionToFileRecursively()
 * Arguments - dagnode (DAGNode *) : which is to be printed
 *                      isRoot (bool ) : is this node is a root node?
 *                      outfile (ofstream *) : the file into which the expression is to be printed
 * Returns - true if the expression is printed recursively successfully
 */
bool t_ExpressionAsDag::printExpressionToFileRecursively(t_DAGNode *dagnode, bool isRoot, ofstream *outfile)
{
    if (dagnode == NULL)
        return false;

    if (dagnode->isNodeShared() && !isRoot)
    {
        *outfile << "$" << dagnode->getLabel() << "_" << dagnode->getTempID();
        return true;
    }
    else
    {
        t_ExpressionValue *val = (t_ExpressionValue *) dagnode->getValue();
        TypeOfExpressionTuple typeTuple = val->m_typeInfo;
        *outfile << dagnode->getLabel() << "[" << typeTuple.m_type << "," << typeTuple.m_width << "]";
    }

       /*** /
    t_ExpressionValue *val = (t_ExpressionValue *) dagnode->getValue();
    t_Expression *expr = val->m_backPointerToExpression;
    *outfile<<endl<< "\nLABEL="<<dagnode->getLabel() <<endl
            << "TYPE="<<val->getOnlyTypeOfExpression()<<endl
            << "WIDTH="<<val->getWidth()<<endl
            << "LABEL-TYPE="<<val->m_labelType<<endl
            << "VALID-VALUE="<<expr->m_validValue<<endl
            << "VALUE="<< val->getActualValueAsString()<<endl; /**/
    //Need to print the value of DAGNode (like type,labeltype etc)
    int num_operands = dagnode->getSizeOfOutList();
    if (num_operands > 0)
    {
        *outfile << "(";
        int i = 0;
        list<t_DAGNode *>::iterator it = dagnode->getOutListBeginIterator();
        list<t_DAGNode *>::iterator outlist_end_iterator = dagnode->getOutListEndIterator();
        for (; it != outlist_end_iterator; it++)
        {
            bool printed_child = printExpressionToFileRecursively(*it, false, outfile);
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
 * Method : printSharedNodesFromQueue()
 * Arguments : outfile(ofstream *) - The file into which the expressions are to be printed
 *                      shared_nodes_queue (vector<DAGNode *>) vector of the shared nodes which are to be printed
 * Description: Recursively prints the shared expressions from the vector of shared_nodes_queue
 * Returns: true - if successfullly printing of the shared_nodes_queue is finished
 *               false - otherwise
 */
bool t_ExpressionAsDag::printSharedNodesFromQueue(ofstream *outfile, vector<t_DAGNode *> & shared_nodes_queue)
{
    //cout << "Printing the shared nodes from the queue\n";
    int num_shared_nodes = shared_nodes_queue.size();
    for (int i = 0; i < num_shared_nodes; i++)
    {
        *outfile << "$" << shared_nodes_queue[i]->getLabel() << "_" << shared_nodes_queue[i]->getTempID() << " :=  ";
        bool print_expression = printExpressionToFileRecursively(shared_nodes_queue[i], true, outfile);
        *outfile << endl;
        if (!print_expression)
        {
            //cout << "ERROR!! Failed printing a node from shared nodes queue\n";
            return false;
        }
    }
    //cout << "Successfully Printed the shared nodes from the queue\n";
    return true;
}

string t_ExpressionAsDag::formPrintString(t_ExpressionAsDag *dagExpression, string &name)
{
    string printBufferOfExpression = "";
    map<string, t_LableTypeAndValueforPrinting *> mapOfReplacements;
    vector<TypeOfExpressionTuple *>typeTupleVector;
    t_ExpressionValue *eValue = (t_ExpressionValue *) ((dagExpression->m_dag)->getValue());
    if (eValue->m_labelType == SYMBOL || eValue->m_labelType == CONSTANT)
    {
        //cout<<"Unable to print the expression that doesnot have any operator\n";
        return "";
    }
    else
    {
        list<t_DAGNode *>::iterator beginOutList = (dagExpression->m_dag)->getOutListBeginIterator();
        list<t_DAGNode *>::iterator endOutList = (dagExpression->m_dag)->getOutListEndIterator();
        list<t_DAGNode *>::iterator iter;
        int i = 1;
        for (iter = beginOutList; iter != endOutList; iter++)
        {
            t_ExpressionValue *value = (t_ExpressionValue *) ((*iter)->getValue());
            string label = (*iter)->getLabel();
            if (value->m_labelType == SYMBOL)
            {
                t_LableTypeAndValueforPrinting *ltvp = new t_LableTypeAndValueforPrinting();
                ltvp->label = label;
                ltvp->typeInfo = value->m_typeInfo;
                mapOfReplacements["%label" + toString(i++)] = ltvp;
                //  cout<<"\nPushing into map:"<<(i-1)<<endl;
            }
            else if (value->m_labelType == CONSTANT)
            {
                t_LableTypeAndValueforPrinting *ltvp = new t_LableTypeAndValueforPrinting();
                string labelOfConst = label;
                ltvp->typeInfo = value->m_typeInfo;
                if (value->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
                    value->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
                {
                    string actual_bv = labelOfConst.substr(2, labelOfConst.length() - 2);
                    ltvp->label = actual_bv;
                }
                else
                {
                    ltvp->label = labelOfConst;
                }
                mapOfReplacements["%label" + toString(i++)] = ltvp;
            }
            else
            {
                t_Expression *expr = value->m_backPointerToExpression;
                string name = expr->getTempName();
                printBufferOfExpression += formPrintString(expr->m_dagExpression, name);
                t_LableTypeAndValueforPrinting *ltvp = new t_LableTypeAndValueforPrinting();
                ltvp->label = expr->getTempName();
                ltvp->typeInfo = value->m_typeInfo;
                mapOfReplacements["%label" + toString(i++)] = ltvp;
                //   cout<<"\nPushing into map:"<<(i-1)<<endl;
            }
            t_Expression *expr = value->m_backPointerToExpression;
            typeTupleVector.push_back((expr->m_dagExpression)->getTypeOfExpression());
        }

    }

    t_LableTypeAndValueforPrinting *mapValue = new t_LableTypeAndValueforPrinting();
    mapValue->label = name;

    TypeOfExpressionTuple *typeInfo = dagExpression->getTypeOfExpression();
    (mapValue->typeInfo).m_type = typeInfo->m_type;
    (mapValue->typeInfo).m_width = typeInfo->m_width;

    mapOfReplacements["%name"] = mapValue;
    t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();
    string operatorSymbol = (dagExpression->m_dag)->getLabel();
    string signature = inferenceRule->generateKey(operatorSymbol, typeTupleVector);
    //cout<<"\n Signature:"<<signature;
    map <string, t_InferTypeOfExpression *> ::iterator it;
    it = mapOfTypeInferenceRules.find(signature);
    if (it != mapOfTypeInferenceRules.end())
    {
        inferenceRule = (*it).second;
    }
    else
    {
        signature = operatorSymbol + "@" + toString(typeTupleVector[0]->m_type);
        it = mapOfTypeInferenceRules.find(signature);
        if (it != mapOfTypeInferenceRules.end())
        {
            inferenceRule = (*it).second;
        }
        else
        {
            cout << "\nUnknown Operator Symbol:" << operatorSymbol << endl;
            return "";
        }
    }
    string printString = inferenceRule->printExpressionFormat;
    if (inferenceRule->unlimitedNumberOfArguments)
    {
        printString = inferenceRule->parseUnlimitedNumberOfOperandsPrintString(printString, typeTupleVector);
        printString = inferenceRule->parsePrintStringOfOneOperator(mapOfReplacements, printString, typeTupleVector.size());
    }
    else
    {
        printString = inferenceRule->parsePrintStringOfOneOperator(mapOfReplacements, printString, inferenceRule->maxargs);
    }
    printBufferOfExpression += printString;
    return printBufferOfExpression;
}
bool t_ExpressionAsDag::printExpressionToFileAsTree1(t_DAGNode *dagnode, ofstream *outfile)
{
    if (dagnode == NULL)
    {
        return false;
    }
    *outfile << dagnode->getLabel();
    int num_operands = dagnode->getSizeOfOutList();
    if (num_operands > 0)
    {
        *outfile << "(";
        int i = 0;
        list<t_DAGNode *>::iterator it = dagnode->getOutListBeginIterator();
        list<t_DAGNode *>::iterator outlist_end_iterator = dagnode->getOutListEndIterator();
        for (; it != outlist_end_iterator; it++)
        {
            bool printed_child = printExpressionToFileAsTree1(*it, outfile);
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

bool t_ExpressionAsDag::printExpressionToFileAsDAG(t_DAGNode *dagnode, ostream &outfile)
{
    assert(dagnode != NULL);
    dagnode->printExpressionToFile(outfile);
}

/**
 * Prints the Expression to the outfile.
 * LHS is already printed in atom, It prints the RHS(Expression which is in form of DAG) to file.
 * @param dagnode
 * @param outfile
 * @return 
 */
bool t_ExpressionAsDag::printExpressionToFileAsDAG(t_DAGNode *dagnode, ofstream *outfile)
{
    if (dagnode == NULL)
    {
        return false;
    }

    *outfile << dagnode->getLabel();

    int num_operands = dagnode->getSizeOfOutList();
    if (num_operands > 0)
    {
        *outfile << "(";
        int i = 0;
        list<t_DAGNode *>::iterator it = dagnode->getOutListBeginIterator();
        list<t_DAGNode *>::iterator outlist_end_iterator = dagnode->getOutListEndIterator();
        for (; it != outlist_end_iterator; it++)
        {
            if ((*it)->getSizeOfOutList() > 0)
            {
                *outfile << "["<< (*it)->getNodeID()<< "]"; // << hex << (long int) *it <<
            }
            else
            {
                *outfile << (*it)->getLabel();
            }

            if (i != num_operands - 1)
                *outfile << ",";

            i++;
        }
        *outfile << ")"<<endl;
        dagnode->assignVisitedFlag(true);

        it = dagnode->getOutListBeginIterator();
        for (; it != outlist_end_iterator; it++)
        {
            if (((*it)->isNodeVisited()==false) && ((*it)->getSizeOfOutList() > 0))
            {
                *outfile << "["<< (*it)->getNodeID()<< "] := ";
                printExpressionToFileAsDAG(*it, outfile);		
            }
        }

    }
    return true;
}
/**
 *  Get Label of the current Expression node
 */
string t_ExpressionAsDag::getLabelOfExpressionNode()
{
    t_DAGNode *dagNode = this->m_dag;
    return dagNode->getLabel();
}

/**
 *
 *  Get Type of Expression Tuple
 */
TypeOfExpressionTuple t_ExpressionAsDag::getTypeOfExpressionTuple()
{
    t_DAGNode *dagNode = this->m_dag;
    t_ExpressionValue *valueInExpression = (t_ExpressionValue *) dagNode->getValue();
    TypeOfExpressionTuple typeInfo = valueInExpression->m_typeInfo;
    return typeInfo;
}

/**
 * Method - getValueStoredInExpressionLabel()
 * Arguments - .
 * Description - Get the value type (SYMBOL,CONSTANT,OPERATOR) stored in Expression label
 * Returns - labelType stored in value filed of the current expression
 */
t_TypeOfValueStoredInExpressionLabel t_ExpressionAsDag::getValueStoredInExpressionLabel()
{
    t_DAGNode *dagNode = this->m_dag;
    t_ExpressionValue *valueInExpression = (t_ExpressionValue *) dagNode->getValue();
    t_TypeOfValueStoredInExpressionLabel labelType = valueInExpression->m_labelType;
    return labelType;
}

/**
 * Method - getVectorOfOperands()
 * Arguments - Current expression object (implicit)
 * Description - Get the vector of operands and make a vector of those expressions
 * Returns - vectorOfLeafNodes (vector <Expression *> ) which contains all operand expressions of current expression (dag)
 */
vector<t_Expression *>t_ExpressionAsDag::getVectorOfOperands()
{
    t_DAGNode *dagNode = this->m_dag;
    list <t_DAGNode *> ::iterator beginit = dagNode->getOutListBeginIterator();
    list <t_DAGNode *> ::iterator endit = dagNode->getOutListEndIterator();
    int num_children = dagNode->getSizeOfOutList();

    // #ifdef added by Ajith John on 27 Jan 2014 for debugging
    // Later commented by Ajith John on 29 Jan 2014
    /*
    #ifdef QE 
    vector<t_Expression *> vector_of_operands;    
    if (num_children == 0)
    {
        return vector_of_operands;
    } 

    for (list <t_DAGNode *> ::iterator it = beginit; it != endit; it++)
    {
        t_DAGNode *current_operand = *it;
	assert(current_operand != NULL);
        t_ExpressionValue *valueInExpression = (t_ExpressionValue *) current_operand->getValue();
	assert(valueInExpression != NULL);
        t_Expression * expression_pointer = valueInExpression->m_backPointerToExpression;
	assert(expression_pointer != NULL);

        vector_of_operands.push_back(expression_pointer);
        if(expression_pointer->m_dagExpression->m_dag!=current_operand)
        {
            cout<<" ExpressionValue and expression are different:"<<current_operand->getNodeID();
            if(expression_pointer->m_dagExpression->m_dag!=NULL)
            {
                cout<<"-->"<<expression_pointer->getID()<<endl;
            }
        }
    }
    return vector_of_operands;
    #else */

    vector<t_Expression *>vector_of_operands(0,NULL);
    if (num_children == 0)
    {
        return vector_of_operands;
    } 

    vector_of_operands.reserve(num_children);
    int count = 0;
    for (list <t_DAGNode *> ::iterator it = beginit; it != endit; it++)
    {
        count++;
        
        t_DAGNode *current_operand = *it;
        t_ExpressionValue *valueInExpression = (t_ExpressionValue *) current_operand->getValue();
        t_Expression * expression_pointer = valueInExpression->m_backPointerToExpression;
        vector_of_operands.push_back(expression_pointer);
        if(expression_pointer->m_dagExpression->m_dag!=current_operand)
        {
            cout<<" ExpressionValue and expression are different:"<<current_operand->getNodeID();
            if(expression_pointer->m_dagExpression->m_dag!=NULL)
            {
                cout<<"-->"<<expression_pointer->getID()<<endl;
            }
        }
    }
    return vector_of_operands;

    /*
    Later commented by Ajith John on 29 Jan 2014

    #endif

    */    
}

/**
 * Replace a leaf of the dag by another dag expression
 */
bool t_ExpressionAsDag::replaceLeafOfADAGExpressionByDAGExpression(t_DAGManager *&dagmanager, t_ExpressionAsDag *dagExpressionToReplace, t_ExpressionAsDag *dagExpressionReplaceBy)
{
    //Does it also replace values??
    t_DAGNode *toReplace = dagExpressionToReplace->m_dag;
    t_DAGNode *replaceBy = dagExpressionReplaceBy->m_dag;
    bool replace_result = dagmanager->ReplaceLeafByDAG(toReplace, replaceBy, true);
    return replace_result;
}
