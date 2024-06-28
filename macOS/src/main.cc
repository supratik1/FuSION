#include <string>
#include <cstring>
#include <cassert>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iomanip> // for setting precision of floating point values
#include <fstream>
#include <sstream>
#include <limits>
#include "../include/GraphManager.h"
#include "../include/GraphManagerNew.h"
#include "../include/XMLParser.h"
#include "../include/Z3Solver.h"
#include "../include/ModifiedExpressionManager.h"
#include "../include/macros.h"
// #include "../include/template_datatypes.h"
// #include "../include/template_scanner.tab.h"
#include "../include/rules_parser_datatypes.h"
#include "../include/rules_parser.tab.h"
#include "../include/add_extra_edges.h" // To be cleaned later

// #include "../include/Rules.h"
#include <ctime>
#include <unistd.h>
#include <math.h> // required for 'sleep' on gcc version >= 4.7
#include <vector>
#include <queue>
#include <chrono>
#include <stdlib.h>

// #include "z3++.h"
// #include "smt/theory_bv.h"

using namespace std;

// Default log file "mylog.txt" is created in the same directory from where the tool is invoked.
// If you are setting unique_prefix (strongly recommended if you are going to run the tool multiple
// times), you should set unique_prefix as the first action when running the tool (either in interactive mode
// or in batch mode).
// This will change the log file name to unique_prefix_mylog.txt
// If you set unique_prefix after running some commands, it is possible that part of your logs are dumped
// in the default "mylog.txt" file and the remainder is dumped in unique_prefix_mylog.txt.
// This is strongly not advised.

ofstream debug_log("mylog.txt");

// anjan

set<int> get_min_cut(int i, int j, map<string, set<int>> &map_i_j_to_mincut_edges)
{
    stringstream ss;

    if (i < j)
        ss << j << ":" << i;
    else
        ss << i << ":" << j;

    return map_i_j_to_mincut_edges[ss.str()];
}

void getMinCutMatrix_dummy(GraphNew *graph, vector<vector<vector<int>>> &minCutMatrix)
{

    int v = graph->get_node_ids().size();
    int e = graph->get_edge_ids().size();

    for (int i = 0; i < v; i++)
    {
        for (int j = 0; j < v; j++)
        {
            int outlist_size, inlist_size;
            outlist_size = graph->get_outlist(i + 1).size();
            inlist_size = graph->get_inlist(j + 1).size();
            if (inlist_size < outlist_size)
                minCutMatrix[i][j] = graph->get_inlist(j + 1);
            else
                minCutMatrix[i][j] = graph->get_outlist(i + 1);
        }
    }
}

// constraint solver CNF related global data
int idx = 1, clauses = 0;                              // clauses is num of clauses, idx stores the number that is to be given to the next variable, thus idx-1 is the total variables
map<string, t_Expression *> getExpressionMap;          // maps variable name as string to expression as t_Expression*
map<t_Expression *, pair<string, int>> getCNFIndexMap; // maps expression as t_Expression* to a pair<variable name as string, internal variable index in CNF format>

void read_z3_output(string inp_filename, string out_filename);
vector<int> add_cnf(vector<int> u, vector<int> v, int &idx, int &clauses, ofstream &fout_CNF_file);
string buildRelationalExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
string buildArithmaticExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
string buildSelectExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
string buildConcatExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
string buildBracketExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);
string buildVariableExpression(string reader, t_ExpressionManager *em, t_Expression *&expr);

t_Expression *build_OR_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em);
t_Expression *build_AND_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em);
t_Expression *build_OR_constraints(t_Expression *e1, t_Expression *e2, int a1, int a2, t_ExpressionManager *em);
t_Expression *build_AND_constraints(t_Expression *e1, t_Expression *e2, int a1, int a2, t_ExpressionManager *em);

bool nonempty_intersection_files(string file1, string file2)
{
    string command1 = "sort -u " + file1 + " > temp-in-1";
    string command2 = "sort -u " + file2 + " > temp-in-2";
    string command3 = "sort temp-in-1 temp-in-2 | uniq -d | wc -l > temp-in-3";

    string command = command1 + " ; " + command2 + " ; " + command3;
    system(command.c_str());

    ifstream fin("temp-in-3");

    if (!fin)
    {
        cerr << "Error: can't open file temp-in-3 file for reading, exiting ..." << endl;
        exit(1);
    }
    else
    {
        int count;
        fin >> count;
        fin.close();
        system("rm temp-in-1 temp-in-2 temp-in-3");
        if (count == 0)
        {
            return false;
        }
        return true;
    }
}

void read_z3_output(string inp_filename, string out_filename)
{
    ifstream fin(inp_filename.c_str());
    ofstream fout(out_filename.c_str(), ofstream::out);

    string s;

    fin >> s;

    if (s != "sat")
    {
        fout << "Solution is not sat\n";
        return;
    }

    int num;

    map<int, bool> soln_map;

    while (fin >> num)
    {
        if (num > 0)
            soln_map[num] = true;
        else
            soln_map[-num] = false;
    }

    for (map<t_Expression *, pair<string, int>>::iterator it = getCNFIndexMap.begin(); it != getCNFIndexMap.end(); it++)
    {
        int var_idx = (it->second).second;
        if (var_idx != -1)
        {
            fout << (it->second).first;
            if (soln_map[var_idx] == true)
                fout << " true\n";
            else
                fout << " false\n";
        }
    }
}

// the bit sequences represented by u and v are added and there sum is represented by the returned vector
// this summing up of u and v and its outcome is written as constraints in the fout_CNF_file

vector<int> add_cnf(vector<int> u, vector<int> v, int &idx, int &clauses, ofstream &fout_CNF_file)
{
    vector<int> r;
    fout_CNF_file << -idx << " 0\n";
    clauses++;

    int carry = idx;
    idx++;

    for (int i = 0; i < u.size(); i++)
    {
        vector<int> v1;
        v1.push_back(u[i]);
        v1.push_back(v[i]);
        v1.push_back(carry);
        r.push_back(idx++);
        carry = idx++;
        for (int a = 0; a < 8; a++)
        {
            int mask = a;
            // fout<<mask<<" --- mask\n";
            int sum = 0;
            for (int j = 0; j < 3; j++)
            {
                sum += mask % 2;
                if (mask % 2 == 0)
                    fout_CNF_file << v1[j] << " ";
                else
                    fout_CNF_file << -v1[j] << " ";

                mask /= 2;
            }

            if (sum % 2 == 0)
                fout_CNF_file << -r[i] << " 0\n";
            else
                fout_CNF_file << r[i] << " 0\n";
            clauses++;

            mask = a;
            sum = 0;
            for (int j = 0; j < 3; j++)
            {
                sum += mask % 2;
                if (mask % 2 == 0)
                    fout_CNF_file << v1[j] << " ";
                else
                    fout_CNF_file << -v1[j] << " ";

                mask /= 2;
            }

            if (sum > 1)
                fout_CNF_file << carry << " 0\n";
            else
                fout_CNF_file << -carry << " 0\n";
            clauses++;
        }
    }
    r.push_back(carry);

    /*	fout<<"bro\n";
            for(int i=0;i<r.size();i++)
            fout<<r[i]<<" ";
            fout<<endl;
     */

    return r;
}

string buildExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    // check for relational operators
    t_Expression *first;
    trimInPlace(reader);
    // assert( ! reader.empty());
    if (reader.empty())
    {
        cout << "empty reader" << endl;
        expr = NULL;
        return reader;
    }
    reader = buildRelationalExpression(reader, em, first);

    trimInPlace(reader);

    if (reader.empty())
    {
        expr = first;
        return reader;
    }
    string oper = reader.substr(0, 2);
    if (oper == "&&" || oper == "||")
        reader = reader.substr(2);
    else
    {
        expr = first;
        return reader;
    }

    while (oper == "&&" || oper == "||")
    {
        t_Expression *second;
        reader = buildRelationalExpression(reader, em, second);
        if (second == NULL)
            break;
        string label;
        if (oper == "&&")
        {
            label = em->m_operatorLabelLogAND;
        }
        else if (oper == "||")
            label = em->m_operatorLabelLogOR;

        first = em->createOneBitExpressionWithTwoOperands(label, first, second);
        trimInPlace(reader);

        if (reader.empty())
            break;
        oper = reader.substr(0, 2);
        reader = reader.substr(2);
    }

    expr = first;
    return reader;
}

string buildRelationalExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    t_Expression *first;
    reader = buildArithmaticExpression(reader, em, first);

    trimInPlace(reader);

    if (reader.empty())
    {
        expr = first;
        return reader;
    }
    string oper = reader.substr(0, 2);
    string label;
    if (oper == ">=")
    {
        label = em->m_operatorLabelGreaterThanOrEqual;
        reader = reader.substr(2);
    }
    else if (oper == "<=")
    {
        label = em->m_operatorLabelLessThanOrEqual;
        reader = reader.substr(2);
    }
    else if (oper == "==")
    {
        label = em->m_operatorLabelLogicalEquality;
        reader = reader.substr(2);
    }
    else
    {
        oper = reader.substr(0, 1);
        if (oper == "<")
        {
            label = em->m_operatorLabelLessThan;
            reader = reader.substr(1);
        }
        else if (oper == ">")
        {
            label = em->m_operatorLabelGreaterThan;
            reader = reader.substr(1);
        }
        else // no relational operator
        {
            expr = first;
            return reader;
        }
    }

    trimInPlace(reader);

    t_Expression *second;
    reader = buildArithmaticExpression(reader, em, second);
    if (second == NULL)
    {
        expr = first;
        // cout<<"No second operand found."<<endl;
        return reader;
    }
    //    if(0)
    //    {
    //        ofstream out("ParsedExpression", ios::app);
    //        out<<"Testing: operands of "<<label<<endl;
    //        string name = "first";
    //        em->printExpressionToFileAsDAG(name, first, &out);
    //        name = "second";
    //        em->printExpressionToFileAsDAG(name, second, &out);
    //        out<<endl;
    //    }

    expr = em->createOneBitExpressionWithTwoOperands(label, first, second);

    // TypeOfExpressionTuple type_info = {TYPE_BOOL, 1};
    // vector<t_Expression*> vec_operands = t_ExpressionManager::buildVector(first,second);
    // expr = em->createExpression(label, vec_operands, type_info);

    return reader;
}

string buildArithmaticExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    t_Expression *first;
    reader = buildSelectExpression(reader, em, first);

    trimInPlace(reader);
    if (reader.empty())
    {
        expr = first;
        return reader;
    }
    string oper = reader.substr(0, 1);
    if (oper == "+" || oper == "-" || oper == "*" || oper == "/" || oper == "%")
    {
        // reader = reader.substr(1);
    }
    else
    {
        expr = first;
        return reader;
    }

    while (oper == "+" || oper == "-" || oper == "*" || oper == "/" || oper == "%")
    {
        reader = reader.substr(1); // skip the operator
        string label;
        if (oper == "+")
            label = em->m_operatorLabelADD;
        else if (oper == "-")
            label = em->m_operatorLabelSUB;
        else if (oper == "*")
            label = em->m_operatorLabelMultiply;
        else if (oper == "/")
            label = em->m_operatorLabelDivide;
        else if (oper == "%")
            label = em->m_operatorLabelModulus;

        t_Expression *second;
        reader = buildSelectExpression(reader, em, second);
        if (second == NULL)
            break;

        vector<t_Expression *> vec_operands = t_ExpressionManager::buildVector(first, second);
        int wid = max(em->getWidth(first), em->getWidth(second));
        // TypeOfExpressionTuple operatorType = {TYPE_UNSIGNED_INTEGER, -1};
        TypeOfExpressionTuple operatorType = {TYPE_UNSIGNED_BITVECTOR, wid};
        first = em->createExpression(label, vec_operands, operatorType);

        trimInPlace(reader);
        if (reader.empty())
            break;
        oper = reader.substr(0, 1);
    }

    expr = first;
    return reader;
}

string buildSelectExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    t_Expression *first;
    reader = buildConcatExpression(reader, em, first);
    trimInPlace(reader);

    if (reader.empty())
    {
        expr = first;
        return reader;
    }
    string oper = reader.substr(0, 1);
    bool foundSelect = false;
    if (oper == "[")
    {
        foundSelect = true;
    }
    else
    {
        expr = first;
        return reader;
    }

#ifdef OLD
    if (foundSelect)
    {
        reader = reader.substr(1);
        t_Expression *ub_expr;
        t_Expression *lb_expr;
        reader = buildConcatExpression(reader, em, ub_expr);
        trimInPlace(reader);

        assert(ub_expr != NULL);
        assert(!reader.empty() && reader[0] == ':');

        reader = reader.substr(1);
        int ub = em->getConstantValuePresentInExpression(ub_expr);

        reader = buildConcatExpression(reader, em, lb_expr);
        assert(lb_expr != NULL);
        int lb = em->getConstantValuePresentInExpression(lb_expr);

        int width = ub - lb + 1;
        vector<t_Expression *> operands = t_ExpressionManager::buildVector(first, lb_expr, ub_expr);
        string label = "select";

        TypeOfExpressionTuple operatorType = {TYPE_UNSIGNED_BITVECTOR, width};
        expr = em->createExpression(label, operands, operatorType);

        assert(reader[0] == ']');
        reader = reader.substr(1);

        return reader;
    }
    else
    {
        assert(false);
    }
#else
    int openidx = reader.find('[');
    int mididx = reader.find(':');
    int closeidx = reader.find(']');

    assert(openidx >= 0 && openidx < mididx);
    assert(mididx < closeidx);
    assert(closeidx <= reader.length());

    string ubStr = reader.substr(openidx + 1, mididx - openidx - 1);
    string lbStr = reader.substr(mididx + 1, closeidx - mididx - 1);

    TypeOfExpressionTuple constIntType = {TYPE_UNSIGNED_INTEGER, -1};
    vector<t_Expression *> operands(3, NULL);
    operands[0] = first;
    operands[1] = em->createConstant(lbStr, constIntType);
    operands[2] = em->createConstant(ubStr, constIntType);

    assert(operands[1] != NULL);
    assert(operands[2] != NULL);

    int ub = stringToInteger(ubStr);
    int lb = stringToInteger(lbStr);
    assert(lb >= 0);
    assert(ub >= lb);
    TypeOfExpressionTuple typ = {TYPE_UNSIGNED_BITVECTOR, ub - lb + 1};
    string label = "select";
    expr = em->createExpression(label, operands, typ);

    return reader.substr(closeidx + 1); // done forget to eat away the expression just parsed
#endif
}

string buildConcatExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    trimInPlace(reader);
    assert(!reader.empty());
    if (reader[0] != '{')
    {
        return buildBracketExpression(reader, em, expr);
    }

    vector<t_Expression *> operands;
    int width = 0;
    while (reader[0] != '}')
    {
        reader = reader.substr(1); // eat away the '{' ',' '}'
        t_Expression *e;
        reader = buildBracketExpression(reader, em, e);
        assert(e != NULL);
        assert(!reader.empty()); //'}' should be left in the string
        operands.push_back(e);
        width += em->getWidth(e);
        assert(reader[0] == ',' || reader[0] == '}');
    }

    assert(operands.empty() == false);
    {
        string label = "concat";
        TypeOfExpressionTuple type = {TYPE_UNSIGNED_BITVECTOR, width};
        expr = em->createExpression(label, operands, type);
    }

    return reader.substr(1); // eat away the '}'
}

string buildBracketExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    trimInPlace(reader);
    bool negate = false;
    if (reader[0] == '!')
    {
        negate = true;
        reader = reader.substr(1);
        trimInPlace(reader);
    }

    t_Expression *first;

    if (reader[0] == '(')
    {
        // cout << "'(' found.:" << reader << endl;

        int brackCount = 1, i;
        for (i = 1; i < reader.size() && brackCount > 0; ++i)
        {
            if (reader[i] == ')')
                brackCount--;
            else if (reader[i] == '(')
                brackCount++;

            if (brackCount == 0)
                break;
        }
        // Now i is the position of corresponding ')'
        string insideBracket = reader.substr(1, i - 1);
        reader = reader.substr(i + 1);

        // cout << "InsideBracket:" << insideBracket << endl;
        insideBracket = buildExpression(insideBracket, em, first);
        trimInPlace(insideBracket);

        if (insideBracket.empty() == false)
        {
            cout << "ERROR: Incomplete expression found:" << insideBracket << endl;
        }
        // cout << "Expression with bracket parsed. Remaining:" << reader << endl;

        trimInPlace(reader);
    }
    else
    {
        reader = buildVariableExpression(reader, em, first);
    }

    if (negate)
    {
        expr = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, first);
    }
    else
        expr = first;

    return reader;
}

string buildVariableExpression(string reader, t_ExpressionManager *em, t_Expression *&expr)
{
    trimInPlace(reader);
    // cout << "BuildVariable:" << reader << endl;
    if (reader[0] == ')')
    {
        // cout << "Closing parenthesis found" << endl;
        expr = NULL;
        return reader;
    }
    string operand;
    int i = 0;
    static string specialChars("_$.#@");
    for (int i = 0; i < reader.size(); ++i)
    {
        char c = reader[i];
        if (isalnum(c) || specialChars.find(c) != string::npos)
        {
            operand += c;
        }
        else
            break;
    }

    // const char* strPtr = operand.c_str();
    // char* end;
    // long val = strtol(strPtr, &end, 0);
    // if (end == strPtr)
    reader = reader.substr(operand.size());
    if (!isdigit((char)operand[0]))
    {
        // Not a number'
        int width, ub, lb;
        string symName = operand;
        cout << "Processing " << operand << endl;
        operand = extractNumberAtEnd(operand, lb);
        operand = extractNumberAtEnd(operand, ub);
        width = ub - lb + 1;
        assert(width > 0);
        TypeOfExpressionTuple symbolType = {TYPE_UNSIGNED_BITVECTOR, width};
        // cout << "Creating symbol:" << symName << " of size " << width << endl;
        expr = em->createSymbol(symName, symbolType);
    }
    else
    {
        // cout << "Creating constant:" << operand << " ";

        {
            if (operand.find("0b") != 0) // not starts with 0b
            {
                // integer
                int op;
                if (operand.find("0x") == 0)
                {
                    istringstream ss(operand);
                    ss >> op;
                }
                else
                {
                    op = stringToInteger(operand);
                }

                string bitv = integerToBinaryString(op);
                TypeOfExpressionTuple constantType = {TYPE_UNSIGNED_BITVECTOR, (int)bitv.size()}; // .size() returns size_t - typecasted to int by sukanya on 26 Feb 2016
                expr = em->createConstant(bitv, constantType);
                // cout<<"type integer to "<<bitv.size()<<" bit bitvector"<<endl;
            }
            else
            {
                operand = operand.substr(2);
                TypeOfExpressionTuple constantType = {TYPE_UNSIGNED_BITVECTOR, (int)operand.size()}; // .size() returns size_t - typecasted to int by sukanya on 26 Feb 2016
                expr = em->createConstant(operand, constantType);
                // cout<<"type bitvector"<<endl;
            }
        }
        // TypeOfExpressionTuple constantType = {TYPE_UNSIGNED_BITVECTOR, operand.size()};
        // expr = em->createConstant(operand, constantType);
    }

    // reader = reader.substr(operand.size()); //skip the operand
    // cout<<"  Remaining:"<<reader<<endl;
    return reader;
}

// function added by sai krishna
// s1 = (not e1) if a1==1
// s2 = (not e2) if a2==1
// retuns the expression (or e1 e2)

t_Expression *build_OR_constraints(t_Expression *e1, t_Expression *e2, int a1, int a2, t_ExpressionManager *em)
{
    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};

    vector<t_Expression *> v;

    if (a1 == 1)
        e1 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, e1);

    if (a2 == 1)
        e2 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, e2);

    v.push_back(e1);
    v.push_back(e2);

    return em->createExpression(em->m_operatorLabelLogOR, v, te);
}

// function added by sai krishna
// s1 = (not e1) if a1==1
// s2 = (not e2) if a2==1
// retuns the expression (and e1 e2)

t_Expression *build_AND_constraints(t_Expression *e1, t_Expression *e2, int a1, int a2, t_ExpressionManager *em)
{
    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};

    vector<t_Expression *> v;

    if (a1 == 1)
        e1 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, e1);

    if (a2 == 1)
        e2 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, e2);

    v.push_back(e1);
    v.push_back(e2);

    return em->createExpression(em->m_operatorLabelLogAND, v, te);
}

// function added by sai krishna
// s1 = (not s1) if a1==1
// s2 = (not s2) if a2==1
// retuns the expression (or s1 s2)

t_Expression *build_OR_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em)
{
    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};

    if (getExpressionMap.find(s1) == getExpressionMap.end() || getExpressionMap.find(s2) == getExpressionMap.end())
    {
        cout << "no....!!! " << s1 << " " << s2 << endl;
        // shoudl add some cerror here later
        return NULL;
    }

    t_Expression *expr1, *expr2;
    vector<t_Expression *> v;

    expr1 = getExpressionMap[s1];
    expr2 = getExpressionMap[s2];

    if (a1 == 1)
        expr1 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expr1);

    if (a2 == 1)
        expr2 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expr2);

    v.push_back(expr1);
    v.push_back(expr2);

    return em->createExpression(em->m_operatorLabelLogOR, v, te);
}

// function added by sai krishna
// s1 = (not s1) if a1==1
// s2 = (not s2) if a2==1
// returns the expression (and s1 s2)

t_Expression *build_AND_constraints(string s1, string s2, int a1, int a2, t_ExpressionManager *em)
{
    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};

    if (getExpressionMap.find(s1) == getExpressionMap.end() || getExpressionMap.find(s2) == getExpressionMap.end())
    {
        cout << "no....!!! " << s1 << " " << s2 << endl;
        // shoudl add some cerror here later
        return NULL;
    }

    t_Expression *expr1, *expr2;
    vector<t_Expression *> v;

    expr1 = getExpressionMap[s1];
    expr2 = getExpressionMap[s2];

    if (a1 == 1)
        expr1 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expr1);

    if (a2 == 1)
        expr2 = em->createOneBitExpressionWithOneOperand(em->m_operatorLabelLogNOT, expr2);

    v.push_back(expr1);
    v.push_back(expr2);

    return em->createExpression(em->m_operatorLabelLogAND, v, te);
}

t_Expression *build_XOR_constraints(t_Expression *a, t_Expression *b, t_ExpressionManager *em)
{
    // a XOR b = ab' + a'b
    t_Expression *a_and_b_prime = build_AND_constraints(a, b, 0, 1, em);
    t_Expression *a_prime_and_b = build_AND_constraints(a, b, 1, 0, em);
    return build_OR_constraints(a_and_b_prime, a_prime_and_b, 0, 0, em);
}

t_Expression *build_EQUIV_constraint(t_Expression *a, t_Expression *b, t_ExpressionManager *em)
{
    // a EQUIV b = a implies b AND b implies A
    t_Expression *a_implies_b = build_OR_constraints(a, b, 1, 0, em);
    t_Expression *b_implies_a = build_OR_constraints(b, a, 1, 0, em);
    return build_AND_constraints(a_implies_b, b_implies_a, 0, 0, em);
}

t_Expression *build_AND_constraints_from_vector(vector<t_Expression *> &vec, t_ExpressionManager *em)
{
    t_Expression *to_return = NULL;
    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};
    if (!vec.empty())
    {
        to_return = em->createExpression(em->m_operatorLabelLogAND, vec, te);
    }

    /*if(!vec.empty()){
      to_return = vec[0];
      for(vector<t_Expression*>::iterator itr = vec.begin()+1; itr != vec.end(); ++itr){
      t_Expression* temp = build_AND_constraints(to_return, *itr, 0, 0, em);
      to_return = temp;
      }
      }
      else{
      cerr << "vec is empty" << endl;
      }
      if(!to_return){
      cerr << "Error: returning NULL" << endl;
      }*/
    return to_return;
}

t_Expression *build_OR_constraints_from_vector(vector<t_Expression *> &vec, t_ExpressionManager *em)
{
    t_Expression *to_return = NULL;
    TypeOfExpressionTuple te = {TYPE_UNSIGNED_BITVECTOR, 1};
    if (!vec.empty())
    {
        to_return = em->createExpression(em->m_operatorLabelLogOR, vec, te);
    }

    /*if(!vec.empty()){
      to_return = vec[0];
      for(vector<t_Expression*>::iterator itr = vec.begin()+1; itr != vec.end(); ++itr){
      t_Expression* temp = build_OR_constraints(to_return, *itr, 0, 0, em);
      to_return = temp;
      }
      }
      else{
      cerr << "vec is empty" << endl;
      }
      if(!to_return){
      cerr << "Error: returning NULL" << endl;
      }*/
    return to_return;
}

t_Expression *get_expression_sequence_positive_constraint(vector<t_Expression *> &nid_dist_variable_seq, t_ExpressionManager *em)
{
    t_Expression *to_return = build_OR_constraints_from_vector(nid_dist_variable_seq, em);
    // if the value is non zero then atleast one bit should be True, thus ORing all bits must give True
    return to_return;
}

#define SUBTRACT_MODE 0
#define INTERSECT_MODE 1
#define MERGE_MODE 2
// #define DOTPATHSTRING "/usr/local/bin/dot"
#define NUM_KEGG_FILES 39

int Node::id_counter = 1;
int Graph::graph_id_counter = 1;

extern int yyparse();
extern FILE *yyin;
extern list<rule_t *> list_of_rules;

void display_commands()
{
    cout << endl;
    cout << "Commands and usage: " << endl;
    cout << endl;
    cout << "help \t(or h) " << endl;
    cout << "read_graph_xml \t(or rgx) \t<filename>" << endl;
    cout << "read_graph_sbml (or rgs) \t<filename>" << endl;
    cout << "read_graph_ppin (or rgp) \t<filename>" << endl;
    cout << "read_graph_htri (or rgh) \t<filename>" << endl;
    cout << "read_graph_reactome_all (or rgr) \t<filename>" << endl;
    cout << "read_phenotypes (or pheno)" << endl;
    cout << "size \t(or sz) \t<graph-id>" << endl;
    cout << "find_intersecting_nodes (or fin) \t<graph-id1> \t<graph-id2>" << endl;
    cout << "find_node \t(or fn)" << endl;
    cout << "display \t(or dis) \t<graph-id>" << endl;
    cout << "convert \t(or cnv) \t<graph-id>" << endl; // written by uddipaan
    cout << "reach \t(or rch) \t<graph-id>" << endl;
    cout << "print_graph_info \t(or pgi) \t<graph-id>" << endl;
    cout << "print_graph_man_info \t(or pgmi) " << endl;
    cout << "merge \t(or mg)" << endl;
    cout << "merge_from_file (or mff) \t<filename>" << endl;
    cout << "genesis \t(or gen) \t<graph-id>" << endl;
    cout << "write_graph_xml (or wgx) \t<graph-id>" << endl;
    cout << "read_rules \t(or rr) \t<rule_file>" << endl;
    cout << "generate_constraints \t(or gencn) " << endl;
    cout << "solve" << endl;
    cout << "print_GErels \t(or pgrel) \t<graph-id>" << endl;
    cout << "subgraph \t\t(or sg) \t<graph-id> \t<file_name>" << endl;
    cout << "select \t(or sel) \t<graph-id> \t<file_name>" << endl;
    cout << "get_assignments_z3 \t(or get_assg_z3) \t<z3_cnf_result_file> \t<output_file>" << endl;
    cout << "get_all_solutions \t(or get_all_solns) " << endl;
    cout << "call_ApproxMC \t\t(or approxmc)" << endl;
    cout << "process_microarr_data \t(or pmd) \t<graph-id>" << endl;
    cout << "add_projected_variables \t(or proj)" << endl;
    cout << "clear_expression_maps \t\t(or clxm)" << endl;
    cout << "fwd_bkwd_rch \t\t\t(or fb_rch) \t<graph-id>" << endl;
    cout << "kegg_consistency_check \t\t(or kcc) \t<graph-id>" << endl;
    cout << "list_pathways_with_nodes \t(or lpwn ) \t<filename>" << endl;
    cout << "mincut \t(or mc) \t<graoh-id>" << endl;
    cout << "connect \t<graph-id>" << endl;
    cout << "clr" << endl;
    cout << "exit" << endl;
    cout << endl;
}

// print a generic map ---useful as there are many maps in the code

template <typename Key, typename Val>
void print_map(map<Key, Val> &map2print)
{
    for (typename map<Key, Val>::iterator itr = map2print.begin(); itr != map2print.end(); ++itr)
    {
        cout << itr->first << " ==> " << itr->second << endl;
    }
}

std::string concatenate_strings(std::vector<std::string> vec_strings, std::string delim)
{
    if (vec_strings.empty())
    {
        return "";
    }
    std::string to_return = *vec_strings.begin();
    for (std::vector<std::string>::iterator itr = vec_strings.begin() + 1; itr != vec_strings.end(); itr++)
    {
        to_return = to_return + delim + *itr;
    }
    return to_return;
}

std::string concatenate_strings2(std::vector<std::string> vec_strings, std::string delim)
{
    if (vec_strings.empty())
    {
        return "-";
    }
    std::string to_return = *vec_strings.begin();
    for (std::vector<std::string>::iterator itr = vec_strings.begin() + 1; itr != vec_strings.end(); itr++)
    {
        to_return = to_return + delim + *itr;
    }
    return to_return;
}

// split string based on a delimiter

set<string> split_string(string name_str, string delim)
{
    set<string> splitted_strings;
    unsigned start_pos = 0;
    while (start_pos < name_str.length())
    {
        std::size_t found = name_str.find(delim, start_pos);
        if (found != std::string::npos)
        {
            splitted_strings.insert(name_str.substr(start_pos, found - start_pos));
            start_pos = found + delim.length();
        }
        else
        {
            splitted_strings.insert(name_str.substr(start_pos));
            break;
        }
    }
    return splitted_strings;
}

void split_string_into_list(string name_str, string delim, list<string> &list_of_strings)
{

    unsigned start_pos = 0;
    while (start_pos < name_str.length())
    {
        std::size_t found = name_str.find(delim, start_pos);
        if (found != std::string::npos)
        {
            (list_of_strings).push_back(name_str.substr(start_pos, found - start_pos));
            start_pos = found + delim.length();
        }
        else
        {
            (list_of_strings).push_back(name_str.substr(start_pos));
            break;
        }
    }
}

bool compare_pair_on_second_ascending(const pair<int, int> &i, const pair<int, int> &j)
{
    if (i.second == j.second)
        return i.first > j.first;
    return i.second < j.second;
}

bool compare_pair_on_second_descending(const pair<int, int> &i, const pair<int, int> &j)
{
    if (i.second == j.second)
        return i.first < j.first;
    return i.second > j.second;
}

bool compare_pair_on_second_descending_int_float(const pair<int, float> &i, const pair<int, float> &j)
{
    if (i.second == j.second)
        return i.first < j.first;
    return i.second > j.second;
}

bool compare_pair_on_second_descending_int_double(const pair<int, double> &i, const pair<int, double> &j)
{
    if (i.second == j.second)
        return i.first < j.first;
    return i.second > j.second;
}

bool compare_pair_on_second_descending_string_to_int(const pair<string, int> &i, const pair<string, int> &j)
{
    if (i.second == j.second)
        // return strcmp(i.first.c_str(), j.first.c_str()) < 0;
        return i.first < j.first;
    return i.second > j.second;
}

string remove_substr(string base_string, string substr_to_remove)
{
    std::size_t n = substr_to_remove.length();
    for (std::size_t i = base_string.find(substr_to_remove); i != std::string::npos; i = base_string.find(substr_to_remove))
    {
        base_string.erase(i, n);
    }
    return base_string;
}

string IntToString(int num)
{
    ostringstream ss;
    ss << num;
    return ss.str();
}

string FloatToString(float num)
{
    ostringstream ss;
    ss << fixed << setprecision(PRECISION) << num;
    return ss.str();
}

int main(int argc, char *argv[])
{
    bool b_mode = false;
    int cmdCount = inf, cnt_cmd = 0;
    vector<string> commands;
    string argv_0(argv[0]);

    if (argc > 1)
    {
      string argv_1(argv[1]);
      string bmode = "-b";
      string dmode = "-d";
      string hmode = "-h";

        if (argv_1.compare(bmode) == 0)
        {
            ifstream the_file(argv[2]);
            if (!the_file.is_open())
                cout << "Could not open file" << endl;
            else
            {
                string s;
                cmdCount = 0;
                while (getline(the_file, s))
                {
                    // read the commands
                    commands.push_back(s);
                    cmdCount++;
                }
                the_file.close();
                // print the command -- can be taken in place of cin>>
                // for(int i=0;i<cmdCount;i++)
                //      cout<<commands[i]<<endl;
                cout << "Entering batch mode" << endl;
                b_mode = true;
            }
        }
        else if (argv_1.compare(dmode) == 0)
        {
	  cout << "Interactive debug mode not supported in this version" << endl;
	  cout << "Run in batch mode as \"" << argv_0 << " -b <batch_command_file_name>\"" << endl;
	  cout << endl << "Partial listing of commands that can be used in batch_command_file:" << endl;
	  display_commands();
	}
        else if (argv_1.compare(hmode) == 0)
        {
	  cout << "Run as \"" << argv_0 << " -b <batch_command_file_name>\"" << endl;
	  cout << endl << "Partial listing of commands that can be used in batch_command_file:" << endl;
	  display_commands();
	  exit(0);
	}
	else {
	  cout << "Run the tool as \"" << argv_0 << " -b <batch_command_file_name>\"" << endl;
	  cout << endl << "Partial listing of commands that can be used in batch_command_file:" << endl;
	  display_commands();
	  exit(-1);
        }
    }
    else {
      cout << "Run the tool as \"" << argv_0 << " -b <batch_command_file_name>\"" << endl;
      cout << endl << "Partial listing of commands that can be used in batch_command_file:" << endl;
      display_commands();
      exit(-1);
    }

    if (b_mode)
    {
        //  cout << "-------------------------------" << endl;
        //  cout << commands[cnt_cmd] << endl;
        //  cout << "-------------------------------" << endl;
    }
    // display_commands();
    const string prompt = "sysbio> ";

    // string config_file_path(SUPPORTING_FILES_DIR);
    // config_file_path += "/config.txt";
    t_ConfigurationOptions config_options("");
    t_ExpressionManager *em = new t_ExpressionManager(config_options);

    Z3Solver *zs = new Z3Solver();
    z3::context c;
    z3::solver s(c);
    z3::expr_vector all_z3_expr_vec(c);

    // zs->test_method(c, s);

    // Rules* rules_ptr = new Rules; //for storing rules index wise starting at 1
    std::map<unsigned int, std::set<Node *>> map_potential_conflict_nodes;

    std::map<int, std::vector<int>> map_nid_to_unrolled_graph_nids;
    std::map<int, std::vector<int>> map_eid_to_unrolled_graph_eids;

    time_t time1, time2, time3, time4;
    double time_in_sec;
    string command;

    std::cout.precision(PRECISION);
    std::cout.setf(std::ios::fixed);

    std::map<string, string> macro_defs_in_batch_inp;
    GraphManagerNew *graph_man = NULL;

    while (1)
    {

        // tokenize commands[cmdCount] and put those into another array
        if (cnt_cmd == cmdCount)
            b_mode = false;
        char *myString = const_cast<char *>(commands[cnt_cmd].c_str());
        char *p = strtok(myString, " ");
        vector<string> token_cmd(1000000);
        int token_cnt = 0;
        if (b_mode)
        {
            cnt_cmd++;
            if (p == NULL)
            { // Empty line in batch input file
                continue;
            }
            if (strcmp(p, ";;") != 0)
            {
                // cout << "-------------------------------" << endl;
                while (p)
                {
                    // cout << p << " ";
                    token_cmd[token_cnt++] = p;
                    p = strtok(NULL, " ");
                }
                //  cout << endl;
                //  cout << "-------------------------------" << endl;
            }
            else
            {
                token_cmd[0] = ";;";
            }
        }

        if (!b_mode)
        {
            std::cout << prompt;
            std::cin >> command;
        }
        else
        {
            command = token_cmd[0];
            if ((command[0] == ';') && (command[1] == ';'))
            { // for writing comments in batch input file
                continue;
            }
            else
            {
                // Expanding macros, if any, in the command line
                int tok_index = 0;
                bool macro_found;

                // Added by summer intern Nitya, debugged by Supratik
                int M, N, k, j;
                string str1, str2;

                while (tok_index < token_cnt)
                {
                    str1 = token_cmd[tok_index];
                    M = str1.length();
                    int i = 0;
                    while (i < M)
                    {
                        if (str1[i] == '$')
                        {
                            macro_found = false;
                            map<string, string>::iterator itr;
                            for (itr = macro_defs_in_batch_inp.begin();
                                 (!macro_found) && (itr != macro_defs_in_batch_inp.end()); ++itr)
                            {
                                str2 = itr->first;
                                N = str2.length();
                                if (M - i < N)
                                    continue; // No match possible
                                for (j = 0; j < N; j++)
                                {
                                    if (str1[i + j] == str2[j])
                                    {
                                        if (j == (N - 1))
                                        {
                                            macro_found = true;
                                            str1.replace((i), (N), itr->second);
                                            i += ((itr->second).length() - 1);
                                            M = str1.length();
                                        }
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        i++;
                    }

                    token_cmd[tok_index] = str1;
                    tok_index++;
                }

                if (command == "let")
                {
                    // cout << "Processing let" << endl;
                    string macro_key = token_cmd[1];
                    if (macro_key[0] != '$')
                    {
                        cerr << "\"let\" to be used with macro key starting with $" << endl;
                        cerr << macro_key << "doesn't start with $; Skipping ..." << endl;
                        continue;
                    }

                    string macro_value = token_cmd[2];
                    if (macro_defs_in_batch_inp.find(macro_key) == macro_defs_in_batch_inp.end())
                    {
                        if (macro_value == "#LASTGRAPHNUM")
                        {
                            if (graph_man == NULL)
                            {
                                cerr << "Graph Manager not created yet.  Ignoring LASTGRAPHNUM directive." << endl;
                            }
                            else
                            {
                                macro_value = to_string(graph_man->graph_id_count);
                            }
                        }
                        macro_defs_in_batch_inp.insert(pair<string, string>(macro_key, macro_value));
                        // cout << "Inserted " << macro_defs_in_batch_inp[macro_key] << " for key " << macro_key << endl;
                    }
                    else
                    {
                        cout << "Macro definition for " << macro_key << "already present. Ignoring value " << macro_value << endl;
                    }
                    continue;
                }

                // while (tok_index < token_cnt) {
                //   if (macro_defs_in_batch_inp.find(token_cmd[tok_index]) != macro_defs_in_batch_inp.end()) {
                //     macro_found = true;
                //     token_cmd[tok_index] = macro_defs_in_batch_inp[token_cmd[tok_index]];
                //   }
                //   tok_index++;
                // }
                // if (macro_found) {
                //  cout << "Macro expanded: " << endl;
                // cout << "-------------------------------" << endl;
                tok_index = 0;
                while (tok_index < token_cnt)
                {
                    cout << token_cmd[tok_index++] << " ";
                }
                cout << endl;
                cout << "-------------------------------" << endl;
                // }
            }
        }

        if (command == "start")
        { // Hack to make the earlier way of invoking graph manager work
          // which required the first four lines of batch command file to contain
          // specific file names in a specific order.
            vector<string> graph_manager_parameter_files;
            int i = 0;
            graph_manager_parameter_files.push_back(token_cmd[1]);
            graph_manager_parameter_files.push_back(token_cmd[2] + " -1");
            graph_manager_parameter_files.push_back(token_cmd[3]);
            graph_manager_parameter_files.push_back(token_cmd[4]);
            graph_man = new GraphManagerNew(graph_manager_parameter_files, i, b_mode);
            continue;
        }
        else if (command == "addx")
        {
            if (token_cnt < 3) {
                cout << "Error: addx called with insufficient arguments"<<endl;
            }
            else {
                string inp_file_name = token_cmd[1];
                string out_file_name = token_cmd[2];

                c_SubGraph oG(inp_file_name, out_file_name);
                oG.readEdges();
                oG.writeEdges();

            }
            continue;
        }
        else if (command == "exec")
        {
            string system_cmd = "";
            int tok_index = 1;
            while (tok_index < token_cnt)
            {
                system_cmd += token_cmd[tok_index];
                system_cmd += " ";
                tok_index++;
            }
            int ret_val = system(system_cmd.c_str());
            continue;
        }
        else if ((command == "unique_prefix") || (command == "upfx"))
        {
            std::string unique_prefix = token_cmd[1];
            // graph_man->unique_prefix = token_cmd[1];
            debug_log.close();
            string debug_log_filename_with_unique_prefix = unique_prefix + "_mylog.txt";
            debug_log.open(debug_log_filename_with_unique_prefix);
            graph_man->set_unique_prefix(unique_prefix);
            continue;
        }
        else if (command == "read_graph_xml" || command == "rgx")
        {
            string filename;
            int cmd_index = 1;
            if (!b_mode)
                cin >> filename;
            else
                filename = token_cmd[cmd_index++];

            int thres = 1;
            bool tool_written = false;
            char tool_written_ch;
            if (!b_mode)
            {
                cout << "Tool-written: (y/n): ";
                cin >> tool_written_ch;

                cout << "Split node threshold: ";
                cin >> thres;
            }
            else
            {
                tool_written_ch = token_cmd[cmd_index++][0];
                thres = stoi(token_cmd[cmd_index++]);
            }

            if (tool_written_ch == 'y' || tool_written_ch == 'Y')
            {
                tool_written = true;
            }

            time(&time1);

            vector<string> MAPK_merged_ids;
            map<string, set<string>> MAPK_node_to_pathway_id_map;
            cout << "Creating graph..." << endl;
            // int gid = graph_man->read_graph_xml_file(filename, tool_written);
            int gid = graph_man->read_graph_xml_file(filename, thres, MAPK_merged_ids, MAPK_node_to_pathway_id_map, tool_written);
            cout << "Done!" << endl;

            time(&time2);

            if (gid != -1)
            {
                cout << "New graph id: " << gid << endl;
            }

            time_in_sec = difftime(time2, time1);

            cout << "Time taken (in sec): " << time_in_sec << endl;
        }

        else if (command == "name")
        {
            int graphnum;
            string filename = "EGFR_down_reg_names";

            graph_man->get_hsa_from_name(filename);
        }
        //        else if (command == "create_reactome_map" || command == "rmap") {
        //            string filename;
        //            cin >> filename;
        //            graph_man->create_map_from_sbml(filename);
        //        }

        else if (command == "create_kegg_map_from_graph" || command == "kmap")
        {
            int graphnum;
            string filename = "all_names_new.txt";

            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {

                ofstream fout(filename.c_str());
                cout << "Number of nodes: " << graph->get_node_ids().size();
                vector<int> nodes = graph->get_node_ids();
                for (vector<int>::iterator itr = nodes.begin(); itr != nodes.end(); itr++)
                {
                    int curr_nid = *itr;
                    fout << graph->get_rep_id_from_nid(curr_nid) << "\t" << graph_man->kegg_hsa_id_to_display_name_map[graph->get_rep_id_from_nid(curr_nid)] << endl;
                }
            }
        }

        else if (command == "create_display_names_from_graph" || command == "dmap")
        {
            int graphnum;
            string filename = "all_full_names_new.txt";

            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {

                ofstream fout(filename.c_str());
                cout << "Number of nodes: " << graph->get_node_ids().size();
                vector<int> nodes = graph->get_node_ids();
                for (vector<int>::iterator itr = nodes.begin(); itr != nodes.end(); itr++)
                {
                    int curr_nid = *itr;
                    fout << graph->get_rep_id_from_nid(curr_nid) << "\t" << concatenate_strings2(graph->get_all_display_ids_of_node(curr_nid), ",") << endl;
                }
            }
        }

        //        else if (command == "read_graph_sbml" || command == "rgs") {
        //            string filename;
        //            cin >> filename;
        //            int gid = graph_man->read_graph_from_sbml(filename);
        //            if (gid != -1) {
        //                cout << "New graph id: " << gid << endl;
        //            }
        //
        //        }

        else if (command == "read_graph_ppin" || command == "rgp")
        {

            string filename;
            cin >> filename;
            time(&time1);
            int gid = graph_man->create_graph_PPIN(filename);
            time(&time2);
            if (gid != -1)
            {
                cout << "New graph id: " << gid << endl;
            }
            else
            {
                cerr << "Could not create PPIN" << endl;
            }
            time_in_sec = difftime(time2, time1);
            cout << "Time taken (in sec): " << time_in_sec << endl;
        }

        else if (command == "read_graph_htri" || command == "rgh")
        {

            string filename;
            cin >> filename;
            int gid = graph_man->create_graph_HTRI(filename);
            if (gid != -1)
            {
                cout << "New graph id: " << gid << endl;
            }
            else
            {
                cerr << "Could not create HTRI" << endl;
            }
        }

        else if (command == "read_graph_reactome_all" || command == "rgr")
        {

            string filename;
            cin >> filename;
            int gid = graph_man->create_graph_reactome_all(filename);
            if (gid != -1)
            {
                cout << "New graph id: " << gid << endl;
            }
            else
            {
                cerr << "Could not create Reactome all interactions network" << endl;
            }
        }

        else if (command == "read_phenotypes" || command == "pheno")
        {
            cout << "Enter filename containing phenotype edges: ";
            string filename;
            cin >> filename;
            int gid = graph_man->add_phenotype_edges(filename);
            if (gid != -1)
            {
                cout << "New graph id: " << gid << endl;
            }
            else
            {
                cerr << "Could not create graph with phenotype info" << endl;
            }
        }

        else if (command == "size1" || command == "sz1")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {
                cout << "Number of nodes: " << graph->get_node_ids().size() << endl;
                cout << "Number of edges: " << graph->get_edge_ids().size() << endl;
            }
        }

        else if (command == "size" || command == "sz")
        {
            int graphnum;

            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {
                int genes = 0;
                int complex = 0;
                int compound = 0;
                int others = 0;
                int isolated = 0;

                cout << "Number of nodes: " << graph->get_node_ids().size();
#ifdef DEBUG_FLAG
                debug_log << "Number of nodes: " << graph->get_node_ids().size();
#endif
                vector<int> nodes = graph->get_node_ids();
                for (vector<int>::iterator itr = nodes.begin(); itr != nodes.end(); itr++)
                {
                    if (graph->is_isolated_node(*itr))
                        isolated++;

                    if (graph->get_node_type(*itr) == "gene")
                        genes++;
                    else if (graph->get_node_type(*itr) == "group")
                        complex++;
                    else if (graph->get_node_type(*itr) == "compound")
                        compound++;
                    else
                        others++;
                }
                cout << " (" << genes << " genes, ";
                cout << complex << " complexes, ";
                cout << compound << " compounds, ";
                cout << others << " others) ";
                cout << isolated << " isolated" << endl;
                cout << "Number of edges: " << graph->get_edge_ids().size() << endl;
#ifdef DEBUG_FLAG
                debug_log << " (" << genes << " genes, ";
                debug_log << complex << " complexes, ";
                debug_log << compound << " compounds, ";
                debug_log << others << " others)" << endl;
                debug_log << "Number of edges: " << graph->get_edge_ids().size() << endl;
#endif
            }
        }

        else if (command == "display" || command == "dis")
        {
            int graphnum;
            string dis_name;
            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph != NULL)
            {
                std::string color_map_filename;
                if (!b_mode)
                {
                    cout << "Enter color map filename (0 to skip): ";
                    cin >> color_map_filename;
                }
                else
                {
                    color_map_filename = token_cmd[2];
                }
                graph->display_graph(color_map_filename, graph_man);
            }
            else
            {
                cerr << "Graph id " << graphnum << " is not present" << endl;
            }
        }

        else if (command == "display_rich" || command == "dr")
        {
            int graphnum;
            string dis_name;
            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph != NULL)
            {
                std::string color_map_filename;
                if (!b_mode)
                {
                    cout << "Enter color map filename (0 to skip): ";
                    cin >> color_map_filename;
                    cout << "\nEnter a file name for the graph display without .dot extension (e.g. mygraph): ";
                    cin >> dis_name;
                }
                else
                {
                    color_map_filename = token_cmd[2];
                    dis_name = token_cmd[3];
                }

                graph->display_graph_richer(color_map_filename, graph_man, dis_name);
            }
            else
            {
                cerr << "Graph id " << graphnum << " is not present" << endl;
            }
        }

        else if (command == "display_rich_with_edges" || command == "dre")
        {
            int graphnum;
            string file_prefix;
            string dis_name;
            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            map<int, int> eid_to_color_map;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph != NULL)
            {
                string node_color_map_filename, edge_color_map_filename;
                string active_nodes_filename, inactive_nodes_filename;
                string relaxed_nodes_filename, relaxed_edges_filename;
                if (!b_mode)
                {
                    cout << "Enter file prefix: ";
                    cin >> file_prefix;
                    cout << "Enter node color map filename (0 to skip): ";
                    cin >> node_color_map_filename;
                    cout << "Enter edge color map filename (0 to skip): ";
                    cin >> edge_color_map_filename;
                    cout << "Enter active nodes filename (0 to skip): ";
                    cin >> active_nodes_filename;
                    cout << "Enter inactive nodes filename (0 to skip): ";
                    cin >> inactive_nodes_filename;
                    cout << "Enter relaxed nodes filename (0 to skip): ";
                    cin >> relaxed_nodes_filename;
                    cout << "Enter relaxed edges filename (0 to skip): ";
                    cin >> relaxed_edges_filename;
                    cout << "\nEnter a file name for the graph display without .dot extension (e.g. mygraph): ";
                    cin >> dis_name;
                }
                else
                {
                    file_prefix = token_cmd[2];
                    node_color_map_filename = token_cmd[3];
                    edge_color_map_filename = token_cmd[4];
                    active_nodes_filename = token_cmd[5];
                    inactive_nodes_filename = token_cmd[6];
                    relaxed_nodes_filename = token_cmd[7];
                    relaxed_edges_filename = token_cmd[8];
                    dis_name = token_cmd[9];
                }

                file_prefix = file_prefix + "_";

                if (active_nodes_filename != "0")
                {
                    active_nodes_filename = OUTPUT_FILES_DIR_PREFIX + file_prefix + active_nodes_filename;
                }
                if (inactive_nodes_filename != "0")
                {
                    inactive_nodes_filename = OUTPUT_FILES_DIR_PREFIX + file_prefix + inactive_nodes_filename;
                }
                if (relaxed_nodes_filename != "0")
                {
                    relaxed_nodes_filename = OUTPUT_FILES_DIR_PREFIX + file_prefix + relaxed_nodes_filename;
                }
                if (relaxed_edges_filename != "0")
                {
                    relaxed_edges_filename = OUTPUT_FILES_DIR_PREFIX + file_prefix + relaxed_edges_filename;
                }
                dis_name = OUTPUT_FILES_DIR_PREFIX + file_prefix + dis_name;

                graph->display_graph_richer_with_edge_color(node_color_map_filename, edge_color_map_filename, eid_to_color_map, active_nodes_filename, inactive_nodes_filename, relaxed_nodes_filename, relaxed_edges_filename, graph_man, dis_name);
            }
            else
            {
                cerr << "Graph id " << graphnum << " is not present" << endl;
            }
        }

        else if (command == "display_ghtree" || command == "dgh")
        {
            int graphnum;
            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[1]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph != NULL)
            {
                std::string color_map_filename;
                if (!b_mode)
                {
                    cout << "Enter color map filename (0 to skip): ";
                    cin >> color_map_filename;
                }
                else
                {
                    color_map_filename = token_cmd[2];
                }
                graph->display_gomoryhu_tree(color_map_filename, graph_man);
            }
            else
            {
                cerr << "Graph id " << graphnum << " is not present" << endl;
            }
        }

        else if (command == "find_intersecting_nodes" || command == "fin")
        {
            int graphnum1, graphnum2;
            cin >> graphnum1;
            if (graphnum1 == 0)
                graphnum1 = graph_man->graph_id_count;
            cin >> graphnum2;
            if (graphnum2 == 0)
                graphnum2 = graph_man->graph_id_count;

            GraphNew *graph1 = graph_man->get_graph(graphnum1);
            GraphNew *graph2 = graph_man->get_graph(graphnum2);
            if (graph1 == NULL)
                cerr << "Invalid graph id " << graphnum1 << endl;
            else if (graph2 == NULL)
                cerr << "Invalid graph id " << graphnum2 << endl;
            else
            {
                std::vector<int> nids_g1 = graph1->get_node_ids();
                std::vector<int> nids_g2 = graph2->get_node_ids();
                std::set<string> rep_ids_g1, rep_ids_g2, rep_ids_intersection;
                std::vector<int>::iterator itr;
                std::set<string>::iterator itr_str;
                for (itr = nids_g1.begin(); itr != nids_g1.end(); itr++)
                {
                    std::string rep_id = graph1->get_rep_id_from_nid(*itr);
                    rep_ids_g1.insert(rep_id);
                }
                for (itr = nids_g2.begin(); itr != nids_g2.end(); itr++)
                {
                    std::string rep_id = graph2->get_rep_id_from_nid(*itr);
                    rep_ids_g2.insert(rep_id);
                }
                set_intersection(rep_ids_g1.begin(), rep_ids_g1.end(), rep_ids_g2.begin(), rep_ids_g2.end(), inserter(rep_ids_intersection, rep_ids_intersection.begin()));
                string filename;
                cout << "Enter filename to store intersecting rep-ids: ";
                cin >> filename;
                ofstream fout(filename.c_str());
                if (fout.is_open())
                {
                    for (itr_str = rep_ids_intersection.begin(); itr_str != rep_ids_intersection.end(); itr_str++)
                    {
                        fout << (*itr_str) << " ";
                    }
                    fout << endl;
                }
                else
                {
                    cerr << "File " << filename << " could not be opened" << endl;
                }
            }
        }

        else if (command == "find_node" || command == "fn")
        {
            int graphnum;
            string node_id;
            cout << "Enter graph number: ";
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No graph with id " << graphnum << endl;
            }
            else
            {
                string node_name;
                set<string> nodes_set;
                cout << "Enter ids of nodes to find ending with -1, e.g. hsa100 hsa200 -1 : ";
                cin >> node_name;
                ofstream fout("find_nodes_result.txt");
                while (node_name != "-1")
                {
                    nodes_set.insert(node_name);
                    cin >> node_name;
                }
                for (set<string>::iterator itr = nodes_set.begin(); itr != nodes_set.end(); itr++)
                {
                    string curr_id = (*itr);
                    string rep_id = graph->get_rep_id_from_id(curr_id);
                    int nid = graph->get_nid_from_rep_id(rep_id);
                    if (nid == -1)
                    {
                        // fout << (curr_id) << "\t" << graph->get_node_from_nid(nid)->get_disp_ids().front() << "\t" << "Not found in graph" << endl;
                        fout << graph_man->kegg_hsa_id_to_display_name_map[curr_id] << "\t"
                             << "Not found in graph" << endl;
                    }
                    else
                    {
                        // fout << (curr_id) << "\t" << graph->get_node_from_nid(nid)->get_disp_ids().front() << "\t" <<  "Found in graph" << endl;
                        fout << graph_man->kegg_hsa_id_to_display_name_map[curr_id] << "\t"
                             << "Found in graph" << endl;
                    }
                }
            }
        }

        else if (command == "kegg_consistency_check" || command == "kcc")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {
                graph->perform_kegg_consistency_check();
            }
        }

        else if (command == "consistent_subgraph" || command == "csg")
        {
            int graphnum, new_gid;
            string up_reg_filename, down_reg_filename;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph != NULL)
            {
                cout << "\nEnter up regulated file: ";
                cin >> up_reg_filename;
                cout << "\nEnter down regulated file: ";
                cin >> down_reg_filename;
                new_gid = graph_man->get_data_consistent_subgraph(graph, up_reg_filename, down_reg_filename);
            }
            else
                cerr << "Graph id " << graphnum << " is not present" << endl;

            if (new_gid != -1)
                cout << "New graph id: " << new_gid << endl;
            else
                cerr << "Could not create graph" << endl;
        }

        else if (command == "reach" || command == "rch")
        {

            int graphnum;
            int direction;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
                cout << "Direction (2 for undirected, 1 for back, 0 for fwd (default)): ";
                cin >> direction;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                direction = stoi(token_cmd[cmd_index++]);
            }
            if (direction == 2)
            {
                direction = FORWARD_BACKWARD;
            }
            else if (direction == 1)
            {
                direction = BACKWARD;
            }
            else
            {
                if (direction != 0)
                {
                    cerr << "Invalid direction " << direction << ". Using default: fwd\n";
                }
                direction = FORWARD;
            }

            string node_name;
            vector<string> source_nodes_set;
            if (!b_mode)
            {
                cout << "Source node ids(any id from equiv class of that node) list ending with -1, e.g. hsa100 hsa200 -1 : ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            int cmd_itr = cmd_index;
            while (node_name != "-1")
            {
                source_nodes_set.push_back(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            vector<string> excluded_nodes_set;
            if (!b_mode)
            {
                cout << "Similarly, excluded node list (end by -1), e.g. hsa500 hsa600 -1: ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_itr];
                cmd_itr++;
            }

            while (node_name != "-1")
            {
                excluded_nodes_set.push_back(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            cout << "Bound: ";
            int bound;
            if (!b_mode)
            {
                cin >> bound;
            }
            else
            {
                bound = stoi(token_cmd[cmd_itr]);
            }
            ofstream fout("nodes_created");
            int new_gid = graph_man->bounded_reach(graphnum, direction, source_nodes_set, excluded_nodes_set, bound);
            if (new_gid != -1)
            {
                cout << "Reachability Graph: " << new_gid << endl;
                GraphNew *graph = graph_man->get_graph(new_gid);
                vector<int> all_nids = graph->get_node_ids();
                for (vector<int>::iterator itr = all_nids.begin(); itr != all_nids.end(); itr++)
                {
                    fout << graph->get_rep_id_from_nid(*itr) << endl;
                }
                fout << all_nids.size() << endl;
            }
            else
            {
                cerr << "Error: reachability didn't work" << endl;
            }
        }

        else if (command == "fb_rch")
        {
            int graphnum;
            int cmd_index = 1;

            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            string node_name;
            set<string> source_nodes_set;
            if (!b_mode)
            {
                cout << "Source node ids list ending with -1: ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            int cmd_itr = cmd_index;
            while (node_name != "-1")
            {
                source_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            set<string> target_nodes_set;
            if (!b_mode)
            {
                cout << "Target node ids list ending with -1: ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_itr];
                cmd_itr++;
            }

            while (node_name != "-1")
            {
                target_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            set<string> excluded_nodes_set;
            if (!b_mode)
            {
                cout << "Excluded node ids list ending with -1: ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_itr];
                cmd_itr++;
            }

            while (node_name != "-1")
            {
                excluded_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            int bound;
            if (!b_mode)
            {
                cin >> bound;
            }
            else
            {
                bound = stoi(token_cmd[cmd_itr]);
                cmd_itr++;
            }
            cout << "Bound: " << bound << endl;

            int target_act_expr_flag;
            if (!b_mode)
            {
                cin >> target_act_expr_flag;
            }
            else
            {
                target_act_expr_flag = stoi(token_cmd[cmd_itr]);
                cmd_itr++;
            }

            int new_gid = graph_man->bounded_forward_backward_reach(graphnum, source_nodes_set, target_nodes_set, excluded_nodes_set, bound, target_act_expr_flag);
            if (new_gid != -1)
            {
                cout << "Reachability Graph: " << new_gid << endl;
            }
            else
            {
                cerr << "Error: reachability didn't work" << endl;
            }
        }

        else if (command == "print_graph_info" || command == "pgi")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cerr << "No graph with id " << graphnum << "is present" << endl;
            }
            else
            {
                graph->print_all_graph_info();
            }
        }

        else if (command == "print_graph_man_info" || command == "pgmi")
        {
            graph_man->print_all_graph_manager_info();
        }

        else if (command == "merge" || command == "mg")
        {
            int cmd_index = 1;
            int graphnum;
            std::list<int> glist;
            if (!b_mode)
            {
                cout << "Enter graphs (end by -1): ";
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            while (graphnum != -1)
            {
                glist.push_back(graphnum);
                if (!b_mode)
                    cin >> graphnum;
                else
                    graphnum = stoi(token_cmd[cmd_index++]);

                if (graphnum == 0)
                    graphnum = graph_man->graph_id_count;
            }
            if (glist.size() == 0)
            {
                cerr << "No graphs to merge" << endl;
            }
            else
            {
                string filename;
                list<unordered_map<string, set<string>>> map_list;
                if (!b_mode)
                {
                    cout << "Enter map file names (end by -1): ";
                    cin >> filename;
                }
                else
                {
                    filename = token_cmd[cmd_index++];
                }
                while (filename != "-1")
                {
                    unordered_map<string, set<string>> mapname;
                    graph_man->read_naming_map(filename, mapname);
                    map_list.push_back(mapname);
                    if (!b_mode)
                        cin >> filename;
                    else
                        filename = token_cmd[cmd_index++];
                }
                vector<string> MAPK_merged_ids;
                map<string, set<string>> MAPK_node_to_pathway_id_map;

                int merged_graph_id = graph_man->merge_graphs(glist, map_list, MAPK_merged_ids, MAPK_node_to_pathway_id_map);

                if (merged_graph_id == -1)
                    cerr << "No merged graph produced" << endl;
                else
                    cout << "New graph: " << merged_graph_id << endl;
            }
        }

        else if (command == "merge_from_file" || command == "mff")
        {
            int cmd_index = 1;
            string filename;
            string format;
            int thres;
            std::list<int> glist;
            string file;
            int graph_id;
            bool tool_written = false;
            char tool_written_ch;
            string mapfilename;
            list<unordered_map<string, set<string>>> map_list;

            if (!b_mode)
            {
                cin >> filename;
                cout << "Enter file format (xml/sbml): ";
                cin >> format;
                cout << "Split node threshold: ";
                cin >> thres;
                cout << "Were these graph earlier written by this tool?, 'y' or 'Y' for yes, otherwise any other key: ";
                cin >> tool_written_ch;
                cout << "Enter map file names (end by -1): ";
                cin >> mapfilename;
            }
            else
            {
                filename = token_cmd[cmd_index++];
                format = token_cmd[cmd_index++];
                thres = stoi(token_cmd[cmd_index++]);
                tool_written_ch = token_cmd[cmd_index++][0];
                mapfilename = token_cmd[cmd_index++];
            }

            map<string, set<string>> MAPK_node_to_pathway_id_map;

            vector<string> MAPK_merged_ids;
            //            MAPK_merged_ids.push_back("hsa5594");
            //            MAPK_merged_ids.push_back("hsa5595");
            //            MAPK_merged_ids.push_back("hsa5599");
            //            MAPK_merged_ids.push_back("hsa1432");
            //            MAPK_merged_ids.push_back("hsa5600");
            //            MAPK_merged_ids.push_back("hsa6300");
            //

            time(&time3);
            ifstream ifs(filename.c_str());

            if (tool_written_ch == 'y' || tool_written_ch == 'Y')
            {
                tool_written = true;
            }

            // read map files

            while (mapfilename != "-1")
            {
                unordered_map<string, set<string>> mapname;
                graph_man->read_naming_map(mapfilename, mapname);
                cout << "Read map with " << mapname.size() << " entries" << endl;
                map_list.push_back(mapname);
                if (!b_mode)
                {
                    cin >> mapfilename;
                }
                else
                {
                    mapfilename = token_cmd[cmd_index++];
                }
            }
            // string pause;
            // cin >> pause;
            if (format == "xml")
            {
                while (getline(ifs, file))
                {
                    time(&time1);
                    cout << "Starting to read files" << endl;
                    graph_id = graph_man->read_graph_xml_file(file, thres, MAPK_merged_ids, MAPK_node_to_pathway_id_map, tool_written);
                    time(&time2);
                    time_in_sec = difftime(time2, time1);
                    // cout << "Time taken (in sec): " << time_in_sec << endl;

                    if (graph_id != -1)
                        glist.push_back(graph_id);
                }
                //            } else if (format == "sbml") {
                //                while (getline(ifs, file)) {
                //                    time(&time1);
                //                    graph_id = graph_man->read_graph_from_sbml(file);
                //                    time(&time2);
                //                    time_in_sec = difftime(time2, time1);
                //                    cout << "Time taken (in sec): " << time_in_sec << endl;
                //
                //                    if (graph_id != -1)
                //                        glist.push_back(graph_id);
                //                }
            }
            else
            {
                cerr << "Unknown format\n";
            }

            if (glist.size() == 0)
            {
                cerr << "No graphs to merge" << endl;
            }
            else
            {

                int merged_graph_id = graph_man->merge_graphs(glist, map_list, MAPK_merged_ids, MAPK_node_to_pathway_id_map);
                if (merged_graph_id == -1)
                    cerr << "No merged graph produced" << endl;
                else
                {
                    std::string oper = "Merged graphs from file " + filename;
                    std::list<int> oper_graph_ids;
                    oper_graph_ids.empty();

                    graph_man->add_graph_genesis(merged_graph_id, oper, oper_graph_ids);
                    cout << "New graph: " << merged_graph_id << endl;

                    ////                    GraphNew * g = graph_man->get_graph(merged_graph_id);
                    ////                    map<int, set<int> > m =  g->merged_eid_to_construction_eids_map;
                    ////                    for (auto i = m.begin(); i != m.end(); i++) {
                    ////                        int e = i->first;
                    ////                        set<int> s = i->second;
                    ////                        cout << e << ": {";
                    ////                        for (auto i2 = s.begin(); i2 != s.end(); i2++) {
                    ////                            int p = *i2;
                    ////                            cout << "(" << graph_man->get_graph_id_for_edge(p) << "," << p << ")";
                    ////                        }
                    ////                        cout << "}" << endl;
                    ////
                    ////                    }
                }
            }
            time(&time4);
            cout << "Total time taken (in sec): " << difftime(time4, time3) << endl;

            //            for (auto m = MAPK_merged_ids.begin(); m != MAPK_merged_ids.end(); m++) {
            //                cout << *m << " " << graph_man->kegg_hsa_id_to_display_name_map[*m] << endl;
            //                set<string> m_pathways = MAPK_node_to_pathway_id_map[*m];
            //                for (auto mp = m_pathways.begin(); mp != m_pathways.end(); mp++) {
            //                    cout << *mp << " " << graph_man->pathway_name_map[*mp] << endl;
            //                }
            //                cout << endl;
            //            }
        }

        else if (command == "list_pathways_with_nodes" || command == "lpwn")
        {
            string filename;
            cin >> filename;
            ifstream ifs(filename.c_str());
            string file;
            int graph_id;

            bool tool_written = false;
            char tool_written_ch;
            cout << "Were these graph earlier written by this tool?, 'y' or 'Y' for yes, otherwise any other key: ";
            cin >> tool_written_ch;
            if (tool_written_ch == 'y' || tool_written_ch == 'Y')
            {
                tool_written = true;
            }

            string diff_nodes_file, curr_node;
            cout << "\nEnter the nodes file that contains the differentially nodes to be looked for: ";
            cin >> diff_nodes_file;
            ifstream fin_nodes_diff(diff_nodes_file.c_str());
            set<string> diff_ids_to_look_for;

            if (fin_nodes_diff.is_open())
            {
                while (fin_nodes_diff >> curr_node)
                {
                    diff_ids_to_look_for.insert(curr_node);
                }
            }
            else
            {
                cerr << "\nError: Couldn't open file " + diff_nodes_file << endl;
            }

            string perturbed_node;
            cout << "\nEnter the id of the perturbed node: ";
            cin >> perturbed_node;

            string selected_pathways_file;
            cout << "\nEnter the file path to which to write the list of pathways that contain any node from the list of nodes: ";
            cin >> selected_pathways_file;
            ofstream fout_select_path(selected_pathways_file.c_str());

            vector<string> MAPK_merged_ids;
            map<string, set<string>> MAPK_node_to_pathway_id_map;
            int thres;

            if (fout_select_path.is_open())
            {
                while (getline(ifs, file))
                {
                    graph_id = graph_man->read_graph_xml_file(file, tool_written, MAPK_merged_ids, MAPK_node_to_pathway_id_map);
                    bool graph_selected = false;
                    if (graph_id != -1)
                    {
                        GraphNew *graph = graph_man->get_graph(graph_id);
                        for (set<string>::iterator ids_itr = diff_ids_to_look_for.begin(); ids_itr != diff_ids_to_look_for.end(); ids_itr++)
                        {
                            string rid = graph->get_rep_id_from_id(*ids_itr);
                            if (rid != "")
                            {
                                int nid = graph->get_nid_from_rep_id(rid);
                                NodeNew *node = graph->get_node_from_nid(nid);
                                if (node->is_target_of_GErel_edge(graph))
                                {
                                    fout_select_path << file << endl;
                                    graph_selected = true;
                                    break;
                                }
                            }
                        }
                        if (!graph_selected)
                        {
                            string rid = graph->get_rep_id_from_id(perturbed_node);
                            if (rid != "")
                            {
                                fout_select_path << file << endl;
                            }
                        }
                    }
                }
            }
            else
            {
                cerr << "\nError: Couldn't open file " + selected_pathways_file << endl;
            }
        }

        else if (command == "genesis" || command == "gen")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            graph_man->print_genesis_new(graphnum, cout);
        }

        else if (command == "write_graph_xml" || command == "wgx")
        {
            int graphnum;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[1]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph_ptr = graph_man->get_graph(graphnum);

            if (graph_ptr != NULL)
            {
                string filename;
                if (!b_mode)
                {
                    cout << "Enter the file path to store the graph in xml format: ";
                    cin >> filename;
                }
                else
                {
                    filename = token_cmd[2];
                }

                // filename = OUTPUT_FILES_DIR_PREFIX + filename;
                ofstream fout(filename.c_str());
                if (fout.is_open())
                {
                    graph_ptr->write_to_xml_file(fout, b_mode, token_cmd[3]);
                    fout.close();
                }
                else
                {
                    cerr << "Error: couldn't open file " << filename << endl;
                }
            }
            else
            {
                cerr << "Graph id " << graphnum << " is not present" << endl;
            }
        }

        else if (command == "create_up_down_file" || command == "cudf")
        {
            string fold_change_filename, up_reg_filename, down_reg_filename;
            float up_threshold, down_threshold;
            if (!b_mode)
            {
                cin >> fold_change_filename;
                cin >> up_threshold;
                cin >> down_threshold;
                cin >> up_reg_filename;
                cin >> down_reg_filename;
            }
            else
            {
                fold_change_filename = token_cmd[1];
                up_threshold = stof(token_cmd[2]);
                down_threshold = stof(token_cmd[3]);
                up_reg_filename = token_cmd[4];
                down_reg_filename = token_cmd[5];
            }

            ifstream fold_change_file(fold_change_filename.c_str());
            ofstream up_reg_file(up_reg_filename.c_str());
            ofstream down_reg_file(down_reg_filename.c_str());

            bool all_files_ok = true;
            if (!fold_change_file.is_open())
            {
                all_files_ok = false;
                cerr << "Error: Couldn't open file " << fold_change_filename << endl;
            }
            if (!up_reg_file.is_open())
            {
                all_files_ok = false;
                cerr << "Error: Couldn't open file " << up_reg_filename << endl;
            }
            if (!down_reg_file.is_open())
            {
                all_files_ok = false;
                cerr << "Error: Couldn't open file " << down_reg_filename << endl;
            }

            string s, gene_name;
            float fold_change;

            if (all_files_ok)
            {

                while (getline(fold_change_file, s))
                {
                    stringstream(s) >> gene_name >> fold_change;
                    // cout << "Gene: " << gene_name << " Fold change: " << fold_change << endl;
                    if (fold_change >= up_threshold)
                    {
                        up_reg_file << gene_name << endl;
                    }
                    if (fold_change <= down_threshold)
                    {
                        down_reg_file << gene_name << endl;
                    }
                }

                fold_change_file.close();
                up_reg_file.close();
                down_reg_file.close();
            }
            else
            {
                cerr << "Skipping generation of up and down regulated files." << endl;
            }
        }

        else if (command == "generate_constraints" || command == "gencn")
        {
            int graphnum;
            cout << "Enter graph number on which rules are to be applied: ";
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            graph_man->generate_constraints_using_rules_parser(graphnum, list_of_rules, em);
        }

        else if (command == "solve")
        {
            int graphnum;
            string cnf_file, mapping_filename, z3_out_file_name;
            string soln, soln_file;

            cout << "\nFor graph number: ";
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            cout << "\nEnter DIMACS format file: ";
            cin >> cnf_file;
            cout << "\nEnter z3 output file name: ";
            cin >> z3_out_file_name;

            string systemCommand = "z3 -dimacs " + cnf_file + " > " + z3_out_file_name;
            // cout << systemCommand << endl;
            soln_file = z3_out_file_name;
            system(systemCommand.c_str());
            sleep(2);
            ifstream fin(soln_file.c_str());
            fin >> soln;

            if (soln == "unsat")
                cout << "\nNo solutions";

            else if (soln == "sat")
            {

                string output_assigns = z3_out_file_name + "_assignment";
                read_z3_output(z3_out_file_name, output_assigns);

                GraphNew *graph = graph_man->get_graph(graphnum);
                GraphNew *subgraph = graph_man->get_subgraph_with_edges_retained(graph, output_assigns);

                cout << "Solution subgraph is Graph " << subgraph->get_graph_id() << endl;
            }
        }

        else if (command == "print_GErels" || command == "pgrel")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph_ptr = graph_man->get_graph(graphnum);
            if (graph_ptr != NULL)
            {
                char input;
                cout << "\nEnter one of the options:\n'a' for getting GErel information of all edges\n'b' for getting all GErel target node ids\n";
                cin >> input;
                switch (input)
                {
                case 'a':
                    graph_ptr->print_GErel_edges();
                    break;

                case 'b':
                    graph_ptr->print_GErel_targets();
                    break;
                }
            }
            else
            {
                cerr << "Graph id " << graphnum << " is not present" << endl;
            }
        }

        else if (command == "subgraph" || command == "sg")
        {
            int graphnum;
            string filename;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            cin >> filename;

            GraphNew *graph = graph_man->get_graph(graphnum);
            GraphNew *subgraph = graph_man->get_subgraph_with_edges_retained(graph, filename);
            if (subgraph != NULL)
            {
                cout << "New subgraph id: " << subgraph->get_graph_id() << endl;
            }
            else
            {
                cerr << "Subgraph not created" << endl;
            }

            string file_visible_edges;
            cout << "Enter the file name to store the edges that are visible (to be colored red) but not present (i.e. not doing their job)" << endl;
            cin >> file_visible_edges;

            ofstream fout(file_visible_edges.c_str());

            if (fout.is_open())
            {
                graph_man->print_visible_but_not_present_edges_from_z3_assignment(filename, fout);
                fout.close();
            }
            else
            {
                cerr << "Error: file " + file_visible_edges + " couldn't be opened" << endl;
            }
        }

        else if (command == "select")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            std::string input_file, line;
            std::cin >> input_file;
            set<string> nodes_set;
            std::ifstream fin;
            fin.open(input_file.c_str(), std::ifstream::in);
            if (fin.is_open())
            {
                while (getline(fin, line))
                {
                    std::stringstream s(line);
                    std::string id;
                    s >> id;
                    nodes_set.insert(id);
                }
                fin.close();
            }
            else
            {
                std::cerr << "\nUnable to open the file " + input_file << std::endl;
            }

            int new_gid = graph_man->select_subgraph(graphnum, nodes_set);

            if (new_gid != -1)
            {
                cout << "Graph: " << new_gid << endl;
            }
            else
            {
                cerr << "Error in select" << endl;
            }
        }

        else if (command == "process_microarr_data" || command == "pmd")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            graph->write_microarr_decl_to_file();
            graph->write_microarr_assert_to_file();
        }

        else if (command == "add_projected_variables" || command == "proj")
        {
            string cnf_file, mapping_filename, cnf_out_file;
            cout << "\nEnter DIMACS format file: ";
            cin >> cnf_file;
            cout << "\nEnter mapping file name: ";
            cin >> mapping_filename;
            cout << "\nEnter output CNF file name: ";
            cin >> cnf_out_file;

            string systemCommand = "../supporting_scripts/add_proj_vars_to_cnf_file.py " + cnf_file + " " + mapping_filename + " " + cnf_out_file;
            cout << systemCommand << endl;
            system(systemCommand.c_str());
            sleep(2);

            cout << "Written to CNF file " << cnf_out_file << endl;
        }

        else if (command == "get_all_solutions_from_cnf" || command == "get_all_solns_cnf")
        {
            int graphnum;
            string cnf_file, mapping_filename, z3_out_file_prefix;
            string soln, soln_file;
            list<int> resultant_subgraphs_to_merge;
            int merged_graphnum = graphnum;

            string xdot_path_name = XDOT_PATH_NAME;
            string merged_result = "merged_result.dot";
            string merged_vis_file = "merged_vis";

            cout << "\nFor graph number: ";
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            cout << "\nEnter DIMACS format file: ";
            cin >> cnf_file;
            cout << "\nEnter mapping filename: ";
            cin >> mapping_filename;
            cout << "\nEnter z3 output file prefix: ";
            cin >> z3_out_file_prefix;

            string file_diff_nodes;
            cout << "Enter the file path with the list differentially expressed nodes : ";
            cin >> file_diff_nodes;

            string color_map_filename;
            cout << "\nEnter color map file name: ";
            cin >> color_map_filename;
            // string vis_edges_filename;
            // cout << "\nEnter the file containing visible edges list: ";
            // cin >> vis_edges_filename;
            string dis_name;
            cout << "\nEnter a file prefix for the graph display: ";
            cin >> dis_name;

            int soln_counter = 0;
            string systemCommand = "z3 -dimacs " + cnf_file + " > " + z3_out_file_prefix + (soln_counter + 1);
            // cout << systemCommand << endl;
            soln_file = z3_out_file_prefix + (soln_counter + 1);
            system(systemCommand.c_str());
            sleep(2);
            ifstream fin(soln_file.c_str());
            fin >> soln;

            if (soln == "unsat")
                cout << "\nNo solutions";

            else if (soln == "sat")
            { // got a new sat solution

                string project_option;
                bool project = false;
                int num_soln_option = 1;
                cout << "How many do you want to see? (default is 1): ";
                cin >> num_soln_option;
                cout << "Count solutions projected on present edges? (Y/n): ";
                cin >> project_option;
                if (project_option == "y" || project_option == "Y")
                    project = true;

                // copying original CNF file - intermediate copies will be deleted
                string systemCommand2 = "cp " + cnf_file + " " + cnf_file + (soln_counter + 1);
                // cout << systemCommand2 << endl;
                system(systemCommand2.c_str());

                while (1)
                {
                    soln_counter++;
                    cout << "Solutions counted: " << soln_counter << endl;

                    // display first n solutions
                    if (soln_counter <= num_soln_option)
                    {

                        ////cout << "Displaying solution " << z3_out_file_prefix << soln_counter << endl;
                        string z3_result = z3_out_file_prefix + IntToString(soln_counter);
                        string output_assigns = z3_result + "_assignment";
                        read_z3_output(z3_result, output_assigns);

                        GraphNew *graph = graph_man->get_graph(graphnum);
                        GraphNew *subgraph = graph_man->get_subgraph_with_edges_retained(graph, output_assigns);

                        vector<string> diif_exp_nodes_set;
                        vector<string> excl_exp_nodes_set;
                        excl_exp_nodes_set.clear();

                        ifstream fin(file_diff_nodes.c_str());
                        string node_name;
                        while (fin >> node_name)
                        {
                            string diff_rid = subgraph->get_rep_id_from_id(node_name);
                            if (diff_rid != "")
                            {
                                // from the diff_exp_nodes_set pick only those that are actually targets of GErel edges in the graph fwd_gid
                                int diff_nid = subgraph->get_nid_from_rep_id(diff_rid);
                                vector<int> inlist_diff_node = subgraph->get_inlist(diff_nid);
                                for (vector<int>::iterator itr = inlist_diff_node.begin(); itr != inlist_diff_node.end(); ++itr)
                                {
                                    diif_exp_nodes_set.push_back(node_name);
                                }
                            }
                        }

                        int breach_on_sg_id = graph_man->bounded_reach(subgraph->get_graph_id(), BACKWARD, diif_exp_nodes_set, excl_exp_nodes_set, 25);
                        GraphNew *breach_on_sg = graph_man->get_graph(breach_on_sg_id);
                        string file_visible_edges = "empty";
                        // cout << "Enter the file name to store the edges that are visible (to be colored red) but not present (i.e. not doing their job)" << endl;
                        // cin >> file_visible_edges;

                        ////                                        string file_visible_edges = output_assigns + "_vis";
                        ////                                        ofstream fout(file_visible_edges.c_str());
                        ////                                        if(fout.is_open()){
                        ////                                                graph_man->print_visible_but_not_present_edges_from_z3_assignment(output_assigns, fout);
                        ////                                                fout.close();
                        ////                                        }
                        ////                                        else{
                        ////                                                cerr << "Error: file " + file_visible_edges + " couldn't be opened" << endl;
                        ////                                        }

                        /** Merging solutions begins **/
                        //                                        string output_assigns_prev;
                        //                                        if(soln_counter == 0 || soln_counter == 1)
                        //                                            output_assigns_prev = z3_out_file_prefix + "1" + "_assignment";
                        //                                        else
                        //                                            output_assigns_prev = z3_out_file_prefix + gm->toString(soln_counter-1) + "_assignment";
                        //
                        //                                        ofstream vis_fout(merged_vis_file.c_str());
                        //                                        if(vis_fout.is_open()) {
                        //                                            graph_man->merge_vis_files(output_assigns_prev, output_assigns, vis_fout);
                        //                                            vis_fout.close();
                        //                                        }
                        //                                        else{
                        //                                            cerr << "Error: File " + merged_vis_file + " could not be opened" << endl;
                        //                                        }
                        //
                        //                                        resultant_subgraphs_to_merge.push_back(subgraph->get_graph_id());
                        //                                        GraphNew * merged_result_graph = graph_man->get_graph(graph_man->merge_graphs(resultant_subgraphs_to_merge));
                        //                                        merged_graphnum = merged_result_graph->get_graph_id();
                        //
                        /** Merging solutions ends **/
                        // string color_map_filename = "0";
                        // string dot_filename = "graph_" + gm->toString(soln_counter) + ".dot";
                        string dot_filename = dis_name + "_" + IntToString(soln_counter) + ".dot";
                        ////subgraph->display_graph_silent(color_map_filename, file_visible_edges, dot_filename, graph_man);
                        breach_on_sg->display_graph_silent(color_map_filename, file_visible_edges, dot_filename, graph_man);

                        //					string xdot_path_name = XDOT_PATH_NAME;
                        string systemCommand_xdot = xdot_path_name + dot_filename + " & ";
                        system(systemCommand_xdot.c_str());
                    }

                    //                                        string tps = " -Tps ";
                    //                                        string dotCommand = DOT_PATH_NAME + tps + result_graph_name + " -o MERGED_RESULT.pdf &";
                    //                                        system(dotCommand.c_str());
                    //                                        string pdf = " MERGED_RESULT.pdf &";
                    //                                        string evinceCommand = PDF_VIEWER_PATH_NAME + pdf;
                    //                                        system(evinceCommand.c_str());

                    // string systemCommand_xdot_result =  xdot_path_name + result_graph_name + " & ";
                    // system(systemCommand_xdot_result.c_str());

                    int temp = soln_counter + 1;
                    // string systemCommand3 = "../supporting_scripts/negate_z3_cnf_solution.py " + cnf_file + soln_counter + " " + std::string("mapping") + " " + z3_out_file_prefix + soln_counter + " " + cnf_file + temp;
                    string systemCommand3;
                    set<int> eid_set;
                    set<int>::iterator eid_itr;
                    if (project == true)
                    {

                        systemCommand3 = "../supporting_scripts/negate_z3_cnf_solution_only_black_edges.py " + cnf_file + soln_counter + " " + mapping_filename + " " + z3_out_file_prefix + soln_counter + " " + cnf_file + temp;
                    }
                    else
                        systemCommand3 = "../supporting_scripts/negate_z3_cnf_solution2.py " + cnf_file + soln_counter + " " + mapping_filename + " " + z3_out_file_prefix + soln_counter + " " + cnf_file + temp;
                    // cout << systemCommand3 << endl;
                    system(systemCommand3.c_str());
                    sleep(2);

                    string systemCommand5 = "rm " + cnf_file + soln_counter;
                    // cout << systemCommand5 << endl;
                    system(systemCommand5.c_str());

                    string systemCommand6 = "z3 -dimacs " + cnf_file + temp + " > " + z3_out_file_prefix + temp;
                    // cout << systemCommand6 << endl;
                    system(systemCommand6.c_str());
                    sleep(2);

                    string systemCommand4 = "rm " + z3_out_file_prefix + soln_counter;
                    // cout << systemCommand4 << endl;
                    system(systemCommand4.c_str());

                    string soln_filename = z3_out_file_prefix + IntToString(temp);

                    ifstream infile(soln_filename.c_str());
                    string soln;
                    infile >> soln;
                    // cout << soln << endl;
                    if (soln == "unsat")
                    {
                        cout << "\nNo more solutions" << endl;
                        break;
                    }
                    else if (soln != "sat")
                    {
                        cout << "\nError in z3 output" << endl;
                    }
                }
            }

            cout << "Total solutions: " << soln_counter << endl;
            cout << "Merged vis file written to " << merged_vis_file << endl;
            cout << "Merged graph id: " << merged_graphnum << endl;
        }

        else if (command == "call_ApproxMC" || command == "approxmc")
        {
            string cnf_file, delta, epsilon, logging;
            string logfile = "log/logging/" + cnf_file + ".txt";
            cout << "\nEnter DIMACS format file: ";
            cin >> cnf_file;
            cout << "\nEnter delta parameter: ";
            cin >> delta;
            cout << "\nEnter epsilon parameter: ";
            cin >> epsilon;
            cout << "\nEnable logging (0/1): ";
            cin >> logging;
            if (logging == "1")
            {
                cout << "\nEnter destination for logfile: ";
                cin >> logfile;
            }
            string approxmc = APPROXMC_PATH_NAME;
            string systemCommand = "python " + approxmc + " -delta=" + delta + " -epsilon=" + epsilon + " " + cnf_file;
            cout << systemCommand << endl;
            system(systemCommand.c_str());
            sleep(2);
        }

        else if (command == "clear_expression_maps" || command == "clxm")
        {
            // empty the internal maps
            getCNFIndexMap.clear();
            getExpressionMap.clear();
            idx = 1;     // added by Sukanya
            clauses = 0; // added by Sukanya
            // more maps need to be cleared - not complete
        }

        else if (command == "clr")
        {
            system("clear");
        } //"fwd_bkwd_rch \t<graph-id>"

        else if (command == "fwd_bkwd_rch")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            string perturbed_node;
            cout << "Enter the id of the perturbed node, (e.g. hsa100) : " << endl;
            cin >> perturbed_node;
            string perturb_rid = graph->get_rep_id_from_id(perturbed_node);
            if (perturb_rid == "")
            {
                cerr << "Error: No rep id found in the graph for the perturbed node " + perturbed_node << endl;
                continue;
            }
            vector<string> source_nodes_set;
            source_nodes_set.push_back(perturb_rid);

            vector<string> empty_set;
            int max_bound = std::numeric_limits<int>::max() - 1;
            int direction = FORWARD;

            char bound_choice;
            cout << "Enter 'y' or 'Y' to give a bound, any other key to not... ";
            cin >> bound_choice;

            if (bound_choice == 'y' || bound_choice == 'Y')
            {
                int bound;
                cout << "Enter the bound: ";
                cin >> bound;
                max_bound = bound;
            }

            // do forward reachability from perturbed nodes
            int fwd_bkwd_gid = -1;
            int fwd_gid = graph_man->bounded_reach(graphnum, direction, source_nodes_set, empty_set, max_bound);
            if (fwd_gid != -1)
            {
                // do backward reachability from differen expressed nodes
                GraphNew *graph_fwd = graph_man->get_graph(fwd_gid);
                string node_name, file_diff_nodes;
                vector<string> diif_exp_nodes_set;
                /*cout << "Enter the id of the differentially expressed nodes list ending with -1, e.g. hsa100 hsa200 -1 : ";
                  cin >> node_name;*/
                cout << "Enter the file path with the list differentially expressed nodes : ";
                cin >> file_diff_nodes;
                ifstream fin(file_diff_nodes.c_str());

                while (fin >> node_name)
                {
                    string diff_rid = graph_fwd->get_rep_id_from_id(node_name);
                    if (diff_rid != "")
                    {
                        // from the diff_exp_nodes_set pick only those that are actually targets of GErel edges in the graph fwd_gid
                        int diff_nid = graph_fwd->get_nid_from_rep_id(diff_rid);
                        vector<int> inlist_diff_node = graph_fwd->get_inlist(diff_nid);
                        for (vector<int>::iterator itr = inlist_diff_node.begin(); itr != inlist_diff_node.end(); ++itr)
                        {
                            if (graph_fwd->get_edge_type(*itr) == "GErel" && (graph_fwd->edge_has_subtype(*itr, "expression") || graph_fwd->edge_has_subtype(*itr, "repression")))
                            {
                                diif_exp_nodes_set.push_back(node_name);
                                break;
                            }
                        }
                    }
                    else
                    {
                        cerr << "No rep id found in the graph for the differentiallly expressed node " + node_name + ", ignoring..." << endl;
                    }
                }

                if (diif_exp_nodes_set.empty())
                {
                    cerr << "Error: None of the differentially expressed are nodes common with the fwd reachable graph from the pertubed nodes" << endl;
                    continue;
                }

                direction = BACKWARD;

                fwd_bkwd_gid = graph_man->bounded_reach(fwd_gid, direction, diif_exp_nodes_set, empty_set, max_bound);
                if (fwd_bkwd_gid != -1)
                {
                    cout << "Forward-Backward Reachability Graph: " << fwd_bkwd_gid << endl;

                    cout << "Writing color file \"fwd_bkwd_color\" for coloring source and target nodes" << endl;
                    ofstream fout("fwd_bkwd_color");

                    if (fout.is_open())
                    {
                        // give bluish color to source node:
                        fout << perturb_rid + " 128 128 255" << endl;

                        // give reddish color to differen-expressed nodes
                        // 255 128 128

                        for (vector<string>::iterator itr = diif_exp_nodes_set.begin(); itr != diif_exp_nodes_set.end(); itr++)
                        {
                            fout << *itr << " 255 128 128" << endl;
                        }
                        fout.close();
                    }
                    else
                    {
                        cerr << "Error: couldn't openfile fwd_bkwd_color for color info" << endl;
                    }

                    // Testing starts:
                    /*GraphNew* graph_fwd_bkwd = graph_man->get_graph(fwd_bkwd_gid);
                      graph_fwd_bkwd->print_nodes_rids_with_no_in_edges(cout);*/
                    // Testing ends
                }
                else
                {
                    cerr << "Error: bkwd reachability from perturbed didn't work" << endl;
                }
            }
            else
            {
                cerr << "Error: fwd reachability from perturbed didn't work" << endl;
            }
        }

        else if (command == "read_rules" || command == "rr")
        {
            string filename;
            cin >> filename;
            yyin = fopen(filename.c_str(), "r");
            yyparse();
            fclose(yyin);
            cout << "Number of rules in template file: " << list_of_rules.size() << endl;
            // rules_ptr->add(global_list_of_rules);
            // global_list_of_rules.clear();//make empty so t
            /*these encodings come from supportinghat it can store another rules file*/
        }

        else if (command == "get_overlap_of_data" || command == "overlap2")
        {

            int graphnum;

            int cmd_index = 1;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
                cout << "No such graph" << endl;
            else
            {
                string up_reg_filename, down_reg_filename;

                if (!b_mode)
                {
                    cout << "Enter up regulated file: ";
                    cin >> up_reg_filename;
                    cout << "Enter down regulated file: ";
                    cin >> down_reg_filename;
                }
                else
                {
                    up_reg_filename = token_cmd[cmd_index++];
                    down_reg_filename = token_cmd[cmd_index++];
                }

                int consistent_subgraph_gid = graph_man->get_data_consistent_subgraph(graph, up_reg_filename, down_reg_filename, false);
                //                cout << "Resultant subgraph has id: " << consistent_subgraph_gid << endl;
            }
        }

        else if (command == "overlap")
        {

            int graphnum;

            int cmd_index = 1;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
                cout << "No such graph" << endl;
            else
            {
                string up_reg_filename, down_reg_filename;

                if (!b_mode)
                {
                    cout << "Enter up regulated file: ";
                    cin >> up_reg_filename;
                    cout << "Enter down regulated file: ";
                    cin >> down_reg_filename;
                }
                else
                {
                    up_reg_filename = token_cmd[cmd_index++];
                    down_reg_filename = token_cmd[cmd_index++];
                }

                graph_man->get_data_overlap(graph, up_reg_filename, down_reg_filename);
                //                cout << "Resultant subgraph has id: " << consistent_subgraph_gid << endl;
            }
        }

        else if (command == "get_fold_change" || command == "fold")
        {
            int cmd_index = 1;
            string log_value_filename, output_filename;

            if (!b_mode)
            {
                cout << "Enter log_value_filename: ";
                cin >> log_value_filename;
                cout << "Enter output file name: ";
                cin >> output_filename;
            }
            else
            {
                log_value_filename = token_cmd[cmd_index++];
                output_filename = token_cmd[cmd_index++];
            }

            graph_man->get_fold_change_from_log_values(log_value_filename, output_filename);
        }

        else if (command == "get_consistent_subgraph" || command == "cg")
        {

            int graphnum;

            int cmd_index = 1;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
                cout << "No such graph" << endl;
            else
            {
                string up_reg_filename, down_reg_filename;

                if (!b_mode)
                {
                    cout << "Enter up regulated file: ";
                    cin >> up_reg_filename;
                    cout << "Enter down regulated file: ";
                    cin >> down_reg_filename;
                }
                else
                {
                    up_reg_filename = token_cmd[cmd_index++];
                    down_reg_filename = token_cmd[cmd_index++];
                }

                int consistent_subgraph_gid = graph_man->get_data_consistent_subgraph(graph, up_reg_filename, down_reg_filename, true);
                cout << "Resultant subgraph has id: " << consistent_subgraph_gid << endl;
            }
        }

        else if (command == "weight_graph" || command == "wg")
        {
            int graphnum;
            int cmd_index = 1;

            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            string node_name;
            vector<string> source_nodes_set;
            if (!b_mode)
            {
                cout << "Source node ids (end with -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            int cmd_itr = cmd_index;
            while (node_name != "-1")
            {
                source_nodes_set.push_back(graph->get_rep_id_from_id(node_name));
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            vector<string> target_nodes_set;
            if (!b_mode)
            {
                cout << "Target node ids (end by -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_itr];
                cmd_itr++;
            }

            while (node_name != "-1")
            {
                target_nodes_set.push_back(graph->get_rep_id_from_id(node_name));
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            string fold_change_filename;

            if (!b_mode)
            {
                cout << "Enter fold change file: ";
                cin >> fold_change_filename;
            }
            else
            {
                fold_change_filename = token_cmd[cmd_itr];
                cmd_itr++;
            }

            cout << "fold_change_filename: " << fold_change_filename << endl;

            float coeff = 1.0;
            float order = 1.0;
            vector<float> vec_coeff;
            vector<float> vec_order;

            cout << "Enter weight coefficients for: " << endl;
            if (!b_mode)
            {
                cout << "Forward distance from overall source: " << endl;
                cout << "Coeff: ";
                cin >> coeff;
                cout << "\n Power: ";
                cin >> order;
                vec_coeff.push_back(coeff);
                vec_order.push_back(order);

                cout << "Backward distance from overall target: " << endl;
                cout << "Coeff: ";
                cin >> coeff;
                cout << "\n Power: ";
                cin >> order;
                vec_coeff.push_back(coeff);
                vec_order.push_back(order);

                cout << "Fold change value of edge source: " << endl;
                cout << "Coeff: ";
                cin >> coeff;
                cout << "\n Power: ";
                cin >> order;
                vec_coeff.push_back(coeff);
                vec_order.push_back(order);

                cout << "Fold change value of edge target: " << endl;
                cout << "Coeff: ";
                cin >> coeff;
                cout << "\n Power: ";
                cin >> order;
                vec_coeff.push_back(coeff);
                vec_order.push_back(order);
            }
            else
            {
                coeff = stof(token_cmd[cmd_itr++]);
                vec_coeff.push_back(coeff);

                order = stof(token_cmd[cmd_itr++]);
                vec_order.push_back(order);

                coeff = stof(token_cmd[cmd_itr++]);
                vec_coeff.push_back(coeff);

                order = stof(token_cmd[cmd_itr++]);
                vec_order.push_back(order);

                coeff = stof(token_cmd[cmd_itr++]);
                vec_coeff.push_back(coeff);

                order = stof(token_cmd[cmd_itr++]);
                vec_order.push_back(order);

                coeff = stof(token_cmd[cmd_itr++]);
                vec_coeff.push_back(coeff);

                order = stof(token_cmd[cmd_itr++]);
                vec_order.push_back(order);
            }

            int weighted_graph_gid = graph_man->put_weight_on_graph_edges(graph, source_nodes_set, target_nodes_set, fold_change_filename, vec_coeff, vec_order);

            if (weighted_graph_gid == -1)
            {
                cout << "Could not create weighted graph" << endl;
            }
            cout << "Resultant weighted graph has id: " << weighted_graph_gid << endl;
        }

        else if (command == "get_path" || command == "path")
        {

            int graphnum;
            int cmd_index = 1;
            string smt_solver;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                exit(1);
            }
            else
            {
                map<int, bool> nid_to_value_pairs;
                map<int, bool> eid_to_value_pairs;
                string up_reg_filename, down_reg_filename;
                string file_prefix = "";
                string curr_dir = "";
                if (!b_mode)
                {
                    cout << "Enter preferred file prefix: ";
                    cin >> file_prefix;
                    cout << "Enter smt-solver: ";
                    cin >> smt_solver;
                    cout << "Enter up regulated file: ";
                    cin >> up_reg_filename;
                    cout << "Enter down regulated file: ";
                    cin >> down_reg_filename;
                }
                else
                {
                    file_prefix = token_cmd[cmd_index++];
                    smt_solver = token_cmd[cmd_index++];
                    up_reg_filename = token_cmd[cmd_index++];
                    down_reg_filename = token_cmd[cmd_index++];
                }

                file_prefix = OUTPUT_FILES_DIR_PREFIX + file_prefix;
                //                vector<int> node_ids = graph->get_node_ids();
                //                vector<int> gomoryhu_parents;
                //                vector<set<int> > cut_edges(node_ids.size());

                // compute Gomory-Hu tree
                int graphnum_undirected = graph_man->convert_to_undirected_graph(graphnum);
                GraphNew *ugraph = graph_man->get_graph(graphnum_undirected);

                vector<int> node_ids; // = ugraph->get_node_ids();
                vector<int> gomoryhu_parents;

                graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);
                graph_man->fill_adj_matrix_for_graph(graph);
                int n = graph->get_node_ids().size();
                //                cout << "GRAPH ADJ MATRIX: " << endl;
                //                for (int i = 0; i < n; i++) {
                //
                //                    cout << endl;
                //
                //                    for (int j = 0; j < n; j++) {
                //
                //                        cout << graph->adj_matrix[i][j] << " ";
                //                    }
                //                    cout << endl;
                //                }
                //                cout << endl;
                vector<vector<bool>> closure_matrix(n, vector<bool>(n, false));
                graph_man->compute_transitive_closure(graph, closure_matrix);
                //                cout << "GRAPH TRANS CLOSURE MATRIX: " << endl;
                //                for (int i = 0; i < n; i++) {
                //
                //                    cout << endl;
                //
                //                    for (int j = 0; j < n; j++) {
                //
                //                        cout << closure_matrix[i][j] << " ";
                //                    }
                //                    cout << endl;
                //                }
                //                cout << endl;
                //

                node_ids.clear();
                gomoryhu_parents.clear();

                ugraph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

                vector<set<int>> cut_edges(node_ids.size());

                vector<vector<int>> call_level_matrix(n, vector<int>(n, BIGNUM));
                vector<vector<int>> edge_level_matrix(n, vector<int>(n, BIGNUM));
                vector<vector<int>> call_count_matrix(n, vector<int>(n, 0));
                vector<vector<int>> edge_count_matrix(n, vector<int>(n, 0));

#ifdef COMPUTE_MINCUT_EDGES_FLAG
                time(&time1);
                int ghtree_gid = graph_man->compute_gh_tree(graphnum_undirected, node_ids, gomoryhu_parents, cut_edges);
                cout << "Gomory-Hu tree has graph id: " << ghtree_gid << endl;
                time(&time2);
                time_in_sec = difftime(time2, time1);
                cout << "Time taken to construct GH tree (in sec): " << time_in_sec << endl;

#endif

                // get queries and data files
                string connect_pairs_filename, disconnect_pairs_filename, essential_filename, nodes_to_avoid_filename, fold_change_filename;

                if (!b_mode)
                {
                    cout << "Enter connect pairs file: ";
                    cin >> connect_pairs_filename;
                    cout << "Enter disconnect pairs file: ";
                    cin >> disconnect_pairs_filename;
                    cout << "Enter essential nodes file: ";
                    cin >> essential_filename;
                    cout << "Enter file with nodes to be asserted false: ";
                    cin >> nodes_to_avoid_filename;
                    cout << "Enter fold change file: ";
                    cin >> fold_change_filename;
                }
                else
                {
                    connect_pairs_filename = token_cmd[cmd_index++];
                    disconnect_pairs_filename = token_cmd[cmd_index++];
                    essential_filename = token_cmd[cmd_index++];
                    nodes_to_avoid_filename = token_cmd[cmd_index++];
                    fold_change_filename = token_cmd[cmd_index++];
                }

                set<int> nids_as_source;
                set<int> nids_as_target;
                vector<pair<int, int>> connect_pairs;
                vector<pair<int, int>> disconnect_pairs;
                graph_man->read_pairs_file(graph, connect_pairs_filename, connect_pairs, nids_as_source, nids_as_target);
                graph_man->read_pairs_file(graph, disconnect_pairs_filename, disconnect_pairs, nids_as_source, nids_as_target);
                cout << connect_pairs.size() << " connect pairs" << endl;
                cout << disconnect_pairs.size() << " disconnect pairs" << endl;

                unordered_map<string, bool> data_map;
                graph_man->read_data_file_into_map(graph, up_reg_filename, down_reg_filename, data_map);

                set<int> up_reg_nids, down_reg_nids, essential_nids, avoid_nids, all_up_reg_nids;
                graph_man->read_data_file_into_set_of_ints(graph, up_reg_filename, all_up_reg_nids);
                graph_man->read_data_file_into_set_of_ints(graph, down_reg_filename, down_reg_nids);
                graph_man->read_data_file_into_set_of_ints(graph, essential_filename, essential_nids);
                graph_man->read_data_file_into_set_of_ints(graph, nodes_to_avoid_filename, avoid_nids);

                // remove nodes to avoid from up_reg_nids
                set_difference(all_up_reg_nids.begin(), all_up_reg_nids.end(), avoid_nids.begin(), avoid_nids.end(), inserter(up_reg_nids, up_reg_nids.begin()));

                cout << "Avoid: " << avoid_nids.size() << endl;
                cout << "Up: " << up_reg_nids.size() << " Down: " << down_reg_nids.size() << endl;

                int up_reg_down_reg_size = up_reg_nids.size() + down_reg_nids.size();

                //                map<int, int> nid_to_expr_value_map; // fold change is rounded off into int
                //                graph_man->read_fold_change_values_into_map(graph, nid_to_expr_value_map, fold_change_filename);

                char choice;
                // default values for bounds
                int num_of_nodes_bound = graph->get_node_ids().size();
                int num_of_edges_bound = graph->get_edge_ids().size();
                int path_length_bound = 1;
                int node_relaxation_bound = 0;
                int edge_relaxation_bound = 0;

                // generate and solve constraints
                string smt_filename = file_prefix + ".smt2";
                string smt_out_filename = file_prefix + "_smt_output";
                string z3_error_filename = file_prefix + "_smt_error";

                string header_filename = file_prefix + "_header";
                string footer_filename = file_prefix + "_footer";
                string decl_filename = file_prefix + "_decl";
                string cons_filename = file_prefix + "_cons";
                string bound_filename = file_prefix + "_bound";
                string bound_cons_filename = file_prefix + "_bound_cons";

                // write header and footer files
                ofstream header_fout(header_filename.c_str());
                ofstream footer_fout(footer_filename.c_str());

                header_fout << "(set-info :smt-lib-version 2.0)\n"
                            << endl;
                footer_fout << "\n(check-sat)\n(get-model)\n(exit)\n"
                            << endl;

                header_fout.close();
                footer_fout.close();

                bool is_sat = false;
                bool rewrite_smt_flag = true;

                if (!b_mode)
                {
                    cout << "\nEnter path length bound: ";
                    cin >> path_length_bound;
                }
                else
                {
                    path_length_bound = stoi(token_cmd[cmd_index++]);
                }

                graph_man->get_connectivity(graphnum, graphnum_undirected, closure_matrix, call_level_matrix, edge_level_matrix, call_count_matrix, edge_count_matrix, cut_edges, gomoryhu_parents, connect_pairs, disconnect_pairs, up_reg_nids, down_reg_nids, essential_nids, avoid_nids, nids_as_source, nids_as_target, path_length_bound, fold_change_filename, file_prefix, em);

                // -- print call_level_matrix
                string call_level_filename = file_prefix + "_call_level_matrix";
                string edge_level_filename = file_prefix + "_edge_level_matrix";
                string call_count_filename = file_prefix + "_call_count_matrix";
                string edge_count_filename = file_prefix + "_edge_count_matrix";

                ofstream call_level(call_level_filename.c_str());
                ofstream edge_level(edge_level_filename.c_str());
                ofstream call_count(call_count_filename.c_str());
                ofstream edge_count(edge_count_filename.c_str());

                call_level << "\t";
                edge_level << "\t";
                call_count << "\t";
                edge_count << "\t";
                for (int i = 0; i < n; i++)
                {
                    call_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                    edge_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                    call_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                    edge_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                }

                call_level << endl;
                edge_level << endl;
                call_count << endl;
                edge_count << endl;

                for (int i = 0; i < n; i++)
                {
                    call_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                    edge_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                    call_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                    edge_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";

                    for (int j = 0; j < n; j++)
                    {

                        int curr_entry = call_level_matrix[i][j];
                        string curr_entry_str = IntToString(curr_entry);
                        if (curr_entry == inf)
                            int curr_entry_str = stoi("0");
                        call_level << curr_entry_str << "\t";

                        curr_entry = edge_level_matrix[i][j];
                        curr_entry_str = IntToString(curr_entry);
                        if (curr_entry == inf)
                            curr_entry_str = stoi("0");
                        edge_level << curr_entry_str << "\t";

                        call_count << call_count_matrix[i][j] << "\t";
                        edge_count << edge_count_matrix[i][j] << "\t";
                    }
                    call_level << endl;
                    edge_level << endl;
                    call_count << endl;
                    edge_count << endl;
                }
                call_level << endl;
                edge_level << endl;
                call_count << endl;
                edge_count << endl;
                // --

                cout << "Printed matrices" << endl;

                // -- TEMPORARILY put on 15 Jan 2018
                // exit(1);
                // --

                if (!b_mode)
                {
                    cout << "Need user inputs. Continue (Y/n)? ";
                    cin >> choice;
                }
                else
                    choice = token_cmd[cmd_index++][0];

                if (choice == 'n' || choice == 'N')
                    break;

                int bound_choice;
                if (!b_mode)
                {
                    cout << "\n1 for no. of nodes";
                    cout << "\n2 for no. of edges";
                    cout << "\n3 for length of path";
                    cout << "\n4 for node relaxation bound";
                    cout << "\n5 for edge relaxation bound";
                    cout << "\n-1 for none";
                    cout << "\nEnter choice of parameter for strict bound: ";
                    cin >> bound_choice;
                }
                else
                    bound_choice = stoi(token_cmd[cmd_index++]);

                switch (bound_choice)
                {
                case 1:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of edges: ";
                        cin >> num_of_edges_bound;
                        //                                cout << "\nEnter path length bound: ";
                        //                                cin >> path_length_bound;
                        cout << "\nEnter bound for no. of nodes relaxed: ";
                        cin >> node_relaxation_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> edge_relaxation_bound;
                    }
                    else
                    {
                        num_of_edges_bound = stoi(token_cmd[cmd_index++]);
                        // path_length_bound = stoi(token_cmd[cmd_index++]);
                        node_relaxation_bound = stoi(token_cmd[cmd_index++]);
                        edge_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, BIN_SEARCH_ON_NODE_BOUND, up_reg_down_reg_size, smt_solver);
                    break;

                case 2:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of nodes: ";
                        cin >> num_of_nodes_bound;
                        //                                cout << "\nEnter path length bound: ";
                        //                                cin >> path_length_bound;
                        cout << "\nEnter bound for no. of nodes relaxed: ";
                        cin >> node_relaxation_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> edge_relaxation_bound;
                    }
                    else
                    {
                        num_of_nodes_bound = stoi(token_cmd[cmd_index++]);
                        // path_length_bound = stoi(token_cmd[cmd_index++]);
                        node_relaxation_bound = stoi(token_cmd[cmd_index++]);
                        edge_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, BIN_SEARCH_ON_EDGE_BOUND, up_reg_down_reg_size, smt_solver);
                    break;

                case 3:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of nodes: ";
                        cin >> num_of_nodes_bound;
                        cout << "\nEnter bound for no. of edges: ";
                        cin >> num_of_edges_bound;
                        cout << "\nEnter bound for no. of nodes relaxed: ";
                        cin >> node_relaxation_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> edge_relaxation_bound;
                    }
                    else
                    {
                        num_of_nodes_bound = stoi(token_cmd[cmd_index++]);
                        num_of_edges_bound = stoi(token_cmd[cmd_index++]);
                        node_relaxation_bound = stoi(token_cmd[cmd_index++]);
                        edge_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, BIN_SEARCH_ON_PATH_BOUND, up_reg_down_reg_size, smt_solver);
                    break;

                case 4:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of nodes: ";
                        cin >> num_of_nodes_bound;
                        cout << "\nEnter bound for no. of edges: ";
                        cin >> num_of_edges_bound;
                        //                                cout << "\nEnter bound for no. of nodes relaxed: ";
                        //                                cin >> path_length_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> edge_relaxation_bound;
                    }
                    else
                    {
                        num_of_nodes_bound = stoi(token_cmd[cmd_index++]);
                        num_of_edges_bound = stoi(token_cmd[cmd_index++]);
                        // path_length_bound = stoi(token_cmd[cmd_index++]);
                        edge_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, BIN_SEARCH_ON_NODE_RELAX_BOUND, up_reg_down_reg_size, smt_solver);
                    break;

                case 5:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of nodes: ";
                        cin >> num_of_nodes_bound;
                        cout << "\nEnter bound for no. of edges: ";
                        cin >> num_of_edges_bound;
                        //                                cout << "\nEnter bound for no. of nodes relaxed: ";
                        //                                cin >> path_length_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> node_relaxation_bound;
                    }
                    else
                    {
                        num_of_nodes_bound = stoi(token_cmd[cmd_index++]);
                        num_of_edges_bound = stoi(token_cmd[cmd_index++]);
                        // path_length_bound = stoi(token_cmd[cmd_index++]);
                        node_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, BIN_SEARCH_ON_EDGE_RELAX_BOUND, up_reg_down_reg_size, smt_solver);
                    break;

                case 6:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of nodes: ";
                        cin >> num_of_nodes_bound;
                        cout << "\nEnter bound for no. of edges: ";
                        cin >> num_of_edges_bound;
                        //                                cout << "\nEnter path length bound: ";
                        //                                cin >> path_length_bound;
                        cout << "\nEnter bound for no. of nodes relaxed: ";
                        cin >> node_relaxation_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> edge_relaxation_bound;
                    }
                    else
                    {
                        num_of_nodes_bound = stoi(token_cmd[cmd_index++]);
                        num_of_edges_bound = stoi(token_cmd[cmd_index++]);
                        // path_length_bound = stoi(token_cmd[cmd_index++]);
                        node_relaxation_bound = stoi(token_cmd[cmd_index++]);
                        edge_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, EXPERIMENTAL, up_reg_down_reg_size, smt_solver);
                    break;

                default:
                    if (!b_mode)
                    {
                        cout << "\nEnter bound for no. of nodes: ";
                        cin >> num_of_nodes_bound;
                        cout << "\nEnter bound for no. of edges: ";
                        cin >> num_of_edges_bound;
                        //                                cout << "\nEnter path length bound: ";
                        //                                cin >> path_length_bound;
                        cout << "\nEnter bound for no. of nodes relaxed: ";
                        cin >> node_relaxation_bound;
                        cout << "\nEnter bound for no. of edges relaxed: ";
                        cin >> edge_relaxation_bound;
                    }
                    else
                    {
                        num_of_nodes_bound = stoi(token_cmd[cmd_index++]);
                        num_of_edges_bound = stoi(token_cmd[cmd_index++]);
                        // path_length_bound = stoi(token_cmd[cmd_index++]);
                        node_relaxation_bound = stoi(token_cmd[cmd_index++]);
                        edge_relaxation_bound = stoi(token_cmd[cmd_index++]);
                    }
                    graph_man->solve_mincut_constraints(em, graph, file_prefix, num_of_nodes_bound, num_of_edges_bound, path_length_bound, node_relaxation_bound, edge_relaxation_bound, NO_BIN_SEARCH, up_reg_down_reg_size, smt_solver);
                    break;
                }
                cout << "Before processing smt output" << endl;
                graph_man->process_smt_output(graph, file_prefix, smt_out_filename, up_reg_nids, nid_to_value_pairs, eid_to_value_pairs, smt_solver, false);
                cout << "After processing smt output" << endl;

                set<string> node_repids_present_in_curr_soln;
                for (auto i = nid_to_value_pairs.begin(); i != nid_to_value_pairs.end(); i++)
                {
                    int curr_nid = i->first;
                    bool curr_value = i->second;

                    if (curr_value)
                    {
                        string rep_id = graph->get_rep_id_from_nid(curr_nid);
                        node_repids_present_in_curr_soln.insert(rep_id);
                    }
                }

                string eid_to_mincut_level_filename = file_prefix + "_edge_to_mincut_level";
                ofstream mincut_level_fout(eid_to_mincut_level_filename.c_str());
                for (auto i = eid_to_value_pairs.begin(); i != eid_to_value_pairs.end(); i++)
                {
                    int curr_eid = i->first;
                    bool curr_value = i->second;

                    if (curr_value)
                    {
                        int curr_eid_src_nid = graph->get_source_node(curr_eid);
                        int curr_eid_tgt_nid = graph->get_target_node(curr_eid);
                        string curr_edge_type = concatenate_strings2(graph->get_edge_subtypes_of_edge(curr_eid), " ");
                        mincut_level_fout << graph_man->kegg_hsa_id_to_display_name_map[graph->get_rep_id_from_nid(curr_eid_src_nid)] << "\t";
                        mincut_level_fout << curr_edge_type << "\t";
                        mincut_level_fout << graph_man->kegg_hsa_id_to_display_name_map[graph->get_rep_id_from_nid(curr_eid_tgt_nid)] << "\t";

                        int eid_src_nseq = GET_NODE_SEQ_NUM(graph, curr_eid_src_nid);
                        int eid_tgt_nseq = GET_NODE_SEQ_NUM(graph, curr_eid_tgt_nid);

                        mincut_level_fout << edge_level_matrix[eid_src_nseq][eid_tgt_nseq] << endl;
                    }
                }
                mincut_level_fout.close();
                // edge_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0]

                int new_sg_gid = graph_man->select_operation_on_graph(graphnum, node_repids_present_in_curr_soln);
                assert(new_sg_gid != -1);

                GraphNew *new_sg = graph_man->get_graph(new_sg_gid);
                cout << "Resultant subgraph is " << new_sg->get_graph_id() << endl;
            }
        }

        else if (command == "get_all_solutions" || command == "soln")
        {
            TypeOfExpressionTuple te1 = {TYPE_BOOL, 1};
            list<int> all_sg_gids;

            int graphnum;
            string file_prefix;
            int num_soln;

            int cmd_index = 1;
            if (!b_mode)
            {
                cout << "\nFor graph number: ";
                cin >> graphnum;
                cout << "\nEnter file prefix: ";
                cin >> file_prefix;
                cout << "\nHow many solutions? (enter integer number): ";
                cin >> num_soln;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                file_prefix = token_cmd[cmd_index++];
                num_soln = stoi(token_cmd[cmd_index++]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            int soln_counter = 0;

            file_prefix = OUTPUT_FILES_DIR_PREFIX + file_prefix;

            // For constructing smt2 file
            string smt_filename = file_prefix + ".smt2";
            string smt_output_filename = file_prefix + "_smt_output";
            string blocking_clause_filename = file_prefix + "_blocking_clauses";
            ofstream blocking(blocking_clause_filename.c_str());
            string header_filename = file_prefix + "_header";
            string footer_filename = file_prefix + "_footer";
            string decl_filename = file_prefix + "_decl";
            string cons_filename = file_prefix + "_cons";
            string bound_filename = file_prefix + "_bound";
            string bound_cons_filename = file_prefix + "_bound_cons";

            // For counting occurrences
            GraphNew *graph = graph_man->get_graph(graphnum);
            vector<int> original_graph_nids = graph->get_node_ids();
            vector<int> original_graph_eids = graph->get_edge_ids();

            map<int, int> nid_to_occurrence_map;
            map<int, int> eid_to_occurrence_map;

            for (auto i = original_graph_nids.begin(); i != original_graph_nids.end(); i++)
            {
                nid_to_occurrence_map.insert(pair<int, int>(*i, 0));
            }

            for (auto i = original_graph_eids.begin(); i != original_graph_eids.end(); i++)
            {
                eid_to_occurrence_map.insert(pair<int, int>(*i, 0));
            }

            // For writing solutions to dot file
            string node_color_map_filename = "p9_colorfile";
            string edge_color_map_filename = file_prefix + "_colorfile";
            map<int, int> eid_to_color_map;
            //            string active_nodes_filename = file_prefix + "_active_nodes_repids";
            //            string inactive_nodes_filename = file_prefix + "_inactive_nodes_repids";
            //            string relaxed_nodes_filename = file_prefix + "_relaxed_nodes_repids";
            //            string relaxed_edges_repids_filename = file_prefix + "_relaxed_edges_repids";
            string display_name = file_prefix + "_all_solutions_merged" + ".dot";

            // temporarily asserting absence of PIK3R5
            //            ofstream ofs(bound_cons_filename.c_str(), std::ios_base::app | std::ios_base::out);
            //            int pik3r5_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id("hsa23533"));
            //            assert (pik3r5_nid != -1);
            //
            //            ofs << "(assert (not __node_" << pik3r5_nid << "))" << endl;
            //            ofs.close();
            //            cout << "PIK3R5 asserted absent" << endl;

            while (1)
            {
                string systemCommand1 = "cat " + header_filename + " " + decl_filename + " " + cons_filename + " " + bound_cons_filename + " " + footer_filename + " > " + smt_filename;
                system(systemCommand1.c_str());
                sleep(2);

                string smt_solver = Z3_PATH_NAME;
                string systemCommand2 = smt_solver + " -smt2 " + smt_filename + " > " + smt_output_filename;
                system(systemCommand2.c_str());
                sleep(2);

                string soln;
                ifstream fin_soln(smt_output_filename.c_str());
                fin_soln >> soln;

                if (soln == "unsat")
                {
                    // cout << "\nNo solutions";
                    break;
                }

                else if (soln == "sat")
                { // got a sat solution
                    soln_counter++;

                    if (soln_counter > num_soln)
                    {
                        cout << "Soln bound reached." << endl;
                        break;
                    }

                    map<int, bool> nid_to_value_pairs;
                    map<int, bool> eid_to_value_pairs;
                    set<string> repids_present_in_curr_soln;
                    set<int> up_reg_nids;

                    graph_man->process_smt_output(graph, file_prefix, smt_output_filename, up_reg_nids, nid_to_value_pairs, eid_to_value_pairs, "z3", true);

                    // Writing and appending blocking clause
                    ofstream ofs(bound_cons_filename.c_str(), std::ios_base::app | std::ios_base::out);
                    ofs << "(assert (not (and ";

                    int id;
                    bool value;

                    for (auto i = nid_to_value_pairs.begin(); i != nid_to_value_pairs.end(); i++)
                    {
                        int curr_nid = i->first;
                        bool curr_value = i->second;

                        if (curr_value)
                        {
                            ofs << " "
                                << "__node_" << curr_nid;
                            nid_to_occurrence_map[curr_nid]++; // Updating node occurrence map
                            repids_present_in_curr_soln.insert(graph->get_rep_id_from_nid(curr_nid));
                        }
                        else
                        {
                            ofs << " (not "
                                << "__node_" << curr_nid << ")";
                        }
                    }

                    ofs << ")))" << endl;
                    ofs.close();

                    // Updating edge occurrence map
                    for (auto i = eid_to_value_pairs.begin(); i != eid_to_value_pairs.end(); i++)
                    {
                        int curr_eid = i->first;
                        bool curr_value = i->second;
                        if (curr_value)
                            eid_to_occurrence_map[curr_eid]++;
                    }

                    int new_sg_gid = graph_man->select_operation_on_graph(graphnum, repids_present_in_curr_soln);
                    assert(new_sg_gid != -1);

                    GraphNew *new_subgraph = graph_man->get_graph(new_sg_gid);

                    // Display solution statistics
                    cout << endl;
                    cout << "Solution " << soln_counter << endl;
                    cout << "No. of nodes " << new_subgraph->get_node_ids().size() << endl;
                    cout << "No. of edges " << new_subgraph->get_edge_ids().size() << endl;

                    // Writing solution graph as xml
                    string outfilename = file_prefix + "_soln" + soln_counter + ".xml";
                    ofstream fout(outfilename.c_str());
                    if (fout.is_open())
                        new_subgraph->write_to_xml_file(fout, b_mode, "KEGG");
                    fout.close();

                    // writing solution graph as dot
                    string dot_outfilename = file_prefix + "_soln" + soln_counter; // do not append ".dot" here

                    string active_nodes_filename = file_prefix + "_soln" + soln_counter + "_active_nodes_repids";
                    string inactive_nodes_filename = file_prefix + "_soln" + soln_counter + "_inactive_nodes_repids";
                    string relaxed_nodes_filename = file_prefix + "_soln" + soln_counter + "_relaxed_nodes_repids";
                    string relaxed_edges_repids_filename = file_prefix + "_soln" + soln_counter + "_relaxed_edges_repids";

                    new_subgraph->write_graph_as_dot_richer_with_edge_color(node_color_map_filename, edge_color_map_filename, eid_to_color_map, active_nodes_filename, inactive_nodes_filename, relaxed_nodes_filename, relaxed_edges_repids_filename, graph_man, dot_outfilename);

                    all_sg_gids.push_back(new_sg_gid);
                }

                else
                {
                    cerr << "Error in smt output file" << endl;
                }
            }

            cout << endl;
            if (soln_counter == 0)
            {
                cout << "No solutions found" << endl;
            }
            else
            {
                cout << "Solutions counted: " << soln_counter - 1 << endl;

                list<unordered_map<string, set<string>>> map_list;
                map_list.clear();

                vector<string> MAPK_merged_ids;
                map<string, set<string>> MAPK_node_to_pathway_id_map;

                int merged_graphnum = graph_man->merge_graphs(all_sg_gids, map_list, MAPK_merged_ids, MAPK_node_to_pathway_id_map);
                cout << endl;
                cout << "Merged graph id: " << merged_graphnum << endl;

                // Display merged solution graph statistics
                GraphNew *merged_graph = graph_man->get_graph(merged_graphnum);
                cout << "No. of nodes " << merged_graph->get_node_ids().size() << endl;
                cout << "No. of edges " << merged_graph->get_edge_ids().size() << endl;

                // Write merged solutions to dot file
                merged_graph->display_graph_richer_with_edge_color(node_color_map_filename, edge_color_map_filename, eid_to_color_map, "0", "0", "0", "0", graph_man, display_name);

                // Write merged solutions to xml file
                string merged_xml_filename = file_prefix + "_merged_soln_graph.xml";
                ofstream merged_out(merged_xml_filename.c_str());
                if (merged_out.is_open())
                {
                    merged_graph->write_to_xml_file(merged_out, b_mode, "KEGG");
                    merged_out.close();
                    cout << "Written merged solutions to file " << merged_xml_filename << endl;
                }
                else
                {
                    cerr << "Error: couldn't open file " << merged_xml_filename << endl;
                }

                // Write node occurrences in files sorted by value
                vector<pair<int, int>> vec_nid_occurrence; // map needs to be converted to vector of pairs
                for (auto i = nid_to_occurrence_map.begin(); i != nid_to_occurrence_map.end(); i++)
                {
                    int curr_nid = i->first;
                    int curr_occurrence = i->second;
                    vec_nid_occurrence.push_back(make_pair(curr_nid, curr_occurrence));
                }
                sort(vec_nid_occurrence.begin(), vec_nid_occurrence.end(), compare_pair_on_second_descending);

                string node_occurrence_filename = file_prefix + "_node_occurrence_sorted";
                ofstream node_occ_ofs(node_occurrence_filename.c_str());

                for (auto i = vec_nid_occurrence.begin(); i != vec_nid_occurrence.end(); i++)
                {
                    int curr_nid = i->first;
                    int curr_occurrence = i->second;

                    node_occ_ofs << curr_occurrence << "\t" << graph->get_all_display_ids_of_node(curr_nid)[0] << endl;
                }
                node_occ_ofs.close();

                // Write edge occurrences in files sorted by value
                vector<pair<int, int>> vec_eid_occurrence; // map needs to be converted to vector of pairs
                for (auto i = eid_to_occurrence_map.begin(); i != eid_to_occurrence_map.end(); i++)
                {
                    int curr_eid = i->first;
                    int curr_occurrence = i->second;
                    vec_eid_occurrence.push_back(make_pair(curr_eid, curr_occurrence));
                }
                sort(vec_eid_occurrence.begin(), vec_eid_occurrence.end(), compare_pair_on_second_descending);

                string edge_occurrence_filename = file_prefix + "_edge_occurrence_sorted";
                ofstream edge_occ_ofs(edge_occurrence_filename.c_str());

                for (auto i = vec_eid_occurrence.begin(); i != vec_eid_occurrence.end(); i++)
                {
                    int curr_eid = i->first;
                    int curr_occurrence = i->second;

                    edge_occ_ofs << curr_occurrence << "\t" << graph->get_all_display_ids_of_node(graph->get_source_node(curr_eid))[0] << "\t" << graph->get_all_display_ids_of_node(graph->get_target_node(curr_eid))[0] << "\t" << concatenate_strings2(graph->get_edge_subtypes_of_edge(curr_eid), "_") << endl;
                }
                edge_occ_ofs.close();
            }
        }

        else if (command == "get_path_with_z3" || command == "pathz3")
        {

            int graphnum;

            int node_relax_bound_lower_limit, node_relax_bound_upper_limit, edge_relax_bound_lower_limit, edge_relax_bound_upper_limit;
            size_t num_of_edges = edge_relax_bound_upper_limit + 1;
            size_t num_of_nodes = node_relax_bound_lower_limit + 1;

            int mode;
            int path_bound;
            int w, x, y, z;
            int num_of_solns_to_count;
            int num_of_solns_to_explore;
            unsigned long timeout1, timeout2;
            int cmd_index = 1;

            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                exit(1);
            }
            else
            {
                map<string, bool> var_value_pairs;

                string file_prefix = "";
                string connect_pairs_filename;
                string up_reg_filename;
                string down_reg_filename;
                string essential_node_filename;
                string avoid_node_filename;
                string essential_edge_filename;
                string avoid_edge_filename;
                string active_filename;
                string inactive_filename;
                string confirmed_up_reg_filename;
                string confirmed_down_reg_filename;
                string relaxed_nodes_filename;
                string nonrelaxed_nodes_filename;
                string relaxed_edges_filename;
                string nonrelaxed_edges_filename;
                string fold_change_filename;

                string PO_points_input_file;

                if (!b_mode)
                {
                    cout << "Operation mode: ";
                    cin >> mode;
                    cout << "Enter connect pairs file: ";
                    cin >> connect_pairs_filename;
                    cout << "Node relax bounds: ";
                    cin >> w;
                    cin >> x;
                    cin >> y;
                    cin >> z;
                    cout << "Enter path bound: ";
                    cin >> path_bound;
                    cout << "Enter preferred file prefix: ";
                    cin >> file_prefix;
                    cout << "Enter up regulated filename: ";
                    cin >> up_reg_filename;
                    cout << "Enter down regulated filename: ";
                    cin >> down_reg_filename;
                    cout << "Enter essential nodes filename:";
                    cin >> essential_node_filename;
                    cout << "Enter avoid nodes filename:";
                    cin >> avoid_node_filename;
                    cout << "Enter essential edges filename:";
                    cin >> essential_edge_filename;
                    cout << "Enter avoid edges filename:";
                    cin >> avoid_edge_filename;
                    cout << "Enter active nodes filename:";
                    cin >> active_filename;
                    cout << "Enter inactive nodes filename:";
                    cin >> inactive_filename;
                    cout << "Enter confirmed up-reg filename:";
                    cin >> confirmed_up_reg_filename;
                    cout << "Enter confirmed down-reg filename:";
                    cin >> confirmed_down_reg_filename;
                    cout << "Enter relaxed nodes filename:";
                    cin >> relaxed_nodes_filename;
                    cout << "Enter non-relaxed nodes filename:";
                    cin >> nonrelaxed_nodes_filename;
                    cout << "Enter relaxed edges filename:";
                    cin >> relaxed_edges_filename;
                    cout << "Enter non-relaxed edges filename:";
                    cin >> nonrelaxed_edges_filename;
                    cout << "Enter fold change file: ";
                    cin >> fold_change_filename;
                    cout << "At each PO point, count until number of solns exceeds: ";
                    cin >> num_of_solns_to_count;
                    cout << "Explore a solution until number of solns exceeds: ";
                    cin >> num_of_solns_to_explore;
                    cout << "Timeout for incremental solver (in millisecs): ";
                    cin >> timeout1;
                    cout << "Timeout for non-incremental solver (in millisecs): ";
                    cin >> timeout2;
                    if (mode == 1)
                    {
                        cout << "Enter file with PO points: ";
                        cin >> PO_points_input_file;
                    }
                }
                else
                {
                    mode = stoi(token_cmd[cmd_index++]);
                    connect_pairs_filename = token_cmd[cmd_index++];
                    w = stoi(token_cmd[cmd_index++]);
                    x = stoi(token_cmd[cmd_index++]);
                    y = stoi(token_cmd[cmd_index++]);
                    z = stoi(token_cmd[cmd_index++]);
                    path_bound = stoi(token_cmd[cmd_index++]);
                    file_prefix = token_cmd[cmd_index++];
                    up_reg_filename = token_cmd[cmd_index++];
                    down_reg_filename = token_cmd[cmd_index++];
                    essential_node_filename = token_cmd[cmd_index++];
                    avoid_node_filename = token_cmd[cmd_index++];
                    essential_edge_filename = token_cmd[cmd_index++];
                    avoid_edge_filename = token_cmd[cmd_index++];
                    active_filename = token_cmd[cmd_index++];
                    inactive_filename = token_cmd[cmd_index++];
                    confirmed_up_reg_filename = token_cmd[cmd_index++];
                    confirmed_down_reg_filename = token_cmd[cmd_index++];
                    relaxed_nodes_filename = token_cmd[cmd_index++];
                    nonrelaxed_nodes_filename = token_cmd[cmd_index++];
                    relaxed_edges_filename = token_cmd[cmd_index++];
                    nonrelaxed_edges_filename = token_cmd[cmd_index++];
                    fold_change_filename = token_cmd[cmd_index++];
                    num_of_solns_to_count = stoi(token_cmd[cmd_index++]);
                    num_of_solns_to_explore = stoi(token_cmd[cmd_index++]);
                    timeout1 = stoul(token_cmd[cmd_index++]);
                    timeout2 = stoul(token_cmd[cmd_index++]);
                    if (mode == 1)
                    {
                        PO_points_input_file = token_cmd[cmd_index++];
                    }
                }

                file_prefix = OUTPUT_FILES_DIR_PREFIX + file_prefix;

                set<int> nids_as_source;
                set<int> nids_as_target;
                vector<pair<int, int>> connect_pairs;

                graph_man->read_pairs_file(graph, connect_pairs_filename, connect_pairs, nids_as_source, nids_as_target);

                cout << connect_pairs.size() << " connect pairs" << endl;

                // compute Gomory-Hu tree
                int graphnum_undirected = graph_man->convert_to_undirected_graph(graphnum);
                GraphNew *ugraph = graph_man->get_graph(graphnum_undirected);

                vector<int> node_ids;
                vector<int> gomoryhu_parents;

                graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

                graph_man->fill_adj_matrix_for_graph(graph);
                int n = graph->get_node_ids().size();

                vector<vector<bool>> closure_matrix(n, vector<bool>(n, false));

                graph_man->compute_transitive_closure(graph, closure_matrix);

                node_ids.clear();
                gomoryhu_parents.clear();

                ugraph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

                vector<set<int>> cut_edges(node_ids.size());

                vector<vector<int>> call_level_matrix(n, vector<int>(n, BIGNUM));
                vector<vector<int>> edge_level_matrix(n, vector<int>(n, BIGNUM));
                vector<vector<int>> call_count_matrix(n, vector<int>(n, 0));
                vector<vector<int>> edge_count_matrix(n, vector<int>(n, 0));

                // #ifdef COMPUTE_MINCUT_EDGES_FLAG
                //                 time(&time1);
                //                 int ghtree_gid = graph_man->compute_gh_tree(graphnum_undirected, node_ids, gomoryhu_parents, cut_edges);
                //                 cout << "Gomory-Hu tree has graph id: " << ghtree_gid << endl;
                //                 time(&time2);
                //               time_in_sec = difftime(time2, time1);
                //                 cout << "Time taken to construct GH tree (in sec): " << time_in_sec << endl;

                // #endif

                //                set<int> nids_as_source;
                //                set<int> nids_as_target;
                //                vector<pair<int, int> > connect_pairs;
                //
                //                graph_man->read_pairs_file(graph, connect_pairs_filename, connect_pairs, nids_as_source, nids_as_target);
                //
                //                cout << connect_pairs.size() << " connect pairs" << endl;

                unordered_map<string, bool> data_map;
                graph_man->read_data_file_into_map(graph, up_reg_filename, down_reg_filename, data_map);

                set<int> up_reg_nids;
                set<int> down_reg_nids;
                set<int> essential_nids;
                set<int> avoid_nids;
                set<int> essential_eids;
                set<int> avoid_eids;
                set<int> up_reg_minus_avoid_nids;

                set<int> active_nids;
                set<int> inactive_nids;
                set<int> confirmed_up_reg_nids;
                set<int> confirmed_down_reg_nids;
                set<int> up_reg_nids_minus_confirmed_down_reg_nids;
                set<int> down_reg_nids_minus_confirmed_up_reg_nids;

                set<int> up_reg_nids_to_use;
                set<int> down_reg_nids_to_use;

                set<int> relaxed_nids;
                set<int> nonrelaxed_nids;

                set<int> relaxed_eids;
                set<int> nonrelaxed_eids;

                graph_man->read_data_file_into_set_of_ints(graph, up_reg_filename, up_reg_nids);
                graph_man->read_data_file_into_set_of_ints(graph, down_reg_filename, down_reg_nids);
                graph_man->read_data_file_into_set_of_ints(graph, essential_node_filename, essential_nids);
                graph_man->read_data_file_into_set_of_ints(graph, avoid_node_filename, avoid_nids);
                graph_man->read_edge_data_file_into_set_of_ints(graph, essential_edge_filename, essential_eids);
                graph_man->read_edge_data_file_into_set_of_ints(graph, avoid_edge_filename, avoid_eids);

                // remove nodes to avoid from up_reg_nids
                set_difference(up_reg_nids.begin(), up_reg_nids.end(), avoid_nids.begin(), avoid_nids.end(), inserter(up_reg_minus_avoid_nids, up_reg_minus_avoid_nids.begin()));

                graph_man->read_data_file_into_set_of_ints(graph, active_filename, active_nids);
                graph_man->read_data_file_into_set_of_ints(graph, inactive_filename, inactive_nids);
                graph_man->read_data_file_into_set_of_ints(graph, confirmed_up_reg_filename, confirmed_up_reg_nids);
                graph_man->read_data_file_into_set_of_ints(graph, confirmed_down_reg_filename, confirmed_down_reg_nids);

                set_difference(up_reg_minus_avoid_nids.begin(), up_reg_minus_avoid_nids.end(), confirmed_down_reg_nids.begin(), confirmed_down_reg_nids.end(), inserter(up_reg_nids_minus_confirmed_down_reg_nids, up_reg_nids_minus_confirmed_down_reg_nids.begin()));
                set_difference(down_reg_nids.begin(), down_reg_nids.end(), confirmed_up_reg_nids.begin(), confirmed_up_reg_nids.end(), inserter(down_reg_nids_minus_confirmed_up_reg_nids, down_reg_nids_minus_confirmed_up_reg_nids.begin()));

                set_union(up_reg_nids_minus_confirmed_down_reg_nids.begin(), up_reg_nids_minus_confirmed_down_reg_nids.end(), confirmed_up_reg_nids.begin(), confirmed_up_reg_nids.end(), inserter(up_reg_nids_to_use, up_reg_nids_to_use.begin()));
                set_union(down_reg_nids_minus_confirmed_up_reg_nids.begin(), down_reg_nids_minus_confirmed_up_reg_nids.end(), confirmed_down_reg_nids.begin(), confirmed_down_reg_nids.end(), inserter(down_reg_nids_to_use, down_reg_nids_to_use.begin()));

                graph_man->read_data_file_into_set_of_ints(graph, relaxed_nodes_filename, relaxed_nids);
                graph_man->read_data_file_into_set_of_ints(graph, nonrelaxed_nodes_filename, nonrelaxed_nids);
                graph_man->read_edge_data_file_into_set_of_ints(graph, relaxed_edges_filename, relaxed_eids);
                graph_man->read_edge_data_file_into_set_of_ints(graph, nonrelaxed_edges_filename, nonrelaxed_eids);

                cout << "Nodes to avoid: " << avoid_nids.size() << endl;
                cout << "Edges to avoid: " << avoid_eids.size() << endl;
                cout << "Nodes to be active: " << active_nids.size() << endl;
                cout << "Nodes to be inactive: " << inactive_nids.size() << endl;
                cout << "Nodes not to relax: " << nonrelaxed_nids.size() << endl;
                cout << "Edges not to relax: " << nonrelaxed_eids.size() << endl;
                cout << endl;
                cout << "Up: " << up_reg_nids_to_use.size() << " Down: " << down_reg_nids_to_use.size() << endl;
                int up_down_size = up_reg_nids_to_use.size() + down_reg_nids_to_use.size();

                // Adjust upper and lower bounds of node relaxation, if needed.
                // This cannot be more than up_down_size: Supratik, Nov 18, 2019

                bool nrelax_bound_resize = false;
                if (up_down_size < x)
                {
                    x = up_down_size;
                    nrelax_bound_resize = true;
                }
                if (up_down_size < z)
                {
                    z = up_down_size;
                    nrelax_bound_resize = true;
                }
                if (nrelax_bound_resize == true)
                {
                    cout << "Adjusting node relaxn bound to be consistent with max diff expr genes in reachable graph ("
                         << up_down_size << ")" << endl;
                }

                // default values for bounds
                int node_relaxation_bound = 0;
                int edge_relaxation_bound = 0;

                // create z3 context and solver
                z3::context c;
                z3::solver s(c, "QF_BV");
                // z3::solver s = z3::tactic(c, "smt").mk_solver();

                // p.set(":logic", QF_BV);

                z3::params p(c);
                //                //p.set(":ignore_solver1", true);

                p.set(":solver2_timeout", (unsigned)timeout1); // timeout for incremental solver
                p.set(":timeout", (unsigned)timeout2);         // overall timeout

                // p.set(":verbose", 15u); // verbosity level 15
                s.set(p);

                vector<int> nids = graph->get_node_ids();
                vector<int> eids = graph->get_edge_ids();

                // map of z3 expressions to ensure no duplicate expressions are created
                map<string, z3::expr> var_to_expr_map;

                graph_man->create_bool_expressions_one_id_with_z3(c, nids, "__node_", var_to_expr_map);
                graph_man->create_bool_expressions_one_id_with_z3(c, nids, "__nodeactive_", var_to_expr_map);
                graph_man->create_bool_expressions_one_id_with_z3(c, nids, "__noderelax_", var_to_expr_map);

                graph_man->create_bool_expressions_one_id_with_z3(c, eids, "__edge_", var_to_expr_map);
                graph_man->create_bool_expressions_one_id_with_z3(c, eids, "__edgerelax_", var_to_expr_map);
                graph_man->create_bool_expressions_one_id_with_z3(c, eids, "__edgepathflag_", var_to_expr_map);

                // graph_man->create_bool_expressions_two_ids_with_z3(c, connect_pairs, "__connect_", "_", var_to_expr_map);

                graph_man->create_bv_expressions_one_id_with_z3(c, nids, "__nodedistance_", PATH_BOUND_BIT_LENGTH(path_bound), var_to_expr_map);
                graph_man->create_bv_expression_single_var_with_z3(c, "__pathbound", PATH_BOUND_BIT_LENGTH(path_bound), var_to_expr_map);
                graph_man->create_bv_expression_single_var_with_z3(c, "__nodebound", NODE_BOUND_BIT_LENGTH(graph), var_to_expr_map);
                graph_man->create_bv_expression_single_var_with_z3(c, "__edgebound", EDGE_BOUND_BIT_LENGTH(graph), var_to_expr_map);
                graph_man->create_bv_expression_single_var_with_z3(c, "__nodestatusrelaxbound", NODE_RELAX_BOUND_BIT_LENGTH(up_down_size), var_to_expr_map);
                graph_man->create_bv_expression_single_var_with_z3(c, "__edgestatusrelaxbound", EDGE_RELAX_BOUND_BIT_LENGTH(graph), var_to_expr_map);

                // s.push();

                cout << "Beginning to generate constraints" << endl;

                graph_man->get_connectivity_with_z3(c, s, var_to_expr_map, graphnum, graphnum_undirected, closure_matrix, cut_edges, gomoryhu_parents, call_level_matrix, edge_level_matrix, call_count_matrix, edge_count_matrix, connect_pairs, path_bound, nids_as_source, nids_as_target, up_reg_nids_to_use, down_reg_nids_to_use, essential_nids, avoid_nids, essential_eids, avoid_eids, active_nids, inactive_nids, confirmed_up_reg_nids, confirmed_down_reg_nids, relaxed_nids, nonrelaxed_nids, relaxed_eids, nonrelaxed_eids, fold_change_filename, file_prefix);

                cout << "Finished generating constraints" << endl;

                cout << "Not printing matrices in the current run" << endl;

                /********************** Commenting out printing of matrices

                // -- print mincut matrices
                        string call_level_filename = file_prefix + "_call_level_matrix";
                        string edge_level_filename = file_prefix + "_edge_level_matrix";
                        string call_count_filename = file_prefix + "_call_count_matrix";
                        string edge_count_filename = file_prefix + "_edge_count_matrix";

                        ofstream call_level(call_level_filename.c_str());
                        ofstream edge_level(edge_level_filename.c_str());
                        ofstream call_count(call_count_filename.c_str());
                        ofstream edge_count(edge_count_filename.c_str());

                        call_level << "\t";
                        edge_level << "\t";
                        call_count << "\t";
                        edge_count << "\t";
                        for (int i = 0; i < n; i++) {
                            call_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                            edge_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                            call_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                            edge_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                        }

                        call_level << endl;
                        edge_level << endl;
                        call_count << endl;
                        edge_count << endl;

                        for (int i = 0; i < n; i++) {
                            call_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                            edge_level << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                            call_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";
                            edge_count << graph->get_all_display_ids_of_node(GET_NODE_ID(graph, i))[0] << "\t";

                            for (int j = 0; j < n; j++) {

                                int curr_entry = call_level_matrix[i][j];
                                string curr_entry_str = IntToString(curr_entry);
                                if (curr_entry  == inf)
                                    int curr_entry_str = stoi("0");
                                call_level << curr_entry_str << "\t";

                                curr_entry = edge_level_matrix[i][j];
                                curr_entry_str = IntToString(curr_entry);
                                if (curr_entry  == inf)
                                    curr_entry_str = stoi("0");
                                edge_level << curr_entry_str << "\t";

                                call_count << call_count_matrix[i][j] << "\t";
                                edge_count << edge_count_matrix[i][j] << "\t";
                            }
                            call_level << endl;
                            edge_level << endl;
                            call_count << endl;
                            edge_count << endl;
                        }
                        call_level << endl;
                        edge_level << endl;
                        call_count << endl;
                        edge_count << endl;
                        // --


                        cout << "Printed matrices" << endl;
                ****************************/

                //                z3::check_result is_sat_constraints = s.check();
                //                cout << "Constraints are " << is_sat_constraints << endl;
                //                if (is_sat_constraints == z3::unsat){
                //                    exit(1);
                //                }

                vector<pair<int, int>> PO_value_pairs;
                //                graph_man->get_solution_z3(c, s, var_to_expr_map, graph, PO_value_pairs);
                //                exit(1);
                // graph_man->get_PO_curve (c, s, var_to_expr_map, graph, node_relax_bound_lower_limit, node_relax_bound_upper_limit, edge_relax_bound_lower_limit, edge_relax_bound_upper_limit, PO_value_pairs);

                // string timefile = graph_man->get_unique_prefix() + "limits_timefile.txt";
                // cout << graph_man->get_unique_prefix();
                string timefile = graph_man->unique_prefix + "limits_timefile.txt";
                ofstream fout(timefile.c_str());

                // string PO_points_file = graph_man->get_unique_prefix() + "PO.dat";
                string PO_points_file = graph_man->unique_prefix + "PO.dat";
                ofstream POout(PO_points_file.c_str());

                // string PO_points_txt_file = "PO.txt";
                // ofstream PO_txtout(PO_points_txt_file.c_str());

                int edge_relax_bound_lower_limit = w;
                int node_relax_bound_lower_limit = x;
                int edge_relax_bound_upper_limit = y;
                int node_relax_bound_upper_limit = z;

                //                vector< vector<sat_state_t> > sat_state_matrix(num_of_edges, vector<sat_state_t>(num_of_nodes));//, UNKNOWN) );
                //
                //
                //                for (size_t i = 0; i < num_of_edges; i++) {
                //                    vector<sat_state_t> nodes_vector(num_of_nodes);
                //                    std::fill(nodes_vector.begin(), nodes_vector.end(), sat_state_t::UNDEFINED);
                //                    sat_state_matrix.at(i) = nodes_vector;
                //
                //                }

                if (mode == 0)
                {
                    cout << "Solving constraints to generate PO points..." << endl;
                    cout << "This may take some time ... See limits_timefile.txt to check progress" << endl;
                    graph_man->get_PO_points_within_limits(c, s, var_to_expr_map, graph, y, z, y, x, w, z, PO_value_pairs, fout, 0, up_down_size); //, sat_state_matrix);
                }
                else if (mode == 1)
                {
                    cout << "Solving constraints to check if PO curve touches given set of points ..." << endl;
                    cout << "This may take some time ... See limits_timefile.txt to check progress" << endl;
                    ifstream fin(PO_points_input_file.c_str());

                    if (!fin.is_open())
                    {
                        cerr << "File " + PO_points_input_file + " could not be opened." << endl;
                        continue;
                    }

                    string line;
                    int n, e;
                    while (getline(fin, line))
                    {
                        stringstream l(line);
                        l >> n >> e;
                        PO_value_pairs.push_back(make_pair(e, n));
                    }

                    bool is_sat_on_PO_curve = graph_man->is_sat_at_existing_PO_points(c, s, var_to_expr_map, graph, PO_value_pairs, up_down_size);
                    if (is_sat_on_PO_curve)
                    {
                        cout << "Sat point obtained on previous curve" << endl;
                    }
                    else
                    {
                        cout << "New curve completely separated from previous points" << endl;
                    }
                    continue;
                }
                else
                {
                    cout << "Invalid mode for pathz3";
                    continue;
                }

                cout << PO_value_pairs.size() << " PO points found" << endl;
                std::sort(PO_value_pairs.begin(), PO_value_pairs.end(), compare_pair_on_second_descending);

                // assert (PO_value_pairs.size() > 0);

                int last_edge_relax_bound = -1;
                for (auto iter = PO_value_pairs.begin(); iter != PO_value_pairs.end(); iter++)
                {
                    int e = iter->first;
                    int n = iter->second;

                    if (e == last_edge_relax_bound)
                    {
                        cout << "(" << e << " ," << n << ") is dominated" << endl;
                        continue;
                    }

                    else
                    {
                        cout << "(" << e << " ," << n << ") is not dominated" << endl;
                        last_edge_relax_bound = e;

                        POout << n << "\t" << e << endl;
                        // PO_txtout << "(" << e << "," << n << ")" << endl;
                    }
                }

                fout.close();
                POout.close();
                // PO_txtout.close();

                if (PO_value_pairs.size() == 0)
                {
                    cout << "No point in PO curve" << endl;
                    continue;
                }

                /********* Commented by Supratik: Nov 19, 2019
                 ********* Plotting of PO curves is now automatically taken care of from input script

                        system("gnuplot -p -e \"set term png; set output 'PO.png'; plot 'PO.dat' with linespoints\"");

                *********/

                string all_solns_filename = file_prefix + "_solutions.txt";
                string all_relax_filename = file_prefix + "_relaxations.txt";
                ofstream soln_out(all_solns_filename.c_str());
                ofstream relax_out(all_relax_filename.c_str());

                int num_of_PO_points = PO_value_pairs.size();
                for (auto PO_iter = PO_value_pairs.begin(); PO_iter != PO_value_pairs.end(); PO_iter++)
                {
                    pair<int, int> curr_PO_pair = *PO_iter;
                    int curr_edge_relax_bound = curr_PO_pair.first;
                    int curr_node_relax_bound = curr_PO_pair.second;
                    soln_out << "Current PO point: " << curr_node_relax_bound << " nodes, " << curr_edge_relax_bound << " edges relaxed" << endl;
                    relax_out << "Current PO point: " << curr_node_relax_bound << " nodes, " << curr_edge_relax_bound << " edges relaxed" << endl;

                    int num_of_solns_at_curr_PO_point = 0;

                    s.push();

                    s.add(ule(var_to_expr_map.at("__edgestatusrelaxbound"), curr_edge_relax_bound));
                    s.add(ule(var_to_expr_map.at("__nodestatusrelaxbound"), curr_node_relax_bound));

                    /********* Commented by Supratik: Nov 19, 2019
                     ********* This is needed only for debugging purposes

                    // Added by Supratik for debugging: Nov 17, 2019

                    string smtfile = "genSolns_nr_";
                    smtfile += (to_string(curr_node_relax_bound) + "_er_" + to_string(curr_edge_relax_bound) + ".smt2");
                    ofstream smt_out(smtfile.c_str());
                    smt_out << s.to_smt2() << endl;
                    smt_out.close();

                    **********/

                    while (num_of_solns_at_curr_PO_point <= num_of_solns_to_count)
                    {

                        z3::check_result is_sat;
                        is_sat = s.check();

                        // if sat, obtain solution model
                        if (is_sat == z3::sat)
                        {

                            z3::model m(c, s.get_model());
                            // cout << m << endl;

                            num_of_solns_at_curr_PO_point++;
                            soln_out << "Solution " << toString(num_of_solns_at_curr_PO_point) << endl;
                            relax_out << "Solution " << toString(num_of_solns_at_curr_PO_point) << endl;
                            cout << "Solution " << toString(num_of_solns_at_curr_PO_point) << endl;

                            vector<int> present_node_nids;
                            vector<int> present_edge_eids;
                            vector<int> active_node_nids;
                            vector<int> relaxed_node_nids;
                            vector<int> relaxed_edge_eids;

                            z3::expr soln_clause = c.bool_val(true);
                            graph_man->process_z3_model(graph, var_to_expr_map, m, present_node_nids, present_edge_eids, active_node_nids, relaxed_node_nids, relaxed_edge_eids, soln_clause);

                            if (num_of_solns_at_curr_PO_point <= num_of_solns_to_explore)
                            {

                                graph_man->print_variables_from_z3_model(graph, up_reg_nids_to_use, down_reg_nids_to_use, present_node_nids, present_edge_eids, active_node_nids, relaxed_node_nids, relaxed_edge_eids, soln_out, relax_out);
                            }

                            s.add(!soln_clause);
                            // is_sat = s.check();
                        }

                        // if unsat, exit loop
                        else if (is_sat == z3::unsat)
                        {
                            soln_out << "No more solutions. Obtained unsat" << endl;
                            relax_out << "No more solutions. Obtained unsat" << endl;
                            cout << "No more solutions. Obtained unsat" << endl;
                            break;
                        }

                        // if unknown, find reason and exit loop
                        else if (is_sat == z3::unknown)
                        {
                            soln_out << s.reason_unknown() << endl;
                            relax_out << s.reason_unknown() << endl;
                            break;
                        }

                        else
                        {
                            soln_out << "Invalid sat result " << is_sat << endl;
                            relax_out << "Invalid sat result " << is_sat << endl;
                            break;
                        }
                    }
                    soln_out << endl;
                    relax_out << endl;

                    s.pop();
                }

                soln_out.close();
                relax_out.close();
            }
        }

        else if (command == "rank")
        {
            int graphnum;
            string file_prefix;

            int cmd_index = 1;

            if (!b_mode)
            {
                cin >> graphnum;
                cout << "Enter output file prefix: ";
                cin >> file_prefix;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                file_prefix = token_cmd[cmd_index++];
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            //            string betweenness_filename = file_prefix + "_nodes_ranked_by_betweenness";
            string degree_filename = file_prefix + "_nodes_ranked_by_degree";

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            //            graph_man->print_nodes_ranked_by_betweenness(graph, betweenness_filename);
            graph_man->print_nodes_ranked_by_degree(graph, degree_filename);
        }

        else if (command == "count")
        {
            int graphnum;
            string file_prefix;
            int soln_bound;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
                cout << "File prefix: ";
                cin >> file_prefix;
                cout << "Upto how many solns: ";
                cin >> soln_bound;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                file_prefix = token_cmd[cmd_index++];
                soln_bound = stoi(token_cmd[cmd_index++]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            graph_man->count_smt_solutions(em, graph, file_prefix, soln_bound);
        }

        //////        else if (command == "countz3") {
        //////            int graphnum;
        //////            string file_prefix;
        //////            int soln_bound;
        //////            int cmd_index = 1;
        //////            if (!b_mode) {
        //////                cin >> graphnum;
        //////                cout << "File prefix: ";
        //////                cin >> file_prefix;
        //////                cout << "Upto how many solns: ";
        //////                cin >> soln_bound;
        //////            }
        //////            else {
        //////                graphnum = stoi(token_cmd[cmd_index++]);
        //////                file_prefix = token_cmd[cmd_index++];
        //////                soln_bound = stoi(token_cmd[cmd_index++]);
        //////            }
        //////
        //////            GraphNew * graph = graph_man->get_graph(graphnum);
        //////            if (graph == NULL) {
        //////                cout << "No such graph" << endl;
        //////                continue;
        //////            }
        //////
        //////
        //////
        //////            vector<int> result_gids;
        //////            graph_man->count_smt_solutions_with_z3(b_mode, c, s, var_to_expr_map, file_prefix, graph, soln_bound);
        //////
        //////        }

        else if (command == "enumerate_paths" || command == "enum")
        {
            int graphnum;
            string node_name;
            set<string> source_nodes_set;
            string nodes_filename, fold_change_filename;
            string out_filename;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
                cout << "Source node ids (end with -1): ";
                cin >> node_name;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                node_name = token_cmd[cmd_index++];
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            int cmd_itr = cmd_index;
            while (node_name != "-1")
            {
                source_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            // read target nodes
            set<string> target_nodes_set;
            if (!b_mode)
            {
                cout << "Target node ids (end by -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_itr];
                cmd_itr++;
            }

            while (node_name != "-1")
            {
                target_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_itr];
                    cmd_itr++;
                }
            }

            // read output filename from user
            if (!b_mode)
            {
                cout << "Enter output filename prefix: ";
                cin >> out_filename;
                cout << "Enter nodes filename for rank-ordering of paths: ";
                cin >> nodes_filename;
                cout << "Enter fold change filename: ";
                cin >> fold_change_filename;
            }
            else
            {
                out_filename = token_cmd[cmd_itr++];
                nodes_filename = token_cmd[cmd_itr++];
                fold_change_filename = token_cmd[cmd_itr++];
            }

            // get graph
            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }
            map<int, vector<string>> path_id_to_path_map;
            map<int, vector<int>> path_id_to_eids_map;
            map<int, vector<int>> path_id_to_nids_map;
            // call method to enumerate and print paths into text file
            graph_man->print_all_paths(graphnum, source_nodes_set, target_nodes_set, path_id_to_path_map, path_id_to_eids_map, out_filename);

            // rank ordering paths
            set<int> nodes_nids_for_rank_ordering;
            graph_man->read_data_file_into_set_of_ints(graph, nodes_filename, nodes_nids_for_rank_ordering);

            map<int, float> nid_to_expr_value_map;
            graph_man->read_fold_change_values_into_map(graph, nid_to_expr_value_map, fold_change_filename, 0);

            string ranked_paths_out_filename = out_filename + "_ranked";

            vector<int> path_ids_sorted_by_size;
            vector<int> path_ids_sorted_by_enrchmnt;
            graph_man->print_all_paths_rank_ordered(graph, nodes_nids_for_rank_ordering, nid_to_expr_value_map, path_id_to_path_map, path_ids_sorted_by_size, path_ids_sorted_by_enrchmnt, path_id_to_eids_map, ranked_paths_out_filename);

            // color top-ranked paths
            char coloring_choice;

            if (!b_mode)
            {
                cout << "Color paths (Y/n)? ";
                cin >> coloring_choice;
            }
            else
            {
                coloring_choice = token_cmd[cmd_itr++][0];
            }

            if (coloring_choice == 'n' || coloring_choice == 'N')
                continue;

            std::string color_map_filename;
            int similarity_pc, num_bot_nodes;
            if (!b_mode)
            {
                cout << "Enter node color map filename (0 to skip): ";
                cin >> color_map_filename;
                cout << "Path similarity percentage (1 to 100): ";
                cin >> similarity_pc;
                cout << "Enter number of bottleneck nodes to show: ";
                cin >> num_bot_nodes;
            }
            else
            {
                color_map_filename = token_cmd[cmd_itr++];
                similarity_pc = stoi(token_cmd[cmd_itr++]);
                num_bot_nodes = stoi(token_cmd[cmd_itr++]);
            }

            map<int, vector<int>> map_color_id_to_path_id;
            map<int, string> map_eid_to_color;

            map<int, int> eid_to_color_map;
            int color_index;
            vector<string> color_array;

            // group paths
            map<int, vector<int>> group_id_to_path_ids_map;
            graph->group_paths_by_similarity(path_id_to_path_map, path_ids_sorted_by_enrchmnt, similarity_pc, NUM_OF_COLORS, group_id_to_path_ids_map);
            for (auto i = group_id_to_path_ids_map.begin(); i != group_id_to_path_ids_map.end(); i++)
            {
                cout << i->first << "-> " << i->second.front() << endl;
            }

            int path_id;
            for (color_index = 1; color_index <= NUM_OF_COLORS; color_index++)
            {
                vector<int> paths_in_group = group_id_to_path_ids_map[color_index];
                if (paths_in_group.empty())
                    break; // paths exhausted before colors
                for (auto path_itr = paths_in_group.begin(); path_itr != paths_in_group.end(); path_itr++)
                {
                    path_id = *path_itr;
                    cout << color_index << ": ";
                    vector<int> eids_in_path = path_id_to_eids_map[path_id];
                    cout << "Path" << path_id << " ";
                    for (auto iter = eids_in_path.begin(); iter != eids_in_path.end(); iter++)
                    {
                        eid_to_color_map.insert(pair<int, int>(*iter, color_index));
                        cout << graph->get_all_display_ids_of_node(graph->get_source_node(*iter))[0] << "--" << graph->get_all_display_ids_of_node(graph->get_target_node(*iter))[0] << "\t";
                    }
                    cout << endl;
                }
            }
            string display_name = ranked_paths_out_filename + "_colored";
            string bot_color_map_filename = out_filename + "_colorfile";
            ofstream color_fout(bot_color_map_filename.c_str());

            ifstream nodes_color_in(color_map_filename.c_str());

            ofstream combined_colorfile("combined_colorfile");
            combined_colorfile << nodes_color_in.rdbuf() << color_fout.rdbuf();

            map<int, int> nid_to_degree_map_sorted;
            map<int, int> eid_to_degree_map_sorted;

            graph->get_nids_sorted_by_degree(path_id_to_path_map, path_id_to_eids_map, path_ids_sorted_by_enrchmnt, nid_to_degree_map_sorted, num_bot_nodes, combined_colorfile); // for detecting bottleneck nodes based on degree

            graph->get_eids_sorted_by_degree(path_id_to_path_map, path_id_to_eids_map, path_ids_sorted_by_enrchmnt, eid_to_degree_map_sorted, num_bot_nodes, eid_to_color_map);

            graph->display_graph_richer_with_edge_color("combined_colorfile", "0", eid_to_color_map, "0", "0", "0", "0", graph_man, display_name);

            //            map<int, float> nid_to_betw_map_sorted;
            //            graph->get_nids_sorted_by_betweenness(nid_to_betw_map_sorted);
        }

        else if (command == "gerstein")
        {
            int graphnum;
            string filename;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
                cout << "Enter output file prefix: ";
                cin >> filename;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                filename = token_cmd[cmd_index++];
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            string betweenness_filename = filename + "_nodes_ranked_by_betweenness";
            ofstream fout(betweenness_filename.c_str());

            GraphNew *graph = graph_man->get_graph(graphnum);
            map<int, float> nid_to_betw_map;
            graph->get_nids_sorted_by_betweenness(nid_to_betw_map);

            list<pair<int, float>> sorted_nid_to_betw_list;
            for (auto itr = nid_to_betw_map.begin(); itr != nid_to_betw_map.end(); itr++)
            {
                int key = itr->first;
                float value = itr->second;
                sorted_nid_to_betw_list.push_back(make_pair(key, value));
            }

            // std::sort(sorted_nid_to_betw_list.begin(), sorted_nid_to_betw_list.end(), compare_pair_on_second_descending_int_float);

            int count = 1;
            for (auto l_itr = sorted_nid_to_betw_list.begin(); l_itr != sorted_nid_to_betw_list.end(); l_itr++)
            {
                int nid = l_itr->first;
                // string name = graph->get_all_display_ids_of_node(nid)[0];
                string name = graph_man->kegg_hsa_id_to_display_name_map[graph->get_rep_id_from_nid(nid)];

                fout << count << "\t" << graph->get_rep_id_from_nid(nid) << "\t" << name << "\t" << l_itr->second << endl;
                count++;
            }
            fout << endl;
            fout.close();
        }

        else if (command == "common_paths" || command == "cpaths")
        {
            // int graphnum;
            string all_files_name_vec;

            list<int> gid_list;
            int cmd_index = 1;
            if (!b_mode)
            {

                cout << "Enter file with input file names vector: ";
                cin >> all_files_name_vec;
            }
            else
            {

                all_files_name_vec = token_cmd[cmd_index++];
            }

            ifstream ifs(all_files_name_vec.c_str());
            string gid;
            while (ifs >> gid)
            {
                gid_list.push_back(stoi(gid));
            }
            int cmd_itr = cmd_index;

            string filename;
            list<unordered_map<string, set<string>>> map_list;
            if (!b_mode)
            {
                cout << "Enter map file names (end by -1): ";
                cin >> filename;
            }
            else
            {
                filename = token_cmd[cmd_itr++];
            }
            while (filename != "-1")
            {
                unordered_map<string, set<string>> mapname;
                graph_man->read_naming_map(filename, mapname);
                map_list.push_back(mapname);
                if (!b_mode)
                    cin >> filename;
                else
                    filename = token_cmd[cmd_itr++];
            }

            string node_name;
            set<string> source_nodes_set;

            cmd_index = cmd_itr;
            if (!b_mode)
            {
                cout << "Source node ids (end with -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }

            cmd_itr = cmd_index;
            while (node_name != "-1")
            {
                source_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_index++];
                    cmd_itr++;
                }
            }

            // read target nodes
            set<string> target_nodes_set;
            if (!b_mode)
            {
                cout << "Target node ids (end by -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
                cmd_itr++;
            }

            while (node_name != "-1")
            {
                target_nodes_set.insert(node_name);
                if (!b_mode)
                {
                    cin >> node_name;
                }
                else
                {
                    node_name = token_cmd[cmd_index++];
                    cmd_itr++;
                }
            }

            int node_relax_cutoff;
            int edge_relax_cutoff;
            if (!b_mode)
            {
                cout << "Relax node intersection by (%): ";
                cin >> node_relax_cutoff;
                cout << "Relax edge intersection by (%): ";
                cin >> edge_relax_cutoff;
            }
            else
            {
                node_relax_cutoff = stoi(token_cmd[cmd_index++]);
                edge_relax_cutoff = stoi(token_cmd[cmd_index++]);
            }

            cout << gid_list.size() << " gids" << endl;

            GraphNew *graph;
            set<int> graph_set;
            for (auto i = gid_list.begin(); i != gid_list.end(); i++)
            {
                graph = graph_man->get_graph(*i);
                if (graph == NULL)
                    cout << "No graph with id " << *i << endl;
                else
                    graph_set.insert(*i);
            }
            if (graph_set.empty())
            {
                cout << "No graphs in set" << endl;
                continue;
            }

            vector<string> MAPK_merged_ids;
            map<string, set<string>> MAPK_node_to_pathway_id_map;

            int mgid = graph_man->merge_graphs(gid_list, map_list, MAPK_merged_ids, MAPK_node_to_pathway_id_map);

            GraphNew *merged_graph;

            if (mgid == -1)
            {
                cerr << "Could not merge graphs in cpaths" << endl;
                continue;
            }

            merged_graph = graph_man->get_graph(mgid);

            map<string, set<int>> edge_to_freq;
            map<string, set<int>> rep_id_to_freq_map;
            graph_man->get_common_nodes_from_graphs(graph_set, rep_id_to_freq_map);
            graph_man->get_common_edges_from_graphs(graph_set, edge_to_freq);

            graph_man->get_common_nodes_from_merged_graph(merged_graph, source_nodes_set, target_nodes_set, rep_id_to_freq_map, node_relax_cutoff);
            graph_man->get_subgraph_from_common_edges(merged_graph, source_nodes_set, target_nodes_set, edge_to_freq, edge_relax_cutoff);
        }

        //        else if (command == "bottlenecks" || command == "bot") {
        //            int graphnum, num_bot_nodes;
        //            string out_filename;
        //
        //            int cmd_index = 1;
        //
        //            // read output filename from user
        //            if (!b_mode) {
        //                cin >> graphnum;
        //                cout << "Enter number of bottleneck nodes to show: ";
        //                cin >> num_bot_nodes;
        //                cout << "\nEnter output filename prefix: ";
        //                cin >> out_filename;
        //            }
        //            else {
        //                graphnum = stoi(token_cmd[cmd_index++]);
        //                num_bot_nodes = stoi(token_cmd[cmd_index++]);
        //                out_filename = token_cmd[cmd_index++];
        //
        //            }
        //
        //            // get graph
        //            GraphNew * graph = graph_man->get_graph(graphnum);
        //            if (graph == NULL) {
        //                cout << "No such graph" << endl;
        //                continue;
        //            }
        //
        //
        //            map<int, int> nid_to_degree_map_sorted;
        //            map<int, int> eid_to_end_nodes_degree_map_sorted;
        //
        //            string color_map_filename = out_filename + "_colorfile";
        //            ofstream color_fout(color_map_filename.c_str());
        //
        //            map<int, int> eids_to_colormap;
        //
        //            graph->get_nids_sorted_by_degree(nid_to_degree_map_sorted, num_bot_nodes, color_fout);
        //            //graph->get_eids_sorted_by_end_nodes_degrees(eid_to_end_nodes_degree_map_sorted);
        //
        //            graph->display_graph_richer_with_edge_color(color_map_filename, eids_to_colormap, graph_man, out_filename);
        //        }

        else if (command == "select_sg" || command == "sel")
        {
            int graphnum;
            string filename_with_rep_ids;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
                cin >> filename_with_rep_ids;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                filename_with_rep_ids = token_cmd[cmd_index++];
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
                cout << "No such graph" << endl;
            else
            {
                set<string> repids;
                graph_man->read_data_file_into_set_of_strings(graph, filename_with_rep_ids, repids);
                int new_subgraph_gid = graph_man->select_operation_on_graph(graphnum, repids);
                cout << "Resultant subgraph (with backloops) id: " << new_subgraph_gid << endl;
            }
        }

        else if (command == "help" || command == "h")
        {
            display_commands();
        }

        else if (command == "exit")
        {
            break;
        }

        else if (command == "get_assignments_z3" || command == "get_assg_z3")
        {
            string z3_result, output_assigns;
            cin >> z3_result >> output_assigns;
            cout << "\nNote: for this command to successfully work\n the mapping of internal variables in the hash maps should be the recent ones\n";
            read_z3_output(z3_result, output_assigns);
        }

        else if (command == "undir" || command == "u")
        {
            int graphnum;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[1]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum); // keep track of first node id -- subtract to get vector index -- through a macro

            // create undirected graph from graph
            int graphnum_undirected = graph_man->convert_to_undirected_graph(graphnum);
            cout << "New graph: " << graphnum_undirected << endl;
        }

        else if (command == "bcc")
        {
            int graphnum;
            cin >> graphnum;
            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);

            vector<int> orig_node_ids = graph->get_node_ids();
            graph->first_offset = orig_node_ids.front();

            // shuffle node seq numbers
            int num_of_nodes = orig_node_ids.size();
            int index = 0;
            vector<int>::iterator nid_itr;
            vector<int> node_ids;
            int counter1 = 0;
            for (nid_itr = orig_node_ids.begin(); nid_itr != orig_node_ids.end(); nid_itr++)
            {
                if (graph->is_isolated_node((*nid_itr)))
                {
                    int nseq = num_of_nodes - counter1;
                    counter1++;
                    graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, nseq));
                    graph->map_nseq_to_nid.insert(pair<int, int>(nseq, *nid_itr));
                }
                else
                {
                    node_ids.push_back(*nid_itr);
                }
            }

            vector<int> gomoryhu_parents;

            // std::random_shuffle(node_ids.begin(), node_ids.end());

            //            for (vector<int>::iterator temp_itr = node_ids.begin(); temp_itr != node_ids.end(); temp_itr++)
            //                cout << *temp_itr << endl;
            //            cout << endl;

            graph->sort_nids_by_degree(node_ids);

            //            for (vector<int>::iterator temp_itr = node_ids.begin(); temp_itr != node_ids.end(); temp_itr++)
            //                cout << *temp_itr << endl;
            //            cout << endl;

            for (index = 0, nid_itr = node_ids.begin(); nid_itr != node_ids.end(); index++, nid_itr++)
            {
                graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, index));
                graph->map_nseq_to_nid.insert(pair<int, int>(index, *nid_itr));

                gomoryhu_parents.push_back(0);
            }

            int count = graph_man->bcc(graph);
            cout << count << " biconnected components" << endl;
        }

        else if (command == "mincut" || command == "mc")
        {
            int graphnum;
            int cmd_index = 1;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *orig_graph = graph_man->get_graph(graphnum - 1);
            GraphNew *graph = graph_man->get_graph(graphnum);

            vector<int> node_ids;
            vector<int> gomoryhu_parents;
            graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

            vector<set<int>> cut_edges(node_ids.size());
            vector<set<int>> fwd_cut_edges(node_ids.size());
            vector<set<int>> bkd_cut_edges(node_ids.size());
            time(&time1);

            // int graphnum_undirected = graph_man->convert_to_undirected_graph(graphnum);

#ifdef DEBUG_FLAG
            debug_log << "Before calling compute_gh_tree" << endl;
#endif
            int new_gid = graph_man->compute_gh_tree(graphnum, node_ids, gomoryhu_parents, cut_edges);
            // int new_gid = graph_man->compute_gh_tree_2(graphnum, node_ids, gomoryhu_parents, cut_edges, fwd_cut_edges, bkd_cut_edges);
            cout << "Gomory-Hu tree has graph id: " << new_gid << endl;

            time(&time2);
            cout << "Total time taken (in sec): " << difftime(time2, time1) << endl;
#ifdef DEBUG_FLAG
            debug_log << "After returning from compute_gh_tree" << endl;
            debug_log << "Total time taken (in sec): " << difftime(time2, time1) << endl;
#endif

            // else
            // graphnum = stoi(token_cmd[cmd_index++]);

            // added by sukanya on 27 Aug 2016
            // generate all pair min cut file
            vector<int> nids1 = graph->get_node_ids();
            vector<int> nids2 = graph->get_node_ids();

            string up_reg_filename, down_reg_filename;
            if (!b_mode)
            {
                cout << "\nEnter file for up regulated entries: ";
                cin >> up_reg_filename;
                cout << "\nEnter file for down regulated entries: ";
                cin >> down_reg_filename;
            }
            else
            {
                up_reg_filename = token_cmd[cmd_index++];
                down_reg_filename = token_cmd[cmd_index++];
            }

            string id;
            ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
            set<string> up_reg_rep_ids, down_reg_rep_ids;

            if (!ifs_up.is_open())
            {
                cerr << "Error: file " + up_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
            }
            if (!ifs_down.is_open())
            {
                cerr << "Error: file " + down_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
            }
            while (ifs_up >> id)
            {
                string repid = graph->get_rep_id_from_id(id);
                if (repid != "")
                {
                    up_reg_rep_ids.insert(repid);
                }
            }
            ifs_up.close();
            while (ifs_down >> id)
            {
                string repid = graph->get_rep_id_from_id(id);
                if (repid != "")
                {
                    down_reg_rep_ids.insert(repid);
                }
            }
            ifs_down.close();

            string all_pair_file_name;
            if (!b_mode)
            {
                cout << "Enter all pair output file name: ";
                cin >> all_pair_file_name;
            }
            else
            {
                all_pair_file_name = token_cmd[cmd_index++];
            }

            time(&time1);
            auto start_time = std::chrono::high_resolution_clock::now();
            // get mincuts
            string source, target;
            int source_nid, target_nid;
            set<int> mincut_eids;
            map<string, int> nodes_in_all_mincuts;

            while (1)
            {
                if (!b_mode)
                {
                    cout << "Source: ";
                    cin >> source;
                    cout << endl;

                    cout << "Target: ";
                    cin >> target;
                    cout << endl;
                }
                else
                {
                    source = token_cmd[cmd_index++];
                    target = token_cmd[cmd_index++];
                }
                if (source == "-1" || target == "-1")
                    break;

                string rep = graph->get_rep_id_from_id(source);
                string source_name, target_name;
                source_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id(source));
                if (source_nid == -1)
                    continue;
                source_name = graph->get_all_display_ids_of_node(source_nid)[0];
                target_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id(target));
                if (target_nid == -1)
                    continue;
                target_name = graph->get_all_display_ids_of_node(target_nid)[0];
                int node_u_in_tree; // represents the u of the u-v cut in the GH-tree
                int is_src_side;
                ofstream outfile;
                ostringstream filename;
                filename << "temp";
                outfile.open(filename.str());
                // mincut_eids = graph_man->my_getMinCutEdges(graph, cut_edges, gomoryhu_parents, source_nid, target_nid);
                // mincut_eids = graph_man->getMinCutEdges(graph, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), node_u_in_tree, is_src_side, outfile);

                //                graph_man->get_min_cut_edges_from_ghtree(graph, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), mincut_eids);
                //                cout << "mincut_eids size: " << mincut_eids.size() << endl;
                mincut_eids = graph_man->getMinCutEdges_directed(graph, fwd_cut_edges, bkd_cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), node_u_in_tree, is_src_side, outfile);
                set<int> directed_mincut_eids;
                directed_mincut_eids = graph_man->getFilteredCutEdges(graph, mincut_eids, source_nid, target_nid);

                // sukanya's method for getting directed cut from undirected mincut
                // set<int> directed_cut_eids = mincut_eids;
                // graph_man->get_directed_cut(graph, source_nid, target_nid, mincut_eids, directed_cut_eids);
                ////graph_man->get_directed_cut2(orig_graph, graph, graph->get_other_value1_from_node(source_nid), graph->get_other_value1_from_node(target_nid), mincut_eids, directed_cut_eids);

                char response;
                if (!b_mode)
                {
                    cout << "Create subgraph (y/n): ";
                    cin >> response;
                }
                else
                    response = token_cmd[cmd_index++][0];

                if (response == 'y')
                {
                    int sg_id = graph_man->get_subgraph_edges_removed(graphnum - 1, mincut_eids);
                    cout << "New subgraph: " << sg_id << endl;
                    //                    if (sg_id != -1) {
                    //                        GraphNew* graph_ptr = graph_man->get_graph(sg_id);
                    //
                    //                        string filename;
                    //                        if (!b_mode) {
                    //                            cout << "Enter the file path to store the graph in xml format: ";
                    //                            cin >> filename;
                    //                        } else {
                    //                            filename = token_cmd[cmd_index++];
                    //                        }
                    //                        ofstream fout(filename.c_str());
                    //                        if (fout.is_open()) {
                    //                            graph_ptr->write_to_xml_file(fout, false, filename);
                    //                            fout.close();
                    //                        } else {
                    //                            cerr << "Error: couldn't open file " << filename << endl;
                    //                        }
                    //
                    //
                    //                    }
                }

                //                                            int node_u_in_tree;// represents the u of the u-v cut in the GH-tree
                //                                            int is_src_side;
                //                                            ofstream outfile;
                //                                            ostringstream filename;
                filename << static_cast<unsigned long>(::time(0)) << ".dat";
                outfile.open(filename.str());
                //////                set<int> curr_min_cut = graph_man->getMinCutEdges(graph, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), node_u_in_tree, is_src_side, outfile);
                set<int> curr_min_cut = graph_man->getMinCutEdges_directed(graph, fwd_cut_edges, bkd_cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), node_u_in_tree, is_src_side, outfile);
                // graph_man->get_min_cut_edges_from_ghtree(graph, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), mincut_eids);
                // cout << "Mincut edges: " << endl;

                ofstream hout(all_pair_file_name.c_str());
                map<string, int> nodes_in_mincut;
                hout << "<!DOCTYPE html>" << endl;
                hout << "<html>" << endl;
                hout << "<head>" << endl;
                hout << "<title>All pair min cuts</title>" << endl;
                hout << "</head>" << endl;
                // hout<<"<body style=\"background-color:powderblue;\">"<<endl;
                hout << "<body style=\"background-color:#FFFFF0;\">" << endl;

                hout << "<h2 style=\"color:red;text-align:center;font-family:courier;\">Min cuts</h2>" << endl;

                hout << "\n\n"
                     << endl;
                hout << "<p style=\"text-align:left;\">";
                hout << "<br> <br> To disconnect <b>" << source_name << "</b> ";
                hout << "from <b> " << target_name << "</b>";
                hout << "<br>remove all of the following edges <br>" << endl;
                hout << "<br>" << endl;

                set<string> written_edges;
                for (set<int>::iterator set_itr = directed_mincut_eids.begin(); set_itr != directed_mincut_eids.end(); set_itr++)
                {
                    graph->write_mincut_edge_to_html(*set_itr, hout, up_reg_rep_ids, down_reg_rep_ids, nodes_in_mincut, nodes_in_all_mincuts, written_edges);
                }

                hout << "</p>" << endl;

                hout << "<br>" << endl;
                hout << "</body>" << endl;
                hout << "</html>" << endl;
            }

            time(&time2);

            time_in_sec = difftime(time2, time1);

            cout << "TIME: " << time_in_sec << endl;
            auto end_time = std::chrono::high_resolution_clock::now();
            auto time = end_time - start_time;

            std::cout << "TIME: " << std::chrono::duration_cast<std::chrono::microseconds>(time).count() << endl;

            list<int> source_nodes_nid_list;
            string node_name, rep_id;
            int node_id;
            if (!b_mode)
            {
                cout << "Source node ids list (end with -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id == "")
                {
                    cout << "Not found " << node_name << endl;
                    // continue;
                }
                else
                {
                    node_id = graph->get_nid_from_rep_id(rep_id);

#ifdef ASSERT_FLAG
                    assert(node_id != -1);
#endif
                    source_nodes_nid_list.push_back(node_id);
                }
                if (!b_mode)
                    cin >> node_name;
                else
                    node_name = token_cmd[cmd_index++];
            }

            list<int> target_nodes_nid_list;
            if (!b_mode)
            {
                cout << "Target node list (end with -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id == "")
                {
                    cout << "Not found " << node_name << endl;
                    // continue;
                }
                else
                {
                    node_id = graph->get_nid_from_rep_id(rep_id);
#ifdef ASSERT_FLAG
                    assert(node_id != -1);
#endif
                    target_nodes_nid_list.push_back(node_id);
                }
                if (!b_mode)
                    cin >> node_name;
                else
                    node_name = token_cmd[cmd_index++];
            }

            set<int> present_edges;
            list<int>::iterator iter1, iter2;

            ofstream outfile;
            ostringstream filename;
            filename << static_cast<unsigned long>(::time(0)) << ".dat";
            outfile.open(filename.str());
            for (iter1 = source_nodes_nid_list.begin(); iter1 != source_nodes_nid_list.end(); iter1++)
            {
                for (iter2 = target_nodes_nid_list.begin(); iter2 != target_nodes_nid_list.end(); iter2++)
                {

                    // graph_man->connect(graph, present_edges, cut_edges, cut_edge_index, *iter1, *iter2); // some more things may be need to be output
                    graph_man->new_connect(graph, present_edges, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, *iter1), GET_NODE_SEQ_NUM(graph, *iter2), outfile);
                    //                                map<int, int> map_nid_to_dfs_index;
                    //                                graph_man->dfs_numbering(graph, *iter1, map_nid_to_dfs_index);
                    //                                graph_man->connect_with_dfs_numbering(graph, present_edges, fwd_cut_edges, bkd_cut_edges, gomoryhu_parents, *iter1, *iter2, map_nid_to_dfs_index, outfile);
                }
            }

            int subgraph = graph_man->get_subgraph_with_edge_ids(graph, present_edges);
            if (subgraph != -1)
            {
                cout << "New subgraph id: " << subgraph << endl;
            }
            else
            {
                cerr << "Subgraph not created" << endl;
            }
        }

        else if (command == "mincut1" || command == "mc1")
        {
            int graphnum;
            int cmd_index = 1;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);

            if (graph == NULL)
            {
                cout << "Invalid graph" << endl;
                if (b_mode)
                    exit(1);
            }

            GraphNew *ugraph = graph_man->get_graph(graph_man->convert_to_undirected_graph(graphnum));

            if (ugraph == NULL)
            {
                cout << "Invalid graph" << endl;
                if (b_mode)
                    exit(1);
            }

            vector<int> node_unids;
            vector<int> gomoryhu_parents;
            vector<bool> selected_mincut_vec;
            ugraph->preprocess_graph_for_mincut(node_unids, gomoryhu_parents);

            vector<set<int>> cut_edges(node_unids.size());

            time(&time1);

#ifdef DEBUG_FLAG
            debug_log << "Before calling compute_gh_tree" << endl;
#endif
            int new_gid = graph_man->compute_gh_tree(ugraph->get_graph_id(), node_unids, gomoryhu_parents, cut_edges);
            cout << "Gomory-Hu tree has graph id: " << new_gid << endl;

            time(&time2);
            cout << "Total time taken (in sec): " << difftime(time2, time1) << endl;

#ifdef DEBUG_FLAG
            debug_log << "After returning from compute_gh_tree" << endl;
            debug_log << "Total time taken (in sec): " << difftime(time2, time1) << endl;
#endif

            // added by sukanya on 27 Aug 2016
            // generate all pair min cut file
            vector<int> unids1 = ugraph->get_node_ids();
            vector<int> unids2 = ugraph->get_node_ids();

            string up_reg_filename, down_reg_filename;
            if (!b_mode)
            {
                cout << "\nEnter file for up regulated entries: ";
                cin >> up_reg_filename;
                cout << "\nEnter file for down regulated entries: ";
                cin >> down_reg_filename;
            }
            else
            {
                up_reg_filename = token_cmd[cmd_index++];
                down_reg_filename = token_cmd[cmd_index++];
            }

            string id;
            ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
            set<string> up_reg_rep_ids, down_reg_rep_ids;

            if (!ifs_up.is_open())
                cerr << "Error: file " + up_reg_filename + " cannot be opened." << endl;

            if (!ifs_down.is_open())
                cerr << "Error: file " + down_reg_filename + " cannot be opened." << endl;

            while (ifs_up >> id)
            {
                string repid = ugraph->get_rep_id_from_id(id);
                if (repid != "")
                    up_reg_rep_ids.insert(repid);
            }
            ifs_up.close();

            while (ifs_down >> id)
            {
                string repid = ugraph->get_rep_id_from_id(id);
                if (repid != "")
                    down_reg_rep_ids.insert(repid);
            }
            ifs_down.close();

            string all_pair_file_name;
            if (!b_mode)
            {
                cout << "Enter all pair output file name: ";
                cin >> all_pair_file_name;
            }
            else
            {
                all_pair_file_name = token_cmd[cmd_index++];
            }

            time(&time1);

            // get mincuts
            string source, target;
            int source_unid, target_unid;
            set<int> mincut_eids;
            map<string, int> nodes_in_all_mincuts;

            while (1)
            {
                if (!b_mode)
                {
                    cout << "Source: ";
                    cin >> source;
                    cout << endl;

                    cout << "Target: ";
                    cin >> target;
                    cout << endl;
                }
                else
                {
                    source = token_cmd[cmd_index++];
                    target = token_cmd[cmd_index++];
                }
                if (source == "-1" || target == "-1")
                    break;

                source_unid = ugraph->get_nid_from_rep_id(ugraph->get_rep_id_from_id(source));
                if (source_unid == -1)
                    continue;

                target_unid = ugraph->get_nid_from_rep_id(ugraph->get_rep_id_from_id(target));
                if (target_unid == -1)
                    continue;

                set<int> directed_cut_eids;

                // inconsistent call -- temporarily commented out
                // graph_man->get_min_cut_edges_from_ghtree(graph, ugraph, cut_edges, gomoryhu_parents, selected_mincut_vec, GET_NODE_SEQ_NUM(ugraph, source_unid), GET_NODE_SEQ_NUM(ugraph, target_unid), directed_cut_eids);

                assert(!directed_cut_eids.empty());

                ofstream hout(all_pair_file_name.c_str());
                map<string, int> nodes_in_mincut;
                hout << "<!DOCTYPE html>" << endl;
                hout << "<html>" << endl;
                hout << "<head>" << endl;
                hout << "<title>All pair min cuts</title>" << endl;
                hout << "</head>" << endl;
                hout << "<body style=\"background-color:#FFFFF0;\">" << endl;

                hout << "<h2 style=\"color:red;text-align:center;font-family:courier;\">Min cuts</h2>" << endl;

                hout << "\n\n"
                     << endl;
                hout << "<p style=\"text-align:left;\">";
                hout << "<br> <br> To disconnect <b>" << ugraph->get_all_display_ids_of_node(source_unid)[0] << "</b> ";
                hout << "from <b> " << ugraph->get_all_display_ids_of_node(target_unid)[0] << "</b>";
                hout << "<br>remove all of the following edges <br>" << endl;
                hout << "<br>" << endl;

                set<string> written_edges;
                for (set<int>::iterator set_itr = directed_cut_eids.begin(); set_itr != directed_cut_eids.end(); set_itr++)
                {
                    graph->write_mincut_edge_to_html(*set_itr, hout, up_reg_rep_ids, down_reg_rep_ids, nodes_in_mincut, nodes_in_all_mincuts, written_edges);
                }

                hout << "</p>" << endl;
                hout << "<br>" << endl;
                hout << "</body>" << endl;
                hout << "</html>" << endl;
            }

            time(&time2);

            time_in_sec = difftime(time2, time1);

            cout << "TIME: " << time_in_sec << endl;
        }

        else if (command == "mcd")
        {
            int graphnum;
            int cmd_index = 1;
            if (!b_mode)
                cin >> graphnum;
            else
                graphnum = stoi(token_cmd[cmd_index++]);

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);

            vector<int> node_ids;
            vector<int> gomoryhu_parents;
            graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

            vector<set<int>> cut_edges(node_ids.size());
            vector<set<int>> fwd_cut_edges(node_ids.size());
            vector<set<int>> bkd_cut_edges(node_ids.size());
            time(&time1);

#ifdef DEBUG_FLAG
            debug_log << "Before calling compute_gh_tree" << endl;
#endif
            // int new_gid = graph_man->compute_gh_tree(graphnum, gomoryhu_parents, cut_edges);
            int new_gid = graph_man->compute_gh_tree_dinics(graphnum, node_ids, gomoryhu_parents, cut_edges, fwd_cut_edges, bkd_cut_edges);

            cout << "Gomory-Hu tree has graph id: " << new_gid << endl;

#ifdef DEBUG_FLAG
            debug_log << "After returning from compute_gh_tree" << endl;
            //                        for (vector<int>::iterator vec_itr = node_ids.begin(); vec_itr != node_ids.end(); vec_itr++) {
            //                            debug_log << GET_NODE_SEQ_NUM(graph, *vec_itr) << " -- " << gomoryhu_parents[GET_NODE_SEQ_NUM(graph, *vec_itr)] << endl;
            //                        }
#endif
            time(&time2);
            cout << "Total time taken (in sec): " << difftime(time2, time1) << endl;
#ifdef DEBUG_FLAG
            debug_log << "Total time taken (in sec): " << difftime(time2, time1) << endl;
#endif

            // else
            // graphnum = stoi(token_cmd[cmd_index++]);

            // added by sukanya on 27 Aug 2016
            // generate all pair min cut file
            vector<int> nids1 = graph->get_node_ids();
            vector<int> nids2 = graph->get_node_ids();

            string up_reg_filename, down_reg_filename;
            if (!b_mode)
            {
                cout << "\nEnter file for up regulated entries: ";
                cin >> up_reg_filename;
                cout << "\nEnter file for down regulated entries: ";
                cin >> down_reg_filename;
            }
            else
            {
                up_reg_filename = token_cmd[cmd_index++];
                down_reg_filename = token_cmd[cmd_index++];
            }

            string id;
            ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
            set<string> up_reg_rep_ids, down_reg_rep_ids;

            if (!ifs_up.is_open())
            {
                cerr << "Error: file " + up_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
            }
            if (!ifs_down.is_open())
            {
                cerr << "Error: file " + down_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
            }
            while (ifs_up >> id)
            {
                string repid = graph->get_rep_id_from_id(id);
                if (repid != "")
                {
                    up_reg_rep_ids.insert(repid);
                }
            }
            ifs_up.close();
            while (ifs_down >> id)
            {
                string repid = graph->get_rep_id_from_id(id);
                if (repid != "")
                {
                    down_reg_rep_ids.insert(repid);
                }
            }
            ifs_down.close();

            string all_pair_file_name;
            if (!b_mode)
            {
                cout << "Enter all pair output file name: ";
                cin >> all_pair_file_name;
            }
            else
            {
                all_pair_file_name = token_cmd[cmd_index++];
            }
            // get mincuts
            string source, target;
            int source_nid, target_nid;
            set<int> mincut_eids;
            map<string, int> nodes_in_all_mincuts;

            while (1)
            {
                if (!b_mode)
                {
                    cout << "Source: ";
                    cin >> source;
                    cout << endl;

                    cout << "Target: ";
                    cin >> target;
                    cout << endl;
                }
                else
                {
                    source = token_cmd[cmd_index++];
                    target = token_cmd[cmd_index++];
                }
                if (source == "-1" || target == "-1")
                    break;

                string rep = graph->get_rep_id_from_id(source);
                string source_name, target_name;
                source_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id(source));
                if (source_nid == -1)
                    continue;
                source_name = graph->get_all_display_ids_of_node(source_nid)[0];
                target_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id(target));
                if (target_nid == -1)
                    continue;
                target_name = graph->get_all_display_ids_of_node(target_nid)[0];
                int node_u_in_tree; // represents the u of the u-v cut in the GH-tree
                int is_src_side;
                ofstream outfile;
                ostringstream filename;
                filename << static_cast<unsigned long>(::time(0)) << ".dat";
                outfile.open(filename.str());
                // mincut_eids = graph_man->my_getMinCutEdges(graph, cut_edges, gomoryhu_parents, source_nid, target_nid);
                mincut_eids = graph_man->getMinCutEdges(graph, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), node_u_in_tree, is_src_side, outfile);
                // mincut_eids = graph_man->getMinCutEdges_directed(graph, fwd_cut_edges, bkd_cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph,source_nid), GET_NODE_SEQ_NUM(graph,target_nid), node_u_in_tree, is_src_side, outfile);
                set<int> directed_mincut_eids;
                directed_mincut_eids = graph_man->getFilteredCutEdges(graph, mincut_eids, source_nid, target_nid);

                char response;
                if (!b_mode)
                {
                    cout << "Create subgraph (y/n): ";
                    cin >> response;
                }
                else
                    response = token_cmd[cmd_index++][0];

                if (response == 'y')
                {
                    int sg_id = graph_man->get_subgraph_edges_removed(graphnum, mincut_eids);
                    cout << "New subgraph: " << sg_id << endl;
                    //                    if (sg_id != -1) {
                    //                        GraphNew* graph_ptr = graph_man->get_graph(sg_id);
                    //
                    //                        string filename;
                    //                        if (!b_mode) {
                    //                            cout << "Enter the file path to store the graph in xml format: ";
                    //                            cin >> filename;
                    //                        } else {
                    //                            filename = token_cmd[cmd_index++];
                    //                        }
                    //                        ofstream fout(filename.c_str());
                    //                        if (fout.is_open()) {
                    //                            graph_ptr->write_to_xml_file(fout, false, filename);
                    //                            fout.close();
                    //                        } else {
                    //                            cerr << "Error: couldn't open file " << filename << endl;
                    //                        }
                    //
                    //
                    //                    }
                }

                ////                            int node_u_in_tree;// represents the u of the u-v cut in the GH-tree
                ////                            int is_src_side;
                ////                            ofstream outfile;
                ////                            ostringstream filename;
                ////                            filename << static_cast<unsigned long>(::time(0))<<".dat";
                ////                            outfile.open(filename.str());
                set<int> curr_min_cut = graph_man->getMinCutEdges(graph, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, source_nid), GET_NODE_SEQ_NUM(graph, target_nid), node_u_in_tree, is_src_side, outfile);
                // set<int> curr_min_cut = graph_man->getMinCutEdges_directed(graph, fwd_cut_edges, bkd_cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph,source_nid), GET_NODE_SEQ_NUM(graph,target_nid), node_u_in_tree, is_src_side, outfile);

                // cout << "Mincut edges: " << endl;

                ofstream hout(all_pair_file_name.c_str());
                map<string, int> nodes_in_mincut;
                hout << "<!DOCTYPE html>" << endl;
                hout << "<html>" << endl;
                hout << "<head>" << endl;
                hout << "<title>All pair min cuts</title>" << endl;
                hout << "</head>" << endl;
                // hout<<"<body style=\"background-color:powderblue;\">"<<endl;
                hout << "<body style=\"background-color:#FFFFF0;\">" << endl;

                hout << "<h2 style=\"color:red;text-align:center;font-family:courier;\">Min cuts</h2>" << endl;

                hout << "\n\n"
                     << endl;
                hout << "<p style=\"text-align:left;\">";
                hout << "<br> <br> To disconnect <b>" << source_name << "</b> ";
                hout << "from <b> " << target_name << "</b>";
                hout << "<br>remove all of the following edges <br>" << endl;
                hout << "<br>" << endl;

                set<string> written_edges;
                for (set<int>::iterator set_itr = directed_mincut_eids.begin(); set_itr != directed_mincut_eids.end(); set_itr++)
                {
                    graph->write_mincut_edge_to_html(*set_itr, hout, up_reg_rep_ids, down_reg_rep_ids, nodes_in_mincut, nodes_in_all_mincuts, written_edges);
                }
                hout << "</p>" << endl;

                hout << "<br>" << endl;
                hout << "</body>" << endl;
                hout << "</html>" << endl;
            }

            list<int> source_nodes_nid_list;
            string node_name, rep_id;
            int node_id;
            if (!b_mode)
            {
                cout << "Source node ids list (end with -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id == "")
                {
                    cout << "Not found " << node_name << endl;
                    // continue;
                }
                else
                {
                    node_id = graph->get_nid_from_rep_id(rep_id);

#ifdef ASSERT_FLAG
                    assert(node_id != -1);
#endif
                    source_nodes_nid_list.push_back(node_id);
                }
                if (!b_mode)
                    cin >> node_name;
                else
                    node_name = token_cmd[cmd_index++];
            }

            list<int> target_nodes_nid_list;
            if (!b_mode)
            {
                cout << "Target node list (end with -1): ";
                cin >> node_name;
            }
            else
            {
                node_name = token_cmd[cmd_index++];
            }
            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id == "")
                {
                    cout << "Not found " << node_name << endl;
                    // continue;
                }
                else
                {
                    node_id = graph->get_nid_from_rep_id(rep_id);
#ifdef ASSERT_FLAG
                    assert(node_id != -1);
#endif
                    target_nodes_nid_list.push_back(node_id);
                }
                if (!b_mode)
                    cin >> node_name;
                else
                    node_name = token_cmd[cmd_index++];
            }

            set<int> present_edges;
            list<int>::iterator iter1, iter2;

            ofstream outfile;
            ostringstream filename;
            filename << static_cast<unsigned long>(::time(0)) << ".dat";
            outfile.open(filename.str());
            for (iter1 = source_nodes_nid_list.begin(); iter1 != source_nodes_nid_list.end(); iter1++)
            {
                for (iter2 = target_nodes_nid_list.begin(); iter2 != target_nodes_nid_list.end(); iter2++)
                {

                    // graph_man->connect(graph, present_edges, cut_edges, cut_edge_index, *iter1, *iter2); // some more things may be need to be output
                    graph_man->new_connect(graph, present_edges, cut_edges, gomoryhu_parents, GET_NODE_SEQ_NUM(graph, *iter1), GET_NODE_SEQ_NUM(graph, *iter2), outfile);
                    //                                map<int, int> map_nid_to_dfs_index;
                    //                                graph_man->dfs_numbering(graph, *iter1, map_nid_to_dfs_index);
                    //                                graph_man->connect_with_dfs_numbering(graph, present_edges, fwd_cut_edges, bkd_cut_edges, gomoryhu_parents, *iter1, *iter2, map_nid_to_dfs_index, outfile);
                }
            }

            int subgraph = graph_man->get_subgraph_with_edge_ids(graph, present_edges);
            if (subgraph != -1)
            {
                cout << "New subgraph id: " << subgraph << endl;
            }
            else
            {
                cerr << "Subgraph not created" << endl;
            }
        }

        else if (command == "split_node" || command == "split")
        {
            int cmd_index = 1;
            int graphnum;
            string id;
            vector<string> id_set;

            if (!b_mode)
            {
                cin >> graphnum;
                cout << "Enter ids of nodes to split (end with -1): ";
                cin >> id;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                id = token_cmd[cmd_index++];
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);

            while (id != "-1")
            {
                id_set.push_back(id);
                if (!b_mode)
                    cin >> id;
                else
                    id = token_cmd[cmd_index++];
            }

            string filename;
            if (!b_mode)
            {
                cout << "Enter exception filename: ";
                cin >> filename;
            }
            else
            {
                filename = token_cmd[cmd_index++];
            }

            ofstream fout(filename.c_str());
            if (fout.is_open())
            {
                for (vector<string>::iterator itr = id_set.begin(); itr != id_set.end(); itr++)
                {
                    vector<string> all_ids = graph->get_all_ids_for_rep_id(graph->get_rep_id_from_id(*itr));
                    for (vector<string>::iterator vec_itr = all_ids.begin(); vec_itr != all_ids.end(); vec_itr++)
                    {
                        fout << *vec_itr << "\n";
                    }
                }
                fout.close();
            }
            else
            {
                cerr << "Error: couldn't open file " << filename << endl;
            }
        }

        //                else if  (command == "mincut" || command == "mc"){
        //                        int graphnum;
        //                        cin >> graphnum;
        //
        //                        GraphNew * graph = graph_man->get_graph(graphnum);
        //                        V = graph->get_node_ids().size();
        //                        E = graph->get_edge_ids().size();
        //                        cout << "V = " << V << " E = " << E << endl;
        //                        time(&time1);
        //                        vector< vector <int> > minCutValues(V, vector<int>(V, inf));
        //                        vector< vector <vector<int> > > minCutPointer(V, vector< vector <int> >(V, vector<int>(1,inf)));
        //                        vector< vector <set<int> > > minCutEdgeIds(V, vector< set <int> >(V, set<int>()));
        //                        vector< vector <vector<int> > > minCutEdgeIds2(V, vector< vector <int> >(V, vector<int>(0)));
        //                        myGHTree(graph, minCutPointer, minCutEdgeIds);
        //                        //GHTree(graph, minCutPointer);
        //                        time(&time2);
        //                        cout << "Computed min-cuts for Graph " << graphnum << endl;
        //                        cout << "Total time taken (in sec): " << difftime(time2, time1) << endl;
        //
        //                }
        //
        //                else if (command == "connect") {
        //                        int graphnum;
        //                        cin >> graphnum;
        //                        GraphNew * graph = graph_man->get_graph(graphnum);
        //                        string node_name, rep_id;
        //                        int node_id;
        //
        //                        // get source and target nodes
        //                        // get the corresponding node nids
        //			list<int> source_nodes_nid_list;
        //			cout << "Source node ids list (end with -1): ";
        //			cin >> node_name;
        //			while (node_name != "-1") {
        //                                rep_id = graph->get_rep_id_from_id(node_name);
        //                                if(rep_id == ""){
        //                                        cout << "Not found " << node_name << endl;
        //                                        continue;
        //                                }
        //                                else {
        //                                        node_id = graph->get_nid_from_rep_id(rep_id);
        // #ifdef ASSERT_FLAG
        //                                        assert(node_id != -1);
        // #endif
        //                                        source_nodes_nid_list.push_back(node_id);
        //                                }
        //
        //				cin >> node_name;
        //			}
        //
        //			list<int> target_nodes_nid_list;
        //			cout << "Target node list (end with -1): ";
        //			cin >> node_name;
        //                        while (node_name != "-1") {
        //                                rep_id = graph->get_rep_id_from_id(node_name);
        //                                if(rep_id == ""){
        //                                        cout << "Not found " << node_name << endl;
        //                                        continue;
        //                                }
        //                                else {
        //                                        node_id = graph->get_nid_from_rep_id(rep_id);
        // #ifdef ASSERT_FLAG
        //                                        assert(node_id != -1);
        // #endif
        //                                        target_nodes_nid_list.push_back(node_id);
        //                                }
        //				cin >> node_name;
        //			}
        //
        //                        // get min-cut matrix
        //                        V = graph->get_node_ids().size();
        //                        E = graph->get_edge_ids().size();
        //                        cout << "V = " << V << " E = " << E << endl;
        //                        //vector< vector <vector<int> > > minCutPointer(V, vector< vector<int> >(V, vector<int>(1)));
        //                        //vector< vector <vector<int> > > minCutPointer(V, vector< vector <pair<int, int> > >(V, vector<pair<int, int> >(1, make_pair(-2,-2))));
        //                        //vector< vector <vector<int> > > minCutEdgeIds(V, vector< vector <int> >(V, vector<int>(1)));
        //                        //getMinCutMatrix_dummy(graph, minCutMatrix); // to be replaced with GHTree method
        //                        time(&time1);
        //                        vector< vector <int> > minCutValues(V, vector<int>(V, inf));
        //                        vector< vector <vector<int> > > minCutPointer(V, vector< vector <int> >(V, vector<int>(1, inf)));
        //                        vector< vector <set<int> > > minCutEdgeIds(V, vector< set <int> >(V, set<int>()));
        //                        //myGHTree(graph, minCutPointer, minCutEdgeIds);
        //                        GHTree(graph, minCutPointer);
        //                        time(&time2);
        //                        cout << "Computed min-cuts for Graph " << graphnum << endl;
        //                        cout << "Total time taken (in sec): " << difftime(time2, time1) << endl;
        //
        //                        set<int> present_edges;
        //                        list<int>::iterator iter1, iter2;
        //
        //                        for(iter1 = source_nodes_nid_list.begin(); iter1 != source_nodes_nid_list.end(); iter1++){
        //                            for(iter2 = target_nodes_nid_list.begin(); iter2 != target_nodes_nid_list.end(); iter2++){
        //                                graph_man->connect(graph, present_edges, minCutEdgeIds, *iter1, *iter2);
        //                                //graph_man->connect(graph, present_edges, minCutPointer, *iter1, *iter2);
        //                            }
        //                        }
        //
        //
        ////                        int source_nid = source_nodes_nid_list.front();
        ////                        int target_nid = target_nodes_nid_list.front();
        //
        ////                        graph_man->connect(graph, present_edges, minCutMatrix, source_nid, target_nid);
        //
        //                        int subgraph = graph_man->get_subgraph_with_edge_ids(graph, present_edges);
        //                        if(subgraph != -1){
        //				cout << "New subgraph id: " << subgraph << endl;
        //			}
        //			else{
        //				cerr << "Subgraph not created" << endl;
        //			}
        //
        //                }

        else if (command == "matrix" || command == "mat")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            vector<int> orig_node_ids = graph->get_node_ids();
            graph->first_offset = orig_node_ids.front();

            // shuffle node seq numbers
            int num_of_nodes = orig_node_ids.size();
            int index = 0;
            vector<int>::iterator nid_itr;
            vector<int> node_ids;
            int counter1 = 0;
            for (nid_itr = orig_node_ids.begin(); nid_itr != orig_node_ids.end(); nid_itr++)
            {
                if (graph->is_isolated_node((*nid_itr)))
                {
                    int nseq = num_of_nodes - counter1;
                    counter1++;
                    graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, nseq));
                    graph->map_nseq_to_nid.insert(pair<int, int>(nseq, *nid_itr));
                }
                else
                {
                    node_ids.push_back(*nid_itr);
                }
            }

            for (index = 0, nid_itr = node_ids.begin(); nid_itr != node_ids.end(); index++, nid_itr++)
            {
                graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, index));
                graph->map_nseq_to_nid.insert(pair<int, int>(index, *nid_itr));
            }

            string outfilename = "matrix";
            ofstream fout(outfilename.c_str());

            if (fout.is_open())
            {
                vector<int> nids = node_ids; // graph->get_node_ids();
                vector<int> eids = graph->get_edge_ids();

                // nodes
                fout << "c(" << *nids.begin();
                for (auto nids_iter = nids.begin() + 1; nids_iter != nids.end(); nids_iter++)
                {
                    fout << "," << *nids_iter;
                }
                fout << ")" << endl;
                fout << endl;

                // edges
                int weight = 1;
                int curr_eid = *eids.begin();
                fout << "matrix (c(" << graph->get_source_node(curr_eid) << "," << graph->get_target_node(curr_eid) << "," << weight;
                for (auto eids_iter = eids.begin() + 1; eids_iter != eids.end(); eids_iter++)
                {
                    curr_eid = *eids_iter;
                    fout << ", "; // << endl;
                    fout << graph->get_source_node(curr_eid) << "," << graph->get_target_node(curr_eid) << "," << weight;
                }
                fout << "), ncol = 3, byrow = TRUE)" << endl;

                // nodes
                fout << *nids.begin() << " - " << graph->get_rep_id_from_nid(*nids.begin()) << endl;
                for (auto nids_iter = nids.begin() + 1; nids_iter != nids.end(); nids_iter++)
                {
                    fout << *nids_iter << " - " << graph->get_rep_id_from_nid(*nids_iter) << endl;
                }
                fout << endl;
            }
            fout.close();
        }

        else if (command == "partition" || command == "part")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            // convert to undirected graph
            int ugraphnum = graph_man->convert_to_undirected_graph(graphnum);
            GraphNew *ugraph = graph_man->get_graph(ugraphnum);

            vector<int> orig_node_ids = graph->get_node_ids();
            graph->first_offset = orig_node_ids.front();

            // shuffle node seq numbers
            int num_of_nodes = orig_node_ids.size();
            int index = 0;
            vector<int>::iterator nid_itr;
            vector<int> node_ids;
            int counter1 = 0;
            for (nid_itr = orig_node_ids.begin(); nid_itr != orig_node_ids.end(); nid_itr++)
            {
                if (graph->is_isolated_node((*nid_itr)))
                {
                    int nseq = num_of_nodes - counter1;
                    counter1++;
                    graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, nseq));
                    graph->map_nseq_to_nid.insert(pair<int, int>(nseq, *nid_itr));
                }
                else
                {
                    node_ids.push_back(*nid_itr);
                }
            }

            vector<int> gomoryhu_parents;
            std::random_shuffle(node_ids.begin(), node_ids.end());
            for (index = 0, nid_itr = node_ids.begin(); nid_itr != node_ids.end(); index++, nid_itr++)
            {
                graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, index));
                graph->map_nseq_to_nid.insert(pair<int, int>(index, *nid_itr));

                gomoryhu_parents.push_back(0);
            }
        }

        else if (command == "disconnect" || command == "dc")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);

            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            // convert to undirected graph
            int ugraphnum = graph_man->convert_to_undirected_graph(graphnum);
            GraphNew *ugraph = graph_man->get_graph(ugraphnum);

            vector<int> orig_node_ids = graph->get_node_ids();
            graph->first_offset = orig_node_ids.front();

            // shuffle node seq numbers
            int num_of_nodes = orig_node_ids.size();
            int index = 0;
            vector<int>::iterator nid_itr;
            vector<int> node_ids;
            int counter1 = 0;
            for (nid_itr = orig_node_ids.begin(); nid_itr != orig_node_ids.end(); nid_itr++)
            {
                if (graph->is_isolated_node((*nid_itr)))
                {
                    int nseq = num_of_nodes - counter1;
                    counter1++;
                    graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, nseq));
                    graph->map_nseq_to_nid.insert(pair<int, int>(nseq, *nid_itr));
                }
                else
                {
                    node_ids.push_back(*nid_itr);
                }
            }

            vector<int> gomoryhu_parents;
            std::random_shuffle(node_ids.begin(), node_ids.end());
            for (index = 0, nid_itr = node_ids.begin(); nid_itr != node_ids.end(); index++, nid_itr++)
            {
                graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, index));
                graph->map_nseq_to_nid.insert(pair<int, int>(index, *nid_itr));

                gomoryhu_parents.push_back(0);
            }

            // compute Gomory-Hu tree
            //                    vector<int> node_ids = graph->get_node_ids();
            //                    graph->first_offset = node_ids.front();
            //
            //                    int num_of_nodes = node_ids.size();
            //                    vector<int> gomoryhu_parents(node_ids.size());
            //                    for (vector<int>::iterator vec_itr = node_ids.begin(); vec_itr != node_ids.end(); vec_itr++) {
            //                        gomoryhu_parents[GET_NODE_SEQ_NUM(graph, *vec_itr)] = 0;
            //                    }

            vector<set<int>> cut_edges(node_ids.size());

            int new_gid = graph_man->compute_gh_tree(graphnum, node_ids, gomoryhu_parents, cut_edges);
            cout << "Gomory-Hu tree has graph id: " << new_gid << endl;

            set<int> present_edges;

            // get GH tree
            GraphNew *ghtree = graph_man->get_graph(new_gid);

            // get input nodes for sources, targets and nodes to be avoided
            string source_name;
            string node_name;
            int src_nid, node_nid;
            set<int> set_S; // contains nids in graph
            set<int> set_T; // contains nids in graph
            set<int> set_W; // contains nids in graph

            string rep_id;

            cout << "Source node: ";
            cin >> source_name;
            cout << endl;

            rep_id = graph->get_rep_id_from_id(source_name);
            if (rep_id == "")
            {
                cout << "Source not in graph" << endl;
                continue;
            }
            else
                src_nid = graph->get_nid_from_rep_id(rep_id);

            cout << "Nodes connected to source (end by -1): ";
            cin >> node_name;

            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id != "")
                    set_S.insert(graph->get_nid_from_rep_id(rep_id));
                cin >> node_name;
            }

            cout << "Nodes connected to target (end by -1): ";
            cin >> node_name;

            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id != "")
                    set_T.insert(graph->get_nid_from_rep_id(rep_id));
                cin >> node_name;
            }

            cout << "Nodes to be avoided (end by -1): ";
            cin >> node_name;

            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id != "")
                    set_W.insert(graph->get_nid_from_rep_id(rep_id));
                cin >> node_name;
            }

            // graph_man->disconnect(graph, ghtree, src_nid, set_S, set_T, set_W, present_edges, cut_edges, gomoryhu_parents);
        }

        else if (command == "disconnect2" || command == "dc2")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);

            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            //            // convert to undirected graph
            //            int ugraphnum = graph_man->convert_to_undirected_graph(graphnum);
            //            GraphNew * ugraph = graph_man->get_graph(ugraphnum);

            vector<int> orig_node_ids = graph->get_node_ids();
            graph->first_offset = orig_node_ids.front();

            // shuffle node seq numbers
            int num_of_nodes = orig_node_ids.size();
            int index = 0;
            vector<int>::iterator nid_itr;
            vector<int> node_ids;
            int counter1 = 0;
            for (nid_itr = orig_node_ids.begin(); nid_itr != orig_node_ids.end(); nid_itr++)
            {
                if (graph->is_isolated_node((*nid_itr)))
                {
                    int nseq = num_of_nodes - counter1;
                    counter1++;
                    graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, nseq));
                    graph->map_nseq_to_nid.insert(pair<int, int>(nseq, *nid_itr));
                }
                else
                {
                    node_ids.push_back(*nid_itr);
                }
            }

            vector<int> gomoryhu_parents;
            // std::random_shuffle(node_ids.begin(), node_ids.end());
            graph->sort_nids_by_degree(node_ids);
            for (index = 0, nid_itr = node_ids.begin(); nid_itr != node_ids.end(); index++, nid_itr++)
            {
                graph->map_nid_to_nseq.insert(pair<int, int>(*nid_itr, index));
                graph->map_nseq_to_nid.insert(pair<int, int>(index, *nid_itr));

                gomoryhu_parents.push_back(0);
            }

            vector<set<int>> cut_edges(node_ids.size());

            int new_gid = graph_man->compute_gh_tree(graphnum, node_ids, gomoryhu_parents, cut_edges);
            cout << "Gomory-Hu tree has graph id: " << new_gid << endl;

            set<int> present_edges;

            // get GH tree
            GraphNew *ghtree = graph_man->get_graph(new_gid);

            // create undirected GH tree for correct traversal
            vector<int> temp_eids;
            vector<int> gh_eids = ghtree->get_edge_ids();
            for (auto gh_eid_itr = gh_eids.begin(); gh_eid_itr != gh_eids.end(); gh_eid_itr++)
            {
                int new_eid = *gh_eid_itr;
                int rev_eid = ghtree->create_new_edge();
                if (rev_eid == -1)
                {
                    cerr << "Problem creating reverse edge" << endl;
                }
                temp_eids.push_back(rev_eid);
                ghtree->add_edge_id(rev_eid);
                graph_man->add_edge_id_graph_id(rev_eid, ghtree->get_graph_id());
                ghtree->add_edge_type(rev_eid, ghtree->get_edge_type(new_eid));
                ghtree->add_edge_to_outlist_of_node(ghtree->get_target_node(new_eid), rev_eid);
                ghtree->add_edge_to_inlist_of_node(ghtree->get_source_node(new_eid), rev_eid);
                ghtree->add_source_node(rev_eid, ghtree->get_target_node(new_eid));
                ghtree->add_target_node(rev_eid, ghtree->get_source_node(new_eid));
                ghtree->add_subtype_for_edge(rev_eid, "gh_edge");
                ghtree->add_subtype_for_edge(rev_eid, "dummy_d_to_u");
                vector<string> pathways = ghtree->get_all_pathways_for_edge(new_eid);
                for (auto path_itr = pathways.begin(); path_itr != pathways.end(); path_itr++)
                    ghtree->add_pathway_for_edge(rev_eid, *path_itr);
            }

            // get input nodes for sources, targets and nodes to be avoided
            string node_name;

            set<int> set_S; // contains nids in graph
            set<int> set_T; // contains nids in graph

            string rep_id;

            cout << "Nodes connected to source (end by -1): ";
            cin >> node_name;

            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id != "")
                    set_S.insert(graph->get_nid_from_rep_id(rep_id));
                cin >> node_name;
            }

            cout << "Nodes connected to target (end by -1): ";
            cin >> node_name;

            while (node_name != "-1")
            {
                rep_id = graph->get_rep_id_from_id(node_name);
                if (rep_id != "")
                    set_T.insert(graph->get_nid_from_rep_id(rep_id));
                cin >> node_name;
            }

            graph_man->disconnect2(graph, ghtree, set_S, set_T, present_edges, cut_edges, gomoryhu_parents);

            for (auto eid_iter = temp_eids.begin(); eid_iter != temp_eids.end(); eid_iter++)
            {
                ghtree->remove_edge(*eid_iter);
            }
        }

        // written by uddipaan

        else if (command == "convert" || command == "cnv")
        { // reading the command

            string log_fold_change_filename;
            int graphnum;
            int ch;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum; // input graph number
                cin >> log_fold_change_filename;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                log_fold_change_filename = token_cmd[cmd_index++];
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            if (!b_mode)
            {
                cout << "Choose the desired format: " << endl;
                cout << "\n1.Simple interaction format(.sif) \n2.GraphML(.graphml) \n3.GML(.gml)" << endl;
                cin >> ch;
            }
            else
            {
                ch = stoi(token_cmd[cmd_index]);
                cmd_index++;
            }
            switch (ch)
            {

            case 1:
            {
                GraphNew *graph = graph_man->get_graph(graphnum);
                if (graph != NULL)
                {
                    graph->convert_to_sif(graph_man, b_mode, token_cmd, cmd_index); // converted to sif
                }
                cout << "Conversion Complete!!" << endl;
            }
            break;
            case 2:
            {
                GraphNew *graph = graph_man->get_graph(graphnum);
                if (graph != NULL)
                {
                    std::string file_name;
                    if (!b_mode)
                    {
                        std::cout << "\nEnter a file path along with name for the graph with .graphml extension (e.g. ../foldername/mygraph.graphml): ";
                        std::cin >> file_name;
                    }
                    else
                    {
                        file_name = token_cmd[cmd_index];
                        cmd_index++;
                    }
                    // std::string pth = "/home/sysbio";
                    // creating the file stream
                    ofstream fout((file_name).c_str());
                    if (fout.is_open())
                    {

                        graph->convert_to_graphml_new(graph_man, fout, log_fold_change_filename, b_mode, token_cmd, cmd_index);
                    }
                    fout.close();
                }
                break;
            }

                // jk
            case 3:
            {
                GraphNew *graph = graph_man->get_graph(graphnum);
                if (graph != NULL)
                {
                    std::string file_name;
                    /*std::cout << "\nEnter a file name for the graph with .gml extension (e.g. mygraph.gml): ";
                      std::cin >> sif_name;
                      std::string pth = "/home/sysbio/uddipaan/network_tool/cnv";
                     */
                    std::cout << "\nEnter a file path along with name for the graph with .gml extension (e.g. ../foldername/mygraph.gml): ";
                    std::cin >> file_name;

                    // creating the file stream
                    ofstream fout((file_name).c_str());

                    if (fout.is_open())
                    {

                        graph->convert_to_gml(fout);
                    }
                    fout.close();
                }

                break;
            }
            }
        }

        else if (command == "domination" || command == "dom")
        {
            int graphnum;
            cin >> graphnum;

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph" << endl;
                continue;
            }

            int num_of_nodes = graph->get_node_ids().size();
#ifdef DEBUG_FLAG
            debug_log << "--------------------------------" << endl;
            debug_log << "Dominating factor calculations" << endl;
            debug_log << "Num of nodes = " << num_of_nodes << endl;
#endif
            // get dominating factor for every node
            vector<int> nids = graph->get_node_ids();
            for (vector<int>::iterator vec_itr = nids.begin(); vec_itr != nids.end(); vec_itr++)
            {
                int curr_nid = *vec_itr;
                // get [x] for every x in nids
                // in our case, it is the degree
                // for now, also considering dummy edges to calculate degree
                int degree = graph->get_inlist(curr_nid).size() + graph->get_outlist(curr_nid).size();
                // degree = degree/2;
                long network_degree = degree / 2;
                int dom_factor = (ceil(network_degree) == network_degree) ? (network_degree) + 1 : ceil(network_degree);
#ifdef DEBUG_FLAG
                debug_log << "nid = " << curr_nid << " degree = " << degree << " dom factor = " << dom_factor << endl;
#endif
            }
        }
        else if (command == "cbc" || command == "cal_betweenness_centrality")
        {
            int graphnum;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
            }

            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph!!!";
                continue;
            }

#ifdef DEBUG_FLAG
            debug_log << "--------------------------------" << endl;
            debug_log << "Betweenness centrality calculations" << endl;
#endif
            vector<int> node_ids;
            vector<int> gomoryhu_parents;
            graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

            map<pair<int, int>, double> CB_edge;
            graph_man->compute_Betweenness_centrality(graph, CB_edge);
        }
        else if (command == "ford")
        {
            int graphnum, src, target;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;
                cout << "provide the source id: ";
                cin >> src;
                cout << "provide the target id: ";
                cin >> target;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                src = stoi(token_cmd[cmd_index++]);
                target = stoi(token_cmd[cmd_index++]);
            }
            // preprocess the graph
            GraphNew *graph = graph_man->get_graph(graphnum);
            if (graph == NULL)
            {
                cout << "No such graph!!!";
                continue;
            }

#ifdef DEBUG_FLAG
            debug_log << "--------------------------------" << endl;
            debug_log << "Balanced partition calculations" << endl;
#endif
            vector<int> node_ids;
            vector<int> gomoryhu_parents;
            graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);

            // calculate max flow
            set<int> cut_edges, set_s;
            GraphNew *res_graph;
            map<int, int> map_nseq_to_res_graph_nid;
            map<string, long int> map_src_tgt_to_eid_res;
            map<string, long int> map_src_tgt_to_eid_org;
            int start_node_id = node_ids[0];
            // populate the map_src_tgt_to_eid_org
            // graph_man->populate_map_src_tgt_to_eid(graph, map_src_tgt_to_eid_org, start_node_id);
            // int current_size = graph_man->mincut_ford(graph, src, target, node_ids.size(), cut_edges, set_s);
            // cout<<current_size;
            graph_man->mincut_excess_scaling(graph, src, target, node_ids.size(), cut_edges, set_s, 1);
        }
        else if (command == "cbp")
        {
            map<string, int> nodes_in_all_mincuts; // added by sukanya on 16 May 2017
            string source_str, target_str;
            int src_nid, target_nid;
            int graphnum, src_seq, target_seq;
            int cmd_index = 1;
            if (!b_mode)
            {
                cin >> graphnum;

                cout << "Source(e.g. hsa100):";
                cin >> source_str;
                cout << endl;

                cout << "Target(e.g. hsa200): ";
                cin >> target_str;
                cout << endl;
            }
            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                source_str = token_cmd[cmd_index++];
                target_str = token_cmd[cmd_index++];

                // src = stoi(token_cmd[cmd_index++]);
                // target = stoi(token_cmd[cmd_index++]);
            }
            // Get the graph
            GraphNew *graph = graph_man->get_graph(graphnum);
            string source_name, target_name;

            if (graph == NULL)
            {
                cout << "No such graph!!!";
                continue;
            }

#ifdef DEBUG_FLAG
            debug_log << "--------------------------------" << endl;
            debug_log << "Balanced partition calculations" << endl;
#endif
            vector<int> node_ids;
            vector<int> gomoryhu_parents;

            // preprocess the graph
            graph->preprocess_graph_for_mincut(node_ids, gomoryhu_parents);
            const int num_of_nodes = node_ids.size();
            const int arrsize = num_of_nodes * num_of_nodes;
            // calculate max flow
            set<int> cut_edges, set_s;
            GraphNew *res_graph;
            map<int, int> map_nseq_to_res_graph_nid;
            map<string, long int> map_src_tgt_to_eid_res;
            map<string, long int> map_src_tgt_to_eid_org;
            int start_node_id = node_ids[0];
            vector<bool> path_flag_dyn(num_of_nodes);
            vector<bool> path_flag(num_of_nodes);
            map<int, int> eid_to_path_map_dyn;
            map<int, int> eid_to_path_map;
            src_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id(source_str));
            src_seq = GET_NODE_SEQ_NUM(graph, src_nid);
            source_name = graph->get_all_display_ids_of_node(src_nid)[0];
            target_nid = graph->get_nid_from_rep_id(graph->get_rep_id_from_id(target_str));
            target_seq = GET_NODE_SEQ_NUM(graph, target_nid);
            target_name = graph->get_all_display_ids_of_node(target_nid)[0];

            // populate the map_src_tgt_to_eid_org
            graph_man->populate_map_src_tgt_to_eid(graph, map_src_tgt_to_eid_org, start_node_id);
            int current_size = graph_man->mincut_ford_extended(graph, &res_graph, src_seq, target_seq, node_ids.size(), set_s, map_nseq_to_res_graph_nid, map_src_tgt_to_eid_res, path_flag, eid_to_path_map);
            // calculate Betweenness_centrality
            map<pair<int, int>, double> CB_edge;
            graph_man->compute_Betweenness_centrality(graph, CB_edge);
            // collect the CB_edges in a struct and sort them
            struct a
            {
                double num1;
                pair<int, int> num2;
            };

            vector<a> sorted_edge;
            // assign the elements in the map to the struct
            int i = 0;
            for (map<pair<int, int>, double>::iterator itr = CB_edge.begin(); itr != CB_edge.end(); itr++)
            {
                sorted_edge.push_back(a());
                sorted_edge[i].num1 = itr->second;
                sorted_edge[i++].num2 = itr->first;
            }
            // sort the array of struct
            // sort(array, array+i, array[0]);
            sort(sorted_edge.begin(), sorted_edge.end(), [](a lhs, a rhs)
                 { return lhs.num1 > rhs.num1; });
            int next_max_index = 0;
            vector<pair<int, int>> balanced_cut_edges;
            bool flag = true;
            while (flag)
            {
                cut_edges.clear();
                // get the maximum weighted edge
                int src_node_org = get<0>(sorted_edge[next_max_index].num2);
                int target_node_org = get<1>(sorted_edge[next_max_index++].num2);
                // get the edge from it
                int eid = graph_man->get_node_from_src_tgt(src_node_org, target_node_org, map_src_tgt_to_eid_org);
                // get the path number
                // cout<<eid<<"->";
                int path_no = eid_to_path_map[eid]; // path_no == 0 means the eid is not assigned to any path
                // If the edge is not belongs to any of the class of path leave it
                //---//For the path, we use a path_flag(boolean) vector
                // If the edge belongs to any class then check deleting this edge reduce mincut value -- no - non-delete the edge, yes - take this edge
                // If the path exists
                if (path_flag[path_no])
                {
                    graph->add_subtype_for_edge(eid, "deleted");
                    int mincut_value = graph_man->mincut_ford_extended(graph, &res_graph, src_seq, target_seq, node_ids.size(), set_s, map_nseq_to_res_graph_nid, map_src_tgt_to_eid_res, path_flag_dyn, eid_to_path_map_dyn);
                    if (mincut_value == 0)
                    {
                        balanced_cut_edges.push_back(make_pair(src_node_org, target_node_org));
                        flag = false;
                    }
                    else if (current_size > mincut_value)
                    {
                        balanced_cut_edges.push_back(make_pair(src_node_org, target_node_org));
                        current_size = mincut_value;
                        path_flag[path_no] = false; // to skip all the edges that belongs to the same path
                    }
                    else
                        graph->add_subtype_for_edge(eid, "non-deleted");
                    // continue;
                }
            }
            cout << "Node id pairs" << endl;
            for (vector<pair<int, int>>::iterator itr = balanced_cut_edges.begin(); itr != balanced_cut_edges.end(); itr++)
                cout << itr->first << "-" << itr->second << "(" << graph_man->get_node_from_src_tgt(itr->first, itr->second, map_src_tgt_to_eid_org) << ")" << endl;
            // writing to the html file
            string up_reg_filename, down_reg_filename;
            if (!b_mode)
            {
                cout << "\nEnter file for up regulated entries: ";
                cin >> up_reg_filename;
                cout << "\nEnter file for down regulated entries: ";
                cin >> down_reg_filename;
            }
            else
            {
                up_reg_filename = token_cmd[cmd_index++];
                down_reg_filename = token_cmd[cmd_index++];
            }

            string id;
            ifstream ifs_up(up_reg_filename.c_str()), ifs_down(down_reg_filename.c_str());
            set<string> up_reg_rep_ids, down_reg_rep_ids;

            if (!ifs_up.is_open())
            {
                cerr << "Error: file " + up_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
            }
            if (!ifs_down.is_open())
            {
                cerr << "Error: file " + down_reg_filename + " can't be opened, returning without any microarray assertions..." << endl;
            }
            while (ifs_up >> id)
            {
                string repid = graph->get_rep_id_from_id(id);
                if (repid != "")
                {
                    up_reg_rep_ids.insert(repid);
                }
            }
            ifs_up.close();
            while (ifs_down >> id)
            {
                string repid = graph->get_rep_id_from_id(id);
                if (repid != "")
                {
                    down_reg_rep_ids.insert(repid);
                }
            }
            ifs_down.close();

            string all_pair_file_name;
            if (!b_mode)
            {
                cout << "Enter all pair output file name: ";
                cin >> all_pair_file_name;
            }
            else
            {
                all_pair_file_name = token_cmd[cmd_index++];
            }

            // cout<<source_nid<<"-->"<<target_nid;*/
            ofstream hout(all_pair_file_name.c_str());
            map<string, int> nodes_in_mincut;

            hout << "<!DOCTYPE html>" << endl;
            hout << "<html>" << endl;
            hout << "<head>" << endl;
            hout << "<title>All pair min cuts</title>" << endl;
            hout << "</head>" << endl;
            // hout<<"<body style=\"background-color:powderblue;\">"<<endl;
            hout << "<body style=\"background-color:#FFFFF0;\">" << endl;

            hout << "<h2 style=\"color:red;text-align:center;font-family:courier;\">Min cuts</h2>" << endl;

            hout << "\n\n"
                 << endl;
            hout << "<p style=\"text-align:left;\">";
            hout << "<br> <br> To disconnect <b>" << source_name << "</b> ";
            hout << "from <b> " << target_name << "</b>";
            hout << "<br>remove all of the following edges <br>" << endl;
            hout << "<br>" << endl;

            set<string> written_edges;
            for (vector<pair<int, int>>::iterator itr = balanced_cut_edges.begin(); itr != balanced_cut_edges.end(); itr++)
                graph->write_mincut_edge_to_html(graph_man->get_node_from_src_tgt(itr->first, itr->second, map_src_tgt_to_eid_org), hout, up_reg_rep_ids, down_reg_rep_ids, nodes_in_mincut, nodes_in_all_mincuts, written_edges);

            hout << "</p>" << endl;

            hout << "<br>" << endl;
            hout << "</body>" << endl;
            hout << "</html>" << endl;
        }

        else if (command == "unroll")
        {
            int graphnum;
            string source_id;
            string target_id;
            int levels;

            int cmd_index = 1;

            if (!b_mode)
            {
                cin >> graphnum;
                cout << "\nEnter source node hsa-id: ";
                cin >> source_id;
                cout << "\nEnter target node hsa-id: ";
                cin >> target_id;
                cout << "\nEnter levels of unrolling: ";
                cin >> levels;
            }

            else
            {
                graphnum = stoi(token_cmd[cmd_index++]);
                source_id = token_cmd[cmd_index++];
                target_id = token_cmd[cmd_index++];
                levels = stoi(token_cmd[cmd_index++]);
            }

            if (graphnum == 0)
                graphnum = graph_man->graph_id_count;

            GraphNew *graph = graph_man->get_graph(graphnum);
            string source_repid;
            string target_repid;
            int source_nid;
            int target_nid;
            if (graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {
                source_repid = graph->get_rep_id_from_id(source_id);
                if (source_repid == "")
                {
                    cerr << "No valid source rep-id in graph" << endl;
                }
                source_nid = graph->get_nid_from_rep_id(source_repid);
                if (source_nid == -1)
                {
                    cerr << "No valid source nid in graph" << endl;
                }

                target_repid = graph->get_rep_id_from_id(target_id);
                if (target_repid == "")
                {
                    cerr << "No valid target rep-id in graph" << endl;
                }
                target_nid = graph->get_nid_from_rep_id(target_repid);
                if (target_nid == -1)
                {
                    cerr << "No valid target nid in graph" << endl;
                }

                int unrolled_gid = graph_man->compute_unrolled_DAG(graphnum, source_nid, target_nid, levels, map_nid_to_unrolled_graph_nids, map_eid_to_unrolled_graph_eids);
                if (unrolled_gid == -1)
                {
                    cerr << "Invalid unrolled graph" << endl;
                }
                else
                {
                    cout << "Unrolled graph id: " << unrolled_gid << endl;
                }
            }
        }

        else if (command == "frank")
        {
            int original_graphnum;
            int unrolled_graphnum;
            string file_prefix;
            string source_id;
            string target_id;
            int path_length;

            int cmd_index = 1;

            if (!b_mode)
            {
                cout << "\nEnter original graph number: ";
                cin >> original_graphnum;
                cout << "\nEnter unrolled graph number: ";
                cin >> unrolled_graphnum;
                cout << "\nEnter file prefix: ";
                cin >> file_prefix;
                cout << "\nEnter source node hsa-id: ";
                cin >> source_id;
                cout << "\nEnter target node hsa-id: ";
                cin >> target_id;
                cout << "\nEnter maximum path length: ";
                cin >> path_length;
            }

            else
            {
                original_graphnum = stoi(token_cmd[cmd_index++]);
                unrolled_graphnum = stoi(token_cmd[cmd_index++]);
                file_prefix = token_cmd[cmd_index++];
                source_id = token_cmd[cmd_index++];
                target_id = token_cmd[cmd_index++];
                path_length = stoi(token_cmd[cmd_index++]);
            }

            GraphNew *original_graph = graph_man->get_graph(original_graphnum);
            GraphNew *unrolled_graph = graph_man->get_graph(unrolled_graphnum);
            string source_repid;
            string target_repid;
            int source_nid;
            int target_nid;
            if (unrolled_graph == NULL)
            {
                cerr << "No such graph" << endl;
            }
            else
            {
                // source_repid = graph->get_rep_id_from_id(source_id);
                source_repid = source_id;
                if (source_repid == "")
                {
                    cerr << "No valid source rep-id in graph" << endl;
                }
                source_nid = original_graph->get_nid_from_rep_id(source_repid);
                if (source_nid == -1)
                {
                    cerr << "No valid source nid in graph" << endl;
                }

                // target_repid = graph->get_rep_id_from_id(target_id);
                target_repid = target_id;
                if (target_repid == "")
                {
                    cerr << "No valid target rep-id in graph" << endl;
                }
                target_nid = original_graph->get_nid_from_rep_id(target_repid);
                if (target_nid == -1)
                {
                    cerr << "No valid target nid in graph" << endl;
                }

                int num_of_nodes = unrolled_graph->get_node_ids().size();
                int num_of_edges = unrolled_graph->get_edge_ids().size();

                vector<vector<int>> PathsVertices(num_of_nodes, vector<int>(path_length + 1, 0));
                vector<vector<int>> PathsEdges(num_of_edges, vector<int>(path_length + 1, 0));

                unrolled_graph->preprocess_graph_for_node_edge_seq_num();
                original_graph->preprocess_graph_for_node_edge_seq_num();

                string orig_src_repid = original_graph->get_rep_id_from_nid(source_nid);
                string orig_tgt_repid = original_graph->get_rep_id_from_nid(target_nid);

                string unrolled_src_repid = orig_src_repid + "_0";
                string unrolled_tgt_repid = orig_tgt_repid;

                int unrolled_src_nid = unrolled_graph->get_nid_from_rep_id(unrolled_src_repid);
                if (unrolled_src_nid == -1)
                {
                    cerr << "Source node not found in unrolled graph" << endl;
                }

                int unrolled_tgt_nid = unrolled_graph->get_nid_from_rep_id(unrolled_tgt_repid);
                if (unrolled_tgt_nid == -1)
                {
                    cerr << "Target node not found in unrolled graph" << endl;
                }

                graph_man->count_paths_through_vertex(unrolled_graph, unrolled_src_nid, unrolled_tgt_nid, path_length, PathsVertices, PathsEdges);

                int total_paths = 0;
                graph_man->print_nodes_ranked_by_fraction_of_paths(file_prefix, original_graph, unrolled_graph, source_nid, target_nid, path_length, total_paths, PathsVertices, PathsEdges, map_nid_to_unrolled_graph_nids);
                graph_man->print_edges_ranked_by_fraction_of_paths(file_prefix, original_graph, unrolled_graph, source_nid, target_nid, path_length, total_paths, PathsVertices, PathsEdges, map_eid_to_unrolled_graph_eids);
            }
        }

        else
        {
            cout << "Did you enter one of the following valid commands?" << endl;
            display_commands();
            continue;
        }
    }

    debug_log.close();
}
