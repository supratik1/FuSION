#include "Oprs.h"
#include "helping_functions.h"
#include "ExpressionEvaluator.h"
#include "LogManager.h"
#include "InvalidBitManager.h"
#include "ExpressionEvaluator.h"
#include "ExpressionManager.h"
#include "Expression.h"
#include <iostream>
#include <cassert>
#include<string>
#include<algorithm>
#include<iterator>
#include<map>
#include <cmath>
//#include <map>
#include <set>
#include <climits>

using namespace std;

ofstream *t_Expression::m_logFile = NULL;
int constLabelCount = 0;

t_Expression* t_Expression::createASelectOverConcat(t_Expression* concat_expression,
                                                    unsigned int upper_bound,
                                                    unsigned int lower_bound,
                                                    TypeOfExpressionTuple typeInfo,
                                                    int expressionImplementationType,
                                                    t_ExpressionImplementationManager* &expressionImplementationManagerInstance,
                                                    t_ExpressionEvaluator* &evaluator)
{
    if(lower_bound == 0 && upper_bound == t_ExpressionManager::getSingleton()->getWidth(concat_expression) -1)
        return concat_expression;
    else if (concat_expression->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance) != "concat")
    {
        return NULL;
    }
    //else
    t_ExpressionManager* em = t_ExpressionManager::getSingleton();
    vector<t_Expression*> operands = em->getVectorOfOperands(concat_expression);
    int curr=0;
    int currOffset=0;
    
    vector<t_Expression*> newOperands;
    int endOffset;
    for(curr=operands.size()-1 ; curr>=0 ; --curr, currOffset=endOffset+1)
    {
        endOffset= currOffset+em->getWidth(operands.at(curr))-1;
        
        if(lower_bound>endOffset || upper_bound < currOffset)
            continue;
        
        if(lower_bound<=currOffset && upper_bound>=endOffset)
        {
            newOperands.push_back(operands[curr]);
        }
        else if(lower_bound<=currOffset && upper_bound<endOffset) //select the required LSB part
        {
            t_Expression* e = em->createASelectExpressionOnExpression(operands[curr], upper_bound-currOffset, 0);
            newOperands.push_back(e);
        }
        else if(lower_bound>currOffset && upper_bound>=endOffset) //select the required MSB part
        {
            t_Expression* e = em->createASelectExpressionOnExpression(operands[curr], endOffset-currOffset, lower_bound-currOffset);
            newOperands.push_back(e);
        }
        else if(lower_bound>currOffset && upper_bound<endOffset) //select the required intermediate part.
        {
            t_Expression* e = em->createASelectExpressionOnExpression(operands[curr], upper_bound-currOffset, lower_bound-currOffset);
            newOperands.push_back(e);
        }
        else
        {
            assert("Should not reach here");
        }
    }
    
    reverse(newOperands.begin(), newOperands.end());
    return em->createExpression(em->m_operatorLabelConcat, newOperands, typeInfo);
}


t_Expression* t_Expression::createASelectOverConcat(t_Expression* concat_expression,
                                                    unsigned int upper_bound,
                                                    unsigned int lower_bound,
                                                    int expressionImplementationType,
                                                    t_ExpressionImplementationManager* &expressionImplementationManagerInstance,
                                                    t_ExpressionEvaluator* &evaluator)
{
    if (concat_expression->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance) != "concat")
    {
        return NULL;
        // A concat expression which doesnot have a "concat" label is received.
//        std::string ub_str = toString(upper_bound);
//        std::string lb_str = toString(lower_bound);
//        TypeOfExpressionTuple typeinfo_int = {TYPE_UNSIGNED_INTEGER, -1};
//        t_TypeOfValueStoredInExpressionLabel operator_type = OPERATOR;
//        t_TypeOfValueStoredInExpressionLabel constant_type = CONSTANT;
//        t_Expression* ub_expr = createExpression(ub_str, typeinfo_int, constant_type, expressionImplementationType, expressionImplementationManagerInstance);
//        t_Expression* lb_expr = createExpression(lb_str, typeinfo_int, constant_type, expressionImplementationType, expressionImplementationManagerInstance);
//        vector<t_Expression*> operands_select(3, NULL);
//        operands_select[0] = concat_expression;
//        operands_select[2] = ub_expr;
//        operands_select[1] = lb_expr;
//        TypeOfExpressionTuple typeinfo_select = {TYPE_UNSIGNED_BITVECTOR, upper_bound - lower_bound + 1};
//        string operator_label_select = "select";
//        t_Expression* result = createExpression(operator_label_select, operator_type, typeinfo_select, operands_select, expressionImplementationType, expressionImplementationManagerInstance, evaluator, true);
//        return result;
    }
    else if(lower_bound == 0 && upper_bound == t_ExpressionManager::getSingleton()->getWidth(concat_expression) -1)
        return concat_expression;
    else
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        // Iterate through concat operands and collect their widths
        vector<t_Expression*> concatOperands = concat_expression->getVectorOfOperands(expressionImplementationType, expressionImplementationManagerInstance);
        vector<t_Expression*>::reverse_iterator concatOperandsRevesrseIterator = concatOperands.rbegin();
        vector<t_Expression*>::reverse_iterator concatOperandsEndIterator = concatOperands.rend();

        vector<int> widthsOfConcatOperator;
        while (concatOperandsRevesrseIterator != concatOperandsEndIterator)
        {
            t_ExpressionValue* operandValue = (*concatOperandsRevesrseIterator)->getValue(expressionImplementationType, expressionImplementationManagerInstance);
            int width = operandValue->getWidth();
            widthsOfConcatOperator.push_back(width);
            concatOperandsRevesrseIterator++;
        }

        /**
         * Iterate over concat operands in reverse, since it starts lower bound in last operand
         * if( lowerBoundInOldSelect == 0 &&
         *     upperBoundInOldSelect == Sum Of Widths of i operands in Concat)
         * {
         *        if ( i==1 )
         *                return last operand of concat (0th operand of concat in reverse)
         *        else
         *            create new_concat of i operands
         *            return new_concat
         * }
         * // define : cwidth_i ( cwidth_i = cwidth_(i-1) + width of ith operand in concat )
         * else if( (cwidth_i-1) >= upper bound of slect )
         * {
         *     find in which operand of concat lower bound starts
         *     And start adding the operands to new_concat operands whose (cwidth_i-1) >= lower bound in select
         *        // newLowerBoundForSelect = get new lower bound for select
         *        // newUpperBoundForSelect = get new upper bound for select
         *     if ((cwidth_i-1) > upper boound in select)
         *     {
         *            create new_concat = concat (new_concat_operands)
         *            create new_select = select ( new_concat, newLowerBoundForSelect, newUpperBoundForSelect)
         *            return new_select
         *      }
         *    else
         *    {
         *        create new_concat = concat (new_concat_operands)
         *        return new_concat
         *    }
         * }
         */
        vector<t_Expression*> newOperandsForConcat;
        int currentWidth = 0;
        int sumOfWidthsInConcat = 0;
        int lowerBoundForNewSelect = 0;
        int upperBoundForNewSelect = 0;
        vector<int>::iterator it = widthsOfConcatOperator.begin();
        concatOperandsRevesrseIterator = concatOperands.rbegin();
        bool first_operand_for_new_concat = true;

        string selectLabel = "select";
        string concatLabel = "concat";

        while ((currentWidth == 0 || currentWidth - 1 <= upper_bound) && it != widthsOfConcatOperator.end())
        {
            currentWidth = currentWidth + (*it);
            if ((currentWidth - 1) >= lower_bound)
            {
                if (first_operand_for_new_concat)
                {
                    lowerBoundForNewSelect = (lower_bound - currentWidth + (*it));
                    first_operand_for_new_concat = false;
                    upperBoundForNewSelect = (upper_bound - currentWidth + (*it));
                }
                newOperandsForConcat.insert(newOperandsForConcat.begin(), (*concatOperandsRevesrseIterator));
                sumOfWidthsInConcat = sumOfWidthsInConcat + (*it);
            }
            if ((currentWidth - 1) == upper_bound && lowerBoundForNewSelect == 0)
            {
                if (newOperandsForConcat.size() > 1)
                {
                    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, sumOfWidthsInConcat};
                    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
                    t_Expression* newExpressionForInnerConcat = createExpression(concatLabel, labelType, typeInfo, newOperandsForConcat, expressionImplementationType, expressionImplementationManagerInstance, evaluator);
                    return newExpressionForInnerConcat;
                }
                else if (newOperandsForConcat.size() == 1)
                {
                    return (*concatOperandsRevesrseIterator);
                }
                else
                {
                    t_Logger *logManager = t_Logger::getInstance();
                    logManager->LOG("ERROR! In Expression Creation Simplifying select of concat ", m_logFile, c_RunLevelVerbosity);
                    cout << "ERROR! In Expression Creation Simplifying select of concat " << endl;
                    return NULL;
                }
            }
            else if ((currentWidth - 1) >= upper_bound)
            {
                //create concat of operands collected
                t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
                TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, sumOfWidthsInConcat};
                vector<t_Expression*> newSelectOperands;
                if (newOperandsForConcat.size() == 1)
                {
                    //select on one operand
                    TypeOfExpressionTuple typeInfo1 = {TYPE_SIGNED_INTEGER, 0};
                    t_TypeOfValueStoredInExpressionLabel labelType1 = CONSTANT;
                    string lbStr = toString(lowerBoundForNewSelect);
                    string ubStr = toString(upperBoundForNewSelect);
                    t_Expression* lowerBoundExpressionInNewSelect = createExpression(lbStr, typeInfo1, labelType1, expressionImplementationType, expressionImplementationManagerInstance);
                    t_Expression* upperBoundExpressionInNewSelect = createExpression(ubStr, typeInfo1, labelType1, expressionImplementationType, expressionImplementationManagerInstance);
                    TypeOfExpressionTuple typeInfo2 = {TYPE_UNSIGNED_BITVECTOR, upperBoundForNewSelect - lowerBoundForNewSelect + 1};
                    newSelectOperands.push_back(newOperandsForConcat[0]);
                    newSelectOperands.push_back(lowerBoundExpressionInNewSelect);
                    newSelectOperands.push_back(upperBoundExpressionInNewSelect);
                    t_Expression* newSelectExpression = createExpression(selectLabel, labelType, typeInfo2, newSelectOperands,
                                                                         expressionImplementationType, expressionImplementationManagerInstance, evaluator, false);
                    return newSelectExpression;
                }
                else
                {
                    t_Expression* newExpressionForInnerConcat = createExpression(concatLabel, labelType, typeInfo, newOperandsForConcat, expressionImplementationType, expressionImplementationManagerInstance, evaluator);
                    newSelectOperands.push_back(newExpressionForInnerConcat);

                    TypeOfExpressionTuple typeInfo1 = {TYPE_SIGNED_INTEGER, 0};
                    t_TypeOfValueStoredInExpressionLabel labelType1 = CONSTANT;
                    string lbStr = toString(lowerBoundForNewSelect);
                    string ubStr = toString(upperBoundForNewSelect);
                    t_Expression* lowerBoundExpressionInNewSelect = createExpression(lbStr, typeInfo1, labelType1, expressionImplementationType, expressionImplementationManagerInstance);
                    t_Expression* upperBoundExpressionInNewSelect = createExpression(ubStr, typeInfo1, labelType1, expressionImplementationType, expressionImplementationManagerInstance);
                    newSelectOperands.push_back(lowerBoundExpressionInNewSelect);
                    newSelectOperands.push_back(upperBoundExpressionInNewSelect);
                    TypeOfExpressionTuple typeInfo2 = {TYPE_UNSIGNED_BITVECTOR, upperBoundForNewSelect - lowerBoundForNewSelect + 1};
                    vector<t_ExpressionAsDag *> operandExpressionDags;
                    for (vector<t_Expression *>::iterator it = newSelectOperands.begin();
                            it != newSelectOperands.end(); it++)
                    {
                        t_Expression *expr = (t_Expression *) (*it);
                        operandExpressionDags.push_back(expr->m_dagExpression);
                    }
                    t_Expression* expressionObject = new t_Expression();
                    t_ExpressionAsDag * new_expression_as_dag = new t_ExpressionAsDag(selectLabel, typeInfo2, labelType, operandExpressionDags, expressionObject, dagmanager);
                    expressionObject->m_dagExpression = new_expression_as_dag;

                    expressionObject->m_validValue = false;
                    expressionObject->m_inferredWidth = typeInfo.m_width;
                    expressionObject->m_isRestructured = false;
                    return expressionObject;
                }
            }
            concatOperandsRevesrseIterator++;
            it++;
        }

        // otherwise we could not find bounds matching to the given expression
        // Returning a NULL expression here.
        return NULL;
    }
}

int t_Expression::getAtomNumberFromPositionOfABitInConcat(unsigned int position, int expressionImplementationType, t_ExpressionImplementationManager*& expressionImplementationManagerInstance)
{
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        string label = getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance);
        if (label != "concat")
        {
            return 0;
        }
        else
        {
            vector<t_Expression*> operands_of_concat = getVectorOfOperands(expressionImplementationType, expressionImplementationManagerInstance);
            int current_width = 0;
            vector<t_Expression*>::reverse_iterator it = operands_of_concat.rbegin(),
                    end_it = operands_of_concat.rend();
            unsigned int atom_count = 0;
            while (it != end_it)
            {
                t_Expression* current_expr = (*it);
                unsigned int current_operand_width = current_expr->getValue(expressionImplementationType, expressionImplementationManagerInstance)->getWidth();
                if (current_width <= position && current_operand_width + current_width > position)
                {
                    return atom_count;
                }
                it++;
                atom_count++;
                current_width += current_operand_width;
            }
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

/**
 * Method - createExpression
 * Descritption - For creating the expression that is either constant or Symbol (Based on the expression 
 implementation type check whether the expression exists already or not if exists then return already 
 existing expression or call  corresponding createExpression() method )
 * Arguments - label(string)
 *             typeInfo(TypeOfExpressionTuple) : type of the expression
 *	       labelType(TypeOfValueStoredInExpressionLabel) : the label stored in the label of the expression is either SYMBOL,OPERATOR or CONSTANT
 *	       Expressionimplementationtype(int) : Expression Implemented as DAG_EXPRESSION or Other
 *	       expressionImplementationManagerInstance(ExpressionImplementationManager *) : 
 if Expression implementation is DAG_EXPRESSION then it is dagmanager
 * Returns - New expression created (Expression * - type)
 */
t_Expression *
t_Expression::createExpression
(
 string &label,
 TypeOfExpressionTuple &typeInfo,
 t_TypeOfValueStoredInExpressionLabel &labelType,
 int Expressionimplementationtype,
 t_ExpressionImplementationManager *&expressionImplementationManagerInstance
 )
{

    //    {
    //        ofstream out("labels",ios::app);
    //        out<<label<<endl;
    //    }
    if (label.rfind(EXPRESSION_INVALID_BIT_LABEL) != string::npos && typeInfo.m_width != 1)
    {
        cout << "Passed width(" << typeInfo.m_width << ") is not as expected(1)" << endl;
    }
   
//    if(label.find(EXPRESSION_LABEL_AT_N_PREFIX)!=string::npos)
//    {
//        cout<<"found";
//    }
    {
        if (labelType == CONSTANT && (typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR || typeInfo.m_type == TYPE_SIGNED_BITVECTOR))
        {
            assert(label.length() > 2 && label[0] == '0' && label[1] == 'b');
            if(label.length() != typeInfo.m_width+2)
            {
                cerr<<"Type info has wrong width("<<typeInfo.m_width<<") for the constant. "
                        "Modifying the width to expected size("<<(label.length()-2) <<")."<<endl;
                throw logic_error("Wrong width for constant ");
                typeInfo.m_width = label.length() - 2;
            }
//            if(label.find("11110000010") != string::npos)
//            {
//                cout<<"found"<<endl;
//            }
        }
    }
    if (Expressionimplementationtype == DAG_EXPRESSION)
    {
        t_DAGManager* dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_DAGNode * dag_node = dagmanager->testAndReturnDAGNodeIfItExists(label);
        if (dag_node == NULL)
        {
            t_Expression *expressionObject = new t_Expression();
            t_ExpressionAsDag * new_expression_as_dag = new t_ExpressionAsDag(label, typeInfo, labelType, expressionObject, dagmanager);
            expressionObject->m_dagExpression = new_expression_as_dag;
            expressionObject->m_tempName = "";
            expressionObject->m_validValue = false;
            expressionObject->m_simplified = false;
            expressionObject->m_inferredWidth = -1;
            expressionObject->m_isRestructured = false;
            return expressionObject;
        }
        else
        {
            t_ExpressionValue *valueInExpression = (t_ExpressionValue *) dag_node->getValue();
            this->m_validValue = false;
            if (valueInExpression == NULL)
            {
                return NULL;
            }
            else
            {
                t_Expression * expression_in_dag_value = valueInExpression->m_backPointerToExpression;
                return expression_in_dag_value;
            }
        }
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        string s = "Trying to create Unknown Expression Implementation";
        logManager->LOG(s, m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
    //return NULL;
}

int t_Expression::equalizeOperandWidths(const std::string& label, std::vector<t_Expression*> &operands, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator)
{
#define THROW_ERR(x) throw std::runtime_error(#x);
    static set<string> allowedOperators;
    if (allowedOperators.empty())
    {
#define ADD(x) allowedOperators.insert(#x)
        allowedOperators.insert("add");
        allowedOperators.insert("sub");
        allowedOperators.insert("mul");
        allowedOperators.insert("div");
        allowedOperators.insert("logeq");
        allowedOperators.insert("bitand");
        allowedOperators.insert("ite");
        allowedOperators.insert("select");
        ADD(bitor);
        ADD(bitnor);
        ADD(bitnand);
        ADD(bitxor);
        ADD(casezeq);
        ADD(logand);
        ADD(logor);
        ADD(concat);
        ADD(diseq);
        ADD(gt);
        ADD(ge);
        ADD(lt);
        ADD(le);
        
        
#undef ADD

        ostream_iterator<string> os(cout, ", ");
        copy(allowedOperators.begin(), allowedOperators.end(), os);
        //getchar();

    }
    //    allowedOperators["add"]=true;
    //    allowedOperators["ite"] = true;
    //    allowedOperators["sub"] = true;
    //    allowedOperators["logeq"] = true;
    //    allowedOperators["bitand"]=true;
    //    map<string,bool>::iterator it = allowedOperators.find(label);
    //    
    //    if(it == allowedOperators.end())
    //        return;
    if (allowedOperators.find(label) == allowedOperators.end())
    {
        return -2; //not found
    }
    if (label == "concat")
    {
        //static long int count = 0;
        //cerr<<"count is "<<++count<<endl;
        int totWidth = 0;
        for (int i = 0; i < operands.size(); i++)
        {
            int wd = t_ExpressionManager::getSingleton()->getWidth(operands[i]);
            if (wd == -1)
                return -3; //cannot find the final width
            totWidth += wd;
        }
        return totWidth;
    }
    if (label == "ite")
    {
        int wd1 = t_ExpressionManager::getSingleton()->getWidth(operands.at(1));
        int wd2 = t_ExpressionManager::getSingleton()->getWidth(operands.at(2));

        if (wd1 == wd2)
            return wd1;
        else if (wd1 < wd2)
        {
            cout << "Ids of operands of " << label << " are" << operands[0]->getID() << " " << operands[1]->getID() << " " << operands[2]->getID() << endl;
            cout << "widths of operands of " << label << " are <> " << wd1 << " " << wd2 << endl;
            THROW_ERR(ite op1 < op2);
            //operands[1] = extendExpr(operands[1], wd2, exprImplType, eimInstance, evaluator);
        }
        else if (wd2 < wd1)
        {
            cout << "Ids of operands of " << label << " are" << operands[0]->getID() << " " << operands[1]->getID() << " " << operands[2]->getID() << endl;
            cout << "widths of operands of " << label << " are <> " << wd1 << " " << wd2 << endl;
            THROW_ERR(ite op2 < op1);
            //operands[2] = extendExpr(operands[2], wd1, exprImplType, eimInstance, evaluator);
        }
        return -3;
    }
    if(label == "select")
    {
        int op1Width = t_ExpressionManager::getSingleton()->getWidth(operands[0]);
        int lb = t_ExpressionManager::getSingleton()->getConstantValuePresentInExpression(operands[1]);
        int ub = t_ExpressionManager::getSingleton()->getConstantValuePresentInExpression(operands[2]);
        if(lb <0 || ub <0 || ub>=op1Width || lb>=op1Width ||abs(ub-lb)+1 >op1Width )
        {
            cerr<<"width error in select"<<endl;
            THROW_ERR(select width error)
        }
    }

    //for other 2 operand operators
    if (operands.size() == 2)
    {
        int wd1 = t_ExpressionManager::getSingleton()->getWidth(operands[0]);
        int wd2 = t_ExpressionManager::getSingleton()->getWidth(operands[1]);

        if (wd1 == wd2)
        {
            if (t_ExpressionManager::getSingleton()->getTypeOfOperator(label) == t_ExpressionManager::RELATIONAL ||
                    t_ExpressionManager::getSingleton()->getTypeOfOperator(label) == t_ExpressionManager::LOGICAL)
            {
                return 1;
            }
            else
            {
                return wd1;
            }
        }
        else if (wd1 < wd2)
        {
            cout << "Ids of operands of " << label << " are" << operands[0]->getID() << " " << operands[1]->getID() << endl;
            cout << "widths of operands of " << label << " are " << wd1 << " " << wd2 << endl;
            THROW_ERR(op0 < op1);
            //operands[0] = extendExpr(operands[0], wd2, exprImplType, eimInstance, evaluator);
        }
        else if (wd2 < wd1)
        {
            cout << "Ids of operands of " << label << " are" << operands[0]->getID() << " " << operands[1]->getID() << endl;
            cout << "widths of operands of " << label << " are " << wd1 << " " << wd2 << endl;
            THROW_ERR(op1 < op2);
            //operands[1] = extendExpr(operands[1], wd1, exprImplType, eimInstance, evaluator);
        }
        return -3;
    }
    return -2;
#undef THROW_ERR
}

t_Expression* t_Expression::extendExpr(t_Expression* expr, int width, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator)
{
    static std::string label = "zeroextn";
    TypeOfExpressionTuple typeinfo = *expr->getTypeOfExpression(exprImplType, eimInstance);
    typeinfo.m_width = width;
    t_TypeOfValueStoredInExpressionLabel type = CONSTANT;
    t_Expression* wdExpr;
    {
        std::string strWidth = "0b" + integerToBinaryString(width);
        TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, strWidth.length()};

        wdExpr = createExpression(strWidth, typeInfo, type, exprImplType, eimInstance);
    }
    type = OPERATOR;
    std::vector<t_Expression*> oper = t_ExpressionManager::buildVector(expr, wdExpr);
    
    return createExpression(label, type, typeinfo, oper, exprImplType, eimInstance, evaluator, true);
}

t_Expression* t_Expression::getExprForRead(t_Expression* array_expr, t_Expression* index, t_TypeOfValueStoredInExpressionLabel labelType, TypeOfExpressionTuple typeInfo, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator)
{
    string innerLabel = array_expr->getLabelOfExpression(exprImplType, eimInstance);
    if (innerLabel != "update")
    {
        return NULL; //keep original expression created
    }

    t_TypeOfValueStoredInExpressionLabel indexType = getTypeOfValueStoredInExpressionLabel(index, exprImplType, eimInstance);

    if (indexType == SYMBOL)
    {
        vector<t_Expression*> updateOperands = array_expr->getVectorOfOperands(exprImplType, eimInstance);
        t_TypeOfValueStoredInExpressionLabel updateIndexType = getTypeOfValueStoredInExpressionLabel(updateOperands[1], exprImplType, eimInstance);
        if (updateOperands[1] == index)
        {//if both symbols are same. e.g. read( update(A, i,10), i) --> 10
            return updateOperands[2]; // both the symbols are same in DAG
        }
            //else if( updateIndexType == SYMBOL && index->getLabelOfExpression(exprImplType,eimInstance) == updateOperands[1]->getLabelOfExpression(exprImplType, eimInstance)){
        else if (index->getID() == updateOperands[1]->getID())//(index->checkStructuralEquality(updateOperands[1], exprImplType, eimInstance))
        {
            // both DAGs are symbols and have the same symbol name

            return updateOperands[2];
        }
        else
        {//No such simplification possible
            return NULL;
        }
    }
    else if (indexType == CONSTANT)
    {
        bool array_updated = false;
        //simplify: read( update( update(A, 10, x), 11,y), 10) --> x
        while ("update" == array_expr->getLabelOfExpression(exprImplType, eimInstance))
        {
            vector<t_Expression*> updateOperands = array_expr->getVectorOfOperands(exprImplType, eimInstance);
            t_TypeOfValueStoredInExpressionLabel updateIndexType = getTypeOfValueStoredInExpressionLabel(updateOperands[1], exprImplType, eimInstance);
            if (updateIndexType == CONSTANT)
            {
                //If both values constant
                //Check if both indices same
                if (index == updateOperands[1])
                {
                    return updateOperands[2];
                }
                //or their values match
                int readIndexVal = index->getConstantValuePresentInExpression(exprImplType, eimInstance);
                int updateIndexVal = updateOperands[1]->getConstantValuePresentInExpression(exprImplType, eimInstance);
                //else if(index->getActualExpressionValueAsString(exprImplType,eimInstance) == updateOperands[1]->getActualExpressionValueAsString(exprImplType,eimInstance)){
                //if(index->checkStructuralEquality(updateOperands[1],exprImplType, eimInstance))
                if (readIndexVal == -1 || updateIndexVal == -1)
                {
                    break; //indices not constant
                }
                else if (readIndexVal == updateIndexVal) //both indices equal
                {
                    return updateOperands[2];
                }
                else
                {
                    //this update expression does not modify the read result. so remove from expression
                    array_expr = updateOperands[0];
                    array_updated = true;
                }
            }
            else// if( updateIndexType == SYMBOL )
            {//No simplification possible.
                break;
            }

        }//end while

        if (array_updated)
        {//rebuild the new expression with modified array_expr
            vector<t_Expression*> operands(2, NULL);
            operands[0] = array_expr;
            operands[1] = index;
            string expr_label = "read";
            return createExpression(expr_label, labelType, typeInfo, operands, exprImplType, eimInstance, evaluator, false);
        }
    }
    else
    {
        vector<t_Expression*> operands = array_expr->getVectorOfOperands(exprImplType, eimInstance);
        if (index->getID() == operands[1]->getID())
        {
            //index of read is the same as the index of inner update
            return operands[2];
        }
    }
    //No simplifiacation possible.
    return NULL;

}

pair<bool, t_Expression*> t_Expression::getArrayExprForUpdate(t_Expression* array_expr, t_Expression* index, int expressionImplementationType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator)
{
    if (array_expr == NULL)
    {
        cout << "ERROR::"__FILE__":" << __LINE__ << ":Array Expression of update expression cannot be null." << endl;
        return make_pair(false, array_expr);
    }
    string label = array_expr->getLabelOfExpression(expressionImplementationType, eimInstance);
    if (label != "update")
    {
        return make_pair(false, array_expr);
    }

    //update expression found.
    vector<t_Expression*> operands = array_expr->getVectorOfOperands(expressionImplementationType, eimInstance);

    if (operands[1] == index)
    {//if we have the same index as the root update
        return make_pair(true, operands[0]); //remove this expression from current DAG
    }
    //our index is not same as the given index, check for same in our first operand

    pair<bool, t_Expression*> result = getArrayExprForUpdate(operands[0], index, expressionImplementationType, eimInstance, evaluator);

    if (result.first == false)
    {//No modification in my children. so no change in this expression
        return make_pair(false, array_expr);
    }

    //Some modification in child. recreate the expression.
    t_TypeOfValueStoredInExpressionLabel labelType = getTypeOfValueStoredInExpressionLabel(array_expr, expressionImplementationType, eimInstance);
    TypeOfExpressionTuple *typeInfoPtr = array_expr->getTypeOfExpression(expressionImplementationType, eimInstance);
    TypeOfExpressionTuple typeInfo = *typeInfoPtr;
    operands[0] = result.second;
    t_Expression* new_expr = createExpression(label, labelType, typeInfo, operands, expressionImplementationType, eimInstance, evaluator, false); //false because we do
    //not want to again optimize this expression further. It is already optimized, simply create it. Otherwise we enter into useless loop of optimising the 
    //optimized expression.

    return make_pair(true, new_expr);
}

t_Expression* t_Expression::unknownIndexExpression = NULL;
t_Expression* t_Expression::unknownIndexInvalidBitExpression = NULL;

t_Expression* t_Expression::getUnknownIndexExpression(int exprImplType, t_ExpressionImplementationManager* eimInstance)
{
    if (unknownIndexExpression != NULL)
        return unknownIndexExpression;

    //not already created so create an unknown expression.
    { //create the unknown index. We make it a symbol
        std::string label = "__Update_simplify_unknown_index__";
        t_TypeOfValueStoredInExpressionLabel labelType = SYMBOL;
        TypeOfExpressionTuple typeTuple = {TYPE_UNSIGNED_INTEGER, -1};
        unknownIndexExpression = createExpression(label, typeTuple, labelType, exprImplType, eimInstance);
    }

    { //create the invalid bit if the unknown index. We set it to constant 1.
        std::string new_label_with_type = "0b1";
        t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
        TypeOfExpressionTuple typeTuple = {TYPE_UNSIGNED_BITVECTOR, 1};
        unknownIndexInvalidBitExpression = createExpression(new_label_with_type, typeTuple, labelType, exprImplType, eimInstance);
    }

    return unknownIndexExpression;
}

const std::string t_Expression::updateUnknownSymbol("__Update_simplify_symbol__");

t_Expression* t_Expression::getUpdateSymbol(int exprImplType, t_ExpressionImplementationManager* eimInstance, t_Expression* sizeExpr)
{
    /*if(updateUnknownSymbol != NULL)
        return updateUnknownSymbol;
    
    //else create the symbol, cache and return
    std::string label = "__Update_simplify_symbol__";
    t_TypeOfValueStoredInExpressionLabel labelType = SYMBOL;
    TypeOfExpressionTuple typeTuple = {TYPE_UNSIGNED_INTEGER, -1};
    updateUnknownSymbol = createExpression(label, typeTuple, labelType, exprImplType, eimInstance);
    return updateUnknownSymbol;
     */
    static int counter = 0; //make unique because each update may have different sizes
    ++counter;
    string label = updateUnknownSymbol + integerToString(counter);
    t_Expression* expr = createUnknownMemoryArraySimilarTo(label, SYMBOL, sizeExpr, exprImplType, eimInstance);
    {
        ofstream temp("UpdateSimplify", ios::app);
        temp << "--------------------" << endl;
        expr->printExpressionToFileAsDAG(label, exprImplType, eimInstance, &temp);
    }
    return expr;
}

pair<string, t_Expression*> t_Expression::getUpdateSymbolNameAndMemoryInExpression(int exprImplType, t_ExpressionImplementationManager* eimInstance)
{
    t_Expression* expr = this;
    for (string label = expr->getLabelOfExpression(exprImplType, eimInstance);
            label == "update";
            label = expr->getLabelOfExpression(exprImplType, eimInstance))
    {
        vector<t_Expression*> operands = expr->getVectorOfOperands(exprImplType, eimInstance);
        string indexLabel = operands[1]->getLabelOfExpression(exprImplType, eimInstance);
        if (indexLabel.find(updateUnknownSymbol) == 0)
        {
            return make_pair(indexLabel, operands[0]);
        }
        expr = operands[0]; //go one level below
    }

    //not found
    return make_pair("", expr);
}

t_Expression* t_Expression::createUnknownMemoryArraySimilarTo(string label, t_TypeOfValueStoredInExpressionLabel labelType, t_Expression* expr, int exprImplType, t_ExpressionImplementationManager *eimInstance)
{
    //uValueExpression = expression_manager->createConstant(uMemoryLabel, typeInfo);
    //    t_MemoryArrayBoundsInfo* memBounds = cdfg->getMemoryVariableDimensions(atom->getSignalNameOfAtom());
    //    expression_manager->setMemoryExpressionBounds(uValueExpression, *memBounds);


    /*t_MemoryDataType* memBounds=NULL;
    //throw logic_error(__FILE__+":"+integerToString(__LINE__)+": Not yet implemented: cannot extract bounds info from expr");
    
    {
        t_ExpressionValue* exprValue = expr->getValue(exprImplType, eimInstance);
        if(exprValue == NULL || exprValue->m_typeInfo.m_type!= TYPE_MEMORY_ARRAY)
        {
            //throw logic_error("Invalid expression passed.");
            //value is a simple value and not a memory. so create unknown value of that size.
            TypeOfExpressionTuple typeInfo = *(expr->getTypeOfExpression(exprImplType, eimInstance));
            t_Expression* result;
            if(labelType == CONSTANT && typeInfo.m_width > -1)
            {
                label = getXOrZValueOfLength(t_ExpressionManager::getSingleton()->getWidth(expr), "u");
                result = t_ExpressionManager::getSingleton()->createConstant(label, typeInfo);
            }
            else
            {
                result = createExpression(label,typeInfo, labelType, exprImplType, eimInstance);
            }
            //Now make the expression invalid by setting the invalid bit.
            t_ExpressionManager::getSingleton()->setInvalidBitExpression(result, t_ExpressionManager::getSingleton()->createABitVectorConstantOneExpression());
            return result;
        }
        
        memBounds = exprValue->m_actualExpressionValueStored.exprvalueMemory;
        
        if(memBounds == NULL)
        {
            throw runtime_error("No valid memory bounds found in expr");
        }
    }*/
    if (expr->getTypeOfExpression(exprImplType, eimInstance)->m_type != TYPE_MEMORY_ARRAY)
    {
        assert(expr->getTypeOfExpression(exprImplType, eimInstance)->m_width > 0);
        string label = getXOrZValueOfLength(t_ExpressionManager::getSingleton()->getWidth(expr), "u");
        t_Expression* uValueExpression = t_ExpressionManager::getSingleton()->createConstant(label, *(expr->getTypeOfExpression(exprImplType, eimInstance)));
        t_ExpressionManager::getSingleton()->setInvalidBitExpression(uValueExpression, t_ExpressionManager::getSingleton()->createABitVectorConstantOneExpression());
        return uValueExpression;
    }
    pair<vector<t_MemoryBounds>, int> res = t_ExpressionManager::getSingleton()->getBoundsOfArray(expr);

    TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
    //std::string label = updateUnknownSymbol;
    t_Expression* uValueExpression = createExpression(label, typeInfo, labelType, exprImplType, eimInstance);

    t_ExpressionValue* value = uValueExpression->getValue(exprImplType, eimInstance);
    if (value == NULL)
    {
        cout << "************"__FILE__":" << __LINE__ << ": expression value not set for memory." << endl;
        return uValueExpression;
    }
    if (value->m_typeInfo.m_type != TYPE_MEMORY_ARRAY)
    {
        throw std::runtime_error("Invalid value for memory array:" + label);
    }

    t_MemoryDataType* t = new t_ListOfMemoryEntries();
    t->m_bounds = new vector<t_MemoryBounds > (res.first.begin() + res.second, res.first.end()); //new vector<t_MemoryBounds>(* memBounds->m_bounds);
    //t_MemoryBounds temp;
    //t->m_bounds->resize(memBounds->m_bounds.size(),temp);
    //copy(memBounds->m_bounds.begin(), memBounds->m_bounds.end(), t->m_bounds->begin());
    value->m_actualExpressionValueStored.exprvalueMemory = t;
    //Now make the expression invalid by setting the invalid bit.
    t_ExpressionManager::getSingleton()->setInvalidBitExpression(uValueExpression, t_ExpressionManager::getSingleton()->createABitVectorConstantOneExpression());
    return uValueExpression;
}

t_Expression* t_Expression::getNewUpdateValue(t_Expression* oldValue, t_Expression* newValue, bool alreadyITE, int exprImplType, t_ExpressionImplementationManager* eimInstance, t_ExpressionEvaluator* evaluator)
{
    t_Expression* originalValue = oldValue;
    t_Expression* condition = NULL;
    t_Expression* elseValue = NULL;

    static const string unknownLabel = "__Unknown_";

    if (alreadyITE)
    {
        if (oldValue->getLabelOfExpression(exprImplType, eimInstance) == "ite")
        {
            vector<t_Expression*> operands = oldValue->getVectorOfOperands(exprImplType, eimInstance);
            condition = operands[0];
            originalValue = operands[1];
            elseValue = operands[2];
            {
                string label = elseValue->getLabelOfExpression(exprImplType, eimInstance);
                //Only works if the the else part is kept as a symbol.
                if (label.find(unknownLabel) != 0)//does not begin with __Unknown_
                {
                    cout << __FILE__":" << __LINE__ << ": Not my ite expression:" << label << endl;
                    //reset to original values
                    originalValue = oldValue;
                    condition = NULL;
                    elseValue = NULL;
                    alreadyITE = false;
                }
            }
        }
        else
        {
            cout << __FILE__":" << __LINE__ << ": Ite expression expected, but not present." << endl;
            //Check whether Due to optimization like ite(1=0, 1, X) => X
            if (oldValue->getLabelOfExpression(exprImplType, eimInstance).find(unknownLabel) == 0)//optimized value.
            {
                return oldValue; // since this was optimized to unknown, any addition in condition will still evaluate to unknown.
            }
            //reset values
            originalValue = oldValue;
            condition = NULL;
            elseValue = NULL;
            alreadyITE = false;
        }
    }

    /*if(alreadyITE && oldValue->getLabelOfExpression(exprImplType, eimInstance) == "ite")
    {
        vector<t_Expression*> operands = oldValue->getVectorOfOperands(exprImplType, eimInstance);
        if(operands[2]->getLabelOfExpression(exprImplType, eimInstance).find(unknownLabel) == 0)
        {
            condition = operands[0];
            originalValue = operands[1];
            elseValue = operands[2];
        }
        else
        {
            cout<<__FILE__":"<<__LINE__<<": Not my ite expression."<<endl;
        }
    }*/
    //build originalvalue == newvalue
    t_Expression* origEqNew;
    {
        vector<t_Expression*> operands = t_ExpressionManager::buildVector(originalValue, newValue);
        t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
        TypeOfExpressionTuple typeTuple = {TYPE_UNSIGNED_BITVECTOR, 1};
        std::string label = "logeq";

        origEqNew = createExpression(label, labelType, typeTuple, operands, exprImplType, eimInstance, evaluator);
    }

    //build condition
    if (condition != NULL)
    {
        vector<t_Expression*> operands = t_ExpressionManager::buildVector(origEqNew, condition);
        t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
        TypeOfExpressionTuple typeTuple = {TYPE_UNSIGNED_BITVECTOR, 1};
        std::string label = "logand";

        condition = createExpression(label, labelType, typeTuple, operands, exprImplType, eimInstance, evaluator);
    }
    else
        condition = origEqNew;

    if (elseValue == NULL)
    {
        static int counter = 0;
        string label = unknownLabel + integerToString(counter);
        counter++;
        //Keep this as a symbol so that its easier to check whether the ite was generated by this code or the verilog
        //If made to constant, the label may become 0'bu and the matching fails.
        elseValue = createUnknownMemoryArraySimilarTo(label, SYMBOL, originalValue, exprImplType, eimInstance);
    }

    //Now build the ite and return
    {
        vector<t_Expression*> operands = t_ExpressionManager::buildVector(condition, originalValue, elseValue);
        t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
        //TypeOfExpressionTuple typeTuple = {TYPE_MEMORY_ARRAY, -1};
        TypeOfExpressionTuple typeTuple = *originalValue->getTypeOfExpression(exprImplType, eimInstance);
        std::string label = "ite";
        return createExpression(label, labelType, typeTuple, operands, exprImplType, eimInstance, evaluator);
    }
    //throw logic_error(__FILE__+":"+IntegerToString(__LINE__)+":We should not reach here");
}

pair<t_Expression*, bool> t_Expression::makeUpdateExpression(t_Expression* arrayExpr, t_Expression* index, t_Expression* value, bool booleanVal, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator)
{
    //build the update expression
    string newLabel = "update";
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
    vector<t_Expression*> operands = t_ExpressionManager::buildVector(arrayExpr, index, value);
    t_Expression* newUpdExpr = createExpression(newLabel, labelType, typeInfo, operands, exprImplType, eimInstance, evaluator, false); //no optimizations
    return make_pair(newUpdExpr, booleanVal);
}

pair<t_Expression*, bool> t_Expression::modifyUpdateForUnknownIndex(t_Expression* updExpr, t_Expression* newValue,
                                                                    int exprImplType, t_ExpressionImplementationManager* eimInstance, t_ExpressionEvaluator* evaluator)
{
    std::string label = updExpr->getLabelOfExpression(exprImplType, eimInstance);
    t_Expression* value = NULL;

    if (label != "update")
    {
        //add the ite for update at unknown index
        //        t_Expression* SymEqNewVal;
        //        {
        //            vector<t_Expression*> operands = t_ExpressionManager::buildVector(getUpdateSymbol(exprImplType, eimInstance, newValue),
        //                                                                              newValue);
        //            t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR
        //        }
        //        vector<t_Expression*> operands = t_ExpressionManager::buildVector(updExpr, )
        t_Expression* symbol = getUpdateSymbol(exprImplType, eimInstance, newValue); //size same as newValue
        value = getNewUpdateValue(symbol, newValue, false, exprImplType, eimInstance, evaluator);
        //add the update at unknown index.
        return makeUpdateExpression(updExpr, getUnknownIndexExpression(exprImplType, eimInstance), value, false, exprImplType, eimInstance, evaluator);

    }

    //update expression found
    vector<t_Expression*> operands = updExpr->getVectorOfOperands(exprImplType, eimInstance);
    if (operands[1] == getUnknownIndexExpression(exprImplType, eimInstance))
    {
        //already contains update at unknown index. modify the value.
        value = getNewUpdateValue(operands[2], newValue, true, exprImplType, eimInstance, evaluator);
        return makeUpdateExpression(operands[0], operands[1], value, true, exprImplType, eimInstance, evaluator);
    }
    //else modify the known index update expression
    pair < t_Expression*, bool> result = modifyUpdateForUnknownIndex(operands[0], newValue, exprImplType, eimInstance, evaluator); //evaluate the child updates first
    value = getNewUpdateValue(operands[2], newValue, result.second, exprImplType, eimInstance, evaluator);
    return makeUpdateExpression(result.first, operands[1], value, result.second, exprImplType, eimInstance, evaluator);
}

bool t_Expression::isInvalidExpression(int exprImplType, t_ExpressionImplementationManager* eimInstance)
{
    t_ExpressionManager* em = t_ExpressionManager::getSingleton();
    if (em == NULL)
        return false; //expression manager not found, assume valid expression
    std::string label = getLabelOfExpression(exprImplType, eimInstance);

    //cout<<"checking whether "<<label<<" is invalid."<<endl;

    if (getTypeOfValueStoredInExpressionLabel(this, exprImplType, eimInstance) == CONSTANT && label.find('u') != string::npos)
    {
        //cout<<"label contains 'u' hence invalid."<<endl;
        return true;
    }

    if (getTypeOfValueStoredInExpressionLabel(this, exprImplType, eimInstance) == CONSTANT && label.find('x') != string::npos)
    {
        //cout<<"label contains 'x' hence invalid."<<endl;
        return true;
    }

    if (getTypeOfValueStoredInExpressionLabel(this, exprImplType, eimInstance) == CONSTANT && label.find('z') != string::npos)
    {
        //cout<<"label contains 'x' hence invalid."<<endl;
        return true;
    }

    t_Expression* invBitExpr = em->getInvalidBitExpression(this);
    if (invBitExpr == NULL)
    { //if invalid bit for the expression not found
        //try generating the invalid bit
        t_Expression* temp = this; //generateInvalidBit requires a reference to non-const pointer to t_Expression. Hence we use temp instead of this
        //cout<<"Generating Invalid Bit."<<endl;
        invBitExpr = em->generateInvalidBitExpression(temp);
        if (invBitExpr == NULL)//Could not generate
            return false; //assume valid
    }
    if (invBitExpr->getTypeOfValueStoredInExpressionLabel(invBitExpr, exprImplType, eimInstance) == CONSTANT &&
            invBitExpr->getConstantValuePresentInExpression(exprImplType, eimInstance) == 1)
        return true; //if the invalid bit is constant 1.


    return false; //if invalid bit is not constant 1 assume valid expression
}

pair<vector<int>, bool> t_Expression::getIndicesForRead(t_Expression* arrayExpr, int lb, int ub)
{
    assert(arrayExpr != NULL);
    assert(lb <= ub);
    assert(lb >= 0);

    pair< vector<t_MemoryBounds>, int> res = t_ExpressionManager::getSingleton()->getBoundsOfArray(arrayExpr);
    assert(res.second >= 0);
    assert(res.second <= res.first.size());
    vector<int> selResult;
    vector<t_MemoryBounds> boundsArrayExpr(res.first.begin() + res.second, res.first.end());

    if (boundsArrayExpr.empty())
        return make_pair(selResult, false);

    // for mem[1:0][4:0][1:0] ------>dimWidths is {20,10,2,1}
    vector<unsigned> dimWidths;
    {
        unsigned lastDimWidth = 1;
        for (int i = boundsArrayExpr.size() - 1; i >= 0; --i)
        {
            dimWidths.push_back(lastDimWidth);
            unsigned wd = boundsArrayExpr[i].m_upperbound - boundsArrayExpr[i].m_lowerbound + 1;
            lastDimWidth *= wd;
        }
        dimWidths.push_back(lastDimWidth);
        reverse(dimWidths.begin(), dimWidths.end());
    }

    unsigned width = ub - lb + 1;
    int rem = lb;
    while (dimWidths[ selResult.size() ] > width)                                             
    { //more finer select required
        //unsigned wd = boundsArrayExpr[ selResult.size() ].m_upperbound - boundsArrayExpr[ selResult.size() ].m_lowerbound + 1;
        assert(dimWidths.size() > (selResult.size() + 1));
        unsigned index = rem / dimWidths[selResult.size() + 1]; //index in the current dimension
        rem = rem % dimWidths[ selResult.size() + 1]; //remaining size of the array i.e. read on [4:0][1:0] will result [1:0]

        index += boundsArrayExpr[selResult.size()].m_lowerbound; //case when array starts from non-zero index
        selResult.push_back(index);
    }

    bool isLastDim = (selResult.size() == boundsArrayExpr.size());
    if(isLastDim && width!=1)
    {
        if(t_ConfigurationOptions::getConfig()->m_lastDimensionBitVec)
        {
            return make_pair(selResult, isLastDim);
        }
        copy(dimWidths.begin(), dimWidths.end(), ostream_iterator<unsigned>(cout, "   "));
        cout<<endl;
    }
    assert(isLastDim == (width == 1));
    return make_pair(selResult, isLastDim);
}

t_Expression* t_Expression::modifyExpressionForSelectOfRead(string &label, vector<t_Expression*> &operands, int Expressionimplementationtype,
                                                            t_ExpressionImplementationManager* expressionImplementationManagerInstance,
                                                            t_ExpressionEvaluator* evaluator, bool doSelectConcatSimplification)

{



    //select on a memory
    if (label == "select" && operands[0]->getTypeOfExpression(Expressionimplementationtype, expressionImplementationManagerInstance)->m_type == TYPE_MEMORY_ARRAY)
    {

        int lb = operands[1]->getConstantValuePresentInExpression(Expressionimplementationtype, expressionImplementationManagerInstance);
        int ub = operands[2]->getConstantValuePresentInExpression(Expressionimplementationtype, expressionImplementationManagerInstance);
        assert(lb >= 0 && ub >= 0);

        pair < vector<int>, bool> result = getIndicesForRead(operands[0], lb, ub);
        //assert(!indices.empty());

        vector<int> &indices = result.first;
        bool isLastDim = result.second;
        if (indices.empty())
        {
            return NULL;
        }

//        if (indices.size() == 1 && isLastDim && t_ConfigurationOptions::getConfig()->m_lastDimensionBitVec)
//        {
//            return NULL;
//        }
        t_Expression* arrayExpr = operands[0];
        bool doOptimize = true;
        for (int i = 0; i < indices.size(); ++i)
        {
            string index = integerToBinaryString(indices[i]);
            TypeOfExpressionTuple typeForIndex = {TYPE_UNSIGNED_BITVECTOR, index.length()};
            t_TypeOfValueStoredInExpressionLabel type = CONSTANT;
            index = "0b" + index;
            t_Expression* idx = createExpression(index, typeForIndex, type, Expressionimplementationtype, expressionImplementationManagerInstance);

            string rdLabel = "read";
            vector<t_Expression*> rdOps(2, NULL);
            rdOps[0] = arrayExpr;
            rdOps[1] = idx;
            TypeOfExpressionTuple typeForRead = {TYPE_MEMORY_ARRAY, -1};
            if (i == indices.size() - 1 && isLastDim && t_ConfigurationOptions::getConfig()->m_lastDimensionBitVec)
            {
                rdLabel = "select";
                t_Expression* ubExpr = t_ExpressionManager::getSingleton()->createIntegerConstant(ub);
                t_Expression* lbExpr = t_ExpressionManager::getSingleton()->createIntegerConstant(lb);
                rdOps[1] = lbExpr;
                rdOps.push_back(ubExpr);
                typeForRead.m_type = TYPE_UNSIGNED_BITVECTOR;
                typeForRead.m_width = ub - lb + 1;
                doOptimize = false;
            }


            type = OPERATOR;
            arrayExpr = createExpression(rdLabel, type, typeForRead, rdOps, Expressionimplementationtype, expressionImplementationManagerInstance,
                                         evaluator, doOptimize);

        }
        return arrayExpr;
    }
    return NULL;




}

/*t_Expression* t_Expression::createUnknownMemoryArraySimilarTo(t_Expression* expr)
{
    throw logic_error(__FILE__+":"+integerToString(__LINE__)+":Not yet implemented");
}*/

t_Expression* t_Expression::resizeIndex(t_Expression* array_expr, t_Expression* index, int exprImplType, t_ExpressionImplementationManager *eimInstance, t_ExpressionEvaluator *evaluator)
{
    int reqdWidth;
    try
    {
        pair< vector<t_MemoryBounds>, int> result = t_ExpressionManager::getSingleton()->getBoundsOfArray(array_expr);
        if (result.first.size() <= result.second)
        {//cannot simplify as no bounds data
            return index;
        }

        int ub = result.first[result.second].m_upperbound;
        reqdWidth = (int) ceil(log2(ub + 1));
    }
    catch (runtime_error &err)
    {
        return index;
    }

    int width = t_ExpressionManager::getSingleton()->getWidth(index);

    if (width < reqdWidth)
    {
        string label = "zeroextn";
        t_Expression* reqdWidthExpr;
        {
            string label = integerToBinaryString(reqdWidth);
            TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, label.length()};
            label = "0b" + label;
            t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
            reqdWidthExpr = createExpression(label, typeInfo, labelType, exprImplType, eimInstance);
        }
        vector<t_Expression*> operands(2, NULL);
        operands[0] = index;
        operands[1] = reqdWidthExpr;
        TypeOfExpressionTuple typeInfo = *(index->getTypeOfExpression(exprImplType, eimInstance));
        typeInfo.m_width = reqdWidth;
        t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
        return createExpression(label, labelType, typeInfo, operands, exprImplType, eimInstance, evaluator, true);
    }
    else if (width > reqdWidth)
    {
        t_Expression* lbExpr;
        {
            string label = integerToBinaryString(0);
            TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, label.length()};
            label = "0b" + label;
            t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
            lbExpr = createExpression(label, typeInfo, labelType, exprImplType, eimInstance);
        }
        t_Expression* ubExpr;
        {
            string label = integerToBinaryString(reqdWidth - 1);
            TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, label.length()};
            label = "0b" + label;
            t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
            ubExpr = createExpression(label, typeInfo, labelType, exprImplType, eimInstance);
        }

        string label = "select";
        vector<t_Expression*> operands(3, NULL);
        operands[0] = index;
        operands[1] = lbExpr;
        operands[2] = ubExpr;
        TypeOfExpressionTuple typeInfo = *(index->getTypeOfExpression(exprImplType, eimInstance));
        typeInfo.m_width = reqdWidth;
        t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
        return createExpression(label, labelType, typeInfo, operands, exprImplType, eimInstance, evaluator, true);
    }
    else
        return index;
}



t_Expression* t_Expression::doSimplification(const string &label, vector<t_Expression*> &operands,
        TypeOfExpressionTuple typeInfo, int exprImplType, 
        t_ExpressionImplementationManager* eimInstance, t_ExpressionEvaluator *evaluator)
{
    t_ExpressionManager* em = t_ExpressionManager::getSingleton();
    if(label == em->m_operatorLabelBitwiseAND)
    {
        if(em->isAConstantZeroExpression(operands.at(0)))
            return operands[0];
        else if(em->isAConstantZeroExpression(operands.at(1)))
            return operands[1];
        else if(em->isAConstantOneExpression(operands[0]))
            return operands[1];
        else if(em->isAConstantOneExpression(operands[1]))
            return operands[0];
    }
    else if(label == em->m_operatorLabelBitwiseOR)
    {
        if(em->isAConstantZeroExpression(operands.at(0)))
            return operands[1];
        else if(em->isAConstantZeroExpression(operands.at(1)))
            return operands[0];
        else if(em->isAConstantOneExpression(operands[0]))
            return operands[0];
        else if(em->isAConstantOneExpression(operands[1]))
            return operands[1];
    }
    else if(label == em->m_operatorLabelADD)
    {
        if(em->isAConstantZeroExpression(operands.at(0)))
            return operands.at(1);
        else if(em->isAConstantZeroExpression(operands.at(1)))
            return operands[0];
    }
    else if (label == em->m_operatorLabelSUB)
    {
        if(em->isAConstantZeroExpression(operands.at(1)))
        {
            return operands[0];
        }
    }
    return NULL;
}

/**
 * Method :  createExpression
 * Descritption - For creating the expression that is operator expression (Based on the expression implementation type check whether the expression exists already or not if exists then return already existing expression or call  corresponding createExpression() method )
 * Arguments - label(string)
 *                      typeInfo( TypeOfExpressionTuple) : type of the expression
 *			 labelType(TypeOfValueStoredInExpressionLabel) : the label stored in the label of the expression is either SYMBOL,OPERATOR or CONSTANT
 *			 Expressionimplementationtype(int) : Expression Implemented as DAG_EXPRESSION or Other
 *			 expressionImplementationManagerInstance(ExpressionImplementationManager *) : if Expression implementation is DAG_EXPRESSION then it is dagmanager
 * Returns - New expression created (Expression * - type)
 */
t_Expression *
t_Expression::createExpression(string &label,
                               t_TypeOfValueStoredInExpressionLabel &labelType,
                               TypeOfExpressionTuple &typeInfo,
                               vector<t_Expression *> &operandExpressions,
                               int Expressionimplementationtype,
                               t_ExpressionImplementationManager *&expressionImplementationManagerInstance,
                               t_ExpressionEvaluator* &evaluator, bool doSelectConcatSimplification)
{

    // this hashtable is used to check if an expression is previously created
    // and already simplified or not. if we get the expression inside this
    // hastable this means the expression is already simplified and there we
    // can return without going further for trying to simplify it again
    static t_HashTable<unsigned long long, t_Expression*> simplification_cache;

    //static int counterOfConstantExpressionsEvaluated = 1;
    //    {
    //        ofstream out("labels",ios::app);
    //        out<<label<<endl;
    //    }
    if (Expressionimplementationtype == DAG_EXPRESSION)
    {
        //check whether all operands are constant or not..

        if (doSelectConcatSimplification && (label == "update" || label == "read"))
        {
            operandExpressions[1] = resizeIndex(operandExpressions[0], operandExpressions[1], Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
        }

        vector<t_Expression *>::iterator opIt = operandExpressions.begin();
        bool allOperandsConstant = true;
        int newWd = equalizeOperandWidths(label, operandExpressions, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
        while (opIt != operandExpressions.end())
        {
            t_Expression* currentOperandExpression = (*opIt);
            t_ExpressionValue* exprValueOfThisOperand = currentOperandExpression->getValue(Expressionimplementationtype, expressionImplementationManagerInstance);
            if (exprValueOfThisOperand->m_labelType != CONSTANT)
            {
                allOperandsConstant = false;
                break;
            }
            opIt++;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_Expression *expressionObject = checkIfExpressionExists(label, 
                                                                 operandExpressions, 
                                                                 Expressionimplementationtype, 
                                                                 expressionImplementationManagerInstance);

#define INVALID_DAG_ID ULLONG_MAX
        // this will be used as key in the hashtable
        unsigned long long dag_id=INVALID_DAG_ID;
        
        if (expressionObject == NULL)
        {
            expressionObject = new t_Expression();
            vector<t_ExpressionAsDag *> operandExpressionDags;
            operandExpressionDags.reserve(operandExpressions.size());
            static unsigned long count = 0;
            count++;
            for (vector<t_Expression *>::iterator it = operandExpressions.begin();
                    it != operandExpressions.end(); it++)
            {
                
                t_Expression *expr = (t_Expression *) (*it);
                operandExpressionDags.push_back(expr->m_dagExpression);
            }
            t_ExpressionAsDag * new_expression_as_dag = 
		new t_ExpressionAsDag(label, typeInfo, labelType, operandExpressionDags, expressionObject, dagmanager);

	    // It is possible that an already existing expression was returned, even though checkIfExpressionExists didn't find
	    // the expression.  One way this can happen is as follows:
	    // Suppose we are creating the expression logor(0b0, xyz).  Suppose this expression was never created earlier,
	    // although xyz as an expression was created earlier.  So CheckIfExpressionExists will not find the expression
	    // logor(0b0, xyz).  However, the constructor for t_ExpressionAsDag will ask for a new DAG node to be created
	    // with label "logor" and children/operands DAG nodes with labels "0b0" and "xyz".  If there is a rewrite rule
	    // that says "logor(0b0, $e_1) --> $e_1", then in the process of creating the above DAG node, the already
	    // existing DAG node with label "xyz" will be retuned.  Since the expression xyz was already created earlier,
	    // the DAG node with label "xyz" will point back to that earlier expression.   So, now we will have the following
	    // situation:
	    // new_expression_as_dag->m_dag gives the dag node with label "xyz".  However, this dag node's value field, when
	    // typecast to (t_ExpressionValue *), has a pointer to the earlier expression for xyz in its m_backPointerToExpression
	    // member.  This is of course different from expressionObject.  We rectify this situation by comparing the two
	    // expression pointers for equality.

	    if (((t_ExpressionValue *) (new_expression_as_dag->m_dag->getValue()))->m_backPointerToExpression != expressionObject) {
		delete expressionObject;
		expressionObject = ((t_ExpressionValue *) (new_expression_as_dag->m_dag->getValue()))->m_backPointerToExpression;
		delete new_expression_as_dag;
		new_expression_as_dag = expressionObject->m_dagExpression;
	    }
	    else {
		// Newly created expression
		expressionObject->m_dagExpression = new_expression_as_dag;
		
		expressionObject->m_validValue = false;
		
		if (newWd >= -1)
		    {
			if (newWd != typeInfo.m_width)
			    {
				cout << "Passed width(" << typeInfo.m_width << ") is not same inferred width(" << newWd << ")" <<
				    "ID is " << expressionObject->getID() << endl;
			    }
		    }
		
		if (newWd > 0 && typeInfo.m_width > 0 && newWd != typeInfo.m_width)
		    {
			cout << "Check this case" << endl;
		    }
		if (!(newWd > 0 && typeInfo.m_width == -1))
		    {
			newWd = typeInfo.m_width;
		    }
		expressionObject->m_inferredWidth = newWd;
		expressionObject->m_isRestructured = false;
            }
	    // Expression ID of a newly created expression or an already existing expression obtained by rewrite rule 
	    // based simplification.
            dag_id = expressionObject->getID();
        }
        
        //Check if simplified to some thing in map. t_HashTable<unsigned long long, t_Expression*>
        // If in map. return the simplified version
        // else.
        //2 cases: 1. expression already created and cannot be simplified. Return as it is
        //         2. Newly created. Go further to simplify.
        
        // the expression already exists
        if (dag_id == INVALID_DAG_ID)
        {
            t_HTStatusValue<t_Expression*> result=simplification_cache.hashtable_search(expressionObject->getID());
            
            if (result.success())
            {
                // simplified expression already exists, no need for going further
            // return the simplified expression from the cache
                return result.getValue();
            }
            else
            {
                // no simplification can be done for this expression otherwise
                // it would have already been present in the cache, return the
                // object directly
                return expressionObject;
            }
        }
#undef INVALID_DAG_ID

// macro which populates the simplification cache
#define ADD_TO_CACHE(expr)                                                      \
        if(expr!=NULL && dag_id!= (expr)->getID())                              \
                simplification_cache.hashtable_insert(dag_id, (expr))           \
        
#ifdef DEBUG
       
        //verify expressions
        if (0)
        {
            if (label == "add" || label == "sub")
            {
                if (t_ExpressionManager::getSingleton()->getWidth(operandExpressions[0]) !=
                        t_ExpressionManager::getSingleton()->getWidth(operandExpressions[1]))
                {
                    assert(!"Invalid add expression created. size of operands do not match");
                }
            }

            else if (label == "select")
            {
                int wd = t_ExpressionManager::getSingleton()->getWidth(operandExpressions[0]);
                if (wd != -1)
                {
                    int ub = t_ExpressionManager::getSingleton()->getConstantValuePresentInExpression(operandExpressions[1]);
                    int lb = t_ExpressionManager::getSingleton()->getConstantValuePresentInExpression(operandExpressions[2]);

                    if (wd < ub || wd < lb || ub < lb)
                    {
                        assert(!"Invalid select expression.");
                    }
                }
            }
        }
#endif
        assert((label!="signextn" && label != "zeroextn")|| t_ExpressionManager::getSingleton()->getTypeOfValueStoredInExpressionLabel(operandExpressions[1]) == CONSTANT);
        bool printExpr = true;
        if (doSelectConcatSimplification && t_ConfigurationOptions::getConfig()->m_convertSelectOnMemoryToReads == true && label == "select")
        {
            if (operandExpressions[0]->getTypeOfExpression(Expressionimplementationtype, expressionImplementationManagerInstance)->m_type == TYPE_MEMORY_ARRAY)
            {
                expressionObject->printExpressionToFileAsDAG("selectExpr", Expressionimplementationtype, expressionImplementationManagerInstance, cout);
            }
            t_Expression* retExpr = modifyExpressionForSelectOfRead(label, operandExpressions, Expressionimplementationtype, expressionImplementationManagerInstance,
                                                                    evaluator, doSelectConcatSimplification);
            if (retExpr)
            {
                retExpr->printExpressionToFileAsDAG("readExpr", Expressionimplementationtype, expressionImplementationManagerInstance, cout);
                expressionObject = retExpr;

            }
        }

        if(doSelectConcatSimplification)
        {
            t_Expression* expr = expressionObject->doSimplification(label, operandExpressions,typeInfo, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
            if(expr!=NULL)
                expressionObject = expr;
        }
        
        #ifdef QE //#ifdef added by Ajith John on 9 June 2013, changed on 12 Jan 2014
	if (allOperandsConstant && label != "kadd" && !(label[0] == 'f' && label[1] == '_')) // && label != "kadd" is added by Ajith John
	// !(label[0] == 'f' && label[0] == '_') is added by Ajith John on 12 Jan 2014
	#else
        if (allOperandsConstant)
	#endif
        {
            if(label == "genInv")
            {
                ADD_TO_CACHE(expressionObject);
                return expressionObject;//t_ExpressionManager::getSingleton()->generateInvalidBitExpression(operandExpressions.at(0));
            }
            
            t_ExpressionValue* valueOfEvaluatedOperator = expressionObject->evaluateExpression(Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
            string labelOfConstant;
            
            //labelOfConstant= CONSTANT_VALUE_LABEL+toString(counterOfConstantExpressionsEvaluated++);
            labelOfConstant = "0b" + valueOfEvaluatedOperator->getActualValueAsString();
            t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
            //expressionObject->removeExpression(Expressionimplementationtype,expressionImplementationManagerInstance);
            expressionObject = createExpression(labelOfConstant, typeInfo, labelType, Expressionimplementationtype, expressionImplementationManagerInstance);
            expressionObject->setValue(valueOfEvaluatedOperator, false,
                                       Expressionimplementationtype,
                                       expressionImplementationManagerInstance,
                                       evaluator);
            assert(typeInfo.m_width == -1 || typeInfo.m_width == t_ExpressionManager::getSingleton()->getWidth(expressionObject));
            //expressionObject->m_validValue = true;
        }
        else if (doSelectConcatSimplification && label == "update")
        {


            //Do the following if the index is unknown
            t_Expression* index = operandExpressions[1];
            if (index->isInvalidExpression(Expressionimplementationtype, expressionImplementationManagerInstance))
            {
                cout << "***** Update at unknown index. Simplifying..." << endl;
                pair < t_Expression*, bool> result = modifyUpdateForUnknownIndex(operandExpressions[0], operandExpressions[2], Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
                //ignore the bool value. its not required, its used for recursion
                expressionObject = result.first;
                //Update expression created
                if (printExpr)
                {
                    ofstream temp("UpdateSimplify", ios::app);
                    temp << "----------------------------------------------" << endl;
                    //temp<<"Unknown index"<<endl;
                    string arrExpr("arrayExpr");
                    operandExpressions[0]->printExpressionToFileAsDAG(arrExpr, Expressionimplementationtype, expressionImplementationManagerInstance, &temp);
                    string unkName("Unknown_index");
                    operandExpressions[1]->printExpressionToFileAsDAG(unkName, Expressionimplementationtype, expressionImplementationManagerInstance, &temp);
                    string value("Orig_value");
                    operandExpressions[2]->printExpressionToFileAsDAG(value, Expressionimplementationtype, expressionImplementationManagerInstance, &temp);
                    string name("update_expr");
                    expressionObject->printExpressionToFileAsDAG(name, Expressionimplementationtype, expressionImplementationManagerInstance, &temp);

                }
            }
            else
            {
                //else do the following
                pair<bool, t_Expression*> result = getArrayExprForUpdate(operandExpressions[0], operandExpressions[1], Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
                if (result.first == true)
                { //Some modification in first operand.
                    operandExpressions[0] = result.second; //use modified operand
                    expressionObject = createExpression(label, labelType, typeInfo, operandExpressions, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator, false); //no simplification needed. already simplified.
                }
                if (printExpr)
                {
                    ofstream temp("UpdateSimplify", ios::app);
                    temp << "----------------------------------------------" << endl;
                    temp << "known index: modified:" << boolalpha << result.first << endl;
                    string name("update_expr");
                    expressionObject->printExpressionToFileAsDAG(name, Expressionimplementationtype, expressionImplementationManagerInstance, &temp);

                }
            }
        }
        else if (doSelectConcatSimplification && label == "read")
        {

            if (operandExpressions[1]->isInvalidExpression(Expressionimplementationtype, expressionImplementationManagerInstance) == false)
            {
                t_Expression* res = getExprForRead(operandExpressions[0], operandExpressions[1], labelType, typeInfo, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
                if (res != NULL)
                    expressionObject = res;
            }
        }
            /** Following code nearly 140 lines corresponds to
             *  Another Simplification of expressions
             *  select ( concat ( a[2:0], b [2:0] , c[1:0] ) ,4,0) ==> select ( concat (b[2:0],c[1:0]),0,4)
             *  select ( concat ( a[2:0], b [2:0] , c[1:0] ) ,1,0) ==> c[1:0]
             *  rajkumar@8-04-2011
             */
        else if (label == "select" && doSelectConcatSimplification == true)
        {
            opIt = operandExpressions.begin();
            int lowerBoundInSelect;
            int upperBoundInSelect;

            //Getting width of operand0
            t_ExpressionValue* valueInOperand0 = (*opIt)->getValue(Expressionimplementationtype, expressionImplementationManagerInstance);
            int width = valueInOperand0->getWidth();
            // Get The Lower Bound expression and integer value in it
            opIt++;
            t_Expression* lowerBoundExpressionInSelect = (*opIt);
            t_ExpressionValue* lowerBoundValueInSelect = lowerBoundExpressionInSelect->evaluateExpressionRecursively(Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
            lowerBoundInSelect = lowerBoundValueInSelect->get4BitBitVectorValueAsInteger();
            if (lowerBoundInSelect == -1)
            {
                ADD_TO_CACHE(expressionObject);
                return expressionObject;
            }
            // Get The Upper Bound expression and integer value in it
            opIt++;
            t_Expression* upperBoundExpressionInSelect = (*opIt);
            t_ExpressionValue* upperBoundValueInSelect = upperBoundExpressionInSelect->evaluateExpressionRecursively(Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
            upperBoundInSelect = upperBoundValueInSelect->get4BitBitVectorValueAsInteger();
            if (upperBoundInSelect == -1)
            {
                ADD_TO_CACHE(expressionObject);
                return expressionObject;
            }


            if (upperBoundInSelect < lowerBoundInSelect)
            {
                cerr << "*****************\n*****************\n"
                        " Expression.cc: select expression creation called with bounds inverted " << endl;
                int temp = upperBoundInSelect;
                upperBoundInSelect = lowerBoundInSelect;
                lowerBoundInSelect = temp;
            }

            opIt--;
            opIt--;
            if (lowerBoundInSelect == 0 && upperBoundInSelect == width - 1)
            {
                ADD_TO_CACHE(*opIt);
                return (*opIt);
            }
            //select of select optimization
            if ((*opIt)->getLabelOfExpression(Expressionimplementationtype, expressionImplementationManagerInstance) == "select")
            {
                vector<t_Expression*> v_operands_of_inner_select = (*opIt)->getVectorOfOperands(Expressionimplementationtype, expressionImplementationManagerInstance);
                int ub_inner_select = v_operands_of_inner_select[2]->getConstantValuePresentInExpression(Expressionimplementationtype, expressionImplementationManagerInstance);
                int lb_inner_select = v_operands_of_inner_select[1]->getConstantValuePresentInExpression(Expressionimplementationtype, expressionImplementationManagerInstance);

                string new_ub = toString(lb_inner_select + upperBoundInSelect);
                string new_lb = toString(lb_inner_select + lowerBoundInSelect);
                TypeOfExpressionTuple typeinfo_int = {TYPE_SIGNED_INTEGER, -1};
                t_TypeOfValueStoredInExpressionLabel constant_type = CONSTANT;
                t_Expression* ub_expr = createExpression(new_ub, typeinfo_int, constant_type, Expressionimplementationtype, expressionImplementationManagerInstance);
                t_Expression* lb_expr = createExpression(new_lb, typeinfo_int, constant_type, Expressionimplementationtype, expressionImplementationManagerInstance);
                vector<t_Expression*> v_operands_of_new_select(3, NULL);
                v_operands_of_new_select[0] = v_operands_of_inner_select[0];
                v_operands_of_new_select[2] = ub_expr;
                v_operands_of_new_select[1] = lb_expr;
                t_Expression* result = createExpression(label, labelType, typeInfo, v_operands_of_new_select, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator, true);
                expressionObject->m_dagExpression->m_dag = result->m_dagExpression->m_dag;
                expressionObject->m_dagExpression->m_dag->setValue(result->m_dagExpression->m_dag->getValue());
                expressionObject = result;
                ADD_TO_CACHE(result);
                return result;
            }
                // select of concats optimization.
            else if (operandExpressions[0]->getLabelOfExpression(Expressionimplementationtype, expressionImplementationManagerInstance) == "concat")
            {
                t_Expression* result = createASelectOverConcat(operandExpressions[0], upperBoundInSelect, lowerBoundInSelect, typeInfo, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
                if (result != NULL)
                {
                    //expressionObject->m_dagExpression->m_dag = result->m_dagExpression->m_dag;	      
                    //expressionObject->m_dagExpression->m_dag->setValue(result->m_dagExpression->m_dag->getValue());	      
                    expressionObject = result;
                    ADD_TO_CACHE(result);
                    return result;
                }
            }
        } /*Concat of select*/
        else if (label == "concat" && doSelectConcatSimplification == true)
        {
            vector<t_Expression*>::iterator it = operandExpressions.begin();
            string newSelectLabel = "select";
            string newConcatLabel = "concat";
            vector<t_Expression*> newOperands;
            bool isAnyOperandChanged = false;
            while (it != operandExpressions.end())
            {
                if (newOperands.empty() == true)
                {
                    newOperands.push_back(*it);
                }
                else if ((*it)->m_dagExpression->getLabelOfExpressionNode() == "select")
                {
                    t_Expression* prevOperand = newOperands.back();
                    newOperands.pop_back();
                    if (prevOperand->m_dagExpression->getLabelOfExpressionNode() == "select")
                    {
                        vector<t_Expression*> currInnerOperands = (*it)->getVectorOfOperands(Expressionimplementationtype, expressionImplementationManagerInstance);
                        vector<t_Expression*> prevInnerOperands = prevOperand->getVectorOfOperands(Expressionimplementationtype, expressionImplementationManagerInstance);

                        if (currInnerOperands[0] == prevInnerOperands[0])
                        {
                            t_ExpressionValue* lbInPrevOperand = prevInnerOperands[1]->evaluateExpressionRecursively(Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);

                            int previousLB = lbInPrevOperand->get4BitBitVectorValueAsInteger();
                            if (previousLB == -1)
                            {
                                newOperands.push_back(prevOperand);
                                newOperands.push_back(*it);
                                it++;
                                continue;
                            }

                            t_ExpressionValue* ubInCurrOperand = currInnerOperands[2]->evaluateExpressionRecursively(Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);

                            int currUB = ubInCurrOperand->get4BitBitVectorValueAsInteger();

                            if (currUB == -1)
                            {
                                newOperands.push_back(prevOperand);
                                newOperands.push_back(*it);
                                it++;
                                continue;
                            }

                            if (currUB == previousLB - 1)
                            {
                                t_ExpressionValue* currOperandValue = (*it)->getValue(Expressionimplementationtype, expressionImplementationManagerInstance);
                                t_ExpressionValue* prevOperandValue = prevOperand->getValue(Expressionimplementationtype, expressionImplementationManagerInstance);

                                int sumOfWidth = currOperandValue->getWidth() + prevOperandValue->getWidth();

                                vector<t_Expression*> mergeSelectOperands;
                                mergeSelectOperands.push_back(currInnerOperands[0]);
                                mergeSelectOperands.push_back(currInnerOperands[1]);
                                mergeSelectOperands.push_back(prevInnerOperands[2]);
                                t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
                                TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, sumOfWidth};
                                t_Expression* newSelect = createExpression(newSelectLabel, labelType, typeInfo, mergeSelectOperands, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
                                newOperands.push_back(newSelect);
                                isAnyOperandChanged = true;
                            }
                            else
                            {
                                newOperands.push_back(prevOperand);
                                newOperands.push_back(*it);
                            }
                        } //push both

                        else
                        {
                            newOperands.push_back(prevOperand);
                            newOperands.push_back(*it);
                        }
                    }
                    else
                    {
                        //push both
                        newOperands.push_back(prevOperand);
                        newOperands.push_back(*it);
                    }
                }
                else
                {
                    newOperands.push_back(*it);
                }
                it++;
            }//while ends

            if (newOperands.size() == 1)
            {
                ADD_TO_CACHE(newOperands[0]);
                return newOperands[0];
            }
            else if (isAnyOperandChanged == true)
            {
                t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;

                vector<t_ExpressionAsDag *> operandExpressionDags;
                for (vector<t_Expression *>::iterator it = newOperands.begin();
                        it != newOperands.end(); it++)
                {
                    t_Expression *expr = (t_Expression *) (*it);
                    operandExpressionDags.push_back(expr->m_dagExpression);
                }
                t_Expression* expressionObject = new t_Expression();
                t_ExpressionAsDag * new_expression_as_dag = new t_ExpressionAsDag(newConcatLabel, typeInfo, labelType, operandExpressionDags, expressionObject, dagmanager);
                expressionObject->m_dagExpression = new_expression_as_dag;
                expressionObject->m_validValue = false;
                expressionObject->m_inferredWidth = typeInfo.m_width;
                expressionObject->m_isRestructured = false;
                ADD_TO_CACHE(expressionObject);
                return expressionObject;
            }
        }//else if label of concat ends  */
        ADD_TO_CACHE(expressionObject);
        return expressionObject;
        
#undef ADD_TO_CACHE
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to create Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }

}

/**
 * Default Constructor
 * 
 * Initialize member variables to default values
 */
t_Expression::t_Expression()
{
    m_simplified = false; // Current expression is simplified or not flag
    m_dagExpression = NULL; // To store expression as a dag expression
    m_tempName = ""; // To store temporary name of the expression while printing
    m_inferredWidth = 0; // Inferred width for the current expression
    m_validValue = false;  // This expression is having assigned a value or not
}

/**
 * Method - checkIfExpressionExists
 * Arguments - label (string)  Label of the expression that is to be searched for
 *                      expressionImplementationType (int) DAG_EXPRESSION or implementation of the expression of type
 *                      expressionImplementationManagerInstance (ExpressionImplementationManager * ) if expression implementation type is DAG_EXPRESSION then it is dagmanager
 * Description - if expression Implementation is DAG_EXPRESSION then it will search in the dagmanager for the given expression
 * Returns - If expression is already created then it will  return corresponding expression object
 */
t_Expression * t_Expression::checkIfExpressionExists
(
 string &label,
 int expressionImplementationType,
 t_ExpressionImplementationManager *&expressionImplementationManagerInstance
 )
{
    t_Logger *logManager = t_Logger::getInstance();
    //logManager->LOG(" Checking if expression already exists:" + label, m_logFile, c_DebugLevelVerbosity);
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_DAGNode * dag_node = dagmanager->testAndReturnDAGNodeIfItExists(label);
        if (dag_node != NULL)
        {
            t_ExpressionValue *valueInExpression = (t_ExpressionValue *) dag_node->getValue();
            if (valueInExpression == NULL)
            {
                //logManager->LOG(" Expression:" + label + " is already created But value is NULL", m_logFile, c_RunLevelVerbosity);
                return NULL;
            }
            else
            {
                t_Expression * expression_in_dag_value = valueInExpression->m_backPointerToExpression;
                return expression_in_dag_value;
            }
        }
        else
        {
            // logManager->LOG("Expression:" + label + " is Not already exists", m_logFile, c_RunLevelVerbosity);
            return NULL;
        }
    }
    else
    {

        // logManager->LOG("Trying to create Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

/**
 * Method - checkIfExpressionExists()
 * Arguments - label (string)  Label of the expression that is to be searched for
 *                      operandExpressions (vector<Expression *> ) operand expressions for the expression with label
 *                      expressionImplementationType (int) DAG_EXPRESSION or implementation of the expression of type
 *                      expressionImplementationManagerInstance (ExpressionImplementationManager * ) if expression implementation type is DAG_EXPRESSION then it is dagmanager
 * Description - if expression Implementation is DAG_EXPRESSION then it will search in the dagmanager for the given expression with given operands
 * Returns - If expression is already created then it will  return corresponding expression object
 */
t_Expression *
t_Expression::checkIfExpressionExists(string &label,
                                      vector<t_Expression *> &operandExpressions,
                                      int expressionImplementationType,
                                      t_ExpressionImplementationManager *&expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        vector<t_ExpressionAsDag*, MyAllocator<t_ExpressionAsDag*> > operandExpressionDags;
        vector<t_DAGNode *,MyAllocator<t_DAGNode*> > operandDags;
//        static unsigned long count = 0;
//        ++count;
//        cerr<<"###count is "<<count<<endl;
//        if(count == 3819844)
//        {
//            useMalloc = true;
//        }
        operandExpressionDags.reserve(operandExpressions.size());
        operandDags.reserve(operandExpressions.size());
        
        for (vector<t_Expression*>::iterator it = operandExpressions.begin();
                it != operandExpressions.end(); it++)
        {
            t_Expression *expr = (*it);
            t_ExpressionAsDag *dagExpr = expr->m_dagExpression;
            if (dagExpr == NULL)
            {
                return NULL;
            }
            if (dagExpr->m_dag == NULL)
            {
                return NULL;
            }

            operandExpressionDags.push_back(dagExpr);
            operandDags.push_back(dagExpr->m_dag);
        }
        t_DAGNode * dag_node = dagmanager->testAndReturnDAGNodeIfItExists(
                                                                          label,
                                                                          operandDags
                                                                          );
        if (dag_node != NULL)
        {
            //logManager->LOG(" Expression:" + label + " is already Exists", m_logFile, c_DebugLevelVerbosity);
            t_ExpressionValue *valueInExpression = (t_ExpressionValue *) dag_node->getValue();
            if (valueInExpression == NULL)
            {
                //  logManager->LOG(" Expression:" + label + " is already Exists But with NULL value", m_logFile, c_DebugLevelVerbosity);
                return NULL;
            }
            t_Expression * expression_in_dag_value = valueInExpression->m_backPointerToExpression;
            return expression_in_dag_value;
        }
        else
        {
            //logManager->LOG(" Expression:" + label + " is not Exists", m_logFile, c_DebugLevelVerbosity);
            return NULL;
        }
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

/**
 * Method - checkStructuralEquality()
 * Arguments - e2 (Expression *) : another expression with which the structural equality is to be checked
 *                      expressionType (int) : DAG_EXPRESSION or other
 *                       expressionImplementationManagerInstance (ExpressionImplementationManager *) : if expressionType is DAG_EXPRESSION then it is dagmanager
 * Description - Calls corresponding expression implementation manager for checking structural equality
 * Returns - true if structure of  current and e2 expression structures are same
 *                 false otherwise
 */
bool t_Expression::checkStructuralEquality(t_Expression *e2, int expressionType,
                                           t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        if (m_dagExpression != NULL && e2->m_dagExpression != NULL)
        {
            return m_dagExpression->checkStructuralEquality(e2->m_dagExpression, dagmanager);
        }
        else
        {
            logManager->LOG(" Expression: One of the expressions is NULL in Check structural equality ", m_logFile, c_RunLevelVerbosity);
            return false;
        }
    }
    else
    {
        logManager->LOG("Trying to create Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 * Method - getSizeOfExpression()
 * Arguments - expressionType (int) if expression is of dag expression then it is DAG_EXPRESSION
 *                      expressionImplementationManagerInstance (ExpressionImplementationManager *) if expressionType is DAG_EXPRESSION then it is dagmanager
 * Description - Calls the corresponding expression implementation manager for size
 * Returns - Number (int) of the nodes in the expression
 */
int t_Expression::getSizeOfExpression(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        if (m_dagExpression != NULL)
        {
            return m_dagExpression->getSizeOfExpression(dagmanager);
        }
        else
        {
            logManager->LOG(" Expression :getSizeOfExpression() - dagExpression is NULL ", m_logFile, c_RunLevelVerbosity);
            return 0;
        }
    }
    else
    {
        logManager->LOG("Trying to create Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return 0;
    }
}

/**
 * Function   : removeExpression
 * Arguments: expressionType(int)
 * Description : Deletes the current expression
 * Returns : true if successfully deleted
 */
bool t_Expression::removeExpression(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: removeExpression() - dagExpression is NULL", m_logFile, c_RunLevelVerbosity);
            return false;
        }
        string label = this->getLabelOfExpression(
                                                  expressionType,
                                                  expressionImplementationManagerInstance
                                                  );
        logManager->LOG(" Expression :" + label + " is deleted", m_logFile, c_DebugLevelVerbosity);
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        if (m_dagExpression->removeDagExpression(dagmanager))
        {
            this->~t_Expression();
            return true;
        }
        else
        {
            logManager->LOG("Expression :There is a problem while deleting dagexpression", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 * Destructor
 */
t_Expression::~t_Expression()
{
    t_Logger *logManager = t_Logger::getInstance();
    if (m_dagExpression != NULL) {
        logManager->LOG("Expression :" + m_dagExpression->getLabelOfExpressionNode() + " is being deleted", m_logFile, c_DebugLevelVerbosity);
        cout << "Deleting expression:" << getID() << endl;
        delete m_dagExpression;
    }
}

/**
 * Method  - printExpressionAsYicesExpression
 * Arguments - expression_symbol (string) The expression name to be printed as in the constraint format
 *                      fout (ofstream *) file object to which the expression is to be printed
 *                      expressionImplementationType (int) if the expression is dag expression then it is DAG_EXPRESSION
 *                       expressionImplementationManagerInstance (ExpressionImplementationManager *) : if expressionType is DAG_EXPRESSION then it is dagmanager
 * Description - if expressionImplementationType is DAG_EXPRESSION then Calls the dagExpression's printExpressionAsYicesExpression() with expression_symbol,fout, dagmanager
 for printing
 *Return s - true if printing into file in constraint format is successfully finished
 *                 false otherwise
 */
bool t_Expression::printExpressionAsYicesExpression(string &expression_symbol, ofstream *fout, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: printExpressionAsYicesExpression() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        return m_dagExpression->printExpressionAsYicesExpression(expression_symbol, fout, dagmanager);
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 * Method - printExpressionToFile
 * Arguments - outfile (ofstream *)  file object to which the expression is to be printed
 *                      expressionImplementationType (int) if the expression is dag expression then it is DAG_EXPRESSION
 *                       expressionImplementationManagerInstance (ExpressionImplementationManager *) : if expressionType is DAG_EXPRESSION then it is dagmanager
 * Description - if expressionImplementationType is DAG_EXPRESSION then Calls the dagExpression's printExpressionToFile() with expression_symbol,fout, dagmanager
 for printing
 * Side Effects - Before calling it is necessary to check for the verbosity flag. As this function can increase the logfile entries.
 * Return s - true if printing into file is successfully finished
 *                 false otherwise
 */
bool t_Expression::printExpressionToFile(ofstream *outfile, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        /**/
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: printExpressionToFile() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        m_dagExpression->printExpressionToFile(outfile, dagmanager);
        /***/
        this->printExpressionWithSet(this, outfile, expressionImplementationType, expressionImplementationManagerInstance);
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 *
 */
TypeOfExpressionTuple *t_Expression::getTypeOfExpression(int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: getTypeOfExpression() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        return m_dagExpression->getTypeOfExpression();
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

/**
 *
 */
string t_Expression::getTempName()
{
    return this->m_tempName;
}

/**
 *
 */

bool t_Expression::setTempName(string &name)
{
    this->m_tempName = name;
    return true;
}

bool t_Expression::clearVisitedFlagsUnderThisExpression()
{
    t_HashTable<unsigned long long, bool> VisitedSet;
    m_dagExpression->m_dag->assignVisitedFlagRecursively(false, VisitedSet);
    return true;
}

template<typename T>
bool t_Expression::setVisitedFlagsUnderTheseExpressions(T&vec, bool value)
{
    t_HashTable<unsigned long long, bool> VisitedSet;
    for (typename T::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        bool r = (*it)->m_dagExpression->m_dag->assignVisitedFlagRecursively(value, VisitedSet);
        if (!r)
            return false;
    }
    return true;
}

//specialize
template bool t_Expression::setVisitedFlagsUnderTheseExpressions(vector<t_Expression*>&vec, bool val);
template bool t_Expression::setVisitedFlagsUnderTheseExpressions(set<t_Expression*>&vec, bool val);

string t_Expression::getLabelOfExpression(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: getLabelOfExpression() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        return this->m_dagExpression->getLabelOfExpressionNode();
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

string t_Expression::getLabelOfExpression(int expressionType)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: getLabelOfExpression() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        return this->m_dagExpression->getLabelOfExpressionNode();
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

bool t_Expression::printExpressionToFileAsTree(string &expression_symbol, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, ofstream *outfile)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: printExpressionToFileAsTree() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        *outfile << expression_symbol << " := ";
        m_dagExpression->printExpressionToFileAsTree1(m_dagExpression->m_dag, outfile);
        *outfile << endl;
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

bool t_Expression::printExpressionToFileAsDAG(const string &expression_symbol, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, ostream &outfile)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: printExpressionToFileAsDAG() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        outfile << expression_symbol << "{" << m_dagExpression->m_dag->getNodeID() << "}" << " := ";
        m_dagExpression->printExpressionToFileAsDAG(m_dagExpression->m_dag, outfile);
        outfile << endl;
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

bool t_Expression::printExpressionToFileAsDAG(string &expression_symbol, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, ofstream *outfile)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (this->m_dagExpression == NULL)
        {
            logManager->LOG(" Expression: printExpressionToFileAsDAG() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        *outfile << expression_symbol << "{" << m_dagExpression->m_dag->getNodeID() << "}" << " := ";
        m_dagExpression->printExpressionToFileAsDAG(m_dagExpression->m_dag, outfile);
        *outfile << endl;
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

t_Expression *t_Expression::copyExpressionUsingSuffixForLeafNodes(t_Expression *expression_to_copy, string &suffix, int Expressionimplementationtype, t_ExpressionImplementationManager *expressionImplementationManagerInstance, t_ExpressionEvaluator* &evaluator)
{
    t_Logger *logManager = t_Logger::getInstance();
    map<unsigned long int, unsigned long int> copy_expr_map;
    if (Expressionimplementationtype == DAG_EXPRESSION)
    {
        logManager->LOG("Copying .....with suffix " + suffix, m_logFile, c_DebugLevelVerbosity);
        copy_expr_map.clear();
        t_Expression *copied_root = copyExpressionUsingSuffixForLeafNodesRecursively(expression_to_copy, suffix, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator, copy_expr_map);
        copy_expr_map.clear();
        if (copied_root == NULL)
            logManager->LOG("ERROR from Expression.cc copied root is null", m_logFile, c_NoMessageLevelVerbosity);
        return copied_root;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        //return false; changed by sukanya
	return NULL;
    }
}

t_Expression *t_Expression::copyExpressionUsingSuffixForLeafNodesRecursively
(
 t_Expression *node,
 string &suffix,
 int Expressionimplementationtype,
 t_ExpressionImplementationManager *expressionImplementationManagerInstance,
 t_ExpressionEvaluator* &evaluator,
 map<unsigned long int, unsigned long int> &copy_expr_map
 )
{
    t_Logger *logManager = t_Logger::getInstance();
    if (node == NULL)
    {
        logManager->LOG(" Expression: copyExpressionUsingSuffixForLeafNodesRecursively() - dagExpression is NULL", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
    else if (Expressionimplementationtype != DAG_EXPRESSION)
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
    map <unsigned long int, unsigned long int>::iterator it = copy_expr_map.begin();
    it = copy_expr_map.find((unsigned long int) node);
    if (it != copy_expr_map.end())
    {
        return (t_Expression *) copy_expr_map[(unsigned long int) node];
    }
    t_ExpressionAsDag *dagExpressionOfThis = node->m_dagExpression;
    string label_of_current = dagExpressionOfThis->getLabelOfExpressionNode();
    logManager->LOG("Copying node with label " + label_of_current, m_logFile, c_DebugLevelVerbosity);
    vector<t_Expression *> operands = dagExpressionOfThis->getVectorOfOperands();
    t_Expression *copied_node;
    if (operands.size() == 0)
    {
        logManager->LOG("Creating leaf node with label " + label_of_current, m_logFile, c_DebugLevelVerbosity);
        t_TypeOfValueStoredInExpressionLabel labelType = dagExpressionOfThis->getValueStoredInExpressionLabel();
        string new_label;
        if (labelType == CONSTANT)
        {
            copied_node = node;
        }
        else
        {
            {
                string label = dagExpressionOfThis->getLabelOfExpressionNode();
                string::size_type idx = label.find(EXPRESSION_INVALID_BIT_LABEL);
                if(idx==string::npos)
                {
                    new_label = label+suffix;
                }
                else
                {
                    new_label = label.substr(0,idx)+suffix+EXPRESSION_INVALID_BIT_LABEL;
                }
            }
            //new_label = dagExpressionOfThis->getLabelOfExpressionNode() + suffix;
            TypeOfExpressionTuple typeInfo = dagExpressionOfThis->getTypeOfExpressionTuple();
            copied_node = createExpression(new_label, typeInfo, labelType, Expressionimplementationtype, expressionImplementationManagerInstance);
        }
    }
    else
    {
        logManager->LOG("Creating non leaf node with label " + label_of_current, m_logFile, c_DebugLevelVerbosity);
        string new_label = dagExpressionOfThis->getLabelOfExpressionNode(); //Don't add suffix here!!
        t_TypeOfValueStoredInExpressionLabel labelType = dagExpressionOfThis->getValueStoredInExpressionLabel();
        TypeOfExpressionTuple typeInfoCopy = dagExpressionOfThis->getTypeOfExpressionTuple();
        TypeOfExpressionTuple typeInfo = {typeInfoCopy.m_type, typeInfoCopy.m_width};
        logManager->LOG("Number of operands is " + operands.size(), m_logFile, c_DebugLevelVerbosity);
        vector<t_Expression *>new_operands;
        //generate all the parameters needed for creating the new node
        for (vector<t_Expression*>::iterator it = operands.begin(), end = operands.end();
                it != end; it++)
        {
            t_Expression *child = *it;
            new_operands.push_back(copyExpressionUsingSuffixForLeafNodesRecursively(child, suffix, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator, copy_expr_map));
        }
        copied_node = createExpression(new_label, labelType, typeInfo, new_operands, Expressionimplementationtype, expressionImplementationManagerInstance, evaluator);
    }
    copy_expr_map.insert(make_pair((unsigned long int) (node), (unsigned long int) (copied_node)));
    return copied_node;
}

/**
 * Method - setValidFlag
 * Description - A setter method for setting the validValue flag of the expression
 * Arguments - valid (bool) : validity of the expression
 * Return - true if the valueValid set is successful
 */
bool t_Expression::setValidFlag(bool valid)
{
    this->m_validValue = valid;
    return true;
}

/**
 * Method - isValidValue
 * Description - A getter method to get the valid value  flag of the expression
 * Returns - validValue flag of the expression
 */
bool t_Expression::isValidValue()
{
    return this->m_validValue;
}

/**
 * Method - clearAllValidFlagsofAllAncestorExpressions
 */
//This method is not terminating.... Need to be modified

bool t_Expression::clearAllValidFlagsofAllAncestorExpressions(t_Expression *expr, int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    //cout<<"Inside clear all valid flag"<<endl;
    if (expr == NULL)
    {
        cout << "Null Expression" << endl;
        logManager->LOG(" ERROR ! Null expression received in clearAllValidFlagsOfAllAncestorExpressions, expected - non NULL expression", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    else if (expressionType == DAG_EXPRESSION)
    {
        /**cout << endl;
        cout << "size of the dag is:" << expr->getSizeOfExpression(expressionType, expressionImplementationManagerInstance) << endl;
        cout << "expression is:" << expr->m_dagExpression->m_dag->getLabel(); 
        vector<t_Expression*> operands = expr -> getVectorOfOperands(expressionType, expressionImplementationManagerInstance);
        vector<t_Expression *> ::iterator it;
        for (it = operands.begin(); it != operands.end(); it++)
        {
            t_Expression *temp = *it;
            //cout << " " << temp->m_dagExpression->m_dag->getLabel();
        }
        //cout << endl; */
        expr->m_validValue = false;
        list <t_DAGNode *> ::iterator ptr = expr->m_dagExpression->m_dag->getInListBeginIterator();
        list <t_DAGNode *> ::iterator end = expr->m_dagExpression->m_dag->getInListEndIterator();
        while (ptr != end)
        {
            t_ExpressionValue *val = (t_ExpressionValue *) (*ptr)->getValue();
            if (val == NULL)
            {
                ofstream outfile("NullExpressionErrors", ios::app);
                outfile << "ERROR: (Expression.cc) Recieved a NULL expression value for:" << (*ptr)->getLabel() << ":\t" << (*ptr)->getNodeID() << " \n" <<
                        "which is a parent of " << expr->m_dagExpression->m_dag->getLabel() << endl;
                static string symName = "unknown";
                expr->printExpressionToFileAsDAG(symName, expressionType, expressionImplementationManagerInstance, &outfile);
                outfile << endl;
                outfile.close();

                // Log a message on this
                //cerr << "ERROR: (Expression.cc) Recieved a NULL expression value for:" << (*ptr)->getLabel() << ":\t" << (*ptr)->getNodeID() << " \n" << endl;
                logManager->LOG("ERROR: (Expression.cc) Recieved a NULL expression value for:" + (*ptr)->getLabel() + ":\t" + toString((*ptr)->getNodeID()), m_logFile, c_NoMessageLevelVerbosity);


                //exit(-1);
            }
            else if (val != NULL && val->m_labelType != CONSTANT && val->m_backPointerToExpression->m_validValue == true)
            {
                if (!clearAllValidFlagsofAllAncestorExpressions(val->m_backPointerToExpression, expressionType, expressionImplementationManagerInstance))
                {
                    return false;
                }
            }
            ++ptr;
        }
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

string t_Expression::getActualExpressionValueAsString
(
 int expressionImplementationType,
 t_ExpressionImplementationManager *expressionImplementationManagerInstance
 )
{
    t_ExpressionValue *exprValue = this->getValue(
                                                  expressionImplementationType,
                                                  expressionImplementationManagerInstance);
    if (exprValue->m_labelType == CONSTANT && exprValue->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
    {
        string label = this->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance);
        int length = label.find("0b");
        if (length != -1)
        {
            return label.substr(2, length - 2);
        }
        else
        {
            return label;
        }
    }
    string actual_value;
    if (this->m_validValue == true)
    {
        actual_value = exprValue->getActualValueAsString();
    }
    else
    {
        actual_value = getXOrZValueOfLength(exprValue->getWidth(), toString(X_BIT));
    }
    return actual_value;
}

bool t_Expression::clearAllEvaluatorFlagsForExpressions
(
 vector<t_Expression *> &expressions_tobe_evaluated,
 int expressionType,
 t_ExpressionImplementationManager *expressionImplementationManagerInstance
 )
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        /**
         * STEP 1 : Clear visited flags in the dagmanager.
         */
        if (!dagmanager->ResetVisitedFlagOfDAGNodes())
        {
            logManager->LOG("ERROR!! Failed resetting the visited flags of DAGNodes \n", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }

        int i = 0;
        /**
         * STEP 2 : Clear the valid flags of the expressions to be evaluated.
         */
        for (i = 0; i < expressions_tobe_evaluated.size(); i++)
        {
         //   bool cleared = clearVisitedFlagsUnderAnExpression(expressions_tobe_evaluated[i], expressionType, expressionImplementationManagerInstance);
            bool cleared = expressions_tobe_evaluated[i]->clearVisitedFlagsUnderThisExpression();
            if (!cleared)
            {
                logManager->LOG("ERROR!! Failed clearing visited flags under an expression\n", m_logFile, c_NoMessageLevelVerbosity);
                return false;
            }
        }
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 * Method :  SetValue
 * Precondition :
 * Returns: 
 */
bool t_Expression::setValue(
                            t_ExpressionValue *value,
                            bool resetAncestors,
                            int expressionType,
                            t_ExpressionImplementationManager *expressionImplementationManagerInstance,
                            t_ExpressionEvaluator* evaluatorObject)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        /** Adding as part of shifiting the value of root to some leaf
         *  Need to have Bool and   BitVector[1] should be interchangeable.
         *  06-06-2010
         **/
        //cout<<" Setting value in "<<this->m_dagExpression->getLabelOfExpressionNode()<<":"<<value->getActualValueAsString()<<endl;
        t_ExpressionValue *oldValue_in_expr = this->m_dagExpression->getValue();
        t_ExpressionValue* newValue = oldValue_in_expr;
        BVManager* bvm = BVManager::getInstance();
        if (oldValue_in_expr->m_typeInfo.m_width != -1)
        {
            // Old value is a bitvector and new value is a memory, convert memory to bv
            if (oldValue_in_expr->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR && value->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                newValue->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                newValue->m_typeInfo.m_width = -1;
                newValue->m_actualExpressionValueStored.exprvalueMemory = NULL;
                newValue->copyValueOfMemory(value);
            }

                // Old value is memory and new value is bv, convert bv to memory format
            else if (oldValue_in_expr->m_typeInfo.m_type == TYPE_MEMORY_ARRAY && value->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
            {
                newValue->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                string bv_value = value->getActualValueAsString();
                if (oldValue_in_expr->m_actualExpressionValueStored.exprvalueMemory == NULL)
                {
                    string label_of_expr = this->getLabelOfExpression(expressionType,
                                                                      expressionImplementationManagerInstance);
                    t_ExpressionValue* value_memory = this->getAnInitialValueForMemory(label_of_expr, evaluatorObject);
                    if (value_memory != NULL)
                    {
                        newValue->m_actualExpressionValueStored.exprvalueMemory = value_memory->m_actualExpressionValueStored.exprvalueMemory;
                        newValue->setInitialValueOfMemory(bv_value);
                        newValue->m_typeInfo.m_width = -1;
                    }
                    else
                    {
                        bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_lsb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                        bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_msb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                        newValue->m_typeInfo.m_width = value->m_typeInfo.m_width;
                        newValue->m_typeInfo.m_type = value->m_typeInfo.m_type;
                    }
                }
                else
                {
                    newValue->setInitialValueOfMemory(bv_value);
                }
            }

                // Both values are bv, but widths mismatch: one of the following two conditions will hold
            else if (value->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR && value->m_typeInfo.m_width > oldValue_in_expr->m_typeInfo.m_width
                    && oldValue_in_expr->m_typeInfo.m_width != 0)
            {
                bvm->deleteBV(newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                bvm->deleteBV(newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->select_with_int(value->m_actualExpressionValueStored.exprValue2BitVector.m_msb, oldValue_in_expr->m_typeInfo.m_width - 1, 0);
                newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->select_with_int(value->m_actualExpressionValueStored.exprValue2BitVector.m_lsb,
                                                                                                         oldValue_in_expr->m_typeInfo.m_width - 1, 0);
                newValue->m_typeInfo.m_width = oldValue_in_expr->m_typeInfo.m_width;
            }
            else if (value->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR && value->m_typeInfo.m_width < oldValue_in_expr->m_typeInfo.m_width
                    && oldValue_in_expr->m_typeInfo.m_width != 0)
            {
                bvm->deleteBV(newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                bvm->deleteBV(newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->BVExtend(value->m_actualExpressionValueStored.exprValue2BitVector.m_msb,
                                                                                                  oldValue_in_expr->m_typeInfo.m_width - 1, 0);
                newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->BVExtend(value->m_actualExpressionValueStored.exprValue2BitVector.m_lsb,
                                                                                                  oldValue_in_expr->m_typeInfo.m_width - 1, 1);
                newValue->m_typeInfo.m_width = oldValue_in_expr->m_typeInfo.m_width;
            }
            else if ((value->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR && oldValue_in_expr->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR) ||
                    (value->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR && oldValue_in_expr->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR) ||
                    (value->m_typeInfo.m_type == TYPE_TWOBIT_BITVECTOR && oldValue_in_expr->m_typeInfo.m_type == TYPE_TWOBIT_BITVECTOR))
            {
                bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_lsb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_msb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
            }
            else if (value->m_typeInfo.m_type == TYPE_SIGNED_INTEGER && oldValue_in_expr->m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
            {
                newValue->setActualValueInExpressionValue(value->getSignedIntValueOfExpression());
            }
            else if (value->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER && oldValue_in_expr->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
            {
                newValue->setActualValueInExpressionValue(value->getUnsignedIntValueOfExpression());
            }
            else if (value->m_typeInfo.m_type == TYPE_MEMORY_ARRAY && oldValue_in_expr->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                newValue->copyValueOfMemory(value);
                newValue->m_typeInfo.m_width = -1;
            }
            else
            {

                string value_to_set = value->getActualValueAsString();
                newValue->setActualValueBasedOnType(value_to_set);
            }
        }
        else
        {
            // Old value is memory and new value is bv, convert bv to memory format
            if (oldValue_in_expr->m_typeInfo.m_type == TYPE_MEMORY_ARRAY && value->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
            {
                string bv_value = value->getActualValueAsString();
                if (oldValue_in_expr->m_actualExpressionValueStored.exprvalueMemory == NULL)
                {
                    string label_of_expr = this->getLabelOfExpression(expressionType,
                                                                      expressionImplementationManagerInstance);
                    t_ExpressionValue* value_memory = this->getAnInitialValueForMemory(label_of_expr, evaluatorObject);
                    if (value_memory != NULL)
                    {
                        newValue->m_actualExpressionValueStored.exprvalueMemory = value_memory->m_actualExpressionValueStored.exprvalueMemory;
                        newValue->setInitialValueOfMemory(bv_value);
                    }
                    else
                    {
                        bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_lsb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                        bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_msb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                        newValue->m_typeInfo.m_width = value->m_typeInfo.m_width;
                        newValue->m_typeInfo.m_type = value->m_typeInfo.m_type;
                    }
                }
                else
                {
                    newValue->setInitialValueOfMemory(bv_value);
                }
            }
            else if (oldValue_in_expr->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
            {
                if (value->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
                {
                    bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_lsb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                    bvm->bv_copy(value->m_actualExpressionValueStored.exprValue2BitVector.m_msb, newValue->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                    newValue->m_typeInfo.m_width = value->m_typeInfo.m_width;
                }
                else
                {
                    //cerr << "Expression.cc: Method SetValue() Unexpexted Bitvector with width=-1 \tID:" << this->getID() << endl;
                    string value_to_set = value->getActualValueAsString();
                    newValue->m_typeInfo.m_width = value_to_set.length();
                    newValue->setActualValueBasedOnType(value_to_set);
                }
            }
            else if (value->m_typeInfo.m_type == TYPE_MEMORY_ARRAY && oldValue_in_expr->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                if (getID() == 22)
                {
                    cout << "old value:" << value->getActualValueAsString();
                }

                string lab = getLabelOfExpression(expressionType, expressionImplementationManagerInstance);
                if (lab == "update")
                {
                    vector<t_Expression*> operands = this->getVectorOfOperands(expressionType, expressionImplementationManagerInstance);
                    if (operands[0]->getLabelOfExpression(expressionType, expressionImplementationManagerInstance) == "read")
                    {
                        newValue->m_actualExpressionValueStored.exprvalueMemory = NULL;
                    }
                }
                else if (lab == "read")
                {
                    newValue->m_actualExpressionValueStored.exprvalueMemory = NULL;
                }
                newValue->copyValueOfMemory(value);
                if (getID() == 22)
                {
                    cout << "new value:" << newValue->getActualValueAsString() << endl;
                }
            }
            else
            {
                string value_to_set = value->getActualValueAsString();
                newValue->setActualValueBasedOnType(value_to_set);
            }
        }
        //newValue->m_backPointerToExpression = this;
        // End of conversion of value type
        bool isSet = true; // = m_dagExpression->setValue(value, dagmanager);
        //if (isSet)
        {
            //Value of the expression is set and clear ancestor expression nodes valid flag.
            if (resetAncestors)
            {
                isSet = clearAllValidFlagsofAllAncestorExpressions(this, expressionType, expressionImplementationManagerInstance);
            }
            this->m_validValue = true;
            return isSet;
        }
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

t_ExpressionValue *t_Expression::getValue(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        return this->m_dagExpression->getValue();
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        //return false; changed by sukanya
	return NULL;
    }
}

/**
 *  Will not invalidate the flags of expressions under the current expression
 */
bool t_Expression::clearAllValidFlagsOfAnExpression(t_Expression *expr, int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    if (expr == NULL)
    {
        return false;
    }
    else if (expressionType == DAG_EXPRESSION)
    {
        expr->m_validValue = false;
        list <t_DAGNode *> ::iterator ptr = this->m_dagExpression->m_dag->getOutListBeginIterator();
        list <t_DAGNode *> ::iterator end = this->m_dagExpression->m_dag->getOutListEndIterator();
        while (ptr != end)
        {
            t_ExpressionValue *val = (t_ExpressionValue *) (*ptr)->getValue();
            if (val->m_backPointerToExpression->m_validValue == true)
            {
                if (!clearAllValidFlagsofAllAncestorExpressions(val->m_backPointerToExpression, expressionType, expressionImplementationManagerInstance))
                    return false;
            }
            ptr++;
        }
        return true;
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

vector <t_Expression *> t_Expression::getVectorOfLeafExpressions(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        return this->m_dagExpression->getLeafExpressions(dagmanager);
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        vector <t_Expression *> emptyVector;
        return emptyVector;
    }
}

// TO be modified (getting stuck)

bool t_Expression::setValuesOfLeaves(map<string, string> &leaf_values,
                                     bool clearValidityOfAncestors,
                                     int expressionImplementationType,
                                     t_ExpressionImplementationManager *expressionImplementationManagerInstance,
                                     t_ExpressionEvaluator* &evaluator
                                     )
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        vector <t_Expression *> leaf_expressions;
        int i = 0;
        /***
        vector<t_DAGNode *> leaf_dag_nodes = dagmanager->getVectorOfAllLeafNodesInDAGManager();        

        vector<t_DAGNode*>::iterator leafDagNodesIterator = leaf_dag_nodes.begin();
        while (leafDagNodesIterator != leaf_dag_nodes.end())
        {
            t_ExpressionValue *leaf_val = (t_ExpressionValue *) (*leafDagNodesIterator)->getValue();
            if (leaf_val != NULL && leaf_val->m_backPointerToExpression != NULL)
            {
                leaf_expressions.push_back(leaf_val->m_backPointerToExpression);
            }
            leafDagNodesIterator++;
        } /**/
        for (map<string, string>::iterator it = leaf_values.begin(),
                end = leaf_values.end(); it != end; it++)
        {
            string leaf_label = (*it).first;
            t_Expression* expr = getExpressionWithLabel(leaf_label, expressionImplementationType, expressionImplementationManagerInstance);
            if (expr != NULL)
            {
                leaf_expressions.push_back(expr);
            }
            else
            {
                cout << "***** No Expression found with label " << leaf_label << endl;
            }
        }/**/
        //Set values to the leafs.
        for (i = 0; i < leaf_expressions.size(); i++)
        {
            t_ExpressionValue *leaf_value = leaf_expressions[i]->getValue(expressionImplementationType, expressionImplementationManagerInstance);
            if (leaf_value->m_labelType == SYMBOL)
            {
                string leaf_label = leaf_expressions[i]->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance);
                map<string, string>::iterator leaf_value_toassign_iter = leaf_values.find(leaf_label);
                if (leaf_value_toassign_iter != leaf_values.end())
                {
                    string actual_value = (*leaf_value_toassign_iter).second;
                    //                    cout<<i<<"\t"<<leaf_label<<endl;
                    logManager->LOG(" Label:" + leaf_label + "  Value:" + actual_value, m_logFile, c_DebugLevelVerbosity);
                    if (leaf_value->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
                    {
                        t_ExpressionValue* memValue = leaf_expressions[i]->getAnInitialValueForMemory(leaf_label, evaluator);
                        t_MemoryDataType* mem = memValue->m_actualExpressionValueStored.exprvalueMemory;
                        /**
                        if (mem->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
                        {
                            t_BaseValueOfMemoryType* base = (t_BaseValueOfMemoryType*) mem;
                            int baseWidth = 1;
                            base->bvForThisBaseEntry = convertStringTo2BitBitVector(actual_value, baseWidth);
                            leaf_value = memValue;
                        }
                        else */
                        {
                            /**
                            t_ListOfMemoryEntries* list = (t_ListOfMemoryEntries*) mem;
                            cout<<"assinging to mem"<<endl;
                            list->assignABitStringIntoMemoryFromBeginning(actual_value); **/
                            /**if (leaf_value->m_actualExpressionValueStored.exprvalueMemory == NULL ||
                                    leaf_value->m_actualExpressionValueStored.exprvalueMemory->m_bounds == NULL) {
                                t_ExpressionValue* val = leaf_expressions[i]->getAnInitialValueForMemory(leaf_label, evaluator);
                                leaf_value->m_actualExpressionValueStored.exprvalueMemory = val->m_actualExpressionValueStored.exprvalueMemory;
                            } else if (leaf_value->m_actualExpressionValueStored.exprvalueMemory->m_bounds == NULL) */
                            {
                                t_ExpressionValue* val = leaf_expressions[i]->getAnInitialValueForMemory(leaf_label, evaluator);
                                leaf_value->m_actualExpressionValueStored.exprvalueMemory = val->m_actualExpressionValueStored.exprvalueMemory;
                            }
                            leaf_value->setActualValueBasedOnType(actual_value);
                        }
                    }
                    else
                    {
                        leaf_value->setActualValueBasedOnType(actual_value);
                    }
                    bool setLeaf = leaf_expressions[i]->setValue(leaf_value,
                                                                 clearValidityOfAncestors,
                                                                 expressionImplementationType,
                                                                 expressionImplementationManagerInstance,
                                                                 evaluator);
                    if (!setLeaf)
                    {
                        logManager->LOG(" Label:" + leaf_label + "  Value:" + actual_value + " is FAILED to set", m_logFile, c_DebugLevelVerbosity);
                        //  return false;
                    }
                }
                else
                {
                    //                       leaf_expressions[i]->setValidFlag(false);
                    /** /
                     //Check for memory variables and change the type for them
                     vector<t_MemoryArray*>::iterator memIt = evaluator->m_MemoryArraysForEvaluation.begin();
                     while(memIt != evaluator->m_MemoryArraysForEvaluation.end())
                     {
                     if((*memIt)->m_variable_name == leaf_label)
                     {
                     break;
                     }
                     memIt++;
                     }
                     if(memIt != evaluator->m_MemoryArraysForEvaluation.end())
                     {
                     t_ExpressionValue* newValue = new t_ExpressionValue();
                     newValue->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                     t_MemoryDataType* memValue = new t_ListOfMemoryEntries();
                     memValue->m_nDimensionsWithType = (*memIt);
                     newValue->m_actualExpressionValueStored.exprvalueMemory = memValue;
                     leaf_expressions[i]->setValue(newValue, true, expressionImplementationType, expressionImplementationManagerInstance);
                     }
                     //cout<<endl; /**/
                    //*logFile<<" ERROR : Leaf Expression : "<< leaf_label
                    //<< " does not find a value to assign "<<endl;
                    //return false;
                }
            }
        }
        return true;
    }
    else
    {

        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 * Shift the actual value of this expression to given expression with the given label
 */
bool t_Expression::copyValueFromExpressionToLeafWithLabel(
                                                          t_Expression *toExpr, t_Expression *fromExpr,
                                                          int expressionImplementationType,
                                                          t_ExpressionImplementationManager *expressionImplementationManagerInstance,
                                                          t_ExpressionEvaluator* evaluatorObject
                                                          )
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (toExpr == NULL || fromExpr == NULL)
        {
            logManager->LOG(" One of the expressions is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_ExpressionValue *toExprValue = toExpr->getValue(expressionImplementationType,
                                                          expressionImplementationManagerInstance);
        t_ExpressionValue *fromExprValue = fromExpr->getValue(expressionImplementationType,
                                                              expressionImplementationManagerInstance);
        if (toExprValue == NULL || fromExprValue == NULL)
        {
            logManager->LOG("Values of the Expression are NULL ", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }

        bool isSet = toExpr->setValue(fromExprValue, true,
                                      expressionImplementationType, expressionImplementationManagerInstance,
                                      evaluatorObject);
        if (!isSet)
        {
            logManager->LOG("Expression value is not Copied:" + toExpr->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance), m_logFile, c_NoMessageLevelVerbosity);

            return false;
        }
        else
        {
            string fromExprActualValue = fromExprValue->getActualValueAsString();
            logManager->LOG("Expression value:" + fromExprActualValue + "from:" + fromExpr->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance) + " is  Copied:" + toExpr->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance), m_logFile, c_DebugLevelVerbosity);
            return true;
        }
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

bool t_Expression::evaluateAVectorOfExpressions
(
 vector<t_Expression *> &expressions_tobe_evaluated,
 int expressionType,
 t_ExpressionImplementationManager *expressionImplementationManagerInstance,
 t_ExpressionEvaluator* &evaluator
 )
{
    static int counter = 0;
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        /**
         * STEP 4 : Evaluate each expression in given vector of expressions
         */
        logManager->LOG("Evaluation For Cycle:" + toString(++counter), m_logFile, c_DebugLevelVerbosity);
        for (int i = 0; i < expressions_tobe_evaluated.size(); i++)
        {
            logManager->LOG("----------------------------------\nEvaluating Expression:", m_logFile, c_DebugLevelVerbosity);
            string expr_to_print = "Expression";
            if (logManager->getVerbosityLevel() > c_DebugLevelVerbosity)
            {
                expressions_tobe_evaluated[i]->printExpressionToFileAsTree(expr_to_print, expressionType, expressionImplementationManagerInstance, this->m_logFile);
            }
            t_ExpressionValue *result = expressions_tobe_evaluated[i]->evaluateExpression(expressionType, expressionImplementationManagerInstance, evaluator);
            if (result == NULL)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 * Uses DAGManager's visited flag for clearing the valid value flag only used with evaluating set of expressions
 */
bool t_Expression::clearVisitedFlagsUnderAnExpression(
                                                      t_Expression *expr,
                                                      int expressionType,
                                                      t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        if (expr == NULL)
        {
            logManager->LOG("ERROR : Expression is NULL ", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
        bool dagnodeVisited = expr->m_dagExpression->m_dag->isNodeVisited();
        if (dagnodeVisited)
        {
            return true;
        }
        expr->setValidFlag(false);
        list<t_DAGNode *>::iterator iter = expr->m_dagExpression->m_dag->getOutListBeginIterator();
        list<t_DAGNode *>::iterator end = expr->m_dagExpression->m_dag->getOutListEndIterator();
        while (iter != end)
        {
            t_ExpressionValue *eval = (t_ExpressionValue *) ((*iter)->getValue());
            t_Expression *nextExpr = eval->m_backPointerToExpression;
            if (!clearVisitedFlagsUnderAnExpression(nextExpr, expressionType, expressionImplementationManagerInstance))
            {
                logManager->LOG("ERROR!! Failed clearing visited flags under expression recursively\n", m_logFile, c_NoMessageLevelVerbosity);
                return false;
            }
            iter++;
        }
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

t_ExpressionValue *t_Expression::evaluateExpression(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, t_ExpressionEvaluator* &evaluator)
{
    if (expressionType == DAG_EXPRESSION)
    {
        t_ExpressionValue *result = evaluateExpressionRecursively(expressionType, expressionImplementationManagerInstance, evaluator);
        if (result == NULL)
        {
            return NULL;
        }
        //this->setValue(result, true, expressionType, expressionImplementationManagerInstance);
        return result;
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

t_ExpressionValue *t_Expression::evaluateExpressionRecursively
(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance, t_ExpressionEvaluator* &evaluator)
{
    static int counter = 0;
    counter++;
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_ExpressionValue *val = this->m_dagExpression->getValue();
        if (this->m_validValue == true)
        {
            logManager->LOG("Evaluated  Label:" + this->getLabelOfExpression(expressionType, expressionImplementationManagerInstance) + " Value:" + val->getActualValueAsString(), m_logFile, c_DebugLevelVerbosity);
            return val;
        }
        if (val->m_labelType == CONSTANT)
        {
            // Procedure to convert the label constant into value for that constant
            string labelOfthis = this->getLabelOfExpression(expressionType,
                                                            expressionImplementationManagerInstance);
            logManager->LOG("Converting Constant to Value", m_logFile, c_DebugLevelVerbosity);
            if (val->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                return val;
            }
            else if (val->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || val->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
            {/** /
		 if (labelOfthis.find("0b") == string::npos)
		 {
		 // *m_logFile << " ERROR ! Bitvector Constant but not assigned proper label " << endl;
		 //return NULL;
		 /*Update:07-10-2010@rajkumar* /
		 int w = labelOfthis.length();
		 val->setActualValueBasedOnType(labelOfthis);
		 }
		 else  */
                if (labelOfthis.find(CONSTANT_VALUE_LABEL) != -1)
                {
                    this->setValidFlag(true);
                    return val;
                }
                else if (labelOfthis.find("0b") != -1 && val->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
                {
                    int w = labelOfthis.length() - 2;
                    string convertedResult = labelOfthis.substr(2, w);
                    val->setActualValueBasedOnType(convertedResult);
                }
                else
                {
                    val->setActualValueBasedOnType(labelOfthis);
                }
            }
            else if (val->m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
            {
                val->m_actualExpressionValueStored.exprvalueSignedInt = stringToInteger(labelOfthis);
            }
            else if (val->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
            {
                val-> m_actualExpressionValueStored.exprvalueUnsignedInt = stringToInteger(labelOfthis);
            }
            else if (val->m_typeInfo.m_type == TYPE_REAL)
            {
                val->m_actualExpressionValueStored.exprvalueFloat = stringToFloat(labelOfthis);
            }
            else
            {
                val->m_actualExpressionValueStored.exprvalueBool = stringToBoolean(labelOfthis);
            }
            this->setValidFlag(true);
            return val;
        }
        else if (val->m_labelType == SYMBOL)
        {
            logManager->LOG("Value is not set for this leaf Expression: " + m_dagExpression->getLabelOfExpressionNode(), m_logFile, c_RunLevelVerbosity);
            logManager->LOG(" Evaluation can fail due to u-values", m_logFile, c_RunLevelVerbosity);


            string labelOfthis = this->getLabelOfExpression(expressionType,
                                                            expressionImplementationManagerInstance);
            // Check in the given list of the entries whether the current symbol is a memory
            t_ExpressionValue* memValue = getAnInitialValueForMemory(labelOfthis, evaluator);
            if (memValue != NULL)
            {
                memValue->m_backPointerToExpression = this;
                this->setValue(memValue, false,
                               expressionType,
                               expressionImplementationManagerInstance,
                               evaluator);
                this->setValidFlag(true);
                return memValue;
            }
            else if (val->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || val->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
            {
                int width = val->m_typeInfo.m_width;
                string zeroedBvString = getXOrZValueOfLength(width, toString(X_BIT));
                val->setActualValueBasedOnType(zeroedBvString);
                this->setValidFlag(true);
            }
            else if (val->m_typeInfo.m_type == TYPE_BOOL)
            {
                val->setActualValueInExpressionValue(false);
                this->setValidFlag(true);
            }
            else
            {
                val->setActualValueBasedOnType("0");
                this->setValidFlag(true);
            }
            return val;
        }
        else
        {
            string operatorSymbol = this->m_dagExpression->getLabelOfExpressionNode();
            //cout<<"Evaluating :"<<operatorSymbol<<" size:"<<dagmanager->getSizeOfDAG(this->m_dagExpression->m_dag)<<endl;
            /**ofstream* fout=new ofstream("ExpressionEv",ofstream::app);
            dagmanager->printDAGAsTreeToFile(this->m_dagExpression->m_dag,fout);
            fout->close(); **/
            vector <t_Expression *> operands = this->m_dagExpression->getVectorOfOperands();
            vector <t_ExpressionValue *> operandsValues;
            for (int i = 0; i < operands.size(); i++)
            {
                // cout<<i<<"-->"<<operands[i]->m_dagExpression->getLabelOfExpressionNode()<<endl;
                t_ExpressionValue *valOper =
                        operands[i]->evaluateExpressionRecursively(expressionType, expressionImplementationManagerInstance, evaluator);
                valOper = operands[i]->getValue(expressionType, expressionImplementationManagerInstance);
                if (valOper == NULL)
                {
                    logManager->LOG(" One of the operands is NULL in" + operatorSymbol + " Evaluation", m_logFile, c_NoMessageLevelVerbosity);
                    return NULL;
                }
                operandsValues.push_back(valOper);
            }
            logManager->LOG("----------------------------------------------------\n", m_logFile, c_DebugLevelVerbosity);
            logManager->LOG(toString(counter) + " Operator:" + operatorSymbol + "[" + toString(this->getID()) + "]", m_logFile, c_DebugLevelVerbosity);
            if (logManager->getVerbosityLevel() >= c_DebugLevelVerbosity)
            {
                for (int k = 0; k < operandsValues.size(); k++)
                {
                    logManager->LOG("Operand: " + toString(k) + "(" + operands[k]->getLabelOfExpression(expressionType, expressionImplementationManagerInstance) + "): " + operandsValues[k]->getActualValueAsString(), m_logFile, c_DebugLevelVerbosity);
                }
            }
            TypeOfExpressionTuple typeInfo = val->m_typeInfo;
            t_ExpressionValue *result;

            // actual evaluation is performed here in evaluator
            result = evaluator->performEvaluationOnGivenOperands(operatorSymbol, operandsValues);
            if (result == NULL)
            {
                cout << "Expression Evaluation Failed :" + operatorSymbol + "\t" + toString(m_dagExpression->m_dag->getNodeID()) << endl;
                logManager->LOG("Expression Evaluation Failed :" + operatorSymbol + "\t" + toString(m_dagExpression->m_dag->getNodeID()), m_logFile, c_NoMessageLevelVerbosity);
                return NULL;
            }
            //            if( getID() == 25)
            //            {
            //                cout<<"Result of inner update:"<<result->getActualValueAsString()<<endl;
            //            }
            // result->m_labelType = OPERATOR;
            // Check for the type of the result with the actual value type.
            // Before checking the type and results.
            bool result_set = this->setValue(result, false,
                                             expressionType,
                                             expressionImplementationManagerInstance,
                                             evaluator);

            if (result_set == true)
            {
                if (val != result && result != NULL && result->m_labelType != CONSTANT)
                {
                    delete result;
                    result = NULL;
                }
                this->setValidFlag(true);
                result = getValue(expressionType, expressionImplementationManagerInstance);
            }
            else
            {
                logManager->LOG(" Not able to set value in Expression \nExpression Evaluation Failed :" + operatorSymbol + "\t" + toString(m_dagExpression->m_dag->getNodeID()), m_logFile, c_NoMessageLevelVerbosity);
                return NULL;
            }
            logManager->LOG("Result:" + result->getActualValueAsString() + "----------------------------------------------------\n", m_logFile, c_DebugLevelVerbosity);
            //            if( getID() == 25)
            //            {
            //                cout<<"Result of inner update:"<<result->getActualValueAsString()<<endl;
            //            }
            if (operatorSymbol == "update")
            {
                string label = operands[0]->getLabelOfExpression(expressionType, expressionImplementationManagerInstance);
                if (operands[0]->getTypeOfValueStoredInExpressionLabel(operands[0], expressionType, expressionImplementationManagerInstance) == SYMBOL &&
                        label != "update" && label != "read")
                {
                    cout << "Updating the memory " << label << " to " << result->getActualValueAsString() << endl;
                    operands[0]->setValue(result, false,
                                          expressionType,
                                          expressionImplementationManagerInstance,
                                          evaluator);
                }
            }
            return result;
        }
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

t_ExpressionValue* t_Expression::getAnInitialValueForMemory(string sMemoryName, t_ExpressionEvaluator* &evaluator)
{
    t_ExpressionValue* currentValue = this->m_dagExpression->getValue();
    t_MemoryDataType* thisValue = currentValue->m_actualExpressionValueStored.exprvalueMemory;
    vector<t_MemoryArrayBoundsInfo*>::iterator it = evaluator->m_MemoryArraysForEvaluation.begin();
    t_ExpressionValue* valueForMemory = new t_ExpressionValue();
    while (it != (evaluator->m_MemoryArraysForEvaluation.end()))
    {
        if ((*it)->m_variable_name == sMemoryName || ("__P__" + (*it)->m_variable_name == sMemoryName) ||
                ("N@" + (*it)->m_variable_name == sMemoryName) || ("N@__P__" + (*it)->m_variable_name == sMemoryName))
        {
            break;
        }
        it++;
    }
    if (it != evaluator->m_MemoryArraysForEvaluation.end())
    {
        if (currentValue != NULL && thisValue != NULL)
            return currentValue;
        valueForMemory->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
        t_MemoryDataType* t = new t_ListOfMemoryEntries();
        t->m_bounds = new vector<t_MemoryBounds > ((*it)->m_bounds.begin(), (*it)->m_bounds.end());
        //t_MemoryBounds temp;
        //t->m_bounds->resize((*it)->m_bounds.size(), temp);
        //copy((*it)->m_bounds.begin(), (*it)->m_bounds.end(), t->m_bounds->begin());
        valueForMemory->m_actualExpressionValueStored.exprvalueMemory = t;
        return valueForMemory;
    }
    else
    {
        return NULL;
    }
}

bool t_Expression::printValueInExpressionNode(int expressionType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (expressionType == DAG_EXPRESSION)
    {
        t_ExpressionValue *result = this->getValue(expressionType, expressionImplementationManagerInstance);
        if (!result->isActualValueNULL())
            logManager->LOG("Value :" + result->getActualValueAsString(), m_logFile, c_DebugLevelVerbosity);
        return true;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

/**
 *
 */
bool t_Expression::replaceLeafOfAExpressionByExpression(t_Expression *expression_to_replace, t_Expression *expression_replace_by, int expressionImplementationType, t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        t_Logger *logManager = t_Logger::getInstance();
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_ExpressionAsDag *dagExpressionToReplace = expression_to_replace->m_dagExpression;
        t_ExpressionAsDag *dagExpressionReplaceBy = expression_replace_by->m_dagExpression;
        logManager->LOG("Replacing Leaf: " + dagExpressionToReplace->getLabelOfExpressionNode() + " by " + dagExpressionReplaceBy->getLabelOfExpressionNode(), m_logFile, c_DebugLevelVerbosity);
        bool replace_result = m_dagExpression->replaceLeafOfADAGExpressionByDAGExpression(dagmanager, dagExpressionToReplace, dagExpressionReplaceBy);

        /**
         *  A small bug fix:
         * When to_replace expression is a leaf, and this is pointed by some other expression, that expression must alsot
         * start pointing to the new dag, because dag manager will never know what are the expression star pointing to 
         * this dag node. And there is a one-to-one correspondence between dagnode and expression. That means the expression
         * to_replace is the only one pointing to the dag node which is getting replaced . 
         *  Just change the dag node in this expression to make everything consistent.
         *
        if(replace_result)
        {
            expression_to_replace->m_dagExpression->m_dag=expression_replace_by->m_dagExpression->m_dag;
            expression_to_replace = expression_replace_by;
        } */
        logManager->LOG("Result Of replacement:" + toString(replace_result), m_logFile, c_DebugLevelVerbosity);
        return replace_result;
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        throw std::logic_error("Trying to access Unknown Expression Implementation");
       // return false;
    }

}

bool t_Expression::SimplifyExpression(t_Expression * &expression_to_simplify, int Expressionimplementationtype, t_ExpressionImplementationManager *expressionImplementationManagerInstance, t_DAGSimplifier *ds)
{
    //  cerr<<"Entering SimplifyExpression\n";
    if (expression_to_simplify->m_simplified)
    {
        //cerr<<"Expression is already simplified\n";
        return true;
    }
    if (Expressionimplementationtype == DAG_EXPRESSION)
    {
        //      cerr<<"Implementation type is DAG_EXPRESSION\n";
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_ExpressionAsDag *dagExpressionOfThis = this->m_dagExpression;
        t_DAGNode *dag_expression = dagExpressionOfThis->m_dag;
        t_DAGNode *old_dag_node = dag_expression;
        ds->RunAStrategy(dagExpressionOfThis->m_dag, "PRIO");
        if (old_dag_node != dagExpressionOfThis->m_dag)
        {
            //cerr<<"The root of the expression has changed \n";
            t_ExpressionValue *valueOfDAGNode = (t_ExpressionValue *) dag_expression->getValue();
            t_Expression *new_expression = valueOfDAGNode->m_backPointerToExpression;
            expression_to_simplify->m_simplified = true;
            new_expression->m_simplified = true;
            expression_to_simplify = new_expression;
            //cerr<<"Updated the expression\n";
        }
        else
        {
            //cerr<<"The root of the expression has not changed \n";
        }
        return true;
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

t_Expression * t_Expression::getExpressionWithLabel(string &label, int ExpressionImplementationType, t_ExpressionImplementationManager * expressionImplementationManagerInstance)
{
    t_Logger *logManager = t_Logger::getInstance();
    if (ExpressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        /*** Debugging Code -start *****
             vector<DAGNode *> dag_roots = dagmanager->getVectorOfRootNodes();
         *logFile<<" All root Expressions in DAG form:"<<endl;
             for(int i=0;i<dag_roots.size();i++)
             {
         *logFile<<dag_roots[i]->getLabel()<<endl;
             }
             vector<DAGNode *> dag_leaves = dagmanager->getVectorOfAllLeafNodesInDAGManager();
         *logFile<<" All Leaf Expressions in DAG form:"<<endl;
             for(int i=0;i<dag_leaves.size();i++)
             {
         *logFile<<dag_leaves[i]->getLabel()<<endl;
             }
         *logFile<<"-----------------------------------"<<endl;
         ** End of Debugging Code ***/
        t_DAGNode * dag_node = dagmanager->getNodeWithLabel(label);
        if (dag_node == NULL)
        {
            logManager->LOG("ERROR ! Expression: Get Leaf Node with Label: " + label + "Not Found", m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        t_ExpressionValue *valueOfDAGNode = (t_ExpressionValue *) dag_node->getValue();
        if (valueOfDAGNode == NULL)
        {
            logManager->LOG("ERROR ! Expression: Get Leaf Node with Label: " + label + "Does not have a Value", m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        t_Expression *expression_with_label = valueOfDAGNode->m_backPointerToExpression;
        return expression_with_label;
    }
    else
    {
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return NULL;
    }
}

vector<t_Expression *> t_Expression::getVectorOfOperands
(
 int ExpressionImplementationType,
 t_ExpressionImplementationManager * expressionImplementationManagerInstance
 )
{
    if (ExpressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        t_ExpressionAsDag *dagExpressionOfThis = this->m_dagExpression;
        vector<t_Expression *> operands = dagExpressionOfThis->getVectorOfOperands();
        return operands;
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        vector<t_Expression *> v;
        return v;
    }
}

bool t_Expression::checkWhetherCreatingEdgeFormsCycle
(
 t_Expression *nodeFrom,
 t_Expression *nodeTo,
 int ExpressionImplementationType,
 t_ExpressionImplementationManager *expressionImplementationManagerInstance
 )
{
    if (ExpressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;

        t_ExpressionAsDag *dagExpressionOfFromExpr = nodeFrom->m_dagExpression;
        t_DAGNode *fromNode = dagExpressionOfFromExpr->m_dag;

        t_ExpressionAsDag *dagExpressionOfToExpr = nodeTo->m_dagExpression;
        t_DAGNode *toNode = dagExpressionOfToExpr->m_dag;
        return dagmanager->checkWhetherCreatingEdgeFormsCycle(fromNode, toNode);
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    return false;
}

bool t_Expression::gc
(
 int expressionImplementationType,
 t_ExpressionImplementationManager *expressionImplementationManagerInstance
 )
{
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager *dagmanager = expressionImplementationManagerInstance->dagmanager;
        int collected_node_in_garbage = dagmanager->collect_garbage();
        if (collected_node_in_garbage >= 1)
            return true;
        else
            return false;
    }
    else
    {
        t_Logger *logManager = t_Logger::getInstance();
        logManager->LOG("Trying to access Unknown Expression Implementation", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

bool t_Expression::printExpressionWithSet(
                                          t_Expression *expr_to_print,
                                          ofstream *fout,
                                          int expressionImplementationType,
                                          t_ExpressionImplementationManager *expressionImplementationManagerInstance)
{
    //Check for the implementation type
    set<unsigned int> expression_print_set;
    bool result = false;
    t_ExpressionAsDag *dagExpr_to_print = expr_to_print->m_dagExpression;
    int temp_count = 0;
    result = dagExpr_to_print->setTempNames(true, false, temp_count, expr_to_print->m_dagExpression);
    temp_count = 1;
    result = dagExpr_to_print->setTempNames(false, false, temp_count, expr_to_print->m_dagExpression);
    /* Here it creates the set of nodes to be printed */
    if (result)
    {
        result = printExpressionWithSetRecursively(expr_to_print, expression_print_set, expressionImplementationType,
                                                   expressionImplementationManagerInstance);
    }
    /** Now start printing Each node in the set*/
    if (result)
    {
        set<unsigned int>::iterator it = expression_print_set.begin();
        *fout << "\n";
        while (it != expression_print_set.end())
        {
            /** THis code is giving WARNINGS But useful code NEVER DELETE THIS CODE
          t_Expression* current_expr = (t_Expression*) (*it);
                      t_ExpressionValue* currentExpressionValue = current_expr->getValue(expressionImplementationType, expressionImplementationManagerInstance);
             *fout << "<EXPRESSION>" << endl;
             *fout << "ID=" << current_expr->getTempName() << endl;
             *fout << "LABEL=" << current_expr->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance) << endl
                            << "TYPE=" << currentExpressionValue->getOnlyTypeOfExpression() << endl
                            << "WIDTH=" << currentExpressionValue->getWidth() << endl
                            << "LABEL-TYPE=" << currentExpressionValue->m_labelType << endl
                            << "VALID-VALUE=" << current_expr->m_validValue << endl
                            << "VALUE=" << currentExpressionValue->getActualValueAsString() << endl;
             *fout << "<OPERANDS>" << endl;
                      vector<t_Expression*> operands = current_expr->getVectorOfOperands(expressionImplementationType, expressionImplementationManagerInstance);
                      vector<t_Expression*>::iterator itOperands = operands.begin();
                      while (itOperands != operands.end())
                        {
             *fout << (*itOperands)->m_tempName << endl;
                          itOperands++;
                        }
             *fout << "</OPERANDS>" << endl;
             *fout << "</EXPRESSION>" << endl;**/
            it++;
        }
    }
}

bool t_Expression::printExpressionWithSetRecursively(t_Expression *expr_to_print, set<unsigned int>& expressions_set,
                                                     int expressionImplementationType,
                                                     t_ExpressionImplementationManager *expressionImplementationManagerInstance
                                                     )
{
    if (expr_to_print == NULL)
    {
        return false;
    }
    /** This Part is important DONOT DELETE this /
    t_Logger* logManager = t_Logger::getInstance();
    t_ExpressionValue *value = expr_to_print->m_dagExpression->getValue();
    if (value->m_labelType != OPERATOR)
      {
        expressions_set.insert((unsigned int) expr_to_print);
      }
    else
      {
        expressions_set.insert((unsigned int) expr_to_print);
        vector<t_Expression*> operands = expr_to_print->getVectorOfOperands(expressionImplementationType, expressionImplementationManagerInstance);
        vector<t_Expression*>::iterator it = operands.begin();
        while (it != operands.end())
          {
            bool temp_result = printExpressionWithSetRecursively((*it), expressions_set, expressionImplementationType, expressionImplementationManagerInstance);
            if (!temp_result)
              {
                logManager->LOG("Failed In printing expressions to file " + expr_to_print->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance),
                                this->m_logFile, c_RunLevelVerbosity);
                cerr << "Expression Not able to print ";
		cerr << "(See " << t_ConfigurationOptions::getConfig()->m_sLogDir + "/Expression.log)" << endl;
                return false;
              }
            it++;
          }
      } /***/
    return true;
}

bool t_Expression::replaceExpressionRepresentation(t_Expression* old_expression, t_Expression* new_expression, int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        if (old_expression == NULL || new_expression == NULL)
        {
            return false;
        }
        else
        {
            t_DAGManager* dagManager = expressionImplementationManagerInstance->dagmanager;
            t_DAGNode* old_expression_dag_node = old_expression->m_dagExpression->m_dag;
            t_DAGNode* new_expression_dag_node = new_expression->m_dagExpression->m_dag;
            t_DAGNode* temp;
            while ((temp = dagManager->getReplaceByInfo(old_expression_dag_node)) != NULL)
            {
                old_expression_dag_node = temp;
            }
            bool replace_result = dagManager->ReplaceDAGByDAG(old_expression_dag_node, new_expression_dag_node);
            return replace_result;
        }
    }
    else
    {
        t_Logger* logmanager = t_Logger::getInstance();
        logmanager->LOG("ERROR! Unknown expression implementation type", m_logFile, c_RunLevelVerbosity);
        return false;
    }
}

/**
 * Get the latest restructured version of the expression. Replacement is maintained as kind of link list, get ReplaceByInfo will give you the next
 * node in the list. Last Node will be pointing to NULL so traverse till last to get the latest restructured version.
 * 
 */
t_Expression* t_Expression::getMostRecentRestructuredExpression(int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
    if (expressionImplementationType == DAG_EXPRESSION)
    {
        t_DAGManager* dagManager = expressionImplementationManagerInstance->dagmanager;
        t_DAGNode* expression_dag_node = this->m_dagExpression->m_dag;
        t_DAGNode* temp = expression_dag_node;
        while ((temp = dagManager->getReplaceByInfo(expression_dag_node)) != NULL)
        {
            expression_dag_node = temp;
        }
        t_ExpressionValue* value_in_dag = (t_ExpressionValue*) expression_dag_node->getValue();
        return value_in_dag->m_backPointerToExpression;
    }
    else
    {
        t_Logger* logmanager = t_Logger::getInstance();
        logmanager->LOG("ERROR! Unknown expression implementation type", m_logFile, c_RunLevelVerbosity);
        return NULL;
    }
}

t_Expression* t_Expression::createABitVectorConstantZeroExpression(int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
    static t_Expression* constZeroExpr = NULL;
    if(constZeroExpr == NULL)
    {
        string constant_zero = "0b0";
        TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
        t_TypeOfValueStoredInExpressionLabel constantLabelType = CONSTANT;
        constZeroExpr = createExpression(constant_zero, typeInfo, constantLabelType, expressionImplementationType, expressionImplementationManagerInstance);
    }
    return constZeroExpr;
}

t_Expression* t_Expression::createABitVectorConstantOneExpression(int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
    static t_Expression* constOneExpr = NULL;
    if(constOneExpr == NULL)
    {
        string constant_one = "0b1";
        TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
        t_TypeOfValueStoredInExpressionLabel constantLabelType = CONSTANT;
        constOneExpr = createExpression(constant_one, typeInfo, constantLabelType, expressionImplementationType, expressionImplementationManagerInstance);
    }
    return constOneExpr;
}

/*
t_Expression* t_Expression::getInvalidBitExpression(t_Expression* expr, int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
  if (expressionImplementationType == DAG_EXPRESSION)
    {
      if(expr->getTypeOfValueStoredInExpressionLabel(expr, expressionImplementationType, expressionImplementationManagerInstance)==CONSTANT)
        {
          return createABitVectorConstantZeroExpression(expressionImplementationType, expressionImplementationManagerInstance);
        }
      else
        {
          // return expressionImplementationManagerInstance->invalidbit_manager->getInvalidBitExpression(expr);
        }
    }
  else
    {
        t_Logger* logmanager = t_Logger::getInstance();
        logmanager->LOG("ERROR! Unknown expression implementation type", m_logFile, c_RunLevelVerbosity);
        return NULL;
    }
}
bool t_Expression::setInvalidBitExpression(t_Expression* expr, 
                                           t_Expression* expr_invalidbt, 
                                           int expressionImplementationType, 
                                           t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
  if (expressionImplementationType == DAG_EXPRESSION)
    {
      //      return expressionImplementationManagerInstance->invalidbit_manager->setInvalidBit(expr,expr_invalidbt);
    }
  else
    {
        t_Logger* logmanager = t_Logger::getInstance();
        logmanager->LOG("ERROR! Unknown expression implementation type", m_logFile, c_RunLevelVerbosity);
        return false;
    }
}




bool t_Expression::printExpressionToFileAsDAGWithInvalidBit(t_Expression* expr, 
                                                            ofstream *outfile, 
                                                            int expressionImplementationType, 
                                                            t_ExpressionImplementationManager* expressionImplementationManagerInstance, 
                                                            bool print_invalid_bit)
{
  if(expressionImplementationType==DAG_EXPRESSION)
    {
      if (expr == NULL || expr->m_dagExpression == NULL)
        {	  
          return false;
        }
      t_DAGNode* dagnode = expr->m_dagExpression->m_dag;
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
                {
 *outfile << ",";
                }
              i++;
            }
 *outfile << ")"<<endl;
          dagnode->assignVisitedFlag(true); /*
          if(print_invalid_bit==true)
            {	  
              t_Expression* expr_invalid_bit = getInvalidBitExpression(expr, expressionImplementationType, expressionImplementationManagerInstance);
              if(expr_invalid_bit!=NULL)
                {
 *outfile<<"["<<expr->getID()<<"]@INVALIDBIT {" <<expr_invalid_bit->getID()<<"}";	      
                  if(expr_invalid_bit->m_dagExpression->m_dag->isNodeVisited()==false)
                    {
 *outfile<<" := ";
                      printExpressionToFileAsDAGWithInvalidBit(expr_invalid_bit, outfile, expressionImplementationType, expressionImplementationManagerInstance, false);
                    }
 *outfile<<endl;
                }
                }   *

          it = dagnode->getOutListBeginIterator();
          for (; it != outlist_end_iterator; it++)
            {
              if (((*it)->isNodeVisited()==false) && ((*it)->getSizeOfOutList() > 0))
                {
 *outfile << "["<< (*it)->getNodeID()<< "] := ";
                  t_ExpressionValue* value_it = (t_ExpressionValue*)(*it)->getValue();
                  printExpressionToFileAsDAGWithInvalidBit(value_it->m_backPointerToExpression, outfile, expressionImplementationType, expressionImplementationManagerInstance, true);		
                }
            }
        }    /*
      else if(print_invalid_bit==true)
        {	  
          t_Expression* expr_invalid_bit = getInvalidBitExpression(expr, expressionImplementationType, expressionImplementationManagerInstance);
          if(expr_invalid_bit!=NULL)
            {
 *outfile<<endl;
 *outfile<<"["<<expr->getID()<<"]@INVALIDBIT {" <<expr_invalid_bit->getID()<<"}";	      
              if(expr_invalid_bit->m_dagExpression->m_dag->isNodeVisited()==false)
                {
 *outfile<<" := ";
                  printExpressionToFileAsDAGWithInvalidBit(expr_invalid_bit, outfile, expressionImplementationType, expressionImplementationManagerInstance, false);
                }
 *outfile<<endl;
            }
            }    *
      return true;
    }
  else
    {
      return false;
    }
}
 */

/**
 * Returns a constant value stored in expression value. If the expression is a bit vector, it is converted to integer
 * The default value on error is -1
 * @param expressionImplementationType
 * @param expressionImplementationManagerInstance
 * @return the constant value, if any, in the expression. -1 if no constant value found
 */
int t_Expression::getConstantValuePresentInExpression(int expressionImplementationType, t_ExpressionImplementationManager* expressionImplementationManagerInstance)
{
    if (this == NULL)
    {
        return -1;
    }
    t_ExpressionValue* expr_val = this->getValue(expressionImplementationType, expressionImplementationManagerInstance);
    if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
    {
        return expr_val->getUnsignedIntValueOfExpression();
    }
    else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
    {
        return expr_val->getSignedIntValueOfExpression();
    }
    else if (this->getTypeOfValueStoredInExpressionLabel(this, expressionImplementationType, expressionImplementationManagerInstance) == CONSTANT)
    {
        string label = this->m_dagExpression->getLabelOfExpressionNode();
        return binaryStringToInteger(label.substr(2, label.length() - 2));
    }
    else if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR ||
            expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
            expr_val->getOnlyTypeOfExpression() == TYPE_TWOBIT_BITVECTOR)
    {
        return expr_val->get4BitBitVectorValueAsInteger();
    }
    else
    {
        return -1;
    }
}

