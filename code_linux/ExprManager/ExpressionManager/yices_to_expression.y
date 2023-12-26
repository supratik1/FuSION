%{
/********************************************************************
  This code was developed at IIT Bombay, India partly using public-domain
  free code.  No part of this code may be used for commercial or other 
  purposes without explicit permission from supratik@cse.iitb.ac.in

  August 14 2009
*********************************************************************/

#include <stdio.h>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "idinfo.h"
#include "ExpressionManager.h"
#define YYDEBUG 1
#define t_ENUM 9999

using namespace std;

void yyerror(char *);
 int yylex(void);  
 int yywrap()
 {
   return 1;
 }
struct node_pair
{
	int first;
	int second;
};


class NamedExprPair
{
 public:
  string symbol;
  Expression *node;
  NamedExprPair(string symbol_in, Expression *node_in)
    {
      symbol = symbol_in;
      node = node_in;
    }
};

 vector<NamedExprPair> v_named_exprs;
class VectorOfExpressions
{
 public:
  vector<Expression *> v_expressions;
  VectorOfExpressions(vector<Expression* > v_expressions_to_push)
    {
      for(int i=0;i<v_expressions_to_push.size();i++)
        {
          v_expressions.push_back(v_expressions_to_push[i]);
        }
    }
  VectorOfExpressions() //A do nothing constructor..
    {
    }

};

vector<Expression *> assign_expressions(
                                          class VectorOfExpressions *v_exprs_in
                                          )
{
  vector<Expression *>v_exprs;
  for(int i=0;i<v_exprs_in->v_expressions.size();i++)
    {
      v_exprs.push_back(v_exprs_in->v_expressions[i]);
    }

  return v_exprs;
}




vector<string> graph_nodes;
vector< pair<int,int> > edges;

map<string, int> decl_types_list;

ExpressionManager dm;

int count1 = 0;
int count2 = 0;
int globalnewhashcounter=0;
int globalnewnodenamecounter=0;



%}


%union{
  struct IDinfo *ds_Id;
  class Expression *dagnode;
  class VectorOfExpressions *vector_of_expressions;
  int value;
}

%token <value> LOG_NOT Log_And Log_Or Ternary Define Bool Real Int BitVector
%token <value> Assert Check Define_Type UType t_EQSym t_NESym t_LTSym 
%token <value> t_LESym t_GTSym t_GESym t_Plus t_Minus t_Ampersand
%token <value> t_Star t_Slash t_LeftParen t_RightParen t_doubleColon
%token <value> t_Colon t_Semicolon t_Arrow t_Box t_Bar t_Dot

%token  < ds_Id > t_Identifier t_AbstractLit 

%type <dagnode> named_expr
%type  <value>  yices_basic_types

%type <dagnode> expr type_define_constructs
%type <vector_of_expressions> exprlist assert_constructs
%%

 /*------------------Grammar----------------------------------------*/

start : define_constructs type_define_constructs named_expr assert_constructs
	{
	  

/*	  for(int i=0; i<v_named_exprs.size();i++)
	    {
	      string symbol_to_search = v_named_exprs[i].symbol;
	      Expression *to_replace = dm.createNode(symbol_to_search);
	      cout<<"Symbol: "<<symbol_to_search<<endl;
	      Expression *node_replace_by = v_named_exprs[i].node;
	      cout<<"Replacer.....\n";
	      //	      dm.printDAG(node_replace_by);
	      dm.ReplaceLeafByDAG(to_replace, node_replace_by);
	    }*/
	  cout<<"Printing constructs from assertions\n";

	  vector <Expression *> v_expressions = assign_expressions($4);

	  ofstream *outfile = new ofstream ();
	  outfile->open("dot");
	  //	  dm.printDAGAsDOTFile(v_expressions[0], outfile);
	  /*cout<<"Is Tree:"<<dm.isDAGATree(v_expressions[0])<<endl;
	    vector<Expression *>bfs_nodes = dm.getVectorOfDescendentNodesInBFSOrder(v_expressions[0]);*/
	  /*	  cout<<"BFS Nodes "<<bfs_nodes.size()<<endl;
	  for(int i=0;i<bfs_nodes.size();i++)
	  cout<<bfs_nodes[i]->getLabel()<<endl;*/

	  /*cout<<"Height of DAG :"<<dm.getHeightOfDAG(v_expressions[0])<<endl;*/

	}

;


define_constructs : t_LeftParen Define_Type UType t_RightParen  
            {
	      //This matches with exactly one line
	      //definetype utype
	      cout<<"Matched define_constructs\n";; 
	    }
;



type_define_constructs : 
                       t_LeftParen Define t_Identifier t_doubleColon yices_basic_types t_RightParen 
		       {
			 TypeOfExpressionTuple resultType;
			 
			 resultType.type = TYPE_SIGNED_INTEGER;
			 resultType.width = 0;
			 
			 Expression *node = dm.createSymbol ($3->name, resultType);
			 cout<<"Matched type_define_constructs\n";
			 $$ = node;

		       }

                       | 
                       type_define_constructs t_LeftParen Define t_Identifier t_doubleColon yices_basic_types t_RightParen 
		       {
			 TypeOfExpressionTuple resultType;
			 resultType.type = TYPE_SIGNED_INTEGER;
			 resultType.width = 0;

		
			 Expression *node = dm.createSymbol ($4->name, resultType);
			 $$ = node;
			 cout<<"Matched type_define_constructs\n";
		
		       }
                       |
		       
			 type_define_constructs t_LeftParen Define t_Identifier t_doubleColon t_LeftParen yices_basic_types t_AbstractLit t_RightParen t_RightParen
		       {
			 TypeOfExpressionTuple resultType;
			 resultType.type = TYPE_SIGNED_INTEGER;
			 resultType.width = 0;

			 Expression *node = dm.createSymbol ($4->name, resultType);
			 $$ = node;
			 cout<<"Matched type_define_constructs\n";

		       }
            
		       
;



yices_basic_types : 
                  Bool { 
		    cout<<"Matched basic type Bool\n";
		    $$ = TYPE_BOOL;
		  } 
                  | 
                  Real { 
		    cout<<"Matched basic type Real\n";
		    $$ = TYPE_REAL;
		  } 
                  | 
                  Int { 
		    cout<<"Matched basic type Int\n";
		    $$ = TYPE_SIGNED_INTEGER;
		  }
                   |
		   BitVector
		   {
		     $$ = TYPE_SIGNED_BITVECTOR;
		   }

;



assert_constructs : t_LeftParen Assert exprlist t_RightParen t_LeftParen Check t_RightParen
	{
	  cout<<"Matched assert_constructs\n";
	  vector <Expression *> v_expressions = assign_expressions($3);
	  class VectorOfExpressions *s1=new VectorOfExpressions(v_expressions);
	  $$ = s1;
	}
;


named_expr : 
             t_LeftParen Define t_Identifier t_doubleColon yices_basic_types expr t_RightParen
             {
	       cout<<"Matched an named_expr\n";


	       TypeOfExpressionTuple resultType;
			 
	       resultType.type = TYPE_SIGNED_INTEGER;
	       resultType.width = 0;
	       Expression *temp_node = dm.createSymbol($3->name, resultType);
	       NamedExprPair expr_pair($3->name, $6);
	       //	       v_named_exprs.push_back(expr_pair, resultType);
	       $$ = $6;
             }
             | 
             named_expr t_LeftParen Define t_Identifier t_doubleColon yices_basic_types expr t_RightParen 
             {         	

	       TypeOfExpressionTuple resultType;
	       
	       resultType.type = TYPE_SIGNED_INTEGER;
	       resultType.width = 0;
	       Expression *temp_node = dm.createSymbol($4->name, resultType);
	       NamedExprPair expr_pair($4->name, $7);
	       v_named_exprs.push_back(expr_pair);
	       $$ = $7;

	       cout<<"Matched an named_expr\n";
             }

;



expr : t_LeftParen LOG_NOT exprlist t_RightParen 
              { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("lognot",v_operands);
	       $$ = node;
	      }

;

expr : t_LeftParen Log_And exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("logand",v_operands);
	       

	       $$ = node;
	     }
;

expr : t_LeftParen Log_Or exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("logor",v_operands);


	       $$ = node;
	     }
;


expr : t_LeftParen Ternary exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("if-then-else",v_operands);


	       $$ = node;
	     }
;

expr : t_LeftParen t_EQSym exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("equals",v_operands);


	       $$ = node;
	     }
;


expr : t_LeftParen t_LTSym exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("less-than",v_operands);


	       $$ = node;
	     }
;

expr : t_LeftParen t_GTSym exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("greater-than",v_operands);

	       $$ = node;
	     }
;


expr : t_LeftParen t_GESym exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("greater-equal",v_operands);

	       $$ = node;
	     }
;


expr : t_LeftParen t_LESym exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("less-equal",v_operands);


	       $$ = node;
	     }
;

expr : t_LeftParen t_Plus exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("plus",v_operands);

	       $$ = node;
	     }
;


expr : t_LeftParen t_Minus exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("minus",v_operands);

	       $$ = node;
	     }
;



expr : t_LeftParen t_Star exprlist t_RightParen 
             { 
	       cout<<"Matched an expr\n";
	       vector <Expression *> v_operands;
	       v_operands = assign_expressions($3);
	       Expression * node = dm.createExpression("multiply",v_operands);
	       $$=node;

	     }
;




exprlist: expr { 
                cout<<"Matched an exprlist with expr\n";
	       vector<Expression *> v;
	       v.push_back($1);
	       class VectorOfExpressions *s1=new VectorOfExpressions(v);
	       $$=s1;
	
	       }
             |
          exprlist expr
	     {
	       
	       vector<Expression *> v;
	       v=assign_expressions($1);
	       v.push_back($2);
	       class VectorOfExpressions *s1=new VectorOfExpressions(v);
	       $$=s1;
	       cout<<"Matched an exprlist with expr exprlist\n";

	     }
;



expr     : t_Identifier
	{
	  //Create a leaf node here
	  cout<<"Found an identifier "<<$1->name<<endl;


	  TypeOfExpressionTuple resultType;
	  resultType.type = TYPE_NONE;
	  resultType.width = 0;

	  Expression *node = dm.createSymbol($1->name, resultType);
	  if(node==NULL)
	    cout<<"ERROR !! Found a null node \n";
	  $$ = node;
	}

        |

        t_AbstractLit
	{
	  cout<<"Found a abstract literal "<<$1->name<<endl;

	  TypeOfExpressionTuple resultType;
	  resultType.type = TYPE_SIGNED_INTEGER;
	  resultType.width = 0;
	  Expression *node;
	  node = dm.createConstant($1->name, resultType);
	  if(node==NULL)
	    cout<<"ERROR !! Found a null node \n";

	  $$ = node;
	}

;
%%
extern FILE* yyout;

void yyerror(char* str)
{
  fprintf( yyout, "ERROR from parser: %s\n", str );
}


  

/*        extern FILE *yyin;
	FILE *fp = fopen(argv[1], "r");
	yyin = fp;
	yyparse();

*/
