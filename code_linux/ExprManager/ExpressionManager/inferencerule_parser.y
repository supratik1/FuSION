%{

#include <string>
    //#include "yacc_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>

#include "ExpressionValue.h"
#include "InferTypeOfExpression.h"

    using namespace std;

    int yylex(void);
    void yyerror(char *);

    string getTypeValue(string type) 
    {
      string result;
      if (type == "SIGNED_INTEGER") 
	{
	  result = toString(TYPE_SIGNED_INTEGER);
	} 
      else if (type == "UNSIGNED_INTEGER") 
	{
	  result = toString(TYPE_UNSIGNED_INTEGER);
	} 
      else if (type == "SIGNED_BITVECTOR") 
	{
	  result = toString(TYPE_SIGNED_BITVECTOR);
	} 
      else if (type == "UNSIGNED_BITVECTOR") 
	{
	  result = toString(TYPE_UNSIGNED_BITVECTOR);
	} 
      else if (type == "REAL") 
	{
	  result = toString(TYPE_REAL);
	} 
      else 
	{
	  result = toString(TYPE_BOOL);
	}
      return result;
    }    
    //Map for handling  Inference Rule
    map <string, t_InferTypeOfExpression *> mapOfTypeInferenceRules;
    
%}

/*This is the union used to define YYSTYPE*/
%union {
  char *string_value;
  int int_value;
}


%token <string_value> t_String
%type <string_value> Operator
%type <string_value> t_Max
%type <string_value> t_Min
%type <string_value> t_Add
%type <strnig_value> t_Addall
%type <int_value> INT_NUMBER
%type <int_value> Integer
%type <string_value> InferredTypeTuple

%type <string_value> ArgumentForComputation
%type <string_value> Computation
%type <string_value> TypeTuple
%type <string_value> PrintString
%type <string_value> ParameterOfPrintBlock
%type <string_value> ParameterOfPrintStatement
%type <string_value> ParameterOfPrintWordList
%type <string_value> ParameterOfPrintWord
%type <string_value> OnceOrEvery
%type <string_value> t_KeyWordsOfPrint

%token <string_value> t_StringLiteral
%token <string_value> t_TypeValue

%type <string_value> OperandsNumbers
%type <string_value> TypeCastOperands

%token t_OBrack t_CBrack t_Comma t_Eq t_OSquareBrack t_CSquareBrack t_OperatorString t_MinArgs t_MaxArgs
%token t_Operand1 t_Operand2 t_Operand3 t_OtherOperandsString t_InferString t_InfinityString
%token t_Hash t_Comments t_Add t_Max t_Min t_Addall INT_NUMBER t_ArgumentNumber t_Variable
%token t_print t_FCBrack t_FOBrack t_Once t_Every t_SemiColon t_KeyWordsOfPrint t_printAll
%token t_CastString

%%
ListOfInferenceRules : InferenceRule
			{
			  ////cout << "Inference rule is there\n";
			}
			| InferenceRule ListOfInferenceRules
			 {
			   ////cout << "Inference Rule\n";
			 }

InferenceRule:		t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_Operand3 TypeTuple t_OtherOperandsString TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands
			{
				bool variableNumberOfArguments = ($4 != $6) ? true : false;
				bool unlimitedNumberOfArguments = ($6 == INFINITY_WIDTH) ? true : false;
		    		////cout << "Unlimited operads:" << unlimitedNumberOfArguments;
				/** **Special Case**
				 * Here handling only the operator that is applied to single type and
     				 * infinite number of arguments (Symmetric )
     				 */
				for (int i = $4; i <= $6; i++) {
        				string signature;
        				t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

        				string operatorSymbol = toString($2);

        				int minargs = $4;
        				int maxargs = i;
        				vector <TypeOfExpressionTuple *> operandTypes;
        				if (unlimitedNumberOfArguments)
                                        {
            					inferenceRule->setOperand1Type(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8));
            					operandTypes.push_back(inferenceRule->getOperand1Type());
            					inferenceRule->setOperand2Type( inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($10));
            					operandTypes.push_back(inferenceRule->getOperand2Type());
            					inferenceRule->setOperand3Type(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($12));
            					operandTypes.push_back(inferenceRule->getOperand3Type());
            					inferenceRule->setOtherOperandsType(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($14));
            					operandTypes.push_back(inferenceRule->getOtherOperandsType());
            					inferenceRule->setInferredType ($16);
                                                inferenceRule->setPrintExpressionFormat(toString($17));
						inferenceRule->type_cast_operands = toString($18);
                                                //signature = inferenceRule->generateKey($2, operandTypes);
						// Key is only with one operand type
						signature = operatorSymbol + "@";
						signature = signature + toString(operandTypes[0]->m_type);

            					////cout << "signature\n" << signature 
						//   << "\nInserting into the map\n";
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					break;
        				} 
					else 
					{
            					if (i >= 1) 
						{
	                				inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8);
                					operandTypes.push_back(inferenceRule->operand1Type);
            					}
            					if (i >= 2) 
						{
                					inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($10);
                					operandTypes.push_back(inferenceRule->operand2Type);
            					}
            					if (i >= 3) 
						{
                					inferenceRule->operand3Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($12);
                					operandTypes.push_back(inferenceRule->operand3Type);
            					}
            					if (i > 3) 
						{
                					inferenceRule->otherOperandsType = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($14);
                					operandTypes.push_back(inferenceRule->otherOperandsType);
            					}
            					signature = inferenceRule->generateKey($2, operandTypes);
            					inferenceRule->inferredType = $16;
						inferenceRule->type_cast_operands = toString($18);
                                                inferenceRule->printExpressionFormat=toString($17);
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					////cout << "\nsignature " << signature << "\nInserting into the map\n";
            					mapOfTypeInferenceRules[signature] = inferenceRule;
        				}
        				/**
         			         *  For Infinte length only consider the operator Symbol as Signature
         				 *  push Only other Operand type
         				 */
        				////cout<<"Inference accepted"<<endl;
    				}
			}
			| t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_OtherOperandsString TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands
			{
    				bool variableNumberOfArguments = ($4 != $6) ? true : false;
    				bool unlimitedNumberOfArguments = ($6 == INFINITY_WIDTH) ? true : false;
    				//cout << "Unlimited operads:" << unlimitedNumberOfArguments;
    				/** **Special Case**
     				 * Here handling only the operator that is applied to single type and
     				 * infinite number of arguments (Symmetric )
     				 */
    				for (int i = $4; i <= $6; i++) {
		        		string signature;
        				t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

        				inferenceRule->operatorSymbol = toString($2);

        				inferenceRule->minargs = $4;
        				inferenceRule->maxargs = i;
        				inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
        				inferenceRule->unlimitedNumberOfArguments = unlimitedNumberOfArguments;
                                        inferenceRule->printExpressionFormat=toString($15);
					inferenceRule->type_cast_operands = toString($16);
        				vector <TypeOfExpressionTuple *> operandTypes;
					
        				if (unlimitedNumberOfArguments) {
					  //signature = inferenceRule->generateKey($2, operandTypes);
					  inferenceRule->maxargs = INFINITY_WIDTH;
					  operandTypes.push_back(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8));
					  inferenceRule->setOperand1Type(operandTypes[0]);
						signature = inferenceRule->operatorSymbol + "@";
						signature = signature + toString(operandTypes[0]->m_type);

            					//cout << "signature\n" << signature << "\nInserting into the map\n";
				            	inferenceRule->inferredType = $14;
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					break;
        				} 
					else 
					{
 				           if (i >= 1) 
					   {
	                			inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8);
                				operandTypes.push_back(inferenceRule->operand1Type);
            				   }
            				   if (i >= 2) 
					   {
			                	inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($10);
                				operandTypes.push_back(inferenceRule->operand2Type);
            				   }
            				   inferenceRule->otherOperandsType = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($12);
            				   operandTypes.push_back(inferenceRule->otherOperandsType);
            				   signature = inferenceRule->generateKey($2, operandTypes);
            				   inferenceRule->inferredType = $14;
            				   mapOfTypeInferenceRules[signature] = inferenceRule;
            				   //cout << "\nsignature " << signature << "\nInserting into the map\n";
            				   mapOfTypeInferenceRules[signature] = inferenceRule;
        				}			
        				//cout<<"Inference accepted"<<endl;
    				}
				//cout << "\nPrints:" << $15;
			}
			| t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_Operand3 TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands
			{
	    			bool variableNumberOfArguments = ($4 != $6) ? true : false;
    				bool unlimitedNumberOfArguments = ($6 == INFINITY_WIDTH) ? true : false;
    				for (int i = $4; i <= $6; i++) 
				{
				        string signature;

				        t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

				        inferenceRule->operatorSymbol = toString($2);
	
				        inferenceRule->minargs = $4;
				        inferenceRule->maxargs = i;
                                        inferenceRule->printExpressionFormat=toString($15);
					inferenceRule->type_cast_operands = toString($16);
				        vector <TypeOfExpressionTuple *> operandTypes;
				        inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8);
				        operandTypes.push_back(inferenceRule->operand1Type);
					
				        if (i >= 2) 
					{
				            inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($10);
				            operandTypes.push_back(inferenceRule->operand2Type);
				        }
				        if (i >= 3) 
					{
				            inferenceRule->operand3Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($12);
				            operandTypes.push_back(inferenceRule->operand3Type);
				        }
				        signature = inferenceRule->generateKey($2, operandTypes);
				        inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
				        inferenceRule->inferredType = $14;
				        mapOfTypeInferenceRules[signature] = inferenceRule;
				        //cout << "signature\n" << signature << "\nInserting into the map\n";
				        //cout<<"3 Operands";
				    }
			}
			| t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands
			{
			    bool variableNumberOfArguments = ($4 != $6) ? true : false;
			    bool unlimitedNumberOfArguments = ($6 == INFINITY_WIDTH) ? true : false;
			    for (int i = $4; i <= $6; i++) 
			    {
        			if ($6 != 2) 
				{
			            yyerror("Error while parsing file (Minargs != Maxargs or Maxargs  not 2)");
			        }
				//cout<<"Inference rule of 2 operands:";
			        
			        t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();
				string signature;

			        inferenceRule->operatorSymbol = toString($2);

			        inferenceRule->minargs = i;
			        inferenceRule->maxargs = i;

                                inferenceRule->printExpressionFormat=toString($13);
				inferenceRule->type_cast_operands = toString($14);
			        vector <TypeOfExpressionTuple *> operandTypes;

			        inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8);
			        operandTypes.push_back(inferenceRule->operand1Type);

			        if (i == 2) 
				{
			            inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($10);
			            operandTypes.push_back(inferenceRule->operand2Type);
			        }
			        signature = inferenceRule->generateKey($2, operandTypes);
			        inferenceRule->inferredType = $12;
			        inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
			        mapOfTypeInferenceRules[signature] = inferenceRule;
			        //cout << "\nsignature " << signature << "\nInserted into the map\n";
			    }
			}
			| t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands
			{
			    string signature;
			    if ($6 != $4 || $4 != 1) 
			    {
			        yyerror("Error while parsing file (Minargs != Maxargs or Maxargs  not 1)");
    			    }
			    t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

			    inferenceRule->operatorSymbol = toString($2);

			    inferenceRule->minargs = $4;
			    inferenceRule->maxargs = $6;
                            inferenceRule->printExpressionFormat=toString($11);
			    inferenceRule->type_cast_operands = toString($12);
			    vector <TypeOfExpressionTuple *> operandTypes;
			    inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations($8);
			    operandTypes.push_back(inferenceRule->operand1Type);
			    signature = inferenceRule->generateKey($2, operandTypes);
			    inferenceRule->inferredType = $10;
			    mapOfTypeInferenceRules[signature] = inferenceRule;

			    //cout << "\nsignature " << signature << "Inserted into map\n";
    			    //cout<<"Single Oprand type inferencing \n";
			}
			| Comments
			 {
			    //cout<<"Comments Accepted"<<endl;
			 }

TypeTuple:		t_TypeValue t_OSquareBrack Integer t_CSquareBrack
			{
			    string passtype($1);
			    string result = getTypeValue(passtype);
			    result += "@";
			    result += toString($3);
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    $$ = res;
			    //cout<<"Type tuple is being created \n";
			}
			| t_TypeValue t_OSquareBrack t_Hash Integer t_CSquareBrack
			 {
			    string passtype($1);
			    string result = getTypeValue(passtype);
			    result += "@#";
			    result += toString($4);
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    $$ = res;
			    //cout<<"Type tuple with some operand length is recieved \n";
			}

InferredTypeTuple:	TypeTuple
			{
			    //cout << "Inferring the type directly according to operand\n";
			    //cout << $1;
			    $$ = $1;
			}
			| t_TypeValue t_OSquareBrack Computation t_CSquareBrack
			{
			    string passtype($1);
			    string result = getTypeValue(passtype);
			    result += "@";
			    result += toString($3);
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    $$ = res;
			    //cout<<"\nInferred type:"<<res;
			    //cout<<"Inferring the tuple along with some computation\n";
			}

Computation:		t_Max t_OBrack ArgumentForComputation t_Comma ArgumentForComputation t_CBrack
			{
			    string computation;
			    computation = "%max(" + toString($3) + "," + toString($5) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    $$ = res;
			    //cout<<"Computation - Max\n"<<res;
			}
			| t_Min t_OBrack ArgumentForComputation t_Comma ArgumentForComputation t_CBrack
			{
			    string computation;
			    computation = "%min(" + toString($3) + "," + toString($5) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    $$ = res;
			    //cout<<"Computataion - Min\n"<<res;
			}
			| t_Add t_OBrack ArgumentForComputation t_Comma ArgumentForComputation t_CBrack
			{
			    string computation;	
			    computation = "%add(" + toString($3);
			    computation = computation + "," + toString($5) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    $$ = res;
			    //cout<<"Computataion - Min\n"<<res;
			}
			| t_Addall t_OBrack ArgumentForComputation t_CBrack
			{
			    string computation;
			    computation = "%addall(";
			    computation += (toString($3) + ")");
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    $$ = res;
			    //cout<<"Computation - Add all\n"<<res;
			}

ArgumentForComputation:  t_Hash Integer
			{
			    string res = "#";
			    res.append(toString($2));
			    char *resCopy = new char[res.length() + 1];
			    strcpy(resCopy, res.c_str());
			//cout<<"Hash Int:"<<$2;
			    $$ = resCopy;
			} 
			| Integer
			{
			    string str(toString($1));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    $$ = res;
			} 
			| Computation
			{
			    $$ = $1;
			} 
	
Comments:		t_Comments
			{
			    //Nothing to do
			    //cout<<"Recieved Comments Nothing to do\n";
			}
			|;

Integer:		INT_NUMBER
			{
			    $$ = $1;
			    //cout<<"Integer Number\n"<<$1;
			}
			| t_InfinityString
			{
			    $$ = INFINITY_WIDTH;
			   // //cout << "Infinity Recieved\n";
			}
			| t_Variable
			{
			    $$ = VARIABLE_WIDTH; //For bitvector SIGNED_BITVECTOR[$k]
			}

Operator:		t_String
			{
			    //cout<<"Operator in YACC:"<<$1;
    				$$ = $1;
			}
	

PrintString:		t_print t_FOBrack ParameterOfPrintBlock t_FCBrack
			{
			    //cout<<endl<<"----";
			    //cout<<endl<<toString($3)<<endl;
			    string str($3);
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
    			    $$ = res;
                            //cout<<"\n"<<res;
			}
		

ParameterOfPrintBlock:	ParameterOfPrintStatement ParameterOfPrintBlock
			{
			    //cout<<$1<<endl<<$2<<endl;
				string res($1);
				res.append($2);
				char *result=new char[res.length()+1];
				strcpy(result,res.c_str());
				$$=result;	
			}
			| ParameterOfPrintStatement
			{
			    $$ = $1;
			    //cout<<$1<<endl;
			}

ParameterOfPrintStatement:	OnceOrEvery ParameterOfPrintWordList t_SemiColon
			{
			   // //cout<<$1<<endl<<$2<<endl;
			    string str($1);
			    str += (toString($2) + ";");
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            $$ = res;
			}
                        | OnceOrEvery t_printAll ParameterOfPrintWordList t_SemiColon
                        {
                            string str($1);
                            str+= ("$printall"+toString($3)+";");
                            char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            $$ = res;
                        }


ParameterOfPrintWordList:	ParameterOfPrintWord ParameterOfPrintWordList
			{
			    string str($1);
			    str += (" " + toString($2));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    $$ = res;
                           //cout<<res<<endl;
			}
			| ParameterOfPrintWord
			{
			    //char *res= new char[str.length()+1];
			    //strcpy(res,str.c_str());
			    //$$=res;
			    $$ = $1;
			    //cout<<$1<<endl;
			}
ParameterOfPrintWord:
			t_StringLiteral
			{
			    $$ = $1;
			    //cout<<$1<<endl;
			}
			| t_KeyWordsOfPrint
			{
    				$$ = $1;
			    //cout<<$1<<endl;
			}
OnceOrEvery:		t_Once
			{
			    $$ = "$once";
			    //cout<<"Once"<<endl;
			}
			| t_Every
			{
			    $$ = "$every";
			    //cout<<"Every"<<endl;
			}

TypeCastOperands:      t_CastString OperandsNumbers
                       {
			 //cout<<"Type cast:"<<$2<<endl;
			 $$ = $2;
		       }

OperandsNumbers        : INT_NUMBER t_Comma OperandsNumbers
                        {
			  string str(toString($1));
			  str += ("," + toString($3));
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  $$ = res; 
			}|INT_NUMBER
                        {
			  string str(toString($1));
			  if( $1 == 0 )
			    {
			      str = "";
			    }			  
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  $$ = res;
			}
%%


void yyerror(char *s) 
{
    fprintf(stderr, "%s\n", s);
    abort();
}
/*
int main() {
    yyparse();
    return 0;
}
*/
