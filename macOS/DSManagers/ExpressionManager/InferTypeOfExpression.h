#ifndef INFERTYPEOFEXPRESSION_H
#define INFERTYPEOFEXPRESSION_H

//#include "ExpressionValue.h"
#include "helping_functions.h"

#include<vector>
#include<string>
#include<sstream>
#include<map>

using namespace std;

typedef struct
{
    TypeOfExpressionTuple typeInfo;
    string label;
}t_LableTypeAndValueforPrinting;

/**
 * Class : Infertypeofexpression
 * Description : This class will represent single inference rule present in rule specification file. 
 */

class t_InferTypeOfExpression
{
 public:
  string operatorSymbol;
  int minargs;
  int maxargs;
  TypeOfExpressionTuple *operand1Type;
  TypeOfExpressionTuple *operand2Type;
  TypeOfExpressionTuple *operand3Type;
  TypeOfExpressionTuple *otherOperandsType;

  bool variableNumberOfArguments;
  bool unlimitedNumberOfArguments;
  
  string inferredType;

  string printExpressionFormat;
  string type_cast_operands;


  //Constructors
  t_InferTypeOfExpression();

  t_InferTypeOfExpression(string operatorSymbol,int minargs,int maxargs,string inferredType,
                       string printExpressionFormat,TypeOfExpressionTuple *operand1Type,
                       TypeOfExpressionTuple *operand2Type=NULL,TypeOfExpressionTuple *operand3Type=NULL,
                       TypeOfExpressionTuple *otherOperandsType=NULL,bool variableNumberOfArguments=false,
                       bool unlimitedNumberOfArguments=false);
  

  //set methods
  bool setOperatorSymbol(string operatorSymbol);
  bool setMinargs(int minargs);
  bool setMaxargs(int maxargs);
  bool setOperand1Type(TypeOfExpressionTuple *type);
  bool setOperand2Type(TypeOfExpressionTuple *type);
  bool setOperand3Type(TypeOfExpressionTuple *type);
  bool setOtherOperandsType(TypeOfExpressionTuple *type);
  bool setVariableNumberOfArguments(bool haveVariableArguments);
  bool setUnlimitedNumberOfArguments(bool unlimitedArguments);
  bool setInferredType(string inferString);
  bool setPrintExpressionFormat(string printFormat);

  //get methods
  string getOperatorSymbol();
  int getMinargs();
  int getMaxargs();
  TypeOfExpressionTuple *getOperand1Type();
  TypeOfExpressionTuple *getOperand2Type();
  TypeOfExpressionTuple * getOperand3Type();
  TypeOfExpressionTuple *getOtherOperandsType();
  bool getVariableNumberOfArguments();
  bool getUnlimitedNumberOfArguments();
  string getInferredType();
  string getPrintExpressionFormat();

  //Generate Key for searching in the map for this object
  string generateKey(string operatorSymbol,vector <TypeOfExpressionTuple *> operands);

  //Infer the type based on the operands
  //i.e.,It is identified that the object is inference rule that is 
  // having the specified by the signature of the object
  TypeOfExpressionTuple *evaluateTypeBasedOnInferencing(vector <TypeOfExpressionTuple *> operands);    

  TypeOfExpressionTuple *stringToTypeOfExpressionTupleWithNoComputations(string typeAsString);
  string stringToTypeOfExpressionTupleWithComputations(string inferString,vector <TypeOfExpressionTuple *>operands);

  //Takes map of values to be replaced(%labeli,width and value) in printString
  string parsePrintStringOfOneOperator(map <string,t_LableTypeAndValueforPrinting *> &replaceMap,string printString,int maxargs);

  //Takes printstring buffer and replaces $once,$every,;," etc..
  string makePrintStringIntoActualFormat(string &printBuffer);


  string parseUnlimitedNumberOfOperandsPrintString(string &printBuffer, vector<TypeOfExpressionTuple *> &operandsVector);
};


#endif  /* INFERTYPEOFEXPRESSION_H */
