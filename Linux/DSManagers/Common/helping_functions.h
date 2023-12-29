#ifndef HELPING_FUNCTIONS_H
#define HELPING_FUNCTIONS_H
#include "MemoryArrayBounds.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <cctype>
#include <locale>
#include <cassert>
#include <typeinfo>
#include <cstdio>
#include "../ConfigurationOptions/ConfigurationOptions.h"
using namespace std;

#define CHECKED

#ifndef WEAKEN_SUFFIX
#define WEAKEN_SUFFIX "_specifiedAsWeak"
#define ANTE_DRIVEN_SUFFIX "_drivenByAnte"
#define ANTE_VALUE_SUFFIX "_anteValue"
#define INV_ANTE_SUFFIX "_invAnte"

/*#define NAME_WEAKEN(name) ((name)+WEAKEN_SUFFIX+"_0_0")
#define NAME_ANTECEDENT_DRIVEN(name) ((name)+ANTE_DRIVEN_SUFFIX+"_0_0")
#define NAME_ANTECEDENT_VALUE(name, ub,lb) ((name)+ANTE_VALUE_SUFFIX+"_"+(ub)+"_"+(lb))
#define NAME_ANTECEDENT_INVALID(name) ((name)+INV_ANTE_SUFFIX+"_0_0")
 */



string applySuffix(const string& name, const char* suffix, int ub, int lb, bool isOneBit, int instance_num = -1);

inline string NAME_WEAKEN(const string &name, int ub, int lb, int instance_num = -1) {
    return applySuffix(name, WEAKEN_SUFFIX, ub, lb, true, instance_num);
}

inline string NAME_ANTECEDENT_DRIVEN(const string &name, int ub, int lb, int instance_num = -1) {
    return applySuffix(name, ANTE_DRIVEN_SUFFIX, ub, lb, true, instance_num);
}

inline string NAME_ANTECEDENT_VALUE(const string &name, int ub, int lb, int instance_num = -1) {
    return applySuffix(name, ANTE_VALUE_SUFFIX, ub, lb, false, instance_num);
}

inline string NAME_ANTECEDENT_INVALID(const string &name, int ub, int lb, int instance_num = -1) {
    return applySuffix(name, INV_ANTE_SUFFIX, ub, lb, true, instance_num);
}

#endif

#define EXPRESSION_LABEL_AT_N_PREFIX "N@"
#define EXPRESSION_AT_CYCLE_PREFIX "C@"
#define internal_signals_prefix "__P__"

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR (t_ConfigurationOptions::getConfig()->m_pathSeparator[0])
#define PATH_SEPARATOR_STR (t_ConfigurationOptions::getConfig()->m_pathSeparator)
#endif

#define OPENBRAC_STR "_OPEN"
#define CLOSEBRAC_STR "_CLOSE"
#define SLASH_STR "_SLASH"

#define INTEGER_WIDTH 32
extern const string UNKNOWN_MEM_SYMBOL; //defined in .cc

class Timer {
private:
    clock_t startTime;
    string function;
    int line;
    string file;
    string message;
    ostream& out;
public:

    Timer(const string& fl, int ln, const string& func, ostream&o = clog) :
    startTime(clock()), file(fl), line(ln), function(func), message(""), out(o) {
    }

    Timer(const string& msg, ostream&o = clog) :
    startTime(clock()), file(""), line(0), function(""), message(msg), out(o) {
    }

    Timer(const string msg, const string& fl, int ln, const string& func, ostream&o = clog) :
    startTime(clock()), file(fl), line(ln), function(func), message(msg), out(o) {
    }

    ~Timer() {
        clock_t endTime = clock();
        double tm = (endTime - startTime) / 1000.0;
        if (message.empty() == false)
            out << message << ". ";
        out << "Execution";
        if (!function.empty())
            out << " of " << function;
        out << " took " << tm << "ms";
        if (!file.empty())
            out << " at " << line << " in " << file;
        out << endl;
    }
};

#define TIMER(name) Timer name##_timer(__FILE__, __LINE__, __FUNCTION__)
#define TIMER_MSG(name,msg) Timer name##_timer(msg, __FILE__,__LINE__,__FUNCTION__)
#define TIMER_MSG_NO_FILE(name,msg) Timer name##_timer(msg)

struct Counter {
    unsigned count;
    string text;

    Counter(const string &t, unsigned c = 0) : text(t), count(c) {
    }

    Counter(const char*t, unsigned c = 0) : text(t), count(c) {
    }

    ~Counter() {
        cout << text << " Counter:" << count << endl;
    }
};

#define MAKE_COUNTER(text) \
{  static Counter counter(text); \
   ++counter.count; \
}

#define MAKE_COUNTER_NAME(name, text) \
static Counter count_##name(text); ++count_##name.count;

template<typename T>
class RetValueChecker {
private:
    bool valueSeen;
    T val;
public:

    RetValueChecker(T v) : val(v), valueSeen(false) {
    }

    operator T() {
        valueSeen = true;
        return val;
    }

    ~RetValueChecker() {
        assert(valueSeen && "Value Not Used");
    }
};

#ifdef CHECKED
typedef RetValueChecker<int> CHECKED_INT;
typedef RetValueChecker<unsigned> CHECKED_UINT;
#else
typedef int CHECKED_INT;
typedef unsigned int CHECKED_UINT;
#endif

//void printNodeWidth(DataNode *data_node);
/*int convertToInteger(string str);
  int convertToDigit(char ch);*/
unsigned long binaryStringToUL(const string &str);
long int binaryStringToInteger(string str); // Note that this function is ill-named.
string extractNumberAtEnd(const string input, int& num);
string extractStringAtEnd(const string& input, const char separator = PATH_SEPARATOR);
string integerToBinaryString(int i);
string integerToString(int i);

unsigned getBitsRequired(unsigned long val);

inline string operator+(const string &s, int i) {
    return s + integerToString(i);
}

inline string operator+(int i, const string&s) {
    return integerToString(i) + s;
}
//inline string& operator+=(string&s, int i){ return s+=integerToString(i);}

static inline std::string &ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

static inline std::string &rtrim(string &s) {
    //s.erase(find_if(s.rbegin(),s.rend(), std::not1(std::ptr_fun<int,int>(std::isspace))), s.end());
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
    return s;
}

string createNewNameForDuplicateSignal(const string& sigName, int num, const string& prefix = "", const string& suffix = "");

string removeNewNamePrefixFromSignal(const string& sigName, int &num);

/**
 * removes space from lhs and rhs of original string 
 * @param s
 * @return 
 */
static inline std::string& trim(string& s) {
    return ltrim(rtrim(s));
}
string hexStringToBinaryString(string str);

int stringToInteger(string s);

float stringToFloat(const string &float_representation_of_number);
bool stringToBoolean(const string &boolean_string);

//splits a string into multiple strings based on given delimiters and empty spaces to handle
vector<string> split(const string& s, const string& delim, const bool keep_empty = true);

//template<>

inline string toString(void* t) {
    //  cout<<"Inside new toString"<<endl;
    MAKE_COUNTER(__PRETTY_FUNCTION__);
    static char buffer[50];
    sprintf(buffer, "%p", t);
    return string(buffer);
}

bool startsWith(const string& str, const string& find);
bool startsWith(const string& str, const char* find);
string charpAsString(const char* p);
inline const string& toString(const string& s) {
    return s;
}

/* Synopsis : [Convert basic types to string]
 *
 * Description : [
 * This is a templatized method for converting *basic types* to strings
 * Basic Types mean
 * int, string, long int, [unsinged][long][long] int, float, double
 * DO NOT PASS Objects to this method!!
 * Passing a object should give you a compile time error.
 * If you will pass a reference to a object, it will be treated as a unsinged int (pointer type)
 *]
 *
 *
 * Return Value : [string representation of the argument]
 */
template<class T>
inline string toString(const T& t) {
    stringstream s;
    if (s << t)
        return s.str();
    else
        return "";
}
bool isABinaryNumberString(string binaryString);

inline string getXOrZValueOfLength(int l, string character) {
    string t = "";
    for (int i = 0; i < l; i++) {
        t = t + character;
    }
    return t;
}
/**
 * It will replace the part of string  source before delim by desReplace if the flag is true else it will replace string at delim at the last 
 * by desReplace
 * @param source
 * @param desReplace
 * @param delim
 * @return 
 */
string replaceStringAtDelimOnce(const string& source, const string& desReplace, const string& delim, bool firstOrLast);

/**
 * Takes a string and trims it in place. Should be faster than string copy.
 */
string& trimInPlace(string&str);
/**
 * It will replace all occurences of repl with 'with'
 * @param str
 * @param repl
 * @param with
 * @return 
 */
string& replace(string& str, const string& repl, const string& with);

string& convertInstNameToSSName(string &str, bool skipModuleName =false);

inline string & convertSSNameToInstName(string &str) {
    replace(str, SLASH_STR, PATH_SEPARATOR_STR);
    replace(str, OPENBRAC_STR, "[");
    replace(str, CLOSEBRAC_STR, "]");
    return str;
}
extern bool useMalloc;

template<typename T>
class MyAllocator : public std::allocator<T> {
protected:
    typedef std::allocator<T> super;

    template<typename T1>
    class MemCounter {
    private:
	ofstream outfile;
    public:
        long nAlloc;
        long nDeAlloc;
        unsigned sz;

        MemCounter() : sz(sizeof (T1)), nAlloc(0), nDeAlloc(0) {
	    const string sFileName = 
		t_ConfigurationOptions::getConfig()->m_sTempInfoDir + "/Allocator/Allocations";
	    outfile.open(sFileName.c_str(), ios::app);
        }

        ~MemCounter() {

            cout << "Destroying MemCounter for " << typeid (T1).name() << endl;
            outfile << "MemCounter Statistics:" << typeid (T1).name() << endl;
            outfile << "Allocated:  " << nAlloc << "\n" <<
                    "Deallocated:" << nDeAlloc << "\n" <<
                    "------------------\n" <<
                    "Difference in bytes = " << (nAlloc - nDeAlloc) * sz << endl;
            outfile << endl;
        }
    };

    MemCounter<T>& getMC() {
        static MemCounter<T> mc;
        return mc;
    }
public:

    template<typename _Tp1>
    struct rebind {
        typedef MyAllocator<_Tp1> other;
    };

    typename super::pointer
    allocate(typename super::size_type __n, const void* __p = 0) {
        //cout<<"Allocating "<<__n<<endl;
        getMC().nAlloc += __n;
        if (useMalloc)
            return (typename super::pointer) malloc(sizeof (T) * __n);
        else
            return super::allocate(__n, __p);
    }

    void
    deallocate(typename super::pointer __p, typename super::size_type __n) {
        //cout<<"Deallocating "<<__n<<endl;
        getMC().nDeAlloc += __n;
        //super::deallocate(__p, __n);
    }

    MyAllocator() : super() {
        //cout<<"my allocator called"<<endl;
    }

};

template<typename T>
class myvector : public std::vector<T, MyAllocator<T> > {
protected:
    typedef std::vector<T, MyAllocator<T> > super;
public:

    explicit
    myvector(const typename super::allocator_type& __a = super::allocator_type()) :
    super(__a) {
    }

    explicit
    myvector(typename super::size_type __n, const typename super::value_type& __value = super::value_type(),
            const typename super::allocator_type& __a = super::allocator_type()) :
    super(__n, __value, __a) {
    }

    myvector(const super& __x) : super(__x) {
    }

    template<typename _InputIterator>
    myvector(_InputIterator __first, _InputIterator __last,
            const typename super::allocator_type& __a = super::allocator_type()) :
    super(__first, __last, __a) {
    }

    //    bool checker()
    //    {
    //        T t1;
    //        T t2;
    //        t1++;
    //        t2--;
    //        t1==t2;
    //        return true;
    //    }
    //    ~constructor() { assert(checker());}
};



#ifdef QE //#ifdef added by Ajith John on 9 June 2013
/* The following functions are added by Ajith John */

// A helper that returns binary string of an integer with zeros to pad to correct width
string integerToBinaryStringWithZeroPadding(unsigned long long int i, int width);

// return zero string of given length
string getZeroBinaryStringOfGivenLength(unsigned int length);

// return max string of given length
string getMaxBinaryStringOfGivenLength(unsigned int length);

// return one string of given length
string getOneBinaryStringOfGivenLength(unsigned int length);

// return true if argument is zero, false otherwise
bool isZeroConstant(string &mystring);

// return true if argument is all 1's, false otherwise
bool isMaximumConstant(string &mystring);

// Variant of binaryStringToInteger which returns answer as unsigned long long int
unsigned long long int binaryStringToULLInteger(string str);
#endif //#endif added by Ajith John on 9 June 2013

//------------------------------------ MEMORY INFO --------------------------------
class t_MemoryArrayBoundsInfo;
typedef vector<t_MemoryArrayBoundsInfo*> t_MemoryVariableInfoVector;
extern map<string, t_MemoryArrayBoundsInfo *> m_memVarNameToMemVarInfoMap; //made static since data added here by CDFG.
extern t_MemoryVariableInfoVector m_allMemoryVariablesInDesign;
    
t_MemoryArrayBoundsInfo* getMemoryVariableDimensions(string memoryVariableName);
inline bool isAMemoryVariable(const string &variableName)
{
    return m_memVarNameToMemVarInfoMap.find(variableName) != m_memVarNameToMemVarInfoMap.end();
}
    
bool addMemoryBoundsInfoToMemoryVariableInfo(t_MemoryArrayBoundsInfo* mem_bounds, bool performSLASHReplacement = true);

#endif
