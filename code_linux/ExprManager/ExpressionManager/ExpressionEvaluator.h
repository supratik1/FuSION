#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "ExpressionValue.h"
#include <vector>

class t_ExpressionManager;
using namespace std;

class t_ExpressionEvaluator
{
private:
    vector<t_MemoryArrayBoundsInfo *> m_MemoryArraysForEvaluation;
    bool m_updateXOrZValueFlag;
    t_ExpressionEvaluator()
    {
        // this->m_MemoryArraysForEvaluation.erase(m_MemoryArraysForEvaluation.begin(),m_MemoryArraysForEvaluation.end());
        m_updateXOrZValueFlag = false;
    }

    /** ARITHMETIC OPERATORS */
    template <class T>
    T add(T op1, T op2)
    {
        return op1 + op2;
    }

    template <class T>
    T sub(T op1, T op2)
    {
        return op1 - op2;
    }

    template <class T>
    T multiply(T op1, T op2)
    {
        return op1 * op2;
    }

    template <class T>
    T divide(T op1, T op2)
    {
        if (op2 == 0)
        {
            throw std::invalid_argument("ERROR ! Trying to divide by zero ");
        }
        else
        {
            return op1 / op2;
        }
    }

    int modulus(int op1, int op2)
    {
        return op1 % op2;
    }

    template <class T1, class T2>
    bool greaterthan(T1 a, T2 b)
    {
        return a > b;
    }

    template <class T1, class T2>
    bool greaterthanEqual(T1 a, T2 b)
    {
        return a >= b;
    }

    template <class T1, class T2>
    bool checkEqual(T1 a, T2 b)
    {
        return a == b;
    }

public:
    /**
     *
     * Description : Performs the actual evaluation on the values of the operands and returns the result
     *                      Step  1. Get the number of operands
     *                               2. Check for the operator and Operand Types
     *                               3. Call relevant library to perform the evaluation
     * Returns:      ExpressionValue obtained in the evaluation result, otherwise.
     *               if not able to perform the evaluation Exception
     *              ( May be due to Invalid number of arguments,particular does not support the operator
     *               evaluation, Divide  by Zero etc).
     */

    // t_ExpressionValue *performEvaluationOnGivenOperands(string operatorSymbol,vector<t_ExpressionValue *> operands);
    t_ExpressionValue *performEvaluationOnGivenOperands(string &operatorSymbol, vector<t_ExpressionValue *> &operands);
    t_ExpressionValue *performEvaluationOnGivenOperandsWithOneBit(string &operatorSymbol, vector<t_ExpressionValue *> &operands);
    void printValuesInEvaluation(string &operatorSumbol, vector<t_ExpressionValue *> &operand);

    bvatom getZerosOrOnes(int number, char c);
    bvatom getZerosOrOnes(int number, const string &s)
    {
        assert(s.length() == 1);
        return getZerosOrOnes(number, s[0]);
    }
    t_FourValuedBitVector twoBitNegate(t_FourValuedBitVector bv);
    t_FourValuedBitVector twoBitAnd(t_FourValuedBitVector bv1, t_FourValuedBitVector bv2);
    t_FourValuedBitVector twoBitOr(t_FourValuedBitVector bv1, t_FourValuedBitVector bv2);
    t_FourValuedBitVector countOnes(t_FourValuedBitVector bv);
    bool checkforZbit(bvatom bv1, bvatom bv2);
    bool checkforXbit(bvatom bv1, bvatom bv2);

    bool getXOrZReaderErrorFlag()
    {
        return this->m_updateXOrZValueFlag;
    }
    // bool setXorZReaderErrorFlag(bool value)
    void setXorZReaderErrorFlag(bool value)
    {
        this->m_updateXOrZValueFlag = value; // should be void. no places used.
    }
    // bool clearXorZReaderErrorFlag()
    void clearXorZReaderErrorFlag()
    {
        this->m_updateXOrZValueFlag = false; // should be void. no places used.
    }
    t_ExpressionValue *performUpdateOperationOnMemory(vector<t_ExpressionValue *> &operandsOfUpdate);
    t_ExpressionValue *performReadOperationOnMemory(vector<t_ExpressionValue *> &operandsOfUpdate);
    t_ListOfMemoryEntries *convert2BitBitVectorToOneDimensionalListOfMemoryEntries(t_FourValuedBitVector bvValue, vector<t_MemoryBounds> &memoryDimensions);
    friend class t_Expression;
    friend class t_ExpressionManager;
};
#endif
