#include <fstream>
#include <cmath>
#include <cassert>
#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <set>
#include <stack>
#include <list>

// Added by Supratik: Nov 27, 2013
// Why was this not #define-d here?
#define USE_CACHE
// End comment: Supratik

#ifndef QE //#ifndef added by Ajith John on 9 June 2013
#include "CDFG.h"
#endif 

#include "ExpressionManager.h"
#include "ExpressionValue.h"
#include "DAGSimplifier.h"
#include "ExpressionEvaluator.h"
#include "LogManager.h"
#include "InvalidBitManager.h"

#define MAKE_CYCLIC_MEM_INVALID
#define USE_GEN_INV_FUNCTION

#if defined(USE_GEN_INV_FUNCTION) && !defined(GEN_INV_LABEL)
#define GEN_INV_LABEL "genInv"
#endif

//extern const std::string EXPRESSION_LABEL_AT_N_PREFIX;

#ifndef QE //#ifndef added by Ajith John on 9 June 2013
extern CDFG* cdfg;
#endif  

using namespace std;
t_DAGSimplifier *ds;
ofstream *outfile;

/**Generate aggressively expression for index invalid.
 i.e. if index is invalid, then compare all values equal.
 * if this bit is not set in Config::invBitGenerationLevel, then
 * 
 * if read index is invalid assume the result to be invalid
 * else generate the invalid bits comparing the update values
 */
static const int invRead_indexInvalid = 0x1;

/**
 Generate aggressively if the invalid bit of update in read invalid.
 * if this bit is not set, then
 * if update index is invalid then assume the result to be invalid.
 * else if bit is set consider case for update on invalid index
 */
static const int invRead_updateIndexInvalid = 0x2; //if read(update(..,j,..),i) and j invalid then invalid
bool t_ExpressionManager::performReplacementForInvalidBit = false;


int t_ExpressionManager::getWidth(t_Expression* expr)
{
    if (expr != NULL)
    {
        t_ExpressionValue* val = getValue(expr);

        t_TypeOfExpression typ = getTypeOfExpressionWithOutWidth(expr);
        if (typ == TYPE_UNSIGNED_INTEGER || typ == TYPE_SIGNED_INTEGER)
        {
            return INTEGER_WIDTH; //32 bit integer.
        }
        if (typ == TYPE_MEMORY_ARRAY)
        {
            string label = getLabelOfExpression(expr);
            //if(label == "update" || label == "read")
            //    return getWidth( getVectorOfOperands(expr)[0]);
            //t_MemoryDataType* memDT = val->m_actualExpressionValueStored.exprvalueMemory;
            //if(memDT && memDT->m_bounds){
            pair<vector<t_MemoryBounds>, int> result = getBoundsOfArray(expr);
            const std::vector<t_MemoryBounds> &bounds = result.first;
            //*(memDT->m_bounds);
            
            int width = 1;
            for (int i = result.second; i < bounds.size(); i++)
            {
                width *= bounds[i].m_upperbound - bounds[i].m_lowerbound + 1;
            }
            return width;
            //}
        }
        return val->getWidth();
    }
    else
    {
        return -1;
    }
}

t_Expression* t_ExpressionManager::isWeakenedAtTime(const string& atomName, int time)
{
    if (time >= weakenTimeInfo.size())
        return NULL;
    map<string, t_Expression*>::iterator it = weakenTimeInfo.at(time).find(atomName);
    if (it != weakenTimeInfo[time].end())
    {
        return it->second;
    }
    return NULL;
}
//Defined in WSTE.cpp
string buildExpression(string reader, t_ExpressionManager* em, t_Expression* &expr);

t_Expression* t_ExpressionManager::getCktExprFromWeakenedExpression(t_Expression* expr)
{
    t_ExpressionManager *em = t_ExpressionManager::getSingleton();
    if(em->getLabelOfExpression(expr) != "ite")
	return NULL;
    vector<t_Expression*> iteOps = em->getVectorOfOperands(expr);

    if(em->getLabelOfExpression(iteOps.at(2)).find(ANTE_VALUE_SUFFIX)==string::npos)
	return NULL;
    //t_Expression* cond = iteOps.at(0);
    //assert(em->getLabelOfExpression(cond)=="logand");
    //Skipping the check for the condition because the ANTE_VALUE_SUFFIX is unique for
    //weakening

    return iteOps.at(1);
}

set<string> t_ExpressionManager::generateWeakenListWithTimeInfo(const string& lineSep, const string &fieldSep)
{
    set<string> result;
    ifstream file(t_ConfigurationOptions::getConfig()->m_weakenFile.c_str());

    if (!file)
    {
        return result;
    }
    string line;
    int maxEndTime = 0;
    while (getline(file, line))
    {
        trimInPlace(line);
        if (line.empty())
        {
            continue;
        }
        if (line[0] == '#')
        {
            continue;
        }

        vector<string> fields = split(line, ";", false);
        assert(fields.size() >= 3);
        result.insert(fields[0]);

        int st = stringToInteger(fields[1]);
        int et = stringToInteger(fields[2]);
        t_Expression* guardExpr = NULL;
        if(fields.size()==4) //If guard specified. Optional to make sure we do not break the old examples.
        {
            string guard = fields[3];
            guard = buildExpression(guard, this, guardExpr);
            trimInPlace(guard);
            assert(guard.empty() && guardExpr!=NULL);
        }
        else
        {
            guardExpr = createABitVectorConstantOneExpression();
        }
        assert(st < et);
        if (et > maxEndTime)
        {
            maxEndTime = et;
            weakenTimeInfo.resize(maxEndTime);
        }

        while (st < et)
        {
            weakenTimeInfo.at(st).insert(make_pair(fields[0], guardExpr)); //[fields[0]] = createABitVectorConstantOneExpression();
            ++st;
        }
    }
    return result;
}

t_ExpressionManager* t_ExpressionManager::singleton = NULL;
set<string> t_ExpressionManager::toWeaken;

#ifndef QE //#ifndef added by Ajith John on 9 June 2013
void t_ExpressionManager::updateWeakeningSet()
{

    typedef set<string> weakenSetType;
    weakenSetType weakenSet;

    // get the list of weakened signals specified as part of input

    weakenSet = t_ExpressionManager::getSingleton()->generateWeakenListWithTimeInfo();

    // adding other signals as needed (from antecedents) to the list of weakened signals

    if(t_ConfigurationOptions::getConfig()->m_bPerformWSTE )
    {
        ifstream file(t_ConfigurationOptions::getConfig()->m_antecedentSignalsFile.c_str());
        if (!file)
        {
            cerr << "AntecedentSignals file is not specified" << endl;
            assert(false);
        }

        string line;
        while (getline(file, line))
        {
            trimInPlace(line);
            if (!line.empty())
            {
                weakenSet.insert(line);
            }
        }
    }

    assert(cdfg != NULL);

    for (weakenSetType::iterator it = weakenSet.begin(), end = weakenSet.end(); it != end; ++it)
    {
        string line = *it; //weakenList[i];
        trimInPlace(line);
        if (line.empty())
            continue;
        toWeaken.insert(cdfg->getModuleNameFromInstantiation(line));
    }

    //  tranform( it, end, it2, fun2)
    //=>for(; it!=end; ++it){
    // *it2 = fun2(*it); it2++;}

}

t_Expression* t_ExpressionManager::weakenAtom(t_Expression* expr,
        const string& variable_name, int upper_bound, int lower_bound, int instance_number)
{
    ostringstream name_stream;
    name_stream << variable_name << "_" << upper_bound << "_" << lower_bound;
    string name = name_stream.str();
    //cout<<"Inside weakening "<<t_ConfigurationOptions::getConfig()->m_weakenFile<<endl;

    static bool bUpdatedWeakeningSet = false;
    if (!bUpdatedWeakeningSet && toWeaken.empty())
    {
        cout << "Updating weakening set." << endl;
        updateWeakeningSet();
        bUpdatedWeakeningSet = true;
    }
#ifdef USE_CACHE
    static t_HashTable<string, t_Expression*> cache;

    {
        t_HTStatusValue<t_Expression*> res = cache.hashtable_search(name);
        if (res.success())
            return res.getValue();
    }
#endif
    if (toWeaken.find(name) == toWeaken.end())
    {
        // cout << "Do not weaken " << name << endl;
        return expr;
    }
    // cout << "Weakening " << name << endl;

    static ofstream *weakenedListFile = NULL;
    static bool bOpenWeakenedListFile = false;
    
    if (!bOpenWeakenedListFile) {
	const string sWeakenedListFileName =
	    t_ConfigurationOptions::getConfig()->m_sTempInfoDir + "/WSTE/WeakenedList";	    
        weakenedListFile = new ofstream(sWeakenedListFileName.c_str(), ios::app);
	bOpenWeakenedListFile = true;
    }
    
    *weakenedListFile << name << endl;

#ifdef WEAKEN_ONLY_ONCE
    toWeaken.erase(toWeaken.find(name)); //make sure we do not weaken it again.
#endif

    t_ExpressionManager *em = t_ExpressionManager::getSingleton();

    string weaken = NAME_WEAKEN(variable_name, upper_bound, lower_bound, instance_number);
    string anteDrv = NAME_ANTECEDENT_DRIVEN(variable_name, upper_bound, lower_bound, instance_number);
    string anteValue = NAME_ANTECEDENT_VALUE(variable_name, upper_bound, lower_bound, instance_number);
    string anteInv = NAME_ANTECEDENT_INVALID(variable_name, upper_bound, lower_bound, instance_number);

    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
    t_Expression *weakenExpr = em->createSymbol(weaken, typeInfo);
    //    t_Signal *weakenSig = new t_Signal();
    //    {
    //        weakenSig->m_signal_name = weaken;
    //        weakenSig->m_isInternallyGeneratedBySS = false;
    //        weakenSig->m_lower_bound = weakenSig->m_upper_bound = 0;
    //        t_Atom *atom = new t_Atom(weaken, 0, 0, 1, 0, 0, NULL);
    //        atom->setExpression(weakenExpr);
    //        weakenSig->m_atoms_of_signal.push_back(atom);
    //    }
    t_Expression *anteDrvExpr = em->createSymbol(anteDrv, typeInfo);
    t_Expression *anteInvExpr = em->createSymbol(anteInv, typeInfo);

    TypeOfExpressionTuple typeInfo2 = {TYPE_UNSIGNED_BITVECTOR, upper_bound - lower_bound + 1};
    t_Expression *anteValueExpr = em->createSymbol(anteValue, typeInfo2);



    t_Expression *weakened;
    {
        string logor = "logor";
        string lognot = "lognot";
        string logand = "logand";
        t_Expression* condition = em->createOneBitExpressionWithTwoOperands(logand,
                em->createOneBitExpressionWithTwoOperands(logor,
                em->createOneBitExpressionWithOneOperand(lognot, anteDrvExpr),
                anteInvExpr),
                em->createOneBitExpressionWithOneOperand(lognot, weakenExpr));

        t_Expression* thenPart;
        {
            //            string zeros(typeInfo2.m_width, '0');
            //            t_Expression* zeroExpr = em->createConstant(zeros, typeInfo2);
            //            vector<t_Expression*> operands = t_ExpressionManager::buildVector(weakenExpr, zeroExpr, expr);
            //            string label = "ite";
            thenPart = expr; //em->createExpression(label, operands, typeInfo2);

        }

        t_Expression* elsePart = anteValueExpr;

        string lblIte = "ite";
        vector<t_Expression*> operands = t_ExpressionManager::buildVector(condition, thenPart, elsePart);
        weakened = em->createExpression(lblIte, operands, typeInfo2);
    }

    //    t_Expression* invExpr = em->generateInvalidBitExpression(expr);
    //
    //    t_Expression* weakenedInvExpr; 
    //    {
    //        string logor = "logor";
    //        string logand = "logand";
    //        string lognot = "lognot";
    //
    //        t_Expression *lhs = em->createOneBitExpressionWithTwoOperands(logor, weakenExpr, invExpr);
    //        t_Expression *rhs = em->createOneBitExpressionWithTwoOperands(logor, em->createOneBitExpressionWithOneOperand(lognot, anteDrvExpr),
    //                                                                      anteInvExpr);
    //
    //        weakenedInvExpr = em->createOneBitExpressionWithTwoOperands(logand, lhs, rhs);
    //    }

    //    em->setInvalidBitExpression(weakened, weakenedInvExpr);
//    if (variable_name.find("res104H") != string::npos)
//    {
//        em->printExpressionToFileAsDAG(name, weakened, cout, true);
//        getchar();
//    }
#ifdef USE_CACHE
    cache.hashtable_insert(name, weakened);
#endif
    return weakened;
}
#endif //#endif added by Ajith John on 9 June 2013

t_Expression* t_ExpressionManager::generateWeakenedInvalidBit(t_Expression* expr, t_Expression* weakenExpr, t_Expression* anteDrvExpr,
        t_Expression* anteInvExpr)
{
    t_Expression* invExpr = generateInvalidBitExpression(expr);

    t_Expression* weakenedInvExpr;
    {
        string logor = "logor";
        string logand = "logand";
        string lognot = "lognot";

        t_Expression *lhs = createOneBitExpressionWithTwoOperands(logor, weakenExpr, invExpr);
        t_Expression *rhs = createOneBitExpressionWithTwoOperands(logor, createOneBitExpressionWithOneOperand(lognot, anteDrvExpr),
                anteInvExpr);

        weakenedInvExpr = createOneBitExpressionWithTwoOperands(logand, lhs, rhs);
    }
    return weakenedInvExpr;
}

t_Expression* t_ExpressionManager::generateWeakenedInvalidBitFromITE(t_Expression* iteExpr)
{
    vector<t_Expression*> iteoperands = getVectorOfOperands(iteExpr);
    string label = getLabelOfExpression(iteoperands.at(2));
    t_Expression* expr = NULL;
    t_Expression* anteValue = NULL;
    t_Expression* anteDrvn = NULL;
    t_Expression* specAsWeak = NULL;
    t_Expression* anteInv = NULL;
    if (label.find(ANTE_VALUE_SUFFIX) != string::npos)
    {
        anteValue = iteoperands.at(2);
        expr = iteoperands.at(1);
    }
    else
    {
        return NULL;
    }

    label = getLabelOfExpression(iteoperands.at(0));
    if (label != m_operatorLabelLogAND)
        return NULL;
    vector<t_Expression*> andOper = getVectorOfOperands(iteoperands.at(0));
    label = getLabelOfExpression(andOper.at(1));
    if (label != m_operatorLabelLogNOT)
        return NULL;

    vector<t_Expression*> notOper = getVectorOfOperands(andOper.at(1));
    label = getLabelOfExpression(notOper.at(0));
    if (label.find(WEAKEN_SUFFIX) != string::npos)
        specAsWeak = notOper.at(0);
    else
        return NULL;

    label = getLabelOfExpression(andOper.at(0));
    if (label != m_operatorLabelLogOR)
        return NULL;
    vector<t_Expression*> orOper = getVectorOfOperands(andOper.at(0));

    label = getLabelOfExpression(orOper.at(1));
    if (label.find(INV_ANTE_SUFFIX) != string::npos)
    {
        anteInv = orOper.at(1);
    }
    else
        return NULL;

    label = getLabelOfExpression(orOper.at(0));
    if (label != m_operatorLabelLogNOT)
        return NULL;

    notOper = getVectorOfOperands(orOper.at(0));
    label = getLabelOfExpression(notOper.at(0));
    if (label.find(ANTE_DRIVEN_SUFFIX) != string::npos)
    {
        anteDrvn = notOper.at(0);
    }
    else
        return NULL;

    assert(expr != NULL);
    assert(anteDrvn != NULL);
    assert(anteInv != NULL);
    assert(specAsWeak != NULL);

    return generateWeakenedInvalidBit(expr, specAsWeak, anteDrvn, anteInv);
}

t_Expression* t_ExpressionManager::replaceExpressionInCopy(t_Expression* root, t_Expression* toreplace, t_Expression* replaceby)
{
    assert(root != NULL);
    if (root->getID() == toreplace->getID())
        return replaceby;
    if (isALeafExpression(root))
        return root;
    vector<t_Expression*> operands = getVectorOfOperands(root);
    bool changed = false;
    for (int i = 0; i < operands.size(); ++i)
    {
        t_Expression* orig = operands.at(i);
        t_Expression* newExpr = replaceExpressionInCopy(orig, toreplace, replaceby);
        if (orig->getID() != newExpr->getID())
            changed = true;
        operands.at(i) = newExpr;
    }
    if (!changed)
        return root;
    string label = getLabelOfExpression(root);
    TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(root);
    return createExpression(label, operands, typeInfo);
}

t_DAGNode* t_ExpressionManager::createDagNodeLevelExpression(const std::string& label, const vector<t_DAGNode*>& children)
{
    vector<t_Expression*> childExprs;
    for(vector<t_DAGNode*>::const_iterator it = children.begin(); it!= children.end(); ++it)
    {
        t_ExpressionValue *valueInExpression = (t_ExpressionValue *) ((*it)->getValue());
        t_Expression * expression_pointer = valueInExpression->m_backPointerToExpression;
        assert(expression_pointer!=NULL);
        childExprs.push_back( expression_pointer );
    }
    
    string label2 = label; //Since createExpression cannot accept const strings.
    t_Expression* expr = t_ExpressionManager::getSingleton()->createExpression(label2, childExprs);
    assert(expr!=NULL);
    return expr->m_dagExpression->getDag();
}

/**
 * Constructor  : ExpressionManager()
 * Arguments   : []
 * Description  : Will create required set of initialization of expression manager members  like Creating dummy expression getting the map Of operator semantics 
 **/
t_ExpressionManager::t_ExpressionManager(t_ConfigurationOptions config_options)
{
    m_expressionImplementationType = EXPRESSION_TYPE; //for Expression implementation see #define in ExpressionManager.h
    t_Logger* logManager = t_Logger::getInstance();
    m_logFile = new ofstream();
    const string sExpressionManagerLogFileName = 
	t_ConfigurationOptions::getConfig()->m_sLogDir + "/ExpressionManager.log";
    m_logFile->open(sExpressionManagerLogFileName.c_str());
    logManager->LOG("Expression Manager Instantiated with Expression Type:" + toString(EXPRESSION_TYPE), m_logFile, c_RunLevelVerbosity);
    if (m_expressionImplementationType == DAG_EXPRESSION)
    {
	m_expressionImplementationManagerInstance = new t_ExpressionImplementationManager;
        // Here only simpilification flag is true
        // Other flags such as restricted_mode etc.. are not given(Default flag values remain)
	m_expressionImplementationManagerInstance->dagmanager = new t_DAGManager(true, true, false, config_options.m_restructuring_disable_flag);
	m_expressionImplementationManagerInstance->invalidbit_manager = new t_InvalidBitManager();
        t_DAGManager::setSingleton(m_expressionImplementationManagerInstance->dagmanager);

	// Begin comment: Added by Supratik, Oct 22, 2013
	// No longer need to open outfile here and pass it to rules_parser.y as a global variable.
	// Instead, open and close the file in which the rules are to be written directly in
	// rules_parser.y
	//
	// outfile = new ofstream();
        // outfile ->open("rules");
	//
	// End comment 

        //Not Used by the Expression Manager. The DAGMAnager is responsible for simplification.
        //However the expression manager sets up the parameters for the simplifier.
        ds = new t_DAGSimplifier(m_expressionImplementationManagerInstance->dagmanager);
        
        //TODO: Uncomment this line to call createExpression instead of createDagNode in DAG Simplifier.
        //Commented because create expression not able to infer type.
        ds->setNodeCreationCallBack(&t_ExpressionManager::createDagNodeLevelExpression);
        string envName = "SS";
        char *name = getenv(envName.c_str());

	//assert(name != NULL); //line added by Ajith John on 9 June 2013

        string filename(name);
        filename += "/new_rules.txt";
        FILE *fp = fopen((char*) filename.c_str(), "r");
        if (fp == NULL)
        {
            cerr << "ERROR : " << filename << " Doesnot Exists " << endl;
            exit(-1);
        }
        extern FILE *rules_parserin;
        extern int rules_parserparse();
        rules_parserin = fp;
        rules_parserparse();
        /***
        extern int inferencerule_parserparse();

        logManager->LOG("Reading the operatorSemantics file \n", m_logFile, c_DebugLevelVerbosity);
        extern FILE *inferencerule_parserin;
        FILE *fp_inf_rules = fopen("operatorSemantics", "r");
        if (fp_inf_rules == NULL)
        {
            cerr << "ERROR:  OperatorSemantics File Doesnot exists" << endl;
            exit(-1);
        }
        inferencerule_parserin = fp_inf_rules;
        inferencerule_parserparse();

        logManager->LOG("Finished Reading the operatorSemantics file \n", m_logFile, c_DebugLevelVerbosity);
       // ofstream *inf_rules_file = new ofstream();
       // inf_rules_file->open("InferenceRules.TXT");
        //this->printMapOfTypeInferenceRules(inf_rules_file);
        //inf_rules_file->close(); /**/

        // Here Expression manager need to configure the dag simplifier to create the default expressions
        // Such as 0b0, 0b1 etc.
        string zero = "0b0";
        string one = "0b1";
        t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
        TypeOfExpressionTuple typeInfoForBVConstants0and1 = {TYPE_UNSIGNED_BITVECTOR, 1};
        m_dummyExpression->createExpression(zero, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        m_dummyExpression->createExpression(one, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        string trueStr = "true";
        string falseStr = "false";
        typeInfoForBVConstants0and1.m_type = TYPE_BOOL;
        typeInfoForBVConstants0and1.m_width = 0;
        m_dummyExpression->createExpression(trueStr, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        m_dummyExpression->createExpression(falseStr, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }

    m_operatorLabelADD = "add";
    m_operatorLabelSUB = "sub";
    m_operatorLabelMultiply = "multiply";
    m_operatorLabelDivide = "divide";
    m_operatorLabelModulus = "mod";

    m_operatorLabelRedOR = "redor";
    m_operatorLabelRedAND = "redand";
    m_operatorLabelRedXOR = "redxor";
    m_operatorLabelRedNOR = "rednor";
    m_operatorLabelRedNAND = "rednand";
    m_operatorLabelRedXNOR = "redxnor";

    m_operatorLabelBitwiseNEG = "bitneg";
    m_operatorLabelBitwiseAND = "bitand";
    m_operatorLabelBitwiseOR = "bitor";
    m_operatorLabelBitwiseXOR = "bitxor";
    m_operatorLabelBitwiseXNOR = "bitxnor";
    m_operatorLabelBitwiseNOR = "bitnor";
    m_operatorLabelBitwiseNAND = "bitnand";


    m_operatorLabelLogNOT = "lognot";
    m_operatorLabelLogAND = "logand";
    m_operatorLabelLogOR = "logor";
    m_operatorLabelLogicalEquality = "logeq";
    m_operatorLabelLogicalInEquality = "diseq";
    m_operatorLabelLogicalWildEquality = "wildeq";
    m_operatorLabelCaseXequlity = "casexeq";
    m_operatorLabelCaseZequlity = "casezeq";
    m_operatorLabelLogicalWildInEquality = "wilddiseq";
    m_operatorLabelLessThan = "lt";
    m_operatorLabelLessThanOrEqual = "le";
    m_operatorLabelGreaterThan = "gt";
    m_operatorLabelGreaterThanOrEqual = "ge";

    m_operatorLabelIte = "ite";
    m_operatorLabelConcat = "concat";
    m_operatorLabelZeroextension = "zeroextn";
    m_operatorLabelSignextension = "signextn";
    m_operatorLabelSelect = "select";

    m_operatorLabelLeftShift = "leftshift";
    m_operatorLabelRightShift = "rightshift";
    m_operatorLabelLefttoRightStream = "leftToRightStream";
    m_operatorLabelRighttoLeftStream = "rightToLeftStream";

    m_typeInfoOfInvalidBitExpr.m_type = TYPE_UNSIGNED_BITVECTOR;
    m_typeInfoOfInvalidBitExpr.m_width = 1;

    m_operatorLabelType = OPERATOR;


    this->m_evaluator = new t_ExpressionEvaluator();
    m_dummyExpression = new t_Expression();
    m_dummyExpression->m_logFile = new ofstream();
    const string sExpressionLogFileName = 
	t_ConfigurationOptions::getConfig()->m_sLogDir + "/Expression.log";	
    m_dummyExpression->m_logFile->open(sExpressionLogFileName.c_str());

    m_useEncodingOfExpressions = true;

    if (singleton == NULL)
        singleton = this;
    else
        cout << "-------------- WARNING: Multiple Expression Managers Created. ----------------" << endl;
}

/**
 * Constructor  : ExpressionManager()
 * Arguments   : []
 * Description  : Will create required set of initialization of expression manager members  like Creating dummy expression getting the map Of operator semantics 
 **/
t_ExpressionManager::t_ExpressionManager(bool recursion_recompute_signatures)
{
    m_expressionImplementationType = EXPRESSION_TYPE; //for Expression implementation see #define in ExpressionManager.h
    t_Logger* logManager = t_Logger::getInstance();
    m_logFile = new ofstream();
    const string sExpressionManagerLogFileName =
	t_ConfigurationOptions::getConfig()->m_sLogDir + "/ExpressionManager.log";
    m_logFile->open(sExpressionManagerLogFileName.c_str());
    logManager->LOG("Expression Manager Instantiated with Expression Type:" + toString(EXPRESSION_TYPE), m_logFile, c_RunLevelVerbosity);
    if (m_expressionImplementationType == DAG_EXPRESSION)
    {
	m_expressionImplementationManagerInstance = new t_ExpressionImplementationManager;
        // Here only simpilification flag is true
        // Other flags such as restricted_mode etc.. are not given(Default flag values remain)
	m_expressionImplementationManagerInstance->dagmanager = new t_DAGManager(true, true, false, recursion_recompute_signatures);
	m_expressionImplementationManagerInstance->invalidbit_manager = new t_InvalidBitManager();
        t_DAGManager::setSingleton(m_expressionImplementationManagerInstance->dagmanager);

	// Begin comment: Added by Supratik, Oct 22, 2013
	// No longer need to open outfile here and pass it to rules_parser.y as a global variable.
	// Instead, open and close the file in which the rules are to be written directly in
	// rules_parser.y
	//
	// outfile = new ofstream();
        // outfile ->open("rules");
	//
	// End comment

        //Not Used by the Expression Manager. The DAGMAnager is responsible for simplification.
        //However the expression manager sets up the parameters for the simplifier.
        ds = new t_DAGSimplifier(m_expressionImplementationManagerInstance->dagmanager);
        
        //TODO: Uncomment this line to call createExpression instead of createDagNode in DAG Simplifier.
        //Commented because create expression not able to infer type.
        ds->setNodeCreationCallBack(&t_ExpressionManager::createDagNodeLevelExpression);
        string envName = "SS";
        char *name = getenv(envName.c_str());

	assert(name != NULL); //line added by Ajith John on 9 June 2013

        string filename(name);
        filename += "/new_rules.txt";
        FILE *fp = fopen((char*) filename.c_str(), "r");
        if (fp == NULL)
        {
            cerr << "ERROR : " << filename << " Doesnot Exists " << endl;
            exit(-1);
        }
        extern FILE *rules_parserin;
        extern int rules_parserparse();
        rules_parserin = fp;
        rules_parserparse();
        /***
        extern int inferencerule_parserparse();

        logManager->LOG("Reading the operatorSemantics file \n", m_logFile, c_DebugLevelVerbosity);
        extern FILE *inferencerule_parserin;
        FILE *fp_inf_rules = fopen("operatorSemantics", "r");
        if (fp_inf_rules == NULL)
        {
            cerr << "ERROR:  OperatorSemantics File Doesnot exists" << endl;
            exit(-1);
        }
        inferencerule_parserin = fp_inf_rules;
        inferencerule_parserparse();

        logManager->LOG("Finished Reading the operatorSemantics file \n", m_logFile, c_DebugLevelVerbosity);
       // ofstream *inf_rules_file = new ofstream();
       // inf_rules_file->open("InferenceRules.TXT");
        //this->printMapOfTypeInferenceRules(inf_rules_file);
        //inf_rules_file->close(); /**/

        // Here Expression manager need to configure the dag simplifier to create the default expressions
        // Such as 0b0, 0b1 etc.
        string zero = "0b0";
        string one = "0b1";
        t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
        TypeOfExpressionTuple typeInfoForBVConstants0and1 = {TYPE_UNSIGNED_BITVECTOR, 1};
        m_dummyExpression->createExpression(zero, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        m_dummyExpression->createExpression(one, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        string trueStr = "true";
        string falseStr = "false";
        typeInfoForBVConstants0and1.m_type = TYPE_BOOL;
        typeInfoForBVConstants0and1.m_width = 0;
        m_dummyExpression->createExpression(trueStr, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        m_dummyExpression->createExpression(falseStr, typeInfoForBVConstants0and1, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }

    m_operatorLabelADD = "add";
    m_operatorLabelSUB = "sub";
    m_operatorLabelMultiply = "multiply";
    m_operatorLabelDivide = "divide";
    m_operatorLabelModulus = "mod";

    m_operatorLabelRedOR = "redor";
    m_operatorLabelRedAND = "redand";
    m_operatorLabelRedXOR = "redxor";
    m_operatorLabelRedNOR = "rednor";
    m_operatorLabelRedNAND = "rednand";
    m_operatorLabelRedXNOR = "redxnor";

    m_operatorLabelBitwiseNEG = "bitneg";
    m_operatorLabelBitwiseAND = "bitand";
    m_operatorLabelBitwiseOR = "bitor";
    m_operatorLabelBitwiseXOR = "bitxor";
    m_operatorLabelBitwiseXNOR = "bitxnor";
    m_operatorLabelBitwiseNOR = "bitnor";
    m_operatorLabelBitwiseNAND = "bitnand";


    m_operatorLabelLogNOT = "lognot";
    m_operatorLabelLogAND = "logand";
    m_operatorLabelLogOR = "logor";
    m_operatorLabelLogicalEquality = "logeq";
    m_operatorLabelLogicalInEquality = "diseq";
    m_operatorLabelLogicalWildEquality = "wildeq";
    m_operatorLabelCaseXequlity = "casexeq";
    m_operatorLabelCaseZequlity = "casezeq";
    m_operatorLabelLogicalWildInEquality = "wilddiseq";
    m_operatorLabelLessThan = "lt";
    m_operatorLabelLessThanOrEqual = "le";
    m_operatorLabelGreaterThan = "gt";
    m_operatorLabelGreaterThanOrEqual = "ge";

    m_operatorLabelIte = "ite";
    m_operatorLabelConcat = "concat";
    m_operatorLabelZeroextension = "zeroextn";
    m_operatorLabelSignextension = "signextn";
    m_operatorLabelSelect = "select";

    m_operatorLabelLeftShift = "leftshift";
    m_operatorLabelRightShift = "rightshift";
    m_operatorLabelLefttoRightStream = "leftToRightStream";
    m_operatorLabelRighttoLeftStream = "rightToLeftStream";

    m_typeInfoOfInvalidBitExpr.m_type = TYPE_UNSIGNED_BITVECTOR;
    m_typeInfoOfInvalidBitExpr.m_width = 1;

    m_operatorLabelType = OPERATOR;


    this->m_evaluator = new t_ExpressionEvaluator();
    m_dummyExpression = new t_Expression();
    m_dummyExpression->m_logFile = new ofstream();
    const string sExpressionLogFileName = 
	t_ConfigurationOptions::getConfig()->m_sLogDir + "/Expression.log";
    m_dummyExpression->m_logFile->open(sExpressionLogFileName.c_str());

    m_useEncodingOfExpressions = true;

    if (singleton == NULL)
        singleton = this;
    else
        cout << "-------------- WARNING: Multiple Expression Managers Created. ----------------" << endl;
}

t_Expression* t_ExpressionManager::extendExpr(t_Expression* expr, int newWidth)
{
    TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(expr);
//    if(typeInfo.m_type == TYPE_MEMORY_ARRAY)
//        throw std::logic_error("Attempt to extend memory");
    
    if(getWidth(expr) >= newWidth)
        return expr;
    t_Expression* widthExpr = createIntegerConstant(newWidth);
    
    
    string label = "zeroextn";
    vector<t_Expression*> operands = t_ExpressionManager::buildVector(expr, widthExpr);
    typeInfo.m_width = newWidth;
    return createExpression(label, operands, typeInfo);
}
t_Expression* t_ExpressionManager::createIntegerConstant(int value)
{
    string label = integerToBinaryString(value);
    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, label.length()};
    return createConstant(label, typeInfo);
}

t_Expression* t_ExpressionManager::createIntegerConstant(int value, int width)
{
    string label = integerToBinaryString(value);
    assert( label.length()<= width);
    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, width};
    if(label.length()<width)
        label = string(width-label.length(), '0') + label;
    assert(label.length() == width);
    return createConstant(label, typeInfo);
}

t_Expression *
t_ExpressionManager::createConstant(string &label, TypeOfExpressionTuple &typeTuple)
{
    t_Logger* logManager = t_Logger::getInstance();
    t_TypeOfValueStoredInExpressionLabel labelType = CONSTANT;
    string new_label_with_type;
    if (typeTuple.m_type == TYPE_SIGNED_BITVECTOR || typeTuple.m_type == TYPE_UNSIGNED_BITVECTOR ||
            typeTuple.m_type == TYPE_TWOBIT_BITVECTOR)
    {
        if(!startsWith(label, "0b"))
            new_label_with_type = "0b" + label;
        else
            new_label_with_type = label;
    }
    else
    {
        new_label_with_type = label;
    }
    t_Expression *expr =
            m_dummyExpression->createExpression(new_label_with_type, typeTuple, labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);

    logManager->LOG(" Expression:" + label + "labelType: CONSTANT" + " is Created", m_logFile, c_DebugLevelVerbosity);
    return expr;
}

/**
 *Expression - createSymbol
 */
t_Expression *
t_ExpressionManager::createSymbol(string &label, TypeOfExpressionTuple &typeTuple)
{
    t_Logger* logManager = t_Logger::getInstance();
    t_TypeOfValueStoredInExpressionLabel labelType = SYMBOL;
    t_Expression *expr = m_dummyExpression->createExpression(label, typeTuple,
            labelType, m_expressionImplementationType, m_expressionImplementationManagerInstance);

    logManager->LOG(" Expression: " + label + " labelType: SYMBOL is Created", m_logFile, c_DebugLevelVerbosity);
    return expr;
}

/**
 *Expression - createExpression
 */
t_Expression *
t_ExpressionManager::createExpression(string &label, vector<t_Expression *> &operands)
{
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    t_Logger* logManager = t_Logger::getInstance();
    logManager->LOG("Calling infer", m_logFile, c_DebugLevelVerbosity);
    //TypeOfExpressionTuple *typeTuple = inferTypeOfExpression(label, operands);
    
//    if (typeTuple == NULL)
//    {
//        logManager->LOG("ERROR ! Unable to Infer type Of Expression ( Expression is not created )", m_logFile, c_NoMessageLevelVerbosity);
//        cerr << "Expression Manager : Not  able to infer the type of Expression" << endl;
//        cerr << "For more details, please see " << t_ConfigurationOptions::getConfig()->m_sLogDir << "/ExpressionManager.log" << endl;
//        return NULL;
//    }
    TypeOfExpressionTuple typeInfo;
    
    t_Expression* expr = m_dummyExpression->checkIfExpressionExists(label, operands, m_expressionImplementationType, m_expressionImplementationManagerInstance);
    if(expr!=NULL)
    {
        typeInfo = getTypeOfExpressionTuple(expr);
        //This is not a bug, We need to call this method. This is because, the optimizations on read, update, select and concat
        // might not have been applied on the expression that we get out of checkIfExpressionExists method.
        //This does add a bit to the overhead.
        return m_dummyExpression->createExpression(label, labelType, typeInfo,
            operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    }
    if(label == "ite")
    {
        typeInfo = getTypeOfExpressionTuple(operands.at(1));
    }
    else if(label == "zeroextn" || label == "signextn")
    {
        assert(getTypeOfValueStoredInExpressionLabel(operands.at(1)) == CONSTANT);
        int wd = getConstantValuePresentInExpression(operands.at(1));
        typeInfo.m_type = getTypeOfExpressionWithOutWidth(operands.at(0));
        typeInfo.m_width = wd;
    }
    else if(label == "select")
    {
        assert(getTypeOfValueStoredInExpressionLabel(operands.at(1)) == CONSTANT);
        assert(getTypeOfValueStoredInExpressionLabel(operands.at(2)) == CONSTANT);
        
        int lb = getConstantValuePresentInExpression(operands.at(1));
        int ub = getConstantValuePresentInExpression(operands.at(2));
        
        typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
        typeInfo.m_width = ub-lb+1;
        assert(typeInfo.m_width>0);
    }
    else if(getTypeOfOperator(label) == LOGICAL || getTypeOfOperator(label) == RELATIONAL ||
            label.find("red") == 0) //reduction operator
    {
        typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
        typeInfo.m_width = 1;
    }
    else if(label == "concat")
    {
        typeInfo.m_type = TYPE_UNSIGNED_BITVECTOR;
        int wd = 0;
        for(vector<t_Expression*>::iterator it = operands.begin(); it!= operands.end(); ++it)
        {
            wd += getWidth(*it);
        }
        typeInfo.m_width = wd;
    }
    else if(getTypeOfOperator(label) == BITWISE )
    {
        typeInfo = getTypeOfExpressionTuple(operands[0]);
    }
    else
    {
        throw std::logic_error("Cannot infer the type of expression. This may be due to some simplification rule.");
        //typeInfo = getTypeOfExpressionTuple(operands.at(0));
    }
    
    *m_logFile <<"Attempting to infer the type of expression for "<<label<<" operator:"<<((int)typeInfo.m_type)<<" width:"<<typeInfo.m_width<<endl;

//    typeInfo.m_type = typeTuple->m_type;
//    typeInfo.m_width = typeTuple->m_width;
    expr = m_dummyExpression->createExpression(label, labelType, typeInfo,
            operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    return expr;
}

/**
 *Expression - createExpression without type inferencing, Needs the typeTuple already evaluated and must be passed.
 */
t_Expression *
t_ExpressionManager::createExpression(string &label, vector<t_Expression *> &operands, TypeOfExpressionTuple& typeTuple)
{
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    t_Logger* logManager = t_Logger::getInstance();

    TypeOfExpressionTuple typeInfo;
    typeInfo.m_type = typeTuple.m_type;
    typeInfo.m_width = typeTuple.m_width;
    t_Expression *expr = m_dummyExpression->createExpression(label, labelType, typeInfo,
            operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    if (expr != NULL)
    {
        logManager->LOG(" Expression:" + label + " labelType: OPERATOR Type:" + toString(typeInfo.m_type) + " width:" + toString(typeInfo.m_type) + " is Created", m_logFile, c_DebugLevelVerbosity);
    }
    return expr;

}

/**
 * removeExpression - delete expression
 */
bool t_ExpressionManager::removeExpression(t_Expression *expr)
{
    return true;
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        //delete expr;
        //XXX: do not delete exprs.
        logManager->LOG("Expression is deleted", m_logFile, c_DebugLevelVerbosity);
        return true;
    }
    else
    {
        logManager->LOG("Expression trying to deleted is null", m_logFile, c_RunLevelVerbosity);
        return false;
    }
}

/**
 * checkStructuralEquality - Based on Implementation structure
 */
bool t_ExpressionManager::checkStructuralEquality(t_Expression *e1, t_Expression *e2)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (e1 == NULL || e2 == NULL)
    {
        logManager->LOG("Unable to check structural equality", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    return e1->checkStructuralEquality(e2, m_expressionImplementationType, m_expressionImplementationManagerInstance);
}

/**
 * getSizeOfExpression - get Number of nodes in a particular expression
 */
int t_ExpressionManager::getSizeOfExpression(t_Expression *expr)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL)
    {
        logManager->LOG("Expression Object is NULL", m_logFile, c_NoMessageLevelVerbosity);
        return 0;
    }
    return expr->getSizeOfExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
}

/**
 * Expression Manger destructor
 */
t_ExpressionManager::~t_ExpressionManager()
{
    t_Logger* logManager = t_Logger::getInstance();
    logManager->LOG("Expression Manager is deleted", m_logFile, c_DebugLevelVerbosity);
    m_logFile->close();
    m_dummyExpression->m_logFile->close();
}

/**
 * printExpressionAsYicesExpression - Printing the particular expression into a Constraint format
 */
bool t_ExpressionManager::printExpressionAsYicesExpression(t_Expression *expr, string &expression_symbol_to_print, ofstream *fout)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL)
    {
        logManager->LOG("Expression Object is NULL", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    else
    {
        return expr->printExpressionAsYicesExpression(expression_symbol_to_print, fout, m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }
}

/**
 * printExpressionToFile
 */
bool t_ExpressionManager::printExpressionToFile(t_Expression *expr, ofstream *outfile)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL || outfile == NULL)
    {
        logManager->LOG("Expression or file are NULL", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    else
    {
        return expr->printExpressionToFile(outfile, m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }
}

/**
 * Infer type of Expression based on operator and Types of operands
 */
TypeOfExpressionTuple *t_ExpressionManager::inferTypeOfExpression(string operatorSymbol, vector <t_Expression *> &operands)
{
    t_Logger* logManager = t_Logger::getInstance();
    logManager->LOG(" Inferring Type Of Expression:" + operatorSymbol, m_logFile, c_DebugLevelVerbosity);
    logManager->LOG("Operand Types:", m_logFile, c_DebugLevelVerbosity);
    vector <TypeOfExpressionTuple *> operandTypes;
    for (int i = 0; i < operands.size(); i++)
    {
        t_Expression *exprT = operands[i];
        if (exprT == NULL)
        {
            logManager->LOG(" One of the Operands is NULL", m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        TypeOfExpressionTuple *t = exprT->getTypeOfExpression(
                m_expressionImplementationType,
                m_expressionImplementationManagerInstance);

        logManager->LOG("Operand " + toString(i) + ": Type=" + toString(t->m_type) + " Width=" + toString(t->m_width) + ":" + operands[i]->getLabelOfExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance), m_logFile, c_DebugLevelVerbosity);
        operandTypes.push_back(t);
    }

    if (operatorSymbol != "")
    {
        // *logFile << "Entering Inference";
        t_InferTypeOfExpression *typeInferred = new t_InferTypeOfExpression();

        string signature = typeInferred->generateKey(operatorSymbol, operandTypes);
        logManager->LOG("Signature:" + signature, m_logFile, c_DebugLevelVerbosity);
        map <string, t_InferTypeOfExpression *> ::iterator it;
        it = mapOfTypeInferenceRules.find(signature);

        if (it != mapOfTypeInferenceRules.end())
        {
            typeInferred = (*it).second;

            if (typeInferred->type_cast_operands != "")
            {
                TypeOfExpressionTuple *inferResult =
                        inferTypeBasedOnCastingOperands(
                        typeInferred->type_cast_operands,
                        operatorSymbol,
                        operands
                        );
                if (inferResult == NULL)
                    return NULL;
                else
                    return inferResult;
            }
            TypeOfExpressionTuple *type = typeInferred->evaluateTypeBasedOnInferencing(operandTypes);

            /**
             * This is Hard Coded for SELECT operation
             */
            if (operatorSymbol == "select" && operands.size() == 3)
            {
                int op2Val = stringToInteger(operands[1]->getLabelOfExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance));
                int op3Val = stringToInteger(operands[2]->getLabelOfExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance));
                type->m_width = op3Val - op2Val + 1;
                //    logManager->LOG(" select width:"<<op2Val<<":"<<op3Val<<" ="<<type->width,logFile,c_InformationMessage);
            }
            else if (operatorSymbol == "zeroextn" && operands.size() == 2)
            {
                string str = operands[1]->getLabelOfExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
                int op2Val = binaryStringToInteger(str.substr(2, str.length()));
                type->m_width = op2Val;
            }
            logManager->LOG(" Type:" + toString(type->m_type) + " Width:" + toString(type->m_width), m_logFile, c_DebugLevelVerbosity);
            return type;
        }
        else
        {
            logManager->LOG("Check for the Infinite Operand types:.", m_logFile, c_DebugLevelVerbosity);
            //Checking types of operands are same
            vector<TypeOfExpressionTuple *>::iterator itrOperands = operandTypes.begin();
            while (itrOperands != operandTypes.end())
            {
                logManager->LOG(".", m_logFile, c_DebugLevelVerbosity);
                TypeOfExpressionTuple *type1 = (*itrOperands);
                logManager->LOG("Type:" + toString(type1->m_type) + ":" + toString(operandTypes[0]->m_type), m_logFile, c_DebugLevelVerbosity);
                if (type1->m_type != operandTypes[0]->m_type)
                {
                    logManager->LOG(" Types of Operands are not same Or Invalid typeInferencing", m_logFile, c_NoMessageLevelVerbosity);
                    return NULL;
                }
                itrOperands++;
            }
            //Put first operand type in the key
            signature = operatorSymbol + "@" + toString(operandTypes[0]->m_type);
            logManager->LOG("New signature:" + signature, m_logFile, c_DebugLevelVerbosity);

            it = mapOfTypeInferenceRules.find(signature);
            if (it != mapOfTypeInferenceRules.end())
            {
                logManager->LOG(operatorSymbol + " Got the inferred type: but with one operand type", m_logFile, c_DebugLevelVerbosity);
                typeInferred = (*it).second;
                if (typeInferred->unlimitedNumberOfArguments)
                {
                    TypeOfExpressionTuple *type = typeInferred->evaluateTypeBasedOnInferencing(operandTypes);
                    logManager->LOG("Type :" + toString(type->m_type) + " width:" + toString(type->m_width), m_logFile, c_DebugLevelVerbosity);
                    return type;
                }
                else
                {
                    logManager->LOG("Operator:" + operatorSymbol + " Doesnot have infinte operands", m_logFile, c_DebugLevelVerbosity);
                }
            }
            else
            {
                logManager->LOG("\nUnknown Operator Symbol:" + operatorSymbol, m_logFile, c_NoMessageLevelVerbosity);
            }
        }
    }
    return NULL;
}

TypeOfExpressionTuple *t_ExpressionManager::inferTypeBasedOnCastingOperands
(
        string &operands_numbers_to_cast,
        string &operatorSymbol,
        vector <t_Expression *> &operands
        )
{
    t_Logger* logManager = t_Logger::getInstance();
    logManager->LOG("------------------------------------------------\nType Casting Needs to be done over Operands", m_logFile, c_DebugLevelVerbosity);
    logManager->LOG(" Casting:" + operands_numbers_to_cast, m_logFile, c_DebugLevelVerbosity);
    string type_caste_string = operands_numbers_to_cast;
    vector<string> operands_need_to_cast = split(type_caste_string, ",");
    vector<int> operand_numbers;
    int i = 0;
    while (i < operands_need_to_cast.size())
    {
        logManager->LOG("i=" + toString(i) + ":" + operands_need_to_cast[i], m_logFile, c_DebugLevelVerbosity);
        operand_numbers.push_back(stringToInteger(operands_need_to_cast[i]));
        i++;
    }
    vector<t_Expression *> new_operands_after_cast;
    for (i = 0; i < operands.size(); i++)
    {
        bool present = false;
        int j = 0;
        while (j < operand_numbers.size())
        {
            if ((operand_numbers[j] - 1) == i)
            {
                operand_numbers.erase(operand_numbers.begin() + j);
                present = true;
                break;
            }
            j++;
        }
        if (present)
        {
            t_Expression *old_expr = operands[i];
            TypeOfExpressionTuple *typeTuple =
                    old_expr->getTypeOfExpression(m_expressionImplementationType,
                    m_expressionImplementationManagerInstance);

            if (typeTuple->m_type == TYPE_UNSIGNED_BITVECTOR && typeTuple->m_width == 1)
            {
                logManager->LOG(" Converting Unsigned Bitvector Operand to Bool", m_logFile, c_DebugLevelVerbosity);
                TypeOfExpressionTuple typeTuple1bv = {TYPE_UNSIGNED_BITVECTOR, 1};
                string constValueOne = "1";
                t_Expression *expr_1bv = this->createConstant(constValueOne, typeTuple1bv);
                vector<t_Expression *> v_curr_expr_opernads;
                v_curr_expr_opernads.push_back(old_expr);
                v_curr_expr_opernads.push_back(expr_1bv);
                string logeqLabel = "logeq";
                t_Expression *new_expr = this->createExpression(logeqLabel, v_curr_expr_opernads);
                new_operands_after_cast.push_back(new_expr);
            }
            else if (typeTuple->m_type == TYPE_SIGNED_BITVECTOR && typeTuple->m_width == 1)
            {
                logManager->LOG(" Converting Signed Bitvector Operand to Bool", m_logFile, c_DebugLevelVerbosity);
                TypeOfExpressionTuple typeTuple1bv = {TYPE_SIGNED_BITVECTOR, 1};
                string constValueOne = "1";
                t_Expression *expr_1bv = this->createConstant(constValueOne, typeTuple1bv);
                vector<t_Expression *> v_curr_expr_opernads;
                v_curr_expr_opernads.push_back(old_expr);
                v_curr_expr_opernads.push_back(expr_1bv);
                string logeqLabel = "logeq";
                t_Expression *new_expr = this->createExpression(logeqLabel, v_curr_expr_opernads);
                new_operands_after_cast.push_back(new_expr);
            }
            else if (typeTuple->m_type == TYPE_BOOL)
            {
                logManager->LOG(" Converting Boolean Operand to Unsigned Bitvector", m_logFile, c_DebugLevelVerbosity);
                string constValueOne = "1";
                string constValueZero = "0";
                TypeOfExpressionTuple typeTuplebv1 = {TYPE_UNSIGNED_BITVECTOR, 1};
                t_Expression *exprBv1 = this->createConstant(constValueOne, typeTuplebv1);
                t_Expression *exprBv0 = this->createConstant(constValueZero, typeTuplebv1);

                vector<t_Expression *> v_new_operandi_ops;
                v_new_operandi_ops.push_back(operands[i]);
                v_new_operandi_ops.push_back(exprBv1);
                v_new_operandi_ops.push_back(exprBv0);
                string iteLabel = "ite";
                t_Expression *new_expr = this->createExpression(iteLabel, v_new_operandi_ops);
                new_operands_after_cast.push_back(new_expr);
            }
            else
            {
                //new_operands_after_cast.push_back(operands[i]);
                if (typeTuple->m_type == TYPE_UNSIGNED_BITVECTOR && ((typeTuple->m_width) > 1))
                {

                    TypeOfExpressionTuple type_tuple_int = {TYPE_SIGNED_INTEGER, 0};
                    string constValueZero = "0";
                    t_Expression *expr_int_0 = this->createConstant(constValueZero, type_tuple_int);
                    vector<t_Expression *> operands_select;
                    operands_select.push_back(old_expr);
                    operands_select.push_back(expr_int_0);
                    operands_select.push_back(expr_int_0);
                    string selectLabel = "select";
                    t_Expression *new_expr = this->createExpression(selectLabel, operands_select);
                    new_operands_after_cast.push_back(new_expr);
                }
                else
                {
                    logManager->LOG(" ExpressionManager :(Type Inferencing) Not able to Type Cast ", m_logFile, c_NoMessageLevelVerbosity);
                    return NULL;
                }
            }
        }
        else
        {
            new_operands_after_cast.push_back(operands[i]);
        }
    }
    operands.clear();
    for (int it = 0; it < new_operands_after_cast.size(); it++)
    {
        operands.push_back(new_operands_after_cast[it]);
    }
    TypeOfExpressionTuple *inferredTypeAfterChange = inferTypeOfExpression(operatorSymbol, operands);
    if (inferredTypeAfterChange == NULL)
    {
        return NULL;
    }
    else
    {
        return inferredTypeAfterChange;
    }
}

bool t_ExpressionManager::printExpressionToFileAsTree(string &expression_symbol, t_Expression *expr, ofstream *outfile)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL || outfile == NULL)
    {
        logManager->LOG("Expression or file are NULL", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    else
    {
        return expr->printExpressionToFileAsTree(expression_symbol, m_expressionImplementationType, m_expressionImplementationManagerInstance, outfile);
    }
}

bool t_ExpressionManager::printExpressionToFileAsDAG(const string &expression_symbol, t_Expression *expr, ostream& out, bool printInvBit)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL)
    {
        logManager->LOG("Expression is NULL", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    else
    {
        if(t_ConfigurationOptions::getConfig()->m_printFullExpression )
        {
            expr->clearVisitedFlagsUnderThisExpression();
        }
        bool result_print = expr->printExpressionToFileAsDAG(expression_symbol, m_expressionImplementationType, m_expressionImplementationManagerInstance, out);
        if (printInvBit)
        {
            t_Expression* invalidbit = getInvalidBitExpression(expr);
            if (m_useEncodingOfExpressions == true && invalidbit != NULL)
            {
                if(t_ConfigurationOptions::getConfig()->m_printFullExpression )
                {
                    invalidbit->clearVisitedFlagsUnderThisExpression();
                }
                string invalidbit_str = expression_symbol + "@INVALID_BIT";
                invalidbit->printExpressionToFileAsDAG(invalidbit_str, m_expressionImplementationType, m_expressionImplementationManagerInstance, out);
            }
        }
        return result_print;
    }
}

bool t_ExpressionManager::printExpressionToFileAsDAG(string &expression_symbol, t_Expression *expr, ofstream *outfile)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL || outfile == NULL)
    {
        logManager->LOG("Expression or file are NULL", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
    else
    {
        if(t_ConfigurationOptions::getConfig()->m_printFullExpression )
        {
            expr->clearVisitedFlagsUnderThisExpression();
        }
        bool result_print = expr->printExpressionToFileAsDAG(expression_symbol, m_expressionImplementationType, m_expressionImplementationManagerInstance, outfile);
        t_Expression* invalidbit = getInvalidBitExpression(expr);
        if (m_useEncodingOfExpressions == true && invalidbit != NULL)
        {
            if(t_ConfigurationOptions::getConfig()->m_printFullExpression )
            {
                invalidbit->clearVisitedFlagsUnderThisExpression();
            }
            string invalidbit_str = expression_symbol + "@INVALID_BIT";
            invalidbit->printExpressionToFileAsDAG(invalidbit_str, m_expressionImplementationType, m_expressionImplementationManagerInstance, outfile);
        }
        return result_print;
    }
}

string t_ExpressionManager::getActualExpressionValueAsString(t_Expression *expr)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr == NULL)
    {
        logManager->LOG("Expression trying to read is NULL", m_logFile, c_NoMessageLevelVerbosity);
        return "";
    }
    /**  do{ //used to break out of condition
      if(m_useEncodingOfExpressions)
      {
          t_Expression* invalid_bit_expr = getInvalidBitExpression(expr);
          if(invalid_bit_expr == NULL)
          {
              cout<<"Error in "<<__FILE__<<":"<<__LINE__<<":-"<<expr->getID()<<" did not get an invalid bit expression"<<endl;
              break;
          }
          string iv_value = invalid_bit_expr->getActualExpressionValueAsString(m_expressionImplementationType, m_expressionImplementationManagerInstance);
          if( iv_value == "1")
          {
              int width = getWidth(expr);
              string actual_val(width,'u');
              return actual_val;
          }
      }
      }while(0);*/
    string actual_val = expr->getActualExpressionValueAsString(m_expressionImplementationType, m_expressionImplementationManagerInstance);
    logManager->LOG("Label:" + getLableOfExpression(expr) + "  value:" + actual_val, m_logFile, c_DebugLevelVerbosity);
    return actual_val;
}

pair<bool, t_Expression*> t_ExpressionManager::buildUpdateMap(t_Expression* root, vector<updateData>& vec_updateData, map<string, t_Expression*> expr_map)
{
    int level = 0;
    string label = getLabelOfExpression(root);
    bool modified = false;

    while (label == "update")
    {
        vector<t_Expression*> operands = getVectorOfOperands(root);
        t_Expression* index = ReplaceLeavesByExpressionsFromMap(operands[1], expr_map);
        t_Expression* value = ReplaceLeavesByExpressionsFromMap(operands[2], expr_map);

        if (index != operands[1] || value != operands[2])
            modified = true;

        //is the index already present
        bool found = false;
        for (vector<updateData>::iterator it = vec_updateData.begin(), end = vec_updateData.end();
                it != end; ++it)
        {
            if (it->index == index)
            {
                found = true;
                break;
            }
        }
        if (!found)
            vec_updateData.push_back(updateData(index, value, level, getTypeOfExpressionTuple(root)));
        else
            modified = true;

        ++level;
        root = operands[0];
        label = getLabelOfExpression(root);
    }

    return make_pair(modified, root);
}

//Not currently used. See documentation for details.

t_Expression* t_ExpressionManager::replaceLeavesFromMapForUpdate(t_Expression* expr, map<string, t_Expression*> expr_map)
{
    vector<updateData> vec_updateData;
    pair<bool, t_Expression*> res = buildUpdateMap(expr, vec_updateData, expr_map);

    if (res.first == false)
        return expr;

    //sort from highest to lowest level
    sort(vec_updateData.begin(), vec_updateData.end(), updateData::sortOrder);

    t_Expression* array_expr = res.second;

    for (vector<updateData>::iterator it = vec_updateData.begin(), end = vec_updateData.end();
            it != end; ++it)
    {
        string label = "update";
        vector<t_Expression*> operands = buildVector(array_expr, it->index, it->value);
        t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
        //TypeOfExpressionTuple  typeInfo = getTypeOfExpressionTuple(org_expr);

        array_expr = m_dummyExpression->createExpression(label, labelType, it->type_info,
                operands,
                m_expressionImplementationType,
                m_expressionImplementationManagerInstance,
                m_evaluator, false);
    }

    return array_expr;
}

extern int clockCycle;

t_Expression* t_ExpressionManager::ReplaceLeavesByExpressionsFromMap(t_Expression* org_expr, t_HashTable<string, t_Expression*> &expr_map, t_HashTable<unsigned long int, t_Expression*> &cache)
{

    static bool printReplacements = false;
    static string sReplacementsFileName =
	t_ConfigurationOptions::getConfig()->m_sTempInfoDir + "/ExpressionManager/Replacements";
    static ofstream replacementsFile(sReplacementsFileName.c_str());

    /* Algorithm:-
   vector<t_Expression*> operands of this expr;
   replace operands from map if operand is leaf;
    
   partial evaluate each operand to get new operands;
   return createExpression(label of this, operands) //This also does the simplification**/
//        class Temp{
//        public:
//            bool set;
//            Temp(){set = false;}
//            ~Temp(){if(set){printReplacements = false;replacementsFile<<"#############################"<<endl;} }
//        };
    unsigned long exprID = org_expr->getID();
    //    Temp t;
    //    if(exprID == 1266 | exprID == 1254)
    //    {
    //        t.set = true;
    //        replacementsFile<<"############### Time:"<<clockCycle<<" #######################"<<endl; 
    //        printReplacements = true;
    //    }


     if(!printReplacements) //if printing replacements then do not use cache.
    {
        t_HashTable<unsigned long, t_Expression*>::iterator it = cache.find(exprID);
        if (it != cache.end())
        {
            return it.getValue();
        }
    }
    MAKE_COUNTER(__PRETTY_FUNCTION__)
    if (isALeafExpression(org_expr))
    {

        //Replace the leaf by the expression in map, if any else return org_expr
        string label = getLabelOfExpression(org_expr);
        //Remove the N@ prefix
        //if (label.find(EXPRESSION_LABEL_AT_N_PREFIX) == 0)
        //    label = label.substr(string(EXPRESSION_LABEL_AT_N_PREFIX).length());
        t_HashTable<string, t_Expression*> ::iterator it_map = expr_map.find(label);
        if (it_map != expr_map.end())
        {
            cache[exprID] = it_map.getValue();
            if (printReplacements)
            {
//                replacementsFile << "Replacing DAGID " << exprID << " with\n";
//                t_Expression* e = it_map.getValue();
//                printExpressionToFileAsDAG(label, e, replacementsFile, false);
//                replacementsFile << endl;
                t_HashTable<string, t_Expression*> ::iterator it2_map = expr_map.find(label+EXPRESSION_INVALID_BIT_LABEL);
                //t_Expression* invBit=NULL;
                if(it2_map != expr_map.end())
                {
                    //invBit = it2_map.getValue();
                    replacementsFile<<"Original:"<<label<<" Result:["<<getLabelOfExpression(it_map.getValue())<<" , "<<getLabelOfExpression(it2_map.getValue())<<"]\n";
                }
                else
                {
                    replacementsFile<<"Original:"<<label<<" Result:["<<getLabelOfExpression(it_map.getValue())<<" , <no invalid bit found>]\n";
                }
            }
            return it_map.getValue();
        }
        else
        {
            cache[exprID] = org_expr;
            return org_expr;
        }
    }

    string label = getLabelOfExpression(org_expr);

    //    if(label == "update")
    //    {
    //        return replaceLeavesFromMapForUpdate(org_expr, expr_map);
    //    }

    vector<t_Expression*> operands = getVectorOfOperands(org_expr);
#ifdef READ_UPD_REPL
    if (false && label == "read" && getLabelOfExpression(operands[0]) == "update") //read( update(...), index)
    {
        //if the label is read, we will need to replace the update symbol in the update expression
        //with the read at the known index of read.
        bool isIndexValid = true;
        t_Expression* index = operands[1];

        //Check whether the index of read is valid
        {
            t_Expression* invIndex = generateInvalidBitExpression(index);
            //if index is invalid
            if (getTypeOfValueStoredInExpressionLabel(invIndex) == CONSTANT)
            {
                if (getConstantValuePresentInExpression(invIndex) == 1)
                    isIndexValid = false;
            }
        }
        //If valid, build the expression read(array_expr, index of the current read)
        //add a replacement to map that replaces the update symbol (we used in value for update at unknown index)
        //with the newly generated read expression.
        if (isIndexValid)
        {
            pair<string, t_Expression*> symName_arrayExpr = operands[0]->getUpdateSymbolNameAndMemoryInExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
            if (symName_arrayExpr.first != "") //update at unknown index in expression
            {
                t_Expression* readExpr = NULL;
                {//build the read Expression
                    vector<t_Expression*> readOper = buildVector(symName_arrayExpr.second, index);
                    TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
                    string label = "read";
                    readExpr = createExpression(label, readOper, typeInfo);
                }

                expr_map[symName_arrayExpr.first] = readExpr; //add to map and continue with the algorithm.
                cout << "read(update(...),...) found. Replacing " << symName_arrayExpr.first << " with read expression." << endl;
                //The following code will take care of replacing the symbol with the new read expression
            }
        }
    }
#endif
    
    bool operands_modified = false;
    for (int i = 0; i < operands.size(); ++i)
    {
        t_Expression* operand = operands[i];
        if (isALeafExpression(operand))
        {
            string label = getLabelOfExpression(operand);

            //Remove the N@ prefix
            //if (label.find(EXPRESSION_LABEL_AT_N_PREFIX) == 0)
            //    label = label.substr(string(EXPRESSION_LABEL_AT_N_PREFIX).length());

            t_HashTable<string, t_Expression*> ::iterator it_map = expr_map.find(label);
            if (it_map != expr_map.end())
            {
                //t_Expression* replace_by = it_map->second;
                cache[operand->getID()] = it_map.getValue();
                if (printReplacements)
                {
//                    replacementsFile << "Replacing operator DAGID " << operand->getID() << "with\n";
//                    printExpressionToFileAsDAG(label, it_map.getValue(), replacementsFile, false);
//                    replacementsFile << endl;
                    t_HashTable<string, t_Expression*> ::iterator it2_map = expr_map.find(label+EXPRESSION_INVALID_BIT_LABEL);
                    //t_Expression* invBit=NULL;
                    if(it2_map != expr_map.end())
                    {
                        //invBit = it2_map.getValue();
                        replacementsFile<<"Original:"<<label<<" Result:["<<getLabelOfExpression(it_map.getValue())<<" , "<<getLabelOfExpression(it2_map.getValue())<<"]\n";
                    }
                    else
                    {
                        replacementsFile<<"Original:"<<label<<" Result:["<<getLabelOfExpression(it_map.getValue())<<" , <no invalid bit found>]\n";
                    }

                }
		// Changed by Supratik to account for the case that expr_map may not change the value of a leaf operand at all
		// Added on Dec 26, 2013
		if (operand != it_map.getValue()) {
		    // Operand is changed

		    operand = it_map.getValue(); //replace_by;
		    operands_modified = true;
		    operands[i] = operand; //No need to simplify as they will already be simplified.
		}
            }
        }
        else
        {
            operands[i] = ReplaceLeavesByExpressionsFromMap(operand, expr_map, cache);
            if (operand != operands[i]) {
                operands_modified = true;
	    }
        }
    }

    if (operands_modified == false) //no simplification and no replacement done. 
    {
        cache[exprID] = org_expr;
        //Return the same expression itself
        return org_expr;
    }

    //otherwise, build a new expression from the modified operands.

    //string label = getLabelOfExpression(org_expr);
    TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(org_expr);
    //return createExpression(label,operands,typeInfo);
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    /*t_Expression *
t_Expression::createExpression(string &label,
                               t_TypeOfValueStoredInExpressionLabel &labelType,
                               TypeOfExpressionTuple &typeInfo,
                               vector<t_Expression *> &operandExpressions,
                               int Expressionimplementationtype,
                               t_ExpressionImplementationManager *&expressionImplementationManagerInstance,
                               t_ExpressionEvaluator* &evaluator, bool doSelectConcatSimplification)*/
    t_Expression* res = m_dummyExpression->createExpression(label, labelType, typeInfo,
            operands,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance,
            m_evaluator, true);
    cache[exprID] = res;
    if (printReplacements)
    {
//        replacementsFile << "Replacing operator DAGID " << exprID << "with\n";
//        printExpressionToFileAsDAG(label, res, replacementsFile, false);
//        replacementsFile << endl;
        //t_HashTable<string, t_Expression*> ::iterator it2_map = expr_map.find(label+EXPRESSION_INVALID_BIT_LABEL);
        t_Expression* invBitOrgExpr=getInvalidBitExpression(org_expr);
        string inv_label = "<no invalid bit found>";
        if(invBitOrgExpr != NULL)
        {
            printReplacements = false;
            t_Expression* invBit = ReplaceLeavesByExpressionsFromMap(invBitOrgExpr, expr_map, cache);
            printReplacements = true;
            inv_label = getLabelOfExpression(invBit);
        }
        replacementsFile<<"Original:"<<exprID<<" Result:["<<getLabelOfExpression(res)<<" , "<<inv_label<<"]\n";
    }
    return res;
    /*  
  string label;
  vector<t_Expression*> leaves_of_expr = getVectorOfLeafExpressions(org_expr);

  for (vector<t_Expression*>::iterator it = leaves_of_expr.begin(), end = leaves_of_expr.end(); it != end; ++it)
  {
      label = getLabelOfExpression(*it);
      map<string, t_Expression*> ::iterator it_map = expr_map.begin();
      it_map = expr_map.find(label);
      if (it_map != expr_map.end())
      {
          t_Expression* replace_by = it_map->second;
          replaceLeafOfAExpressionByExpression(*it,replace_by);
            
            
      }

  }
    //test code
  if(1)
  {
      ofstream outfile("testfile");
      string lab = getLabelOfExpression(org_expr);
      printExpressionToFileAsDAG(lab,org_expr,&outfile);
  }
     **/
}

t_Expression* t_ExpressionManager::ReplaceLeavesByExpressionsFromMap(t_Expression* org_expr, map<string, t_Expression*> &expr_map)
{
    /* Algorithm:-
   vector<t_Expression*> operands of this expr;
   replace operands from map if operand is leaf;
    
   partial evaluate each operand to get new operands;
   return createExpression(label of this, operands) //This also does the simplification**/

    if (isALeafExpression(org_expr))
    {

        //Replace the leaf by the expression in map, if any else return org_expr
        string label = getLabelOfExpression(org_expr);
        //Remove the N@ prefix
        if (label.find(EXPRESSION_LABEL_AT_N_PREFIX) == 0)
            label = label.substr(string(EXPRESSION_LABEL_AT_N_PREFIX).length());
        map<string, t_Expression*> ::iterator it_map = expr_map.find(label);
        if (it_map != expr_map.end())
            return it_map->second;
        else
            return org_expr;
    }

    string label = getLabelOfExpression(org_expr);

    //    if(label == "update")
    //    {
    //        return replaceLeavesFromMapForUpdate(org_expr, expr_map);
    //    }

    vector<t_Expression*> operands = getVectorOfOperands(org_expr);

    if (false && label == "read" && getLabelOfExpression(operands[0]) == "update") //read( update(...), index)
    {
        //if the label is read, we will need to replace the update symbol in the update expression
        //with the read at the known index of read.
        bool isIndexValid = true;
        t_Expression* index = operands[1];

        //Check whether the index of read is valid
        {
            t_Expression* invIndex = generateInvalidBitExpression(index);
            //if index is invalid
            if (getTypeOfValueStoredInExpressionLabel(invIndex) == CONSTANT)
            {
                if (getConstantValuePresentInExpression(invIndex) == 1)
                    isIndexValid = false;
            }
        }
        //If valid, build the expression read(array_expr, index of the current read)
        //add a replacement to map that replaces the update symbol (we used in value for update at unknown index)
        //with the newly generated read expression.
        if (isIndexValid)
        {
            pair<string, t_Expression*> symName_arrayExpr = operands[0]->getUpdateSymbolNameAndMemoryInExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
            if (symName_arrayExpr.first != "") //update at unknown index in expression
            {
                t_Expression* readExpr = NULL;
                {//build the read Expression
                    vector<t_Expression*> readOper = buildVector(symName_arrayExpr.second, index);
                    TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
                    string label = "read";
                    readExpr = createExpression(label, readOper, typeInfo);
                }

                expr_map[symName_arrayExpr.first] = readExpr; //add to map and continue with the algorithm.
                cout << "read(update(...),...) found. Replacing " << symName_arrayExpr.first << " with read expression." << endl;
                //The following code will take care of replacing the symbol with the new read expression
            }
        }
    }

    bool operands_modified = false;
    for (int i = 0; i < operands.size(); ++i)
    {
        t_Expression* operand = operands[i];
        if (isALeafExpression(operand))
        {
            string label = getLabelOfExpression(operand);

            //Remove the N@ prefix
            if (label.find(EXPRESSION_LABEL_AT_N_PREFIX) == 0)
                label = label.substr(string(EXPRESSION_LABEL_AT_N_PREFIX).length());

            map<string, t_Expression*> ::iterator it_map = expr_map.find(label);
            if (it_map != expr_map.end())
            {
                //t_Expression* replace_by = it_map->second;
                operand = it_map->second; //replace_by;
                operands[i] = operand; //No need to simplify as they will already be simplified.
                operands_modified = true;
            }
        }
        else
        {
            operands[i] = ReplaceLeavesByExpressionsFromMap(operand, expr_map);
            if (operand != operands[i])
                operands_modified = true;
        }
    }

    if (operands_modified == false)//no simplification and no replacement done. 
        //Return the same expression itself
        return org_expr;

    //otherwise, build a new expression from the modified operands.

    //string label = getLabelOfExpression(org_expr);
    TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(org_expr);
    //return createExpression(label,operands,typeInfo);
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    /*t_Expression *
t_Expression::createExpression(string &label,
                               t_TypeOfValueStoredInExpressionLabel &labelType,
                               TypeOfExpressionTuple &typeInfo,
                               vector<t_Expression *> &operandExpressions,
                               int Expressionimplementationtype,
                               t_ExpressionImplementationManager *&expressionImplementationManagerInstance,
                               t_ExpressionEvaluator* &evaluator, bool doSelectConcatSimplification)*/
    return m_dummyExpression->createExpression(label, labelType, typeInfo,
            operands,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance,
            m_evaluator, true);
    /*  
  string label;
  vector<t_Expression*> leaves_of_expr = getVectorOfLeafExpressions(org_expr);

  for (vector<t_Expression*>::iterator it = leaves_of_expr.begin(), end = leaves_of_expr.end(); it != end; ++it)
  {
      label = getLabelOfExpression(*it);
      map<string, t_Expression*> ::iterator it_map = expr_map.begin();
      it_map = expr_map.find(label);
      if (it_map != expr_map.end())
      {
          t_Expression* replace_by = it_map->second;
          replaceLeafOfAExpressionByExpression(*it,replace_by);
            
            
      }

  }
    //test code
  if(1)
  {
      ofstream outfile("testfile");
      string lab = getLabelOfExpression(org_expr);
      printExpressionToFileAsDAG(lab,org_expr,&outfile);
  }
     **/
}

#ifdef QE // Added by Ajith John on 6 Jan 2014
t_Expression* t_ExpressionManager::ReplaceLeafByExpression(t_Expression* org_expr, string &leaf_label_to_replace, t_Expression* expr_to_replace, t_HashTable<unsigned long, t_Expression*> &cache, bool use_second_level_cache_in_compose, unsigned long long int &first_level_cache_hits, unsigned long long int &first_level_cache_misses, unsigned long long int &second_level_cache_hits, unsigned long long int &second_level_cache_misses, unsigned long long int &leaf_cases, unsigned long long int &node_cases, unsigned long long int &no_recreation_cases, unsigned long long int &recreation_cases)
{
    assert(org_expr != NULL);
    if(!use_second_level_cache_in_compose)
    {
	    unsigned long exprID = org_expr->getID();
	    t_HashTable<unsigned long, t_Expression*>::iterator it = cache.find(exprID);
	    if (it != cache.end())
	    {
		first_level_cache_hits++;

	    	return it.getValue();
	    }
		
	    first_level_cache_misses++;
	    second_level_cache_misses++;

	    if (isALeafExpression(org_expr))
	    {
		leaf_cases++;		

		//Replace leaf with label as leaf_label_to_replace by expr_to_replace, and return org_expr for other leaves
		string label = getLabelOfExpression(org_expr);
		if (label == leaf_label_to_replace)
		{
		    cache[exprID] = expr_to_replace;
		    return expr_to_replace;
		}
		else
		{
		    cache[exprID] = org_expr;
		    return org_expr;
		}
	    }// if org_expr is leaf ends here
	    else
	    {
		node_cases++;
		
		vector<t_Expression*> operands = getVectorOfOperands(org_expr);
		bool operands_modified = false;
		for (int i = 0; i < operands.size(); ++i)
		{
			t_Expression* operand = operands[i];
			operands[i] = ReplaceLeafByExpression(operand, leaf_label_to_replace, expr_to_replace, cache, use_second_level_cache_in_compose, first_level_cache_hits, first_level_cache_misses, second_level_cache_hits, second_level_cache_misses, leaf_cases, node_cases, no_recreation_cases, recreation_cases);
		    	if (operand != operands[i]) 
			{
		        	operands_modified = true;
		    	}
		}
		if (operands_modified == false) //no replacement done. 
	    	{
			no_recreation_cases++;

			//Return the same expression itself
			cache[exprID] = org_expr;        	
			return org_expr;
	    	}
		else
		{
			recreation_cases++;
			
			//otherwise, build a new expression from the modified operands.
			string label = getLabelOfExpression(org_expr);
	    		TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(org_expr);
	   		t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
	    		t_Expression* changed_expr = m_dummyExpression->createExpression(label, labelType, typeInfo,
		         operands,
		         m_expressionImplementationType,
		         m_expressionImplementationManagerInstance,
		         m_evaluator, true);

			cache[exprID] = changed_expr;
	   		return changed_expr;
		}
	     }// if org_expr is non-leaf ends here
    }// if(!use_second_level_cache_in_compose) ends here
    else // use second level cache - second level cache catches everything in first
    // level cache. Hence, when the second level cache is on, we can disable the first level cache
    {
	    first_level_cache_misses++;

            // check in second-level cache
	    static t_HashTable<string, t_Expression*> second_level_cache;
            // form the hash-key
	    string key = toString(org_expr);
            key += leaf_label_to_replace;
            key += toString(expr_to_replace);

	    t_HashTable<string, t_Expression*>::iterator second_it = second_level_cache.find(key);
	    if (second_it != second_level_cache.end())
	    {
		second_level_cache_hits++;

	    	return second_it.getValue();
	    }

	    second_level_cache_misses++;

	    if (isALeafExpression(org_expr))
	    {
		leaf_cases++;	

		//Replace leaf with label as leaf_label_to_replace by expr_to_replace, and return org_expr for other leaves
		string label = getLabelOfExpression(org_expr);
		if (label == leaf_label_to_replace)
		{
		    second_level_cache[key] = expr_to_replace;
		    return expr_to_replace;
		}
		else
		{
		    second_level_cache[key] = org_expr;
		    return org_expr;
		}
	    }// if org_expr is leaf ends here
	    else
	    {
		node_cases++;

		vector<t_Expression*> operands = getVectorOfOperands(org_expr);
		bool operands_modified = false;
		for (int i = 0; i < operands.size(); ++i)
		{
			t_Expression* operand = operands[i];
			operands[i] = ReplaceLeafByExpression(operand, leaf_label_to_replace, expr_to_replace, cache, use_second_level_cache_in_compose, first_level_cache_hits, first_level_cache_misses, second_level_cache_hits, second_level_cache_misses, leaf_cases, node_cases, no_recreation_cases, recreation_cases);
		    	if (operand != operands[i]) 
			{
		        	operands_modified = true;
		    	}
		}
		if (operands_modified == false) //no replacement done. 
	    	{
			no_recreation_cases++;

			//Return the same expression itself
			second_level_cache[key] = org_expr;        	
			return org_expr;
	    	}
		else
		{
			recreation_cases++;
		
			//otherwise, build a new expression from the modified operands.
			string label = getLabelOfExpression(org_expr);
	    		TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(org_expr);
	   		t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
	    		t_Expression* changed_expr = m_dummyExpression->createExpression(label, labelType, typeInfo,
		         operands,
		         m_expressionImplementationType,
		         m_expressionImplementationManagerInstance,
		         m_evaluator, true);

			second_level_cache[key] = changed_expr;
	   		return changed_expr;
		}
	     }// if org_expr is non-leaf ends here	
    } //if(use_second_level_cache_in_compose) ends here
   
}
#endif // Added by Ajith John on 6 Jan 2014 ends here

t_ExpressionValue * t_ExpressionManager::getValue(t_Expression *expr)
{
    return expr->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
}

bool t_ExpressionManager::setValue(t_Expression *expr, signed int siVal)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {

        t_ExpressionValue *val = expr->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
        if (val->m_typeInfo.m_type == TYPE_SIGNED_INTEGER)
        {
            val->m_actualExpressionValueStored.exprvalueSignedInt = siVal;
            return expr->setValue(val, true,
                    m_expressionImplementationType,
                    m_expressionImplementationManagerInstance,
                    m_evaluator);
        }
        else
        {
            logManager->LOG("Unable to set expression value TYPE-CONFLICT !", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool t_ExpressionManager::setValue(t_Expression *expr, unsigned int usiVal)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        t_ExpressionValue *val = expr->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
        if (val->m_typeInfo.m_type == TYPE_UNSIGNED_INTEGER)
        {
            val->m_actualExpressionValueStored.exprvalueUnsignedInt = usiVal;
            expr->setValue(val, true,
                    m_expressionImplementationType,
                    m_expressionImplementationManagerInstance,
                    m_evaluator);
        }
        else
        {
            logManager->LOG("Unable to set expression value TYPE-CONFLICT !", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool t_ExpressionManager::setValue(t_Expression *expr, float fval)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        t_ExpressionValue *val = expr->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
        if (val->m_typeInfo.m_type == TYPE_REAL)
        {
            val->m_actualExpressionValueStored.exprvalueFloat = fval;
            expr->setValue(val, true,
                    m_expressionImplementationType,
                    m_expressionImplementationManagerInstance,
                    m_evaluator);
        }
        else
        {
            logManager->LOG("Unable to set expression value TYPE-CONFLICT !", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool t_ExpressionManager::setValue(t_Expression *expr, bvatom bvValue)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        t_ExpressionValue *val = expr->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
        if (val->m_typeInfo.m_type == TYPE_SIGNED_BITVECTOR || val->m_typeInfo.m_type == TYPE_UNSIGNED_BITVECTOR)
        {
            val->m_actualExpressionValueStored.exprvalueBitvector = bvValue;
            expr->setValue(val, true,
                    m_expressionImplementationType,
                    m_expressionImplementationManagerInstance,
                    m_evaluator);
            return true;
        }
        else
        {
            logManager->LOG("Unable to set expression value TYPE-CONFLICT !", m_logFile, c_NoMessageLevelVerbosity);
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool t_ExpressionManager::ResetEvaluator(vector<t_Expression *> &expressions_tobe_evaluated)
{
    t_Logger* logManager = t_Logger::getInstance();
    bool reset_evaluator = m_dummyExpression->clearAllEvaluatorFlagsForExpressions
            (
            expressions_tobe_evaluated,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance
            );
    return reset_evaluator;
}

string t_ExpressionManager::getLableOfExpression(t_Expression *expr)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        return expr->getLabelOfExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }
    else
    {
        logManager->LOG("Unable to get Label of the expression : NULL !", m_logFile, c_NoMessageLevelVerbosity);
        // return NULL; // changed by sukanya
        return "";
    }
}

bool t_ExpressionManager::printValueInExpressionNode(t_Expression *expr)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        return expr->printValueInExpressionNode(m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }
    else
    {
        logManager->LOG("Unable to get Label of the expression : NULL !", m_logFile, c_NoMessageLevelVerbosity);
        return false;
    }
}

bool t_ExpressionManager::replaceLeafOfAExpressionByExpression
(
        t_Expression *expression_to_replace,
        t_Expression *expression_replace_by
        )
{
    bool res = m_dummyExpression->replaceLeafOfAExpressionByExpression
            (
            expression_to_replace,
            expression_replace_by,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance
            );
    if (performReplacementForInvalidBit)
    {
        if (getTypeOfValueStoredInExpressionLabel(expression_to_replace) == SYMBOL &&
                getTypeOfValueStoredInExpressionLabel(expression_replace_by) == SYMBOL)
        {
            //replace only if both are symbols and flag is true
            string label_replace_by = getLabelOfExpression(expression_replace_by);
            string label_to_replace = getLabelOfExpression(expression_to_replace);
            label_replace_by += EXPRESSION_INVALID_BIT_LABEL;
            label_to_replace += EXPRESSION_INVALID_BIT_LABEL;

            t_Expression* to_replace_inv = createSymbol(label_to_replace, m_typeInfoOfInvalidBitExpr);
            t_Expression* replace_by_inv = createSymbol(label_replace_by, m_typeInfoOfInvalidBitExpr);
            res &= m_dummyExpression->replaceLeafOfAExpressionByExpression
                    (
                    to_replace_inv,
                    replace_by_inv,
                    m_expressionImplementationType,
                    m_expressionImplementationManagerInstance
                    );
        }
    }
    return res;
}

vector<t_Expression *> t_ExpressionManager::getVectorOfLeafExpressions(t_Expression *expr)
{
    t_Logger* logManager = t_Logger::getInstance();
    if (expr != NULL)
    {
        return expr->getVectorOfLeafExpressions(m_expressionImplementationType, m_expressionImplementationManagerInstance);
    }
    else
    {
        logManager->LOG("Expression is NULL", m_logFile, c_NoMessageLevelVerbosity);
        vector<t_Expression *> emptyVector;
        return emptyVector;
    }
}

vector<t_Expression*> t_ExpressionManager::getVectorOfLeafExpressions2(t_Expression* expr)
{
    vector<t_Expression*> leaves;
    t_HashTable<unsigned long long, bool> visited;
    getVectorOfLeafExpressionsRecurse(expr, leaves, visited);
    return leaves;
}

void t_ExpressionManager::getVectorOfLeafExpressionsRecurse(t_Expression* expr, vector<t_Expression*>& leaves, t_HashTable<unsigned long long, bool> &visited)
{
    if (expr == NULL || visited.hashtable_search(expr->getID()).success())
        return;
    visited.hashtable_insert(expr->getID(), true);
    if (isALeafExpression(expr))
    {
        leaves.push_back(expr);
        return;
    }

    vector<t_Expression*> operands = getVectorOfOperands(expr);
    for (int i = 0; i < operands.size(); ++i)
    {
        getVectorOfLeafExpressionsRecurse(operands[i], leaves, visited);
    }
    return;
}

/**
 * Part of evaluation assign values to leaves
 */
bool t_ExpressionManager::setValuesOfLeaves(map<string, string> &leaf_values, bool clearValidityOfAncestors)
{
    bool isSet = m_dummyExpression->setValuesOfLeaves
            (
            leaf_values,
            clearValidityOfAncestors,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance,
            m_evaluator
            );
    return isSet;
}

/**
 * Shift the actual value of this expression to given expression with the given label
 */
bool t_ExpressionManager::copyValueFromExpressionToLeafWithLabel(string &label, t_Expression *expr)
{
    t_Logger* logManager = t_Logger::getInstance();
    t_Expression *toExpr = m_dummyExpression->getExpressionWithLabel
            (
            label,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance
            );
    if (toExpr == NULL)
    {
        return false;
    }
    else if (toExpr == NULL && label.find("_0") != -1)
    {
        int upto = (label.length() - 2) < 0 ? label.length() : label.length() - 2;
        string label_without_instance = label.substr(0, upto);
        logManager->LOG(" Trying to find node without Instance:" + label_without_instance, m_logFile, c_DebugLevelVerbosity);
        toExpr = m_dummyExpression->getExpressionWithLabel
                (
                label_without_instance,
                m_expressionImplementationType,
                m_expressionImplementationManagerInstance
                );
    }
    bool isCopied = m_dummyExpression->copyValueFromExpressionToLeafWithLabel(toExpr, expr,
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance,
            m_evaluator);
    return isCopied;
}

bool t_ExpressionManager::evaluateExpression(t_Expression *expr)
{
    expr->evaluateExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    return true;
}

bool t_ExpressionManager::evaluateAVectorOfExpressions(
        vector <t_Expression *> &expression_to_evaluate
        )
{
    return m_dummyExpression->evaluateAVectorOfExpressions(expression_to_evaluate, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
}

bool t_ExpressionManager::SimplifyExpression(t_Expression * &expression_to_simplify)
{
    return expression_to_simplify->SimplifyExpression(expression_to_simplify, m_expressionImplementationType, m_expressionImplementationManagerInstance, ds);
}

t_Expression *t_ExpressionManager::copyExpressionUsingSuffix(t_Expression *expression_to_copy, string &suffix)
{
    //    t_Logger* logManager = t_Logger::getInstance();
    t_Expression *expr = m_dummyExpression->copyExpressionUsingSuffixForLeafNodes(expression_to_copy, suffix, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    if (expr == NULL)
        cerr << "ERROR from expression manager. expr is null\n";
    return expr;
}

/*** /
 *
 * This code is added for the purpose of Debugging.
 * Currently this is eliminated in the phase of optimization.
bool t_ExpressionManager::printMapOfTypeInferenceRules(ofstream *inf_rules_file)
{
    t_Logger* logManager = t_Logger::getInstance();
    map<string, t_InferTypeOfExpression *> ::iterator it;
    int counter = 0;
    for (it = mapOfTypeInferenceRules.begin(); it != mapOfTypeInferenceRules.end(); it++)
    {
 *inf_rules_file << "Rule Number " << ++counter << endl;
 *inf_rules_file << it->first << endl;
    }
    return true;
}
/**/
t_Expression *t_ExpressionManager::getExpressionWithLabel(string &label)
{
    t_Expression *expr = m_dummyExpression->getExpressionWithLabel(label, this->m_expressionImplementationType, this->m_expressionImplementationManagerInstance);

    return expr;
}

vector<t_Expression *> t_ExpressionManager::getVectorOfOperands(t_Expression *expression)
{
    vector<t_Expression *> operands =
            expression->getVectorOfOperands(this->m_expressionImplementationType, this->m_expressionImplementationManagerInstance);
    return operands;
}

bool t_ExpressionManager::checkWhetherCreatingEdgeFormsCycle(t_Expression *nodeFrom, t_Expression *nodeTo)
{
    bool result = m_dummyExpression->checkWhetherCreatingEdgeFormsCycle
            (
            nodeFrom,
            nodeTo,
            this->m_expressionImplementationType,
            this->m_expressionImplementationManagerInstance
            );

    return result;

}

bool t_ExpressionManager::gc()
{
    return m_dummyExpression->gc
            (
            m_expressionImplementationType,
            m_expressionImplementationManagerInstance
            );
}

bool t_ExpressionManager::replaceExpressionRepresentationInEM(t_Expression* old_expression, t_Expression* new_expression)
{
    bool replace_result = m_dummyExpression->replaceExpressionRepresentation(old_expression, new_expression, m_expressionImplementationType, m_expressionImplementationManagerInstance);
    return replace_result;
}

bool t_ExpressionManager::setMemoryExpressionBounds(t_Expression*& expression, t_MemoryArrayBoundsInfo& boundsInfo)
{
    if (expression == NULL)
    {
        return false;
    }
    else
    {
        t_ExpressionValue* value = expression->getValue(m_expressionImplementationType,
                m_expressionImplementationManagerInstance);
        if (value == NULL || value->m_typeInfo.m_type != TYPE_MEMORY_ARRAY)
        {
            return false;
        }// expression is not a memory type

        else
        {
            t_MemoryDataType* t = new t_ListOfMemoryEntries();
            t->m_bounds = new vector<t_MemoryBounds > (boundsInfo.m_bounds.begin(), boundsInfo.m_bounds.end());
            //            t_MemoryBounds temp;
            //            t->m_bounds->resize(boundsInfo.m_bounds.size(), temp);
            //            copy(boundsInfo.m_bounds.begin(), boundsInfo.m_bounds.end(), t->m_bounds->begin());
            value->m_actualExpressionValueStored.exprvalueMemory = t;
            return true;
        } // copied bounds
    } // expression is not null
}

t_Expression* t_ExpressionManager::createABitVectorConstantZeroExpression()
{
    return m_dummyExpression->createABitVectorConstantZeroExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
}

t_Expression* t_ExpressionManager::createABitVectorConstantOneExpression()
{
    return m_dummyExpression->createABitVectorConstantOneExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
}

t_Expression* t_ExpressionManager::createTwoOperandOperatorFromMultipleOperands(string& operator_label, vector<t_Expression*>& v_operands, TypeOfExpressionTuple& typeInfo)
{
    if (v_operands.size() == 0)
    {
        return NULL;
    }
    else if (v_operands.size() == 1)
    {
        return v_operands[0];
    }
    else
    {
        t_Expression* result = v_operands[0];
        t_TypeOfValueStoredInExpressionLabel operatorLabelType = OPERATOR;
        for (vector<t_Expression*>::iterator it = v_operands.begin() + 1, end = v_operands.end(); it != end; it++)
        {
            vector<t_Expression*> v_operands_new_expr(2);
            v_operands_new_expr[0] = result;
            v_operands_new_expr[1] = (*it);
            result = m_dummyExpression->createExpression(operator_label, operatorLabelType, typeInfo, v_operands_new_expr, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator, false);
        }
        //printExpressionToFileAsDAG("multi",result,cout);
        return result;
    }
}

t_Expression* t_ExpressionManager::createOneBitExpressionWithThreeOperands(string& label, t_Expression* operand1, t_Expression* operand2, t_Expression* operand3)
{
    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
    vector<t_Expression*> v_operands(3);
    v_operands[0] = operand1;
    v_operands[1] = operand2;
    v_operands[2] = operand3;
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    t_Expression* expr = m_dummyExpression->createExpression(label, labelType, typeInfo, v_operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    return expr;
}

t_Expression* t_ExpressionManager::createOneBitExpressionWithTwoOperands(string& label, t_Expression* operand1, t_Expression* operand2)
{
    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
    vector<t_Expression*> v_operands(2);
    v_operands[0] = operand1;
    v_operands[1] = operand2;
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    t_Expression* expr = m_dummyExpression->createExpression(label, labelType, typeInfo, v_operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    return expr;
}

t_Expression* t_ExpressionManager::createOneBitExpressionWithOneOperand(string& label, t_Expression* operand1)
{

    assert(operand1 != NULL);
    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
    vector<t_Expression*> v_operands(1, operand1);

    //  v_operands[0] =operand1;
    t_TypeOfValueStoredInExpressionLabel labelType = OPERATOR;
    t_Expression* expr = m_dummyExpression->createExpression(label, labelType, typeInfo, v_operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
    return expr;
}

bool t_ExpressionManager::isAConstantOneExpression(t_Expression* expr)
{
    if(getTypeOfValueStoredInExpressionLabel(expr) != CONSTANT)
        return false;
    switch(getTypeOfExpressionWithOutWidth(expr))
    {
        case TYPE_UNSIGNED_BITVECTOR:
        case TYPE_SIGNED_BITVECTOR:
        {
            string label = getLabelOfExpression(expr);
            string::size_type idx = label.find_first_not_of("1", 2);
            return idx == string::npos;
        }
        case TYPE_SIGNED_INTEGER:
        case TYPE_UNSIGNED_INTEGER:
        {
            
            return getConstantValuePresentInExpression(expr) == -1;
        }
    }
    return false;
}

bool t_ExpressionManager::isAConstantZeroExpression(t_Expression* expr)
{
     if(getTypeOfValueStoredInExpressionLabel(expr) != CONSTANT)
        return false;
    switch(getTypeOfExpressionWithOutWidth(expr))
    {
        case TYPE_UNSIGNED_BITVECTOR:
        case TYPE_SIGNED_BITVECTOR:
        {
            string label = getLabelOfExpression(expr);
            string::size_type idx = label.find_first_not_of("0", 2);
            return idx == string::npos;
        }
        case TYPE_SIGNED_INTEGER:
        case TYPE_UNSIGNED_INTEGER:
        {
            return getConstantValuePresentInExpression(expr) == 0;
        }
    }
    return false;
}

pair<t_Expression*,bool> t_ExpressionManager::simplifyByEvaluatingConstantOpers(t_Expression* expr, t_HashTable<unsigned long long, t_Expression*> &alreadySimplified)
{
    {
        t_HTStatusValue<t_Expression*> res = alreadySimplified.hashtable_search(expr->getID());
        if (res.success() == true)
        {
            t_Expression* newexpr = res.getValue();
            return make_pair(newexpr, getTypeOfValueStoredInExpressionLabel(newexpr) == CONSTANT);
        }
    }
    if (isALeafExpression(expr))
    {
        alreadySimplified.hashtable_insert(expr->getID(), expr);
        bool isConst = getTypeOfValueStoredInExpressionLabel(expr) == CONSTANT;
        return make_pair(expr,isConst);
    }
    unsigned long long exprID = expr->getID();
    vector<t_Expression*> operands = getVectorOfOperands(expr);
    pair<t_Expression*,bool> newoper;
    bool operandChanged = false;
    bool allConst = true;
    for (int i = 0; i < operands.size(); ++i)
    {
        newoper = simplifyByEvaluatingConstantOpers(operands[i], alreadySimplified);
        if(operands[i]->getID() != newoper.first->getID())
        {
            operandChanged = true;
        }
        allConst &= newoper.second;
        operands[i] = newoper.first;
    }
    
    
   if(operandChanged || allConst)
    {
       MAKE_COUNTER("Simplifications")
        std::string label = getLabelOfExpression(expr);
        TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(expr);
        expr = createExpression(label, operands, typeInfo);
        alreadySimplified.hashtable_insert(exprID, expr);
        return make_pair(expr, getTypeOfValueStoredInExpressionLabel(expr) == CONSTANT);
    }
    else
    {
        alreadySimplified.hashtable_insert(expr->getID(), expr);
        return make_pair(expr, getTypeOfValueStoredInExpressionLabel(expr) == CONSTANT);
    }
}

bool t_ExpressionManager::allOperandsConstant(vector<t_Expression*> ops)
{
    bool allOperandsConstant = true;
    for (int i = 0; i < ops.size(); ++i)
    {
        if (getTypeOfValueStoredInExpressionLabel(ops[i]) != CONSTANT)
        {
            allOperandsConstant = false;
            break;
        }
    }
    return allOperandsConstant;
}

t_Expression* t_ExpressionManager::getInvalidBitExpression(t_Expression* expr)
{
    if (expr == NULL)
    {
        return NULL;
    }

    if (getLableOfExpression(expr) != m_operatorLabelSelect)
    {
        int ub = getWidth(expr) - 1;
        int lb = 0;
        return getInvalidBitExpression(expr, ub, lb, RESULT_KEY);
    }
    else
    {
        vector<t_Expression*> v_operands = getVectorOfOperands(expr);
        int lb = getConstantValuePresentInExpression(v_operands[1]);
        int ub = getConstantValuePresentInExpression(v_operands[2]);
        return getInvalidBitExpression(v_operands[0], ub, lb, RESULT_KEY);
    }
}

t_Expression* t_ExpressionManager::getInvalidBitExpression(t_Expression* expr, int ub, int lb, string type)
{
    string key = generateInvalidBitManagerKey(expr, ub, lb, type);
    t_Expression* invalidbit = m_expressionImplementationManagerInstance->invalidbit_manager->getInvalidBitExpression(key);
    return invalidbit;
}

bool t_ExpressionManager::setInvalidBitExpression(t_Expression* expr, t_Expression* expr_invalid_bit)
{
    if (expr == NULL || expr_invalid_bit == NULL)
    {
        return false;
    }
    else if (getLableOfExpression(expr) != m_operatorLabelSelect)
    {
        int ub = getWidth(expr) - 1;
        int lb = 0;
        return setInvalidBitExpression(expr, ub, lb, RESULT_KEY, expr_invalid_bit);
    }
    else
    {
        vector<t_Expression*> v_operands = getVectorOfOperands(expr);
        int lb = getConstantValuePresentInExpression(v_operands[1]);
        int ub = getConstantValuePresentInExpression(v_operands[2]);
        return setInvalidBitExpression(v_operands[0], ub, lb, RESULT_KEY, expr_invalid_bit);
    }
}

bool t_ExpressionManager::setInvalidBitExpression(t_Expression* expr, int ub, int lb, string type, t_Expression* expr_invalid_bit)
{
    string key = generateInvalidBitManagerKey(expr, ub, lb, type);
    return m_expressionImplementationManagerInstance->invalidbit_manager->setInvalidBit(key, expr_invalid_bit);
}

//static bool print =false;

t_Expression* t_ExpressionManager::generateInvalidBitExpression(t_Expression*& expr)
{
    t_Expression* expr_invalid_bit = getInvalidBitExpression(expr);
    if (expr_invalid_bit != NULL)
    {
        // Invalid bit is already computed
        return expr_invalid_bit;
    }
    static int counter = 0;

    //will only be executed when the expression is not already in cache.
    //if it is already in cache, getInvalidBitExpression wil return non null value and we
    //should not reach here
    if (expr != NULL && expr == m_dummyExpression->getUnknownIndexExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance))
    {
        //add the invalid bit of unknown index expression in the cache, when accessed.
        setInvalidBitExpression(t_Expression::unknownIndexExpression, t_Expression::unknownIndexInvalidBitExpression);
        ++counter;
        return t_Expression::unknownIndexInvalidBitExpression;
    }

    /*if(expr!=NULL)
    {
        if( expr->getID() == 12149)
        {
            print = true;
            cout<<"print is set to true"<<endl;
        }
        if(print)
        {
            cout<<"Generating invalid bit for: "<<expr->getID()<<endl;
        }
    }**/

    // Otherwise compute it now
    string expr_label = getLabelOfExpression(expr);
    t_Expression* new_invalid_bit = NULL;
    if (getTypeOfValueStoredInExpressionLabel(expr) == CONSTANT)
    {
        if (expr_label.find(U_BIT) != string::npos || expr_label.find(X_BIT) != string::npos)
        {
            new_invalid_bit = createABitVectorConstantOneExpression();
        }
        else
        {
            new_invalid_bit = createABitVectorConstantZeroExpression();
        }
        setInvalidBitExpression(expr, new_invalid_bit);
    }
    else if (getTypeOfValueStoredInExpressionLabel(expr) == SYMBOL)
    {
        string label_invalid_bit = expr_label + EXPRESSION_INVALID_BIT_LABEL;
        TypeOfExpressionTuple typeInfo = m_typeInfoOfInvalidBitExpr; //getTypeOfExpressionTuple(expr);
        t_TypeOfValueStoredInExpressionLabel type_symbol = SYMBOL;
        new_invalid_bit = m_dummyExpression->createExpression(label_invalid_bit, typeInfo, type_symbol, m_expressionImplementationType, m_expressionImplementationManagerInstance);
        setInvalidBitExpression(expr, new_invalid_bit);
    }
    else if (expr_label == m_operatorLabelSelect)
    {
        vector<t_Expression*> operands_of_select = getVectorOfOperands(expr);
        t_Expression* first_operand = operands_of_select[0];
        t_Expression* lb_on_select = operands_of_select[1];
        t_Expression* ub_on_select = operands_of_select[2];

        int ub_val = getConstantValuePresentInExpression(ub_on_select);
        int lb_val = getConstantValuePresentInExpression(lb_on_select);

        if (ub_val == -1 || lb_val == -1)
        {
            t_Logger* logManager = t_Logger::getInstance();
            logManager->LOG("ERROR !! ExpressionManager: Select expression with unknown bounds found " + toString(expr->getID()), m_logFile, c_NoMessageLevelVerbosity);
            return NULL;
        }
        new_invalid_bit = generateInvalidBitExpressionOfSelectExpression(first_operand, ub_val, lb_val);
    }
    else
    {
        new_invalid_bit = generateInvalidBitExpressionOfSelectExpression(expr, getWidth(expr) - 1, 0);
    }

    //    string invalid_bit_str = "Invlaid Bit" + toString(expr->getID());
    //    string name = "Temp" + toString(counter++);
    //    cout << " Creating file:" << name << endl;
    //    ofstream* fout = new ofstream(name.c_str());
    //    new_invalid_bit->printExpressionToFileAsDAG(invalid_bit_str, m_expressionImplementationType, m_expressionImplementationManagerInstance, fout);
    //    fout->close();    
    return new_invalid_bit;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForBitwiseOperators(t_Expression* expr, int ub, int lb)
{
    vector<t_Expression*> operands_of_expr = getVectorOfOperands(expr);
    t_Expression* op1_invalid_bit = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[0], ub, lb);
    t_Expression* op2_invalid_bit = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[1], ub, lb);
    t_Expression* op1_atom = createASelectExpressionOnExpression(operands_of_expr[0], ub, lb);
    t_Expression* op2_atom = createASelectExpressionOnExpression(operands_of_expr[1], ub, lb);
    t_Expression* temp_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR, op1_invalid_bit, op2_invalid_bit);
    string label = getLabelOfExpression(expr);

    t_Expression* result_invalid_bit = NULL;
    if (label == m_operatorLabelBitwiseAND || label == m_operatorLabelBitwiseNAND || label == m_operatorLabelLogAND)
    {
        t_Expression* negation_op1_invalid_bit = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, op1_invalid_bit);
        t_Expression* redor_op2_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, op2_atom);
        t_Expression* redor_op2_atom_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR, redor_op2_atom, op2_invalid_bit);
        t_Expression* bitor_negation_op1_inv_and_redor_op2_atom_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR, negation_op1_invalid_bit, redor_op2_atom_with_invalid_bit);
        t_Expression* temp_expression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, temp_invalid_bit, bitor_negation_op1_inv_and_redor_op2_atom_with_invalid_bit);

        t_Expression* negation_op2_invalid_bit = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, op2_invalid_bit);
        t_Expression* redor_op1_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, op1_atom);
        t_Expression* redor_op1_atom_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR, redor_op1_atom, op1_invalid_bit);
        t_Expression* bitor_negation_op2_inv_and_redor_op1_atom_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR, negation_op2_invalid_bit, redor_op1_atom_with_invalid_bit);
        result_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, temp_expression, bitor_negation_op2_inv_and_redor_op1_atom_with_invalid_bit);

    }
    else if (label == m_operatorLabelBitwiseOR || label == m_operatorLabelBitwiseNOR || label == m_operatorLabelLogOR)
    {
        t_Expression* redand_op2_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedAND, op2_atom);
        t_Expression* and_of_redand_op2_atom_op1_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, op1_invalid_bit, redand_op2_atom);
        t_Expression* negation_of_above = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, and_of_redand_op2_atom_op1_invalid_bit);
        t_Expression* temp_expr = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, temp_invalid_bit, negation_of_above);

        t_Expression* redand_op1_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedAND, op1_atom);
        t_Expression* and_of_redand_op1_atom_op2_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, op2_invalid_bit, redand_op1_atom);
        t_Expression* negation_of_above_1 = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, and_of_redand_op1_atom_op2_invalid_bit);


        result_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, temp_expr, negation_of_above_1);

        t_Expression* and_of_op1inv_and_op2inv = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, op1_invalid_bit, op2_invalid_bit);
        result_invalid_bit = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, and_of_op1inv_and_op2inv,
                createABitVectorConstantOneExpression(), result_invalid_bit);


    }
    else if (label == m_operatorLabelBitwiseXOR || label == m_operatorLabelBitwiseXNOR)
    {
        result_invalid_bit = temp_invalid_bit;

    }

    return result_invalid_bit;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOfSelectExpression(t_Expression* expr, int ub, int lb)
{
    if (expr == NULL)
    {
        cout << " ERROR ! Invalid Bit Of SelectExpression first operand is NULL" << endl;
        return NULL;
    }
    string expr_label = getLabelOfExpression(expr);

    {
        int width = getWidth(expr);
        if (width != -1)//not a memory operation
        {
            if (!(width > ub) || !(width > lb) || !(ub >= lb) || !(lb >= 0))
            {
                cout << "**** Exceptional Expression created ****" << endl;
                cout << "Width:" << width << " UB:" << ub << " LB:" << lb << endl;
                printExpressionToFileAsDAG("check", expr, cout);
                //assert(width>ub);
                //assert(width>lb);
                //assert(ub>=lb);
                //assert(lb>=0);
                if (ub < lb)
                    swap(ub, lb);
            }
        }
    }

    t_Expression* invalid_bit = getInvalidBitExpression(expr, ub, lb, RESULT_KEY);

    if (invalid_bit != NULL) //use cache
        return invalid_bit;
    /*
    if(0 && expr!=NULL)
    {
        if( expr->getID() == 12149)
        {
            print = true;
            cout<<"print is set to true"<<endl;
        }
        if(print)
        {
            cout<<"Generating invalid bit for select: "<<expr->getID()<<endl;
        }
    }**/
    static int __count = 0;
    {
        __count++;
        cout << "   Generating invalid bit:" << __count << " for exprID:" << expr->getID() << "\n";
    }

    if (isALeafExpression(expr) == true)
    {
        invalid_bit = generateInvalidBitExpression(expr);
    }
    else if (ub < 0)
    {
        vector<t_Expression*> ops = getVectorOfOperands(expr);
        t_Expression* invs = generateInvalidBitExpression(ops[0]);

        for (int i = 1; i < ops.size(); ++i)
        {
            t_Expression* temp = generateInvalidBitExpression(ops[i]);
            invs = createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR, invs, temp);
        }
        invalid_bit = invs;
        //assert(false && "Not handled. unknown width");
    }
    else if (expr_label == m_operatorLabelConcat || expr_label == m_operatorLabelLefttoRightStream)
    {
        vector<t_Expression*> v_atoms_ub_lb = getAtomsBetweenUbAndLb(expr, ub, lb);
        assert(v_atoms_ub_lb.empty() == false);
        vector<t_Expression*> v_atoms_invalid_bits(v_atoms_ub_lb.size(), NULL);
        int i = 0;
        for (vector<t_Expression*>::iterator it = v_atoms_ub_lb.begin(),
                end = v_atoms_ub_lb.end(); it != end; it++, i++)
        {
            v_atoms_invalid_bits[i] = generateInvalidBitExpression(v_atoms_ub_lb[i]);
        }
        assert(v_atoms_invalid_bits.empty() == false);
        if (v_atoms_invalid_bits.size() == 1)
        {
            invalid_bit = v_atoms_invalid_bits[0];
        }
        else
        {
            invalid_bit = createTwoOperandOperatorFromMultipleOperands(m_operatorLabelBitwiseOR, v_atoms_invalid_bits, m_typeInfoOfInvalidBitExpr);
        }
    }
    else if (expr_label == m_operatorLabelADD)
    {
        invalid_bit = generateInvalidBitExpressionOnPartOfADDExpression(expr, ub, lb, false);
    }
    else if (expr_label == m_operatorLabelSUB)
    {
        invalid_bit = generateInvalidBitExpressionOnPartOfSUBExpression(expr, ub, lb, false);
    }
    else if (expr_label == m_operatorLabelBitwiseAND || expr_label == m_operatorLabelBitwiseOR || expr_label == m_operatorLabelBitwiseNAND
            || expr_label == m_operatorLabelBitwiseNOR || expr_label == m_operatorLabelBitwiseXNOR || expr_label == m_operatorLabelBitwiseXOR ||
            expr_label == m_operatorLabelLogAND || expr_label == m_operatorLabelLogOR)
    {
        invalid_bit = generateInvalidBitExpressionForBitwiseOperators(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelRedAND || expr_label == m_operatorLabelRedNAND || expr_label == m_operatorLabelRedNOR ||
            expr_label == m_operatorLabelRedOR || expr_label == m_operatorLabelRedXNOR || expr_label == m_operatorLabelRedXOR || expr_label == m_operatorLabelBitwiseNEG || expr_label == m_operatorLabelLogNOT)
    {
        invalid_bit = generateInvalidBitExpressionForUnaryOperators(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelMultiply)
    {
        invalid_bit = generateInvalidBitExpressionOnPartOfMULTExpression(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelDivide)
    {
        invalid_bit = generateInvalidBitExpressionOnPartOfDIVExpression(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelModulus)
    {
        invalid_bit = generateInvalidBitExpressionOnPartOfMODExpression(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelZeroextension || expr_label == m_operatorLabelSignextension)
    {

        vector<t_Expression*> v_operands = getVectorOfOperands(expr);
        int wd = getWidth(v_operands[0]);

        if (ub >= wd && lb >= wd) //selection on the zero extended part (not affected by v_operands[0]
        {
            //cout<<"1 called  between  "<<ub<<":"<<lb<<" width is:"<<wd<<getLabelOfExpression(v_operands[0])<<endl;
            //in zero extn part. invalid bit is zero
            if(expr_label == m_operatorLabelZeroextension)
            {
//                TypeOfExpressionTuple typeTuple1bv = {TYPE_UNSIGNED_BITVECTOR, 1};
//                string constValueZero = "0";
//                t_Expression *expr_1bv = this->createConstant(constValueZero, typeTuple1bv);
                invalid_bit = createABitVectorConstantZeroExpression();
            }
            else if(expr_label == m_operatorLabelSignextension)
            {//for sign extension, the extended part depends on MSB of operands[0]
                invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands.at(0), wd-1, wd-1);
            }
        } /*else if (ub >= wd && lb < wd)   
        {
            //            cout << "2 called  between  " << ub << ":" << lb << " width is:" << wd << getLabelOfExpression(v_operands[0]) << endl;
            //range is wd to lb
            invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[0], wd - 1, lb);
        }
        else
        {
            //            cout << "3 called  between  " << ub << ":" << lb << " width is:" << wd << getLabelOfExpression(v_operands[0]) << endl;
            //            range is ub to lb
            invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[0], ub, lb);
        }*/
        else
        {//newub = min(ub, wd-1) and newlb = max(0,lb) = lb
            invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[0], min(ub, wd - 1), lb);
        }
    }
    else if (expr_label == m_operatorLabelLeftShift)
    {

        invalid_bit = generateInvalidBitExpressionForLEFTSHIFTOperators(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelRightShift)
    {
        invalid_bit = generateInvalidBitExpressionForRIGHTSHIFTOperators(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelLessThan || expr_label == m_operatorLabelLessThanOrEqual ||
            expr_label == m_operatorLabelGreaterThan || expr_label == m_operatorLabelGreaterThanOrEqual ||
            expr_label == m_operatorLabelLogicalEquality || expr_label == m_operatorLabelLogicalInEquality)
    {
        invalid_bit = generateInvalidBitExpressionForCOMPOperators(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelLogicalWildEquality || expr_label == m_operatorLabelLogicalWildInEquality || expr_label == m_operatorLabelCaseXequlity || expr_label == m_operatorLabelCaseZequlity)
    {
        TypeOfExpressionTuple typeTuple1bv = {TYPE_UNSIGNED_BITVECTOR, 1};
        string constValueZero = "0";
        t_Expression *expr_1bv = this->createConstant(constValueZero, typeTuple1bv);
        invalid_bit = expr_1bv;
    }
    else if (expr_label == m_operatorLabelRighttoLeftStream)
    {
        invalid_bit = generateInvalidBitExpressionForRightToLeftStreamOperator(expr, ub, lb);

    }
    else if (expr_label == m_operatorLabelIte)
    {
        invalid_bit = generateInvalidBitExpressionForIteOperator(expr, ub, lb);
    }
    else if (expr_label == m_operatorLabelSelect)
    {
        vector<t_Expression*> v_operands_of_select = getVectorOfOperands(expr);

        int lb_in_select = getConstantValuePresentInExpression(v_operands_of_select[1]);

        {
            int wd = getWidth(v_operands_of_select[0]);
            int newub = min(ub + lb_in_select, wd - 1);
            int newlb = min(lb + lb_in_select, newub);

            invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_select[0], newub, newlb); //ub + lb_in_select, lb + lb_in_select);
        }
        //cout << "Error !! Select of select is detected which is taken care during generatrion" << endl;
    }
    else if (expr_label == "read")
    {
        cout << "Generating invalid bit of read " << endl;
        invalid_bit = generateInvalidBitExpressionOfReadExpression(expr, ub, lb);
        cout << boolalpha << (invalid_bit == NULL) << endl;
    }
    else if (expr_label == "update")
    {
        //        {
        //            static string label = "temp_update_symbol";
        //            static TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_INTEGER,-1};
        //            static t_Expression* symbol = createSymbol(label, typeInfo);
        //            static t_Expression* inv_Bit = createABitVectorConstantOneExpression();
        //            setInvalidBitExpression(symbol,inv_Bit);
        //            
        //            //The typeInfo passed is unused as it is only reqd for read(ite(...)) exprs
        //            invalid_bit = generateInvalidBitExpressionOfReadExpression(expr,symbol,ub,lb,typeInfo);
        //        }        
        //        getchar();
        {
            cout << "Invalid bit for update. Returning 0" << endl;
            invalid_bit = createABitVectorConstantZeroExpression();
        }
    }
    else if (expr_label == "replic")
    {
        vector<t_Expression*> ops = getVectorOfOperands(expr);
        assert(ops.size() == 2);
        assert(ops[1] != NULL);
        assert(getTypeOfValueStoredInExpressionLabel(ops[1]) == CONSTANT);

        invalid_bit = generateInvalidBitExpression(ops[0]);
    }
    else
    {
        throw std::runtime_error("ERROR!! This " + expr_label + " is not handled ");

    }
    assert(invalid_bit != NULL);
    setInvalidBitExpression(expr, ub, lb, RESULT_KEY, invalid_bit);
    return invalid_bit;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForCOMPOperators(t_Expression* expr, int ub, int lb)
{
    vector<t_Expression*> v_operands = getVectorOfOperands(expr);
    t_Expression* invalid_bit_op1 = generateInvalidBitExpressionOfSelectExpression(v_operands[0], getWidth(v_operands[0]) - 1, 0);
    t_Expression* invalid_bit_op2 = generateInvalidBitExpressionOfSelectExpression(v_operands[1], getWidth(v_operands[1]) - 1, 0);
    t_Expression* result_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, invalid_bit_op1, invalid_bit_op2);
    return result_invalid_bit;

}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForLEFTSHIFTOperators(t_Expression* expr, int ub, int lb)
{
    vector<t_Expression*> v_operands = getVectorOfOperands(expr);
    t_Expression* op2_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[1], getWidth(v_operands[1]) - 1, 0);

    int op2_width = getWidth(v_operands[1]);
    op2_width = (op2_width == -1) ? 32 : op2_width;
    int op2_max = (1 << op2_width) - 1;
    t_Expression* temp_expr1_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[0], ub, max(lb - op2_max, 0));
    //string constValueUB = integerToBinaryString(ub);
    //string constValueZero = "0";
    //TypeOfExpressionTuple typeTuplebv1 = {TYPE_UNSIGNED_BITVECTOR, 1};
    unsigned ub_bits = max(getBitsRequired(ub), (unsigned)getWidth(v_operands[1]));
    
    t_Expression *exprUB = createIntegerConstant(ub,ub_bits);//createConstant(constValueUB, typeTuplebv1);
    t_Expression *exprZero = createABitVectorConstantZeroExpression();//createConstant(constValueZero, typeTuplebv1);
    
    t_Expression* comp_op2_ub = createOneBitExpressionWithTwoOperands(m_operatorLabelGreaterThan,
            extendExpr(v_operands[1], ub_bits),
            exprUB);
    vector<t_Expression*> ite_operands(3, NULL);
    ite_operands[0] = comp_op2_ub;
    ite_operands[1] = exprZero;
    ite_operands[2] = temp_expr1_invalid_bit;
    t_Expression* ite_expression = createExpression(m_operatorLabelIte, ite_operands, m_typeInfoOfInvalidBitExpr);

    ite_operands[0] = op2_invalid_bit;
    ite_operands[1] = temp_expr1_invalid_bit;
    ite_operands[2] = ite_expression;
    t_Expression* final_ite_expression = createExpression(m_operatorLabelIte, ite_operands, m_typeInfoOfInvalidBitExpr);
    return final_ite_expression;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForRIGHTSHIFTOperators(t_Expression* expr, int ub, int lb)
{
    vector<t_Expression*> v_operands = getVectorOfOperands(expr);
    t_Expression* op2_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[1], getWidth(v_operands[1]) - 1, 0);

    int op2_width = getWidth(v_operands[1]);
    op2_width = (op2_width == -1) ? 32 : op2_width;
    int op2_max = (1 << op2_width) - 1;
    t_Expression* temp_expr1_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands[0], min(ub + op2_max, getWidth(v_operands[0]) - 1), lb);
    //ub - op2max is modified to ub + op2max by tanmay and dinesh
    //check
    //string constValueUB = integerToBinaryString(getWidth(v_operands[0]) - 1 - lb);
    //string constValueZero = "0";
    //TypeOfExpressionTuple typeTuplebv1 = {TYPE_UNSIGNED_BITVECTOR, 1};
    unsigned ub_bits = max(getBitsRequired(getWidth(v_operands[0]) - 1 - lb), (unsigned)getWidth(v_operands[1]));
    
    t_Expression *exprUB = createIntegerConstant(getWidth(v_operands[0]) - 1 - lb, ub_bits);// createConstant(constValueUB, typeTuplebv1);
    t_Expression *exprZero = createABitVectorConstantZeroExpression();//createConstant(constValueZero, typeTuplebv1);
    t_Expression* comp_op2_ub = createOneBitExpressionWithTwoOperands(m_operatorLabelGreaterThan,
            extendExpr(v_operands[1], ub_bits),
            exprUB);
    vector<t_Expression*> ite_operands(3, NULL);
    ite_operands[0] = comp_op2_ub;
    ite_operands[1] = exprZero;
    ite_operands[2] = temp_expr1_invalid_bit;
    t_Expression* ite_expression = createExpression(m_operatorLabelIte, ite_operands, m_typeInfoOfInvalidBitExpr);

    ite_operands[0] = op2_invalid_bit;
    ite_operands[1] = temp_expr1_invalid_bit;
    ite_operands[2] = ite_expression;
    t_Expression* final_ite_expression = createExpression(m_operatorLabelIte, ite_operands, m_typeInfoOfInvalidBitExpr);
    return final_ite_expression;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForRightToLeftStreamOperator(t_Expression* expr, int ub, int lb)
{
    /**
     * op1= n
     * N = number of blocks op2/n
     * k=op2 block in which ub lies = N-ub/n-1
     * m=op2 block in which lb lies = N-lb/n-1
     * sel(op2,m*n-1,(k+1)*n) || sel(op2,k*n+ub%n,k*n)||  sel(op2,(m+1)*n-1,m*n+(lb%n))
     */
    vector<t_Expression*> v_operands = getVectorOfOperands(expr);
    cout << "size of org operands is:" << v_operands.size() << endl;
    /*if (v_operands.size() == 1 || ub == lb || getTypeOfValueStoredInExpressionLabel(v_operands[0]) != CONSTANT)
    {
        TypeOfExpressionTuple typeinfo_expr = {TYPE_UNSIGNED_BITVECTOR, getWidth(expr)};
        t_Expression* operand1 = createExpression(m_operatorLabelConcat, v_operands, typeinfo_expr);
        int width_of_op1 = getWidth(operand1) - 1;
        return generateInvalidBitExpressionOfSelectExpression(operand1, width_of_op1 - lb, width_of_op1 - ub);
    }*/

    vector<t_Expression*> v_remaining_operands(v_operands.begin() + 1, v_operands.end());
    cout << v_remaining_operands.size() << " is the size of operands" << endl;
    TypeOfExpressionTuple typeinfo_expr = {TYPE_UNSIGNED_BITVECTOR, getWidth(expr)};
    t_Expression* operand2 = createExpression(m_operatorLabelConcat, v_remaining_operands, typeinfo_expr);

    int n = getConstantValuePresentInExpression(v_operands[0]);
    int width = getWidth(operand2);

    if (n == 1 || ub == lb)
    {
        TypeOfExpressionTuple typeinfo_expr = {TYPE_UNSIGNED_BITVECTOR, getWidth(expr)};
        t_Expression* operand1 = createExpression(m_operatorLabelConcat, v_remaining_operands, typeinfo_expr);
        //int width_of_op1 = getWidth(operand1) - 1;
        return generateInvalidBitExpressionOfSelectExpression(operand1, width - 1 - lb, width - 1 - ub);
    }

#ifdef __OLD_CODE_RTOLSTREAM__
    int N = width / n;

    int k = N - ub / n - 1;
    int m = N - lb / n - 1;

    /**
       @TODO: see fil+
     * e /home/rajkumar/Desktop/Toytests/sec7_operators_and_exprs.right_to_left_stream
       Case width is not divisible by n
     */
    t_Expression* temp1 = NULL;
    if (m - k > 1)
    {
        temp1 = generateInvalidBitExpressionOfSelectExpression(operand2, m * n - 1, (k + 1) * n);
    }

    t_Expression* temp2 = generateInvalidBitExpressionOfSelectExpression(operand2, k * n + ub % n, k * n);
    t_Expression* temp3 = generateInvalidBitExpressionOfSelectExpression(operand2, (m + 1) * n - 1, m * n + (lb % n));

    t_Expression* temp4 = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, temp2, temp3);
    if (temp1 == NULL)
        return temp4;

    return createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, temp1, temp4);
#else

    /**
     *  k = n (block size)
     * M = width of all operands other than zero
     * Suppose the grouping of k-bits
into k-sized blocks starts from the least-significant bit of {a1, ... an} and proceeds towards the
left.  How many blocks will I get?  Clearly, ceil(M/k).  Let's call this number (ceil(M/k)) as B.
Let us call these B blocks of {a1, ... an} as Z[B-1] ... Z[0].  Note that all blocks except perhaps
Z[B-1] has exactly k bits.  Z[B-1] may have fewer than k bits, because we started grouping
bits into k-sized blocks starting from the right.  Thus, {a1, ... an} = {Z[B-1], ... Z[0]}
     *Assuming the bits are indexed from 0 to M-1, with 0 being the least significant bit,
the ub-th bit of {Z[0], ... Z[B-1]} is present in the C-th block from the left, where
C = ceil((M-ub)/k).  This block is nothing but Z[C-1].  In fact, the ub-th bit of {Z[0], ... Z[B-1]}
is (Z[C-1])[c], where c = k - (M - ub - (C-1)*k), and I have assumed that within block
Z[C-1], the rightmost bit is indexed 0, and the leftmost bit is indexed k-1.

By a very similar reasoning, the lb-th bit of {Z[0], ... Z[B-1]} is (Z[D-1])[d], where
D = ceil((M-lb)/k) and d = k - (M - lb - (D-1)*k).

So, what is sel(<<k{a1, ... an}, ub, lb)?

If C < D -1, then it is {(Z[C-1])[c:0], (Z[C])[k-1:0],  ... (Z[D-2])[k-1:0], (Z[D-1])[k-1:d]}
If C = D -1, then it is {(Z[C-1])[c:0], (Z[D-1])[k-1:d]}
If C = D, then it is (Z[C-1])[c:d]

     */

    int k = n;
    int M = width;
    int C = (M - ub) / k;
    int D = (M - lb) / k;
    int c = /* k - (M - ub - (C)*k)*/ ub % k;
    int d = lb % k; //k - (M - lb - (D)*k);
    t_Expression* temp1;
    if (C == D)
    {
        temp1 = generateInvalidBitExpressionOfSelectExpression(operand2, min(D * k + d, M - 1), C * k + c); //min to handle the case that D is in the last block
        //which is smaller than block size
        return temp1;
    }
    else if (C <= D - 1)
    {
        t_Expression* temp2 = generateInvalidBitExpressionOfSelectExpression(operand2, C * k + c, C * k); //for ub block
        t_Expression* temp3 = generateInvalidBitExpressionOfSelectExpression(operand2, min(D * k + (k - 1), M - 1), D * k + d); //for lb block

        t_Expression* temp4 = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, temp2, temp3);

        if (C == D - 1)
        {
            // no blocks between C,D
            return temp4;
        }

        //else
        t_Expression* temp5 = generateInvalidBitExpressionOfSelectExpression(operand2, D * k - 1, (C + 1) * k);
        temp1 = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, temp4, temp5);
        return temp1;
    }
    else
    {
        cerr << "ERROR!! C should always be <= D" << endl;
        return NULL;
    }
#endif
}

/**
 * NOT(certain), where

certain = ((op1 == 0) AND (e1 == 0)) OR ((op2 == 0) AND (e2 == 0)) OR ((e1 == 0) AND (e2 == 0))
 * 
 * The above one is simplified to,
 * (op1 or e1) and (op2 or e2) and (e1 or e2)
 * now op1 is redor(op1)
 * 
 */
t_Expression* t_ExpressionManager::generateInvalidBitExpressionOnPartOfMULTExpression(t_Expression* expr, int ub, int lb)
{
    t_Expression* result_invalid_bit_expression;
    //cout << "called" << endl;
    vector<t_Expression*> v_operands_of_expr = getVectorOfOperands(expr);
    t_Expression* op1_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[0], ub, 0);
    t_Expression* op2_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[1], ub, 0);
    t_Expression* op1_atom = createASelectExpressionOnExpression(v_operands_of_expr[0], ub, 0);
    t_Expression* op2_atom = createASelectExpressionOnExpression(v_operands_of_expr[1], ub, 0);
    t_Expression* redor_op1_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, op1_atom);
    t_Expression* redor_op2_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, op2_atom);
    t_Expression* temp1_expr = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, redor_op1_atom, op1_invalid_bit);
    t_Expression* temp2_expr = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, redor_op2_atom, op2_invalid_bit);
    t_Expression* temp3_expr = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, op1_invalid_bit, op2_invalid_bit);
    t_Expression* temp4_expr = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, temp1_expr, temp2_expr);

    result_invalid_bit_expression = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, temp4_expr, temp3_expr);

    return result_invalid_bit_expression;

}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOnPartOfMODExpression(t_Expression* expr, int ub, int lb)
{
    /**
     * ite(inv(b), 1, temp), where
temp = ite(b=0, 1, temp1), where
temp1 = ite((b <= 2^lb), 0, ite(b = 2^i, inv(t), inv(a)), where
t =  sel(a, min(ub, width(b) - 1), lb)

and of course, we need to conjoin
(b >> i != 0) AND (b >> (i+1) = 0)*/


    string operator_label = getLabelOfExpression(expr);
    vector<t_Expression*> operands_of_expr = getVectorOfOperands(expr);
    t_Expression* op2_invalid_bit = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[1], ub, lb);
    int op2_width = getWidth(operands_of_expr[1]);
    int op1_width = getWidth(operands_of_expr[0]);
    string label = getLabelOfExpression(operands_of_expr[1]);
    if (label == m_operatorLabelZeroextension)
    {
        vector<t_Expression*> operands_of_op2 = getVectorOfOperands(operands_of_expr[1]);
        op2_width = getWidth(operands_of_op2[0]);
    }
    label = getLabelOfExpression(operands_of_expr[0]);
    if (label == m_operatorLabelZeroextension)
    {
        vector<t_Expression*> operands_of_op1 = getVectorOfOperands(operands_of_expr[0]);
        op1_width = getWidth(operands_of_op1[0]);
    }
    int two_power_lb = 1 << lb;
    t_Expression* temp1;
    t_Expression* op2_equal_zero;
    t_Expression* temp;
    t_Expression* final_invalid_bit_expr;
    if (min(ub, op2_width - 1) < lb)
    {
        temp1 = createABitVectorConstantZeroExpression();
        op2_equal_zero = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, operands_of_expr[1], createABitVectorConstantZeroExpression());
        temp = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, op2_equal_zero, createABitVectorConstantOneExpression(), temp1);
        final_invalid_bit_expr = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, op2_invalid_bit, createABitVectorConstantOneExpression(), temp);
        return final_invalid_bit_expr;
    }
    else
    {

        //string two_power_lb_label = integerToBinaryString(two_power_lb);
        //TypeOfExpressionTuple type_info_2powlb = {TYPE_UNSIGNED_BITVECTOR, };
        t_Expression* expr_two_power_lb = createIntegerConstant(two_power_lb, getWidth(operands_of_expr[1]));//createConstant(two_power_lb_label, type_info_2powlb);
        vector<t_Expression*> operands_of_2power_gte_b(2, NULL);
        operands_of_2power_gte_b[1] = operands_of_expr[1];
        operands_of_2power_gte_b[0] = expr_two_power_lb;
        TypeOfExpressionTuple type_info_gte = {TYPE_BOOL, 1};


        t_Expression* two_power_lb_gte_b = createExpression(m_operatorLabelGreaterThanOrEqual, operands_of_2power_gte_b, type_info_gte);

        t_Expression* i = getConstraintFunction(operator_label, operands_of_expr);


        vector<t_Expression*> operands_of_2Power_i(2, NULL);
        operands_of_2Power_i[0] = createABitVectorConstantOneExpression();
        operands_of_2Power_i[1] = i;
        TypeOfExpressionTuple typeinfo_2Power_i = {TYPE_UNSIGNED_BITVECTOR, op2_width};
        t_Expression* two_power_i = createExpression(m_operatorLabelLeftShift, operands_of_2Power_i, typeinfo_2Power_i);

        vector<t_Expression*> operands_of_b_minus_2power_i(2, NULL);
        operands_of_b_minus_2power_i[0] = operands_of_expr[1];
        operands_of_b_minus_2power_i[1] = two_power_i;
        t_Expression* j = createExpression(m_operatorLabelSUB, operands_of_b_minus_2power_i, typeinfo_2Power_i);
        t_Expression* j_eq_0 = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, j, createIntegerConstant(0, typeinfo_2Power_i.m_width));

        t_Expression* t = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[0], min(ub, op2_width - 1), lb);
        t_Expression* op1_invalid_bit = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[0], op1_width - 1, 0);
        t_Expression* temp2 = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, j_eq_0, t, op1_invalid_bit);
        temp1 = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, two_power_lb_gte_b, createABitVectorConstantZeroExpression(), temp2);

        op2_equal_zero = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, operands_of_expr[1], createIntegerConstant(0, getWidth(operands_of_expr[1])));
        ;
        temp = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, op2_equal_zero, createABitVectorConstantOneExpression(), temp1);
        final_invalid_bit_expr = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, op2_invalid_bit, createABitVectorConstantOneExpression(), temp);
        return final_invalid_bit_expr;
    }
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOnPartOfDIVExpression(t_Expression* expr, int ub, int lb)
{
    /*
  For sel(a/b, ub, lb), the inv bit is given by

ite(inv(b), 1, temp), where
temp = ite(b=0, 1, temp1), where
temp1 = ite(b-2^i = 0, temp3, temp2), where
temp2 = inv(sel(a, width(a)-1, lb))
temp3 = inv(sel(a, min(ub+width(b)-1, width(a)-1), lb))

I've used width(a) - 1 = a_max_index

Of course, we'll also need to add the constraint
(b >> i != 0) AND (b >> (i+1) = 0) 
*/



    string operator_label = getLabelOfExpression(expr);
    vector<t_Expression*> operands_of_expr = getVectorOfOperands(expr);

    string op_label = getLabelOfExpression(operands_of_expr[0]);
    int a_width = getWidth(operands_of_expr[0]), b_width = getWidth(operands_of_expr[1]);
    vector<t_Expression*> operands_of_zero_ext;
    if (op_label == m_operatorLabelZeroextension)
    {
        operands_of_zero_ext = getVectorOfOperands(operands_of_expr[0]);
        a_width = getWidth(operands_of_zero_ext[0]);
    }
    op_label = getLabelOfExpression(operands_of_expr[1]);
    if (op_label == m_operatorLabelZeroextension)
    {
        operands_of_zero_ext = getVectorOfOperands(operands_of_expr[1]);
        b_width = getWidth(operands_of_zero_ext[0]);
    }


    t_Expression* i = getConstraintFunction(operator_label, operands_of_expr);


    vector<t_Expression*> operands_of_2Power_i(2, NULL);
    operands_of_2Power_i[0] = createIntegerConstant(1, b_width);
    operands_of_2Power_i[1] = i;
    TypeOfExpressionTuple typeinfo_2Power_i = {TYPE_UNSIGNED_BITVECTOR, b_width};
    t_Expression* two_power_i = createExpression(m_operatorLabelLeftShift, operands_of_2Power_i, typeinfo_2Power_i);



    vector<t_Expression*> operands_of_b_minus_2power_i(2, NULL);
    operands_of_b_minus_2power_i[0] = operands_of_expr[1];
    operands_of_b_minus_2power_i[1] = two_power_i;
    t_Expression* j = createExpression(m_operatorLabelSUB, operands_of_b_minus_2power_i, typeinfo_2Power_i);



    t_Expression* j_eq_0 = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, j, createIntegerConstant(0, typeinfo_2Power_i.m_width));



    t_Expression* temp2 = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[0], a_width - 1, lb);

    t_Expression* temp3 = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[0], min(ub + b_width - 1, a_width - 1), lb);


    t_Expression* temp1 = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, j_eq_0, temp3, temp2);


    t_Expression* b_equal_zero = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, operands_of_expr[1], createIntegerConstant(0, getWidth(operands_of_expr[1])));

    t_Expression* temp = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, b_equal_zero, createABitVectorConstantOneExpression(), temp1);

    t_Expression* iv_of_b = generateInvalidBitExpression(operands_of_expr[1]);
    t_Expression* final_iv_expr = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, iv_of_b, createABitVectorConstantOneExpression(), temp);
    return final_iv_expr;
}

t_Expression* t_ExpressionManager::getConstraintFunction(string& operator_label, vector<t_Expression*>& operands_of_expr)
{
    static int counter = 0;
    if (operator_label == m_operatorLabelDivide || operator_label == m_operatorLabelModulus)
    {
        string constraint_symbol_label = CONSTRAINT_EXPR_NAME + operator_label + "#" + toString(operands_of_expr[1]->getID()) + "#" + toString(counter++);
        TypeOfExpressionTuple typeinfo_int = {TYPE_UNSIGNED_INTEGER, -1};
        //(b >> i  != 0) AND (b >> (i+1) == 0)  
        vector<t_Expression*> v_operands_of_rt_shift(2, NULL);
        v_operands_of_rt_shift[0] = operands_of_expr[1];
        v_operands_of_rt_shift[1] = createSymbol(constraint_symbol_label, typeinfo_int);

        TypeOfExpressionTuple typeinfo_b = {TYPE_UNSIGNED_BITVECTOR, getWidth(operands_of_expr[1])};
        t_Expression* b_rt_shift_i = createExpression(m_operatorLabelRightShift, v_operands_of_rt_shift, typeinfo_b);

        t_Expression* b_rt_shift_i_not_eq_0 = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalInEquality, b_rt_shift_i, createIntegerConstant(0, typeinfo_b.m_width));

        vector<t_Expression*> v_operands_of_i_plus_1(2, NULL);
        v_operands_of_i_plus_1[0] = v_operands_of_rt_shift[1];
        string integer_const_1 = "1";
        v_operands_of_i_plus_1[1] = createConstant(integer_const_1, typeinfo_int);
        t_Expression* i_plus_1 = createExpression(m_operatorLabelADD, v_operands_of_i_plus_1, typeinfo_int);

        vector<t_Expression*> v_operands_of_b_rt_i_plus_1(2, NULL);
        v_operands_of_b_rt_i_plus_1[0] = operands_of_expr[1];
        v_operands_of_b_rt_i_plus_1[1] = i_plus_1;
        t_Expression* b_rt_i_plus_1 = createExpression(m_operatorLabelRightShift, v_operands_of_b_rt_i_plus_1, typeinfo_b);

        t_Expression* b_rt_i_plus_1_eq_0 = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, b_rt_i_plus_1, createIntegerConstant(0, typeinfo_b.m_width));

        t_Expression* final_constraint_expr = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, b_rt_shift_i_not_eq_0, b_rt_i_plus_1_eq_0);

        m_mapOfConstraintNameAndConstraintExpressions[constraint_symbol_label] = final_constraint_expr; // store the constraint expression in map
        return v_operands_of_rt_shift[1]; //return that constraint label and at last print existentially quantified constraint.
    }
    return NULL;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForUnaryOperators(t_Expression* expr, int ub, int lb)
{
    vector<t_Expression*> v_operands_of_expr = getVectorOfOperands(expr);
    t_Expression* operand_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[0], getWidth(v_operands_of_expr[0]) - 1, 0);

    string label = getLabelOfExpression(expr);
    t_Expression* result_invalid_bit = NULL;
    if (label == m_operatorLabelBitwiseNEG || label == m_operatorLabelRedAND || label == m_operatorLabelRedNAND || label == m_operatorLabelRedOR || label == m_operatorLabelRedNOR ||
            label == m_operatorLabelRedXOR || label == m_operatorLabelRedXNOR || label == m_operatorLabelLogNOT)
    {
        result_invalid_bit = operand_invalid_bit;
    }

    return result_invalid_bit;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOnPartOfSUBExpression(t_Expression* expr, int ub, int lb, bool return_carry_invalid_bit)
{
    vector<t_Expression*> v_operands_of_expr = getVectorOfOperands(expr);
    t_Expression* first_operand_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[0], ub, lb);
    t_Expression* second_operand_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[1], ub, lb);

    t_Expression* temp_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, first_operand_invalid_bit, second_operand_invalid_bit);

    if (lb == 0 && return_carry_invalid_bit == false)
    {
        return temp_invalid_bit;
    }
    else if (lb == 0 && return_carry_invalid_bit == true)
    {
        t_Expression* first_operand_atom = createASelectExpressionOnExpression(v_operands_of_expr[0], ub, lb);
        t_Expression* second_operand_atom = createASelectExpressionOnExpression(v_operands_of_expr[1], ub, lb);
        t_Expression* redor_second_operand_without_invalid_bit = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, second_operand_atom);
        t_Expression* redor_second_operand_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, redor_second_operand_without_invalid_bit, second_operand_invalid_bit);
        t_Expression* bitand_of_redor_second_operand_with_invalid_bit_and_temp_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, redor_second_operand_with_invalid_bit, temp_invalid_bit);
        t_Expression* redand_first_operand_atom = createOneBitExpressionWithOneOperand(m_operatorLabelRedAND, first_operand_atom);
        t_Expression* negation_first_operand_invalidbit = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, first_operand_invalid_bit);
        t_Expression* bitwise_and_of_above_two = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, redand_first_operand_atom, negation_first_operand_invalidbit);
        t_Expression* negation_of_above_one = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, bitwise_and_of_above_two);
        t_Expression* final_carry_expression = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, negation_of_above_one, bitand_of_redor_second_operand_with_invalid_bit_and_temp_invalid_bit);
        return final_carry_expression;

    }
    else
    {
        t_Expression* prev_carry_invalid_bit = generateInvalidBitExpressionOnPartOfSUBExpression(expr, lb - 1, 0, true);
        setInvalidBitExpression(expr, ub, lb, CARRY_KEY, prev_carry_invalid_bit);
        t_Expression* result_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, temp_invalid_bit, prev_carry_invalid_bit);
        return result_invalid_bit;
    }

}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOnPartOfADDExpression(t_Expression* expr, int ub, int lb, bool return_carry_invalid_bit)
{
    vector<t_Expression*> v_operands_of_expr = getVectorOfOperands(expr);
    t_Expression* first_operand_atom_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[0], ub, lb);
    t_Expression* second_operand_atom_invalid_bit = generateInvalidBitExpressionOfSelectExpression(v_operands_of_expr[1], ub, lb);

    t_Expression* temp_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, first_operand_atom_invalid_bit, second_operand_atom_invalid_bit);
    if (lb == 0 && return_carry_invalid_bit == false)
    {
        return temp_invalid_bit;
    }
    else if (lb == 0 && return_carry_invalid_bit == true) //Compute carry invalid bit
    {
        t_Expression* first_operand_atom = createASelectExpressionOnExpression(v_operands_of_expr[0], ub, lb);
        t_Expression* second_operand_atom = createASelectExpressionOnExpression(v_operands_of_expr[1], ub, lb);
        t_Expression* redor_first_operand_without_invalid_bit = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, first_operand_atom);
        t_Expression* redor_second_operand_without_invalid_bit = createOneBitExpressionWithOneOperand(m_operatorLabelRedOR, second_operand_atom);
        t_Expression* redor_first_operand_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, redor_first_operand_without_invalid_bit, first_operand_atom_invalid_bit);
        t_Expression* redor_second_operand_with_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, redor_second_operand_without_invalid_bit, second_operand_atom_invalid_bit);
        t_Expression* redor_first_second_operands_with_ivs = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, redor_first_operand_with_invalid_bit, redor_second_operand_with_invalid_bit);
        t_Expression* result_carry_invalid_bit = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseAND, temp_invalid_bit, redor_first_second_operands_with_ivs);
        return result_carry_invalid_bit;
    }
    else
    {
        t_Expression* previous_carry_invalid_bit = generateInvalidBitExpressionOnPartOfADDExpression(expr, lb - 1, 0, true);
        setInvalidBitExpression(expr, ub, lb, CARRY_KEY, previous_carry_invalid_bit);
        t_Expression* redor_of_prev_carry_and_current_atoms_ivs = createOneBitExpressionWithTwoOperands(m_operatorLabelBitwiseOR, temp_invalid_bit, previous_carry_invalid_bit);
        return redor_of_prev_carry_and_current_atoms_ivs;
    }
}


pair< vector<t_MemoryBounds>, int> t_ExpressionManager::getBoundsOfArray(t_Expression* array_expr, int &baseWidth)
{
    string label = getLabelOfExpression(array_expr);
    try
    {

        if (label == "update")
        {
            vector<t_Expression*> operands = getVectorOfOperands(array_expr);
            return getBoundsOfArray(operands[0], baseWidth);
        }
        else if (label == "read")
        {
            vector<t_Expression*> operands = getVectorOfOperands(array_expr);
            pair< vector<t_MemoryBounds>, int> result = getBoundsOfArray(operands[0], baseWidth);
            result.second++;
            return result;
        }
        else if (label == "ite")
        {
            vector<t_Expression*> operands = getVectorOfOperands(array_expr);
            //pair< vector<t_MemoryBounds>, int> result = getBoundsOfArray(operands[1]);
            //return result;
            if (getTypeOfExpressionWithOutWidth(operands[1]) == TYPE_MEMORY_ARRAY)
            {
                return getBoundsOfArray(operands[1], baseWidth);
            }
            else if (getTypeOfExpressionWithOutWidth(operands[2]) == TYPE_MEMORY_ARRAY)
            {
                return getBoundsOfArray(operands[2], baseWidth);
            }
            else
            {
                throw runtime_error(string(__FILE__":") + __LINE__ + ": Ite operands are not memory. ID of expression:");
            }
        }
        else if (label == "zeroextn")
        {
            vector<t_Expression*> operands = getVectorOfOperands(array_expr);
            //pair< vector<t_MemoryBounds>, int> result = getBoundsOfArray(operands[1]);
            //return result;
            if (getTypeOfExpressionWithOutWidth(operands[0]) == TYPE_MEMORY_ARRAY)
            {
                t_MemoryBounds temp;
                temp.m_upperbound = getConstantValuePresentInExpression(operands[1]) - 1;
                temp.m_lowerbound = 0;
                temp.m_packed = false;
                baseWidth = -1;
                return make_pair(vector<t_MemoryBounds > (1, temp), 0);
                // return getBoundsOfArray(operands[0])+;
            }
            else
            {
                throw runtime_error(string(__FILE__":") + __LINE__ + ": zeroxtn operand 0 is  not a memory. ID of expression:");
            }
        }
#ifdef USE_GEN_INV_FUNCTION
        else if (label == GEN_INV_LABEL)
        {
            vector<t_Expression*> operands = getVectorOfOperands(array_expr);
            assert(operands.size() == 1);
            return getBoundsOfArray(operands[0], baseWidth);
        }
#endif
#ifndef QE //#ifndef added by Ajith John on 9 June 2013
        else if (getTypeOfExpressionWithOutWidth(array_expr) == TYPE_MEMORY_ARRAY)
        {
            label = getActualSignalName(label);
            int num = 0;
            label = removeNewNamePrefixFromSignal(label, num);
            t_MemoryArrayBoundsInfo* info = getMemoryVariableDimensions(label);
            cout << "Getting memory for " << label << endl;
            //assert(info != NULL);
            if (info == NULL)
            {
                throw runtime_error(label + " does not have memory bounds.");
            }
            baseWidth = info->m_nBaseTypeWidth;
            return make_pair(info->m_bounds, 0);
        }
#endif //#endif added by Ajith John on 9 June 2013
        else //error
        {
            throw runtime_error(__FILE__":" + integerToString(__LINE__) + ": Not a memory:" + label);
        }
    }
    catch (runtime_error &re)
    {
        cerr << "Id of expression:" << array_expr->getID() << endl;
        printExpressionToFileAsDAG(integerToString(array_expr->getID()), array_expr, cerr, false);
        cerr << "---------------------" << endl;
        throw;
    }
}



pair<t_Expression*, t_Expression*> t_ExpressionManager::getNewSymbolLimitedToBound(int bound)
{
    typedef pair<t_Expression*, t_Expression*> exprPair;
    static int countSuffix = 1;
    static t_HashTable<int, exprPair> cache;

    t_HTStatusValue<exprPair> res = cache.hashtable_search(bound);
    if (res.success())
    {
        return res.getValue();
    }
    string temp = "__IndexSS";
    string label = temp + integerToString(countSuffix);
    ++countSuffix;
    int wd = (int) ceil(log2(bound));
    if (wd < 1) wd = 1;

    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, wd};
    t_Expression* index = createSymbol(label, typeInfo);

    t_Expression* constraint;
    {
        string label = integerToBinaryString(bound);
        t_Expression* constBound = createConstant(label, typeInfo);

        constraint = createOneBitExpressionWithTwoOperands(m_operatorLabelLessThanOrEqual, index, constBound);
    }

    cache.hashtable_insert(bound, make_pair(index, constraint));
    return make_pair(index, constraint);

}

string t_ExpressionManager::getActualSignalName(string label, bool removeBounds, bool removeInstanceNum, bool removeInternalPrefix)
{
    if (label.find(EXPRESSION_LABEL_AT_N_PREFIX) == 0)
        label = label.substr(string(EXPRESSION_LABEL_AT_N_PREFIX).length());

    if (label.find(EXPRESSION_AT_CYCLE_PREFIX) == 0)
        label = label.substr(string(EXPRESSION_AT_CYCLE_PREFIX).length());

    if (removeInternalPrefix && label.find(internal_signals_prefix) == 0)
        label = label.substr(string(internal_signals_prefix).length());


    //label = label.substr(0, label.rfind('_')); //remove instance num
    //label = label.substr(0, label.rfind('_')); //remove lb
    //label = label.substr(0, label.rfind('_')); //remove ub
    int ins, ub, lb;
    try
    {
        if (removeInstanceNum)
        {
            label = extractNumberAtEnd(label, ins);
        }
        if (removeBounds)
        {
            label = extractNumberAtEnd(label, lb);
            label = extractNumberAtEnd(label, ub);
        }
    }
    catch (std::logic_error &err)
    {
        //Do nothing, if number not found. (Possibly called before passII and instance number is not found.
    }
    return label;
}

t_Expression* t_ExpressionManager::createExpressionToEquateAllValuesForUpdateOnly(t_Expression* updExpr, int ub, int lb)
{
    string label = getLabelOfExpression(updExpr);
    if (label != "update")
    {
        /*if(label.find(EXPRESSION_LABEL_AT_N_PREFIX) == 0)
            label = label.substr(string(EXPRESSION_LABEL_AT_N_PREFIX).length());
        if(label.find(internal_signal_prefix) == 0)
            label = label.substr( string(internal_signal_prefix).length());
        
        label = label.substr(0, label.rfind('_'));      //remove instance num
        label = label.substr(0, label.rfind('_'));      //remove lb
        label = label.substr(0, label.rfind('_'));      //remove ub
        
        string symLabel = "inv
         */
        return generateInvalidBitOfReadOfArrayExpression(updExpr, ub, lb); //no update expression in read.
    }

    vector<t_Expression*> values;
    while (label == "update")
    {
        vector<t_Expression*> operands = getVectorOfOperands(updExpr);
        values.push_back(operands[2]);
        updExpr = operands[0];
        label = getLabelOfExpression(updExpr);
    }

    if (getTypeOfExpressionWithOutWidth(updExpr) != TYPE_MEMORY_ARRAY)
        throw runtime_error(__FILE__":" + integerToString(__LINE__) + ": Not a memory:" + label + ":" + integerToString(updExpr->getID()));

    t_Expression* allArrayValue;
    {
        string label = getLabelOfExpression(updExpr);

        {
            label = getActualSignalName(label);
        }
        string symLabel = "symbolic_values_in_" + label + "_" + integerToString(updExpr->getID()); //set to value of the elements in the array
        //i.e. if all the original values in the array are set to same value then this symbol is that value.
        // otherwise the symbol is unknown.
        //allArrayValue = createSymbol(symLabel, typeInfo);
        do
        {
            t_ExpressionValue* exprValue = values[0]->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
            if (exprValue == NULL || exprValue->m_typeInfo.m_type != TYPE_MEMORY_ARRAY)
            {
                //throw logic_error("Invalid expression passed.");
                //value is a simple value and not a memory. so create unknown value of that size.
                allArrayValue = createSymbol(symLabel, exprValue->m_typeInfo);
                break;
            }
            assert(exprValue != NULL);
            t_MemoryDataType* memBounds = exprValue->m_actualExpressionValueStored.exprvalueMemory;
            assert(memBounds != NULL);

            TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
            allArrayValue = createSymbol(symLabel, typeInfo);
            t_ExpressionValue* value = allArrayValue->getValue(m_expressionImplementationType, m_expressionImplementationManagerInstance);
            if (value == NULL)
            {
                cout << "************"__FILE__":" << __LINE__ << ": expression value not set for memory." << endl;
                break;
            }
            assert(value != NULL && value->m_typeInfo.m_type == TYPE_MEMORY_ARRAY);
            t_MemoryDataType* t = new t_ListOfMemoryEntries();
            t->m_bounds = new vector<t_MemoryBounds > (* memBounds->m_bounds);
            value->m_actualExpressionValueStored.exprvalueMemory = t;
            //now we have set the size for the symbol
        }
        while (0);
    }
    //    cout<<"created and pushed the "<<getLabelOfExpression(allArrayValue)<<endl;
    //    getchar();
    values.push_back(allArrayValue);

    vector<t_Expression*> valueInvBits(values.size(), NULL);

    for (int i = 0; i < values.size(); ++i)
    {
        t_Expression* expr = values[i];
        valueInvBits[i] = generateInvalidBitExpression(expr);
    }
    //valueInvBits.push_back(generateInvalidBitExpression(allArrayValuesEqual));

    t_Expression* orInvBits; //or of all invalid bits
    if (valueInvBits.size() > 1)
    {

        string label = m_operatorLabelLogOR;
        TypeOfExpressionTuple type = m_typeInfoOfInvalidBitExpr;
        orInvBits = createTwoOperandOperatorFromMultipleOperands(label, valueInvBits, type);
    }
    else
    {
        //cout<<"Only one value in the read expression"<<endl;
        orInvBits = valueInvBits[0];
    }

    t_Expression* allValuesEqual; //compare all the values
    {
        vector<t_Expression*> allValueCompare(values.size() - 1, NULL);
        t_Expression* firstVal = values[0];
        for (int i = 1; i < values.size(); ++i)
        {
            allValueCompare[i - 1] = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, firstVal, values[i]);
        }

        if (allValueCompare.size() >= 2)
            allValuesEqual = createTwoOperandOperatorFromMultipleOperands(m_operatorLabelLogAND, allValueCompare, m_typeInfoOfInvalidBitExpr); //one bit result of comparing all values
        else if (allValueCompare.size() == 1)
        {
            cout << "Only two values in the read expression" << endl;
            allValuesEqual = allValueCompare[0]; //only one comparison
        }
        else
        {
            assert(false && "Msg: Should not reach here.");
        }

    }

    //Now build the ite.
    t_Expression* ifAllEqual = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, allValuesEqual, createABitVectorConstantZeroExpression(), createABitVectorConstantOneExpression());
    //ie if all values equal then valid else invalid

    t_Expression* result = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, orInvBits, createABitVectorConstantOneExpression(), ifAllEqual);
    //i.e. if any value is invalid, then result is invalid else (if all values equal then valid else invalid)
    return result;

}

t_Expression* t_ExpressionManager::createExpressionToEquateAllValues(t_Expression* updExpr, int ub, int lb)
{
    //throw logic_error("Not yet Implemented.");
    vector<t_Expression*> values;
    string label = getLabelOfExpression(updExpr);
    while (label == "update")
    {
        vector<t_Expression*> operands = getVectorOfOperands(updExpr);
        values.push_back(operands[2]);
        updExpr = operands[0];
        label = getLabelOfExpression(updExpr);
    }
    int updateValuesUpto = values.size(); //These many values are the values of update. all the following values will be those in the array
    //updExpr is now a array expression
    //t_MemoryArrayBoundsInfo bounds = g
    if (getTypeOfExpressionWithOutWidth(updExpr) != TYPE_MEMORY_ARRAY)
        throw runtime_error(__FILE__":" + integerToString(__LINE__) + ": Not a memory:" + label);

    /*t_ExpressionValue* array_value = getValue(updExpr);
    if(array_value == NULL)
        throw runtime_error(__FILE__":"+integerToString(__LINE__)+": No value found for "+label);
    t_MemoryDataType* memData = array_value->m_actualExpressionValueStored.exprvalueMemory;      //get the memory value
    if(memData == NULL)
        throw runtime_error(__FILE__":"+integerToString(__LINE__)+": Memory Data not found in array value for "+label);
     */


    //t_MemoryArrayBoundsInfo boundsInfo = cdfg->getMemoryVariableDimensions()
    pair< vector<t_MemoryBounds>, int> arrBounds = getBoundsOfArray(updExpr);

    t_MemoryBounds bounds = arrBounds.first[arrBounds.second];
    /*if(0)
    {
        vector<t_MemoryBounds>::iterator it = memData->beginIteratorOnBounds();
        vector<t_MemoryBounds>::iterator end = memData->endIteratorOnBounds();
        
        int index = memData->getIndex();
        
        //Move forward to the current index to get the bounds;
        for(int i=0;i<index && it != end;++i)
            ++it;
        
        if(it == end)
            throw runtime_error(__FILE__":"+integerToString(__LINE__)+": Not enough bounds info");
        
        bounds = *(it);
    }**/

    //Now that we have bounds, we can create read expression for each of the index and get the values.
    int low = bounds.m_lowerbound;
    int high = bounds.m_upperbound;

    if (high < low)
    {
        cout << "Upper bound smaller than lower bound." << endl;
        swap(high, low);
    }

    for (; low <= high; ++low)
    {
        TypeOfExpressionTuple type = {TYPE_UNSIGNED_INTEGER, -1};
        string label = integerToString(low);
        t_Expression* index = createConstant(label, type);
        t_Expression* read_expr;
        {
            label = "read";
            TypeOfExpressionTuple type = {TYPE_MEMORY_ARRAY, -1};
            vector<t_Expression*> readOper = buildVector(updExpr, index);
            read_expr = createExpression(label, readOper, type);
        }
        values.push_back(read_expr);
    }

    //Now we have all the values in the update expression and the array in the values vector.
    //Now generate invalid bits for all these values.
    vector<t_Expression*> valueInvBits(values.size(), NULL);

    for (int i = 0; i < values.size(); ++i)
    {
        t_Expression* expr = values[i];
        if (i < updateValuesUpto)
        {
            cout << "selecting inv bit:" << getLabelOfExpression(expr) << endl;
            //valueInvBits[i] = generateInvalidBitExpressionOfSelectExpression(expr, ub, lb);
            valueInvBits[i] = generateInvalidBitExpression(expr);
        }
        else
        {
            cout << "reading inv bit:" << getLabelOfExpression(expr) << endl;
            valueInvBits[i] = generateInvalidBitOfReadOfArrayExpression(expr, ub, lb);
        }
    }

    //Now we have all the invalid bits generated.

    t_Expression* orInvBits; //or of all invalid bits
    if (valueInvBits.size() > 1)
    {

        string label = m_operatorLabelLogOR;
        TypeOfExpressionTuple type = m_typeInfoOfInvalidBitExpr;
        orInvBits = createTwoOperandOperatorFromMultipleOperands(label, valueInvBits, type);
    }
    else
    {
        cout << "Only one value in the read expression" << endl;
        return valueInvBits[0]; //Otherwise the result is the same as the invalid bit of the single value.
    }

    t_Expression* allValuesEqual; //compare all the values
    {
        vector<t_Expression*> allValueCompare(values.size() - 1, NULL);
        t_Expression* firstVal = values[0];
        for (int i = 1; i < values.size(); ++i)
        {
            allValueCompare[i - 1] = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, firstVal, values[i]);
        }

        if (allValueCompare.size() > 2)
            allValuesEqual = createTwoOperandOperatorFromMultipleOperands(m_operatorLabelLogAND, allValueCompare, m_typeInfoOfInvalidBitExpr); //one bit result of comparing all values
        else
        {
            cout << "Only two values in the read expression" << endl;
            allValuesEqual = allValueCompare[0]; //only one comparison
        }
    }

    //Now build the ite.
    t_Expression* ifAllEqual = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, allValuesEqual, createABitVectorConstantZeroExpression(), createABitVectorConstantOneExpression());
    //ie if all values equal then valid else invalid

    t_Expression* result = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, orInvBits, createABitVectorConstantOneExpression(), ifAllEqual);
    //i.e. if any value is invalid, then result is invalid else (if all values equal then valid else invalid)
    return result;

}

t_Expression* t_ExpressionManager::generateInvalidBitOfReadOfArrayExpression(t_Expression* array_expr, t_Expression* readIndex, int ub, int lb)
{
    //throw logic_error(__FILE__":"+integerToString(__LINE__)+":Not yet implemented.");
#ifdef MAKE_CYCLIC_MEMORY_INVALID
    {
        if (getLabelOfExpression(array_expr).find(EXPRESSION_AT_CYCLE_PREFIX) == 0)
        {
            return createABitVectorConstantOneExpression();
        }
    }
#endif
#ifdef USE_GEN_INV_FUNCTION
    {
        static string label = "read"; //reuse the same label, no need to create again for each call
        static vector<t_Expression*> operands(2, NULL);

        operands[0] = array_expr;
        operands[1] = readIndex;
        TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
        return generateInvalidBitOfReadOfArrayExpression(createExpression(label, operands, typeInfo), ub, lb);
    }
#else
    {
        ofstream temp("UpdateSimplify", ios::app);
        temp << "Generating invalid bit symbol of read expression:" << endl;
        string label = "READ OF";
        printExpressionToFileAsDAG(label, array_expr, &temp);
        label = "READ AT";
        printExpressionToFileAsDAG(label, readIndex, &temp);
        //assert(false && "Should not reach here: invalid bit of read");
        //return createABitVectorConstantZeroExpression();
    }
    string invLabel = "read_" + integerToString(array_expr->getID()) + "_" + integerToString(readIndex->getID()) + EXPRESSION_INVALID_BIT_LABEL;
    return createSymbol(invLabel, m_typeInfoOfInvalidBitExpr);
#endif
}

t_Expression* t_ExpressionManager::generateInvalidBitOfReadOfArrayExpression(t_Expression* read_expr, int ub, int lb)
{
    //throw logic_error(__FILE__":"+integerToString(__LINE__)+":Not yet implemented.");
    if (getLabelOfExpression(read_expr) != "read") //read was optimized
    {
        //return generateInvalidBitExpressionOfSelectExpression(read_expr, ub, lb);     //since no valid ub,lb for memories is passed.
        assert(getLabelOfExpression(read_expr) != "update"); //all updates should have been handles explicitly
        return generateInvalidBitExpression(read_expr);
    }
#ifdef MAKE_CYCLIC_MEM_INVALID
    {
        t_Expression* op0 = getVectorOfOperands(read_expr).at(0);
        if (getLabelOfExpression(op0).find(EXPRESSION_AT_CYCLE_PREFIX) == 0)
        {
            return createABitVectorConstantOneExpression();
        }
    }
#endif
    //otherwise, leave it as a symbol
#ifdef USE_GEN_INV_FUNCTION
    //generate a new operator genInv which will be printed as uninterpreted function in the SMT output
    vector<t_Expression*> operands(1, read_expr);
    string label = GEN_INV_LABEL;
    TypeOfExpressionTuple typeInfo = {TYPE_UNSIGNED_BITVECTOR, 1};
    return createExpression(label, operands, typeInfo);
#else
    vector<t_Expression*> operands = getVectorOfOperands(read_expr);
    return generateInvalidBitOfReadOfArrayExpression(operands[0], operands[1], ub, lb);
    //    static unsigned count = 0;
    //    string invLabel = "read_"+integerToString(operands[0]->getID()) + "_" + integerToString(operands[1]->getID()) + EXPRESSION_INVALID_BIT_LABEL;
    //    return createSymbol(invLabel, m_typeInfoOfInvalidBitExpr);
    //return NULL;
#endif
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOfReadOfUpdateExpression(t_Expression* updExpr, t_Expression* readIndex, int ub, int lb)
{
    if (getLabelOfExpression(updExpr) != "update")
    {
        //we found the array expression. now return invalid bit of read(array, readIndex)
        //throw logic_error(__FILE__":"+integerToString(__LINE__)+": Not yet Implemented");
        return generateInvalidBitOfReadOfArrayExpression(updExpr, readIndex, ub, lb);
    }

    //else found an update expression
    vector<t_Expression*> operands = getVectorOfOperands(updExpr);

    t_Expression* invIndex = generateInvalidBitExpression(operands[1]);
    if (operands[1] == t_Expression::unknownIndexExpression) //if index is 'X' added by update simplification for unknown index.
    {
        //return generateInvalidBitExpressionOfReadOfUpdateExpression(operands[0],readIndex,ub,lb);
        //there's no point of ite as we know the index is invalid
        //and we can return valid if all values equal.
        return createExpressionToEquateAllValuesForUpdateOnly(updExpr, ub, lb);
    }

    t_Expression* thenPart;
    if (t_ConfigurationOptions::getConfig()->invBitGenerationLevel bitand invRead_updateIndexInvalid)
    {
        pair < t_Expression*, bool> result = m_dummyExpression->modifyUpdateForUnknownIndex(operands[0], operands[2], m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
        t_Expression* newInnerUpdExpr = result.first; //result.second is unused.
        //    cout<<"New Update expression created."<<endl;
        //    printExpressionToFileAsDAG("symbol",updExpr, cout);
        //    cout<<endl;
        //    printExpressionToFileAsDAG("New",newInnerUpdExpr, cout);
        //    cout<<"Press any key to continue..."<<endl;
        //    getchar();
        //New update expression assuming that the current index is invalid.
        thenPart = generateInvalidBitExpressionOfReadOfUpdateExpression(newInnerUpdExpr, readIndex, ub, lb); //recursive call
    }
    else
    {
        thenPart = createABitVectorConstantOneExpression();
    }

    t_Expression* elsePart;
    {
        //else part i.e. ite(i==j,...)
        t_Expression* cond = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, operands[1], readIndex);
        t_Expression* then = generateInvalidBitExpression(operands[2]);

        elsePart = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, cond, then,
                //recursive call
                generateInvalidBitExpressionOfReadOfUpdateExpression(operands[0], readIndex, ub, lb));
    }

    return createOneBitExpressionWithThreeOperands(m_operatorLabelIte,
            createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, invIndex, createABitVectorConstantOneExpression()),
            thenPart, elsePart);
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOfReadOfITEExpression(t_Expression* arrayExpr, t_Expression* readIndex, int ub, int lb, TypeOfExpressionTuple &typeInfoOfRead)
{
    assert(getLabelOfExpression(arrayExpr) == "ite");
    assert(getTypeOfExpressionWithOutWidth(arrayExpr) == TYPE_MEMORY_ARRAY);
    cout << "Read of ite found. Press any key to continue..." << endl;
    //getchar();

    vector<t_Expression*> operands = getVectorOfOperands(arrayExpr);

    assert(getTypeOfExpressionWithOutWidth(operands[1]) == getTypeOfExpressionWithOutWidth(operands[2]));
    assert(getWidth(operands[1]) == getWidth(operands[2]));


    if (getTypeOfExpressionWithOutWidth(operands[1]) != TYPE_MEMORY_ARRAY &&
            getTypeOfExpressionWithOutWidth(operands[2]) != TYPE_MEMORY_ARRAY)
    {
        return generateInvalidBitExpression(arrayExpr);
    }

    assert(getTypeOfExpressionWithOutWidth(operands[1]) == TYPE_MEMORY_ARRAY);
    assert(getTypeOfExpressionWithOutWidth(operands[2]) == TYPE_MEMORY_ARRAY);
    //inv( read( ite(clk,A,B), index)  =>
    //inv( ite(clk,read(A,index),read(B,index)) )

    //TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(arrayExpr);

    t_Expression* condition = operands[0];
    //t_Expression* thenPart = generateInvalidBitExpressionOfReadExpression(operands[1],readIndex,ub,lb);
    //t_Expression* elsePart = generateInvalidBitExpressionOfReadExpression(operands[2],readIndex,ub,lb);
    t_Expression* thenPart;
    {
        string label = "read";
        vector<t_Expression*> oper = buildVector(operands[1], readIndex);
        thenPart = createExpression(label, oper, typeInfoOfRead);
    }

    t_Expression* elsePart;
    {
        string label = "read";
        //TypeOfExpressionTuple typeInfo = {TYPE_MEMORY_ARRAY, -1};
        vector<t_Expression*> oper = buildVector(operands[2], readIndex);
        elsePart = createExpression(label, oper, typeInfoOfRead);
    }
    //string label = "ite";
    //return createOneBitExpressionWithThreeOperands(label, condition,thenPart,elsePart);

    t_Expression* iteExpr;
    {
        string label = "ite";
        vector<t_Expression*> oper = buildVector(condition, thenPart, elsePart);
        iteExpr = createExpression(label, oper, typeInfoOfRead);
    }

    return generateInvalidBitExpression(iteExpr);
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOfReadExpression(t_Expression* expr, int ub, int lb)
{
    //See MEMORY_SIMPLIFY.TXT for algorithm to generate the invalid bit of a read expression.

    {
        ofstream temp("UpdateSimplify", ios::app);
        temp << "Generating invalid bit of read expression:" << endl;
        string label = "READ";
        printExpressionToFileAsDAG(label, expr, &temp);
        //assert(false && "Should not reach here: invalid bit of read");
        //return createABitVectorConstantZeroExpression();
    }

    assert(expr != NULL);
    assert(getLabelOfExpression(expr) == "read");
    assert(getTypeOfExpressionWithOutWidth(expr) == TYPE_MEMORY_ARRAY);

    vector<t_Expression*> operands = getVectorOfOperands(expr);

    assert(operands[0] != NULL);
    assert(operands[1] != NULL);
    TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(expr);
    {
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == SYMBOL &&
                getLabelOfExpression(operands[0]).find(EXPRESSION_LABEL_AT_N_PREFIX) != 0)
        {
            string replaceBy = EXPRESSION_LABEL_AT_N_PREFIX + getLabelOfExpression(operands[0]);
            t_Expression* replaceByExpr = createSymbol(replaceBy, typeInfo);
            replaceLeafOfAExpressionByExpression(operands[0], replaceByExpr);
            operands[0] = replaceByExpr;
        }
    }
    return generateInvalidBitExpressionOfReadExpression(operands[0], operands[1], ub, lb, typeInfo);
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionOfReadExpression(t_Expression* arrayExpr, t_Expression*indexExpr, int ub, int lb, TypeOfExpressionTuple &typeInfoOfRead)
{
    if (getLabelOfExpression(arrayExpr) == "ite")
    {
        return generateInvalidBitExpressionOfReadOfITEExpression(arrayExpr, indexExpr, ub, lb, typeInfoOfRead);
    }
    else if (getLabelOfExpression(arrayExpr) == "concat")
    {
        return createOneBitExpressionWithTwoOperands(m_operatorLabelLogOR,
                generateInvalidBitExpression(arrayExpr),
                generateInvalidBitExpression(indexExpr)
                );
    }
    else
    {
        assert(getTypeOfExpressionWithOutWidth(arrayExpr) == TYPE_MEMORY_ARRAY);
    }
    t_Expression* invIndex = generateInvalidBitExpression(indexExpr);


    t_Expression* exprForInvIndex; //Should be 'all values in the array and update expression equal' then inv of that value else, unknown
    if (t_ConfigurationOptions::getConfig()->invBitGenerationLevel bitand invRead_indexInvalid)
    {
        //assume we build the exprForInvIndex
        //throw logic_error(__FILE__":"+integerToString(__LINE__)+": Not implemented");
        cout << "Starting step1 " << ub << " " << lb << endl;
        exprForInvIndex = createExpressionToEquateAllValuesForUpdateOnly(arrayExpr, ub, lb);
        cout << "Step 1 complete" << endl;
    }
    else
    {
        exprForInvIndex = createABitVectorConstantOneExpression();
    }


    t_Expression* elseIteExpression = generateInvalidBitExpressionOfReadOfUpdateExpression(arrayExpr, indexExpr, ub, lb); //expression for the else part of invIndex
    cout << "step 2 complete" << endl;
    t_Expression* invIndexEq1 = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, invIndex, createABitVectorConstantOneExpression());
    cout << "step 3 complete" << endl;
    //Now build the ite expression
    //vector<t_Expression*> iteOperands = buildVector(invIndexEq1 , exprForInvIndex, elseIteExpression);
    return createOneBitExpressionWithThreeOperands(m_operatorLabelIte, invIndexEq1, exprForInvIndex, elseIteExpression);
    //cout<<"Read expression invalid bit generation not fully Implemented:"__FILE__":"<<__LINE__<<endl;
    //return createABitVectorConstantZeroExpression();//Assume read expression to be valid Only for testing
}

/**
 * Precondition: Must be called only on leaf expression or a concat expression (A symbol with atoms)
 */
vector<t_Expression*> t_ExpressionManager::getAtomsBetweenUbAndLb(t_Expression* expr, int ub, int lb)
{
    vector<t_Expression*> v_atoms;
    if (expr == NULL || ub < lb)
    {
        t_Logger* logManager = t_Logger::getInstance();
        logManager->LOG("ERROR !! ExpressionManager: Select expression with inverted bounds found " + toString(expr->getID()), m_logFile, c_NoMessageLevelVerbosity);
        assert(expr != NULL);
        assert(ub >= lb);
        return v_atoms;
    }
    else
    {
        string expr_label = getLabelOfExpression(expr);
        int expr_width = getWidth(expr);
        if (getTypeOfExpressionWithOutWidth(expr) == TYPE_UNSIGNED_INTEGER ||
                getTypeOfExpressionWithOutWidth(expr) == TYPE_SIGNED_INTEGER)
            expr_width = 32; //32 bit integer
        if (false && expr_width < ub)
        {
            cout << "Patch Activated..."__FILE__":" << __LINE__ << ":" << __FUNCTION__ << endl;
            if (isALeafExpression(expr))
                v_atoms.push_back(expr);
            else if (expr_label == m_operatorLabelConcat || expr_label == m_operatorLabelLefttoRightStream)
            {
                vector<t_Expression*> v_concat_operands = getVectorOfOperands(expr);
                for (vector<t_Expression*>::iterator it = v_concat_operands.begin(),
                        end = v_concat_operands.end(); it != end; it++)
                {
                    v_atoms.push_back(*it);
                }
            }
            else
                assert(false);

            assert(!v_atoms.empty());
            return v_atoms;
        }

        {
            if (expr_width <= lb)
            {
                cout << "Patch Activated:"__FILE__":" << __LINE__ << endl;
                string label(ub - lb + 1, '0');
                TypeOfExpressionTuple type = {TYPE_UNSIGNED_BITVECTOR, ub - lb + 1};
                v_atoms.push_back(createConstant(label, type));
                assert(!v_atoms.empty());
                return v_atoms;
            }
            if (expr_width <= ub)
            {
                cout << "Patch2 Activated..."__FILE__":" << __LINE__ << endl;
                ub = min(expr_width - 1, ub);
                lb = min(ub, lb);
            }
        }

        if (isALeafExpression(expr) == true)
        {
            t_Expression* atom_with_ub_and_lb = createASelectExpressionOnExpression(expr, ub, lb);
            v_atoms.push_back(atom_with_ub_and_lb);
        }
        else if (expr_label == m_operatorLabelConcat || expr_label == m_operatorLabelLefttoRightStream)
        {
#ifdef OLD
            assert(isALeafExpression(expr) == false);
            t_Expression* concat_expr = m_dummyExpression->createASelectOverConcat(expr, ub, lb, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator);
            assert(concat_expr != NULL);
            if (isALeafExpression(concat_expr) == true)
            {
                v_atoms.push_back(concat_expr);
            }
            else
            {
                vector<t_Expression*> v_concat_operands = getVectorOfOperands(concat_expr);
                for (vector<t_Expression*>::iterator it = v_concat_operands.begin(),
                        end = v_concat_operands.end(); it != end; it++)
                {
                    v_atoms.push_back(*it);
                }
            }
#else
            int width = 0;
            vector<t_Expression*> operands = getVectorOfOperands(expr);
            for (int i = operands.size() - 1; i >= 0; i--)
            {
                int start = width;
                width += getWidth(operands[i]);
                if (lb >= width || ub < start)
                {
                    //not required
                }
                else
                    v_atoms.push_back(operands[i]);
            }
#endif
        }
        else if (expr_width - 1 == ub && lb == 0)
        {
            v_atoms.push_back(expr);
        }
        else
        {
            cout << " ERROR ! Received  a non leaf expression instead of Leaf Expression " << endl;
        }
        assert(!v_atoms.empty());
        return v_atoms;
    }
}

t_Expression* t_ExpressionManager::createASelectExpressionOnExpression(t_Expression* expr, int ub, int lb)
{

    {
        int wd = getWidth(expr);
        //        if(getTypeOfExpressionWithOutWidth(expr) == TYPE_UNSIGNED_INTEGER ||
        //            getTypeOfExpressionWithOutWidth(expr) == TYPE_SIGNED_INTEGER)
        //            wd = 32;//actual width of int

        if (wd != -1 && lb >= wd)
        {
            //Return a constant 0 of length ub-lb+1
            cout << "Patch "__FILE__":" << __LINE__ << ": activated" << endl;
            TypeOfExpressionTuple type = {TYPE_UNSIGNED_BITVECTOR, ub - lb + 1};
            string label(ub - lb + 1, '0');
            return createConstant(label, type);
        }
        else if (wd != -1 && ub >= wd)
        {
            cout << "Patch "__FILE__":" << __LINE__ << ": activated" << endl;
            ub = max(min(ub, wd - 1), 0);
            lb = min(ub, lb);
        }
    }

    //string ub_label = toString(ub), lb_label = toString(lb);
    //TypeOfExpressionTuple type_int = {TYPE_UNSIGNED_INTEGER, -1};
    //t_Expression* ub_expr = createConstant(ub_label, type_int);
    //t_Expression* lb_expr = createConstant(lb_label, type_int);
    string ub_label = integerToBinaryString(ub);
    TypeOfExpressionTuple ub_type = {TYPE_UNSIGNED_BITVECTOR, ub_label.length()};
    t_Expression* ub_expr = createConstant(ub_label, ub_type);

    string lb_label = integerToBinaryString(lb);
    TypeOfExpressionTuple lb_type = {TYPE_UNSIGNED_BITVECTOR, lb_label.length()};
    t_Expression* lb_expr = createConstant(lb_label, lb_type);

    vector<t_Expression*> v_operands(3, NULL);
    v_operands[0] = expr;
    v_operands[1] = lb_expr;
    v_operands[2] = ub_expr;
    TypeOfExpressionTuple type_select = {TYPE_UNSIGNED_BITVECTOR, ub - lb + 1};
    t_Expression* select_expr = m_dummyExpression->createExpression(m_operatorLabelSelect, m_operatorLabelType, type_select, v_operands, m_expressionImplementationType, m_expressionImplementationManagerInstance, m_evaluator, true);
    return select_expr;
}

int t_ExpressionManager::getConstantValuePresentInExpression(t_Expression* expr)
{
    if (expr == NULL)
    {
        return -1;
    }
    return expr->getConstantValuePresentInExpression(m_expressionImplementationType, m_expressionImplementationManagerInstance);
}

string t_ExpressionManager::generateInvalidBitManagerKey(t_Expression* expr, int ub, int lb, string type)
{

    string key = toString(expr->getID()) + "#" + toString(ub) + "#" + toString(lb) + "#" + type;

    return key;
}

/**
 * Constraints generated during div and mod's invalid bit generation.
 * @param file_to_print
 * @return 
 */
bool t_ExpressionManager::printAllConstraintExpressions(ofstream* file_to_print)
{
    for (map<string, t_Expression*>::const_iterator it = m_mapOfConstraintNameAndConstraintExpressions.begin(),
            end = m_mapOfConstraintNameAndConstraintExpressions.end(); it != end; it++)
    {
        *file_to_print << "E" << " ." << (*it).first << endl;
        string expr_label = (*it).first;
        bool print_res = printExpressionToFileAsDAG(expr_label, (*it).second, file_to_print);
        if (print_res == false)
        {
            return false;
        }
        *file_to_print << endl;
    }
    return true;
}

t_Expression* t_ExpressionManager::generateInvalidBitExpressionForIteOperator(t_Expression* expr, int ub, int lb)
{
    t_Expression* weakInvExpr = generateWeakenedInvalidBitFromITE(expr);
    if (weakInvExpr != NULL)
    {
        return weakInvExpr;
    }
    vector<t_Expression*> operands_of_expr = getVectorOfOperands(expr);
    t_Expression* invalid_bit_condition = generateInvalidBitExpression(operands_of_expr[0]);
    t_Expression* invalid_bit_of_operand2 = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[1], ub, lb);
    t_Expression* invalid_bit_of_operand3 = generateInvalidBitExpressionOfSelectExpression(operands_of_expr[2], ub, lb);
    t_Expression* iv_ite_on_condition = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, operands_of_expr[0], invalid_bit_of_operand2, invalid_bit_of_operand3);

    t_Expression* part_of_op1 = createASelectExpressionOnExpression(operands_of_expr[1], ub, lb);
    t_Expression* part_of_op2 = createASelectExpressionOnExpression(operands_of_expr[2], ub, lb);
    t_Expression* operands_equal = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, part_of_op1, part_of_op2);
    t_Expression* both_valid = createOneBitExpressionWithTwoOperands(
            m_operatorLabelBitwiseAND,
            createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, invalid_bit_of_operand2),
            createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, invalid_bit_of_operand3)
            );
    operands_equal = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, both_valid, operands_equal);
    t_Expression* ite_on_operands_equal = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, operands_equal, createABitVectorConstantZeroExpression(), createABitVectorConstantOneExpression());

    t_Expression* iv_ite_part = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, invalid_bit_condition, ite_on_operands_equal, iv_ite_on_condition);
    //t_Expression* iv_ite_part = createOneBitExpressionWithThreeOperands(m_operatorLabelIte, invalid_bit_condition, createABitVectorConstantOneExpression(), iv_ite_on_condition);
    return iv_ite_part;
}

vector<t_Expression*> t_ExpressionManager::buildVector(t_Expression* first, t_Expression* second, t_Expression* third)
{
    vector<t_Expression*> vec;
    vec.push_back(first);
    if (second != NULL)
        vec.push_back(second);

    if (third != NULL)
        vec.push_back(third);
    return vec;
}

map<string, string>& t_ExpressionManager::generateMap(istream& input, map<string, string>& operatorMap, char fieldSeparator, char entrySeparator)
{
    string line;
    while (getline(input, line, entrySeparator))
    {
        if (line.find("//") != -1 || line.empty())
            continue; //skip comments and empty lines
        string::size_type idx = line.find(fieldSeparator);
        if (idx == string::npos)
            continue;
        string oper = line.substr(0, idx);
        string syntax = line.substr(idx + 1);

        trimInPlace(oper);
        trimInPlace(syntax);
        operatorMap.insert(make_pair(oper, syntax));
    }

    return operatorMap;
}
//Ideally we should not need this set. The visited flag in the expression should be enough
//This is used to indicate which declarations have been printed.
t_HashTable<unsigned long long int, bool> printedSet;
//set of printed uninterpreted functions
static t_HashTable<string, bool> printedFunctions;

bool t_ExpressionManager::printArrayHeader(ostream& out, t_Expression* expr, const string& prefix)
{
    if (getTypeOfExpressionWithOutWidth(expr) != TYPE_MEMORY_ARRAY)
        return false;

    string label;
    t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel == SYMBOL)
    {
        label = getLabelOfExpression(expr);
    }
    else if (typeLabel == OPERATOR)
    {
        label = prefix + expr->getID();
    }
        //    else if (typeLabel == CONSTANT && getLabelOfExpression(expr).find(UNKNOWN_MEM_SYMBOL) == 0)
        //    {
        //        label = getLabelOfExpression(expr);
        //    }
    else
    {
        assert("Case not handled." && false);
    }

    pair<vector<t_MemoryBounds>, int> bounds_res = getBoundsOfArray(expr);
    vector<t_MemoryBounds> &bounds = bounds_res.first;
    int start = bounds_res.second;

    if (start > bounds.size())
        throw std::out_of_range("Incorrect bound info for mem");

    out << "(declare-fun " << label << " ()";

    int bracCount = 1; //1 for declare-fun

    int size = bounds.size();
    if (t_ConfigurationOptions::getConfig()->m_lastDimensionBitVec && size != 1)
    {
        --size;
    }
    for (; start < size; start++, bracCount++)
    {
        int indexSize;
        {
            int ub = bounds[start].m_upperbound;
            indexSize = (int) ceil(log2(ub + 1));
        }

        out << " (Array (_ BitVec " << indexSize << ")";
    }
    //print base size
    if (t_ConfigurationOptions::getConfig()->m_lastDimensionBitVec && size != 1)
    {
        int wd = bounds.at(start).m_upperbound - bounds[start].m_lowerbound + 1;
        out << " (_ BitVec " << wd << ")";
    }
    else
    {
        out << " (_ BitVec 1)";
    }

    out << string(bracCount, ')') << "\n";
    return true;
}

ostream& t_ExpressionManager::printDagHeader(ostream& out, t_Expression* expr, const string& prefix)
{
    //if(isVisited(expr))
    if (printedSet.hashtable_search(expr->getID()).success())
    {
        //cout << "***** Already Declared:" << getLabelOfExpression(expr) << " " << getExpressionID(expr) << endl;
        return out;
    }
    printedSet.hashtable_insert(expr->getID(), true);
    t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel == SYMBOL /*|| (typeLabel == CONSTANT && getLabelOfExpression(expr).find(UNKNOWN_MEM_SYMBOL) == 0)*/)
    {
        if (printArrayHeader(out, expr, prefix) == false) //if not array
        {
            string label = getLabelOfExpression(expr);
            int width = getWidth(expr);
            if (width == 1)// commented out by sukanya
		#ifndef QE //#ifndef added by Ajith John on 24 July 2013
                    if (getTypeOfExpressionWithOutWidth(expr) == TYPE_BOOL)
                        out << "(declare-fun " << label << " () Bool)\n";
                    else
			out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
		#else
                    if (getTypeOfExpressionWithOutWidth(expr) == TYPE_BOOL)
                        out << "(declare-fun " << label << " () Bool)\n";
                    else
			out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
		#endif //#ifndef added by Ajith John on 24 July 2013 ends here
            else
                out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
            // debug
            // assert(label.compare("ctrl_ps_31_0_0_0")!=0);
            // debug
        }
    }
    else if (typeLabel == OPERATOR)
    {
        if (printArrayHeader(out, expr, prefix) == false)
        {
            string label = prefix + integerToString(getExpressionID(expr));
            int width = getWidth(expr);
            switch (getTypeOfOperator(expr))
            {
                case LOGICAL:
                case RELATIONAL:
                    out << "(declare-fun " << label << " () Bool)\n";
                    break;
                default:
                    out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
            }
        }
    }
    return out;
}

t_ExpressionManager::type_of_operator t_ExpressionManager::getTypeOfOperator(const string& label)
{
    static set<string> relational;
    if (relational.empty())
    {
#define ADD(x) relational.insert(#x)

        ADD(lt);
        ADD(gt);
        ADD(le);
        ADD(ge);
        ADD(logeq);
        ADD(diseq);
        ADD(wildeq);
        ADD(casexeq);
        ADD(casezeq);
        ADD(wilddiseq);

#undef ADD
    }

    if (relational.find(label) != relational.end())
        return RELATIONAL;
    if (label.find("log") == 0 && label.length()>3)
        return LOGICAL;
    if (label.find("bit") == 0 && label.length()>3)
        return BITWISE;
#ifdef QE //#ifdef added by Ajith John on 24 July 2013
    if (label == "eqz" || label == "diseqz" || label == "leq")
	return RELATIONAL;
    if (label == "kand")
	return LOGICAL;
#endif //#ifdef added by Ajith John on 24 July 2013 ends here
    return UNKNOWN;
}
//used to clear the visited flag of expressions that are generated by reduction and rightToLeftStream operators
//For operators like reduction Nor/xor/xnor ... new equivalent expressions need to be created while printing
//these expressions. These new expressions may or may not have appeared as sub expressions earlier.
//So createdIntermediateExpressions stores all such new expressions.
static vector<t_Expression*> createdIntermediateExpressions;
static t_HashTable<unsigned long long, bool> visitedExprs;

bool t_ExpressionManager::isVisited(t_Expression* expr)
{
    assert(expr != NULL);
    unsigned long long id = expr->getID();
    return visitedExprs.hashtable_search(id).success();
}

void t_ExpressionManager::setVisited(t_Expression* expr, bool value)
{
    assert(expr != NULL);
    expr->m_dagExpression->getDag()->assignVisitedFlag(value);
    unsigned long long id = expr->getID();
    if (value)
        visitedExprs.hashtable_insert(id, true);
    else
        visitedExprs.hashtable_delete(id);
}


// original method
ostream& t_ExpressionManager::printExpression(ostream&output, ostream&declarations, t_Expression* expr, map<string, string>&operatorMap, bool printNodeId)
{
    string label = getLabelOfExpression(expr);
    t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel != OPERATOR)
    {
        if (typeLabel == CONSTANT)
        {
            long int value;
            {
                t_ExpressionValue* expr_val = this->getValue(expr); //this->getValue( expressionImplementationType, expressionImplementationManagerInstance);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
                {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(expr) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
                {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(expr) << ")";
                }
                    //                else if( expr_val->getOnlyTypeOfExpression() == TYPE_MEMORY_ARRAY &&
                    //                        label.find(UNKNOWN_MEM_SYMBOL) == 0)
                    //                {
                    //                    printDagHeader(declarations, expr, "temp_node");
                    //                    output << label;
                    //                    setVisited(expr);
                    //                    return output;
                    //                }
                else //if(this->getTypeOfValueStoredInExpressionLabel(this, expressionImplementationType, expressionImplementationManagerInstance)==CONSTANT)
                {
                    string label = getLabelOfExpression(expr);

                    if (label.find_first_of("uxz") != string::npos)
                    {
                        //value = 0;
                        output<<"#b"<<string(label.length()-2, '0');
                    }
                    else
                    {
                        //skip 0b
                        //value = binaryStringToInteger(label.substr(2));
                        output<<"#b"<<label.substr(2);
                    }
//                    output<<"#b"<<label.substr(2)
                }
            }
            
        }
        else
        {
            //Symbol
            printDagHeader(declarations, expr, "temp_node");
            output << label;
        }
        setVisited(expr);
        return output;
    }

    if (isVisited(expr))
    {
        output << "temp_node" << getExpressionID(expr);
        return output;
    }
    //setVisited(expr);

    if (printNodeId)
    {
        output << "\n(= temp_node" << getExpressionID(expr) << " ";
        setVisited(expr);
    }

    //now the expr is an operator node.
    vector<t_Expression*> operands = getVectorOfOperands(expr);

    if (label == "concat" || label == "leftToRightStream" || label == "replic")
    {
        //special handling for concat.
        int bracCount = 0;
        vector<t_Expression*> opers;
        if (label == "replic") //Convert {4{in1}} to concat(in1,in1,in1,in1)
        {
            t_Expression* expr1 = operands.at(0);
            t_Expression* countExpr = operands.at(1);
            int count = getConstantValuePresentInExpression(countExpr);
            operands.clear();
            //operands.resize(count, expr1);
            operands.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                operands.push_back(expr1);
            }
            label = "concat";
        }

        for (int i = 0; i < operands.size() - 1; i++)
        {
            output << "(concat ";
            bracCount++;
            if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
            {
                output << "temp_node" << getExpressionID(operands[i]);
                opers.push_back(operands[i]);
                printDagHeader(declarations, operands[i], "temp_node");
            }
            else
            {
                printExpression(output, declarations, operands[i], operatorMap);
                //printDagHeader(declarations, operands[i], "temp_node");
            }
            output << " ";
        }

        int i = operands.size() - 1; //for last operand
        if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
        {
            output << "temp_node" << getExpressionID(operands[i]);
            opers.push_back(operands[i]);
            printDagHeader(declarations, operands[i], "temp_node");
        }
        else
        {
            //printDagHeader(declarations, operands[i], "temp_node");
            printExpression(output, declarations, operands[i], operatorMap);
        }

        for (int i = 0; i < bracCount; i++)
            output << ")";
        if (printNodeId)
            output << ")";
        output << endl;

        for (int i = 0; i < opers.size(); i++)
        {
            if (!isVisited(opers[i]))
            {
                printExpression(output, declarations, opers[i], operatorMap);
            }
        }

        return output;
    }
#ifdef QE //#ifdef added by Ajith John on 24 July 2013
    if (label == "kadd" || label == "kand")
    {
        //special handling for kadd and kand
        int bracCount = 0;
        vector<t_Expression*> opers;
        
        for (int i = 0; i < operands.size() - 1; i++)
        {
	    if(label == "kadd")
	    {
            	output << "(bvadd ";
	    }
	    else //label == "kand" 
	    {
		output << "(and ";
	    }

            bracCount++;
            if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
            {
                output << "temp_node" << getExpressionID(operands[i]);
                opers.push_back(operands[i]);
                printDagHeader(declarations, operands[i], "temp_node");
            }
            else
            {
                printExpression(output, declarations, operands[i], operatorMap);                
            }
            output << " ";
        }

        int i = operands.size() - 1; //for last operand
        if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
        {
            output << "temp_node" << getExpressionID(operands[i]);
            opers.push_back(operands[i]);
            printDagHeader(declarations, operands[i], "temp_node");
        }
        else
        {
            printExpression(output, declarations, operands[i], operatorMap);
        }

        for (int i = 0; i < bracCount; i++)
            output << ")";
        if (printNodeId)
            output << ")";
        
        for (int i = 0; i < opers.size(); i++)
        {
            if (!isVisited(opers[i]))
            {
                printExpression(output, declarations, opers[i], operatorMap);
            }
        }

        return output;
    }
    else if (label == "eqz" || label == "diseqz")
    {
        int wd = getWidth(operands[0]);

	if(label == "eqz")
	{
		output << "(= ";
	}
	else // label == "diseqz"
	{
		output << "(not (= ";
	}

        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
            output << "temp_node" << operands[0]->getID();
        else
            printExpression(output, declarations, operands[0], operatorMap);
        
	output << " ";
	output << "#b" << getZeroBinaryStringOfGivenLength(wd) ;

        if (printNodeId)
            output << ")";

	if(label == "eqz")
	{
        	output << ")";
	}
	else // label == "diseqz"
	{
		output << "))";
	}

        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR &&
                !isVisited(operands[0]))
        {
            printDagHeader(declarations, operands[0], "temp_node");
            printExpression(output, declarations, operands[0], operatorMap);
        }

        return output;
    }	
#endif //#ifdef added by Ajith John on 24 July 2013 ends here
    else if (label == "zeroextn" || label == "signextn")
    {
        int wd = getWidth(operands[0]);
        int finalWd = getConstantValuePresentInExpression(operands[1]);
        //((_ zero_extend i) t)
        string smtLabel = (label == "zeroextn") ? "zero_extend" : "sign_extend";
        output << "((_ "<<smtLabel <<" " << (finalWd - wd) << ") ";
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
            output << "temp_node" << operands[0]->getID();
        else
            printExpression(output, declarations, operands[0], operatorMap);
        if (printNodeId)
            output << ")";
        output << ")\n";

        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR &&
                !isVisited(operands[0]))
        {
            printDagHeader(declarations, operands[0], "temp_node");
            printExpression(output, declarations, operands[0], operatorMap, true);
        }

        return output;
    }
    else if (label == "rightToLeftStream")
    {

        // >>4{a,b} will be converted to (assume final size is 8 bits
        // concat( select( concat(a,b), 0, 3), select(concat(a,b), 4, 7) )
        //will perform select concat optimizations as necessary
        int wd = getWidth(expr);
        int N = getConstantValuePresentInExpression(operands[0]);
        t_Expression* innerConcat;
        {
            if (operands.size() == 2)
                innerConcat = operands[1];
            else
            {
                string label = "concat";
                TypeOfExpressionTuple type = getTypeOfExpressionTuple(expr);
                vector<t_Expression*> temp(operands.begin() + 1, operands.end());
                innerConcat = createExpression(label, temp, type);
            }
        }
        vector<t_Expression*> selects;
        for (int i = 0; i <= wd - N; i += N)
        {
            /*label  = "select";
            TypeOfExpressionTuple type = { getTypeOfExpressionWithOutWidth(expr), N};
            
            string ublabel = integerToBinaryString( i+N-1 );
            TypeOfExpressionTuple ubtype = {TYPE_UNSIGNED_BITVECTOR, ublabel.length() };
            t_Expression* ub = createConstant(ublabel, ubtype);
            
            string lblabel = integerToBinaryString( i );
            TypeOfExpressionTuple lbtype = {TYPE_UNSIGNED_BITVECTOR, lblabel.length() };
            t_Expression* lb = createConstant(lblabel, lbtype);
            
            vector<t_Expression*> sel_oper = buildVector(innerConcat, lb,ub);
            selects.push_back( createExpression(label, sel_oper, type) );*/
            cout << "----- RTOL -----Selecting from " << (i + N - 1) << " to " << i << "\n";
            selects.push_back(createASelectExpressionOnExpression(innerConcat, i + N - 1, i));
        }

        string con_label = "concat";
        TypeOfExpressionTuple type = getTypeOfExpressionTuple(expr);
        t_Expression* rToL_modified = createExpression(con_label, selects, type);
        //        cout<<endl;
        //        printExpressionToFileAsDAG("temp", rToL_modified, cout);
        //        cout<<endl;
        if (getTypeOfValueStoredInExpressionLabel(rToL_modified) != OPERATOR)
            printExpression(output, declarations, rToL_modified, operatorMap);
        else
            output << "temp_node" << getExpressionID(rToL_modified);
        if (printNodeId)
            output << ")";
        //output<<"(= temp_node"<<getExpressionID(expr)<<" temp_node"<<getExpressionID(rToL_modified)<<")"<<endl;
        output << "\n";
        if (getTypeOfValueStoredInExpressionLabel(rToL_modified) == OPERATOR &&
                !isVisited(rToL_modified))
        {
            createdIntermediateExpressions.push_back(rToL_modified);

            printDagHeader(declarations, rToL_modified, "temp_node");
            printExpression(output, declarations, rToL_modified, operatorMap, true);
        }
        return output;
    }
    else if(label == "redor")
    {
//#define NEW_EXPR_HAS_ID
#ifdef NEW_EXPR_HAS_ID //Old code
        int wd = getWidth(operands.at(0));
        string constLabel(wd,'0');
        TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(operands[0]);
        t_Expression* constExpr = createConstant(constLabel, typeInfo);
        
        t_Expression* e = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalInEquality, operands[0], constExpr);
        if (getTypeOfValueStoredInExpressionLabel(e) != OPERATOR)
            printExpression(output, declarations, e, operatorMap, false);
        else
            output << "temp_node" << getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            printDagHeader(declarations, e, "temp_node");
            printExpression(output, declarations, e, operatorMap, true);
        }
        return output;
#else
        output<<"(not (= ";
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) != OPERATOR)
            printExpression(output, declarations, operands[0], operatorMap, false);
        else
            output << "temp_node" << getExpressionID(operands[0]);
        
        output<<" #b"<<string(getWidth(operands[0]), '0')<<"))";
        if (printNodeId)
            output << ")";
        if (!isVisited(operands[0]) && getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
        {
            printDagHeader(declarations, operands[0], "temp_node");
            printExpression(output, declarations, operands[0], operatorMap, true);
        }
        return output;
#endif
    }
    else if(label == "redand")
    {
#ifdef NEW_EXPR_HAS_ID //Old code
        int wd = getWidth(operands.at(0));
        string constLabel(wd,'1');
        TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(operands[0]);
        t_Expression* constExpr = createConstant(constLabel, typeInfo);
        
        t_Expression* e = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, operands[0], constExpr);
        if (getTypeOfValueStoredInExpressionLabel(e) != OPERATOR)
            printExpression(output, declarations, e, operatorMap, false);
        else
            output << "temp_node" << getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            printDagHeader(declarations, e, "temp_node");
            printExpression(output, declarations, e, operatorMap, true);
        }
        return output;
#else
        output<<"(= ";
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) != OPERATOR)
            printExpression(output, declarations, operands[0], operatorMap, false);
        else
            output << "temp_node" << getExpressionID(operands[0]);
        
        output<<" #b"<<string(getWidth(operands[0]), '1')<<")";
        if (printNodeId)
            output << ")";
        
        if (!isVisited(operands[0]) && getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
        {
            printDagHeader(declarations, operands[0], "temp_node");
            printExpression(output, declarations, operands[0], operatorMap, true);
        }
        return output;
#endif
    }
    else if (/*label == "redor" || label == "redand" || */label == "redxor" || label == "rednor" || label == "rednand" || label == "redxnor")
    {
        vector<t_Expression*> selects;
        int wd = getWidth(operands.at(0));
        for (int i = 0; i < wd; i++)
        {
            t_Expression* sel = createASelectExpressionOnExpression(operands[0], i, i);
            /*t_Expression* sel;
            {
                string labelSel  = "select";
                TypeOfExpressionTuple type = { getTypeOfExpressionWithOutWidth(expr), 1};

                string ublabel = integerToBinaryString( i );
                TypeOfExpressionTuple ubtype = {TYPE_UNSIGNED_BITVECTOR, ublabel.length() };
                t_Expression* ub = createConstant(ublabel, ubtype);

                vector<t_Expression*> sel_oper = buildVector(operands[0], ub ,ub);
                sel = createExpression(labelSel, sel_oper, type);
            }*/
            selects.push_back(sel);
        }

        typeOfExpressionTuple type = {TYPE_UNSIGNED_BITVECTOR, 1};
        string new_label = std::string("bit") + label.substr(3); //convert redor->bitor, redand->bitand, ...
        t_Expression* e = createTwoOperandOperatorFromMultipleOperands(new_label, selects, type);
        //printExpressionToFileAsDAG("temp",e,cout);
        if (getTypeOfValueStoredInExpressionLabel(e) != OPERATOR)
            printExpression(output, declarations, e, operatorMap, false);
        else
            output << "temp_node" << getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            printDagHeader(declarations, e, "temp_node");
            printExpression(output, declarations, e, operatorMap, true);
        }
        return output;
    }
#if defined(USE_GEN_INV_FUNCTION) //&& defined(PRINT_GEN_INV_INTERNALLY)
    else if (label == GEN_INV_LABEL)
    {
        bool performInvalidBitSimplification = true;
        {
            t_Expression* readExpr = operands[0];
            if (getLabelOfExpression(readExpr) == "read")
            {
                vector<t_Expression*> read_operands = getVectorOfOperands(readExpr);
                assert(read_operands.size() == 2);
                if (getTypeOfValueStoredInExpressionLabel(read_operands[0]) != OPERATOR)
                {
                    //since the read is a simple read on memory we cannot simplify the invalid bit
                    //further. So keep it as it is.
                    performInvalidBitSimplification = false;
                }
            }
        }

        if (performInvalidBitSimplification)
        {
            t_Expression* inv_Bit = generateInvalidBitExpression(operands[0]);
            createdIntermediateExpressions.push_back(inv_Bit);
            if (getTypeOfValueStoredInExpressionLabel(inv_Bit) == OPERATOR)
            {
                output << "temp_node" << inv_Bit->getID();
            }
            else
            {
                printExpression(output, declarations, inv_Bit, operatorMap, false);
            }
            if (printNodeId)
                output << ")";
            output << "\n ;Here was gen-inv(" << operands[0]->getID() << ") simplified to " << inv_Bit->getID() << "\n";
            if (getTypeOfValueStoredInExpressionLabel(inv_Bit) == OPERATOR && !isVisited(inv_Bit))
            {
                printDagHeader(declarations, inv_Bit, "temp_node");
                printExpression(output, declarations, inv_Bit, operatorMap, true);
            }
            return output;
        }
        else
        {
            //#define OLD_GEN_INV_PRINT
#ifdef OLD_GEN_INV_PRINT
            output << "(gen-inv ";
            assert(getLabelOfExpression(operands[0]) == "read");
            operands[0]->m_dagExpression->getDag()->assignVisitedFlag(false); //make sure we print the expression.
            //printExpression(output, declarations, operands[0], operatorMap, false);//XXX: might fail if the index of read is an operator.
            t_Expression* printLater = NULL;
            if (true)
            {
                vector<t_Expression*> oper = getVectorOfOperands(operands[0]);
                output << "(select ";
                //print the memory variable name
                assert(getTypeOfValueStoredInExpressionLabel(oper[0]) != OPERATOR);
                printExpression(output, declarations, oper[0], operatorMap, false);
                output << " ";
                //print the index
                if (getTypeOfValueStoredInExpressionLabel(oper[1]) == OPERATOR)
                {
                    output << "temp_node" << oper[1]->getID();
                    printLater = oper[1];
                }
                else
                {
                    printExpression(output, declarations, oper[1], operatorMap, false);
                }
            }
            output << ")";
            if (printNodeId)
                output << ")";
            output << endl;
            if (printLater && !isVisited(printLater))
            {
                printDagHeader(declarations, printLater, "temp_node");
                printExpression(output, declarations, printLater, operatorMap, true);
            }
            return output;
#endif
#ifndef OLD_GEN_INV_PRINT
#ifndef QE //#ifndef added by Ajith John on 9 June 2013
            assert("read" == getLabelOfExpression(operands[0]));
            vector<t_Expression*> readOperands = getVectorOfOperands(operands[0]);
            assert(readOperands.size() == 2);
            string memName = getLabelOfExpression(readOperands[0]);
            string funcName = "gen-inv-" + memName;
            t_MemoryArrayBoundsInfo* bounds = cdfg->getMemoryVariableDimensions(getActualSignalName(memName, true, true));
            assert(bounds != NULL);
            assert(bounds->m_bounds.empty() == false);
            int upperBound = bounds->m_bounds[0].m_upperbound;
            int lowerBound = bounds->m_bounds[0].m_lowerbound;
            int memWidth = upperBound - lowerBound + 1;
            int indexWidth = static_cast<int> (ceil(log2(memWidth)));
            if (printedFunctions.hashtable_search(funcName).success() == false)
            {
                //add a declaration for the uninterpreted function
                declarations << "(declare-fun " << funcName << " ((_ BitVec " << indexWidth << ")) Bool)\n";
                printedFunctions.hashtable_insert(funcName, true);
            }
            output << "(" << funcName << " ";
            t_Expression*printLater = NULL;
            t_Expression*index = readOperands[1];
            if (getTypeOfValueStoredInExpressionLabel(index) == OPERATOR)
            {
                output << "temp_node" << index->getID();
                printLater = index;
            }
            else
            {
                printExpression(output, declarations, index, operatorMap, false);
            }
            output << ")";
            if (printNodeId)
                output << ")";
            output << "\n";

            if (printLater && !isVisited(printLater))
            {
                printDagHeader(declarations, printLater, "temp_node");
                printExpression(output, declarations, printLater, operatorMap, true);
            }
            return output;
#endif //#endif added by Ajith John on 9 June 2013
#endif
        }
        //else fall through to the SMT OPerator map printing which will print the normal expression.
    }
#endif


    map<string, string>::iterator it = operatorMap.find(label);
    if (it == operatorMap.end())
    {
        cerr << "WARNING!!! No operator labelled " << label << " found." << endl;
        cerr << "size of map is " << operatorMap.size() << endl;
        assert(it != operatorMap.end());
    }
    const string &syntax = it->second;

    vector<t_Expression*> opers;

    for (int i = 0; i < syntax.length(); i++)
    {
        if (syntax[i] == '$')
        {
            /*if(syntax[i+1] == '$') //if $$ print all operands separated by ' '
            {
                for(int j=0;j<operands.size();j++)
                {
                    printExpression(output, operands[j], operatorMap);
                    output<<" ";
                }
                i++;
            }
            else */
            if (syntax[i + 1] == 'm') //if $m0 print operands[0] as constant memory
            {
                int operNum = syntax[i + 2] - '0';
                assert(operNum >= 0 && operNum <= 9);

                if (true || getTypeOfValueStoredInExpressionLabel(operands[operNum]) != CONSTANT ||
                        getTypeOfExpressionWithOutWidth(operands[operNum]) == TYPE_MEMORY_ARRAY)
                {
                    if (getTypeOfValueStoredInExpressionLabel(operands[operNum]) == OPERATOR)
                    {
                        output << "temp_node" << getExpressionID(operands[operNum]);
                        opers.push_back(operands[operNum]);
                        printDagHeader(declarations, operands[operNum], "temp_node");
                    }
                    else
                    {
                        //printDagHeader(declarations, operands[operNum], "temp_node");
                        printExpression(output, declarations, operands[operNum], operatorMap);
                    }
                    i++;
                }
                else
                {
                    int result = 0;
                    t_TypeOfExpression type = getTypeOfExpressionWithOutWidth(operands[operNum]);
                    if (type == TYPE_UNSIGNED_BITVECTOR || type == TYPE_SIGNED_BITVECTOR)
                        result = binaryStringToInteger(getLabelOfExpression(operands[operNum]).substr(2));
                    else if (type == TYPE_UNSIGNED_INTEGER || type == TYPE_SIGNED_INTEGER)
                        result = stringToInteger(getLabelOfExpression(operands[operNum]));

                    output << "((as const ";
                    int wd = getWidth(operands[operNum]); //no of bit reqd to represent upper bound. for wd = 4 indexsize = 2 for wd = 5 indexsize = 3
                    int indexsize = (int) ceil(log2(wd));
                    output << "(Array (_ BitVec " << indexsize << ") (_ BitVec 1))";
                    output << ")"; //ends as const

                    output << result << ")"; //ends final

                    i += 2;
                }
            }
            else if (syntax[i + 1] == 'i') //if $i0 print operands[0] as decimal integer(for selects)
            {
                int operNum = syntax[i + 2] - '0';
                assert(operNum >= 0 && operNum <= 9);

                int result = 0;
                t_TypeOfExpression type = getTypeOfExpressionWithOutWidth(operands[operNum]);
                if (type == TYPE_UNSIGNED_BITVECTOR || type == TYPE_SIGNED_BITVECTOR)
                    result = binaryStringToInteger(getLabelOfExpression(operands[operNum]).substr(2));
                else if (type == TYPE_UNSIGNED_INTEGER || type == TYPE_SIGNED_INTEGER)
                    result = stringToInteger(getLabelOfExpression(operands[operNum]));

//                cout << "Getting operand:" << operNum << " " << result << endl;
//                printExpressionToFileAsDAG("temp", operands[operNum], cout);
//                cout << endl;
                output << result; //getConstantValuePresentInExpression(operands[operNum]);
                i += 2;
            }
            else
            {
                int operNum = syntax[i + 1] - '0';
                assert(operNum >= 0 && operNum <= 9);
                if (getTypeOfValueStoredInExpressionLabel(operands[operNum]) == OPERATOR)
                {
                    output << "temp_node" << getExpressionID(operands[operNum]);
                    opers.push_back(operands[operNum]);
                    printDagHeader(declarations, operands[operNum], "temp_node");
                }
                else
                {cout << getTypeOfValueStoredInExpressionLabel(operands[operNum]) << " " << getLabelOfExpression(operands[operNum]) << endl;//sukanya
                    //printDagHeader(declarations, operands[operNum], "temp_node");
                    printExpression(output, declarations, operands[operNum], operatorMap);
                }
                i++;
            }
        }
        else if (syntax[i] == '\\')
        {
            i++;
            switch (syntax[i])
            {
                case 'n': output << "\n";
                    break;
                case 't': output << "\t";
                    break;
                default: output << syntax[i];
                    break;
            }
        }
        else
            output << syntax[i];
    }

    if (printNodeId)
        output << ")";

    for (int i = 0; i < opers.size(); i++)
    {
        if (isVisited(opers[i]))
            continue;
        //output<<"\n(= temp_node"<<getExpressionID(opers[i])<<" ";
        cout << getTypeOfValueStoredInExpressionLabel(opers[i]) << " " << getLabelOfExpression(opers[i]) << endl;//sukanya
        printExpression(output, declarations, opers[i], operatorMap, true);
        //output<<")";
    }

    return output;
}
// original method ends here



// method modified by sukanya on 8 Dec 2016
// converting from recursive to iterative
ostream& t_ExpressionManager::mod_printExpression(ostream&output, ostream&declarations, t_Expression* expr, map<string, string>&operatorMap, bool printNodeId)
{
    list<pair<t_Expression*, bool> > expression_list;
    expression_list.push_back(pair<t_Expression*, bool>(expr, printNodeId));
    
    while (!expression_list.empty()) {
        pair<t_Expression*, bool> expr_list_elem = expression_list.back();
        expr = expr_list_elem.first;
        printNodeId = expr_list_elem.second;
        expression_list.pop_back();
    
    string label = getLabelOfExpression(expr);
    t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel != OPERATOR)
    {
        if (typeLabel == CONSTANT)
        {   
            //cout << getTypeOfValueStoredInExpressionLabel(expr) << " " << getLabelOfExpression(expr) << endl;//sukanya
            long int value;
            {
                t_ExpressionValue* expr_val = this->getValue(expr); //this->getValue( expressionImplementationType, expressionImplementationManagerInstance);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
                {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(expr) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
                {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(expr) << ")";
                }
                    //                else if( expr_val->getOnlyTypeOfExpression() == TYPE_MEMORY_ARRAY &&
                    //                        label.find(UNKNOWN_MEM_SYMBOL) == 0)
                    //                {
                    //                    printDagHeader(declarations, expr, "temp_node");
                    //                    output << label;
                    //                    setVisited(expr);
                    //                    return output;
                    //                }
                else //if(this->getTypeOfValueStoredInExpressionLabel(this, expressionImplementationType, expressionImplementationManagerInstance)==CONSTANT)
                {
                    string label = getLabelOfExpression(expr);
                    
                    if (label.find_first_of("uxz") != string::npos)
                    {
                        //value = 0;
                        output<<"#b"<<string(label.length()-2, '0');
                    }
                    else
                    {
                        //skip 0b
                        //value = binaryStringToInteger(label.substr(2));
                        output<<"#b"<<label.substr(2);
                    }
//                    output<<"#b"<<label.substr(2)
                }
            }
            
        }
        else
        {
            //Symbol
            printDagHeader(declarations, expr, "temp_node");
            //cout << getTypeOfValueStoredInExpressionLabel(expr) << " " << getLabelOfExpression(expr) << endl;//sukanya
            output << label;
        }
        setVisited(expr);
        ////return output;
        continue;
    }

    if (isVisited(expr))
    {
        output << "temp_node" << getExpressionID(expr);
        ////return output;
        continue;
    }
    //setVisited(expr);

    if (printNodeId)
    {
        output << "\n(= temp_node" << getExpressionID(expr) << " ";
        setVisited(expr);
    }

    //now the expr is an operator node.
    vector<t_Expression*> operands = getVectorOfOperands(expr);

    if (label == "concat" || label == "leftToRightStream" || label == "replic")
    {
        //special handling for concat.
        int bracCount = 0;
        vector<t_Expression*> opers;
        if (label == "replic") //Convert {4{in1}} to concat(in1,in1,in1,in1)
        {
            t_Expression* expr1 = operands.at(0);
            t_Expression* countExpr = operands.at(1);
            int count = getConstantValuePresentInExpression(countExpr);
            operands.clear();
            //operands.resize(count, expr1);
            operands.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                operands.push_back(expr1);
            }
            label = "concat";
        }

        for (int i = 0; i < operands.size() - 1; i++)
        {
            output << "(concat ";
            bracCount++;
            if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
            {
                output << "temp_node" << getExpressionID(operands[i]);
                opers.push_back(operands[i]);
                printDagHeader(declarations, operands[i], "temp_node");
            }
            else
            {
                if (getTypeOfValueStoredInExpressionLabel(operands[i]) == CONSTANT) {
                    long int value;
                    t_ExpressionValue* expr_val = this->getValue(operands[i]);
                    if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                        value = expr_val->getUnsignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(expr) << ")";
                    }
                    else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                        value = expr_val->getSignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(operands[i]) << ")";
                    }
                    else {
                        string label = getLabelOfExpression(operands[i]);
                        output << "#b" << label.substr(2);
                    }
                }
                else {
                    //printExpression(output, declarations, operands[i], operatorMap); //call replaced
                    //expression_list.push_back(pair<t_Expression*, bool>(operands[i], printNodeId));
                    output << getLabelOfExpression(operands[i]) << " ";
                    printDagHeader(declarations, operands[i], "temp_node");
                }
            }
            ////output << " ";
        }

        int i = operands.size() - 1; //for last operand
        if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
        {
            output << "temp_node" << getExpressionID(operands[i]);
            opers.push_back(operands[i]);
            printDagHeader(declarations, operands[i], "temp_node");
        }
        else {
            
            if (getTypeOfValueStoredInExpressionLabel(operands[i]) == CONSTANT) {
                long int value;
                t_ExpressionValue* expr_val = this->getValue(expr);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(expr) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(expr) << ")";
                }
                else {
                    string label = getLabelOfExpression(expr);
                    output << "#b" << label.substr(2);
                }
            }
            else {
                assert (getTypeOfValueStoredInExpressionLabel(operands[i]) == SYMBOL);
                printDagHeader(declarations, operands[i], "temp_node");
                output << getLabelOfExpression(operands[i]) << " ";
            }
        }
            

        for (int i = 0; i < bracCount; i++)
            output << ")";
        if (printNodeId)
            output << ")";
        output << endl;

        for (int i = 0; i < opers.size(); i++)
        {
            if (!isVisited(opers[i]))
            {
                //printExpression(output, declarations, opers[i], operatorMap); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(opers[i], false));
            }
        }

        ////return output;
        continue;
    }
#ifdef QE //#ifdef added by Ajith John on 24 July 2013
    if (label == "kadd" || label == "kand")
    {
        //special handling for kadd and kand
        int bracCount = 0;
        vector<t_Expression*> opers;
        
        for (int i = 0; i < operands.size() - 1; i++)
        {
	    if(label == "kadd")
	    {
            	output << "(bvadd ";
	    }
	    else //label == "kand" 
	    {
		output << "(and ";
	    }

            bracCount++;
            if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
            {
                output << "temp_node" << getExpressionID(operands[i]);
                opers.push_back(operands[i]);
                printDagHeader(declarations, operands[i], "temp_node");
            }
            else
            {
                if (getTypeOfValueStoredInExpressionLabel(operands[i]) == CONSTANT) {
                    long int value;
                    t_ExpressionValue* expr_val = this->getValue(operands[i]);
                    if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                        value = expr_val->getUnsignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(expr) << ")";
                    }
                    else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                        value = expr_val->getSignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(operands[i]) << ")";
                    }
                    else {
                        string label = getLabelOfExpression(operands[i]);
                        output << "#b" << label.substr(2);
                    }
                }
                else {
                    assert (getTypeOfValueStoredInExpressionLabel(operands[i]) == SYMBOL);
                    output << getLabelOfExpression(operands[i]) << " ";
                    printDagHeader(declarations, operands[i], "temp_node");
                }
            }
        }

        int i = operands.size() - 1; //for last operand
        if (getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
        {
            output << "temp_node" << getExpressionID(operands[i]);
            opers.push_back(operands[i]);
            printDagHeader(declarations, operands[i], "temp_node");
        }
        else
            {
                if (getTypeOfValueStoredInExpressionLabel(operands[i]) == CONSTANT) {
                    long int value;
                    t_ExpressionValue* expr_val = this->getValue(expr);
                    if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                        value = expr_val->getUnsignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(expr) << ")";
                    }
                    else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                        value = expr_val->getSignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(expr) << ")";
                    }
                    else {
                        string label = getLabelOfExpression(expr);
                        output << "#b" << label.substr(2);
                    }
                }
                else {
                        assert (getTypeOfValueStoredInExpressionLabel(operands[i]) == SYMBOL);
                         output << getLabelOfExpression(operands[i]) << " ";
                         printDagHeader(declarations, operands[i], "temp_node");
                }
            }
        

        for (int i = 0; i < bracCount; i++)
            output << ")";
        if (printNodeId)
            output << ")";
        
        for (int i = 0; i < opers.size(); i++)
        {
            if (!isVisited(opers[i]))
            {
                //printExpression(output, declarations, opers[i], operatorMap); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(opers[i], false));
            }
        }

        ////return output;
        continue;
    }
    else if (label == "eqz" || label == "diseqz")
    {
        int wd = getWidth(operands[0]);

	if(label == "eqz")
	{
		output << "(= ";
	}
	else // label == "diseqz"
	{
		output << "(not (= ";
	}

        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
            output << "temp_node" << operands[0]->getID();
        else {
            if (getTypeOfValueStoredInExpressionLabel(operands[0]) == CONSTANT) {
                long int value;
                t_ExpressionValue* expr_val = this->getValue(operands[0]);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(operands[0]) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(operands[0]) << ")";
                }
                else {
                    string label = getLabelOfExpression(operands[0]);
                    output << "#b" << label.substr(2);
                }
            }
            else {
                //printExpression(output, declarations, operands[0], operatorMap); //call replaced
                //expression_list.push_back(pair<t_Expression*, bool>(operands[0], printNodeId));
                output << getLabelOfExpression(operands[0]) << " ";
                printDagHeader(declarations, operands[0], "temp_node");
            }
        }
        
	output << " ";
	output << "#b" << getZeroBinaryStringOfGivenLength(wd) ;

        if (printNodeId)
            output << ")";

	if(label == "eqz")
	{
        	output << ")";
	}
	else // label == "diseqz"
	{
		output << "))";
	}

        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR &&
                !isVisited(operands[0]))
        {
            printDagHeader(declarations, operands[0], "temp_node");
            //printExpression(output, declarations, operands[0], operatorMap); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(operands[0], false));
        }

        ////return output;
        continue;
    }	
#endif //#ifdef added by Ajith John on 24 July 2013 ends here
    else if (label == "zeroextn" || label == "signextn")
    {
        int wd = getWidth(operands[0]);
        int finalWd = getConstantValuePresentInExpression(operands[1]);
        //((_ zero_extend i) t)
        string smtLabel = (label == "zeroextn") ? "zero_extend" : "sign_extend";
        output << "((_ "<<smtLabel <<" " << (finalWd - wd) << ") ";
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
            output << "temp_node" << operands[0]->getID();
        else {
            if (getTypeOfValueStoredInExpressionLabel(operands[0]) == CONSTANT) {
                long int value;
                t_ExpressionValue* expr_val = this->getValue(operands[0]);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(operands[0]) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(operands[0]) << ")";
                }
                else {
                    string label = getLabelOfExpression(operands[0]);
                    output << "#b" << label.substr(2);
                }
            }
            else {
                //printExpression(output, declarations, operands[0], operatorMap); //call replaced
                //expression_list.push_back(pair<t_Expression*, bool>(operands[0], printNodeId));
                output << getLabelOfExpression(operands[0]) << " ";
                printDagHeader(declarations, operands[0], "temp_node");
            }
        }
        if (printNodeId)
            output << ")";
        output << ")\n";

        if (getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR &&
                !isVisited(operands[0]))
        {
            printDagHeader(declarations, operands[0], "temp_node");
            //printExpression(output, declarations, operands[0], operatorMap, true); //call replaced
            expression_list.push_back(pair<t_Expression*, bool>(operands[0], true));
        }

        ////return output;
        continue;
    }
    else if (label == "rightToLeftStream")
    {

        // >>4{a,b} will be converted to (assume final size is 8 bits
        // concat( select( concat(a,b), 0, 3), select(concat(a,b), 4, 7) )
        //will perform select concat optimizations as necessary
        int wd = getWidth(expr);
        int N = getConstantValuePresentInExpression(operands[0]);
        t_Expression* innerConcat;
        {
            if (operands.size() == 2)
                innerConcat = operands[1];
            else
            {
                string label = "concat";
                TypeOfExpressionTuple type = getTypeOfExpressionTuple(expr);
                vector<t_Expression*> temp(operands.begin() + 1, operands.end());
                innerConcat = createExpression(label, temp, type);
            }
        }
        vector<t_Expression*> selects;
        for (int i = 0; i <= wd - N; i += N)
        {
            /*label  = "select";
            TypeOfExpressionTuple type = { getTypeOfExpressionWithOutWidth(expr), N};
            
            string ublabel = integerToBinaryString( i+N-1 );
            TypeOfExpressionTuple ubtype = {TYPE_UNSIGNED_BITVECTOR, ublabel.length() };
            t_Expression* ub = createConstant(ublabel, ubtype);
            
            string lblabel = integerToBinaryString( i );
            TypeOfExpressionTuple lbtype = {TYPE_UNSIGNED_BITVECTOR, lblabel.length() };
            t_Expression* lb = createConstant(lblabel, lbtype);
            
            vector<t_Expression*> sel_oper = buildVector(innerConcat, lb,ub);
            selects.push_back( createExpression(label, sel_oper, type) );*/
            cout << "----- RTOL -----Selecting from " << (i + N - 1) << " to " << i << "\n";
            selects.push_back(createASelectExpressionOnExpression(innerConcat, i + N - 1, i));
        }

        string con_label = "concat";
        TypeOfExpressionTuple type = getTypeOfExpressionTuple(expr);
        t_Expression* rToL_modified = createExpression(con_label, selects, type);
        //        cout<<endl;
        //        printExpressionToFileAsDAG("temp", rToL_modified, cout);
        //        cout<<endl;
        if (getTypeOfValueStoredInExpressionLabel(rToL_modified) != OPERATOR) {
            //printExpression(output, declarations, rToL_modified, operatorMap); //call replaced
            //expression_list.push_back(pair<t_Expression*, bool>(rToL_modified, printNodeId));
            output << getLabelOfExpression(rToL_modified) << " ";
            printDagHeader(declarations, rToL_modified, "temp_node");
        }
        else
            output << "temp_node" << getExpressionID(rToL_modified);
        if (printNodeId)
            output << ")";
        //output<<"(= temp_node"<<getExpressionID(expr)<<" temp_node"<<getExpressionID(rToL_modified)<<")"<<endl;
        output << "\n";
        if (getTypeOfValueStoredInExpressionLabel(rToL_modified) == OPERATOR &&
                !isVisited(rToL_modified))
        {
            createdIntermediateExpressions.push_back(rToL_modified);

            printDagHeader(declarations, rToL_modified, "temp_node");
            //printExpression(output, declarations, rToL_modified, operatorMap, true); //call replaced
            expression_list.push_back(pair<t_Expression*, bool>(rToL_modified, true));
        }
        ////return output;
        continue;
    }
    else if(label == "redor")
    {
//#define NEW_EXPR_HAS_ID
#ifdef NEW_EXPR_HAS_ID //Old code
        int wd = getWidth(operands.at(0));
        string constLabel(wd,'0');
        TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(operands[0]);
        t_Expression* constExpr = createConstant(constLabel, typeInfo);
        
        t_Expression* e = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalInEquality, operands[0], constExpr);
        if (getTypeOfValueStoredInExpressionLabel(e) != OPERATOR)
            //printExpression(output, declarations, e, operatorMap, false); //call replaced
            //expression_list.push_back(pair<t_Expression*, bool>(e, false));
            output << getLabelOfExpression(e) << " ";
        else
            output << "temp_node" << getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            printDagHeader(declarations, e, "temp_node");
            //printExpression(output, declarations, e, operatorMap, true); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(e, true));
        }
        ////return output;
        continue;
#else
        output<<"(not (= ";
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) != OPERATOR) {
            if (getTypeOfValueStoredInExpressionLabel(operands[0]) == CONSTANT) {
                long int value;
                t_ExpressionValue* expr_val = this->getValue(operands[0]);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(operands[0]) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << getWidth(operands[0]) << ")";
                }
                else {
                    string label = getLabelOfExpression(operands[0]);
                    output << "#b" << label.substr(2);
                }
            }
            else {
                //printExpression(output, declarations, operands[0], operatorMap, false); //call replaced
                //expression_list.push_back(pair<t_Expression*, bool>(operands[0], false));//printing directly
                output << getLabelOfExpression(operands[0]) << " ";
                printDagHeader(declarations, operands[0], "temp_node");
            }
        }
        else
            output << "temp_node" << getExpressionID(operands[0]);
        
        output<<" #b"<<string(getWidth(operands[0]), '0')<<"))";
        if (printNodeId)
            output << ")";
        if (!isVisited(operands[0]) && getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
        {
            printDagHeader(declarations, operands[0], "temp_node");
            //printExpression(output, declarations, operands[0], operatorMap, true); //call replaced
            expression_list.push_back(pair<t_Expression*, bool>(operands[0], true));
        }
        ////return output;
        continue;
#endif
    }
    else if(label == "redand")
    {
#ifdef NEW_EXPR_HAS_ID //Old code
        int wd = getWidth(operands.at(0));
        string constLabel(wd,'1');
        TypeOfExpressionTuple typeInfo = getTypeOfExpressionTuple(operands[0]);
        t_Expression* constExpr = createConstant(constLabel, typeInfo);
        
        t_Expression* e = createOneBitExpressionWithTwoOperands(m_operatorLabelLogicalEquality, operands[0], constExpr);
        if (getTypeOfValueStoredInExpressionLabel(e) != OPERATOR)
            //printExpression(output, declarations, e, operatorMap, false); //call replaced
            //expression_list.push_back(pair<t_Expression*, bool>(e, false));//printing directly
            output << getLabelOfExpression(e) << " ";
        else
            output << "temp_node" << getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            printDagHeader(declarations, e, "temp_node");
            //printExpression(output, declarations, e, operatorMap, true); //call replaced
            expression_list.push_back(pair<t_Expression*, bool>(e, true));
        }
        ////return output;
        continue;
#else
        output<<"(= ";
        if (getTypeOfValueStoredInExpressionLabel(operands[0]) != OPERATOR) {
            //printExpression(output, declarations, operands[0], operatorMap, false); //call replaced
            // expression_list.push_back(pair<t_Expression*, bool>(operands[0], false));//printing directly
            output << getLabelOfExpression(operands[0]) << " ";
            printDagHeader(declarations, operands[0], "temp_node");
        }
        else
            output << "temp_node" << getExpressionID(operands[0]);
        
        output<<" #b"<<string(getWidth(operands[0]), '1')<<")";
        if (printNodeId)
            output << ")";
        
        if (!isVisited(operands[0]) && getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
        {
            printDagHeader(declarations, operands[0], "temp_node");
            //printExpression(output, declarations, operands[0], operatorMap, true); //call replaced
            expression_list.push_back(pair<t_Expression*, bool>(operands[0], true));
        }
        ////return output;
        continue;
#endif
    }
    else if (/*label == "redor" || label == "redand" || */label == "redxor" || label == "rednor" || label == "rednand" || label == "redxnor")
    {
        vector<t_Expression*> selects;
        int wd = getWidth(operands.at(0));
        for (int i = 0; i < wd; i++)
        {
            t_Expression* sel = createASelectExpressionOnExpression(operands[0], i, i);
            /*t_Expression* sel;
            {
                string labelSel  = "select";
                TypeOfExpressionTuple type = { getTypeOfExpressionWithOutWidth(expr), 1};

                string ublabel = integerToBinaryString( i );
                TypeOfExpressionTuple ubtype = {TYPE_UNSIGNED_BITVECTOR, ublabel.length() };
                t_Expression* ub = createConstant(ublabel, ubtype);

                vector<t_Expression*> sel_oper = buildVector(operands[0], ub ,ub);
                sel = createExpression(labelSel, sel_oper, type);
            }*/
            selects.push_back(sel);
        }

        typeOfExpressionTuple type = {TYPE_UNSIGNED_BITVECTOR, 1};
        string new_label = std::string("bit") + label.substr(3); //convert redor->bitor, redand->bitand, ...
        t_Expression* e = createTwoOperandOperatorFromMultipleOperands(new_label, selects, type);
        //printExpressionToFileAsDAG("temp",e,cout);
        if (getTypeOfValueStoredInExpressionLabel(e) != OPERATOR) {
            //printExpression(output, declarations, e, operatorMap, false); //call replaced
            //expression_list.push_back(pair<t_Expression*, bool>(e, false));//printing directly
            output << getLabelOfExpression(e) << " ";
            printDagHeader(declarations, e, "temp_node");
        }
        else
            output << "temp_node" << getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            printDagHeader(declarations, e, "temp_node");
            //printExpression(output, declarations, e, operatorMap, true); //call replaced
            expression_list.push_back(pair<t_Expression*, bool>(e, true));
        }
        ////return output;
        continue;
    }
#if defined(USE_GEN_INV_FUNCTION) //&& defined(PRINT_GEN_INV_INTERNALLY)
    else if (label == GEN_INV_LABEL)
    {
        bool performInvalidBitSimplification = true;
        {
            t_Expression* readExpr = operands[0];
            if (getLabelOfExpression(readExpr) == "read")
            {
                vector<t_Expression*> read_operands = getVectorOfOperands(readExpr);
                assert(read_operands.size() == 2);
                if (getTypeOfValueStoredInExpressionLabel(read_operands[0]) != OPERATOR)
                {
                    //since the read is a simple read on memory we cannot simplify the invalid bit
                    //further. So keep it as it is.
                    performInvalidBitSimplification = false;
                }
            }
        }

        if (performInvalidBitSimplification)
        {
            t_Expression* inv_Bit = generateInvalidBitExpression(operands[0]);
            createdIntermediateExpressions.push_back(inv_Bit);
            if (getTypeOfValueStoredInExpressionLabel(inv_Bit) == OPERATOR)
            {
                output << "temp_node" << inv_Bit->getID();
            }
            else
            {
                if (getTypeOfValueStoredInExpressionLabel(inv_Bit) == CONSTANT) {
                    long int value;
                    t_ExpressionValue* expr_val = this->getValue(inv_Bit);
                    if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                        value = expr_val->getUnsignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(inv_Bit) << ")";
                    }
                    else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                        value = expr_val->getSignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(inv_Bit) << ")";
                    }
                    else {
                        string label = getLabelOfExpression(inv_Bit);
                        output << "#b" << label.substr(2);
                    }
                }
                else {
                    //printExpression(output, declarations, inv_Bit, operatorMap, false); //call replaced
                    //expression_list.push_back(pair<t_Expression*, bool>(inv_Bit, false));//printing directly
                    output << getLabelOfExpression(inv_Bit) << " ";
                }
            }
            if (printNodeId)
                output << ")";
            output << "\n ;Here was gen-inv(" << operands[0]->getID() << ") simplified to " << inv_Bit->getID() << "\n";
            if (getTypeOfValueStoredInExpressionLabel(inv_Bit) == OPERATOR && !isVisited(inv_Bit))
            {
                printDagHeader(declarations, inv_Bit, "temp_node");
                //printExpression(output, declarations, inv_Bit, operatorMap, true); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(inv_Bit, true));
            }
            ////return output;
            continue;
        }
        else
        {
            //#define OLD_GEN_INV_PRINT
#ifdef OLD_GEN_INV_PRINT
            output << "(gen-inv ";
            assert(getLabelOfExpression(operands[0]) == "read");
            operands[0]->m_dagExpression->getDag()->assignVisitedFlag(false); //make sure we print the expression.
            //printExpression(output, declarations, operands[0], operatorMap, false);//XXX: might fail if the index of read is an operator.
            t_Expression* printLater = NULL;
            if (true)
            {
                vector<t_Expression*> oper = getVectorOfOperands(operands[0]);
                output << "(select ";
                //print the memory variable name
                assert(getTypeOfValueStoredInExpressionLabel(oper[0]) != OPERATOR);
                //printExpression(output, declarations, oper[0], operatorMap, false); //call replaced
                //expression_list.push_back(pair<t_Expression*, bool>(oper[0], false));// print directly
                output << getLabelOfExpression(oper[0]) << " ";
                
                //print the index
                if (getTypeOfValueStoredInExpressionLabel(oper[1]) == OPERATOR)
                {
                    output << "temp_node" << oper[1]->getID();
                    printLater = oper[1];
                }
                else
                {
                    if (getTypeOfValueStoredInExpressionLabel(oper[1]) == CONSTANT) {
                        long int value;
                        t_ExpressionValue* expr_val = this->getValue(oper[1]);
                        if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                            value = expr_val->getUnsignedIntValueOfExpression();
                            assert(value>=0);
                            output << "(_ bv" << value << " " << getWidth(oper[1]) << ")";
                        }
                        else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                            value = expr_val->getSignedIntValueOfExpression();
                            assert(value>=0);
                            output << "(_ bv" << value << " " << getWidth(oper[1]) << ")";
                        }
                        else {
                            string label = getLabelOfExpression(oper[1]);
                            output << "#b" << label.substr(2);
                        }
                    }
                    else {
                        //printExpression(output, declarations, oper[1], operatorMap, false); //call replaced
                        //expression_list.push_back(pair<t_Expression*, bool>(oper[1], false)); // printing directly
                        output << getLabelOfExpression(oper[1]) << " ";
                    }
                }
            }
            output << ")";
            if (printNodeId)
                output << ")";
            output << endl;
            if (printLater && !isVisited(printLater))
            {
                printDagHeader(declarations, printLater, "temp_node");
                //printExpression(output, declarations, printLater, operatorMap, true); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(printLater, true));
            }
            /////return output;
            continue;
#endif
#ifndef OLD_GEN_INV_PRINT
#ifndef QE //#ifndef added by Ajith John on 9 June 2013
            assert("read" == getLabelOfExpression(operands[0]));
            vector<t_Expression*> readOperands = getVectorOfOperands(operands[0]);
            assert(readOperands.size() == 2);
            string memName = getLabelOfExpression(readOperands[0]);
            string funcName = "gen-inv-" + memName;
            t_MemoryArrayBoundsInfo* bounds = cdfg->getMemoryVariableDimensions(getActualSignalName(memName, true, true));
            assert(bounds != NULL);
            assert(bounds->m_bounds.empty() == false);
            int upperBound = bounds->m_bounds[0].m_upperbound;
            int lowerBound = bounds->m_bounds[0].m_lowerbound;
            int memWidth = upperBound - lowerBound + 1;
            int indexWidth = static_cast<int> (ceil(log2(memWidth)));
            if (printedFunctions.hashtable_search(funcName).success() == false)
            {
                //add a declaration for the uninterpreted function
                declarations << "(declare-fun " << funcName << " ((_ BitVec " << indexWidth << ")) Bool)\n";
                printedFunctions.hashtable_insert(funcName, true);
            }
            output << "(" << funcName << " ";
            t_Expression*printLater = NULL;
            t_Expression*index = readOperands[1];
            if (getTypeOfValueStoredInExpressionLabel(index) == OPERATOR)
            {
                output << "temp_node" << index->getID();
                printLater = index;
            }
            else
            {
                if (getTypeOfValueStoredInExpressionLabel(index) == CONSTANT) {
                    long int value;
                    t_ExpressionValue* expr_val = this->getValue(index);
                    if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                        value = expr_val->getUnsignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(index) << ")";
                    }
                    else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                        value = expr_val->getSignedIntValueOfExpression();
                        assert(value>=0);
                        output << "(_ bv" << value << " " << getWidth(index) << ")";
                    }
                    else {
                        string label = getLabelOfExpression(index);
                        output << "#b" << label.substr(2);
                    }
                }
                else {
                    //printExpression(output, declarations, index, operatorMap, false); //call replaced
                    //expression_list.push_back(pair<t_Expression*, bool>(index, false)); // printing directly
                    output << getLabelOfExpression(index) << " ";
                    printDagHeader(declarations, index, "temp_node");
                }
            }
            output << ")";
            if (printNodeId)
                output << ")";
            output << "\n";

            if (printLater && !isVisited(printLater))
            {
                printDagHeader(declarations, printLater, "temp_node");
                //printExpression(output, declarations, printLater, operatorMap, true); //call replaced
                expression_list.push_back(pair<t_Expression*, bool>(printLater, true));
            }
            ////return output;
            continue;
#endif //#endif added by Ajith John on 9 June 2013
#endif
        }
        //else fall through to the SMT OPerator map printing which will print the normal expression.
    }
#endif


    map<string, string>::iterator it = operatorMap.find(label);
    if (it == operatorMap.end())
    {
        cerr << "WARNING!!! No operator labelled " << label << " found." << endl;
        cerr << "size of map is " << operatorMap.size() << endl;
        assert(it != operatorMap.end());
    }
    const string &syntax = it->second;

    vector<t_Expression*> opers;

    for (int i = 0; i < syntax.length(); i++)
    {
        if (syntax[i] == '$')
        {
            /*if(syntax[i+1] == '$') //if $$ print all operands separated by ' '
            {
                for(int j=0;j<operands.size();j++)
                {
                    printExpression(output, operands[j], operatorMap);
                    output<<" ";
                }
                i++;
            }
            else */
            if (syntax[i + 1] == 'm') //if $m0 print operands[0] as constant memory
            {
                int operNum = syntax[i + 2] - '0';
                assert(operNum >= 0 && operNum <= 9);

                if (true || getTypeOfValueStoredInExpressionLabel(operands[operNum]) != CONSTANT ||
                        getTypeOfExpressionWithOutWidth(operands[operNum]) == TYPE_MEMORY_ARRAY)
                {
                    if (getTypeOfValueStoredInExpressionLabel(operands[operNum]) == OPERATOR)
                    {
                        output << "temp_node" << getExpressionID(operands[operNum]);
                        opers.push_back(operands[operNum]);
                        printDagHeader(declarations, operands[operNum], "temp_node");
                    }
                    else
                    {
                        if (getTypeOfValueStoredInExpressionLabel(operands[operNum]) == CONSTANT) {
                            long int value;
                            t_ExpressionValue* expr_val = this->getValue(operands[operNum]);
                            if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                                value = expr_val->getUnsignedIntValueOfExpression();
                                assert(value>=0);
                                output << "(_ bv" << value << " " << getWidth(operands[operNum]) << ")";
                            }
                            else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                                value = expr_val->getSignedIntValueOfExpression();
                                assert(value>=0);
                                output << "(_ bv" << value << " " << getWidth(operands[operNum]) << ")";
                            }
                            else {
                                string label = getLabelOfExpression(operands[operNum]);
                                output << "#b" << label.substr(2);
                            }
                        }
                        else {
                            printDagHeader(declarations, operands[operNum], "temp_node");
                            //printExpression(output, declarations, operands[operNum], operatorMap); //call replaced
                            //expression_list.push_back(pair<t_Expression*, bool>(operands[operNum], printNodeId)); // commenting out, printing directly
                            output << getLabelOfExpression(operands[operNum]) << " ";
                        }
                    }
                    i++;
                }
                else
                {
                    int result = 0;
                    t_TypeOfExpression type = getTypeOfExpressionWithOutWidth(operands[operNum]);
                    if (type == TYPE_UNSIGNED_BITVECTOR || type == TYPE_SIGNED_BITVECTOR)
                        result = binaryStringToInteger(getLabelOfExpression(operands[operNum]).substr(2));
                    else if (type == TYPE_UNSIGNED_INTEGER || type == TYPE_SIGNED_INTEGER)
                        result = stringToInteger(getLabelOfExpression(operands[operNum]));

                    output << "((as const ";
                    int wd = getWidth(operands[operNum]); //no of bit reqd to represent upper bound. for wd = 4 indexsize = 2 for wd = 5 indexsize = 3
                    int indexsize = (int) ceil(log2(wd));
                    output << "(Array (_ BitVec " << indexsize << ") (_ BitVec 1))";
                    output << ")"; //ends as const

                    output << result << ")"; //ends final

                    i += 2;
                }
            }
            else if (syntax[i + 1] == 'i') //if $i0 print operands[0] as decimal integer(for selects)
            {
                int operNum = syntax[i + 2] - '0';
                assert(operNum >= 0 && operNum <= 9);

                int result = 0;
                t_TypeOfExpression type = getTypeOfExpressionWithOutWidth(operands[operNum]);
                if (type == TYPE_UNSIGNED_BITVECTOR || type == TYPE_SIGNED_BITVECTOR)
                    result = binaryStringToInteger(getLabelOfExpression(operands[operNum]).substr(2));
                else if (type == TYPE_UNSIGNED_INTEGER || type == TYPE_SIGNED_INTEGER)
                    result = stringToInteger(getLabelOfExpression(operands[operNum]));

//                cout << "Getting operand:" << operNum << " " << result << endl;
//                printExpressionToFileAsDAG("temp", operands[operNum], cout);
//                cout << endl;
                output << result; //getConstantValuePresentInExpression(operands[operNum]);
                i += 2;
            }
            else
            {
                int operNum = syntax[i + 1] - '0';
                assert(operNum >= 0 && operNum <= 9);
                if (getTypeOfValueStoredInExpressionLabel(operands[operNum]) == OPERATOR)
                {
                    output << "temp_node" << getExpressionID(operands[operNum]);
                    opers.push_back(operands[operNum]);
                    printDagHeader(declarations, operands[operNum], "temp_node");
                }
                else
                {
                    if (getTypeOfValueStoredInExpressionLabel(operands[operNum]) == CONSTANT) {
                        long int value;
                        t_ExpressionValue* expr_val = this->getValue(operands[operNum]);
                        if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER) {
                            value = expr_val->getUnsignedIntValueOfExpression();
                            assert(value>=0);
                            output << "(_ bv" << value << " " << getWidth(operands[operNum]) << ")";
                        }
                        else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER) {
                            value = expr_val->getSignedIntValueOfExpression();
                            assert(value>=0);
                            output << "(_ bv" << value << " " << getWidth(operands[operNum]) << ")";
                        }
                        else {
                            string label = getLabelOfExpression(operands[operNum]);
                            output << "#b" << label.substr(2);
                        }
                    }
                    else {
                        //cout << getTypeOfValueStoredInExpressionLabel(operands[operNum]) << " " << getLabelOfExpression(operands[operNum]) << endl;//sukanya
                        printDagHeader(declarations, operands[operNum], "temp_node");
                        //printExpression(output, declarations, operands[operNum], operatorMap); //call replaced
                        //expression_list.push_back(pair<t_Expression*, bool>(operands[operNum], false)); // printing directly
                        output << getLabelOfExpression(operands[operNum]) << " ";
                    }
                }
                i++;
            }
        }
        else if (syntax[i] == '\\')
        {
            i++;
            switch (syntax[i])
            {
                case 'n': output << "\n";
                    break;
                case 't': output << "\t";
                    break;
                default: output << syntax[i];
                    break;
            }
        }
        else
            output << syntax[i];
    }

    if (printNodeId)
        output << ")";

    for (int i = 0; i < opers.size(); i++)
    {
        if (isVisited(opers[i]))
            continue;
        //output<<"\n(= temp_node"<<getExpressionID(opers[i])<<" ";
        //cout << getTypeOfValueStoredInExpressionLabel(opers[i]) << " " << getLabelOfExpression(opers[i]) << " " << getVectorOfOperands(opers[i]).size() << endl;//sukanya
        //printExpression(output, declarations, opers[i], operatorMap, true); //call replaced
        expression_list.push_back(pair<t_Expression*, bool>(opers[i], true));
        //output<<")";
    }

    }
    return output;
    
}
//method modified by sukanya ends here


void t_ExpressionManager::clearPrintingSets()
{
    printedSet.clear();
    for_each(createdIntermediateExpressions.begin(), createdIntermediateExpressions.end(),
            mem_fun(&t_Expression::clearVisitedFlagsUnderThisExpression));
    createdIntermediateExpressions.clear();
    printedFunctions.clear();
    visitedExprs.clear();
}

template<class T>
ostream& t_ExpressionManager::printExpressions(ostream&output, ostream&declarations, const T &exprs, const string& exprSeparator)
{
    static map<string, string> operatorMap;
    if (operatorMap.empty())
    {
        string envVar = "SS";
        char *envPath = getenv(envVar.c_str());

	assert(envPath != NULL); //line added by Ajith John on 9 June 2013

        string filePath(envPath);
        filePath += "/SMT_OperatorMap";
        ifstream mapFile((char*) filePath.c_str());
        cout << "Generating operator Map from file:" << filePath << endl;
        generateMap(mapFile, operatorMap);
        cout << "Size of operator map:" << operatorMap.size() << endl;
        //getchar();
    }
    clearPrintingSets();
    for_each(exprs.begin(), exprs.end(), mem_fun(&t_Expression::clearVisitedFlagsUnderThisExpression));

    //for(int i=0;i<exprs.size(); i++)
    for (typename T::const_iterator it = exprs.begin(), end = exprs.end(); it != end; ++it)
    {
        t_Expression* ex = *it; //exprs[i];
        printExpression(output, declarations, ex, operatorMap, false);
        output << exprSeparator;
    }
    output.flush();

    for_each(createdIntermediateExpressions.begin(), createdIntermediateExpressions.end(),
            mem_fun(&t_Expression::clearVisitedFlagsUnderThisExpression));
    return output;
}

template<class T>
void t_ExpressionManager::printExpressionsToFileInSMT(const T &exprs, 
                                                      const string&exprSeparator, 
                                                      const string& final, 
                                                      bool negateAll, 
                                                      const string& decl, 
                                                      const string& cons)
{
    static vector<t_Expression*> empty;

    printExpressionsToFileInSMT(exprs, empty, exprSeparator, false, false, negateAll, final, decl, cons);
    return;

    //===================== UNUSED ===========================================
//    {
//        ofstream declarations(decl.empty() ? (final + "_decl").c_str() : decl.c_str());
//        ofstream constraints(cons.empty() ? (final + "_cons").c_str() : cons.c_str());
//
//        declarations << "(set-logic QF_AUFBV)\n(set-info :smt-lib-version 2.0)\n";
//
//        //printHeader(con_smt, exprs);
//        constraints << "\n(assert (and" << endl;
//        if (negateAll)
//        {
//            cerr << "*********** negateAll unhandled **********************" << endl;
//        }
//        negateAll = false;
//        //TODO: correct this function
//        //        if(negateAll)
//        //        {
//        //            T exprsNegated;
//        //            typename T::const_iterator it = exprs.begin(), end = exprs.end();
//        //            for(; it!= end; ++it)
//        //            {
//        //                t_Expression* e = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, *it);
//        //                exprsNegated.insert(e);
//        //            }
//        //            printExpressions(constraints, declarations, exprs, exprSeparator);
//        //        }
//        //        else
//        {
//            printExpressions(constraints, declarations, exprs, exprSeparator);
//        }
//
//        constraints << "\n))";
//
//        constraints << "\n(check-sat)\n(exit)" << endl;
//    }
//    {
//        ofstream f(final.c_str());
//
//        ifstream declarations(decl.empty() ? (final + "_decl").c_str() : decl.c_str());
//        f << declarations.rdbuf() << endl;
//        declarations.close();
//
//        ifstream constraints(cons.empty() ? (final + "_cons").c_str() : cons.c_str());
//        f << constraints.rdbuf() << endl;
//        constraints.close();
//
//        f.close();
//    }
}

const bool bPrintExpressions = true;

template<typename T1, typename T2>
void t_ExpressionManager::printExpressionsToFileInSMT(const T1 &exprs, 
                                                      const T2 &inpConstr, 
                                                      const string&exprSeparator,
                                                      bool negateAll, 
                                                      bool negateConstraints, 
                                                      bool negateExpression,
                                                      const string& final,
                                                      const string& decl, 
                                                      const string& cons)
{
    static map<string, string> operatorMap;
    {
        if (operatorMap.empty())
        {
            string envVar = "SS";
            char *envPath = getenv(envVar.c_str());

	    assert(envPath != NULL); //line added by Ajith John on 9 June 2013

            string filePath(envPath);
            filePath += "/SMT_OperatorMap";
            ifstream mapFile((char*) filePath.c_str());

            generateMap(mapFile, operatorMap);
        }
        //createdIntermediateExpressions.clear();
        //printedSet.clear();
        clearPrintingSets();
        //t_HashTable<unsigned long long, bool> VisitedSet;
        //for_each(exprs.begin(), exprs.end(), mem_fun(&t_Expression::clearVisitedFlagsUnderThisExpression));
        /* for (typename T1::const_iterator it = exprs.begin(), end = exprs.end(); it != end; ++it)
         {
             t_Expression* ex = *it;
             ex->assignVisitedFlagRecursively(false, VisitedSet);
         }

         for (typename T2::const_iterator it = inpConstr.begin(), end = inpConstr.end(); it != end; ++it)
         {
             t_Expression* ex= *it;
             ex->assignVisitedFlagRecursively(false, VisitedSet);
         }*/
    }

    {
        t_Expression* conseqExpr = NULL;
        t_Expression* inpConsExpr = NULL;

        ofstream declarations(decl.empty() ? (final + "_decl").c_str() : decl.c_str());
        ofstream constraints(cons.empty() ? (final + "_cons").c_str() : cons.c_str());

        declarations << "(set-logic QF_ABV)\n(set-info :smt-lib-version 2.0)\n";

        //printHeader(con_smt, exprs);
        if (negateAll)
            cerr << "--------- NegateAll not used -----------" << endl;
        negateAll = false; //Unused.
        //        if( negateAll ) //TODO: Make the entire expression instead of printing Ands and NOTs
        //            constraints <<"\n(assert (not (and"<<endl;
        //        else
        //            constraints << "\n(assert (and" << endl;
        //printExpressions(constraints,declarations, exprs, string("\n"));

        {//printing input constraints
            t_Expression* finalExpression = NULL;
            for (typename T2::const_iterator it = inpConstr.begin(), end = inpConstr.end();
                    it != end; ++it)
            {
                t_Expression *ex = *it;
                if (finalExpression == NULL)
                    finalExpression = ex;
                else
                {
                    finalExpression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND,
                            finalExpression, ex);
                }
            }

            if (negateConstraints && finalExpression != NULL)
            {
                finalExpression = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT,
                        finalExpression);
            }
            //printExpression(constraints, declarations, finalExpression, operatorMap, false);
            //constraints<<exprSeparator;
            inpConsExpr = finalExpression;
        }

        {
            //printing consequent constraints/vaccous constraints
            t_Expression* finalExpression = NULL;
            bool constraintsEmpty = true; //exprs.empty();
            for (typename T1::const_iterator it = exprs.begin(), end = exprs.end();
                    it != end; ++it)
            {
                constraintsEmpty = false;
                t_Expression *ex = *it;
                if (finalExpression == NULL)
                    finalExpression = ex;
                else
                {
                    finalExpression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND,
                            finalExpression, ex);
                }
            }
            if (constraintsEmpty) // since no constraints
            {
                finalExpression = createABitVectorConstantZeroExpression();
            }
            assert(finalExpression != NULL);

            if (negateExpression)
            {

                finalExpression = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT,
                        finalExpression);
            }

            //printExpression(constraints, declarations, finalExpression, operatorMap, false);
            //constraints<<exprSeparator;
            conseqExpr = finalExpression;
        }

        //assert(inpConsExpr != NULL);
        assert(conseqExpr != NULL);

        t_Expression* finalExpression = NULL;
        if (inpConsExpr == NULL)
            finalExpression = conseqExpr;
        else
            finalExpression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, inpConsExpr, conseqExpr);

        assert(finalExpression != NULL);

        if (negateAll)
        {
            finalExpression = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, finalExpression);
        }

        {
            t_HashTable<unsigned long long, bool> VisitedSet;
            finalExpression->assignVisitedFlagRecursively(false, VisitedSet);
        }
        //        if(negateAll)
        //            constraints<<"\n)))";
        //        else
        //            constraints << "\n))";
        constraints << "(assert (and" << endl;
        if(bPrintExpressions)
        {
            if(final == "ConsequentWithWeakenedSMT")
            {
                ofstream file("STE_ConsequentWithWeakened");
                finalExpression->clearVisitedFlagsUnderThisExpression();
                printExpressionToFileAsDAG("Constraint", finalExpression, file, false);
                finalExpression->clearVisitedFlagsUnderThisExpression();
            }
                    printExpression(constraints, declarations, finalExpression, operatorMap, false);

        }
        else
        {
            t_HashTable<unsigned long long, bool> visited;
            vector<t_Expression*> leaves;
            getVectorOfLeafExpressionsRecurse(finalExpression, leaves, visited);
            cerr<<final<<" ExprDagNodes:"<<visited.size()<<endl;
            return ;
        }
        constraints << exprSeparator << "))" << endl;
        constraints << "\n(check-sat)\n(exit)" << endl;
    }

    {
        t_Expression::setVisitedFlagsUnderTheseExpressions(createdIntermediateExpressions, false);
        ofstream f(final.c_str());

        ifstream declarations(decl.empty() ? (final + "_decl").c_str() : decl.c_str());
        f << declarations.rdbuf() << endl;
        declarations.close();

        ifstream constraints(cons.empty() ? (final + "_cons").c_str() : cons.c_str());
        f << constraints.rdbuf() << endl;
        constraints.close();

        f.close();
    }
}

// modified methods added by sukanya start here
template<class T>
void t_ExpressionManager::mod_printExpressionsToFileInSMT(const T &exprs, const string&exprSeparator, const string& final, bool negateAll, const string& decl, const string& cons) {
    static vector<t_Expression*> empty;
    mod_printExpressionsToFileInSMT(exprs, empty, exprSeparator, false, false, negateAll, final, decl, cons);
    return;

}


template<typename T1, typename T2>
void t_ExpressionManager::mod_printExpressionsToFileInSMT(const T1 &exprs, const T2 &inpConstr, const string&exprSeparator, bool negateAll, bool negateConstraints, 
                                                      bool negateExpression, const string& final, const string& decl, const string& cons) {
    static map<string, string> operatorMap;
    {
        if (operatorMap.empty()) {
            string envVar = "SS";
            char *envPath = getenv(envVar.c_str());

	    assert(envPath != NULL); //line added by Ajith John on 9 June 2013

            string filePath(envPath);
            filePath += "/SMT_OperatorMap";
            ifstream mapFile((char*) filePath.c_str());

            generateMap(mapFile, operatorMap);
        }
        //createdIntermediateExpressions.clear();
        //printedSet.clear();
        clearPrintingSets();
        //t_HashTable<unsigned long long, bool> VisitedSet;
        //for_each(exprs.begin(), exprs.end(), mem_fun(&t_Expression::clearVisitedFlagsUnderThisExpression));
        /* for (typename T1::const_iterator it = exprs.begin(), end = exprs.end(); it != end; ++it)
         {
             t_Expression* ex = *it;
             ex->assignVisitedFlagRecursively(false, VisitedSet);
         }

         for (typename T2::const_iterator it = inpConstr.begin(), end = inpConstr.end(); it != end; ++it)
         {
             t_Expression* ex= *it;
             ex->assignVisitedFlagRecursively(false, VisitedSet);
         }*/
    }

    {
        t_Expression* conseqExpr = NULL;
        t_Expression* inpConsExpr = NULL;

        ofstream declarations(decl.empty() ? (final + "_decl").c_str() : decl.c_str());
        ofstream constraints(cons.empty() ? (final + "_cons").c_str() : cons.c_str());

        ////declarations << "(set-logic QF_ABV)\n(set-info :smt-lib-version 2.0)\n";

        //printHeader(con_smt, exprs);
        if (negateAll)
            cerr << "--------- NegateAll not used -----------" << endl;
        negateAll = false; //Unused.
        //        if( negateAll ) //TODO: Make the entire expression instead of printing Ands and NOTs
        //            constraints <<"\n(assert (not (and"<<endl;
        //        else
        //            constraints << "\n(assert (and" << endl;
        //printExpressions(constraints,declarations, exprs, string("\n"));

        {//printing input constraints
            t_Expression* finalExpression = NULL;
            for (typename T2::const_iterator it = inpConstr.begin(), end = inpConstr.end(); it != end; ++it) {
                t_Expression *ex = *it;
                if (finalExpression == NULL)
                    finalExpression = ex;
                else {
                    finalExpression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, finalExpression, ex);
                }
            }

            if (negateConstraints && finalExpression != NULL) {
                finalExpression = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, finalExpression);
            }
            //printExpression(constraints, declarations, finalExpression, operatorMap, false);
            //constraints<<exprSeparator;
            inpConsExpr = finalExpression;
        }

        {
            //printing consequent constraints/vaccous constraints
            t_Expression* finalExpression = NULL;
            bool constraintsEmpty = true; //exprs.empty();
            for (typename T1::const_iterator it = exprs.begin(), end = exprs.end(); it != end; ++it) {
                constraintsEmpty = false;
                t_Expression *ex = *it;
                if (finalExpression == NULL)
                    finalExpression = ex;
                else {
                    finalExpression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, finalExpression, ex);
                }
            }
            if (constraintsEmpty) // since no constraints
            {
                finalExpression = createABitVectorConstantZeroExpression();
            }
            assert(finalExpression != NULL);

            if (negateExpression)
            {

                finalExpression = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, finalExpression);
            }

            //printExpression(constraints, declarations, finalExpression, operatorMap, false);
            //constraints<<exprSeparator;
            conseqExpr = finalExpression;
        }

        //assert(inpConsExpr != NULL);
        assert(conseqExpr != NULL);

        t_Expression* finalExpression = NULL;
        if (inpConsExpr == NULL)
            finalExpression = conseqExpr;
        else
            finalExpression = createOneBitExpressionWithTwoOperands(m_operatorLabelLogAND, inpConsExpr, conseqExpr);

        assert(finalExpression != NULL);

        if (negateAll)
        {
            finalExpression = createOneBitExpressionWithOneOperand(m_operatorLabelLogNOT, finalExpression);
        }

        {
            t_HashTable<unsigned long long, bool> VisitedSet;
            finalExpression->assignVisitedFlagRecursively(false, VisitedSet);
        }
        //        if(negateAll)
        //            constraints<<"\n)))";
        //        else
        //            constraints << "\n))";
        constraints << "(assert (and" << endl;
        if(bPrintExpressions)
        {
            if(final == "ConsequentWithWeakenedSMT")
            {
                ofstream file("STE_ConsequentWithWeakened");
                finalExpression->clearVisitedFlagsUnderThisExpression();
                printExpressionToFileAsDAG("Constraint", finalExpression, file, false);
                finalExpression->clearVisitedFlagsUnderThisExpression();
            }
                    mod_printExpression(constraints, declarations, finalExpression, operatorMap, false);

        }
        else
        {
            t_HashTable<unsigned long long, bool> visited;
            vector<t_Expression*> leaves;
            getVectorOfLeafExpressionsRecurse(finalExpression, leaves, visited);
            cerr<<final<<" ExprDagNodes:"<<visited.size()<<endl;
            return ;
        }
        constraints << exprSeparator << "))" << endl;
        ////constraints << "\n(check-sat)\n(get-model)\n(exit)" << endl;
    }

    {
        t_Expression::setVisitedFlagsUnderTheseExpressions(createdIntermediateExpressions, false);
        ofstream f(final.c_str());

        ifstream declarations(decl.empty() ? (final + "_decl").c_str() : decl.c_str());
        f << declarations.rdbuf() << endl;
        declarations.close();

        ifstream constraints(cons.empty() ? (final + "_cons").c_str() : cons.c_str());
        f << constraints.rdbuf() << endl;
        constraints.close();

        f.close();
    }
}

// modified methods added by sukanya ends here



void t_ExpressionManager::printExpressionToCerr(t_Expression* expr)
{
    if (expr == NULL)
    {
        cerr << "\nNULL Expression" << endl;
        return;
    }
    cerr << "\n";
    expr->clearVisitedFlagsUnderThisExpression();
    t_ExpressionManager::getSingleton()->printExpressionToFileAsDAG("expr", expr, cerr, false);
    cerr << endl;
}

//it is similar to doing post order travresal on binary trees using STACK instead of recursion but here it is DAG so extra check neeeded
void t_ExpressionManager::printExpressionInCNF_iterative(t_Expression * expr,ofstream & fout,int &idx,int & clauses,map<t_Expression*, pair<string,int> >& getCNFIndexMap, ofstream& em_log){
	if(!expr){
		em_log << "NULL expression??, exiting . . . " << endl;
		exit(1);
	}

	unsigned long long id=expr->getID();

	em_log << "Getting CNF for t_Expression with id " << id << endl;
	map<unsigned long long,int>::iterator exprID_cnfID_itr = exprID_cnfID.find(id);
	t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
	string label = getLabelOfExpression(expr);

	//if there is already a CNF index for this expression then we can use that at the place of this function call
	if(exprID_cnfID_itr != exprID_cnfID.end())
	{
		//expression was already visited
		em_log << "expression was already visited with CNF id: " << exprID_cnfID_itr->second << " ,returning..." << endl;
		return;
	}

	stack<t_Expression*>* expression_stack = new stack<t_Expression*>;

	do{
		while(expr){//keep going to the left until leaf is met or an expression already visited is met
			label = getLabelOfExpression(expr);//this is either  the variable name of expression if not an operator, or an operators such as AND, OR, NOT if operator
			typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
			id=expr->getID();
			exprID_cnfID_itr = exprID_cnfID.find(id);

			if(typeLabel != OPERATOR){//the leaf case - simply push on to the stack
				expression_stack->push(expr);
				expr = NULL;
			}
			else{//the non leaf case has two subcases -- visited one and non-visited one
				if(exprID_cnfID_itr == exprID_cnfID.end()){//has not been visited
					//if not visisted fetch its subtrees --- assumed to be max two
					vector<t_Expression*> operands = getVectorOfOperands(expr);
					assert(operands.size() <= 2);

					if(label == "logor" || label == "logand"){
						//first push right child
						expression_stack->push(operands[1]);
						expression_stack->push(expr);
						expr = operands[0];
					}
					else if(label == "lognot"){
						expression_stack->push(expr);
						expr = operands[0];
					}
					else{
						//will consider other cases later
						cerr << "This case isn't being considered now" << endl;
						assert(false);
					}
				}
				else{//been visisted
					expression_stack->push(expr);
					expr = NULL;
				}
			}
			em_log << "stack size " << expression_stack->size() << endl;
		};

		em_log << "out of first loop" << endl;

		em_log << "out stack size " << expression_stack->size() << endl;

		//the stack filled so far has a property, if a node has right child then it is just under that node in the stack
		expr = expression_stack->top();
		expression_stack->pop();

		string label = getLabelOfExpression(expr);//this is either  the variable name of expression if not an operator, or an operators such as AND, OR, NOT if operator
		t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);
		id=expr->getID();
		exprID_cnfID_itr = exprID_cnfID.find(id);

		em_log << "got expr stuff outside , with label " << label << endl;

		if(typeLabel != OPERATOR){//the leaf case - assign CNF id if not assigned
			if(exprID_cnfID_itr == exprID_cnfID.end()){
				em_log << "in leaf not visisted" << endl;
				em_log << "t_Expression* type is not operator and its label is " + label << endl;
				//expression is symbol or value
				em_log << "Assigning cnf id to the expression " << endl;
				exprID_cnfID[id]=idx++;
				em_log << "Storing cnf id in the getCNFIndexMap" << endl;
				getCNFIndexMap[expr].second=idx-1;
				em_log << "CNF id is " << idx-1 << endl;
				expr = NULL;
			}
			else{
				//if already id assigned nothing to do
				em_log << "in leaf already visisted" << endl;
				expr = NULL;
			}
		}
		else{//the non leaf case has two subcases -- visited one and non-visited one
			if(exprID_cnfID_itr == exprID_cnfID.end()){//has not been visited

				//if not visisted fetch its subtrees --- assumed to be max two
				vector<t_Expression*> operands = getVectorOfOperands(expr);
				assert(operands.size() <= 2);

				if(label == "logor" || label == "logand"){
					//if there is a right child and it is on the stack then that branch needs to be explored
					if(!expression_stack->empty() && operands[1] == expression_stack->top()){//the right child needs to be explored before we come to this one
						em_log << "in OR or AND CNF with matching right with top" << endl;
						em_log << "stack size " << expression_stack->size()  << " popping"<< endl;
						expression_stack->pop();
						expression_stack->push(expr);
						expr = operands[1];// the right one becomes the new root on which the traversal begins again as if it were root
					}
					else{//there is a right child but it is not on top of stack right  now -- meaning we are done on both sides of the expr
						//can do CNF writing

						if(label == "logor")
						{
							exprID_cnfID[id]=idx++;
							// considering 'or' and 'and' has only two oeprands
							// a <=> (b or c) can be represented as
							// (!a or b or c) and (a or !b) and (a or !c)

							fout<< -(idx-1);
							for(int i=0;i<operands.size();i++){
								em_log << "in OR CNF 1" << endl;
								fout << " " << exprID_cnfID[operands[i]->getID()];
								//fout<<" "<< operands_id[i];
							}
							fout<<" 0\n";
							clauses++;

							for(int i=0;i<operands.size();i++) {
								em_log << "in OR CNF 2" << endl;
								fout << (idx -1) << " " << (-exprID_cnfID[operands[i]->getID()]) << " 0\n";
								//fout<< (idx-1) <<" "<< (-operands_id[i]) <<" 0\n";
								clauses++;
							}
						}

						if(label == "logand")
						{
							exprID_cnfID[id]=idx++;

							// a <=> (b and c) can be represented as
							// (a or !b or !c) and (!a or b) and (!a or c)

							fout<<(idx-1);
							for(int i=0;i<operands.size();i++){
								em_log << "in AND CNF 1" << endl;
								fout << " " << (-exprID_cnfID[operands[i]->getID()]);
								//fout<<" "<< (-operands_id[i]);
							}
							fout<<" 0\n";
							clauses++;

							for(int i=0;i<operands.size();i++) {
								em_log << "in AND CNF 2" << endl;
								fout  << -(idx-1) << " " << exprID_cnfID[operands[i]->getID()] << " 0\n";
								//fout<< -(idx-1) <<" "<< operands_id[i]<<" 0\n";
								clauses++;
							}
						}
						expr = NULL;
					}
				}
				else if(label == "lognot"){//has no right child
					em_log << "in NOT CNF 2" << endl;
					exprID_cnfID[id] = (-exprID_cnfID[operands[0]->getID()]);
					expr = NULL;
				}
				else{
					//will consider other cases later
					cerr << "This case isn't being considered now" << endl;
					assert(false);
				}



			}
			else{//been visisted
				em_log << "in non leaf already visisted" << endl;
				//if already id assigned nothing to do
				expr = NULL;
			}
		}
	}while(!expression_stack->empty());
}

// added by sai krishna

void t_ExpressionManager::printExpressionInCNF(t_Expression * expr,ofstream & fout,int &idx,int & clauses,map<t_Expression*, pair<string,int> > & getCNFIndexMap, ofstream & em_log)
{
	if(!expr){
		em_log << "NULL expression??, exiting . . . " << endl;
		exit(1);
	}

	unsigned long long id=expr->getID();
    
	em_log << "Getting CNF for t_Expression with id " << id << endl;
	map<unsigned long long,int>::iterator exprID_cnfID_itr = exprID_cnfID.find(id);

	//if there is already a CNF index for this expression then we can use that at the place of this function call
	if(exprID_cnfID_itr != exprID_cnfID.end())
    {
        //expression was already visited
    	em_log << "expression was already visited with CNF id: " << exprID_cnfID_itr->second << " ,returning..." << endl;
        return;
    }
    
    string label = getLabelOfExpression(expr);//this is either  the variable name of expression if not an operator, or an operators such as AND, OR, NOT if operator
    t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(expr);

    if (typeLabel != OPERATOR)
    {
    	em_log << "t_Expression* type is not operator and its label is " + label << endl;
        //expression is symbol or value
    	em_log << "Assigning cnf id to the expression " << endl;
        exprID_cnfID[id]=idx++;
        em_log << "Storing cnf id in the getCNFIndexMap" << endl;
        getCNFIndexMap[expr].second=idx-1;
        em_log << "CNF id is " << idx-1 << endl;
        //cout<<"wow\n";
        //printExpressionToFileAsDAG("expr", expr, cout);
        //cout<<tim-1<<endl;
        //cout<<"wow1\n";
        em_log << "returning" << endl;
        return;
    }

    em_log << "t_Expression* type is operator and has label " + label <<endl;
    em_log << "getting the operands of t_Expression* "  << endl;
    vector<t_Expression*> operands = getVectorOfOperands(expr);
    em_log << "the number of operands are " << operands.size() << endl;
    vector<int> operands_id;
    // visiting all the operands before to make 
    // sure they were assigned a CNF id to represent them
    for(int i=0;i<operands.size();i++) 
    {
    	em_log << "making recursive call to printExpressionInCNF with operand num " << i+1 << " of total operands " << operands.size() << endl;
        printExpressionInCNF(operands[i],fout,idx,clauses,getCNFIndexMap, em_log);
        operands_id.push_back( exprID_cnfID[operands[i]->getID()]);//returning of above calls makes sure that a CNF index has been assigned, so it can be used
    }

    if(label == "lognot")
    {
        exprID_cnfID[id] = - operands_id[0]; // because we are using dimacs format
    }
    else if(label == "logor") 
    {
        exprID_cnfID[id]=idx++;
        // considering 'or' and 'and' has only two oeprands
        // a <=> (b or c) can be represented as
        // (!a or b or c) and (a or !b) and (a or !c) 

        fout<< -(idx-1);
        for(int i=0;i<operands.size();i++)
            fout<<" "<< operands_id[i];
        fout<<" 0\n";
        clauses++;

        for(int i=0;i<operands.size();i++) {
            fout<< (idx-1) <<" "<< (-operands_id[i]) <<" 0\n";
			clauses++;
		}		

    }
    else if(label == "logand")
    {
        exprID_cnfID[id]=idx++;

        // a <=> (b and c) can be represented as
        // (a or !b or !c) and (!a or b) and (!a or c) 

        fout<<(idx-1);
        for(int i=0;i<operands.size();i++)
            fout<<" "<< (-operands_id[i]);
        fout<<" 0\n";
        clauses++;

        for(int i=0;i<operands.size();i++) {
            fout<< -(idx-1) <<" "<< operands_id[i]<<" 0\n";
			clauses++;
		}
    }
    else
    {
        //will consider other cases later
        cerr << "This case isn't being considered now" << endl;
        assert(false);

    }
	
 //    cout<<"bro\n";
 //    printExpressionToFileAsDAG("expr", expr, cout);
 //    cout<<"bro1\n";
 //    cout<<label<<endl;
	// // fout<<	
	// exprID_cnfID[id]=tim++;
		
} 

void t_ExpressionManager::printExpressionsToFileInCNF(vector<t_Expression *> &exprs,string filename,int & idx,int & clauses,map<t_Expression*, pair<string,int> > & getCNFIndexMap) 
{
	cout << "print Expression to file called" << endl;
	ofstream em_log("em_log");
	em_log << "No. of expressions: " << exprs.size() << endl;
	

	//tim=1;	
	exprID_cnfID.clear();
	
	ofstream fout;
	fout.open(filename.c_str(),ios::out);
	
	for(int i=0;i<exprs.size();i++) {
		em_log << "Printing expression number: " << i+1  << endl;
		//printExpressionInCNF(exprs[i],fout,idx,clauses,getCNFIndexMap, em_log);
		printExpressionInCNF_iterative(exprs[i],fout,idx,clauses,getCNFIndexMap, em_log);
		fout<<exprID_cnfID[exprs[i]->getID() ]<<" 0\n";
		clauses++;
		em_log << "Clause count: " << clauses << endl;
		em_log << "Variables count: " << idx-1 << endl;
        //fout<<"----------\n";
	}
	fout.close();
	em_log.close();
}

void t_ExpressionManager::print_expr_var_cnf_id(map<t_Expression*, pair<string, int> >& getCNFIndexMap){
	map<t_Expression*, pair<string, int> >::iterator map_itr;
	ofstream fout("mapping_expr_cnf");
	for(map_itr = getCNFIndexMap.begin(); map_itr != getCNFIndexMap.end(); ++map_itr){
		fout << (map_itr->second).first << " => " << (map_itr->second).second << endl;
	}
}

void t_ExpressionManager::printExpressionInCNFefficient(t_Expression* expr, string filename, int &idx, int & clauses, map<t_Expression*, pair<string, int> >& getCNFIndexMap, ofstream& em_log){
	if(!expr){
		cerr << "Error: expression passed is NULL, returning..." << endl;
		return;
	}
	map<int, t_Expression*> map_CNFid_to_expr;
	map<t_Expression*, int> map_expr_to_cnfID;
	map<t_Expression*, int>::iterator itr_map_expr_to_cnfID;
	int last = 1, curr = 0;

	map_expr_to_cnfID[expr] = 1;
	map_CNFid_to_expr[1] = expr;


	ofstream fout(filename.c_str());

	if(!fout.is_open()){
		cerr << "Error: couldn't open file " + filename  << endl;
	}
	else{
		fout << "1" << " 0\n";
		clauses++;

		if(getTypeOfValueStoredInExpressionLabel(expr) != OPERATOR){//the corner case when it is just a single node then it must be asserted
			getCNFIndexMap[expr].second = 1;
			idx += 1;//idx is number of variables + 1
			return;
		}

		while(curr < last){
			em_log << "Last - Curr size " << last - curr << endl;
			em_log << "map_CNFid_to_expr size " << map_CNFid_to_expr.size() << endl;
			curr++;
			t_Expression* curr_expr = map_CNFid_to_expr[curr];
			t_TypeOfValueStoredInExpressionLabel typeLabel = getTypeOfValueStoredInExpressionLabel(curr_expr);
			string label = getLabelOfExpression(curr_expr);

			vector<int> operands_as_cnf_ids;

			if(typeLabel == OPERATOR){
				vector<t_Expression*> operands_curr_expr = getVectorOfOperands(curr_expr);
				for(vector<t_Expression*>::iterator opern_itr = operands_curr_expr.begin(); opern_itr != operands_curr_expr.end(); ++opern_itr){
					t_Expression* curr_operand = *opern_itr;
					itr_map_expr_to_cnfID = map_expr_to_cnfID.find(curr_operand);
					if(itr_map_expr_to_cnfID == map_expr_to_cnfID.end()){
						map_expr_to_cnfID[curr_operand] = last+1;
						map_CNFid_to_expr[last+1] = curr_operand;
						if(getTypeOfValueStoredInExpressionLabel(curr_operand) != OPERATOR){
							getCNFIndexMap[curr_operand].second = last+1;
						}
						operands_as_cnf_ids.push_back(last+1);
						last++;
					}
					else{
						operands_as_cnf_ids.push_back(map_expr_to_cnfID[curr_operand]);
					}
				}
			}

			//by this time the operands have some CNF ids and CNF can be written for their parent which also has CNfids and then removed from map_CNFid_to_expr;

			if(typeLabel == OPERATOR){
				if(label == "logor"){
					// a <=> (b or c) can be represented as
					// (!a or b or c) and (a or !b) and (a or !c)
					//this can be generalised to any numbger of operands on RHS

					fout << (-curr);
					for(int i = 0; i < operands_as_cnf_ids.size(); i++){
						fout << " " << operands_as_cnf_ids[i];
					}
					fout<<" 0\n";
					clauses++;

					for(int i = 0; i < operands_as_cnf_ids.size(); i++){
						fout << curr << " " << (-operands_as_cnf_ids[i]) << " 0\n";
						clauses++;
					}
				}
				else if(label == "logand"){
					// a <=> (b and c) can be represented as
					// (a or !b or !c) and (!a or b) and (!a or c)
					//this can be generalised to any numbger of operands on RHS


					fout << curr;
					for(int i = 0; i < operands_as_cnf_ids.size(); i++){
						fout << " " << (-operands_as_cnf_ids[i]);
					}
					fout<<" 0\n";
					clauses++;

					for(int i = 0; i < operands_as_cnf_ids.size(); i++){
						fout << (-curr) << " " << operands_as_cnf_ids[i] << " 0\n";
						clauses++;
					}
				}
				else if(label == "lognot"){
					//a <=> !b can be represented as
					//(!a or !b) and (b or a)

					fout << (-curr) << " " << (-operands_as_cnf_ids[0]) << " 0\n";
					clauses++;
					fout << curr << " " << operands_as_cnf_ids[0] << " 0\n";
					clauses++;
				}
				else{
					//will consider other cases later
					cerr << "This case isn't being considered now" << endl;
					assert(false);
				}
			}
			map_CNFid_to_expr.erase(curr);
		}
		idx += last;//idx must be equal to total cnf variables -1
		fout.close();
	}
}

//--------------------------
template ostream& t_ExpressionManager::printExpressions(ostream&output, ostream&declarations, const vector<t_Expression*> &exprs, const string& exprSeparator);
template ostream& t_ExpressionManager::printExpressions(ostream&output, ostream&declarations, const set<t_Expression*> &exprs, const string& exprSeparator);

template void t_ExpressionManager::printExpressionsToFileInSMT(const vector<t_Expression*>&, const string&, const string&, bool, const string&, const string&);
template void t_ExpressionManager::printExpressionsToFileInSMT(const set<t_Expression*>&, const string&, const string&, bool, const string&, const string&);
//template void t_ExpressionManager::printExpressionsToFileInSMT(const list<t_Expression*>&, const string&, const string&, const string&, const string&);
template void t_ExpressionManager::printExpressionsToFileInSMT(const vector<t_Expression*> &exprs, const vector<t_Expression*> &inpConstr, const string&exprSeparator, bool, bool, bool, const string& final, const string& decl, const string& cons);
template void t_ExpressionManager::printExpressionsToFileInSMT(const set<t_Expression*> &exprs, const vector<t_Expression*> &inpConstr, const string&exprSeparator, bool, bool, bool, const string& final, const string& decl, const string& cons);
template void t_ExpressionManager::printExpressionsToFileInSMT(const set<t_Expression*> &exprs, const set<t_Expression*> &inpConstr, const string&exprSeparator, bool, bool, bool, const string& final, const string& decl, const string& cons);

// added by sukanya
template void t_ExpressionManager::mod_printExpressionsToFileInSMT(const vector<t_Expression*>&, const string&, const string&, bool, const string&, const string&);
template void t_ExpressionManager::mod_printExpressionsToFileInSMT(const vector<t_Expression*> &exprs, const vector<t_Expression*> &inpConstr, const string&exprSeparator, bool, bool, bool, const string& final, const string& decl, const string& cons);
