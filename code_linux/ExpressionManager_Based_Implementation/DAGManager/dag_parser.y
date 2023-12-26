%{
#include "DagManager.h"
#include <string>
#include "yacc_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  using namespace std;
  int yylex(void);
  void yyerror(char *);
  int dag_parserlex(void);
  void dag_parsererror(char *);

  
  t_DAGManager dm(true, false);
  t_DAGNode *root;
%}





/*This is the union used to define YYSTYPE*/
%union{
  char  *string_value;
  int   int_value;
  class t_DAGNode *dagnode;
  class t_VectorOfExpressions *vector_of_expressions;

}

%type <dagnode> Symbol_Or_Literal
%type <dagnode> Expr
%type <vector_of_expressions> Operand_Expressions_List
//%type <vector_of_expressions> Expr_Vector

%token <string_value> t_String
%type <string_value> Operator





%token  t_OBrack t_CBrack t_Comma t_Eq





%%

DAGFile : DAGExpressionsList 
         {
	   cout<<"Read the DAGFile successfully\n";
	 }
DAGExpressionsList : DAGExpressionsList  DAGExpression | ;

DAGExpression : t_String t_Eq Expr
         {
	   cout<<"Matched t_String in DAGExperssion\n";
	   /* LHS is the node already created in the DAG
	      RHS is the expression of this node*/
	   if(strcmp($1, "root") ==0 )
	     {
	       cout<<"Found the root\n";
	       root = $3;
	     }
	   else
	     {
	       t_DAGNode *node = dm.createNode($1);
	       bool replace_result = dm.ReplaceLeafByRootOfADAG(node, $3);
	       if(!replace_result)
		 {
		   cerr<<"Could not replace \n";
		   exit(0);
		 }
	     }
	   cout<<"Read an expression string successfully\n";	   
         }



/**create tokens for all operators that are to be added */
Expr :  Operator t_OBrack Operand_Expressions_List t_CBrack
	{
	  t_VectorOfExpressions *v=$3;

	  vector<t_DAGNode *> operands;
	  for(int i=0;i<v->v_expressions.size();i++)
	    {
	      operands.push_back(v->v_expressions[i]);
	    }
	  t_DAGNode *operator_node=dm.createNode($1, operands);
	  cout<<"Matching a Expr\n";
	  $$=(t_DAGNode *)operator_node;
	    
	}
        |
       Symbol_Or_Literal
	{
	  cout<<"Matching a Expr\n";
	  $$=$1;
	}




       /*Operand_Expressions_List is the list of (comma seperated) optional operands of an operator. Do not confuse with Expr_Vector*/
Operand_Expressions_List : Operand_Expressions_List t_Comma Expr
	{
	vector<t_DAGNode *> v;
        v=assign_expressions($1);
        v.push_back($3);
        class t_VectorOfExpressions *s1=new t_VectorOfExpressions(v);
	cout<<"Matching a Operand_Experssion_List\n";
        $$=s1;
	}
	  | 
	Expr 
	{
	vector<t_DAGNode *> v;
        v.push_back($1);
	class t_VectorOfExpressions *s1=new t_VectorOfExpressions(v);
	cout<<"Matching a Operand_Experssion_List\n";
        $$=s1;
	}



Symbol_Or_Literal:
        t_String
        {
	  cout<<"Found a symbol or literal "<<$1<<endl;
	  t_DAGNode *node;
	  node = dm.createNode($1);
	  if(node==NULL)
	    cout<<"ERROR !! Found a null node \n";
	  $$ = node;
        }
        

/**

Expr_Vector: 
           Expr_Vector Expr 
	   {
	     vector<t_DAGNode *> v;
	     v=assign_expressions($1);
	     v.push_back($2);
	     class t_VectorOfExpressions *s1=new t_VectorOfExpressions(v);
	     cout<<"Matching a Expr_Vector\n";
	     $$=s1;
	     
	   }
            | 
 	   {
	     vector<t_DAGNode *> v;
	     class t_VectorOfExpressions *s1=new t_VectorOfExpressions();
	     cout<<"Matching a Expr_Vector\n";
	     $$=s1;
	     
	   };
**/
Operator : t_String { cout<<"Matching an operator "<<$1<<endl; /*ifvalidoperator() */ $$=$1;}
%%






void yyerror(char *s) 
{
  fprintf(stderr, "%s\n", s);
  fprintf(stderr, "ERROR!! In reading the DAG from file \n");
  abort();
}


