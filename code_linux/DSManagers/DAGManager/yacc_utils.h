#ifndef DAG_READER_H
#define DAG_READER_H
#include<vector>

#include "DagManager.h"

using namespace std;





class t_VectorOfExpressions
{
 public:
  vector<t_DAGNode *> v_expressions;
  t_VectorOfExpressions(vector<t_DAGNode* > v_expressions_to_push)
    {
      for(int i=0;i<v_expressions_to_push.size();i++)
        {
          v_expressions.push_back(v_expressions_to_push[i]);
        }
    }
  t_VectorOfExpressions() //A do nothing constructor..
    {
    }

};


vector<t_DAGNode *> assign_expressions(class t_VectorOfExpressions *v_exprs_inst);




#endif
