%{

#include <string>
#include "DAGSimplifier.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DagManager.h"
#include "helping_functions.h"
  using namespace std;
  int yylex(void);
  void yyerror(char *);
  extern ofstream *outfile;
  int rules_parserlex(void);
  void rules_parsererror(char *);
  vector<t_RewriteRuleExpression *> assign_arguments_of_simplification_expressions
    (
     class t_VectorOfRewriteRuleExpressions *v_exprs_in
     );

 extern t_DAGSimplifier *ds;
%}





/*This is the union used to define YYSTYPE*/
%union{
  char  *string_value;
  int   int_value;
  class t_RewriteRuleExpression *bool_expression_type;
  class t_RewriteRuleExpression *expression_type;
  class t_RewriteRule *rule_type;
  
  class t_VectorOfRewriteRuleExpressions *vector_of_exprs_type;
}

%token <string_value> t_String t_Arrow t_True t_False t_Constant t_Integer t_Variable
%token <string_value> t_OBrack t_CBrack t_Comma t_Eq t_BoolVariable

%type <string_value> Operator
%type <expression_type> nt_LHSExpr nt_RHSExpr
%type <rule_type> Rule
%type <vector_of_exprs_type> nt_LHS_Arguments_List nt_RHS_Arguments_List

%type <expression_type> LHSConstant LHSVariable LHSBoolVariable RHSConstant RHSVariable RHSBoolVariable

%%

RulesFile : RulesList 
         {
	   //cout<<"Read the RulesFile successfully\n";
	   const string sRewriteRulesFile = 
	       t_ConfigurationOptions::getConfig()->m_sTempInfoDir + "/DAGManager/RewriteRulesRead";
	   ofstream *outfile = new ofstream();
	   outfile->open(sRewriteRulesFile.c_str());
	   ds->printAllRewriteRules(outfile);
	   outfile->close();
	 }
RulesList : RulesList  Rule 
          {
	    //cout<<"Matched RulesList \n";
	  }
  
          |  
	  {
	    //cout<<"Matched RulesList with 0 rules\n";
	  }
            ;

Rule :  t_Integer nt_LHSExpr t_Arrow nt_RHSExpr
         {
	   //cout<<"----------------------------------------\n";
	   //cout<<"Read an rule successfully\n";
	   t_RewriteRule * rule = new t_RewriteRule($2, $4, stringToInteger(toString($1)));
	   ds->addARewriteRule(rule);

	   t_RewriteRuleExpression *lhs_expr = $2;
	   ds->addValidStringOfLHSOfARule(lhs_expr->getValidSubstringOfRewriteRuleExpression());
	   $$ = rule;
         }



/**create tokens for all operators that are to be added */
nt_LHSExpr :  t_OBrack Operator nt_LHS_Arguments_List t_CBrack
	{
	  //cout<<"Creating an nt_Expr with operator and arguments\n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions($3);
	  string valid_substring = $2;
	  for(int i=0; i<arguments.size();i++)
	    {
	      valid_substring += ",";
	      valid_substring+=arguments[i]->getValidSubstringOfRewriteRuleExpression();
	    }
	  //cout<<"Adding the VSS "<<valid_substring<<endl;
	  ds->addValidSubstring(valid_substring);
	  
	  t_RewriteRuleExpression *expression = new t_RewriteRuleExpression($2, other, arguments, 0,valid_substring);
	  //cout<<"Created an nt_Expr\n";
	  $$ = expression;
	}
        |
	LHSVariable
	{
	  //cout<<"Creating an nt_Expr with Variable\n";
	  $$ = $1;
	}
        |
	LHSConstant
	{
	  //cout<<"Creating an nt_Expr with Constant\n";
	  $$ = $1;
	}
        |
	LHSBoolVariable
	{
	  //cout<<"Creating an nt_Expr with Bool Variable\n";
	  //cout<<"Created and VariableRewriteRuleExpression\n";
	  $$ = $1;
	}
	|
	t_String
	{
	string variable_name = $1;
	   int variable_index = 0;
	   ds->addValidSubstring($1);
	   //cout<<"Adding the VSS "<<"$1"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression($1, placeholder,variable_index, $1);
	   //cout<<"Creating a expression\n";
	   $$ = expr;

	}
LHSVariable : t_Variable
         {
	   //these are basically e_1 e_2 ...
	   string variable_name = $1;
	   //string var_to_create = "e";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("e");
	   //cout<<"Adding the VSS e"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression($1, placeholder, variable_index, "e");
	   //cout<<"Creating a expression\n";
	   $$ = expr;
	   
          }
         
LHSBoolVariable : t_BoolVariable
         {
	   //These are basically b_0 b_1 b_2 ...
	   string variable_name = $1;
	   //string var_to_create = "b";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("b");
	   //cout<<"Adding the VSS "<<"b"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression($1, placeholder,variable_index, "b");
	   //cout<<"Creating a expression\n";
	   $$ = expr;
         }

LHSConstant : 
        t_True 
	{
	  //this is single constant true
	  t_RewriteRuleExpression *true_expr = new t_RewriteRuleExpression($1, fixed_string,0, "true");
	  string valid_substring = "true";
	  //cout<<"Adding the VSS "<<valid_substring<<endl;
	  ds->addValidSubstring(valid_substring);

	  //cout<<"Creating a constant true\n";
	  $$ = true_expr;
	}
       |
        t_False
       {
	  //this is single constant false
	 t_RewriteRuleExpression *false_expr = new t_RewriteRuleExpression($1, fixed_string,0,"false");
	 string valid_substring = "false";
	 //cout<<"Adding the VSS "<<valid_substring<<endl;
	 ds->addValidSubstring(valid_substring);
	 //cout<<"Creating a constant false\n";
	 $$ = false_expr;
       }
        |
	t_Constant
	{
	  //these are basically constants like c_1 c_2 c_3....
	  string variable_name = $1;
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("c");
	   //cout<<"Adding the VSS c\n";
	   t_RewriteRuleExpression *const_expr = new t_RewriteRuleExpression($1, placeholder,variable_index,"c");
	   //cout<<"Creating a constant false\n";
	   $$ = const_expr;
	}
	|
       t_Integer
       {
	 ds->addValidSubstring($1);
	 //cout<<"Adding the VSS  "<<$1<<endl;
	 t_RewriteRuleExpression *integer_expr = new t_RewriteRuleExpression($1, fixed_string,0,$1);
	 string valid_substring = $1;
	 //cout<<"Adding the VSS "<<valid_substring<<endl;
	 ds->addValidSubstring(valid_substring);
	 //cout<<"Creating an integer \n";
	 $$ = integer_expr;
       }





/**create tokens for all operators that are to be added */
nt_RHSExpr :  t_OBrack Operator nt_RHS_Arguments_List t_CBrack
	{
	  //cout<<"Creating an nt_Expr with operator and arguments\n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions($3);
	  t_RewriteRuleExpression *expression = new t_RewriteRuleExpression($2, other, arguments, 0,"");
	  //cout<<"Created an nt_Expr\n";
	  $$ = expression;
	}
        |
	RHSVariable
	{
	  //cout<<"Creating an nt_Expr with Variable\n";
	  $$ = $1;
	}
        |
	RHSConstant
	{
	  //cout<<"Creating an nt_Expr with Constant\n";
	  $$ = $1;
	}
        |
	RHSBoolVariable
	{
	  //cout<<"Creating an nt_Expr with Bool Variable\n";
	  //cout<<"Created and VariableRewriteRuleExpression\n";
	  $$ = $1;
	}
	|
	t_String
	{
	string variable_name = $1;
	   int variable_index = 0;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression($1, placeholder,variable_index, $1);
	   //cout<<"Creating a expression\n";
	   $$ = expr;

	}

RHSVariable : t_Variable
         {
	   //these are basically e_1 e_2 ...
	   string variable_name = $1;
	   //string var_to_create = "e";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   //ds->addValidSubstring("e");
	   ////cout<<"Adding the VSS e"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression($1, placeholder, variable_index, "e");
	   //cout<<"Creating a expression\n";
	   $$ = expr;
	   
          }
         
RHSBoolVariable : t_BoolVariable
         {
	   //These are basically b_0 b_1 b_2 ...
	   string variable_name = $1;
	   //string var_to_create = "b";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   //ds->addValidSubstring("b");
	   ////cout<<"Adding the VSS "<<"b"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression($1, placeholder,variable_index, "b");
	   //cout<<"Creating a expression\n";
	   $$ = expr;
         }

RHSConstant : 
        t_True 
	{
	  //this is single constant true
	  t_RewriteRuleExpression *true_expr = new t_RewriteRuleExpression($1, fixed_string,0, "true");
	  //cout<<"Creating a constant true\n";
	  $$ = true_expr;
	}
       |
        t_False
       {
	  //this is single constant false
	 t_RewriteRuleExpression *false_expr = new t_RewriteRuleExpression($1, placeholder,0,"false");
	 //cout<<"Creating a constant false\n";
	 $$ = false_expr;
       }
        |
	t_Constant
	{
	  //these are basically constants like c_1 c_2 c_3....
	  string variable_name = $1;
	   //string var_to_create = "c";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   t_RewriteRuleExpression *const_expr = new t_RewriteRuleExpression($1, placeholder,variable_index,"c");
	   //cout<<"Creating a constant false\n";
	   $$ = const_expr;
	}
	|
       t_Integer
       {
	 t_RewriteRuleExpression * integer_expr;
	 integer_expr = new t_RewriteRuleExpression($1, fixed_string,0,$1);
	 //cout<<"Creating an integer \n";
	 $$ = integer_expr;
       /*Operand_RewriteRuleExpressions_List is the list of (comma seperated) optional operands of an operator. Do not confuse with Expr_Vector*/
       }





nt_LHS_Arguments_List : nt_LHS_Arguments_List nt_LHSExpr
	{
	  //cout<<"Creating vector of arguments \n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions($1);
	  t_RewriteRuleExpression *arg_to_add = $2;
	  arguments.push_back(arg_to_add);
	  t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	  //cout<<"Created vector of arguments\n";
	  $$ = vector_of_expressions;
	}
	  | 
	  nt_LHSExpr
	  {
	    //cout<<"Creating vector of arguments with only one argument\n";
	    vector<t_RewriteRuleExpression *> arguments;
	    arguments.push_back($1);
	    t_VectorOfRewriteRuleExpressions *vector_of_expressions
	      = new t_VectorOfRewriteRuleExpressions(arguments);
	    
	    //cout<<"Created vector of arguments with only one argument\n";
	    $$ = vector_of_expressions;
	  }
         
nt_RHS_Arguments_List : nt_RHS_Arguments_List nt_RHSExpr
	{
	  //cout<<"Creating vector of arguments \n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions($1);
	  t_RewriteRuleExpression *arg_to_add = $2;
	  arguments.push_back(arg_to_add);
	  t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	  //cout<<"Created vector of arguments\n";
	  $$ = vector_of_expressions;
	}
	  | 
	  nt_RHSExpr
	  {
	    //cout<<"Creating vector of arguments with only one argument\n";
	    vector<t_RewriteRuleExpression *> arguments;
	    arguments.push_back($1);
	    t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	    //cout<<"Created vector of arguments with only one argument\n";
	    $$ = vector_of_expressions;
	  }



/*Operator : nt_Expr
          {
	    $$ = $1;
	    }*/
Operator : t_String 
           { //cout<<"Matching an operator "<<$1<<endl;  
	     $$=$1;
	   }
%%






void yyerror(char *s) 
{
  fprintf(stderr, "%s\n", s);
  fprintf(stderr, "ERROR!! In Parsing the simplification rules file \n");
  abort();
}


