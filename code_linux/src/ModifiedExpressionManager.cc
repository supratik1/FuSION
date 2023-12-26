#include "../include/ModifiedExpressionManager.h"
#include "ExpressionManager.h"

ModifiedExpressionManager::ModifiedExpressionManager() {
    
}

ModifiedExpressionManager::~ModifiedExpressionManager() {
    
}




static vector<t_Expression*> createdIntermediateExpressions;
t_HashTable<unsigned long long int, bool> copy_printedSet;
static t_HashTable<string, bool> printedFunctions;
static t_HashTable<unsigned long long, bool> copy_visitedExprs;

map<string, string>& ModifiedExpressionManager::copy_generateMap(istream& input, map<string, string>& operatorMap, char fieldSeparator, char entrySeparator)
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

void ModifiedExpressionManager::mod_clearPrintingSets(t_ExpressionManager * em)
{
    copy_printedSet.clear();
    for_each(createdIntermediateExpressions.begin(), createdIntermediateExpressions.end(),
            mem_fun(&t_Expression::clearVisitedFlagsUnderThisExpression));
    createdIntermediateExpressions.clear();
    printedFunctions.clear();
    copy_visitedExprs.clear();
}

bool ModifiedExpressionManager::mod_printArrayHeader(t_ExpressionManager * em, ostream& out, t_Expression* expr, const string& prefix)
{
    if (em->getTypeOfExpressionWithOutWidth(expr) != TYPE_MEMORY_ARRAY)
        return false;

    string label;
    t_TypeOfValueStoredInExpressionLabel typeLabel = em->getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel == SYMBOL)
    {
        label = em->getLabelOfExpression(expr);
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

    pair<vector<t_MemoryBounds>, int> bounds_res = em->getBoundsOfArray(expr);
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

//t_HashTable<unsigned long long int, bool> printedSet;
//set of printed uninterpreted functions



ostream& ModifiedExpressionManager::mod_printDagHeader(t_ExpressionManager * em, ostream& out, t_Expression* expr, const string& prefix)
{
    //if(isVisited(expr))
    if (copy_printedSet.hashtable_search(expr->getID()).success())
    {
        //cout << "***** Already Declared:" << getLabelOfExpression(expr) << " " << getExpressionID(expr) << endl;
        return out;
    }
    copy_printedSet.hashtable_insert(expr->getID(), true);
    t_TypeOfValueStoredInExpressionLabel typeLabel = em->getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel == SYMBOL /*|| (typeLabel == CONSTANT && getLabelOfExpression(expr).find(UNKNOWN_MEM_SYMBOL) == 0)*/)
    {
        if (mod_printArrayHeader(em, out, expr, prefix) == false) //if not array
        {
            string label = em->getLabelOfExpression(expr);
            int width = em->getWidth(expr);
            if (width == 1)// commented out by sukanya
		//#ifndef QE //#ifndef added by Ajith John on 24 July 2013
                	out << "(declare-fun " << label << " () Bool)\n";
		//#else
		//	out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
		//#endif //#ifndef added by Ajith John on 24 July 2013 ends here
            else
                out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
            // debug
            // assert(label.compare("ctrl_ps_31_0_0_0")!=0);
            // debug
        }
    }
    else if (typeLabel == OPERATOR)
    {
        if (mod_printArrayHeader(em, out, expr, prefix) == false)
        {
            string label = prefix + integerToString(em->getExpressionID(expr));
            int width = em->getWidth(expr);
            switch (em->getTypeOfOperator(expr))
            {
                case em->LOGICAL:
                case em->RELATIONAL:
                    out << "(declare-fun " << label << " () Bool)\n";
                    break;
                default:
                    out << "(declare-fun " << label << " () (_ BitVec " << width << "))\n";
            }
        }
    }
    return out;
}
static t_HashTable<unsigned long long, bool> visitedExprs;
bool ModifiedExpressionManager::isVisited(t_Expression* expr)
{
    assert(expr != NULL);
    unsigned long long id = expr->getID();
    return visitedExprs.hashtable_search(id).success();
}

void ModifiedExpressionManager::setVisited(t_Expression* expr, bool value)
{
    assert(expr != NULL);
    //expr->m_dagExpression->getDag()->assignVisitedFlag(value); // changed by sukanya
    expr->getDagExpression()->getDag()->assignVisitedFlag(value);
    
    unsigned long long id = expr->getID();
    if (value)
        visitedExprs.hashtable_insert(id, true);
    else
        visitedExprs.hashtable_delete(id);
}

ostream& ModifiedExpressionManager::mod_printExpression(t_ExpressionManager * em, ostream&output, ostream&declarations, t_Expression* expr, map<string, string>&operatorMap, bool printNodeId)
{

    string label = em->getLabelOfExpression(expr);
    t_TypeOfValueStoredInExpressionLabel typeLabel = em->getTypeOfValueStoredInExpressionLabel(expr);
    if (typeLabel != OPERATOR)
    {
        if (typeLabel == CONSTANT)
        {
            long int value;
            {
                t_ExpressionValue* expr_val = em->getValue(expr); //this->getValue( expressionImplementationType, expressionImplementationManagerInstance);
                if (expr_val->getOnlyTypeOfExpression() == TYPE_UNSIGNED_INTEGER)
                {
                    value = expr_val->getUnsignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << em->getWidth(expr) << ")";
                }
                else if (expr_val->getOnlyTypeOfExpression() == TYPE_SIGNED_INTEGER)
                {
                    value = expr_val->getSignedIntValueOfExpression();
                    assert(value>=0);
                    output << "(_ bv" << value << " " << em->getWidth(expr) << ")";
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
                    string label = em->getLabelOfExpression(expr);

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
            mod_printDagHeader(em, declarations, expr, "temp_node");
            output << label;
        }
        setVisited(expr);
        return output;
    }

    if (isVisited(expr))
    {
        output << "temp_node" << em->getExpressionID(expr);
        return output;
    }
    //setVisited(expr);

    if (printNodeId)
    {
        output << "\n(= temp_node" << em->getExpressionID(expr) << " ";
        setVisited(expr);
    }

    //now the expr is an operator node.
    vector<t_Expression*> operands = em->getVectorOfOperands(expr);

    if (label == "concat" || label == "leftToRightStream" || label == "replic")
    {
        //special handling for concat.
        int bracCount = 0;
        vector<t_Expression*> opers;
        if (label == "replic") //Convert {4{in1}} to concat(in1,in1,in1,in1)
        {
            t_Expression* expr1 = operands.at(0);
            t_Expression* countExpr = operands.at(1);
            int count = em->getConstantValuePresentInExpression(countExpr);
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
            if (em->getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
            {
                output << "temp_node" << em->getExpressionID(operands[i]);
                opers.push_back(operands[i]);
                mod_printDagHeader(em, declarations, operands[i], "temp_node");
            }
            else
            {
                mod_printExpression(em, output, declarations, operands[i], operatorMap);
                //printDagHeader(declarations, operands[i], "temp_node");
            }
            output << " ";
        }

        int i = operands.size() - 1; //for last operand
        if (em->getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
        {
            output << "temp_node" << em->getExpressionID(operands[i]);
            opers.push_back(operands[i]);
            mod_printDagHeader(em, declarations, operands[i], "temp_node");
        }
        else
        {
            //printDagHeader(declarations, operands[i], "temp_node");
            mod_printExpression(em, output, declarations, operands[i], operatorMap);
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
                mod_printExpression(em, output, declarations, opers[i], operatorMap);
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
            if (em->getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
            {
                output << "temp_node" << em->getExpressionID(operands[i]);
                opers.push_back(operands[i]);
                mod_printDagHeader(em, declarations, operands[i], "temp_node");
            }
            else
            {
                mod_printExpression(em, output, declarations, operands[i], operatorMap);                
            }
            output << " ";
        }

        int i = operands.size() - 1; //for last operand
        if (em->getTypeOfValueStoredInExpressionLabel(operands[i]) == OPERATOR)
        {
            output << "temp_node" << em->getExpressionID(operands[i]);
            opers.push_back(operands[i]);
            mod_printDagHeader(em, declarations, operands[i], "temp_node");
        }
        else
        {
            mod_printExpression(em, output, declarations, operands[i], operatorMap);
        }

        for (int i = 0; i < bracCount; i++)
            output << ")";
        if (printNodeId)
            output << ")";
        
        for (int i = 0; i < opers.size(); i++)
        {
            if (!isVisited(opers[i]))
            {
                mod_printExpression(em, output, declarations, opers[i], operatorMap);
            }
        }

        return output;
    }
    else if (label == "eqz" || label == "diseqz")
    {
        int wd = em->getWidth(operands[0]);

	if(label == "eqz")
	{
		output << "(= ";
	}
	else // label == "diseqz"
	{
		output << "(not (= ";
	}

        if (em->getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
            output << "temp_node" << operands[0]->getID();
        else
            mod_printExpression(em, output, declarations, operands[0], operatorMap);

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

        if (em->getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR &&
                !isVisited(operands[0]))
        {
            mod_printDagHeader(em, declarations, operands[0], "temp_node");
            mod_printExpression(em, output, declarations, operands[0], operatorMap);
        }

        return output;
    }	
#endif //#ifdef added by Ajith John on 24 July 2013 ends here
    else if (label == "zeroextn" || label == "signextn")
    {
        int wd = em->getWidth(operands[0]);
        int finalWd = em->getConstantValuePresentInExpression(operands[1]);
        //((_ zero_extend i) t)
        string smtLabel = (label == "zeroextn") ? "zero_extend" : "sign_extend";
        output << "((_ "<<smtLabel <<" " << (finalWd - wd) << ") ";
        if (em->getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
            output << "temp_node" << operands[0]->getID();
        else
            mod_printExpression(em, output, declarations, operands[0], operatorMap);
        if (printNodeId)
            output << ")";
        output << ")\n";

        if (em->getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR &&
                !isVisited(operands[0]))
        {
            mod_printDagHeader(em, declarations, operands[0], "temp_node");
            mod_printExpression(em, output, declarations, operands[0], operatorMap, true);
        }

        return output;
    }
    else if (label == "rightToLeftStream")
    {

        // >>4{a,b} will be converted to (assume final size is 8 bits
        // concat( select( concat(a,b), 0, 3), select(concat(a,b), 4, 7) )
        //will perform select concat optimizations as necessary
        int wd = em->getWidth(expr);
        int N = em->getConstantValuePresentInExpression(operands[0]);
        t_Expression* innerConcat;
        {
            if (operands.size() == 2)
                innerConcat = operands[1];
            else
            {
                string label = "concat";
                TypeOfExpressionTuple type = em->getTypeOfExpressionTuple(expr);
                vector<t_Expression*> temp(operands.begin() + 1, operands.end());
                innerConcat = em->createExpression(label, temp, type);
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
            selects.push_back(em->createASelectExpressionOnExpression(innerConcat, i + N - 1, i));
        }

        string con_label = "concat";
        TypeOfExpressionTuple type = em->getTypeOfExpressionTuple(expr);
        t_Expression* rToL_modified = em->createExpression(con_label, selects, type);
        //        cout<<endl;
        //        printExpressionToFileAsDAG("temp", rToL_modified, cout);
        //        cout<<endl;
        if (em->getTypeOfValueStoredInExpressionLabel(rToL_modified) != OPERATOR)
            mod_printExpression(em, output, declarations, rToL_modified, operatorMap);
        else
            output << "temp_node" << em->getExpressionID(rToL_modified);
        if (printNodeId)
            output << ")";
        //output<<"(= temp_node"<<getExpressionID(expr)<<" temp_node"<<getExpressionID(rToL_modified)<<")"<<endl;
        output << "\n";
        if (em->getTypeOfValueStoredInExpressionLabel(rToL_modified) == OPERATOR &&
                !isVisited(rToL_modified))
        {
            createdIntermediateExpressions.push_back(rToL_modified);

            mod_printDagHeader(em, declarations, rToL_modified, "temp_node");
            mod_printExpression(em, output, declarations, rToL_modified, operatorMap, true);
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
        if (em->getTypeOfValueStoredInExpressionLabel(operands[0]) != OPERATOR)
            mod_printExpression(em, output, declarations, operands[0], operatorMap, false);
        else
            output << "temp_node" << em->getExpressionID(operands[0]);
        
        output<<" #b"<<string(em->getWidth(operands[0]), '0')<<"))";
        if (printNodeId)
            output << ")";
        if (!isVisited(operands[0]) && em->getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
        {
            mod_printDagHeader(em, declarations, operands[0], "temp_node");
            mod_printExpression(em, output, declarations, operands[0], operatorMap, true);
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
        if (em->getTypeOfValueStoredInExpressionLabel(operands[0]) != OPERATOR)
            mod_printExpression(em, output, declarations, operands[0], operatorMap, false);
        else
            output << "temp_node" << em->getExpressionID(operands[0]);
        
        output<<" #b"<<string(em->getWidth(operands[0]), '1')<<")";
        if (printNodeId)
            output << ")";
        
        if (!isVisited(operands[0]) && em->getTypeOfValueStoredInExpressionLabel(operands[0]) == OPERATOR)
        {
            mod_printDagHeader(em, declarations, operands[0], "temp_node");
            mod_printExpression(em, output, declarations, operands[0], operatorMap, true);
        }
        return output;
#endif
    }
    else if (/*label == "redor" || label == "redand" || */label == "redxor" || label == "rednor" || label == "rednand" || label == "redxnor")
    {
        vector<t_Expression*> selects;
        int wd = em->getWidth(operands.at(0));
        for (int i = 0; i < wd; i++)
        {
            t_Expression* sel = em->createASelectExpressionOnExpression(operands[0], i, i);
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
        t_Expression* e = em->createTwoOperandOperatorFromMultipleOperands(new_label, selects, type);
        //printExpressionToFileAsDAG("temp",e,cout);
        if (em->getTypeOfValueStoredInExpressionLabel(e) != OPERATOR)
            mod_printExpression(em, output, declarations, e, operatorMap, false);
        else
            output << "temp_node" << em->getExpressionID(e);
        if (printNodeId)
            output << ")";
        output << "\n";
        assert(em->getTypeOfValueStoredInExpressionLabel(e) == OPERATOR);
        if (!isVisited(e))
        {
            createdIntermediateExpressions.push_back(e);

            mod_printDagHeader(em, declarations, e, "temp_node");
            mod_printExpression(em, output, declarations, e, operatorMap, true);
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

                if (true || em->getTypeOfValueStoredInExpressionLabel(operands[operNum]) != CONSTANT ||
                        em->getTypeOfExpressionWithOutWidth(operands[operNum]) == TYPE_MEMORY_ARRAY)
                {
                    if (em->getTypeOfValueStoredInExpressionLabel(operands[operNum]) == OPERATOR)
                    {
                        output << "temp_node" << em->getExpressionID(operands[operNum]);
                        opers.push_back(operands[operNum]);
                        mod_printDagHeader(em, declarations, operands[operNum], "temp_node");
                    }
                    else
                    {
                        //printDagHeader(declarations, operands[operNum], "temp_node");
                        mod_printExpression(em, output, declarations, operands[operNum], operatorMap);
                    }
                    i++;
                }
                else
                {
                    int result = 0;
                    t_TypeOfExpression type = em->getTypeOfExpressionWithOutWidth(operands[operNum]);
                    if (type == TYPE_UNSIGNED_BITVECTOR || type == TYPE_SIGNED_BITVECTOR)
                        result = binaryStringToInteger(em->getLabelOfExpression(operands[operNum]).substr(2));
                    else if (type == TYPE_UNSIGNED_INTEGER || type == TYPE_SIGNED_INTEGER)
                        result = stringToInteger(em->getLabelOfExpression(operands[operNum]));

                    output << "((as const ";
                    int wd = em->getWidth(operands[operNum]); //no of bit reqd to represent upper bound. for wd = 4 indexsize = 2 for wd = 5 indexsize = 3
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
                t_TypeOfExpression type = em->getTypeOfExpressionWithOutWidth(operands[operNum]);
                if (type == TYPE_UNSIGNED_BITVECTOR || type == TYPE_SIGNED_BITVECTOR)
                    result = binaryStringToInteger(em->getLabelOfExpression(operands[operNum]).substr(2));
                else if (type == TYPE_UNSIGNED_INTEGER || type == TYPE_SIGNED_INTEGER)
                    result = stringToInteger(em->getLabelOfExpression(operands[operNum]));

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
                if (em->getTypeOfValueStoredInExpressionLabel(operands[operNum]) == OPERATOR)
                {
                    output << "temp_node" << em->getExpressionID(operands[operNum]);
                    opers.push_back(operands[operNum]);
                    mod_printDagHeader(em, declarations, operands[operNum], "temp_node");
                }
                else
                {
                    //printDagHeader(declarations, operands[operNum], "temp_node");
                    mod_printExpression(em, output, declarations, operands[operNum], operatorMap);
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
        mod_printExpression(em, output, declarations, opers[i], operatorMap, true);
        //output<<")";
    }

    return output;
}

template<class T>
void ModifiedExpressionManager::mod_printExpressionsToFileInSMT(t_ExpressionManager * em, const T &exprs, 
                                                      const string&exprSeparator, 
                                                      const string& final, 
                                                      bool negateAll, 
                                                      const string& decl, 
                                                      const string& cons)
{
    static vector<t_Expression*> empty;

    mod_printExpressionsToFileInSMT(em, exprs, empty, exprSeparator, false, false, negateAll, final, decl, cons);
    return;

    
}


const bool bPrintExpressions = true;

template<typename T1, typename T2>
void ModifiedExpressionManager::mod_printExpressionsToFileInSMT(t_ExpressionManager * em, const T1 &exprs, 
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

            copy_generateMap(mapFile, operatorMap);
        }
        //createdIntermediateExpressions.clear();
        //printedSet.clear();
        em->clearPrintingSets();
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
                    finalExpression = em->createOneBitExpressionWithTwoOperands(em->m_operatorLabelLogAND,
                            finalExpression, ex);
                }
            }

            if (negateConstraints && finalExpression != NULL)
            {
                finalExpression = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT,
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
                    finalExpression = em->createOneBitExpressionWithTwoOperands(em->m_operatorLabelLogAND,
                            finalExpression, ex);
                }
            }
            if (constraintsEmpty) // since no constraints
            {
                finalExpression = em->createABitVectorConstantZeroExpression();
            }
            assert(finalExpression != NULL);

            if (negateExpression)
            {

                finalExpression = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT,
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
            finalExpression = em->createOneBitExpressionWithTwoOperands(em->m_operatorLabelLogAND, inpConsExpr, conseqExpr);

        assert(finalExpression != NULL);

        if (negateAll)
        {
            finalExpression = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, finalExpression);
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
                em->printExpressionToFileAsDAG("Constraint", finalExpression, file, false);
                finalExpression->clearVisitedFlagsUnderThisExpression();
            }
                    //em->printExpression(constraints, declarations, finalExpression, operatorMap, false); // changed by sukanya
                    mod_printExpression(em, constraints, declarations, finalExpression, operatorMap, false);
        }
        else
        {
            t_HashTable<unsigned long long, bool> visited;
            vector<t_Expression*> leaves;
            em->getVectorOfLeafExpressionsRecurse(finalExpression, leaves, visited);
            cerr<<final<<" ExprDagNodes:"<<visited.size()<<endl;
            return ;
        }
        constraints << exprSeparator << "))" << endl;
        //constraints << "\n(check-sat)\n(exit)" << endl;
        constraints << "\n(check-sat)\n(get-model)\n(exit)" << endl; // changed by sukanya
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

//template ostream& ModifiedExpressionManager::mod_printExpressions(t_ExpressionManager * em, ostream&output, ostream&declarations, const vector<t_Expression*> &exprs, const string& exprSeparator);
template void ModifiedExpressionManager::mod_printExpressionsToFileInSMT(t_ExpressionManager * em, const vector<t_Expression*>&, const string&, const string&, bool, const string&, const string&);