#include <vector>
#include<cassert>

#include "ExpressionValue.h"
using namespace std;

ostream & operator <<(ostream &os, const t_TypeOfValueStoredInExpressionLabel &obj)
{
    string value = "";
    switch (obj)
    {
    case SYMBOL:
        value = "SYMBOL";
        break;
    case CONSTANT:
        value = "CONSTANT";
        break;
    case OPERATOR:
        value = "OPERATOR";
        break;
    }
    os << value;
    return os;
}

bool t_ExpressionValue::setActualValueBasedOnType(string val)
{
    if (m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
    {
        m_actualExpressionValueStored.exprvalueSignedInt = stringToInteger(val);
        return true;
    }
    else if (m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
    {
        m_actualExpressionValueStored.exprvalueUnsignedInt = stringToInteger(val);
        return true;
    }
    else if (m_typeInfo.m_type == TYPE_REAL)
    {
        m_actualExpressionValueStored.exprvalueFloat = stringToFloat(val);
        return true;
    }/** /
    else if ( m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
      {
  	BVManager *bvm = BVManager::getInstance();
	if( val.length()>m_typeInfo.m_width)
	  {
	    val=val.substr(0,m_typeInfo.m_width);
	  }

	bvatom res= bvm->getBVatom(m_typeInfo.m_width,val);
	m_actualExpressionValueStored.exprvalueBitvector = res;
	return true;
	
      }*/
    else if (m_typeInfo.m_type == TYPE_TWOBIT_BITVECTOR || m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
    {
        BVManager* bvm = BVManager::getInstance();
        bvm->deleteBV(m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
        bvm->deleteBV(m_actualExpressionValueStored.exprValue2BitVector.m_msb);
        m_actualExpressionValueStored.exprValue2BitVector = convertStringTo2BitBitVector(val, m_typeInfo.m_width);
        return true;
    }
    else if (m_typeInfo.m_type == TYPE_BOOL)
    {
        m_actualExpressionValueStored.exprvalueBool = stringToBoolean(val);
    }
    else if (m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
    {
        return setInitialValueOfMemory(val);
    }
    else
    {
        return false;
    }
}

t_FourValuedBitVector convertStringTo2BitBitVector(string val, int width)
{
  //string msb = "", lsb = "";
    int length = 0;
    char* cmsb = new char[val.length() + 1];
    char* clsb = new char[val.length() + 1];
    while (length < val.length())
    {
        char cm='\0', cl='\0';
        switch (val[length])
        {
        case '0':
        {
            cm = '0';
            cl = '1';
            break;
        }
        case '1':
        {
            cm = '1';
            cl = '0';
            break;
        }
        case X_BIT:
        case U_BIT:
        {
            cm = '1';
            cl = '1';
            break;
        }
        case Z_BIT:
        {
            cm = '0';
            cl = '0';
            break;
        }
        }
        cmsb[length] = cm;
        clsb[length] = cl;
        length++;
    }
    cmsb[length] = '\0';
    clsb[length] = '\0';
    string msb (cmsb, cmsb+length);
    string lsb (clsb, clsb+length);

    BVManager *bvm = BVManager::getInstance();
    if (msb.length() > width)
    {
        msb = msb.substr(0, width);
        lsb = lsb.substr(0, width);
    }
    else if (msb.length() < width)
    {
        cout << " **************************** Warning ****************" << endl;
        cout << " Given string:" << val << " Required width:" << width << endl;
        string temp0 = "";
        string temp1 = "";
        for (int i = 0, len = msb.length(); i < width - len; i++)
        {
            temp0 = temp0 + "0";
            temp1 = temp1 + "1";
        }
        msb = temp0 + msb;
        lsb = temp1 + lsb;
    }
    bvatom msbVector = bvm->getBVatom(width, msb);
    delete[] cmsb; cmsb = NULL;
    bvatom lsbVector = bvm->getBVatom(width, lsb);
    delete[] clsb; clsb = NULL;
    t_FourValuedBitVector result = {msbVector, lsbVector};
    if(msbVector.bv_width != lsbVector.bv_width)
    {
        
        assert(msbVector.bv_width != lsbVector.bv_width);
    }
    return result;
}

string t_ExpressionValue::getActualValueAsString()
{
    string retVal;
    if (m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
    {
        retVal = toString(m_actualExpressionValueStored.exprvalueSignedInt);
        return retVal;
    }
    else if (m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
    {
        retVal = toString(m_actualExpressionValueStored.exprvalueUnsignedInt);
        return retVal;
    }
    else if (m_typeInfo.m_type == TYPE_REAL)
    {
        retVal = toString(m_actualExpressionValueStored.exprvalueFloat);
        return retVal;
    }
    else if (m_typeInfo.m_type == TYPE_BOOL)
    {
        retVal = toString(m_actualExpressionValueStored.exprvalueBool);
        return retVal;
    }/** /
   else if(m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR )
    {
   	BVManager* bvm = BVManager::getInstance();
    string result = bvm->printBVasBinString(m_actualExpressionValueStored.exprvalueBitvector);
	result.substr(result.length()-m_typeInfo.m_width, m_typeInfo.m_width );
	
      if(m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR && m_typeInfo.m_width ==1 && result == "")
	{
	  result = toString(m_actualExpressionValueStored.exprvalueBool);
	}
	    
      return result;
    }*/
    else if (m_typeInfo.m_type == TYPE_TWOBIT_BITVECTOR || m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
    {
        string retVal = getActual4BitValueAsString(m_actualExpressionValueStored.exprValue2BitVector);
        return retVal;
    }
    else if (m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
    {
        retVal = getActualMemoryAsString(m_actualExpressionValueStored.exprvalueMemory, m_actualExpressionValueStored.exprvalueMemory->m_bounds);
        return retVal;
    }
    else
    {
        cout << "ERROR ! Type is not assigned or not able to convert the expression value" << endl;
        return NULL;
    }


}

string t_ExpressionValue::getActual4BitValueAsString(t_FourValuedBitVector bv)
{
    BVManager *bvm = BVManager::getInstance();
    /*
        //     cout<<"Inside getBV str:"<<endl;
        string result1 = bvm->printBVasBinString(bv.m_msb);
        string result2 = bvm->printBVasBinString(bv.m_lsb);
        
        char* res = new char[result1.length() + 1];
        int i, n = result1.length() - 1;
        for (i = 0; i < n + 1; i++)
        {
            switch (result1[i])
            {
            case '1':
                switch (result2[i])
                {
                case '0': res[i] = '1';
                    break;
                case '1': res[i] = U_BIT;
                    break;
                }
                break;
            case '0':
                switch (result2[i])
                {
                case '0': res[i] = Z_BIT;
                    break;
                case '1': res[i] = '0';
                    break;
                }
                break;
            }
        }
        res[i] = '\0';
        reString = toString(res); */
    int avail_block_size = bvm->avail_block_size, i = 0;
    bv_block_t* mask = bvm->mask;
    char* res = new char[bv.m_msb.bv_width + 1];
    for (i = bv.m_msb.bv_width - 1; i >= 0; i--)
    {
        unsigned int bv_block = i / avail_block_size;
        unsigned int bv_offset = i % avail_block_size;
        bv_block_t msb_value = bv.m_msb.bv_array[bv_block] & mask[bv_offset];
        bv_block_t lsb_value = bv.m_lsb.bv_array[bv_block] & mask[bv_offset];
        int j = bv.m_msb.bv_width - i-1;
        if (msb_value != lsb_value)
        {
             if (msb_value != (bv_block_t) 0)
            {
                res[j] = '1';
            }
            else
            {
                res[j] = '0';
            }            
        }
        else
        {
           if (msb_value != (bv_block_t)0)
            {
                res[j] = U_BIT;
            }
            else
            {
                res[j] = Z_BIT;
            }
        }
    }
    res[bv.m_msb.bv_width] = '\0';
    string reString(res);
    delete[] res;
    return reString;
}

string t_ExpressionValue::getActualMemoryAsString(t_MemoryDataType* currentValue, vector<t_MemoryBounds>* &memoryBounds)
{
    t_MemoryDataType* memValue = currentValue;
    if (memValue == NULL)
    {
        return "[MEMORY IS EMPTY]";
    }
    try
    {
        if (memValue->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
        {
            t_BaseValueOfMemoryType* baseValue = (t_BaseValueOfMemoryType*) memValue;
            string baseValueString = getActual4BitValueAsString(baseValue->bvForThisBaseEntry);
            return baseValueString;
        }
        else
        {
            t_ListOfMemoryEntries* listValue = (t_ListOfMemoryEntries*) memValue;
            if ( memoryBounds->empty())
            {
                vector<t_MemoryDataType*>::iterator it = listValue->m_lowLevelMemoryEntries.begin();
                string str = "[";
                while (it != listValue->m_lowLevelMemoryEntries.end())
                {
                    t_MemoryDataType* mem = (*it);
                    string index = toString(mem->m_index);
                    str = str + "[" + index + "," + getActualMemoryAsString(mem, memoryBounds) + "]";
                    it++;
                }
                str = str + "]";
                return str;
            }
            else
            {
                vector<t_MemoryBounds>::iterator it = memoryBounds->begin();
                int ub = (*it).m_upperbound;
                int lb = (*it).m_lowerbound;
                int reverse_bits = (*it).m_packed;
                string result = "";
                it++;
                vector<t_MemoryDataType*>::iterator itMem = listValue->m_lowLevelMemoryEntries.begin();
                vector<t_MemoryBounds>* boundsLowerLevel = new vector<t_MemoryBounds > (it, memoryBounds->end());
                /**cout<<"Old Form  "<<getActualMemoryAsString(listValue, boundsLowerLevel)<<endl;
                while (it != memoryBounds->end()) {
                    boundsLowerLevel->push_back((*it));
                    it++;
                }*/
                int i = lb;
                //if ( reverse_bits==0) //ub < lb &&
                {
                    i = lb;
                    vector<string> vStrings;
                    while (i <= ub)
                    {
                        if (itMem != listValue->m_lowLevelMemoryEntries.end())
                        {
                            if ((*itMem)->m_index == i)
                            {
                                vStrings.push_back(getActualMemoryAsString((*itMem), boundsLowerLevel));
                                itMem++;
                            }
                            else
                            {
                                t_MemoryDataType* temp;
                                if (boundsLowerLevel->empty())
                                {
                                    temp = new t_BaseValueOfMemoryType(1);
                                }
                                else
                                {
                                    temp = new t_ListOfMemoryEntries();
                                }
                                vStrings.push_back(getActualMemoryAsString(temp, boundsLowerLevel));
                            }
                        }
                        else
                        {
                            t_MemoryDataType* temp;
                            if (boundsLowerLevel->empty())
                            {
                                temp = new t_BaseValueOfMemoryType(1);
                            }
                            else
                            {
                                temp = new t_ListOfMemoryEntries();
                            }
                            vStrings.push_back(getActualMemoryAsString(temp, boundsLowerLevel));
                        }
                        i++;
                    }
                    it--;
                    if (reverse_bits == 1)
                    {
                        reverse(vStrings.begin(), vStrings.end());
                    }
                    vector<string>::iterator itStr = vStrings.begin();
                    while (itStr != vStrings.end())
                    {
                        result = (*itStr) + result;
                        itStr++;
                    }

                } /*
                else
                {
                    while (i <= ub)
                    {
                        if (itMem != listValue->m_lowLevelMemoryEntries.end())
                        {
                            if ((*itMem)->m_index == i)
                            {
                                string t = getActualMemoryAsString((*itMem), boundsLowerLevel);
                                string result1 = t + result;
                                result = result1;
                                itMem++;
                            }
                            else
                            {
                                t_MemoryDataType* temp;
                                if (boundsLowerLevel->empty())
                                {
                                    result = "u" + result;
                                }
                                else
                                {
                                    temp = new t_ListOfMemoryEntries();
                                    string t = getActualMemoryAsString(temp, boundsLowerLevel);
                                    string result1 = t + result;
                                    result = result1;
                                }

                            }
                        }
                        else
                        {
                            t_MemoryDataType* temp;
                            if (boundsLowerLevel->empty())
                            {
                                result = "u" + result;
                            }
                            else
                            {
                                temp = new t_ListOfMemoryEntries();
                                string t = getActualMemoryAsString(temp, boundsLowerLevel);
                                string result1 = t + result;
                                result = result1;
                            }
                        }
                        //  cout<<i<<":"<<result<<endl;
                        i++;
                    }
               } */
                //cout<<i<<" ub:"<<ub<<" lb:"<<lb<<" res: "+result<<endl;
                return result;
            }
        }
    }
    catch (...)
    {
        return "[MEMORY IS EMPTY]";
    }
}

bool t_ExpressionValue::isActualValueNULL()
{
    if ((this->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR) ||
            (this->m_typeInfo.m_type == TYPE_SIGNED_INTEGER) ||
            (this->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER) ||
            (this->m_typeInfo.m_type == TYPE_REAL) ||
            (this->m_typeInfo.m_type == TYPE_BOOL) ||
            (this->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR) ||
            (this->m_typeInfo.m_type == TYPE_TWOBIT_BITVECTOR))
    {
        return false;
    }
    else
    {
        return true;
    }
}

t_TypeOfExpression t_ExpressionValue::getOnlyTypeOfExpression()
{
    return this->m_typeInfo.m_type;
}

signed int t_ExpressionValue::getSignedIntValueOfExpression()
{
    if (this->m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
    {
        return this->m_actualExpressionValueStored.exprvalueSignedInt;
    }
    else
    {
        string str = " Trying to read signed int value from type:";
        str = str + toString(m_typeInfo.m_type);
        throw std::invalid_argument("Trying to read signed from non-signed int type");
    }
}

unsigned int t_ExpressionValue::getUnsignedIntValueOfExpression()
{
    if (this->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
    {
        return this->m_actualExpressionValueStored.exprvalueUnsignedInt;
    }
    else
    {
//        string str = " Trying to read unsigned int value from:";
//        str = str + toString(m_typeInfo.m_type);
        throw std::invalid_argument("Trying to read unsigned from non-unsigned int type");
    }
}

bool t_ExpressionValue::getBoolValueOfExpression()
{
    if (this->m_typeInfo.m_type == TYPE_BOOL)
    {
        return this->m_actualExpressionValueStored.exprvalueBool;
    }
    else
    {
//        string str = " Trying to read signed bool value from:" + toString(m_typeInfo.m_type) + " type\n";
        throw std::invalid_argument("Trying to read bool from non-bool type");
    }
}

float t_ExpressionValue::getRealValueOfExpression()
{
    if (this-> m_typeInfo.m_type == TYPE_REAL)
    {
        return this->m_actualExpressionValueStored.exprvalueFloat;
    }
    else
    {
        throw std::invalid_argument("Trying to read real from non-real type");
    }
}

bvatom t_ExpressionValue::getBitvectorValueOfExpression()
{
    if (this->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || this->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
    {
        bvatom val = this->m_actualExpressionValueStored.exprvalueBitvector;
        return val;
    }
    else
    {
        string str = " Trying to read bitvector value from:";
        str = str + toString(m_typeInfo.m_type);

        throw std::invalid_argument("Trying to read bitvector from non-bitvector type");
    }
}

bool t_ExpressionValue::setActualValueInExpressionValue(signed int val)
{
    if (m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
    {
        m_actualExpressionValueStored.exprvalueSignedInt = val;
        return true;
    }
    else
    {
        return false;
    }
}

bool t_ExpressionValue::setActualValueInExpressionValue(unsigned int val)
{
    if (m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
    {
        m_actualExpressionValueStored.exprvalueUnsignedInt = val;
        return true;
    }
    else
    {
        return false;
    }
}

bool t_ExpressionValue::setActualValueInExpressionValue(float val)
{
    if (m_typeInfo.m_type == TYPE_REAL)
    {
        m_actualExpressionValueStored.exprvalueFloat = val;
        return true;
    }
    else
    {
        return false;
    }
}

bool t_ExpressionValue::setActualValueInExpressionValue(bvatom val)
{
    if (m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
    {
        if (m_actualExpressionValueStored.exprvalueBitvector.bv_array != NULL)
        {
           delete m_actualExpressionValueStored.exprvalueBitvector.bv_array;
        }
        m_actualExpressionValueStored.exprvalueBitvector = val;
        return true;
    }
    else
    {
        return false;
    }
}

bool t_ExpressionValue::setActualValueInExpressionValue(bool val)
{
    if (m_typeInfo.m_type == TYPE_BOOL)
    {
        m_actualExpressionValueStored.exprvalueBool = val;
    }
    else
    {
        return false;
    }
}

bool t_ExpressionValue::setInitialValueOfMemory(string initialValue)
{
    //Condition check string is printed as [][] or as a simple bitvector
    if (initialValue == "")
    {
        return true;
    }
    t_MemoryDataType* memEntry = m_actualExpressionValueStored.exprvalueMemory;
    if (memEntry == NULL)
    {
        cout << "Memory is not clear about dimensoins" << endl;
        // Do nothing
        return false;
    }
    if (memEntry->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
    {
        t_FourValuedBitVector valueToPut = convertStringTo2BitBitVector(initialValue, initialValue.length());
        t_BaseValueOfMemoryType* newValue = (t_BaseValueOfMemoryType*) memEntry;
        newValue->bvForThisBaseEntry = valueToPut;
        return true;
    }
    else if (memEntry->getTypeOfMemoryEntry() == LIST_MEMORY_ENTRIES)
    {
        t_ListOfMemoryEntries* memValue = (t_ListOfMemoryEntries*) memEntry;
        if (memValue->m_bounds == NULL || memValue->m_bounds->empty())
        {
            return false;
        }
        else
        {
            t_ListOfMemoryEntries* list = getMemoryEntryFromString(initialValue, memValue->m_bounds);
            if (memValue->m_lowLevelMemoryEntries.empty() == false)
            {
                memValue->deleteListOfMemoryEntries();
            }
            memValue->m_lowLevelMemoryEntries = list->m_lowLevelMemoryEntries;
            return true;
        }
    }
    else
    {
        return false;
    }
}

t_ListOfMemoryEntries* t_ExpressionValue::getMemoryEntryFromString(string value, vector<t_MemoryBounds>* bounds)
{
    vector<t_MemoryBounds>::iterator it = bounds->begin();
    int ub = (*it).m_upperbound;
    int lb = (*it).m_lowerbound;
    t_ListOfMemoryEntries* memValue = new t_ListOfMemoryEntries();
    memValue->m_bounds = new vector<t_MemoryBounds > (bounds->begin(), bounds->end());
    int i = ub;
    if ((*it).m_packed == true)
    {
        reverse(value.begin(), value.end());
    }
    else
    {
        //continue
    }
    memValue->m_lowLevelMemoryEntries.clear();
    it++;
    if (it == bounds->end())
    {
        memValue->assignABitStringIntoMemoryFromBeginning(value);
        return memValue;
    }
    else
    {
        vector<t_MemoryBounds>* lowerBounds = new vector<t_MemoryBounds > ();
        t_MemoryBounds temp;
        while (it != bounds->end())
        {
            lowerBounds->push_back((*it));
            it++;
        }
        int j = 0, i = ub, n = value.length() - 1;
        int range = value.length() / ((ub - lb) + 1);
        while (i >= lb)
        {
            string str = value.substr(n - j - range + 1, range);
            t_ListOfMemoryEntries* lentry = getMemoryEntryFromString(str, lowerBounds);
            lentry->m_index = ub - i;
            memValue->m_lowLevelMemoryEntries.push_back(lentry);
            i--;
            j += range;
        }
        return memValue;
    }
}

bool t_BaseValueOfMemoryType::updateAnEntryInBaseType(int index, t_MemoryDataType* insertValue)
{
    return false;
}

bool t_ListOfMemoryEntries::updateAnEntryInListType(int index, t_MemoryDataType* insertValue)
{
    vector<t_MemoryDataType*>::iterator it = m_lowLevelMemoryEntries.begin();
    int i = 0;
    insertValue->m_index = index;
    while (it != m_lowLevelMemoryEntries.end())
    {
        t_MemoryDataType* currentIndexValue = (*it);
        if (currentIndexValue->m_index == index)
        {

            m_lowLevelMemoryEntries.erase(it);
            break;
        }
        else if (currentIndexValue->m_index > index)
        {
            break;
        }
        it++;
        i++;
    }
    if (it == m_lowLevelMemoryEntries.end())
    {
        // cout<<" At the end "<<index<<endl;
        m_lowLevelMemoryEntries.push_back(insertValue);
    }
    else
    {
        // cout<<" Before "<<(*it)->m_index<<endl<<"Inserting:"<<insertValue->m_index<<endl;
        m_lowLevelMemoryEntries.insert(it, insertValue);
    }
    return true;

}

t_MemoryDataType* t_ListOfMemoryEntries::performReadOnList(int index, bool returnUvalueIfNotPresent)
{
    bool found = false;
    vector<t_MemoryDataType*>::iterator it = this->m_lowLevelMemoryEntries.begin();
    while (it != this->m_lowLevelMemoryEntries.end())
    {
        t_MemoryDataType* currentIndexValue = (*it);
        if (currentIndexValue->m_index == index)
        {
            found = true;
            break;
        }
        else if (currentIndexValue->m_index > index)
        {
            found = false;
            break;
        }
        it++;
    }
    cout<<"Found:"<<boolalpha<<found<<endl;
    if (found == true)
    {
        t_MemoryDataType* result = (*it);
        if (result->m_typeOfCurrentNode == BASE_VALUE_MEMORY_ENTRY)
        {
            return ((t_BaseValueOfMemoryType*) result)->performRead();
        }
        else
        {
            t_ListOfMemoryEntries* toReturn = ((t_ListOfMemoryEntries*) result)->createACopy();
            return toReturn;
        }
    }
    else if (returnUvalueIfNotPresent == false)
    {
        return NULL;
    }
    else
    {
        int dim = m_bounds->size();
        if (dim == 1)
        {
            BVManager* bvm = BVManager::getInstance();
            t_BaseValueOfMemoryType* nodeToReturn = new t_BaseValueOfMemoryType();
            string str = "1";
            nodeToReturn->bvForThisBaseEntry.m_lsb = bvm->getBVatom(1, str);
            nodeToReturn->bvForThisBaseEntry.m_msb = bvm->getBVatom(1, str);
            return (t_MemoryDataType*) nodeToReturn;
        }
        else
        {
            t_ListOfMemoryEntries* newListToReturn = new t_ListOfMemoryEntries();
            //Change by Tanmay and Dinesh (m_bounds->begin()  to m_bounds.begin()+1)
            newListToReturn->m_bounds = new vector<t_MemoryBounds > (m_bounds->begin()+1, m_bounds->end());
            return (t_MemoryDataType*) newListToReturn;
        }
    }
}

t_MemoryDataType* t_BaseValueOfMemoryType::performRead()
{
    t_BaseValueOfMemoryType* memBase = new t_BaseValueOfMemoryType();
    BVManager* bvm = BVManager::getInstance();
    bvm->bv_copy(this->bvForThisBaseEntry.m_lsb, memBase->bvForThisBaseEntry.m_lsb);
    bvm->bv_copy(this->bvForThisBaseEntry.m_msb, memBase->bvForThisBaseEntry.m_msb);
    memBase->m_bounds = new vector<t_MemoryBounds > ();
    memBase->m_index = m_index;
    return memBase;
}

t_ListOfMemoryEntries* t_ListOfMemoryEntries::createACopy()
{
    t_ListOfMemoryEntries* copyObj = new t_ListOfMemoryEntries();
    vector<t_MemoryDataType*>::iterator it = m_lowLevelMemoryEntries.begin();
    while (it != m_lowLevelMemoryEntries.end())
    {
        t_MemoryDataType* element = (*it); /**/
        t_MemoryDataType* elementtoCopy;
        if (element->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
        {
            elementtoCopy = ((t_BaseValueOfMemoryType*) element)->performRead();
        }
        else
        {
            elementtoCopy = ((t_ListOfMemoryEntries*) element)->createACopy();
        }
        copyObj->m_lowLevelMemoryEntries.push_back(elementtoCopy);
        it++;
    }
    copyObj->m_index = m_index;
    if (m_bounds != NULL)
    {
        copyObj->m_bounds = new vector<t_MemoryBounds > (m_bounds->begin(),m_bounds->end());
//        t_MemoryBounds temp;
//        copyObj->m_bounds->resize(m_bounds->size(), temp);
//        copy(m_bounds->begin(), m_bounds->end(), copyObj->m_bounds->begin());
    }
    return copyObj;
}

t_FourValuedBitVector t_ListOfMemoryEntries::convertListOfEntriesIntoABitVector(int width)
{
    int dim = m_bounds->size();
    int baseWidth = 1;
    if (dim == 1)
    {
        BVManager* bvm = BVManager::getInstance();
        string x = getXOrZValueOfLength(baseWidth, "1");
        t_FourValuedBitVector bvEmpty;
        bvEmpty.m_msb = bvm->getBVatom(baseWidth, x);
        bvEmpty.m_lsb = bvm->bitwise_negate(bvEmpty.m_msb);
        t_FourValuedBitVector result= {{NULL,0,0},{NULL,0,0}};
        int i = 0;
        vector<t_MemoryDataType*>::iterator it = m_lowLevelMemoryEntries.begin();
        while (it != m_lowLevelMemoryEntries.end() && i < width)
        {
            t_MemoryDataType* currentEntry = (*it);
            if (i == currentEntry->m_index)
            {
                result.m_msb = bvm->concat(result.m_msb, bvEmpty.m_msb);
                result.m_lsb = bvm->concat(result.m_lsb, bvEmpty.m_lsb);
                i = i + baseWidth;
            }
            else
            {
                t_BaseValueOfMemoryType* base = (t_BaseValueOfMemoryType*) currentEntry;
                result.m_msb = bvm->concat(result.m_msb, base->bvForThisBaseEntry.m_msb);
                result.m_lsb = bvm->concat(result.m_lsb, base->bvForThisBaseEntry.m_lsb);
                i = i + baseWidth;
                it++;
            }
        }
        return result;
    }
    else
    {
        cerr << " Recieved a List Of entries that has to be converted to BV"
                << "But bounds of the entry at dimension:" << dim << " Is required" << endl;
        t_FourValuedBitVector res={{NULL,0,0},{NULL,0,0}};
        return res;
    }
}

t_FourValuedBitVector t_ExpressionValue::convertMemoryToFourValuedBV(int width)
{
    t_MemoryDataType* currentValue = m_actualExpressionValueStored.exprvalueMemory;
    t_FourValuedBitVector bvRes;
    if (currentValue->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
    {
        BVManager* bvm = BVManager::getInstance();
        t_BaseValueOfMemoryType* base = (t_BaseValueOfMemoryType*) currentValue;
        bvRes.m_msb = base->bvForThisBaseEntry.m_msb;
        bvRes.m_lsb = base->bvForThisBaseEntry.m_lsb;
        int baseWidth = 1;
        if (baseWidth > width)
        {
            bvRes.m_msb = bvm->select_with_int(base->bvForThisBaseEntry.m_msb, width, 0);
            bvRes.m_lsb = bvm->select_with_int(base->bvForThisBaseEntry.m_lsb, width, 0);
        }
        else if (width > baseWidth)
        {
            string xvalue = getXOrZValueOfLength(width - baseWidth, "1");
            bvRes.m_msb = bvm->concat(bvm->getBVatom(baseWidth - width, xvalue), bvRes.m_msb);
            bvRes.m_lsb = bvm->concat(bvm->getBVatom(baseWidth - width, xvalue), bvRes.m_lsb);
        }
        return base->bvForThisBaseEntry;
    }
    else
    {
        t_ListOfMemoryEntries* list = (t_ListOfMemoryEntries*) currentValue;
        t_FourValuedBitVector bvResult = list->convertListOfEntriesIntoABitVector(width);
    }
}

bool t_BaseValueOfMemoryType::copyValueFromMemory(t_MemoryDataType* mem_value)
{
    if (mem_value->getTypeOfMemoryEntry() != BASE_VALUE_MEMORY_ENTRY)
        return false;
    else
    {
        BVManager* bvm = BVManager::getInstance();
        t_FourValuedBitVector bvCopy = ((t_BaseValueOfMemoryType*) mem_value)->bvForThisBaseEntry;
        bvm->bv_exchange(this->bvForThisBaseEntry.m_msb, bvCopy.m_msb);
        bvm->bv_exchange(this->bvForThisBaseEntry.m_lsb, bvCopy.m_lsb);
        return true;
    }
}

/**
 * Has a side effect, if the bounds of copying memory array is not matched, then dimensions will be changed to mem_value dimensions. 
 */
bool t_ListOfMemoryEntries::copyValueFromMemory(t_MemoryDataType* mem_value)
{
    if (mem_value->getTypeOfMemoryEntry() != LIST_MEMORY_ENTRIES)
        return false;
    else
    {
        /**if (this->m_bounds->size() != mem_value->m_bounds->size())
            return false;
        else */
        {
            if (this->m_bounds == NULL)
            {
                this->m_bounds = new vector<t_MemoryBounds > (mem_value->m_bounds->begin(), mem_value->m_bounds->end());
            }
            else if (this->m_bounds->size() != mem_value->m_bounds->size())
            {
                //@IMPORTANT Changing the dimensions of the array, beware of this side effect.
                this->m_bounds = new vector<t_MemoryBounds > (mem_value->m_bounds->begin(), mem_value->m_bounds->end());
            }
            t_ListOfMemoryEntries* list_mem_value = (t_ListOfMemoryEntries*) mem_value;
            //int i = 0, n = list_mem_value->m_lowLevelMemoryEntries.size();
            for (vector<t_MemoryDataType*>::iterator it = list_mem_value->m_lowLevelMemoryEntries.begin(),
                    end = list_mem_value->m_lowLevelMemoryEntries.end(); it!=end; ++it)//i < n; i++, it = list_mem_value->m_lowLevelMemoryEntries.begin() + i)
            {
                t_MemoryDataType* value;
                int index = (*it)->m_index;
                if ((*it)->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
                {
                    value = new t_BaseValueOfMemoryType();
                    value->copyValueFromMemory(*it);
                }
                else
                {
                    value = new t_ListOfMemoryEntries();
                    value->copyValueFromMemory(*it);
                }
                this->updateAnEntryInListType(index, value);
            }
        }
        return true;
    }
    return true;
}

bool t_ListOfMemoryEntries::deleteListOfMemoryEntries()
{ /*
    for (vector<t_MemoryDataType*>::iterator it = m_lowLevelMemoryEntries.end(),
            end = m_lowLevelMemoryEntries.begin(); it != end; it--)
    {
        t_MemoryDataType* ce = (*it);
        delete ce;
    } */
    return true;
}

bool t_ListOfMemoryEntries::assignABitStringIntoMemoryFromBeginning(string bv)
{
    BVManager* bvm = BVManager::getInstance();
    int w = 1;
    int n = bv.size() - 1;
    for (int i = 0; i < n + 1;)
    {
        string subBv = bv.substr(i, w);
        t_BaseValueOfMemoryType* baseEntry = new t_BaseValueOfMemoryType(w);
        baseEntry->bvForThisBaseEntry = convertStringTo2BitBitVector(subBv, w);
        baseEntry->m_index = (n - i) / w;
        updateAnEntryInListType(baseEntry->m_index, baseEntry);
        i += w;
    }
    return true;
}

//Constructor

t_ExpressionValue::t_ExpressionValue()
{
    m_backPointerToExpression = NULL;
    m_actualExpressionValueStored.exprvalueMemory = NULL;
    m_actualExpressionValueStored.exprValue2BitVector.m_lsb.bv_array = NULL;
    m_actualExpressionValueStored.exprValue2BitVector.m_msb.bv_array = NULL;
}

//Destructor

t_ExpressionValue::~t_ExpressionValue()
{/*
 //   cout << "Deleting ExpressionValue:" << getActualValueAsString() << "\t Type:" << m_typeInfo.m_type << endl;
    if (m_typeInfo.m_type == TYPE_MEMORY_ARRAY)
    {
        if (m_actualExpressionValueStored.exprvalueMemory != NULL)
        {
           // delete m_actualExpressionValueStored.exprvalueMemory;
        }
    }
    else*/ if (m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR && m_typeInfo.m_width > 0)
    {
//        BVManager* bvm = BVManager::getInstance();
//        bvm->deleteBV(m_actualExpressionValueStored.exprValue2BitVector.m_lsb);
//        bvm->deleteBV(m_actualExpressionValueStored.exprValue2BitVector.m_msb);
    }
}

bool t_ExpressionValue::copyValueOfMemory(t_ExpressionValue* from)
{
    static int counter = 0;
    counter++;
    t_MemoryDataType* fromValue = from->m_actualExpressionValueStored.exprvalueMemory;
    t_MemoryDataType* toValue = this->m_actualExpressionValueStored.exprvalueMemory;
    //cout<<"Copying from "<<from->getActualValueAsString() <<" to "<<getActualValueAsString()<<endl;
    if ((toValue == NULL) || (toValue->getTypeOfMemoryEntry() != BASE_VALUE_MEMORY_ENTRY &&
            toValue->getTypeOfMemoryEntry() != LIST_MEMORY_ENTRIES))
    {
        if (fromValue->getTypeOfMemoryEntry() == BASE_VALUE_MEMORY_ENTRY)
        {
            this->m_actualExpressionValueStored.exprvalueMemory = new t_BaseValueOfMemoryType();
            toValue = this->m_actualExpressionValueStored.exprvalueMemory;
        }
        else
        {
            this->m_actualExpressionValueStored.exprvalueMemory = new t_ListOfMemoryEntries();
            toValue = this->m_actualExpressionValueStored.exprvalueMemory;
        }
    }
    return toValue->copyValueFromMemory(fromValue);
}

int t_ExpressionValue::get4BitBitVectorValueAsInteger()
{
    if (m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
    {
        t_FourValuedBitVector bvValue = m_actualExpressionValueStored.exprValue2BitVector;
        BVManager* bvm = BVManager::getInstance();
        bool noZbit = bvm->checkBVForZero(bvm->bitwise_negate(bvm->bitwise_or(bvValue.m_msb, bvValue.m_lsb)));
        bool noXbit = bvm->checkBVForZero(bvm->bitwise_and(bvValue.m_msb, bvValue.m_lsb));
        if (!noXbit || !noZbit)
            return -1;
        else
            return bvm->convertBVToUnsignedInt(bvValue.m_msb);
    }
    else if (m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
    {
        return m_actualExpressionValueStored.exprvalueSignedInt;
    }
    else if (m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
    {
        return m_actualExpressionValueStored.exprvalueUnsignedInt;
    }
    else
    {
        return -1;
    }
}

bool t_ListOfMemoryEntries::isEquals(t_MemoryDataType* mem2)
{
    if (mem2 == NULL)
    {
        return false;
    }
    else
    {
        int mem2Type = mem2->getTypeOfMemoryEntry();
        if (mem2Type != LIST_MEMORY_ENTRIES)
        {
            return false;
        }
        else
        {
            t_ListOfMemoryEntries* list_mem2 = (t_ListOfMemoryEntries*) mem2;
            if (list_mem2->m_bounds->size() != m_bounds->size())
            {
                return false;
            }
            else if (list_mem2->m_lowLevelMemoryEntries.size() != m_lowLevelMemoryEntries.size())
            {
                return false;
            }
            else
            {
                t_MemoryBounds currentBounds = *(m_bounds->begin());
                if (m_bounds->size() >= 2)
                {
                    cerr << "ERROR: (ExpressionValue.cc) Multi-dimensional Memory comparison not supported in current version of SS" << endl;
                    return true;
                }
                int i = currentBounds.m_lowerbound;
                for (; i <= currentBounds.m_upperbound; i++)
                {
                    t_MemoryDataType* v1 = this->performReadOnList(i, false);
                    t_MemoryDataType* v2 = list_mem2->performReadOnList(i, false);
                    if (v1 == NULL && v2 == NULL)
                    {
                        continue;
                    }
                    else if (v1 == NULL || v2 == NULL)
                    {
                        return false;
                    }
                    else if (v1->isEquals(v2) == false)
                    {
                        return false;
                    }
                    else
                    {
                        continue;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

bool t_BaseValueOfMemoryType::isEquals(t_MemoryDataType* mem2)
{
    if (mem2 == NULL)
    {
        return false;
    }
    else
    {
        if (mem2->getTypeOfMemoryEntry() != BASE_VALUE_MEMORY_ENTRY)
        {
            return false;
        }
        else
        {
            t_BaseValueOfMemoryType* base2 = (t_BaseValueOfMemoryType*) mem2;
            BVManager* bvm = BVManager::getInstance();
            if (bvm->checkBVEquality(base2->bvForThisBaseEntry.m_msb, bvForThisBaseEntry.m_msb) == false)
                return false;
            else if (bvm->checkBVEquality(base2->bvForThisBaseEntry.m_lsb, bvForThisBaseEntry.m_lsb) == false)
                return false;
            else
                return true;
        }
    }
    return false;
}

bool t_ListOfMemoryEntries::hasEqualElementsInAllIndex()
{
    if (m_bounds == NULL || m_bounds->empty() == true)
    {
        return true;
    }
    else
    {
        t_MemoryBounds currentBounds = *(m_bounds->begin());
        if (m_lowLevelMemoryEntries.size() != currentBounds.m_upperbound - currentBounds.m_lowerbound + 1)
        {
            return false;
        }
        t_MemoryDataType* firstElement = *(m_lowLevelMemoryEntries.begin());
        for (int i = 1, n = m_lowLevelMemoryEntries.size(); i < n; i++)
        {
            if (firstElement->isEquals(m_lowLevelMemoryEntries[i]) == false)
            {
                return false;
            }
            else
            {
                continue;
            }
        }
        return true;
    }
}

bool t_BaseValueOfMemoryType::valueIsXorZ()
{
    BVManager* bvm = BVManager::getInstance();
    bvatom bv1 = bvForThisBaseEntry.m_lsb;
    bvatom bv2 = bvForThisBaseEntry.m_msb;
    bool isX = !(bvm->checkBVForZero(bvm->bitwise_and(bv1, bv2)));
    bool isZ = !(bvm->checkBVForZero(bvm->bitwise_negate(bvm->bitwise_or(bv1, bv2))));
    return (isX || isZ);
}

bool t_ListOfMemoryEntries::valueIsXorZ()
{
    if (m_bounds == NULL || m_bounds->empty() == true)
    {
        return true;
    }
    else
    {
        int i = 0, n = m_lowLevelMemoryEntries.size();
        for (; i < n; i++)
        {
            t_MemoryDataType* cEntry = m_lowLevelMemoryEntries[i];
            if (cEntry->valueIsXorZ() == true)
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
}

