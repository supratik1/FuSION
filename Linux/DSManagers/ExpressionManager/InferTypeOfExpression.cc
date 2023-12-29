
#include <string>
#include <vector>
#include "ExpressionValue.h"
#include "helping_functions.h"

#include "InferTypeOfExpression.h"

t_InferTypeOfExpression::t_InferTypeOfExpression()
{
}
t_InferTypeOfExpression::t_InferTypeOfExpression(string operatorSymbol,int minargs,int maxargs,string inferredType,
                        string printExpressionFormat,TypeOfExpressionTuple *operand1Type,
                        TypeOfExpressionTuple *operand2Type,TypeOfExpressionTuple *operand3Type,
                        TypeOfExpressionTuple *otherOperandsType,bool variableNumberOfArguments,
                        bool unlimitedNumberOfArguments)
{
    this->operatorSymbol=operatorSymbol;
    this->minargs=minargs;
    this->maxargs=maxargs;
    this->operand1Type=operand1Type;
    this->operand2Type=operand2Type;
    this->operand3Type=operand3Type;
    this->otherOperandsType=otherOperandsType;
    this->inferredType=inferredType;
    this->unlimitedNumberOfArguments=unlimitedNumberOfArguments;
    this->variableNumberOfArguments=variableNumberOfArguments;
    this->printExpressionFormat=printExpressionFormat;
}

/**
 * Generate Key to Search in Map of Inferece Rules
 */
string t_InferTypeOfExpression::generateKey(string operatorSymbol, vector <TypeOfExpressionTuple *> operands)
{
    string signature = operatorSymbol;
    int i = 0;
    for (i = 0; i < operands.size(); i++)
    {
        signature += ("@" + toString(operands[i]->m_type));
    }
    return signature;
}

/**
 * evaluate the type of expression based on mapping with operands
 */
TypeOfExpressionTuple *t_InferTypeOfExpression::evaluateTypeBasedOnInferencing(vector <TypeOfExpressionTuple *> operands)
{
  //cout<<"Inside evaluateTypeBasedOnInferencing :"<<this->inferredType<<endl;
  //cout<<"Minargs:"<<minargs<<"\t maxargs:"<<maxargs<<endl;
  if ( (operands.size() <= maxargs && operands.size() >= minargs) || maxargs == INFINITY_WIDTH)
    {
      ////cout<<"\nInferring as: "<<this->inferredType<<endl;
        vector <string> TypeFromStringWithPercentage = split(this->inferredType, "%", false);
        vector <string> TypeFromStringWithHash = split(this->inferredType, "#", false);
        if (TypeFromStringWithHash.size() == 1 && TypeFromStringWithPercentage.size() == 1)
        {
            TypeOfExpressionTuple *type = stringToTypeOfExpressionTupleWithNoComputations(inferredType);

	    // cast to bv to  boolean 
            return type;
        }
        else //Width is Computation or dependency on operands
        {
            string typeStringWithOutComputations = stringToTypeOfExpressionTupleWithComputations(inferredType, operands);
            ////cout<<typeStringWithOutComputations;
            TypeOfExpressionTuple *type = stringToTypeOfExpressionTupleWithNoComputations(typeStringWithOutComputations);
            return type;
        }
    }
    else
    {
        //cout << "Not able to infer the type as number of arguments in the operator rule\n";
        return NULL;
    }
}

/**
 * Inference type is stored in the string form to add computations 
 * Replace the computations according to the operands
 * It is a recursive procedure for replacing the computations
 */
string t_InferTypeOfExpression::stringToTypeOfExpressionTupleWithComputations(string inferString, vector <TypeOfExpressionTuple *>operands)
{
    string newTypeToInfer = inferString;
    vector<string> typeOfOperandInStrings = split(inferString, "@", false);
    /**
     * Check any Computations or replacements left??
     */
    if (typeOfOperandInStrings[1].find("#") == -1 && typeOfOperandInStrings[1].find("%") == -1)
    {
        return newTypeToInfer;
    }

    /**
     * Checking for the replacements of operand widths
     */
    //   //cout<<"\n Inferring .....:"<<newTypeToInfer;
    if (typeOfOperandInStrings[1].find("#") != -1)
    {
        vector <string> withDependStrings = split(typeOfOperandInStrings[1], "#", false);
	int k=0;

        for (int i = 0; i < withDependStrings.size(); i++)
	  {
	    istringstream iStream(withDependStrings[i]);
	    //cout<<"\n"<<withDependStrings[i];
	    int val;	    
	    if(iStream >> val)
	      {
		int replaceLength;
		if (i == withDependStrings.size() - 1)
		  {
		    vector <string> spl = split(withDependStrings[i], ")", false);
		    replaceLength = 1 + spl[0].length();
		  }
		else
		  {
		    replaceLength = (toString(val)).length() + 1;
		  }
		// //cout<<endl<<operands[val-1]->width <<endl;
		newTypeToInfer.replace(newTypeToInfer.find("#" + withDependStrings[i]), 
				       replaceLength, toString(operands[val - 1]->m_width));
	      }
	  }
        //cout << "New inference: " << newTypeToInfer << endl;
    }

        /**
         * Now there are no replacements of operand widths
         * Check for the computations and perform the computations on operand widths and
         * replace them in newTypeToInfer
         */
    else if (typeOfOperandInStrings[1].find("%") != -1)
    {
        ////cout<<"Computing";
        vector <string> braces = split(newTypeToInfer, ")");
        ////cout<<braces[0];
        string firstComputation = newTypeToInfer.substr(newTypeToInfer.find_last_of('%'), braces[0].length() - newTypeToInfer.find_last_of('%') + 1);
        ////cout << endl << "First Computes:" << firstComputation;
        if (firstComputation.find("max") != -1)
        {
            vector <string> opts1 = split(firstComputation, "(", true);
            ////cout<<opts1.size();
            vector <string> opts2 = split(opts1[1], ",", true);
            ////cout<<opts2.size();
            istringstream iStream2(opts2[0]);
            int w1;
            iStream2 >> w1;
            istringstream iStream3(opts2[1]);
            int w2;
            iStream3 >> w2;
            // //cout<<endl<<w1<<endl<<w2;
            int newW = (w1 > w2) ? w1 : w2;
            // //cout << "\nReplaces :"<<newTypeToInfer.substr(newTypeToInfer.find_last_of('%'),newTypeToInfer.find(")"))<<endl;
            newTypeToInfer.replace(newTypeToInfer.find_last_of('%'), firstComputation.length(), toString(newW));
            //   //cout<<newTypeToInfer<<endl;
        }
        else if (firstComputation.find("min") != -1)
        {
            vector <string> opts1 = split(firstComputation, "(", true);
            ////cout<<opts1.size();
            vector <string> opts2 = split(opts1[1], ",", true);
            ////cout<<opts2.size();
            istringstream iStream2(opts2[0]);
            int w1;
            iStream2 >> w1;
            istringstream iStream3(opts2[1]);
            int w2;
            iStream3 >> w2;
            // //cout<<endl<<w1<<endl<<w2;
            int newW = (w1 < w2) ? w1 : w2;
            newTypeToInfer.replace(newTypeToInfer.find_last_of('%'), firstComputation.length(), toString(newW));
            ////cout<<newTypeToInfer<<endl;
        }
        else if (firstComputation.find("addall") != -1)
        {
            int result = 0;
            for (int i = 0; i < operands.size(); i++)
            {
                result = result + operands[i]->m_width;
            }
            //cout << "\n Result" << result;
            newTypeToInfer.replace(newTypeToInfer.find_last_of('%'), firstComputation.length(), toString(result));
        }
        else if (firstComputation.find("add") != -1)
        {
            vector <string> opts1 = split(firstComputation, "(", false);
            ////cout<<opts1.size();
            vector <string> opts2 = split(opts1[1], ",", false);
            ////cout<<opts2.size();
            istringstream iStream2(opts2[0]);
            int w1;
            iStream2 >> w1;
            istringstream iStream3(opts2[1]);
            int w2;
            iStream3 >> w2;
            ////cout<<endl<<w1<<endl<<w2;
            int newW = w1 + w2;
            newTypeToInfer.replace(newTypeToInfer.find_last_of('%'), firstComputation.length(), toString(newW));
            ////cout<<newTypeToInfer<<endl;
        }
        else
        {
            //Unknown computation
            //cout << "TypeInferencing:An Invalid Computation reached" << endl;
            return NULL;
        }
    }
    return stringToTypeOfExpressionTupleWithComputations(newTypeToInfer, operands);
}

/**
 * Inference is stored in the form of a string 
 * If string does not have any computations and replacements
 * Then it generates the TypeOfExpressionTuple based on argument passed
 */
TypeOfExpressionTuple * t_InferTypeOfExpression::stringToTypeOfExpressionTupleWithNoComputations(string typeAsString)
{
    TypeOfExpressionTuple *type1 = new TypeOfExpressionTuple();
    string operand1 = typeAsString;
    vector<string> typeOfOperand1InStrings = split(operand1, "@", false);
    istringstream iStream1(typeOfOperand1InStrings[0]);
    int type;
    iStream1 >> type;
    type1->m_type = (t_TypeOfExpression) type;
    int found = typeAsString.find("#");
    if (found > typeAsString.length())
    {
        istringstream iStream2(typeOfOperand1InStrings[1]);
        iStream2 >> type1->m_width;
        return type1;
    }
    else
    {
        type1->m_width = VARIABLE_WIDTH;
        return type1;
    }
}

string t_InferTypeOfExpression::parsePrintStringOfOneOperator(map <string, t_LableTypeAndValueforPrinting *> &replaceMap, string printString, int maxargs)
{
    string replaceString;
    bool labelReplacement = true;
    bool nameReplacement = false;
    int replacePoint = printString.find("%");
    int i = maxargs;
    
     
    while (replacePoint < printString.length())
    {
        //Only label replacements implemented here...
        /**
         * TODO  To be implemented for Values
         */
      ////cout<<"\n"<<printString<<endl;
        if (i > 0 && labelReplacement)
        {
            replaceString = "%label" + toString(i);
            if (replaceMap.find(replaceString) != replaceMap.end())
            {
                //replaces only %labeli s
                ////cout << endl << replaceString;

                replacePoint = printString.find(replaceString);
                t_LableTypeAndValueforPrinting *typeValue = replaceMap[replaceString];
                string replaceWith = typeValue->label;
                printString.replace(replacePoint, replaceString.length(), replaceWith);
		////cout <<endl<< printString << endl;
                string replaceWidthString = "%" + toString(i);

                if (printString.find(replaceString) < printString.length())
                {
                    //i value should remain same for next %labeli replacement
                }
                else
                {
                    i = i - 1;
                }

            }
            else
            {
                //cout << "No replacement found for symbol :" << replaceString << endl;
                return printString;
            }
        }
        else if (!nameReplacement)
        {
            if (labelReplacement)
            {
                // Just label replacements are completed
                // So make label replacements to false
                labelReplacement = false;
                i = maxargs;
            }

            replaceString = "%label" + toString(i);
            if (replaceMap.find(replaceString) != replaceMap.end())
            {
                //replaces only %i s (Widths)
                string replaceWidthString = "%" + toString(i);
                if (printString.find(replaceWidthString) < printString.length())
                {
                    t_LableTypeAndValueforPrinting *typeValue = replaceMap[replaceString];
                    string replaceWith = toString((typeValue->typeInfo).m_width);
                    printString.replace(printString.find(replaceWidthString), replaceWidthString.length(), replaceWith);
                }
                else
                {
                    // %i is not found so try next replacement 
                    i--;
                }
            }
            else if (i <= 0 && !nameReplacement)
            {
                //i<=0 i.e., %i replacements are completed
                // So make name replacements to start
                nameReplacement = true;
                i = 0;
            }
            else
            {
                //cout << "No replacement found for symbol :" << replaceString << endl;
                return printString;
            }
        }
        else if (i == 0)
        {
            replaceString = "%name";
            replacePoint = printString.find(replaceString);
            if (replacePoint < printString.length())
            {
                if (replaceMap.find(replaceString) != replaceMap.end())
                {
                    //replaces only %name
                    t_LableTypeAndValueforPrinting *typeValue = replaceMap[replaceString];
                    string replaceWith = typeValue->label;
                    printString.replace(replacePoint, replaceString.length(), replaceWith);
                    i = 0;
                }
                else
                {
                    //cout << "No replacement found for symbol :" << replaceString << endl;
                    return printString;
                }

            }
            else if (printString.find("%infer") < printString.length())
            {
                if (replaceMap.find(replaceString) != replaceMap.end())
                {
                    //replaces only %infer
                    t_LableTypeAndValueforPrinting *typeValue = replaceMap[replaceString];
                    string replaceWith = toString((typeValue->typeInfo).m_width);
                    string replaceWord = "%infer";
                    printString.replace(printString.find("%infer"), replaceWord.length(), replaceWith);
                }
                else
                {
                    //cout << "No replacement found for symbol :" << replaceString << endl;
                    return printString;
                }

            }
            else
            {
                break;
            }
        }
        else
        {
            string replaceString = "%label" + toString(i--);
            if (replaceMap.find(replaceString) != replaceMap.end())
            {
                replacePoint = printString.find(replaceString);
                t_LableTypeAndValueforPrinting *typeValue = replaceMap[replaceString];
                string replaceWith = typeValue->label;
                printString.replace(replacePoint, replaceString.length(), replaceWith);
            }
            else
            {
                //cout << "No replacement found for symbol :" << replaceString << endl;
                return printString;
            }
        }
        replacePoint = printString.find("%");
    }
    return printString;
}

string t_InferTypeOfExpression::makePrintStringIntoActualFormat(string &printBuffer)
{
    vector<string> buffers = split(printBuffer, ";", false);
    int i;
    string rString;
    string resultBuffer;
    //replacing $once and $every
    for (i = 0; i < buffers.size(); i++)
    {
        rString = "$every";
        int epos = buffers[i].find(rString);
        if (epos < buffers[i].length())
        {
            buffers[i].replace(epos, rString.length(), "");

        }
        else if (buffers[i].find("$once") < buffers[i].length())
        {
            ////cout<<buffers[i]<<endl;
            int j = i + 1;
            bool erasedIthElement = false;
            while (j < buffers.size())
            {
                ////cout<<buffers[i]<<" : "<<buffers[j]<<endl;
                if (buffers[j] == buffers[i])
                {
                    if (j != buffers.size() - 1)
                    {
                        vector<string>::iterator p = buffers.begin();
                        p += j;
                        //                        //cout << "Erasing:" << (*p) << endl;
                        buffers.erase(p, p + 1);
                    }
                    else
                    {
                        vector<string>::iterator p = buffers.begin();
                        p += i;
                        buffers.erase(p, p + 1);
                        i = i - 1;
                        erasedIthElement = true;
                        break;
                    }
                }
                else
                {
                    j++;
                }
            }
            if (!erasedIthElement)
            {
                rString = "$once";
                buffers[i].replace(buffers[i].find("$once"), rString.length(), "");
            }
        }
    }
    //replace "" (double quotes)
    rString = "\"";
    for (i = 0; i < buffers.size(); i++)
    {
        while (buffers[i].find(rString) < buffers[i].length())
        {
            buffers[i].replace(buffers[i].find(rString), rString.length(), "");
        }
    }
    resultBuffer = "";
    for (i = 0; i < buffers.size(); i++)
    {
        resultBuffer += (buffers[i] + "\n");
    }
    return resultBuffer;
}

string t_InferTypeOfExpression::parseUnlimitedNumberOfOperandsPrintString(string &printBuffer, vector<TypeOfExpressionTuple *> &operandsVector)
{
    string newPrintBuffer = "";
    string printall = "$printall";
    vector<string> buffers = split(printBuffer, ";");
    int nOperands = operandsVector.size();
    for (int j = 0; j < buffers.size(); j++)
    {
        int printallPosition = buffers[j].find(printall);
        int repeatIthContext = buffers[j].find("$i{");
        if (printallPosition < buffers[j].length())
        {
            string beforePrintall = buffers[j].substr(0, printallPosition);
            string repeatString = buffers[j].substr(printallPosition + printall.length(), buffers[j].length());
            string labeli = "%labeli";
            int labeliPosition = repeatString.find(labeli);
            if (labeliPosition < repeatString.length())
            {
                for (int i = 1; i < nOperands + 1; i++)
                {
                    string ithString = beforePrintall;
                    ithString += repeatString.substr(0, labeliPosition);
                    ithString += ("%label" + toString(i));
                    ithString += repeatString.substr(labeliPosition + labeli.length(), repeatString.length());
                    string ithWidth = "%i";
                    if (ithString.find(ithWidth) < ithString.length())
                    {
                        ithString.replace(ithString.find(ithWidth), ithWidth.length(), "%" + toString(i));
                    }
                    newPrintBuffer += (ithString + ";");
                }
            }
        }
        else if (repeatIthContext < buffers[j].length())
        {
            string beforeIthContext = buffers[j].substr(0, repeatIthContext);

            string ith = "$i{";
            int closingIthContext = buffers[j].find("}");

            string repeatString = buffers[j].substr(repeatIthContext + ith.length(), closingIthContext - repeatIthContext - ith.length());

            string afterIthContext = buffers[j].substr(closingIthContext + 1, buffers[j].length() - closingIthContext);
            string iPlusOneString = "%labeli+1";
            string ithString = "%labeli";
            int iPlusOne = repeatString.find(iPlusOneString);
            int ithPos = repeatString.find(ithString);
            string ithContext = repeatString;
            int i;
            for (i = 1; i < nOperands - 1; i++)
            {
                ithContext.replace(ithPos, ithString.length(), "%label" + toString(i));
                ithContext.replace(iPlusOne, iPlusOneString.length(), repeatString);
                ithPos = ithContext.find(ithString);
                iPlusOne = ithContext.find(iPlusOneString);
            }
            ithContext.replace(ithPos, ithString.length(), "%label" + toString(i));
            ithContext.replace(iPlusOne, iPlusOneString.length(), "%label" + toString(i + 1));
            newPrintBuffer += (((beforeIthContext + ithContext) + afterIthContext) + ";");
        }
        else
        {
            newPrintBuffer += (buffers[j] + ";");
        }
    }
    return newPrintBuffer;
}

/**
 * Set methods
 */

bool t_InferTypeOfExpression::setOperatorSymbol(string operatorSymbol)
{
    this->operatorSymbol = operatorSymbol;
    return true;
}

bool t_InferTypeOfExpression::setMinargs(int minargs)
{
    this->minargs = minargs;
    return true;
}

bool t_InferTypeOfExpression::setMaxargs(int maxargs)
{
    this->maxargs = maxargs;
    return true;
}

bool t_InferTypeOfExpression::setOperand1Type(TypeOfExpressionTuple *type)
{
    this->operand1Type = type;
    return true;
}

bool t_InferTypeOfExpression::setOperand2Type(TypeOfExpressionTuple *type)
{
    this->operand2Type = type;
    return true;
}

bool t_InferTypeOfExpression::setOperand3Type(TypeOfExpressionTuple *type)
{
    this->operand3Type = type;
    return true;
}

bool t_InferTypeOfExpression::setOtherOperandsType(TypeOfExpressionTuple *type)
{
    this->otherOperandsType = type;
    return true;
}

bool t_InferTypeOfExpression::setVariableNumberOfArguments(bool haveVariableArguments)
{
    this->variableNumberOfArguments = haveVariableArguments;
    return true;
}

bool t_InferTypeOfExpression::setUnlimitedNumberOfArguments(bool unlimitedArguments)
{
    this->unlimitedNumberOfArguments = unlimitedArguments;
    return true;
}

bool t_InferTypeOfExpression::setInferredType(string inferString)
{
    this->inferredType = inferString;
    return true;
}
bool t_InferTypeOfExpression::setPrintExpressionFormat(string printFormat)
{
    this->printExpressionFormat = printFormat;
    return true;
}

/**
 * Get Methods
 */
string t_InferTypeOfExpression::getOperatorSymbol()
{
    return this->operatorSymbol;
}

int t_InferTypeOfExpression::getMinargs()
{
    return this->minargs;
}

int t_InferTypeOfExpression::getMaxargs()
{
    return this->maxargs;
}

TypeOfExpressionTuple * t_InferTypeOfExpression::getOperand1Type()
{
    return this->operand1Type;
}

TypeOfExpressionTuple *t_InferTypeOfExpression::getOperand2Type()
{
    return this->operand2Type;
}

TypeOfExpressionTuple * t_InferTypeOfExpression::getOperand3Type()
{
    return this->operand3Type;
}

TypeOfExpressionTuple *t_InferTypeOfExpression::getOtherOperandsType()
{
    return this->otherOperandsType;
}

bool t_InferTypeOfExpression::getVariableNumberOfArguments()
{
    return this->variableNumberOfArguments;
}

bool t_InferTypeOfExpression::getUnlimitedNumberOfArguments()
{
    return this->unlimitedNumberOfArguments;
}

string t_InferTypeOfExpression::getInferredType()
{
    return this->inferredType;
}

string t_InferTypeOfExpression::getPrintExpressionFormat()
{
    return this->printExpressionFormat;
}
