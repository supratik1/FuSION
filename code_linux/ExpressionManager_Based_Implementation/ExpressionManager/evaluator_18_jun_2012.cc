#include "ExpressionEvaluator.h"
#include "Expression.h"
t_FourValuedBitVector t_ExpressionEvaluator::countOnes(t_FourValuedBitVector bv)
{
    BVManager *bvm = BVManager::getInstance();

    bvatom temp = bvm->bitwise_xnor(bv.m_msb, bv.m_lsb);
    temp = bvm->bitwise_and(temp, bv.m_msb);

    string tempStr = bvm->printBVasBinString(temp);
    int count = 0;
    for (int i = 0; i < tempStr.length(); i++)
    {
        if (tempStr[i] == '1')
        {
            count++;
        }
    }

    tempStr = integerToBinaryString(count);
    temp = bvm->getBVatom(tempStr.length(), tempStr);
    t_FourValuedBitVector bvRes;

    bvRes.m_msb = temp;
    bvRes.m_lsb = bvm->bitwise_negate(temp);

    return bvRes;

    
}

void t_ExpressionEvaluator::printValuesInEvaluation(string &operatorSumbol, vector<t_ExpressionValue*> &operand)
{
    cout << "Evaluating Expression:" << operatorSumbol << endl;
    vector<t_ExpressionValue*>::iterator it = operand.begin();
    int i = 0;
    while (it != operand.end())
    {
        if ((*it) == NULL)
        {
            cout << "Recieved a NULL operand value" << endl;
        }
        cout << "Operand " << i << " Value:" << (*it)->getActualValueAsString() << " Type:" << (*it)->m_typeInfo.m_type << " width:" << (*it)->m_typeInfo.m_width << endl;
        it++;
        i++;
    }
}

bvatom t_ExpressionEvaluator::getZerosOrOnes(int number, string s)
{
    BVManager *bvm = BVManager::getInstance();

    string temp = "";
    for (int i = 0; i < number; i++)
    {
        temp = temp + s;
    }
    return (bvm->getBVatom(number, temp));
}

t_FourValuedBitVector t_ExpressionEvaluator::twoBitNegate(t_FourValuedBitVector bv)
{
    t_FourValuedBitVector temp;
    temp.m_msb = bv.m_lsb;
    temp.m_lsb = bv.m_msb;
    return temp;
}

t_FourValuedBitVector t_ExpressionEvaluator::twoBitAnd(t_FourValuedBitVector bv1, t_FourValuedBitVector bv2)
{
    t_FourValuedBitVector temp;
    BVManager *bvm = BVManager::getInstance();
    temp.m_msb = bvm->bitwise_and(bv1.m_msb, bv2.m_msb);
    temp.m_lsb = bvm->bitwise_or(bv1.m_lsb, bv2.m_lsb);

    return temp;
}

t_FourValuedBitVector t_ExpressionEvaluator::twoBitOr(t_FourValuedBitVector bv1, t_FourValuedBitVector bv2)
{
    t_FourValuedBitVector temp;
    BVManager *bvm = BVManager::getInstance();

    temp.m_msb = bvm->bitwise_or(bv1.m_msb, bv2.m_msb);
    temp.m_lsb = bvm->bitwise_and(bv1.m_lsb, bv2.m_lsb);

    return temp;
}

bool t_ExpressionEvaluator::checkforZbit(bvatom bv1, bvatom bv2)
{
    BVManager *bvm = BVManager::getInstance();

    bool isZero = bvm->checkBVForZero(bvm->bitwise_negate(bvm->bitwise_or(bv1, bv2)));

    if (isZero == true)
    {
        return false;
    }
    else
    {
        return true;
    }

}

bool t_ExpressionEvaluator::checkforXbit(bvatom bv1, bvatom bv2)
{
    BVManager *bvm = BVManager::getInstance();

    bool isZero = bvm->checkBVForZero(bvm->bitwise_and(bv1, bv2));

    if (isZero == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}

t_ExpressionValue* t_ExpressionEvaluator::performEvaluationOnGivenOperands(string &operatorSymbol, vector<t_ExpressionValue *> &operands)
{
    //printValuesInEvaluation(operatorSymbol, operands);
    bool evaluationCompleted = false;
    if (BITEVALUATION == SINGLEBIT_EVALUATION)
    {
        return performEvaluationOnGivenOperandsWithOneBit(operatorSymbol, operands);
    }
    else
    {
        t_ExpressionValue* result = new t_ExpressionValue();
        BVManager *bvm = BVManager::getInstance();
        bool deleteThirdOperand = false;
        if (operatorSymbol == "update")
        {
            t_ExpressionValue* operand3 = operands[2];
            t_ExpressionValue* operand1 = operands[0];
            t_ExpressionValue* operand2 = operands[1];
            if (operand3->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR ||
                    operand3->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
                    operand3->getOnlyTypeOfExpression() == TYPE_TWOBIT_BITVECTOR)
            {
                int baseWidth = 1;
                if (operand3->m_typeInfo.m_width == baseWidth)
                {
                    t_BaseValueOfMemoryType* newValueAsMemory = new t_BaseValueOfMemoryType();
                    newValueAsMemory->bvForThisBaseEntry.m_lsb = operand3->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
                    newValueAsMemory->bvForThisBaseEntry.m_msb = operand3->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    operand3 = new t_ExpressionValue();
                    operand3->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                    newValueAsMemory->m_bounds = new vector<t_MemoryBounds > ();
                    operand3->m_actualExpressionValueStored.exprvalueMemory = (t_MemoryDataType*) newValueAsMemory;
                    deleteThirdOperand = true;
                    operands[2] = operand3;
                }
                else if (operand1->getOnlyTypeOfExpression() == TYPE_MEMORY_ARRAY)
                {
                    vector<t_MemoryBounds> bounds;
                    vector<t_MemoryBounds>::iterator it = operand1->m_actualExpressionValueStored.exprvalueMemory->m_bounds->begin(),
                            end = operand1->m_actualExpressionValueStored.exprvalueMemory->m_bounds->end();
                    it++;
                    // If the bounds of the lower level are not present, that is next level is base level
                    if (it != end)
                    {
                        bounds=vector<t_MemoryBounds>(it, end);
                        operand3 = new t_ExpressionValue();
                        string memValue = operands[2]->getActualValueAsString();
                        operand3->m_actualExpressionValueStored.exprvalueMemory = operand1->getMemoryEntryFromString(memValue,&bounds);
                        operand3->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                        operand3->m_typeInfo.m_width=-1;
                        deleteThirdOperand = true;
                        operands[2] = operand3;
                    }
                        // Create a base value memory
                    else
                    {
                        t_BaseValueOfMemoryType* newValueAsMemory = new t_BaseValueOfMemoryType();
                        newValueAsMemory->bvForThisBaseEntry.m_lsb = operand3->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
                        newValueAsMemory->bvForThisBaseEntry.m_msb = operand3->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                        operand3 = new t_ExpressionValue();
                        operand3->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                        newValueAsMemory->m_bounds = new vector<t_MemoryBounds > ();
                        operand3->m_actualExpressionValueStored.exprvalueMemory = (t_MemoryDataType*) newValueAsMemory;
                        deleteThirdOperand = true;
                        operands[2] = operand3;
                    }
                }
                else
                {
                    cerr << " ExpressionEvaluation(LineNo:193) called for evaluation of memory update, without memory operands" << endl;
                    delete result;
                    return NULL;
                }
            }
            result = performUpdateOperationOnMemory(operands);
            evaluationCompleted = true;
        }

        else if (operatorSymbol == "read")
        {
            result = performReadOperationOnMemory(operands);
            evaluationCompleted = true;
        }

        // Evaluation Completed above
        if (evaluationCompleted == true)
        {
            if (deleteThirdOperand == true)
            {
                delete operands[2];
            }
            return result;
        }

        /**
         * Its just a concat of all the operands, make operatorSymbol to concat and further the function will take care of.
         */
        else if (operatorSymbol == "leftToRightStream")
        {
            operatorSymbol = "concat";
            if (operands.size() == 1)
            {
                result->m_typeInfo.m_width = operands[0]->m_typeInfo.m_width;
                result->m_typeInfo.m_type = operands[0]->m_typeInfo.m_type;
                bvm->bv_copy(operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_msb,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                bvm->bv_copy(operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                //return result;
                evaluationCompleted = true;
            }
            else
            {
                //operands.erase(operands.begin());
                // Continue the evaluation, dont set the evaluationCompletedFlag.
            }
        }
        else if (operatorSymbol == "rightToLeftStream")
        {
            t_ExpressionValue* op1 = operands[0];
            bvatom msb = op1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
            bvatom lsb = op1->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            /**
             * If only one operand is there, return the same 
             */
            if (operands.size() == 1)
            {
                result->m_typeInfo.m_width = operands[0]->m_typeInfo.m_width;
                result->m_typeInfo.m_type = operands[0]->m_typeInfo.m_type;
                string str = getXOrZValueOfLength(operands[0]->m_typeInfo.m_width, "1");
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->getBVatom(operands[0]->m_typeInfo.m_width, str);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->getBVatom(operands[0]->m_typeInfo.m_width, str);
                evaluationCompleted = true;
            }
            else
            {
                if (checkforXbit(msb, lsb) || checkforZbit(msb, lsb))
                {
                    cerr << " Failed in Evaluation of rightToLeftStream Operator\n Recieved x or z value as first operand" << endl;
                    delete result;
                    return NULL;
                }
                int firstOperandValue = bvm->convertBVToUnsignedInt(msb);
                /**
                 * concatenate the whole result first. 
                 */
                operatorSymbol = "concat";
                operands.erase(operands.begin());
                result = performEvaluationOnGivenOperands(operatorSymbol, operands);
                bvatom resMsbConcat = result->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                bvatom resLsbConcat = result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;

                /**
                 * 
                 * firstoperandvalue has the value of number of bits that should be combined.
                 */
                if (firstOperandValue == 0 || firstOperandValue == 1)
                {
                    //Reverse the result bvatom
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->reverseBV(resMsbConcat);
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->reverseBV(resLsbConcat);
                    evaluationCompleted = true;
                }
                else
                {
                    int i = 0;
                    string empty = "";
                    int width = result->m_typeInfo.m_width;
                    bvatom resMsb = bvm->getZeroedBVatom(width);
                    bvatom resLsb = bvm->getZeroedBVatom(width);
                    while (width > i + firstOperandValue - 1)
                    {
                        bvatom currentSelectMsb = bvm->select_with_int(resMsbConcat, i + firstOperandValue - 1, i);
                        bvatom newCurrentSelectMsb = bvm->reverseBV(currentSelectMsb);
                        bvm->replaceBySelBVatom(newCurrentSelectMsb, resMsb, 0, i, firstOperandValue);

                        bvatom currentSelectLsb = bvm->select_with_int(resLsbConcat, i + firstOperandValue - 1, i);
                        bvatom newCurrentSelectLsb = bvm->reverseBV(currentSelectLsb);
                        bvm->replaceBySelBVatom(newCurrentSelectLsb, resLsb, 0, i, firstOperandValue);

                        bvm->deleteBV(currentSelectMsb);
                        bvm->deleteBV(currentSelectLsb);
                        bvm->deleteBV(newCurrentSelectMsb);
                        bvm->deleteBV(newCurrentSelectLsb);

                        i = i + firstOperandValue;
                    }
                    if (i < width)
                    {
                        bvatom currentSelectMsb = bvm->select_with_int(resMsbConcat, width - 1, i);
                        bvatom newCurrentSelectMsb = bvm->reverseBV(currentSelectMsb);
                        bvm->replaceBySelBVatom(newCurrentSelectMsb, resMsb, 0, i, width - i);

                        bvatom currentSelectLsb = bvm->select_with_int(resLsbConcat, width - 1, i);
                        bvatom newCurrentSelectLsb = bvm->reverseBV(currentSelectLsb);
                        bvm->replaceBySelBVatom(newCurrentSelectLsb, resLsb, 0, i, width - i);
                        
                        bvm->deleteBV(currentSelectMsb);
                        bvm->deleteBV(currentSelectLsb);
                        bvm->deleteBV(newCurrentSelectMsb);
                        bvm->deleteBV(newCurrentSelectLsb);
                    }
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = resMsb;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = resLsb;
                    evaluationCompleted = true;
                }
                bvm->deleteBV(resMsbConcat);
                bvm->deleteBV(resLsbConcat);
            }
        }

        //creating X bit vector
        string s = "1";
        bvatom msbXValue = bvm->getBVatom(1, s);
        bvatom lsbXValue = bvm->getBVatom(1, s);

        //creating Z bit vector
        s = "0";
        bvatom msbZValue = bvm->getBVatom(1, s);
        bvatom lsbZValue = bvm->getBVatom(1, s);

        bool convertResultToMemory = false;
        vector<t_MemoryBounds> boundsToConvertResult;

        if (operands.size() == 1 && evaluationCompleted == false)
        {

            t_ExpressionValue* op1 = operands[0];
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR; //result->m_typeInfo.m_type = TYPE_TWOBIT_BITVECTOR;

            //fetching msb and lsbm_actualExpressionValueStored.exprValue2BitVector
            bvatom msb, lsb;
            bool deleteOperand1=false;
            if (operands[0]->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                string mem_str = operands[0]->getActualValueAsString();
                t_FourValuedBitVector bv4bit = convertStringTo2BitBitVector(mem_str, mem_str.length());
                msb = bv4bit.m_msb;
                lsb = bv4bit.m_lsb;
                convertResultToMemory = true;
                deleteOperand1 = true;
                boundsToConvertResult=vector<t_MemoryBounds>(operands[0]->m_actualExpressionValueStored.exprvalueMemory->beginIteratorOnBounds(),
                     operands[0]->m_actualExpressionValueStored.exprvalueMemory->endIteratorOnBounds());
            }
            else
            {
                msb = operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                lsb = operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            }

            if (operatorSymbol == "bitneg")
            {
                /**
                 * Just exchange msb with lsb.
                 */
                result->m_typeInfo.m_width = op1->getWidth();
                bvm->bv_copy(msb,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                bvm->bv_copy(lsb,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
            else if (operatorSymbol == "countones")
            {
                cerr << "Operation yet not supported " << operatorSymbol<<endl;
                exit(-1);
                delete result;
                return NULL;
            }
            else if(operatorSymbol == "signed")
            {
                cout<<"entered signed block"<<endl;
                int n;
                cin>>n;
            }
            else if (operatorSymbol == "unsigned")
            {
                result->m_typeInfo.m_width = op1->getWidth();
                bvm->bv_copy(lsb,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                bvm->bv_copy(msb,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                evaluationCompleted = true;
            }
                //if a vector contain Z value then return Z value
            else if (checkforZbit(msb, lsb) == true)
            {
                result->m_typeInfo.m_width = 1;
                bvm->bv_copy( msbZValue, result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                bvm->bv_copy( lsbZValue, result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
                //if a vector contains X value then return X value
            else if (checkforXbit(msb, lsb) == true)
            {
                result->m_typeInfo.m_width = 1;
                bvm->bv_copy( msbXValue, result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                bvm->bv_copy( lsbXValue, result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                evaluationCompleted = true;
                convertResultToMemory=false;
            }

            else if (operatorSymbol == "lognot")
            {
                //just flip the bits
                result->m_typeInfo.m_width = op1->getWidth();
                bvm->bv_copy(msb,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                bvm->bv_copy(lsb,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                evaluationCompleted = true;
            }

            else if (operatorSymbol == "redand")
            {
                /**
                 * If a Z value is present, return Z value
                 * If a X value is present ,return X value
                 * else return reduction_and of msbVector in m_msb
                 * 		       reduction_and of lsbVector in m_lsb
                 */
                result->m_typeInfo.m_width = 1;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->reduction_and(msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->reduction_and(lsb);
                evaluationCompleted = true;
                convertResultToMemory=false;
            }//end of redand

            else if (operatorSymbol == "redor")
            {
                /**
                 * If a Z value is present, return Z value
                 * If a X value is present ,return X value
                 * else return reduction_or of msbVector in m_msb
                 * 		       and flip of m_msb in m_lsb
                 */
                result->m_typeInfo.m_width = 1;
                bvatom temp = bvm->reduction_or(msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(temp);
                evaluationCompleted = true;
                convertResultToMemory=false;
            }//end of reduction or
            else if (operatorSymbol == "rednor")
            {
                /**
                 * reduction_nor of msbVector in m_msb
                 * Negation of m_msb in m_lsb
                 */
                result->m_typeInfo.m_width = 1;
                bvatom temp = bvm->reduction_nor(msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                temp = bvm->bitwise_negate(temp);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp;
                evaluationCompleted = true;
                convertResultToMemory=false;
            }//end of reduction nor
            else if (operatorSymbol == "redxor")
            {
                /**
                 * reduction_xor of msbVector in m_msb
                 * and negation of m_msb in m_lsb
                 */
                result->m_typeInfo.m_width = 1;
                bvatom temp = bvm->reduction_xor(msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                temp = bvm->bitwise_negate(temp);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp;
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
            else if (operatorSymbol == "redxnor")
            {
                /**
                 * reduction_xnor of msbVector in m_msb
                 * and negation of m_msb in m_lsb
                 */
                result->m_typeInfo.m_width = 1;
                bvatom temp = bvm->reduction_xnor(msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                temp = bvm->bitwise_negate(temp);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp;
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
            else if (operatorSymbol == "rednand")
            {
                /**
                 * reduction_nand of msbVector in msb
                 * negation of m_msb in m_lsb
                 */
                result->m_typeInfo.m_width = 1;
                bvatom temp = bvm->reduction_nand(msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                temp = bvm->bitwise_negate(temp);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp;
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
            else if (operatorSymbol == "concat")
            {
                result->m_typeInfo.m_width = operands[0]->m_typeInfo.m_width;
                result->m_typeInfo.m_type = operands[0]->m_typeInfo.m_type;
                bvm->bv_copy( operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_msb,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                bvm->bv_copy( operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
            if(deleteOperand1==true)
            {
                bvm->deleteBV(msb);
                bvm->deleteBV(lsb);
            }
        }//if of operand size 1 ends

        else if (operands.size() == 2 && evaluationCompleted == false) // BINARY OPERATORS
        {
            t_ExpressionValue *operand1 = operands[0];
            t_ExpressionValue *operand2 = operands[1];
            vector<t_MemoryBounds> operand1MemoryBounds;
            vector<t_MemoryBounds> operand2MemoryBounds;
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR; //result->m_typeInfo.m_type = TYPE_TWOBIT_BITVECTOR;
            bool deleteOperand1=false,
                    deleteOperand2=false;
            //fetching msb and lsb
            bvatom msb1, lsb1, msb2, lsb2;
            if (operands[0]->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                string mem_str = operands[0]->getActualValueAsString();
                t_FourValuedBitVector bv4bit = convertStringTo2BitBitVector(mem_str, mem_str.length());
                msb1 = bv4bit.m_msb;
                lsb1 = bv4bit.m_lsb;
                operand1MemoryBounds=vector<t_MemoryBounds>(operands[0]->m_actualExpressionValueStored.exprvalueMemory->beginIteratorOnBounds(),
                     operands[0]->m_actualExpressionValueStored.exprvalueMemory->endIteratorOnBounds());
                convertResultToMemory = true;
                boundsToConvertResult = operand1MemoryBounds;
                deleteOperand1=true;
            }
            else
            {
                msb1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                lsb1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            }

            if (operands[1]->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                string mem_str = operands[1]->getActualValueAsString();
                t_FourValuedBitVector bv4bit = convertStringTo2BitBitVector(mem_str, mem_str.length());
                msb2 = bv4bit.m_msb;
                lsb2 = bv4bit.m_lsb;
                operand2MemoryBounds=vector<t_MemoryBounds>(operands[1]->m_actualExpressionValueStored.exprvalueMemory->beginIteratorOnBounds(),
                     operands[1]->m_actualExpressionValueStored.exprvalueMemory->endIteratorOnBounds());
                convertResultToMemory = true;
                boundsToConvertResult = operand2MemoryBounds;
                deleteOperand2=true;
            }
            else
            {
                msb2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                lsb2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            }
	    
	    if(msb1.bv_width > msb2.bv_width)
	      {
		int val = msb1.bv_width - msb2.bv_width;
		bvatom zeroVector = getZerosOrOnes(val, "0");
		bvatom oneVector = getZerosOrOnes(val, "1");
		if(msb2.bv_width==0)
		  {
		    msb2 = zeroVector;
		    lsb2 = oneVector;
		  }
		else
		  {
		    msb2 = bvm->concat(zeroVector, msb2);		
		    lsb2 = bvm->concat(oneVector, lsb2);				
		  }
	      }
	    else if(msb1.bv_width < msb2.bv_width)
	      {
		int val = msb2.bv_width - msb1.bv_width;
		bvatom zeroVector = getZerosOrOnes(val, "0");
		bvatom oneVector = getZerosOrOnes(val, "1");
		msb1 = bvm->concat(zeroVector, msb1);		
		lsb1 = bvm->concat(oneVector, lsb1);		
	      }

            if (operatorSymbol == "bitand")
            {
                result->m_typeInfo.m_width = operand1->getWidth();
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->bitwise_and(msb1, msb2);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_or(lsb1, lsb2);
                evaluationCompleted = true;
            }//bitand if ends

            else if (operatorSymbol == "bitor")
            {
                result->m_typeInfo.m_width = operand1->getWidth();
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->bitwise_or(msb1, msb2);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_and(lsb1, lsb2);
                evaluationCompleted = true;
            }
            else if (operatorSymbol == "bitxor")
            {
                result->m_typeInfo.m_width = operand1->getWidth();
                t_FourValuedBitVector op1 = {msb1,lsb1};
                t_FourValuedBitVector op2 = {msb2,lsb2};
                t_FourValuedBitVector op1Neg = twoBitNegate(op1);
                t_FourValuedBitVector op2Neg = twoBitNegate(op2);
                t_FourValuedBitVector temp1 = twoBitAnd(op1Neg, op2);
                t_FourValuedBitVector temp2 = twoBitAnd(op2Neg, op1);
                t_FourValuedBitVector temp3 = twoBitOr(temp1, temp2);

                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp3.m_msb;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp3.m_lsb;

                bvm->deleteBV(temp1.m_lsb);
                bvm->deleteBV(temp1.m_msb);
                bvm->deleteBV(temp2.m_lsb);
                bvm->deleteBV(temp2.m_msb);
                evaluationCompleted = true;
            }
            else if (operatorSymbol == "bitxnor")
            {
                result->m_typeInfo.m_width = operand1->getWidth();
                t_FourValuedBitVector op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector;
                t_FourValuedBitVector op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector;
                t_FourValuedBitVector op1Neg = twoBitNegate(op1);
                t_FourValuedBitVector op2Neg = twoBitNegate(op2);
                t_FourValuedBitVector temp1 = twoBitAnd(op1, op2);
                t_FourValuedBitVector temp2 = twoBitAnd(op2Neg, op1Neg);
                t_FourValuedBitVector temp3 = twoBitOr(temp1, temp2);

                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp3.m_msb;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp3.m_lsb;
                
                bvm->deleteBV(temp1.m_lsb);
                bvm->deleteBV(temp1.m_msb);
                bvm->deleteBV(temp2.m_lsb);
                bvm->deleteBV(temp2.m_msb);
                evaluationCompleted = true;
            }

            else if (operatorSymbol == "leftshift")
            {
                /*
                 * if operand2 contains x or z values than it will return operand1
                 * otherwise it will leftshift msb1 with msb2 and lsb1 with lsb2
                 */
                if (checkforZbit(msb2, lsb2) == true || checkforXbit(msb2, lsb2) == true)
                {
                    bvm->bv_copy(msb1,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                    bvm->bv_copy(lsb1,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                    evaluationCompleted = true;
                }
                else
                {
                    bvatom temp = bvm->left_shift(msb1, msb2);
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(temp);
                    result->m_typeInfo.m_width = operand1->m_typeInfo.m_width;
                    //cout << bvm->printBVasBinString(result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                    evaluationCompleted = true;
                }
                boundsToConvertResult = operand1MemoryBounds;
            }

            else if (operatorSymbol == "rightshift")
            {
                
                /*
                 * if operand2 contains x or z values than it will return operand1
                 * otherwise it will rightshift msb1 with msb2 and lsb1 with lsb2
                 */
                if (checkforZbit(msb2, lsb2) == true || checkforXbit(msb2, lsb2) == true)
                {
                    bvm->bv_copy(msb1,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                    bvm->bv_copy(lsb1,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                    evaluationCompleted = true;
                }
                else
                    
                {   
                    
                    bvatom temp = bvm->right_shift(msb1, msb2);
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(temp);
                    evaluationCompleted = true;
                    
                    
                }
                boundsToConvertResult = operand1MemoryBounds;
            }
            else if (operatorSymbol == "logand")
            {
                result->m_typeInfo.m_width = 1;
                t_FourValuedBitVector op1 = {msb1,lsb1};
                t_FourValuedBitVector op2 = {msb2,lsb2};
                t_FourValuedBitVector temp = twoBitAnd(op1, op2);

                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp.m_msb;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp.m_lsb;
                evaluationCompleted = true;
                convertResultToMemory = false;
            }
            else if (operatorSymbol == "logor")
            {
                result->m_typeInfo.m_width = 1;
                t_FourValuedBitVector op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector;
                t_FourValuedBitVector op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector;
                t_FourValuedBitVector temp = twoBitOr(op1, op2);

                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp.m_msb;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp.m_lsb;
                evaluationCompleted = true;
                convertResultToMemory = false;
            }
            else if (operatorSymbol == "concat")
            {
                /*
                 * concat msb1 and msb2
                 * concat lsb1 and lsb1
                 */
                bvatom res1 = bvm->concat(msb1, msb2);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res1;
                result->m_typeInfo.m_width = res1.bv_width;
                bvatom res2 = bvm->concat(lsb1, lsb2);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = res2;
                convertResultToMemory = false;
                evaluationCompleted = true;
            }
            else if (operatorSymbol == "zeroextn")
            {
                int val, original_value_if_non_bv;
		if(operand2->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
		  {
		    original_value_if_non_bv = operand2->getUnsignedIntValueOfExpression();		    
		  }
		else if(operand2->m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
		  {
		    original_value_if_non_bv = operand2->getSignedIntValueOfExpression();
		  }
		else
		  {
		    /**
		     * if operand2 contains x or z values than error and exit
		     */
		    if (checkforZbit(msb2, lsb2) == true || checkforXbit(msb2, lsb2) == true)
		      {
			cerr << "zeroextn operator error,operand2 contains x or z value";
			delete result;
			exit(-1);
		      }
		    // @TODO Important Fix this
		    if(msb2.bv_width > (sizeof(unsigned int)*8)) msb2.bv_width -= 1;

		    string val_str=bvm->printBVasBinString(msb2);
		    val = binaryStringToInteger(val_str);
		    original_value_if_non_bv = val;
		  }
                val = original_value_if_non_bv - msb1.bv_width;
                if (val <= 0)
                {
		  val = original_value_if_non_bv;
		  result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->select_with_int(msb1, val-1,0);
		  result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->select_with_int(lsb1, val-1,0);
		  result->m_typeInfo.m_width = original_value_if_non_bv;
		  /*
                    bvm->bv_copy(msb1,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                    bvm->bv_copy(lsb1,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                    result->m_typeInfo.m_width = msb1.bv_width; */
                }
                else
                {
                    bvatom zeroVector = getZerosOrOnes(val, "0");
                    bvatom oneVector = getZerosOrOnes(val, "1");
                    bvatom res1 = bvm->concat(zeroVector, msb1);
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res1;
                    bvatom res2 = bvm->concat(oneVector, lsb1);
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = res2;
                    result->m_typeInfo.m_width = res1.bv_width;
                    bvm->deleteBV(zeroVector);
                    bvm->deleteBV(oneVector);
                }
                evaluationCompleted = true;
                convertResultToMemory = false;
            }
            else if (operatorSymbol == "casezeq")
            {
                bvatom mask1 = bvm->bitwise_or(msb1, lsb1);
                bvatom mask2 = bvm->bitwise_or(msb2, lsb2);
                bvatom mask = bvm->bitwise_and(mask1, mask2);
                bvatom bv1 = bvm->bitwise_and(mask, msb1);
                bvatom bv2 = bvm->bitwise_and(mask, msb2);
                bool res = bvm->checkBVEquality(bv1, bv2);
                bvatom zeroVector = getZerosOrOnes(1, "0");
                bvatom oneVector = getZerosOrOnes(1, "1");

                if (res == false)
                {
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = oneVector;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = zeroVector;
                    result->m_typeInfo.m_width = 1;
                }
                else
                {
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = oneVector;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = zeroVector;
                    result->m_typeInfo.m_width = 1;
                }
                evaluationCompleted = true;
                convertResultToMemory = false;
                bvm->deleteBV(mask1);
                bvm->deleteBV(mask2);
                bvm->deleteBV(mask);
                bvm->deleteBV(bv1);
                bvm->deleteBV(bv2);
            }//casez equality ends
                /*
                 * Wild Equality and Wild Inequality
                 */
            else if (operatorSymbol == "wildeq" || operatorSymbol == "casexeq")
            {
                bvatom mask1 = bvm->bitwise_xnor(msb1, lsb1);
                bvatom mask2 = bvm->bitwise_xnor(msb2, lsb2);
                bvatom mask = bvm->bitwise_or(mask1, mask2);
                bvatom bv1 = bvm->bitwise_or(mask, msb1);
                bvatom bv2 = bvm->bitwise_or(mask, msb2);
                bool res = bvm->checkBVEquality(bv1, bv2);
                bvatom zeroVector = getZerosOrOnes(1, "0");
                bvatom oneVector = getZerosOrOnes(1, "1");
                if (res == false)
                {
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = oneVector;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = zeroVector;
                    result->m_typeInfo.m_width = 1;
                }
                else
                {
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = oneVector;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = zeroVector;
                    result->m_typeInfo.m_width = 1;
                }
                bvm->deleteBV(mask1);
                bvm->deleteBV(mask2);
                bvm->deleteBV(mask);
                bvm->deleteBV(bv1);
                bvm->deleteBV(bv2);
                evaluationCompleted = true;
                convertResultToMemory = false;
            }

            else if (operatorSymbol == "wilddiseq")
            {
                bvatom mask1 = bvm->bitwise_xnor(msb1, lsb1);
                bvatom mask2 = bvm->bitwise_xnor(msb2, lsb2);
                bvatom mask = bvm->bitwise_or(mask1, mask2);
                bvatom bv1 = bvm->bitwise_or(mask, msb1);
                bvatom bv2 = bvm->bitwise_or(mask, msb2);
                bool res = bvm->checkBVEquality(bv1, bv2);
                bvatom zeroVector = getZerosOrOnes(1, "0");
                bvatom oneVector = getZerosOrOnes(1, "1");

                if (res == true)
                {
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = oneVector;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = zeroVector;
                    result->m_typeInfo.m_width = 1;
                }
                else
                {
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = oneVector;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = zeroVector;
                    result->m_typeInfo.m_width = 1;
                }
                evaluationCompleted = true;
                convertResultToMemory = false;
                bvm->deleteBV(mask1);
                bvm->deleteBV(mask2);
                bvm->deleteBV(mask);
                bvm->deleteBV(bv1);
                bvm->deleteBV(bv2);
            }
                //If any of the operand contains Z values,then simply return Z value.
            else if (operatorSymbol == "logeq" || operatorSymbol == "diseq" || operatorSymbol == "gt" ||
                    operatorSymbol == "gt" || operatorSymbol == "lt" || operatorSymbol == "le" || operatorSymbol == "ge")
            {
                convertResultToMemory = false;
                //if an of the operand contains the X bit, then simply return the X bit as complete result.
                if (checkforXbit(msb1, lsb1) == true
                        || checkforXbit(msb2, lsb2) == true
                        || checkforZbit(msb1, lsb1) == true
                        || checkforZbit(msb2, lsb2) == true)
                {
                    result->m_typeInfo.m_width = 1;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = getZerosOrOnes(1, "1");
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = getZerosOrOnes(1, "1");
                    evaluationCompleted = true;
                }

                else if (operatorSymbol == "logeq")
                {
                    /**
                     * if both msb1 and msb2 are equal and lsb1 and lsb2 are equal
                     *
                     */
                    bool equal;
                    if (msb1.bv_width != msb2.bv_width)
                    {
		      bvatom temp1,temp2;
		      if(msb1.bv_width>msb2.bv_width)
			{
			  temp1 = msb1;
			  temp2 = bvm->BVExtend(msb2, msb1.bv_width, 0);
			}
		      else
			{
			  temp1 = bvm->BVExtend(msb1, msb2.bv_width, 0);
			  temp2 = msb2;
			}		      
		      equal = bvm->checkBVEquality(temp1, temp2);
		      /**
		       * Following line of code is commented to support ste encoding on 13 Jun 2012
		       */
		      //equla = bvm->checkBVEquality(bvm->reduction_and(msb1), bvm->reduction_and(msb2));
                    }
                    else
                    {
                        equal = (bvm->checkBVEquality(msb1, msb2)) && (bvm->checkBVEquality(lsb1, lsb2));
                    }
                    result->m_typeInfo.m_width = 1;
                    if (equal == true)
                    {
                        string s = "1";
                        bvatom msbValue = bvm->getBVatom(1, s);
                        s = "0";
                        bvatom lsbValue = bvm->getBVatom(1, s);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
                        evaluationCompleted = true;
                    }
                    else
                    {
                        string s = "0";
                        bvatom msbValue = bvm->getBVatom(1, s);
                        s = "1";
                        bvatom lsbValue = bvm->getBVatom(1, s);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
                        evaluationCompleted = true;
                    }
                }
                else if (operatorSymbol == "diseq")
                {
                    /*
                     * if vectors are not equal than 01 else 10
                     */
                    bool equal;
                    if (msb1.bv_width != msb2.bv_width)
                    {
                        vector<t_ExpressionValue*> operandsOfRedOr1;
                        operandsOfRedOr1.push_back(operand1);
                        equal = bvm->checkBVEquality(bvm->reduction_or(msb1), bvm->reduction_or(msb2));
                    }
                    else
                    {
                        equal = (bvm->checkBVEquality(msb1, msb2)) && (bvm->checkBVEquality(lsb1, lsb2));
                    }
                    result->m_typeInfo.m_width = 1;

                    if (equal == false)
                    {
                        string s = "1";
                        bvatom msbValue = bvm->getBVatom(1, s);
                        s = "0";
                        bvatom lsbValue = bvm->getBVatom(1, s);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
                        evaluationCompleted = true;
                    }
                    else
                    {
                        string s = "0";
                        bvatom msbValue = bvm->getBVatom(1, s);
                        s = "1";
                        bvatom lsbValue = bvm->getBVatom(1, s);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
                        evaluationCompleted = true;
                    }
                }
		else
		  {
		    if(msb1.bv_width < msb2.bv_width)
		      {
			msb1 = bvm->BVExtend(msb1, msb2.bv_width,0);
		      }
		    else if(msb1.bv_width > msb2.bv_width )
		      {
			msb2 = bvm->BVExtend(msb2, msb1.bv_width,0);
		      }
		    if (operatorSymbol == "gt")
		      {
			/** if msb1 is greater than msb2 than 10 else 01                    
			 */
			result->m_typeInfo.m_width = 1;
			bool greater = bvm->checkBVGreaterThan(msb1, msb2);

			if (greater == true)
			  {
			    string s = "1";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "0";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
			else
			  {
			    string s = "0";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "1";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
		      }
		    else if (operatorSymbol == "ge")
		      {
			/** if msb1 is greater than or equal to msb2 than 10 else 01
			 *
			 */
			result->m_typeInfo.m_width = 1;
			bool greater = bvm->checkBVGreaterThan(msb1, msb2) || bvm->checkBVEquality(msb1, msb2);
			if (greater == true)
			  {
			    string s = "1";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "0";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
			else
			  {
			    string s = "0";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "1";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
		      }

		    else if (operatorSymbol == "lt")
		      {
			/** if msb1 is not greater than or equal to msb2 than 10 else 01
			 */
			result->m_typeInfo.m_width = 1;
			bool greater = bvm->checkBVGreaterThan(msb1, msb2) || bvm->checkBVEquality(msb1, msb2);
			if (greater == false)
			  {
			    string s = "1";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "0";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
			else
			  {
			    string s = "0";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "1";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
		      }
		    else if (operatorSymbol == "le")
		      {
			/** if msb1 not is greater than msb2 than 10 else 01
			 */
			result->m_typeInfo.m_width = 1;
			bool greater = bvm->checkBVGreaterThan(msb1, msb2);
			if (greater == false)
			  {
			    string s = "1";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "0";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  }
			else
			  {
			    string s = "0";
			    bvatom msbValue = bvm->getBVatom(1, s);
			    s = "1";
			    bvatom lsbValue = bvm->getBVatom(1, s);
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msbValue;
			    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbValue;
			    evaluationCompleted = true;
			  } 
		      }
		  }
            }
            else
            {
                if (checkforZbit(msb1, lsb1) == true || checkforZbit(msb2, lsb2) == true)
                {
                    result->m_typeInfo.m_width = operand1->getWidth();
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = getZerosOrOnes(operand1->getWidth(), "0");
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = getZerosOrOnes(operand1->getWidth(), "0");
                    evaluationCompleted = true;
                    boundsToConvertResult = operand1MemoryBounds;
                }

                    //if an of the operand contains the X bit, then simply return the X bit as complete result.
                else if (checkforXbit(msb1, lsb1) == true || checkforXbit(msb2, lsb2) == true)
                {
                    result->m_typeInfo.m_width = operand1->getWidth();
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = getZerosOrOnes(operand1->getWidth(), "1");
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = getZerosOrOnes(operand1->getWidth(), "1");
                    evaluationCompleted = true;
                    boundsToConvertResult = operand1MemoryBounds;
                }

                else if (operatorSymbol == "add")
                {
                    /**
                     * Do the arithmetic add on op1 and op2 ,put result in msb and the negation
                     * of the result in lsb
                     */
                    bvatom op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    if (op1.bv_width < op2.bv_width)
                    {
                        op1 = bvm->BVExtend(op1, op2.bv_width - op1.bv_width, 0);
                    }
                    else if (op1.bv_width > op2.bv_width)
                    {
                        op2 = bvm->BVExtend(op2, op1.bv_width - op2.bv_width, 0);
                    }
                    bvatom res = bvm->arith_add(op1, op2);
                    result->m_typeInfo.m_width = res.bv_width;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(res);
                    evaluationCompleted = true;
                }

                else if (operatorSymbol == "sub")
                {
                    /**
                     * Do the arithmetic sub on op1 and op2 ,put result in msb and the negation
                     * of the result in lsb
                     */
                    result->m_typeInfo.m_width = operand1->getWidth();
                    bvatom op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    if (op1.bv_width < op2.bv_width)
                    {
                        op1 = bvm->BVExtend(op1, op2.bv_width - op1.bv_width, 0);
                    }
                    else if (op1.bv_width > op2.bv_width)
                    {
                        op2 = bvm->BVExtend(op2, op1.bv_width - op2.bv_width, 0);
                    }
                    bvatom res = bvm->arith_sub(op1, op2);
                    result->m_typeInfo.m_width = res.bv_width;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(res);
                    evaluationCompleted = true;
                }
                else if (operatorSymbol == "multiply")
                {
                    /**
                     * Do the arithmetic multiplication on op1 and op2 ,put result in msb and the negation
                     * of the result in lsb
                     */
                    bvatom op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom res = bvm->arith_mult(op1, op2);
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(res);
                    evaluationCompleted = true;
                    convertResultToMemory = false;
                }
                else if (operatorSymbol == "divide")
                {
                    /**
                     * Do the arithmetic divide on op1 and op2 ,put result in msb and the negation
                     * of the result in lsb
                     */
                    result->m_typeInfo.m_width = operand1->getWidth();
                    bvatom op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom res = bvm->arith_div(op1, op2);
                    if (res.bv_width == 0)
                    {
                        result->m_typeInfo.m_width = op1.bv_width;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = getZerosOrOnes(op1.bv_width, "0");
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = getZerosOrOnes(op1.bv_width, "0");
                    }
                    else
                    {
                        result->m_typeInfo.m_width = res.bv_width;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(res);
                    }
                    evaluationCompleted = true;
                    convertResultToMemory = false;
                }
                else if (operatorSymbol == "mod")
                {
                    /**
                     * Do the arithmetic mod on op1 and op2 ,put result in msb and the negation
                     * of the result in lsb
                     */
                    result->m_typeInfo.m_width = operand1->getWidth();
                    bvatom op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom op2 = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    bvatom res = bvm->arith_mod(op1, op2);
                    if (res.bv_width == 0)
                    {
                        result->m_typeInfo.m_width = op1.bv_width;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = getZerosOrOnes(op1.bv_width, "0");
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = getZerosOrOnes(op1.bv_width, "0");
                    }
                    else
                    {
                        result->m_typeInfo.m_width = res.bv_width;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = res;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_negate(res);
                    }
                    evaluationCompleted = true;
                }
            }
            if(deleteOperand1==true)
            {
                bvm->deleteBV(lsb1);
                bvm->deleteBV(msb1);
            }
            if(deleteOperand2==true)
            {
                bvm->deleteBV(msb2);
                bvm->deleteBV(lsb2);
            }
        }//operand size 2  else if ends

        else if (operands.size() == 3 && evaluationCompleted == false)
        {
            t_ExpressionValue *operand1 = operands[0];
            t_ExpressionValue *operand2 = operands[1];
            t_ExpressionValue *operand3 = operands[2];
            vector<t_MemoryBounds> operand1MemoryBounds;
            vector<t_MemoryBounds> operand2MemoryBounds;
            vector<t_MemoryBounds> operand3MemoryBounds;
            bool deleteOperand1=false,
                    deleteOperand2=false,
                    deleteOperand3=false;

            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR; //result->m_typeInfo.m_type = TYPE_TWOBIT_BITVECTOR;
            bvatom msb1, lsb1,msb2,lsb2,msb3,lsb3;
           

            if (operands[0]->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
            {
                string mem_str = operands[0]->getActualValueAsString();
                t_FourValuedBitVector bv4bit = convertStringTo2BitBitVector(mem_str, mem_str.length());
                msb1 = bv4bit.m_msb;
                lsb1 = bv4bit.m_lsb;
                convertResultToMemory = true;
                deleteOperand1=true;
                operand1MemoryBounds=vector<t_MemoryBounds>(operand1->m_actualExpressionValueStored.exprvalueMemory->beginIteratorOnBounds(),
                     operand1->m_actualExpressionValueStored.exprvalueMemory->endIteratorOnBounds());
            }
            else
            {
                msb1 = operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                lsb1 = operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            }
            if (operand2->getOnlyTypeOfExpression() == TYPE_MEMORY_ARRAY)
            {
                string mem2Value = operand2->getActualValueAsString();
                t_FourValuedBitVector op2bvs = convertStringTo2BitBitVector(mem2Value, mem2Value.length());
                msb2 = op2bvs.m_msb;
                lsb2 = op2bvs.m_lsb;
                convertResultToMemory = true;
                deleteOperand2=true;
                operand2MemoryBounds=vector<t_MemoryBounds>(operand2->m_actualExpressionValueStored.exprvalueMemory->beginIteratorOnBounds(),
                     operand2->m_actualExpressionValueStored.exprvalueMemory->endIteratorOnBounds());
            }
            else
            {
                msb2 = operands[1]->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                lsb2 = operands[1]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            }
            if (operand3->getOnlyTypeOfExpression() == TYPE_MEMORY_ARRAY)
            {
                string mem3Value = operand3->getActualValueAsString();
                t_FourValuedBitVector op3bvs = convertStringTo2BitBitVector(mem3Value, mem3Value.length());
                msb3 = op3bvs.m_msb;
                lsb3 = op3bvs.m_lsb;
                operand3MemoryBounds=vector<t_MemoryBounds>(operand3->m_actualExpressionValueStored.exprvalueMemory->beginIteratorOnBounds(),
                     operand3->m_actualExpressionValueStored.exprvalueMemory->endIteratorOnBounds());
                convertResultToMemory = true;
                deleteOperand3=true;
            }
            else
            {
                msb3 = operands[2]->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                lsb3 = operands[2]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            }

            if (operatorSymbol == "select" && !operand1->isActualValueNULL() &&
                    !operand2->isActualValueNULL() && !operand3->isActualValueNULL())
            {
                int op2Val, op3Val;
                int flag = 0;
                if (operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                        operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
                {
                    op2Val = operand2->getSignedIntValueOfExpression();
                    op3Val = operand3->getSignedIntValueOfExpression();
                    flag = 1;
                }
                else if (operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                        operand3->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
                {
                    op2Val = operand2->getUnsignedIntValueOfExpression();
                    op3Val = operand3->getUnsignedIntValueOfExpression();
                    flag = 1;
                }

                if (flag == 1)
                {
                    if (msb1.bv_width <= op2Val)
                    {
                        bvatom temp_msb = bvm->BVExtend(msb1, op2Val + 1, 0);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->select_with_int(temp_msb, op2Val, op3Val);
                        bvatom temp_lsb = getZerosOrOnes(op2Val + 1, "1");
                        bvm->replaceBySelBVatom(lsb1, temp_lsb, 0, 0, lsb1.bv_width);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->select_with_int(temp_lsb, op2Val, op3Val);
                        //cout << "\n" << bvm->printBVasBinString(lsb1) << " Extending: " << lsb1.bv_width << " To " << op3Val << " res:" << bvm->printBVasBinString(temp_lsb) << endl;
                        result->m_typeInfo.m_width = op3Val - op2Val + 1;
                    }
                    else
                    {
                        bvatom temp = bvm->select_with_int(msb1, op2Val, op3Val);
                        bvatom lsbVal = bvm->select_with_int(lsb1, op2Val, op3Val);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp;
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsbVal;
                        result->m_typeInfo.m_width = op2Val - op3Val + 1;
                    }
                    evaluationCompleted = true;
                }// op2 and op3 are integers i.e. flag is 1 if ends
                else
                {
                    if (checkforZbit(msb2, lsb2) == true || checkforZbit(msb3, lsb3) == true || checkforXbit(msb2, lsb2) == true
                            || checkforXbit(msb3, lsb3) == true)
                    {
                        cerr << "operand 2 or operand3 contains x or z values";
                        exit(-1);
                    }
                    bvatom temp1 = bvm->select_with_bv(msb1, msb2, msb3);
                    bvatom temp2 = bvm->select_with_bv(lsb1, msb2, msb3);
                    result->m_typeInfo.m_width = temp1.bv_width;

                    result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp1;
                    result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp2;
                    evaluationCompleted = true;
                }
            }//select operator if ends

            else if (operatorSymbol == "ite")
            {
                /**
                 * if the first operand contains z or x values, then return combined value of op2 aND OP3
                 * else check if the operand1's msb is 1 if it is, that means its true else false
                 * On true return operand2 else operand3
                 *
                 */
                if (operand1->getOnlyTypeOfExpression() == TYPE_BOOL)
                {
                    bool condition = operand1->getBoolValueOfExpression();
                    bvatom resultBv = (condition) ? msb2 : msb3;
                    bvatom resultLsb = (condition) ? lsb2 : lsb3;
                    bvm->bv_copy(resultBv,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                    bvm->bv_copy(resultLsb,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                    result->m_typeInfo.m_width = operand2->m_typeInfo.m_width;
                    evaluationCompleted = true;
                    if(convertResultToMemory==true)
                    {
                        if(condition==true)
                        {
                            boundsToConvertResult=operand2MemoryBounds;
                        }
                        else
                        {
                            boundsToConvertResult=operand3MemoryBounds;
                        }
                    }
                }
                else
                {
                    string s = "1";
                    bvatom one = bvm->getBVatom(1, s);
                    bool trueCondition;
                    if (checkforZbit(msb1, lsb1) || checkforXbit(msb1, lsb1))
                    {
                        bvatom bv1 = bvm->bitwise_or(msb2, msb3);
                        bvatom bv2 = bvm->bitwise_or(lsb2, lsb3);
                        bvatom mask1 = bvm->bitwise_or(msb2, lsb2);
                        bvatom mask2 = bvm->bitwise_or(msb3, lsb3);
                        bvatom mask = bvm->bitwise_and(mask1, mask2);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->bitwise_and(mask, bv1);
                        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->bitwise_and(mask, bv2);
                        result->m_typeInfo.m_width = operand2->m_typeInfo.m_width;
                        evaluationCompleted = true;
                        bvm->deleteBV(bv1);
                        bvm->deleteBV(bv2);
                        bvm->deleteBV(mask1);
                        bvm->deleteBV(mask2);
                        bvm->deleteBV(mask);
                    }
                    else if (msb1.bv_width > 1)
                    {
                        trueCondition = true;
                    }
                    else
                    {
                        trueCondition = bvm->checkBVEquality(one, msb1);
                    }
                    bvm->deleteBV(one);
                    if (evaluationCompleted == false)
                    {
                        if (trueCondition == true)
                        {
                            bvm->bv_copy(msb2,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                            bvm->bv_copy(lsb2,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                            result->m_typeInfo.m_width = operand2->m_typeInfo.m_width;
                            evaluationCompleted = true;
                        }
                        else
                        {
                            bvm->bv_copy(msb3,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
                            bvm->bv_copy(lsb3,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
                            result->m_typeInfo.m_width = operand3->m_typeInfo.m_width;
                            evaluationCompleted = true;
                        }
                    } // evaluation not completed yet

                    if(convertResultToMemory==true)
                    {
                        if(trueCondition==true)
                        {
                            boundsToConvertResult=operand2MemoryBounds;
                        }
                        else
                        {
                            boundsToConvertResult=operand3MemoryBounds;
                        }
                    }
                } // not a boolean type
            }//ite operator ends
            else if (operatorSymbol == "concat")
            {
                bvatom temp1 = bvm->concat(msb1, msb2);
                bvatom temp2 = bvm->concat(temp1, msb3);
                bvm->deleteBV(temp1);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = temp2;
                temp1 = bvm->concat(lsb1, lsb2);
                temp2 = bvm->concat(temp1, lsb3);
                bvm->deleteBV(temp1);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = temp2;
                result->m_typeInfo.m_width = temp2.bv_width;
                evaluationCompleted = true;
                convertResultToMemory=false;
            }
            if(deleteOperand1==true)
            {
                bvm->deleteBV(msb1);
                bvm->deleteBV(lsb1);
            }
            if(deleteOperand2==true)
            {
                bvm->deleteBV(msb2);
                bvm->deleteBV(lsb2);
            }
            if(deleteOperand3==true)
            {
                bvm->deleteBV(msb3);
                bvm->deleteBV(lsb3);
            }
        }//else if operands size 3 ends
        else if (evaluationCompleted == false)// OPERATORS with MORE THAN THREE OPERANDS
        {
            if (operatorSymbol == "concat" && operands.size() > 3)
            {
                t_ExpressionValue *operand1 = operands[0];
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR; //result->m_typeInfo.m_type = TYPE_TWOBIT_BITVECTOR;
                bvatom msb1;
                bvatom lsb1;
                if (operands[0]->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
                {
                    string mem_str = operands[0]->getActualValueAsString();
                    t_FourValuedBitVector bv4bit = convertStringTo2BitBitVector(mem_str, mem_str.length());
                    msb1 = bv4bit.m_msb;
                    lsb1 = bv4bit.m_lsb;
                }
                else
                {
                    msb1 = operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                    lsb1 = operands[0]->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
                }
                bvatom msbTemp;
                bvatom lsbTemp;

                for (int i = 1; i < operands.size(); i++)
                {
                    t_ExpressionValue *temp = operands[i];
                    if (temp->m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
                    {
                        string mem_str = temp->getActualValueAsString();
                        t_FourValuedBitVector bv4bit = convertStringTo2BitBitVector(mem_str, mem_str.length());
                        msbTemp = bv4bit.m_msb;
                        lsbTemp = bv4bit.m_lsb;
                    }
                    else
                    {
                        msbTemp = temp->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                        lsbTemp = temp->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
                    }
                    msb1 = bvm->concat(msb1, msbTemp);
                    lsb1 = bvm->concat(lsb1, lsbTemp);
                }

                result->m_typeInfo.m_width = msb1.bv_width;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = msb1;
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = lsb1;
                evaluationCompleted = true;
            }//operator symbol is concat ends
            
            //if operator is other than concat with number or operands greater than  3
            else
            {
                cout << " ERROR : Operator :" << operatorSymbol << " is NOT Implemented ! " << endl;
                delete result;
                return NULL;
            }
        }//else  operands size's else ends
        bvm->deleteBV(msbXValue); bvm->deleteBV(lsbXValue);
        bvm->deleteBV(msbZValue); bvm->deleteBV(lsbZValue);

        if (evaluationCompleted == true)
        {
            if(convertResultToMemory==true && boundsToConvertResult.size()!=0)
            {
                string bv_value = result->getActualValueAsString();
                t_ListOfMemoryEntries* mem_value=result->getMemoryEntryFromString(bv_value,&boundsToConvertResult);
                result->m_typeInfo.m_type= TYPE_MEMORY_ARRAY;
                result->m_typeInfo.m_width= -1;
                result->m_actualExpressionValueStored.exprvalueMemory = mem_value;
            }
            return result;
        }
        else
        {
            delete result;
            return NULL;
        }
    }// two bit bitvector evaluation ends
}//function end

t_ExpressionValue * t_ExpressionEvaluator::performEvaluationOnGivenOperandsWithOneBit(string &operatorSymbol, vector<t_ExpressionValue *> &operands)
{
    t_ExpressionValue *result = new t_ExpressionValue();
    if (operands.size() == 1) // Unary operators
    {
        t_ExpressionValue *operand1 = operands[0];
        t_TypeOfExpression typeInfo1 = operand1->getOnlyTypeOfExpression();
        if (operatorSymbol == "lognot")
        {
            if (typeInfo1 == TYPE_BOOL)
            {
                bool operand1Value = operand1->getBoolValueOfExpression();
                if (!operand1->isActualValueNULL())
                {
                    result->m_typeInfo.m_type = TYPE_BOOL;
                    result->m_typeInfo.m_width = 0;
                    string res = (!operand1Value) ? "true" : "false";
                    result->setActualValueBasedOnType(res);
                    return result;
                }
                else
                {
                    cout << "ERROR !! Operand 1 value is not assigned or not evaluated :"
                            << operatorSymbol << endl;
                    return NULL;
                }
            }
            else
            {
                cout << "ERROR !! Operator: lognot for Invalid Operand Value  : NULL" << endl;
                return NULL;
            }
        }
        else if (typeInfo1 == TYPE_UNSIGNED_BITVECTOR || typeInfo1 == TYPE_SIGNED_BITVECTOR)
            //unary bitvector operators
        {
            BVManager *bvm = BVManager::getInstance();
            bvatom op1 = operand1->getBitvectorValueOfExpression();
            if (operatorSymbol == "redand" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                bvatom resBv = bvm->reduction_and(op1);

                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "redor" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                bvatom resBv = bvm->reduction_or(op1);
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "rednor" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                bvatom resBv = bvm->reduction_nor(op1);
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "redxor" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                bvatom resBv = bvm->reduction_xor(op1);
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "redxnor" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                bvatom resBv = bvm->reduction_xnor(op1);
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "rednand" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                bvatom resBv = bvm->reduction_nand(op1);
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "bitneg" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = op1.bv_width;
                bvatom resBv = bvm->bitwise_negate(op1);
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else if (operatorSymbol == "countones" && !operand1->isActualValueNULL())
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                bvatom resBv = bvm->bitwise_negate(op1);
                result->setActualValueInExpressionValue(resBv);
                result->m_typeInfo.m_width = resBv.bv_width;
                cerr << "Operator: countones  Is not yet implemented in BV-Lib " << endl;
                return result;
            }
            else if (operatorSymbol == "unsigned" && typeInfo1 == TYPE_UNSIGNED_BITVECTOR)
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = op1.bv_width;
                bvatom resBv = op1;
                result->setActualValueInExpressionValue(resBv);
                return result;
            }
            else
            {
                cout << " ERROR : Invalid Operator:" << operatorSymbol
                        << "  (OR) operand 1 is NULL " << endl;
                return NULL;
            }
        }
    }
    else if (operands.size() == 2) // BINARY OPERATORS
    {
        t_ExpressionValue *operand1 = operands[0];
        t_ExpressionValue *operand2 = operands[1];
        if (operatorSymbol == "add" && !operand1->isActualValueNULL() && !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_SIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                signed int res = add(operand1->getSignedIntValueOfExpression(),
                                     operand2->getSignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                unsigned int res = add(operand1->getUnsignedIntValueOfExpression(),
                                       operand2->getUnsignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                result->m_typeInfo.m_type = TYPE_REAL;
                result->m_typeInfo.m_width = 0;
                float res = add(operand1->getRealValueOfExpression(),
                                operand2->getRealValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_add(op1, op2);
                //int maxwidth = operand1->typeInfo.width
                //bvatom res_after_selection  = bvm->select_with_int(op1Val,0,
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_add(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "sub" &&
                !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_SIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                signed int res = sub(operand1->getSignedIntValueOfExpression(),
                                     operand2->getSignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                unsigned int res = sub(operand1->getUnsignedIntValueOfExpression(),
                                       operand2->getUnsignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                result->m_typeInfo.m_type = TYPE_REAL;
                result->m_typeInfo.m_width = 0;
                float res = sub(operand1->getRealValueOfExpression(),
                                operand2->getRealValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_sub(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_sub(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                        << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "multiply" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_SIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                signed int res = multiply(operand1->getSignedIntValueOfExpression(),
                                          operand2->getSignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                unsigned int res = multiply(operand1->getUnsignedIntValueOfExpression(),
                                            operand2->getUnsignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                result->m_typeInfo.m_type = TYPE_REAL;
                result->m_typeInfo.m_width = 0;
                float res = multiply(operand1->getRealValueOfExpression(),
                                     operand2->getRealValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR && /**
        	 * Do the arithmetic add on op1 and op2 ,put result in msb and the negation
        	 * of the result in lsb
        	 *
        	 */
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_mult(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_mult(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "divide" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_SIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                signed int res = divide(operand1->getSignedIntValueOfExpression(),
                                        operand2->getSignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                unsigned int res = divide(operand1->getUnsignedIntValueOfExpression(),
                                          operand2->getUnsignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                result->m_typeInfo.m_type = TYPE_REAL;
                result->m_typeInfo.m_width = 0;
                float res = divide(operand1->getRealValueOfExpression(),
                                   operand2->getRealValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_div(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_div(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "mod" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_SIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                signed int res = modulus(operand1->getSignedIntValueOfExpression(),
                                         operand2->getSignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                result->m_typeInfo.m_type = TYPE_UNSIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                unsigned int res = modulus(operand1->getUnsignedIntValueOfExpression(),
                                           operand2->getUnsignedIntValueOfExpression());
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                    (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
            {
                BVManager *bvm = BVManager::getInstance();
                result->m_typeInfo.m_type = TYPE_SIGNED_INTEGER;
                result->m_typeInfo.m_width = 0;
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->arith_mod(op1, op2);
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                        << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "gt" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            result->m_typeInfo.m_type = TYPE_BOOL;
            result->m_typeInfo.m_width = 0;
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                bool res = greaterthan(operand1->getSignedIntValueOfExpression(),
                                       operand2->getSignedIntValueOfExpression());
                string resString = res ? "true" : "false";
                result->setActualValueBasedOnType(resString);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                string res = greaterthan(operand1->getUnsignedIntValueOfExpression(),
                                         operand2->getUnsignedIntValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                string res = greaterthan(operand1->getRealValueOfExpression(),
                                         operand2->getRealValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = bvm->checkBVGreaterThan(op1, op2) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = bvm->checkBVGreaterThan(op1, op2) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                        << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "ge" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            result->m_typeInfo.m_type = TYPE_BOOL;
            result->m_typeInfo.m_width = 0;
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                string res = greaterthanEqual(operand1->getSignedIntValueOfExpression(),
                                              operand2->getSignedIntValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                string res = greaterthanEqual(operand1->getUnsignedIntValueOfExpression(),
                                              operand2->getUnsignedIntValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                string res = greaterthanEqual(operand1->getRealValueOfExpression(),
                                              operand2->getRealValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res;
                if (op1.bv_width != op2.bv_width)
                {
                    res = "false";
                }
                else
                {
                    res = (bvm->checkBVGreaterThan(op1, op2) ||
                            bvm->checkBVEquality(op1, op2)) ? "true" : "false";
                }
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = (bvm->checkBVGreaterThan(op1, op2) ||
                        bvm->checkBVEquality(op1, op2)) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                        << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "le" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            result->m_typeInfo.m_type = TYPE_BOOL;
            result->m_typeInfo.m_width = 0;
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                string res = (!(greaterthan(operand1->getSignedIntValueOfExpression(),
                                            operand2->getSignedIntValueOfExpression()))) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                string res = !(greaterthan(operand1->getUnsignedIntValueOfExpression(),
                                           operand2->getUnsignedIntValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                string res = (!(greaterthan(operand1->getRealValueOfExpression(),
                                            operand2->getRealValueOfExpression()))) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                    (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = (!(bvm->checkBVGreaterThan(op1, op2))) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "lt" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            result->m_typeInfo.m_type = TYPE_BOOL;
            result->m_typeInfo.m_width = 0;
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                string res = (!(greaterthanEqual(operand1->getSignedIntValueOfExpression(),
                                                 operand2->getSignedIntValueOfExpression()))) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                string res = (!greaterthanEqual(operand1->getUnsignedIntValueOfExpression(),
                                                operand2->getUnsignedIntValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                string res = (!greaterthanEqual(operand1->getRealValueOfExpression(),
                                                operand2->getRealValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                    (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = (!(bvm->checkBVGreaterThan(op1, op2) || bvm->checkBVEquality(op1, op2))) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                        << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "logand" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_BOOL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_BOOL)
            {
                result->m_typeInfo.m_type = TYPE_BOOL;
                result->m_typeInfo.m_width = 0;
                string res = (operand1->getBoolValueOfExpression() &&
                        operand2->getBoolValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "logor" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_BOOL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_BOOL)
            {
                result->m_typeInfo.m_type = TYPE_BOOL;
                result->m_typeInfo.m_width = 0;
                string res = ((operand1->getBoolValueOfExpression() ||
                        operand2->getBoolValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "logeq" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            result->m_typeInfo.m_type = TYPE_BOOL;
            result->m_typeInfo.m_width = 0;
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                string res = checkEqual(operand1->getSignedIntValueOfExpression(),
                                        operand2->getSignedIntValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                string res = checkEqual(operand1->getUnsignedIntValueOfExpression(),
                                        operand2->getUnsignedIntValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                string res = checkEqual(operand1->getRealValueOfExpression(),
                                        operand2->getRealValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                    (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = bvm->checkBVEquality(op1, op2) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_BOOL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_BOOL)
            {
                result->m_typeInfo.m_type = TYPE_BOOL;
                result->m_typeInfo.m_width = 0;
                string res = (operand1->getBoolValueOfExpression() == operand2->getBoolValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "diseq" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            result->m_typeInfo.m_type = TYPE_BOOL;
            result->m_typeInfo.m_width = 0;
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                string res = (!checkEqual(operand1->getSignedIntValueOfExpression(),
                                          operand2->getSignedIntValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                string res = (!checkEqual(operand1->getUnsignedIntValueOfExpression(),
                                          operand2->getUnsignedIntValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_REAL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_REAL)
            {
                string res = (!checkEqual(operand1->getRealValueOfExpression(),
                                          operand2->getRealValueOfExpression())) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                    (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2 = operand2->getBitvectorValueOfExpression();
                string res = (!bvm->checkBVEquality(op1, op2)) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_BOOL &&
                    operand2->getOnlyTypeOfExpression() == TYPE_BOOL)
            {
                result->m_typeInfo.m_type = TYPE_BOOL;
                result->m_typeInfo.m_width = 0;
                string res = (operand1->getBoolValueOfExpression() == operand2->getBoolValueOfExpression()) ? "true" : "false";
                result->setActualValueBasedOnType(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "bitand" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL() &&
                ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)))
        {
            BVManager *bvm = BVManager::getInstance();
            bvatom op1 = operand1->getBitvectorValueOfExpression();
            bvatom op2 = operand2->getBitvectorValueOfExpression();
            bvatom res = bvm->bitwise_and(op1, op2);
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->setActualValueInExpressionValue(res);
            return result;
        }
        else if (operatorSymbol == "bitor" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL() &&
                ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)))
        {
            BVManager *bvm = BVManager::getInstance();
            bvatom op1 = operand1->getBitvectorValueOfExpression();
            bvatom op2 = operand2->getBitvectorValueOfExpression();
            bvatom res = bvm->bitwise_or(op1, op2);
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->setActualValueInExpressionValue(res);
            return result;
        }
        else if (operatorSymbol == "bitxor" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL() &&
                ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)))
        {
            BVManager *bvm = BVManager::getInstance();
            bvatom op1 = operand1->getBitvectorValueOfExpression();
            bvatom op2 = operand2->getBitvectorValueOfExpression();
            bvatom res = bvm->bitwise_xor(op1, op2);
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->setActualValueInExpressionValue(res);
            return result;
        }
        else if (operatorSymbol == "bitxnor" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL() &&
                ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)))
        {
            BVManager *bvm = BVManager::getInstance();
            bvatom op1 = operand1->getBitvectorValueOfExpression();
            bvatom op2 = operand2->getBitvectorValueOfExpression();
            bvatom res = bvm->bitwise_xnor(op1, op2);
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->setActualValueInExpressionValue(res);
            return result;
        }
        else if (operatorSymbol == "leftshift" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
                    operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR) &&
                    (operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                int op2 = operand2->getSignedIntValueOfExpression();
                string zeroedString = "0";
                for (int i = 0; i < op2; i++)
                    zeroedString = zeroedString + "0";
                bvatom op2Bv = bvm->getZeroedBVatom(op2);
                bvatom res = bvm->left_shift(op1, op2Bv);
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->setActualValueInExpressionValue(res);
                return result;
            }
            if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
                    operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR) &&
                    (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
                    operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                bvatom op2Bv = operand2->getBitvectorValueOfExpression();
                bvatom res = bvm->left_shift(op1, op2Bv);
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "rightshift" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if ((operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR ||
                    operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR) &&
                    (operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER))
            {
                BVManager *bvm = BVManager::getInstance();
                bvatom op1 = operand1->getBitvectorValueOfExpression();
                int op2 = operand2->getSignedIntValueOfExpression();
                string zeroedString = "0";
                for (int i = 0; i < op2; i++)
                    zeroedString = zeroedString + "0";
                bvatom op2Bv = bvm->getZeroedBVatom(op2);
                bvatom res = bvm->right_shift(op1, op2Bv);
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->setActualValueInExpressionValue(res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                        << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "concat" && !operand1->isActualValueNULL() &&
                !operand2->isActualValueNULL())
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                bvatom bv1 = operand1->getBitvectorValueOfExpression();
                bvatom bv2 = operand2->getBitvectorValueOfExpression();

                BVManager *bvm = BVManager::getInstance();

                bvatom bv_res = bvm->concat(bv1, bv2);

                result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
                result->m_typeInfo.m_width = bv_res.bv_width;
                result->setActualValueInExpressionValue(bv_res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                bvatom bv1 = operand1->getBitvectorValueOfExpression();
                bvatom bv2 = operand2->getBitvectorValueOfExpression();

                BVManager *bvm = BVManager::getInstance();

                bvatom bv_res = bvm->concat(bv1, bv2);

                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = bv_res.bv_width;
                result->setActualValueInExpressionValue(bv_res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "zeroextn")
        {
            if (operand1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR)
            {
                bvatom bv1 = operand1->getBitvectorValueOfExpression();
                bvatom bv2 = operand2->getBitvectorValueOfExpression();

                BVManager *bvm = BVManager::getInstance();
                string bv2_string = bvm->printBVasBinString(bv2);
                bvatom bv_res = bvm->BVExtend(bv1, (int) binaryStringToInteger(bv2_string), 1);


                result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
                result->m_typeInfo.m_width = bv_res.bv_width;
                result->setActualValueInExpressionValue(bv_res);
                return result;
            }
            else if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    operand2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
            {
                bvatom bv1 = operand1->getBitvectorValueOfExpression();
                bvatom bv2 = operand2->getBitvectorValueOfExpression();

                BVManager *bvm = BVManager::getInstance();
                string bv2_string = bvm->printBVasBinString(bv2);
                bvatom bv_res = bvm->BVExtend(bv1, (int) binaryStringToInteger(bv2_string), 0);

                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = bv_res.bv_width;
                result->setActualValueInExpressionValue(bv_res);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << operand1->getOnlyTypeOfExpression() << ","
                        << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else
        {
            cout << " Operator :" << operatorSymbol << " For : " << operand1->getOnlyTypeOfExpression()
                    << "," << operand2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
            return NULL;
        }

    }
    else if (operands.size() == 3) // OPERATORS with 3 OPERANDS
    {
        t_ExpressionValue *op1 = operands[0];
        t_ExpressionValue *op2 = operands[1];
        t_ExpressionValue *op3 = operands[2];
        if (operatorSymbol == "select" && !op1->isActualValueNULL() &&
                !op2->isActualValueNULL() && !op3->isActualValueNULL())
        {
            BVManager *bvm = BVManager::getInstance();
            bvatom op1Val = op1->getBitvectorValueOfExpression();
            int op2Val, op3Val;
            if (op2->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER &&
                    op3->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
            {
                op2Val = op2->getSignedIntValueOfExpression();
                op3Val = op3->getSignedIntValueOfExpression();
            }
            else if (op2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER &&
                    op3->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
            {
                op2Val = op2->getUnsignedIntValueOfExpression();
                op3Val = op3->getUnsignedIntValueOfExpression();
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : "
                        << op1->getOnlyTypeOfExpression() << ","
                        << op2->getOnlyTypeOfExpression() << ","
                        << op3->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
            if (op1Val.bv_width < op2Val + 1)
            {
                op1Val = bvm->BVExtend(op1Val, op2Val + 1, 0);
            }
            //cout<<" select:"<<bvm->printBVasBinString(op1Val)<<":"<<op2Val<<":"<<op3Val<<endl;
            bvatom resultBv = bvm->select_with_int(op1Val, op2Val, op3Val);
            result->m_typeInfo.m_type = op1->getOnlyTypeOfExpression();
            result->m_typeInfo.m_width = op2Val - op3Val;
            result->setActualValueInExpressionValue(resultBv);
            return result;
        }
        else if (operatorSymbol == "ite" && !op1->isActualValueNULL() &&
                !op2->isActualValueNULL() && !op3->isActualValueNULL())
        {
            if (op1->getOnlyTypeOfExpression() == TYPE_BOOL &&
                    ((op2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                    op3->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR) ||
                    (op2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                    op3->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)))
            {
                bool condition = op1->getBoolValueOfExpression();
                bvatom operandBv1 = op2->getBitvectorValueOfExpression();
                bvatom operandBv2 = op3->getBitvectorValueOfExpression();
                bvatom resultBv = (condition) ? operandBv1 : operandBv2;
                result->m_typeInfo.m_type = op2->getOnlyTypeOfExpression();
                result->m_typeInfo.m_width = op2->m_typeInfo.m_width;
                result->setActualValueInExpressionValue(resultBv);
                return result;
            }
            else
            {
                cout << " Operator :" << operatorSymbol << " For : " << op1->getOnlyTypeOfExpression()
                        << "," << op2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
                return NULL;
            }
        }
        else if (operatorSymbol == "concat" &&
                (op1->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                op2->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR &&
                op3->getOnlyTypeOfExpression() == TYPE_SIGNED_BITVECTOR))
        {
            // call recursively.
            bvatom bv1 = op1->getBitvectorValueOfExpression();
            bvatom bv2 = op2->getBitvectorValueOfExpression();
            bvatom bv3 = op3->getBitvectorValueOfExpression();
            BVManager *bvm = BVManager::getInstance();
            bvatom bv_res_temp = bvm->concat(bv1, bv2);
            bvatom bv_res = bvm->concat(bv_res_temp, bv3);

            result->m_typeInfo.m_type = TYPE_SIGNED_BITVECTOR;
            result->m_typeInfo.m_width = bv_res.bv_width;
            result->setActualValueInExpressionValue(bv_res);
            return result;
        }
        else if (operatorSymbol == "concat" &&
                (op1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR &&
                op2->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR))
        {
            bvatom bv1 = op1->getBitvectorValueOfExpression();
            bvatom bv2 = op2->getBitvectorValueOfExpression();
            bvatom bv3 = op3->getBitvectorValueOfExpression();
            BVManager *bvm = BVManager::getInstance();
            bvatom bv_res_temp = bvm->concat(bv1, bv2);
            bvatom bv_res = bvm->concat(bv_res_temp, bv3);

            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->m_typeInfo.m_width = bv_res.bv_width;
            result->setActualValueInExpressionValue(bv_res);
            return result;
        }
        else
        {
            cout << " Operator :" << operatorSymbol << " For : " << op1->getOnlyTypeOfExpression()
                    << "," << op2->getOnlyTypeOfExpression() << " is Not Implemented" << endl;
            return NULL;
        }
    }
    else // OPERATORS with MORE THAN THREE OPERANDS
    {
        if (operatorSymbol == "concat" && operands.size() > 3)
        {
            BVManager *bvm = BVManager::getInstance();
            t_ExpressionValue *ev1 = operands[0];
            result->m_typeInfo.m_type = ev1->getOnlyTypeOfExpression();
            bvatom prev_res = operands[0]->getBitvectorValueOfExpression();
            for (int i = 1; i < operands.size(); i++)
            {
                if (result->getOnlyTypeOfExpression() != operands[i]->getOnlyTypeOfExpression())
                {
                    cout << "ERROR : Invalid Operand types for Concat " << endl;
                    return NULL;
                }
                bvatom bvi = operands[i]->getBitvectorValueOfExpression();
                prev_res = bvm->concat(prev_res, bvi);
            }
            result->m_typeInfo.m_width = prev_res.bv_width;
            result->setActualValueInExpressionValue(prev_res);
            return result;
        }
        else
        {
            cout << " ERROR : Operator :" << operatorSymbol << " is NOT Implemented ! " << endl;
            return NULL;
        }
    }
    return NULL;
}

t_ExpressionValue * t_ExpressionEvaluator::performUpdateOperationOnMemory(vector<t_ExpressionValue*> &operandsOfUpdate)
{
    t_ExpressionValue* operand1 = operandsOfUpdate[0];
    t_ExpressionValue* operand2 = operandsOfUpdate[1];
    t_ExpressionValue* operand3 = operandsOfUpdate[2];
    t_ExpressionValue* result = new t_ExpressionValue();

    bvatom op2Msb = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
    bvatom op2Lsb = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
    BVManager* bvm = BVManager::getInstance();        
    int index = bvm->convertBVToUnsignedInt(op2Msb);
    
    // First operand of update should be a memory 
    if (operand1->getOnlyTypeOfExpression() != TYPE_MEMORY_ARRAY)
    {
        cerr << " Non memory operand called for memory update operation" << endl;

        string op3Value = operand3->getActualValueAsString();
        t_FourValuedBitVector op3Bv = convertStringTo2BitBitVector(op3Value, op3Value.size());
        cerr << "Operand1:" << operand1->getActualValueAsString()
                << "\n Index:" << index
                << " op3Val:" << op3Value << endl;

        bvatom op1Msb = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
        bvatom op1Lsb = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;

        if (index + op3Value.size() < op1Msb.bv_width)
        {
            bvm->replaceBySelBVatom(op3Bv.m_msb, op1Msb, 0, index, op3Value.length());
            bvm->replaceBySelBVatom(op3Bv.m_lsb, op1Lsb, 0, index, op3Value.length());
        }
        result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
        result->m_typeInfo.m_width = op1Msb.bv_width;

        result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = op1Msb;
        result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = op1Lsb;
        //exit(-1);
        return result;
    } // end of first operand memory check
    
    t_MemoryDataType* memoryArray_Original = operand1->m_actualExpressionValueStored.exprvalueMemory;
    int memoryEntryType = memoryArray_Original->getTypeOfMemoryEntry();
    if (memoryEntryType == BASE_VALUE_MEMORY_ENTRY)
    {
        // First operand of a update always should be a array
        t_ExpressionValue* evalue = new t_ExpressionValue();
        evalue->m_typeInfo.m_type = operand1->m_typeInfo.m_type;
        evalue->m_actualExpressionValueStored.exprvalueMemory = operand1->m_actualExpressionValueStored.exprvalueMemory;
        return evalue;
    }
    
    t_ListOfMemoryEntries* memList = ((t_ListOfMemoryEntries*) memoryArray_Original)->createACopy();
    t_MemoryDataType* valueOfOperand3 = operand3->m_actualExpressionValueStored.exprvalueMemory;
    
    // Bounds check in vlaue of update
    if (valueOfOperand3->m_bounds != NULL) {
        if (valueOfOperand3->m_bounds->empty() == false)
        {
            int dim = memList->m_bounds->size();
            int updatingDim = valueOfOperand3->m_bounds->size();
            if (dim - 1 != updatingDim)
            {
                cerr << " The dimensions of the assigning entry in update operation is lesser dimension than \noriginal operand1 dimension (Value NOT Updated)" << endl;
                cerr << " Current dimensions(Required):" << dim - 1
                        << "\n To Update with:" << updatingDim << endl;
                return NULL;
            }
        }
    }
    
    t_MemoryDataType* insertValue;
    if (valueOfOperand3->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
    {
        t_BaseValueOfMemoryType* base = (t_BaseValueOfMemoryType*) valueOfOperand3;
        insertValue = base->performRead();
    }
    else
    {
        t_ListOfMemoryEntries* listType = (t_ListOfMemoryEntries*) valueOfOperand3;
        insertValue = listType->createACopy();
        if (insertValue->m_bounds == NULL)
        {
            vector<t_MemoryBounds>::iterator it = memList->m_bounds->begin();
            vector<t_MemoryBounds>::iterator end = memList->m_bounds->end();
            insertValue->m_bounds = new vector<t_MemoryBounds > ();
            for (it++; it != end; it++)
                insertValue->m_bounds->push_back((*it));
        }
    }
    
    if (checkforZbit(op2Msb, op2Lsb) || checkforXbit(op2Msb, op2Lsb))
    {
        //Index is not clear
        /** Need to Change:
         *      If index in update(arr,index,value) is 'x' or 'z' then
         *              If value is 'u': return an uninitialized array
         *              else:   
         *                      Iterate through array and for every element in list of lists,
         *                              if existing value is not the same as "value"  then delete that element from the list
         *                                      ( this effectively makes the value at this index as 'u')
         */             
        
        if(insertValue->valueIsXorZ()==true)
        {
            //creating an uninitialized array value
            memList->m_lowLevelMemoryEntries.clear();
        }
        else
        {
            int i=0;
            for( ;i<memList->m_lowLevelMemoryEntries.size();)
            {
                t_MemoryDataType* cEntry = memList->m_lowLevelMemoryEntries[i];
                if(cEntry->isEquals(insertValue)==true)
                {
                    i++;
                    continue;
                }
                else
                {
                    //deleting this entry
                    vector<t_MemoryDataType*>::iterator it = memList->m_lowLevelMemoryEntries.begin()+i;
                    memList->m_lowLevelMemoryEntries.erase(it);
                }
            }            
        }
        result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
        result->m_typeInfo.m_width = -1;
        result->m_actualExpressionValueStored.exprvalueMemory = (t_MemoryDataType*) memList;
        delete insertValue;
        return result;
    }

    vector<t_MemoryBounds>::iterator it = memList->m_bounds->begin();
    if (index > (*it).m_upperbound + 1)
    {
        result->copyValueOfMemory(operand1);
        result->m_typeInfo.m_type = operand1->m_typeInfo.m_type;
        result->m_typeInfo.m_width = operand1->m_typeInfo.m_width;
        return result;
    }

    
    

    result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
    result->m_typeInfo.m_width = -1;

    memList->updateAnEntryInListType(index, insertValue);
    result->m_actualExpressionValueStored.exprvalueMemory = (t_MemoryDataType*) memList;
    return result;
}

t_ExpressionValue * t_ExpressionEvaluator::performReadOperationOnMemory(vector<t_ExpressionValue*> &operandsOfUpdate)
{
    t_ExpressionValue* operand1 = operandsOfUpdate[0];
    t_ExpressionValue* operand2 = operandsOfUpdate[1];
    t_ExpressionValue* result = new t_ExpressionValue();
    if (operand1->getOnlyTypeOfExpression() != TYPE_MEMORY_ARRAY)
    {
        //cerr << " Non memory operand called for memory read operation" << endl;
        if (operand1->getOnlyTypeOfExpression() == TYPE_UNSIGNED_BITVECTOR)
        {
            BVManager* bvm = BVManager::getInstance();
            bvatom op2Msb = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
            bvatom op2Lsb = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->m_typeInfo.m_width = 1;

            if (checkforXbit(op2Msb, op2Lsb) || checkforZbit(op2Msb, op2Lsb))
            {
                // Log for unclear index for read
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = getZerosOrOnes(1, "1");
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = getZerosOrOnes(1, "1");
                return result;
            }
            else
            {
                bvatom op1 = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
                bvatom op1lsb = operand1->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
                int index = bvm->convertBVToUnsignedInt(op2Msb);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->select_with_int(op1, index, index);
                result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->select_with_int(op1lsb, index, index);
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = 1;
                return result;
            }
        }
        return NULL;
    }
    t_MemoryDataType* memoryArray = operand1->m_actualExpressionValueStored.exprvalueMemory;
    if (memoryArray == NULL)
    {
        cout << " Memory value is NULL" << endl;
    }
    int memoryEntryType = memoryArray->getTypeOfMemoryEntry();

    if (memoryEntryType == BASE_VALUE_MEMORY_ENTRY)
    {
        //cout << "Trying to read from base valued memory" << endl;
        result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
        t_BaseValueOfMemoryType* to_read = (t_BaseValueOfMemoryType*)operand1->m_actualExpressionValueStored.exprvalueMemory;
        result->m_actualExpressionValueStored.exprvalueMemory = to_read->performRead();
        return result;
    }
    bvatom op2Msb = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_msb;
    bvatom op2Lsb = operand2->m_actualExpressionValueStored.exprValue2BitVector.m_lsb;
    t_ListOfMemoryEntries* memList = (t_ListOfMemoryEntries*) memoryArray;
    vector<t_MemoryBounds>::iterator it = memList->m_bounds->begin();
    BVManager* bvm = BVManager::getInstance();
    int index = bvm->convertBVToUnsignedInt(op2Msb);

    if (checkforXbit(op2Msb, op2Lsb))
    {
        /** LIRA seems to consider the index as zero when index is given as 'x'
         *  We are trying to mimic the same
         */
        index = 0;
    }
    else if (checkforZbit(op2Msb, op2Lsb))
    {
        /* Need to change:
         *   If the expression is read(arr,'z') then 
         *      if all elements in arr have the same value 'v'  then return 'v'
         *      else return 'u'
         */
        if(memList->hasEqualElementsInAllIndex()==true) // all elements in arr have the same value
        {
            t_MemoryDataType* resultMem = memList->performReadOnList(0);
            if (resultMem->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
            {
                t_BaseValueOfMemoryType* base = (t_BaseValueOfMemoryType*) resultMem;
                result->m_actualExpressionValueStored.exprValue2BitVector = base->bvForThisBaseEntry;
                result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
                result->m_typeInfo.m_width = base->bvForThisBaseEntry.m_lsb.bv_width;
            }
            else
            {
                result->m_actualExpressionValueStored.exprvalueMemory = resultMem;
                result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
                result->m_typeInfo.m_width = -1;
            }
            return result;
        }
        else if (memList->m_bounds->size() == 1)
        {
            // creating a 'u' in base level
            string str = "1";
            result->m_actualExpressionValueStored.exprValue2BitVector.m_msb = bvm->getBVatom(1, str);
            result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb = bvm->getBVatom(1, str);
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->m_typeInfo.m_width = 1;
            return result;
        }
        else
        {
            //creating a list of entries with 'u' value
            result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
            t_ListOfMemoryEntries* list = new t_ListOfMemoryEntries();
            list->m_bounds = new vector<t_MemoryBounds > ();
            t_MemoryBounds temp = {0, 0, false};
            list->m_bounds->resize(memList->m_bounds->size() - 1, temp);
            it++;
            copy(it, memList->m_bounds->end(), list->m_bounds->begin());
            result->m_actualExpressionValueStored.exprvalueMemory = (t_MemoryDataType*) list;
            return result;
        }
    }

    if (index > (*it).m_upperbound + 1)
    {
        //cout<<"out of Bounds read error "<<index<<":"<<(*it).m_upperbound<<endl;
        if (memList->m_bounds->size() == 1)
        {
            string str = "1";
            bvatom bv = bvm->getBVatom(1, str);
            bvm->bv_copy(bv,result->m_actualExpressionValueStored.exprValue2BitVector.m_msb);
            bvm->bv_copy(bv,result->m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
            result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
            result->m_typeInfo.m_width = 1;
            return result;
        }
        else
        {
            result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
            t_ListOfMemoryEntries* list = new t_ListOfMemoryEntries();
            list->m_bounds = new vector<t_MemoryBounds > ();
            t_MemoryBounds temp = {0, 0, false};
            list->m_bounds->resize(memList->m_bounds->size() - 1, temp);
            it++;
            copy(it, memList->m_bounds->end(), list->m_bounds->begin());
            result->m_actualExpressionValueStored.exprvalueMemory = (t_MemoryDataType*) list;
            return result;
        }
    }
    t_MemoryDataType* resultMem = memList->performReadOnList(index);

    if (resultMem->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
    {
        t_BaseValueOfMemoryType* base = (t_BaseValueOfMemoryType*) resultMem;
        result->m_actualExpressionValueStored.exprValue2BitVector = base->bvForThisBaseEntry;
        result->m_typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
        result->m_typeInfo.m_width = base->bvForThisBaseEntry.m_lsb.bv_width;
    }
    else
    {
        result->m_actualExpressionValueStored.exprvalueMemory = resultMem;
        result->m_typeInfo.m_type = TYPE_MEMORY_ARRAY;
        result->m_typeInfo.m_width = -1;
    }
    return result;
}

t_ListOfMemoryEntries * t_ExpressionEvaluator::convert2BitBitVectorToOneDimensionalListOfMemoryEntries
(t_FourValuedBitVector bvValue, vector<t_MemoryBounds> &memoryDimensions)
{
    t_ListOfMemoryEntries* result = new t_ListOfMemoryEntries();
    int lowerBound = 0;
    int upperBound = 0;
    int i = 0;
    BVManager* bvm = BVManager::getInstance();
    while (upperBound < bvValue.m_msb.bv_width)
    {
        t_FourValuedBitVector currentBV;
        currentBV.m_msb = bvm->select_with_int(bvValue.m_msb, upperBound, lowerBound);
        currentBV.m_lsb = bvm->select_with_int(bvValue.m_lsb, upperBound, lowerBound);
        lowerBound = upperBound + 1;
        upperBound = upperBound + 1;
        t_BaseValueOfMemoryType* baseValueEntry = new t_BaseValueOfMemoryType();
        baseValueEntry->m_index = i;
        baseValueEntry->bvForThisBaseEntry = currentBV;
        result->updateAnEntryInListType(i, baseValueEntry);
        i++;
    }
    result->m_bounds = &memoryDimensions;
    return result;
}
