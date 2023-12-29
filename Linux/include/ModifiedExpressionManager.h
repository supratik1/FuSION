/*
 * File:   ModifiedExpressionManager.h
 * Author: sukanya
 *
 * This file contains methods copied from ExpressionManager module that have been modified for use with this project
 *
 *
 * Created on 25 November, 2016, 3:38 AM
 */

#ifndef MODIFIEDEXPRESSIONMANAGER_H
#define MODIFIEDEXPRESSIONMANAGER_H

#include "ExpressionManager.h"
#include "helping_functions.h"

#include <vector>

class ModifiedExpressionManager
{

public:
    ModifiedExpressionManager();
    ~ModifiedExpressionManager();

private:
    map<string, string> &copy_generateMap(istream &input, map<string, string> &operatorMap, char fieldSeparator = '=', char entrySeparator = '\n');

public:
    inline bool isVisited(t_Expression *expr); /* {
         return expr != NULL && expr->m_dagExpression->getDag()->isNodeVisited();
     }*/

    inline void setVisited(t_Expression *expr, bool value = true); /* {
     if (expr != NULL)
         expr->m_dagExpression->getDag()->assignVisitedFlag(value);
 }*/
    void mod_clearPrintingSets(t_ExpressionManager *em);
    bool mod_printArrayHeader(t_ExpressionManager *em, ostream &out, t_Expression *expr, const string &prefix);
    ostream &mod_printDagHeader(t_ExpressionManager *em, ostream &out, t_Expression *expr, const string &prefix);
    ostream &mod_printExpression(t_ExpressionManager *em, ostream &output, ostream &declarations, t_Expression *expr, map<string, string> &operatorMap, bool printNodeId = true);

    template <class T>
    void mod_printExpressionsToFileInSMT(t_ExpressionManager *em, const T &exprs, const string &exprSeparator, const string &smtFilename, bool negateAll = false, const string &declFilename = "", const string &constraintsFilename = "");

    template <typename T1, typename T2>
    void mod_printExpressionsToFileInSMT(t_ExpressionManager *em, const T1 &exprs, const T2 &inpConstr, const string &exprSeparator, bool negateAll, bool negateConstraints, bool negateExpression, const string &final, const string &decl, const string &cons);
};

#endif /* MODIFIEDEXPRESSIONMANAGER_H */
