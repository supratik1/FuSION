#include <cstdlib>
#include "helping_functions.h"
#include <iostream>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

const string UNKNOWN_MEM_SYMBOL("InternalUnknownMem$");
bool useMalloc = false;

string applySuffix(const string& name, const char* suffix, int ub, int lb, bool isOneBit, int instance_num)
{
    stringstream name_stream;
    name_stream << name << "_" << ub << "_" << lb << suffix;
    if (isOneBit)
    {
        name_stream << "_0_0";
    }
    else
    {
        name_stream << "_" << ub << "_" << lb;
    }

    if (instance_num != -1)
    {
        name_stream << "_" << instance_num;
    }
    return name_stream.str();
}

//using namespace std;

/**
   Synopsis: [A helper that allows translation of strings representing integer values to numeric integers]
   Description: [This function uses C's atoi for conversion. Returns 0 for empty string]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[Valid string representing integer like "1234"]
   PostCondition: [Ensures correct conversion of valid input]
 */
int stringToInteger(string s)
{
    //returns 0 if the string is empty
    //Put a check to see whether input string is valid, i.e. does not contain alphabet chars
    for (int i = 0; i < s.size(); i++)
    {
        if (isalpha(s[i]))
        {
            //cerr<<"Invalid string in stringToInteger:"<<s<<endl;
            return -1;
        }
    }
    return atoi(s.c_str());
}

/**
   Synopsis: [A helper that allows translation of binary strings to integers]
   Description: [The function reads every character in binary string (LSB on right) and depending on its position 'i' and value 'v', computes v*(2^i)]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[Valid binary string]
   PostCondition: [Ensures correct conversion of valid input, exits on invalid string]
 */
bool startsWith(const string& str, const string& find)
{
    if(str.length() < find.length())
        return false;
    for(int i=0;i<find.length();++i)
    {
        if(str[i]!=find[i])
            return false;
    }
    return true;
}

bool startsWith(const string& str, const char* find)
{
    for(int i=0; find[i]; ++i)
    {
        if(i>= str.length() || str[i] != find[i])
            return false;
    }
    return true;
}

string charpAsString(const char* p){return string(p);}

long int binaryStringToInteger(string str)
{
    //return binaryStringToUL(str);
    int length = str.size();
    assert(length<=(sizeof(long)*8));
    long int num = 0;
    long int base = 1;
    for (int i = length - 1; i >= 0; i--)
    {
        if ((str[i] != '1') && (str[i] != '0'))
        {
            throw std::runtime_error("Unable to convert binary string to integer: " + str);
            //exit(-1);
        }
        if (str[i] == '1')
        {
            num = num + base;
        }
        base = base * 2;
    }
    return num;


}

unsigned getBitsRequired(unsigned long val)
{
    if(val == 0) return 1;
    unsigned bits=0;
    while(val)
    {
        bits++;
        val = val>>1;
    }
    return bits;
}

unsigned long binaryStringToUL(const string &str)
{
    unsigned long num=0;
    int len = str.length();
    //assert(len<=(sizeof(unsigned long)*8));
    static const unsigned long mask = 1ul<<((sizeof(unsigned long)*8)-1); //MSB
    for(int i=0;i<len;++i)
    {
        if(num & mask)
            throw std::runtime_error("Loss of precision.");
        if(str[i] == '1')
            num = (num<<1)|1;
        else if(str[i] == '0')
            num = num<<1;
        else
            throw std::runtime_error("Unable to convert binary string to integer: " + str);
    }
    return num;
}

string& convertInstNameToSSName(string &str, bool skipModuleName)
{

    if (skipModuleName)
    {
        string::size_type idx = str.find(PATH_SEPARATOR_STR);
        if (idx != string::npos)
        {
            string endPart = str.substr(idx + 1);
            replace(endPart, PATH_SEPARATOR_STR, SLASH_STR);
            string moduleName = str.substr(0, idx);
            str = moduleName + PATH_SEPARATOR_STR + endPart;
         
        }
        

    }
    else
    {
        replace(str, PATH_SEPARATOR_STR, SLASH_STR);
    }
    replace(str, "[", OPENBRAC_STR);
    replace(str, "]", CLOSEBRAC_STR);

    return str;
}

string createNewNameForDuplicateSignal(const string& sigName, int num, const string& prefix, const string& suffix)
{
    string::size_type idx = sigName.rfind(PATH_SEPARATOR);
    if (idx == string::npos)
    {
        return prefix + "$$$" + integerToString(num) + "_" + sigName + suffix;
    }
    else
    {
        return prefix + sigName.substr(0, idx + 1) + "$$$" + integerToString(num) + "_" + sigName.substr(idx + 1) + suffix;
    }
}

std::string removeNewNamePrefixFromSignal(const std::string& sigName, int &num)
{
    std::string::size_type idx1 = sigName.find("$$$");
    if (idx1 == std::string::npos)
        return sigName;
    std::string::size_type idx2 = sigName.find('_', idx1);
    if (idx2 == std::string::npos)
        return sigName;
    if (num == -1)
    {
        std::string numStr = sigName.substr(idx1 + 3, idx2 - idx1 - 3);
        num = stringToInteger(numStr);
    }
    //sigName.erase(sigName.begin()+idx1, sigName.begin()+idx2+1);
    return sigName.substr(0, idx1) + sigName.substr(idx2 + 1);
}








/**
   Synopsis: [A helper that allows translation of special integer strings possible in verilog to integers]
   Description: [The function reads every character in decimal string and depending on its position 'i' and value 'v', computes v*(10^i)]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[Valid integer string]
   PostCondition: [Ensures correct conversion of valid input, exits on invalid string]
 */
/*int convertToInteger(string str)
{
  int ret_val = 0;

  if(str.empty())
    {
      cout << "ERROR!!! Attempting to convert an empty string to integer in helping_functions.cc" << endl;
      ret_val = -1;
    }
  else
    for(int i=0;i<=str.length()-1;i++)
      {
        if(str[i]=='-')
          continue;
        if(isalpha(str[i]))
          {
           cerr<<"Unable to convert a string containing non-digit character to integer: "<<str<<endl;
           exit(-1);
          }
        ret_val = ret_val*10 + convertToDigit(str[i]);
      }
  return ret_val;
}
 */
/**
   Synopsis: [A helper that allows translation of decimal character to integers]
   Description: [The function reads a decimal character ('0' to '9') and returns its decimal value]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[Valid decimal character]
   PostCondition: [Ensures correct conversion of valid input, exits on invalid input]
 */

/*int convertToDigit(char ch)
{
  int ret_val;
  switch(ch)
    {
    case '0' : {
      ret_val = 0;
      break;
    }
    case '1' : {
      ret_val = 1;
      break;
    }
    case '2' : {
      ret_val = 2;
      break;
    }
    case '3' : {
      ret_val = 3;
      break;
    }
    case '4' : {
      ret_val = 4;
      break;
    }
    case '5' : {
      ret_val = 5;
      break;
    }
    case '6' : {
      ret_val = 6;
      break;
    }
    case '7' : {
      ret_val = 7;
      break;
    }
    case '8' : {
      ret_val = 8;
      break;
    }
    case '9' : {
      ret_val = 9;
      break;
    }
    default : {
      cerr << "ERROR!!! Non-digit input given to convertToDigit:  " << ch<<endl;
      ret_val = 0; // Kept as a safety. To be removed. This case should not arise at all.
    }
    }
  return ret_val;
}
 */


string extractNumberAtEnd(const string input, int& num)
{
    string::size_type index = input.rfind('_');
    if (index == string::npos)
    {
        return input;
    }
    string numStr = input.substr(index + 1, input.size() - index - 1);

    num = stringToInteger(numStr);
    if (num == -1)
    {
        throw std::logic_error("_<number> expected in " + input);
        //return input;
    }
    return input.substr(0, index);
}

/**
   Synopsis: [A helper that returns binary string of an integer]
   Description: [Implements general algorithm for converting a integer to binary]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[Integer >=0]
   PostCondition: [Ensures correct conversion of valid input, exits on invalid input]
 */

string integerToBinaryString(int i)
{
    if (i < 0)
        cerr << "ERROR!! Cannot convert " << i << " to binary " << endl;
    if (i == 0)
        return "0";
    string bin_str = "";
    int j = i;
    while (j != 0)
    {
        if (j % 2 == 0)
            bin_str = bin_str + "0";
        else
            bin_str = bin_str + "1";
        j = j / 2;
    }
    string str = "";
    //Reversing the binary string to bring it to final format
    for (int k = bin_str.size() - 1; k >= 0; k--)
        str += bin_str[k];
    return str;
}

string hexStringToBinaryString(string str)
{
    string bin = "";
    while (str != "")
    {
        string hex = str.substr(str.length() - 1, 1);
        if (hex == "0")
            bin = "0000" + bin;
        else if (hex == "1")
            bin = "0001" + bin;
        else if (hex == "2")
            bin = "0010" + bin;
        else if (hex == "3")
            bin = "0011" + bin;
        else if (hex == "4")
            bin = "0100" + bin;
        else if (hex == "5")
            bin = "0101" + bin;
        else if (hex == "6")
            bin = "0110" + bin;
        else if (hex == "7")
            bin = "0111" + bin;
        else if (hex == "8")
            bin = "1000" + bin;
        else if (hex == "9")
            bin = "1001" + bin;
        else if ((hex == "a") || (hex == "A"))
            bin = "1010" + bin;
        else if ((hex == "b") || (hex == "B"))
            bin = "1011" + bin;
        else if ((hex == "c") || (hex == "C"))
            bin = "1100" + bin;
        else if ((hex == "d") || (hex == "D"))
            bin = "1101" + bin;
        else if ((hex == "e") || (hex == "E"))
            bin = "1110" + bin;
        else if ((hex == "f") || (hex == "F"))
            bin = "1111" + bin;
        else
            cout << "ERROR!!! Input not in hex format...in hexStringToBinaryString" << endl;

        str = str.substr(0, str.length() - 1);
    }
    return bin;
}

string integerToString(int i) //converts 123 to "123"
{
    //return toString(i);
    //char* number = new char[10];
    static char number[25];
    if (i == 0)
        return "0";
    int pos = 0;
    string ret_val;
    int j = i;
    while (j != 0)
    {
        switch (j % 10)
        {
            case 0:
            {
                number[pos++] = '0';
                break;
            }
            case 1:
            {
                number[pos++] = '1';
                break;
            }
            case 2:
            {
                number[pos++] = '2';
                break;
            }
            case 3:
            {
                number[pos++] = '3';
                break;
            }
            case 4:
            {
                number[pos++] = '4';
                break;
            }
            case 5:
            {
                number[pos++] = '5';
                break;
            }
            case 6:
            {
                number[pos++] = '6';
                break;
            }
            case 7:
            {
                number[pos++] = '7';
                break;
            }
            case 8:
            {
                number[pos++] = '8';
                break;
            }
            case 9:
            {
                number[pos++] = '9';
                break;
            }
        }
        j = j / 10;
    }
    number[pos++] = '\0';
    ret_val = string(number);
    reverse(ret_val.begin(), ret_val.end());
    //delete[] number;
    return ret_val;
}

vector<string> split(const string& s, const string& delim, const bool keep_empty)
{
    vector<string> result;
    if (delim.empty())
    {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;

    while (true)
    {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty())
        {
            result.push_back(temp);
        }
        if (subend == s.end())
        {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}

float stringToFloat(const string &float_representation_of_number)
{
    float result;
    istringstream ss(float_representation_of_number);
    ss >> result;
    if (!ss)
    {
        throw std::invalid_argument("Read a bad value in stringToFloat :" + float_representation_of_number + "\n");
    }
    return result;
}

bool stringToBoolean(const string &boolean_string)
{
    if (boolean_string == "false")
        return false;
    else if (boolean_string == "true")
        return true;
    else if (boolean_string == "0")
        return false;
    else if (boolean_string == "1")
        return true;
    else if (boolean_string == "0b0")
        return false;
    else if (boolean_string == "0b1")
        return true;
    else
        throw std::invalid_argument("Read a bad value in stringToBoolean:" + boolean_string + "\n");
}

int stringToInteger(const string &s)
{
    istringstream ss(s);
    int result;
    ss >> result;
    if (!ss)
    {
        throw std::invalid_argument("Read a bad value in stringToInteger :" + s + "\n");
    }
    return result;
}

bool isABinaryNumberString(string binaryString)
{
    // cout<<"Binary String is:"<<binaryString<<endl;
    int length = binaryString.size();
    long int num = 0;
    long int base = 1;
    // static int count = 0;
    //cout<<count++;
    //    if(length == 0)
    //    {
    //        return false;
    //    }
    for (int i = length - 1; i >= 0; i--)
    {
        if ((binaryString[i] != '1') && (binaryString[i] != '0') && (binaryString[i] != 'x') && binaryString[i] != 'z')
        {
            return false;
        }
    }
    return true;
}

/**
 * It will replace the part of string  source before delim by desReplace if the flag is true else it will replace string at delim at the last 
 * by desReplace
 * 
 * @param source
 * @param desReplace
 * @param delim
 * @return 
 */
string replaceStringAtDelimOnce(const string& source, const string& desReplace, const string& delim, bool firstOrLast)
{
    int index;
    string res;
    if (firstOrLast == true)
    {
        index = (int) source.find(delim);

        if (index != string::npos)
        {

            res = desReplace + source.substr(index + delim.length());
            return res;
        }
        //cout<<"returning... "<<source<<endl;
        return source;
    }

    else
    {

        index = (int) source.rfind(delim);
        cout << "delim is:" << delim << endl;
        int delim_length = delim.size();

        if (index != string::npos && index + delim_length == source.size())
        {
            cout << source << "::";
            res = source.substr(0, index) + desReplace;
            cout << res << endl;
            return res;
        }
        return source; //cout<<"returnings...."<<source<<endl;
    }

}

string& trimInPlace(string&str)
{
    const static string spaces(" \t\n\r"); //Strings that are considered as space characters

    string::size_type pos = str.find_last_not_of(spaces); //find the last non space character
    if (pos != string::npos)
    {
        str.erase(pos + 1); //erase the last non space characters
        pos = str.find_first_not_of(spaces); //find the first non space character
        //This pos cannot be npos as we know that there is a non space character from the first if condition
        str.erase(0, pos); //erase the spaces.
    }
    else
    {
        //str.erase(str.begin(),str.end());//entire string is space characters.
        str.clear();
    }
    return str;
}

string extractStringAtEnd(const string& input, const char separator)
{
    string::size_type idx = input.rfind(separator);
    if (idx != string::npos)
    {
        return input.substr(idx + 1);
    }
    return input;
}

string& replace(string& str, const string& repl, const string& with)
{
    string::size_type idx = str.find(repl);
    while (idx != string::npos)
    {
        str.replace(idx, repl.length(), with);
        idx = str.find(repl, idx + with.length());
    }
    return str;
}

#ifdef QE //#ifdef added by Ajith John on 9 June 2013
/**
   The following function is added by Ajith John
   Synopsis: [A helper that returns binary string of an integer with zeros to pad to correct width]
   SideEffects:[]
*/

string integerToBinaryStringWithZeroPadding(unsigned long long int i, int width)
{
    if(i==0)
	{
	string ret_str="";
    	for(int i=0; i<width; i++)
      		ret_str += "0";
 	return ret_str;
	}

    string bin_str="";
    
    int j=i;
    while(j!=0)
    {
        if(j%2 == 0)
            bin_str = bin_str + "0";
        else
            bin_str = bin_str + "1";
        j = j / 2;
    }
    string str="";
    //Reversing the binary string to bring it to final format
    for(int k=bin_str.size()-1;k>=0;k--)
        str += bin_str[k];

    int NoOfZeros = width - str.length();
    
    string ret_str="";
    for(int i=0; i<NoOfZeros; i++)
      ret_str += "0";
 
    ret_str += str;
    return ret_str;
}


/* The following function is added by Ajith John */
// return zero string of given length
string getZeroBinaryStringOfGivenLength(unsigned int length)
{
	string result = "";

	for(int i = 0; i < length; i++)
	{
		result += "0";
	}

	return result;
}


/* The following function is added by Ajith John */
// return max string of given length
string getMaxBinaryStringOfGivenLength(unsigned int length)
{
	string result = "";

	for(int i = 0; i < length; i++)
	{
		result += "1";
	}

	return result;
}


/* The following function is added by Ajith John */
// return one string of given length
string getOneBinaryStringOfGivenLength(unsigned int length)
{
	string result = "";

	for(int i = 0; i < length-1; i++)
	{
		result += "0";
	}

	result += "1";

	return result;
}


/* The following function is added by Ajith John */
// return true if argument is zero, false otherwise
bool isZeroConstant(string &mystring)
{
	if(mystring.find_first_of("1") == string::npos) // no 1's; all are zeros
	{
		return true;
	}
	else
	{
		return false;
	}
}


/* The following function is added by Ajith John */
// return true if argument is all 1's, false otherwise
bool isMaximumConstant(string &mystring)
{
	if(mystring.find_first_of("0") == string::npos) // no 0's; all are 1's
	{
		return true;
	}
	else
	{
		return false;
	}
}
		


/**
   Synopsis: [Variant of binaryStringToInteger which returns answer as unsigned long long int]
   Description: [The function reads every character in binary string (LSB on right) and depending on its position 'i' and value 'v', computes v*(2^i)]
   SideEffects:[]
   SeeAlso:[binaryStringToInteger]
   Arguments:[Valid binary string]
   PostCondition: [Ensures correct conversion of valid input, exits on invalid string]
 */


unsigned long long int binaryStringToULLInteger(string str)
{
    unsigned length = str.size();
    unsigned long long int num = 0;
    unsigned long long int base = 1;

    for (int i = length - 1; i >= 0; i--)
    {
	if ((str[i] != '1') && (str[i] != '0'))
        {
            cerr << "Unable to convert binary string to integer in binaryStringToULLInteger: " << str << endl;
            exit(-1);
        }
        if (str[i] == '1')
        {
            num = num + base;
        }
        base = base * 2;
    }


    return num;
}
#endif //#endif added by Ajith John on 9 June 2013

//----------------------------------- MEMORY INFO -----------------------------------------------

map<string, t_MemoryArrayBoundsInfo *> m_memVarNameToMemVarInfoMap; //made static since data added here by CDFG.
t_MemoryVariableInfoVector m_allMemoryVariablesInDesign;

t_MemoryArrayBoundsInfo* getMemoryVariableDimensions(string memoryVariableName)
{   
    /* Begin Commented by Supratik: Nov 27, 2012
    vector<t_MemoryArrayBoundsInfo*>::iterator it = m_allMemoryVariablesInDesign.begin();
    while (it != m_allMemoryVariablesInDesign.end())
    {
        t_MemoryArrayBoundsInfo* currentEntry = (*it);
        if (currentEntry->m_variable_name == memoryVariableName)
        {
            return currentEntry;
        }
        it++;
    }
    return NULL;
    * End Commented by Supratik: Nov 27, 2012 */
    
    // Begin Added by Supratik: Nov 27, 2012
    int num = 0;
    memoryVariableName = removeNewNamePrefixFromSignal(memoryVariableName,num);
    map <string, t_MemoryArrayBoundsInfo *>::iterator m_it = ::m_memVarNameToMemVarInfoMap.find(memoryVariableName);
    if (m_it != m_memVarNameToMemVarInfoMap.end()) {
        return m_it->second;
    }
    else {
        return NULL;
    }
    // End Added by Supratik: Nov 27, 2012
}

bool addMemoryBoundsInfoToMemoryVariableInfo(t_MemoryArrayBoundsInfo* mem_bounds, bool performSLASHReplacement)
    {
        if(performSLASHReplacement)
            convertInstNameToSSName(mem_bounds->m_variable_name,true);
#define VERIFY_BOUNDS
#ifdef VERIFY_BOUNDS
        vector<t_MemoryBounds> bounds = mem_bounds->m_bounds;
        for(int i=0;i<bounds.size();++i)
        {
            int wd = bounds[i].m_upperbound - bounds[i].m_lowerbound + 1;
            if(wd < 1)
                throw std::out_of_range(string("Memory bounds are negative for memory") + mem_bounds->getVariableName());
        }
#endif
        m_allMemoryVariablesInDesign.push_back(mem_bounds);
        // Begin Added by Supratik: Nov 27, 2012
        ::m_memVarNameToMemVarInfoMap.insert(make_pair(mem_bounds->m_variable_name, mem_bounds));
        // End Added by Supratik: Nov 27, 2012
        return true;
    }
