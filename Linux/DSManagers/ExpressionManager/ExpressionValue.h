#ifndef EXPRESSIONVALUE_H
#define EXPRESSIONVALUE_H

#include "DagManager.h"
#include "Oprs.h"
#include "helping_functions.h"
#include <list>
#include <algorithm>

#define DAG_EXPRESSION 1
#define INFINITY_WIDTH 32767
#define VARIABLE_WIDTH -1
#define X_BIT 'x'
#define Z_BIT 'z'
#define U_BIT 'u'
#define SINGLEBIT_EVALUATION 1
#define TWOBIT_EVALUATION 2
#define BITEVALUATION TWOBIT_EVALUATION

#define BASE_VALUE_MEMORY_ENTRY 200
#define LIST_MEMORY_ENTRIES  201
#define INTERMEDIATE_MEMORY_ARRAY_NAME "INTERMEDIATE"
#define INITIAL_VALUE_OF_MEMORY "x"

#define EXPRESSION_INVALID_BIT_LABEL "@INVALID_BIT"
#define EXPRESSION_TYPE DAG_EXPRESSION


/**
 * Forward Declarations of ExpressionManager class,Expression class, ExpressionAsDag
 */
class t_ExpressionAsDag;
class t_Expression;
class t_ExpressionEvaluator;
class t_ExpressionManager;
class t_ExpressionValue;
class t_InvalidBitManager;


typedef enum typeOfExpression
{
    TYPE_NONE = 0, //Further are autonumbered!!
    TYPE_BOOL, //Boolean
    TYPE_SIGNED_INTEGER, //Signed Integer
    TYPE_UNSIGNED_INTEGER,
    TYPE_REAL, //Real
    TYPE_SIGNED_BITVECTOR, //Bit vector
    TYPE_UNSIGNED_BITVECTOR,
    TYPE_TWOBIT_BITVECTOR,
    TYPE_MEMORY_ARRAY
} t_TypeOfExpression;

typedef enum valueTypeStoredInExpressionLabel
{
    SYMBOL = 1, //Label is a symbol
    CONSTANT,   //Label is a Constant
    OPERATOR,   //Label is an operator
    INVALID
} t_TypeOfValueStoredInExpressionLabel;

ostream& operator <<(ostream &os, const t_TypeOfValueStoredInExpressionLabel &obj);

typedef struct typeOfExpressionTuple
{
    t_TypeOfExpression m_type; //Type Of Expression
    int m_width; //width of bivector
} TypeOfExpressionTuple;

/**
 * For 4-valued simulation.(0,1,x,z)
 */
typedef struct fourValuedBitVector
{
    bvatom m_msb;
    bvatom m_lsb;
} t_FourValuedBitVector;


t_FourValuedBitVector convertStringTo2BitBitVector(string val, int width);

class t_MemoryDataType
{
public:
    vector<t_MemoryBounds> *m_bounds;
    int m_typeOfCurrentNode;
    int m_index;

    typedef vector<t_MemoryBounds>::iterator t_MemoryBoundsIterator;
public:

    t_MemoryDataType(int type)
    {
        this->m_typeOfCurrentNode = type;
        this->m_bounds=NULL;
        this->m_index=0;
    }

    t_MemoryDataType(int type, vector<t_MemoryBounds>* &copyObj)
    {
        this->m_typeOfCurrentNode = type;
        this->m_bounds = new vector<t_MemoryBounds > ();
       copy(copyObj->begin(), copyObj->end(), this->m_bounds->begin());
    }

    inline int getTypeOfMemoryEntry()
    {
        return this->m_typeOfCurrentNode;
    }

    inline int getIndex()
    {
        return this->m_index;
    }

    virtual bool copyValueFromMemory(t_MemoryDataType* mem_value)
    {
        return true;
    }

    inline t_MemoryBoundsIterator beginIteratorOnBounds()
    {
        if(m_typeOfCurrentNode==BASE_VALUE_MEMORY_ENTRY && m_bounds==NULL)
        {
            m_bounds=new vector<t_MemoryBounds>();
            return m_bounds->begin();
        }
        else
        {
            return m_bounds->begin();
        }
    }

    inline t_MemoryBoundsIterator endIteratorOnBounds()
    {
        if(m_typeOfCurrentNode==BASE_VALUE_MEMORY_ENTRY && m_bounds==NULL)
        {            
            //return NULL;
            throw logic_error(__FILE__":"+integerToString(__LINE__));
        }
        else
        {
            return m_bounds->end();
        }
    }

    virtual bool isEquals(t_MemoryDataType* mem2)=0;
    virtual bool valueIsXorZ()=0;
    ~t_MemoryDataType()
    {
        if(m_bounds!=NULL)
        {
            delete m_bounds;
        }
    }
    friend class t_ExpressionValue;
    friend class t_ExpressionEvaluator;
    friend class t_Expression;
};

class t_BaseValueOfMemoryType : public t_MemoryDataType
{
    /**
     * Dont forget to assign initial values to this object
     */
    t_FourValuedBitVector bvForThisBaseEntry;
public:

    t_BaseValueOfMemoryType() : t_MemoryDataType(BASE_VALUE_MEMORY_ENTRY)
    {
        bvForThisBaseEntry.m_lsb.bv_array=NULL;
        bvForThisBaseEntry.m_msb.bv_array=NULL;
    }

    t_BaseValueOfMemoryType(int baseWidth) : t_MemoryDataType(BASE_VALUE_MEMORY_ENTRY)
    {
        string initialValue = getXOrZValueOfLength(baseWidth, INITIAL_VALUE_OF_MEMORY);
        t_FourValuedBitVector valueToPut = convertStringTo2BitBitVector(initialValue, initialValue.length());
        this->bvForThisBaseEntry = valueToPut;
    }
    
    bool updateAnEntryInBaseType(int index, t_MemoryDataType* insertValue);

    t_MemoryDataType* performRead();
  
    bool copyValueFromMemory(t_MemoryDataType* mem_value);

    ~t_BaseValueOfMemoryType()
    {
        BVManager* bvm=BVManager::getInstance();
        bvm->deleteBV(bvForThisBaseEntry.m_lsb);
        bvm->deleteBV(bvForThisBaseEntry.m_msb);
    }
    bool isEquals(t_MemoryDataType* mem2);
    bool valueIsXorZ();
    friend class t_ExpressionValue;
    friend class t_ExpressionEvaluator;
    friend class t_Expression;
    friend class t_ListOfMemoryEntries;
};

class t_ListOfMemoryEntries : public t_MemoryDataType
{
    vector<t_MemoryDataType*> m_lowLevelMemoryEntries;
public:

    t_ListOfMemoryEntries() : t_MemoryDataType(LIST_MEMORY_ENTRIES)
    {
    }

    bool assignABitStringIntoMemoryFromBeginning(string bv);
   
    bool updateAnEntryInListType(int index, t_MemoryDataType* insertValue);
    t_MemoryDataType* performReadOnList(int index,bool returnUvalueIfNotPresent=true);

    t_ListOfMemoryEntries* createACopy();
    t_FourValuedBitVector convertListOfEntriesIntoABitVector(int width);

    bool copyValueFromMemory(t_MemoryDataType* mem_value);

    bool deleteListOfMemoryEntries();

    ~t_ListOfMemoryEntries()
    {
        deleteListOfMemoryEntries();
    }
    bool isEquals(t_MemoryDataType* mem2);
    bool hasEqualElementsInAllIndex();
    bool valueIsXorZ();
    friend class t_Expression;
    friend class t_ExpressionValue;
    friend class t_ExpressionEvaluator;
};

/**
   Expression Implementation Manager with specified implementataion manager.
 * So whenever some dag routine is to be called for current expression, expreesionImplementationManager will be passed.
 */
typedef struct expressionImplementationManager
{
  t_DAGManager *dagmanager;     //Expression is implemented as dag,which requires dagmanager
  t_InvalidBitManager* invalidbit_manager;
} t_ExpressionImplementationManager;

/**
 * class ExpressionValue
 */
class t_ExpressionValue
{
private:
    t_Expression *m_backPointerToExpression;
    TypeOfExpressionTuple m_typeInfo; //Type Inferencing should be done for evaluating this tuple
    t_TypeOfValueStoredInExpressionLabel m_labelType;
    
    struct actualExpressionValue
    {
        union{signed int exprvalueSignedInt;
        unsigned int exprvalueUnsignedInt;
        bool exprvalueBool;
        float exprvalueFloat;
        t_MemoryDataType* exprvalueMemory;
        };
        bvatom exprvalueBitvector; //bitvector here
        t_FourValuedBitVector exprValue2BitVector;
        
    } m_actualExpressionValueStored;

    // Takes a string and converts it to the type specified in typeInfo and stored correspondigly
    bool setActualValueBasedOnType(string val);
public:
    // Returns the actual value in string format
    string getActualValueAsString();

    // return true if the actual value is null or not
    bool isActualValueNULL();

    t_TypeOfExpression getOnlyTypeOfExpression();

    signed int getSignedIntValueOfExpression();

    unsigned int getUnsignedIntValueOfExpression();

    bool getBoolValueOfExpression();

    float getRealValueOfExpression();

    bvatom getBitvectorValueOfExpression();
private:
    bool setActualValueInExpressionValue(signed int val);

    bool setActualValueInExpressionValue(unsigned int val);

    bool setActualValueInExpressionValue(float val);

    bool setActualValueInExpressionValue(bvatom val);

    bool setActualValueInExpressionValue(bool val);

    bool setInitialValueOfMemory(string initialValue);

    string getActualMemoryAsString(t_MemoryDataType* currentValue, vector<t_MemoryBounds>* &memoryBounds);

    t_ListOfMemoryEntries* getMemoryEntryFromString(string value, vector<t_MemoryBounds>* bounds);
    
    string getActual4BitValueAsString(t_FourValuedBitVector bv);

    t_FourValuedBitVector convertMemoryToFourValuedBV(int width);

    int get4BitBitVectorValueAsInteger();
    
    bool copyValueOfMemory(t_ExpressionValue* from);

    // Constructor
    t_ExpressionValue();

    //Destructor
    ~t_ExpressionValue();
    
    friend class t_ExpressionAsDag;
    friend class t_Expression;
    friend class t_ExpressionEvaluator;
    friend class t_ExpressionManager;
public:
    // Inlined methods
    inline t_TypeOfValueStoredInExpressionLabel getTypeOfValueStoredInExpressionLabel()
    {
        return m_labelType;
    }

    inline int getWidth()
    {
        return this->m_typeInfo.m_width;
    }
};

#endif /* EXPRESSIONVALUE_H */
