#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "ExpressionAsDAG.h"
#include "DAGSimplifier.h"
#include "helping_functions.h"


#include <vector>
#include <map>

using namespace std;

#define CONSTANT_VALUE_LABEL "CONSTANT_EVALUATED"
class t_ExpressionEvaluator;
class t_InvalidBitManager;

class t_Expression
{
private:
    bool m_simplified; // Current expression is simplified or not flag
    t_ExpressionAsDag *m_dagExpression; // To store expression as a dag expression
    string m_tempName; // To store temporary name of the expression while printing
    int m_inferredWidth; // Inferred width for the current expression
    bool m_validValue;  // This expression is having assigned a value or not
    // Static logfile for printing the log messages based on verbosity level
    // Used only from the t_Logger
    static ofstream *m_logFile;
    
    static int constLabelCount; 
    bool m_isRestructured;
    // Constructor of the expression class
    // Only used by the friend class t_ExpresssionManager
    t_Expression();

    // Destructor for deallocation of the resources allocated for the current expression
    ~t_Expression();

    // Create a leaf expression with given label
    t_Expression * createExpression
    (
     string &label,
     TypeOfExpressionTuple &typeInfo,
     t_TypeOfValueStoredInExpressionLabel &labelType,
     int Expressionimplementationtype,
     t_ExpressionImplementationManager *&expressionImplementationManagerInstance
     );

    // Create an operator expression
    //   Inferred type should already be present.
    t_Expression *createExpression
    (
     string &label,
     t_TypeOfValueStoredInExpressionLabel &labelType,
     TypeOfExpressionTuple &typeInfo,
     vector<t_Expression *> &operandExpressions,
     int Expressionimplementationtype,
     t_ExpressionImplementationManager *&expressionImplementationmanagerinstance,
     t_ExpressionEvaluator* &evaluator,
     bool doSelectConcatSimplification=true //if true and update expression, performs the update optimization specified in PENDING.TXT
     );
    
    
    /**
     * Simplifies the current expression. This is used because new rules simplification might not work on bit vectors of unspecified
     * length.
     * @param label the label of the expression
     * @param operands the operands of the expression
     * @param typeInfo
     * @param Expressionimplementationtype
     * @param expressionImplementationManagerInstance
     * @param evaluator
     * @return returns the new simplified expression or NULL if there's no simplification possible
     */
    t_Expression* doSimplification(const string &label, vector<t_Expression*> &operands,
        TypeOfExpressionTuple typeInfo, int Expressionimplementationtype, 
        t_ExpressionImplementationManager* expressionImplementationManagerInstance, t_ExpressionEvaluator *evaluator);
    /**
     * Equates the width of the operands of expressions.
     * If the width of operands of operators like arithmatic, bitwise and logical operators, is not same
     * we add a zero extn operation on the smaller operand to match its width with the larger operand.
     * @param label
     * @param operands
     * @param exprImplType
     * @param eimInstance
     * @param evaluator
     * @return The width of the operands. This could be used to verify /infer the width of operator<br/>
     *   -2 : width calculation for this operator not supported
     *   -3 : width mismatch in operands.
     */
    int equalizeOperandWidths(const std::string& label, std::vector<t_Expression*> &operands, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator);
    /**
     * Resizes the index expr (using zeroextn or select) to the size of the index
     * that is required according to the dimension of the array expr;
     * @param array_expr
     * @param index
     * @return new index expression
     */
    t_Expression* resizeIndex(t_Expression* array_expr, t_Expression* index, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator);
    t_Expression* extendExpr(t_Expression* expr, int width, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator);
    
    /**
     * This function returns a new array expression (first operand) for update expression.
     * This optimizes the update expression. In this function, if the array expression is an
     * update expression and the index of that expression is the same as the current index,
     * then we can simply remove the inner update as that will not affect the result.
     * For example. the expression:
     * update( update( A,i,j), i,y) can be simplified as
     * update( A,i,y )
     * This function takes the array expr and the index and removes the occurrences of modification
     * at that index from the array expr and returns a new one.
     * The pair contains true, if the value was modified. and the new value or false and the
     * original value.
     */
    pair<bool,t_Expression*> getArrayExprForUpdate( t_Expression* array_expr, t_Expression* index,int expressionImplementationType, t_ExpressionImplementationManager *eimInstamce, t_ExpressionEvaluator *evaluator);

    /**
     *This function returns a new expression for read. This optimizes the expression by comparing the array_expr with the index. If the index matches the
     * index value of the inner update, then we can simply set the result of read as the value set by the update.
     */
    t_Expression* getExprForRead( t_Expression* array_expr, t_Expression* index, t_TypeOfValueStoredInExpressionLabel labelType, TypeOfExpressionTuple typeInfo, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator);
        
    //Cache for the unknown index. We will use this symbol as our own unknown symbol.
    static t_Expression* unknownIndexExpression;
    static t_Expression* unknownIndexInvalidBitExpression;//invalid bit for the unknown symbol
    
    /**
     * This function returns the unknown index expression. If the value is already generated,
     * it returns that value otherwise, the index is generated, cached and returned
     * @param exprImplType expression Implementation Type
     * @param eimInstance
     * @return the unknown index expression
     */
    t_Expression* getUnknownIndexExpression(int exprImplType, t_ExpressionImplementationManager* eimInstance);
    
    //cache for symbol used in ite for update at unknown index
    static const std::string updateUnknownSymbol;
    /**
     * This function returns the symbol for update at the unknown index. When there is a read on the
     * update expression, This symbol will be replaced by read at the index of read.
     * see MEMORY_SIMPLIFY.TXT for more details
     * @param exprImplType
     * @param eimInstance
     * @param sizeExpr the resultant expression will have the same size as sizeExpr expression
     * @return the symbol to be used in update expression
     */
    t_Expression* getUpdateSymbol(int exprImplType, t_ExpressionImplementationManager* eimInstance, t_Expression* sizeExpr);
    
    /**
     * This function returns the name of the symbol that is used when simplifying for updates at unknown index. This is the
     * name of the symbol generated by getUpdateSymbol when generating the update expression. This symbol will be replaced by
     * read at the given index when you apply a read in the update expression. This also returns the array we are working on.
     * If the update expression does not contain update at unknown index, we return <code>make_pair("",array_expr)</code>
     * @param exprImplType
     * @param eimInstance
     * @return tuple containing name of the update symbol and the expression for the array we are working on.
     */
    pair<string, t_Expression*> getUpdateSymbolNameAndMemoryInExpression(int exprImplType, t_ExpressionImplementationManager* eimInstance);
    
    /**
     * This function modifies the current value so as to incorporate the new update at unknown index value.
     * The old value can be an ite or another expression. if alreadyITE is true, then we consider that the
     * ite expression of old value is what we have generated. otherwise it was generated from the verilog.
     * if the oldvalue is generated by us, then we extract the original value from the then part of the expression
     * Otherwise, we use the oldvalue as original value
     * we modify the expression as
     * <pre>
     * ite( oldValueCondition ^ originalValue == newValue, originalValue, X)
     * </pre>
     * @param oldValue the oldvalue in the update expression
     * @param newValue the new value i.e. value of the new update at unknown index expression
     * @param alreadyITE if the ite expression was generated by us
     * @param exprImplType
     * @param eimInstance
     * @param evaluator
     * @return the new ite expression value
     */
    t_Expression* getNewUpdateValue(t_Expression* oldValue, t_Expression* newValue, bool alreadyITE, int exprImplType, t_ExpressionImplementationManager* eimInstance, t_ExpressionEvaluator* evaluator);
    
    /**
     * This function modifies the update expression's value so that it incorporates the update at the unknown index.
     * It also recursively calls itself for nested update expressions. Finally, this expression also adds an update
     * at unknown index at the lowset level if not already present. If it is already present, we simply modify that
     * expression's value by generating an ite.
     * @param updExpr the update expression to modify
     * @param newValue the value set by the new update at unkniown index expression
     * @param exprImplType
     * @param eimInstance
     * @param evaluator
     * @return a tuple with first element as the new expression generated, and the second value is true if the
     * update at unknown index was already present in the expression DAG, false otherwise.
     */
    pair<t_Expression*,bool> modifyUpdateForUnknownIndex(t_Expression* updExpr, t_Expression* newValue, 
                        int exprImplType, t_ExpressionImplementationManager* eimInstance, t_ExpressionEvaluator* evaluator);
    
    /**
     * Build the update expression from the given operands and also sets the boolean value to return in the pair.
     * This function is only used as a helper function for modifyUpdateForUnknownIndex.
     * @see #modifyUpdateForUnknownIndex
     * @param arrayExpr the expression for array
     * @param index the index for update
     * @param value the value to set
     * @param booleanVal the boolean value to return
     * @param exprImplType
     * @param eimInstance
     * @param evaluator
     * @return a pair of update expression and the boolean val
     */
    pair<t_Expression*,bool> makeUpdateExpression(t_Expression* arrayExpr, t_Expression* index, t_Expression* value, bool booleanVal, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator);
    
    /**
     * Checks whether this expression is invalid.
     * @param exprImplType
     * @param eimInstance
     * @return true if this expression is definitely invalid (i.e. InvalidBit is CONSTANT 1) else false
     */
    bool isInvalidExpression(int exprImplType, t_ExpressionImplementationManager* eimInstance);
    /**
     * This function creates memory array that is the same size as the size of expr. The only difference is that all the
     * values in the array are unknown.
     * @param expr the expression whose unknown copy we need.
     * @return the new expression similar in soze to expr but all values unknown
     */
    t_Expression* createUnknownMemoryArraySimilarTo(string label, t_TypeOfValueStoredInExpressionLabel labelType, t_Expression* expr, int exprImplType, t_ExpressionImplementationManager *eimInstance);
    
    t_Expression* createASelectOverConcat(t_Expression* concat_expression,
                                      unsigned int upper_bound,
                                      unsigned int lower_bound,
                                      int expressionImplementationType,
                                      t_ExpressionImplementationManager* &expressionImplementationManagerInstance,
                                      t_ExpressionEvaluator* &evaluator);
    t_Expression* createASelectOverConcat(t_Expression* concat_expression,
                                      unsigned int upper_bound,
                                      unsigned int lower_bound,
                                      TypeOfExpressionTuple typeInfo,
                                      int expressionImplementationType,
                                      t_ExpressionImplementationManager* &expressionImplementationManagerInstance,
                                      t_ExpressionEvaluator* &evaluator);
    // Check whether a leaf expression already exists, or not
    t_Expression * checkIfExpressionExists
    (
     string &label,
     int expressionImplementationType,
     t_ExpressionImplementationManager *&expressionImplementationManagerInstance
     );

    // Check whether a operator expression with given leaf expressions is already present
    t_Expression *checkIfExpressionExists
    (
     string &label,
     vector<t_Expression *> &operandExpressions,
     int expressionImplementationType,
     t_ExpressionImplementationManager *&expressionImplementationManagerInstance
     );

    // Check for the structural equivalence of two expressions ( Current , e2)
    bool checkStructuralEquality
    (
     t_Expression *e2,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Get number of Expressions under current expression
    int getSizeOfExpression
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Get the type of expression tuple (width,type)
    TypeOfExpressionTuple *getTypeOfExpression
    (
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Delete an expression
    bool removeExpression
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Print current expression into constraint format.
    // Must have atleast one operator.
    bool printExpressionAsYicesExpression
    (
     string &expression_symbol,
     ofstream *fout,
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

     // Set and get temporary names assigned while printing 
    bool setTempName(string &name);
    string getTempName();

    // Print the expression into the implementaion format.
    bool printExpressionToFileAsTree
    (
     string &expression_symbol,
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance,
     ofstream *outfile
     );

     // Set the value of the expression
    bool setValue
    (
     t_ExpressionValue *value,
     bool resetAncestors,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance,
     t_ExpressionEvaluator* evaluatorObject
     );

    // Get value of the expression
    t_ExpressionValue *getValue
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Clear valid flags of all ancestor expressions for the current expression.
    bool clearAllValidFlagsofAllAncestorExpressions
    (
     t_Expression *expr,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );
    // Clear all valid flags of a given expression ( all childs of the expression)
    bool clearAllValidFlagsOfAnExpression
    (
     t_Expression *expr,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance
     );

    // Set the valid flag of the expression ( As already evaluated )
    bool setValidFlag(bool valid);
    // Is this current expression is already evaluated or assigned a value.
    bool isValidValue();

    // Get Label of the current expression.
    string getLabelOfExpression
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );
   

     // Get acutal expresssion value as string.( Converts to string bv to string, int to string etc..)
    string getActualExpressionValueAsString
    (
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Clear valid flags of the expressions given in expressions_tobe_evaluated
    bool clearAllEvaluatorFlagsForExpressions
    (
     vector<t_Expression *> &expressions_tobe_evaluated,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );


    /**
     * Part of evaluation assign values to leaves
     */
    bool setValuesOfLeaves(map<string, string> &leaf_values,
                           bool clearValidityOfAncestors,
                           int expressionImplementationType,
                           t_ExpressionImplementationManager *expressionImplementationManagerInstance,
                           t_ExpressionEvaluator* &evaluator
                           );
    /**
     * Shift the actual value of this expression to given expression with the given label
     */
    bool copyValueFromExpressionToLeafWithLabel(t_Expression *toExpr, t_Expression *fromExpr,
                                                int expressionImplementationType,
                                                t_ExpressionImplementationManager *expressionImplementationManagerInstance,
                                                t_ExpressionEvaluator* evaluator
                                                );

    // Get the operand expressions of the current expression
    vector <t_Expression *> getVectorOfLeafExpressions
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance
     );
     // Print the value of the expression 
    bool printValueInExpressionNode
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    // Copy the expressions using suffix for leaf nodes
    t_Expression *copyExpressionUsingSuffixForLeafNodes
    (
     t_Expression *expression_to_copy,
     string &suffix,
     int Expressionimplementationtype,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance,
     t_ExpressionEvaluator* &evaluator
     );

    //Copy the expression using suffix for leaf nodes recursively
    t_Expression *copyExpressionUsingSuffixForLeafNodesRecursively
    (
     t_Expression *node,
     string &suffix,
     int Expressionimplementationtype,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance,
     t_ExpressionEvaluator* &evaluator,
     map<unsigned long int, unsigned long int> &copy_expr_map
     );


     // Replave leaf of an expression by the given expression
    bool replaceLeafOfAExpressionByExpression
    (
     t_Expression *expression_to_replace,
     t_Expression *expression_replace_by,
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );
    
    // Simplify given expression
    bool SimplifyExpression
    (
     t_Expression * &expression_to_simplify,
     int Expressionimplementationtype,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance,
     t_DAGSimplifier *ds
     );


     // Get Expression with label (Only searches for the leaf nodes)
    t_Expression *getExpressionWithLabel
    (
     string &label,
     int ExpressionImplementationType,
     t_ExpressionImplementationManager * expressionImplementationManagerInstance
     );

     // Evaluate a given expression, for which the leaf nodes values are alrady assigned.
    t_ExpressionValue *evaluateExpression
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance,
     t_ExpressionEvaluator* &evaluator
     );

    // EvaluateExpression helper function recursive.
    t_ExpressionValue* evaluateExpressionRecursively
    (
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance,
     t_ExpressionEvaluator* &evaluator
     );

    // clear all visited flags under an expression
    bool clearVisitedFlagsUnderAnExpression
    (
     t_Expression *expr,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance
     );

    // evaluate a vector of expressions
    // Must be assigned values to leaf nodes which are not constants
    bool evaluateAVectorOfExpressions
    (
     vector<t_Expression *> &expressions_tobe_evaluated,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance,
     t_ExpressionEvaluator* &evaluator
     );

    // Get operands of this expression
    vector<t_Expression *> getVectorOfOperands
    (
     int ExpressionImplementationType,
     t_ExpressionImplementationManager * expressionImplementationManagerInstance
     );

    // Check whether adding an edge between nodeFrom to nodeTo forms a cycle
    bool checkWhetherCreatingEdgeFormsCycle
    (
     t_Expression *nodeFrom,
     t_Expression *nodeTo,
     int expressionType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance
     );

    // Garbage collector of DagManager(Expression Implementation manager) will be called here.
    bool gc
    (
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementataionManagerInstance
     );

    // Print the expression into a file ( Should be readable back)
    bool printExpressionToFile
    (
     ofstream *outfile,
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );

    bool printExpressionWithSet(
                                t_Expression *expr_to_print,
                                ofstream *fout,
                                int expressionImplementationType,
                                t_ExpressionImplementationManager *expressionImplementationManagerInstance
                                );

    bool printExpressionWithSetRecursively(
                                           t_Expression *expr_to_print, set<unsigned int>& expressions_set,
                                           int expressionImplementationType,
                                           t_ExpressionImplementationManager *expressionImplementationManagerInstance
                                           );
    bool printExpressionToFileAsDAG(const string &expression_symbol, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, ostream &outfile);
    bool printExpressionToFileAsDAG(string &expression_symbol, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, ofstream *outfile);


    // Parse the expression printed from printExpressionToFile,and generate the expression again.
    bool buildExpressionFromFile
    (
     ifstream *infile,
     int expressionImplementationType,
     t_ExpressionImplementationManager *expressionImplementationManagerInstance
     );
   inline bool clearVisitedFlagsOfAllExpressions(int expressionImplementationType,t_ExpressionImplementationManager* expressionImplementationManagerInstance)
    {
        if(expressionImplementationType==DAG_EXPRESSION)
        {
            return this->m_dagExpression->clearVisitedFlagsOfAllDagNodes(expressionImplementationManagerInstance->dagmanager);
        }
        else
        {
            return false;
        }
    }
    t_ExpressionValue* getAnInitialValueForMemory(string sMemoryName,t_ExpressionEvaluator* &evaluator);

    t_Expression* getMostRecentRestructuredExpression(int expressionImplementationType,t_ExpressionImplementationManager* expressionImplementationManagerInstance);
    bool replaceExpressionRepresentation(t_Expression* old_expression,t_Expression* new_expression,int expressionImplementationType,t_ExpressionImplementationManager* expressionImplementationManagerInstance);

    int getConstantValuePresentInExpression( int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance);
private:
    t_Expression* modifyExpressionForSelectOfRead(string &label, vector<t_Expression*> &operandExpressions, int Expressionimplementationtype,
                                                            t_ExpressionImplementationManager* expressionImplementationManagerInstance,
                                                            t_ExpressionEvaluator* evaluator, bool doSelectConcatSimplification);
    /**
     * Returns the indices to perform the read. LIRA may linearize constant selects on multidimensional arrays,
     * This function undos that operation and returns the indices for which the selection is to be applied.
     * for e.g. for memory mem[4:0][1:0] and select(mem,4,4) can be simplified to mem[2][0]
     * while select(mem,4,5) is mem[2] and the result is the entire final dimension.
     * @param arrayExpr the expr on which the select is applied
     * @param lb lb of select
     * @param ub ub of select
     * @return the indices for select, empty vector if none required and bool value which will say whether we read from last dimension i.e.
     * there is a select on all the dimensions or some dimensions are skipped. (This will be true, if we select one bit from the memory)
     */
    pair<vector<int>,bool> getIndicesForRead(t_Expression* arrayExpr,int lb,int ub);
public:
    
    //following method added by sukanya
    inline t_ExpressionAsDag * getDagExpression() {
        return m_dagExpression;
    }
    
    
    t_Expression* createABitVectorConstantZeroExpression(int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance);
    t_Expression* createABitVectorConstantOneExpression(int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance);
    string getLabelOfExpression
    ( int expressionType );
     inline void setRestructuredFlag(bool flag)
    {
        this->m_isRestructured = flag;
    }
    inline bool getRestructuredFlag()
    {
        return this->m_isRestructured;
    }
    inline int getID()
    {
        return m_dagExpression->m_dag->getNodeID();
    }
    bool clearVisitedFlagsUnderThisExpression();
    bool assignVisitedFlagRecursively(bool val, t_HashTable<unsigned long long, bool>&visited)
    {
        return this->m_dagExpression->m_dag->assignVisitedFlagRecursively(val, visited);
    }
    template<typename T>
    static bool setVisitedFlagsUnderTheseExpressions(T&vec, bool val);

    inline t_TypeOfValueStoredInExpressionLabel getTypeOfValueStoredInExpressionLabel(t_Expression* expr, int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
    {
      if(expressionImplementationType!=DAG_EXPRESSION || expr == NULL)
        {
            return INVALID;
        }
      else
	{
	  t_ExpressionValue* value_in_expr = expr->getValue(expressionImplementationType, expressionImplementationManagerInstance);
	  return value_in_expr->getTypeOfValueStoredInExpressionLabel();
	}
    }


    int getAtomNumberFromPositionOfABitInConcat(unsigned int position, int expressionImplementationType, t_ExpressionImplementationManager*& expressionImplementationManagerInstance);
    // friends to make use of all functionality from here.
    friend class t_ExpressionManager;
    friend class t_ExpressionAsDag;
    friend class t_InvalidBitManager;
};

struct isLessThan{
    
    inline bool operator()(t_Expression* expr1, t_Expression* expr2)
    {
        //if(expr1 == NULL && expr2 == NULL) return false;
        if(expr1 == NULL) return true;
        if(expr2 == NULL) return false;
        return expr1->getID() < expr2->getID();
    }
};

struct isEqual{
    inline bool operator()(t_Expression* expr1, t_Expression* expr2)
    {
        if(expr1 == NULL && expr2 == NULL) return true;
        if(expr1 == NULL || expr2 == NULL) return false;
        return expr1->getID() == expr2->getID();
    }
};
#endif  /* EXPRESSION_H */

