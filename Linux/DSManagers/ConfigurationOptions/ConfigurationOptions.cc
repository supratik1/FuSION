#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string>
#include <vector>
#include <iosfwd>
#include <stdlib.h>
#include "ConfigurationOptions.h"
#include "helping_functions.h"

using namespace std;

t_ConfigurationOptions* t_ConfigurationOptions::config;

/**
 * default constructor
 */
t_ConfigurationOptions::t_ConfigurationOptions():
    m_dagNodeIdFile()
{
    init();
}

/**
 * Constructor for t_ConfigurationOptions
 * 
 * @param filename the configuration option filename
 */

t_ConfigurationOptions::t_ConfigurationOptions(string filename):
m_dagNodeIdFile()
{
    init();
    
    if(filename.empty())
    {
        config = this;
        cout << "No configuration provided." << endl;
        return;
    }
    /*
      Read the options specified in the configuration file
     */
    readOptionsFromConfigFile(filename);

    /*
      Check for errors
      Check if mandatory options were specified
     */
    if (m_sModuleName == "")
    {
        cerr << "Module Name not specified in the configuration file\n";
        exit(-1);
    }
    if (m_sCDFGFileName == "" && m_sRunFromStageName != "READRTL")
    {
        cerr << "CDFG File Name not specified in the configuration file\n";
        exit(-1);
    }
    if (m_sRunToStageName == "EVALANDCOMPARE")
    {
        if (m_sSignalsToObserveFileName == "")
        {
            cerr << "RunTo stage specified as EVALANDCOMPARE but no file specified for names of SignalsToObserve" << endl;
            exit(-1);
        }
    }
    if (m_VerbosityLevel == -1)
    {
        cout << "Verbosity Level is not specified, default Debug level is set as Verbosity Level" << endl;
        m_VerbosityLevel = 4;
    }
    config = this;
}

/**
 * Constructor for t_ConfigurationOptions
 * 
 * @param cmdline_options map containing user specified options as
 * option and their corresponding value as key-value pair
 */

t_ConfigurationOptions::t_ConfigurationOptions(map<string, string>& cmdline_options):
m_dagNodeIdFile()
{
    init();
    
    if(cmdline_options.empty())
    {
        config = this;
        cout << "No configuration provided." << endl;
        return;
    }
    
    // read the configuration options from the cmdline options
    readOptionsFromCmdline(cmdline_options);

    /*
      Check for errors
      Check if mandatory options were specified
     */
    if (m_sModuleName == "")
    {
        cerr << "Module Name not specified in the configuration file\n";
        exit(-1);
    }
    if (m_sCDFGFileName == "" && m_sRunFromStageName != "READRTL")
    {
        cerr << "CDFG File Name not specified in the configuration file\n";
        exit(-1);
    }
    if (m_sRunToStageName == "EVALANDCOMPARE")
    {
        if (m_sSignalsToObserveFileName == "")
        {
            cerr << "RunTo stage specified as EVALANDCOMPARE but no file specified for names of SignalsToObserve" << endl;
            exit(-1);
        }
    }
    if (m_VerbosityLevel == -1)
    {
        cout << "Verbosity Level is not specified, default Debug level is set as Verbosity Level" << endl;
        m_VerbosityLevel = 4;
    }
    config = this;
}

void t_ConfigurationOptions::init()
{
    // Setting the default values to configuration options  
    m_sModuleName = "";
    m_sCDFGFileName = "";
    m_sRunFromStageName = "READRTL";
    m_sRunToStageName = "GENERATEEXPRS";
    m_sSignalsToObserveFileName = "SignalsToObserve";


    m_VerbosityLevel = 0; // changed from -1
    m_nCycleCountForEvaluation = 0;
    m_fixed_input_simulation_flag = 0;
    m_fixed_input_simulation_inputs_file = "";
    m_restructuring_disable_flag = 0;
    m_sCDFGGen_arguments = "--design";
    m_allowCombinationalSignalsInCycle = true;
    m_generateNewInputs = true;
    m_bPerformWSTE = true;
    invBitGenerationLevel = 2;
    m_pathSeparator="/";
    m_extraConstraintsFile = "ExtraConstraintsFile";
    m_weakenFile="";
    m_pruningLevel = 2;
    m_extraSignalSTE = "extraSignalsSTE";
    m_lastDimensionBitVec = true;
    m_printDebugOutput = false; // changed from true
    m_checkAssertion=false;
    m_doBitBlasting=false;
    m_printFullExpression = false;
    m_convertSelectOnMemoryToReads=true;
    m_consequentExactMatch = false;
    m_performBMC = false;
    m_sSTEAntecedentFile = "AtomLevelSte";
    m_sSTEConsequentFile = "AtomLevelSte";
    m_sLogDir = "LogFiles";
    m_sTimeInfoDir = "TimeInfo";
    m_sTempInfoDir = "TempInfoFiles";
    m_antecedentSignalsFile = m_sTempInfoDir + "/WSTE/AntecedentSignalsFile";
    m_consequentSignalsFile = m_sTempInfoDir + "/WSTE/ConsequentSignalsFile";
    m_initialMarkedSignalsFile = m_sTempInfoDir + "/WSTE/InitialMarkedSignalsFile";
    m_bGenerateAtomInformationOnly = false;
    m_sUserInputValuesForPrimaryInputsFile = "UserInputValuesForPrimaryInputsFile";
    m_sUserInputValuesForStateSignalsFile = "UserInputValuesForStateSignalsFile";
}

/**
   Synopsis:[read configuration file]
   Description:[reads configuration file line by line and passes to readUserOptions for reading the option and its value]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[name of configuration file]
 */
void t_ConfigurationOptions::readOptionsFromConfigFile(string config_file_name)
{
    if(config_file_name.empty())
        return;
    ifstream * config_file;
    config_file = new ifstream();
    config_file->open(config_file_name.c_str());
    if (!config_file->is_open())
    {
        cerr << "Config File " << config_file_name << " cannot be opened!\n Is file missing?\n";
        exit(-1);
    }
    while (!config_file->eof())
    {
        string line;
        //cout<<"Reading a line \n";
        getline(*config_file, line);
        if (line.empty())
            continue;
        if (line[0] == ';')
            continue;

        bool result = readUserOption(line);
        if (result == false)
        {
            cerr << "Could not read the option specified on line: \n" << line << endl;
            exit(-1);
        }
    }
    config_file->close();
}

/**
   Synopsis:[Read a user option]
   Description:[Gets a string from config file, splits it and finds a command option and its value]
   SideEffects:[]
   SeeAlso:[]
   Arguments:[a line from command options file]
 */
bool t_ConfigurationOptions::readUserOption(string line)
{
    size_t index = line.find("=");
    if(line.find('#') == 0)
        return true;
    if (index == string::npos)
    {
        cerr << "Line " << line << " is not a valid configuration line. No \"=\" found.\n";
        return false;
    }
    string option = line.substr(0, index);
    string optionvalue = line.substr(index + 1, line.size() - index - 1);
    
#define DEFINE_OPTION(optionName,optionValue) if(option == #optionName) optionValue = optionvalue;
#define DEFINE_OPTION_BOOL(optionName,optionValue) if(option == #optionName) optionValue = stringToBoolean(optionvalue);
#define DEFINE_OPTION_INT(optionName,optionValue) if(option == #optionName) optionValue = stringToInteger(optionvalue);
    
    if (option == "MODULENAME")
    {
        m_sModuleName = optionvalue;
    }
    else if (option == "CDFGFILE")
    {
        m_sCDFGFileName = optionvalue;
    }
    else if (option == "CDFGGENARGUMENTS")
    {
        m_sCDFGGen_arguments = optionvalue;
#ifdef DEBUG
        cout<<"CDFGGEN Arguments are:"<<endl<<optionvalue<<endl<<"end of options"<<endl;
#endif
    }
    else if (option == "RUNFROM")
    {
        m_sRunFromStageName = optionvalue;
    }
    else if (option == "RUNTO")
    {
        m_sRunToStageName = optionvalue;
    }
    else if (option == "SIGNALSFILE")
    {
        m_sSignalsToObserveFileName = optionvalue;
    }
    else if (option == "VERBOSITY")
    {
        m_VerbosityLevel = stringToInteger(optionvalue);
    }
    else if (option == "CYCLES")
    {
        m_nCycleCountForEvaluation = stringToInteger(optionvalue);
    }
    else if (option == "FIXEDINPUT-FLAG")
    {
        m_fixed_input_simulation_flag = stringToInteger(optionvalue);
    }
    else if (option == "FIXEDINPUT-FILE")
    {
        m_fixed_input_simulation_inputs_file = optionvalue;
    }
    else if (option == "DISABLE-RECURSION-RESTRUCTURE")
    {
        m_restructuring_disable_flag = stringToInteger(optionvalue);
    }
    else if (option == "ALLOW-CYCLE-SIGNALS")
    {
        m_allowCombinationalSignalsInCycle = stringToBoolean(optionvalue);
    }
    else if(option == "GENERATE-NEW-INPUTS")
    {
        m_generateNewInputs = stringToBoolean(optionvalue);
    }
    else if( option == "STE-FILE-NAME")
    {
        m_sSTEFileName = optionvalue;
    }
    else if(option == "PERFORM-WSTE")
    {
        m_bPerformWSTE = stringToBoolean(optionvalue);
    }
    else if(option == "ADD-ATOMS")
    {
        m_CustomAtomIndicesFile = optionvalue;
    }
    else if(option == "PYTHON-CONVERT-FILE")
    {
        pythonConvertSTEFile = optionvalue;
    }
    else if(option == "INVALID-BIT-GENERATION-LEVEL")
    {
        invBitGenerationLevel = stringToInteger(optionvalue);
    }
    else if(option == "PATH-SEPARATOR")
    {
        m_pathSeparator = optionvalue;
    }
    else if(option == "WEAKEN-FILE")
    {
        m_weakenFile = optionvalue;
    }
    else if(option == "PRUNING-LEVEL")
    {
        m_pruningLevel = stringToInteger(optionvalue);
    }
    else if(option == "PRINT-DEBUG-OUTPUT")
    {
        m_printDebugOutput = (optionvalue == "true" || optionvalue == "TRUE");
    }
    else if(option == "EXTRA-USER-CONSTRAINTS")
    {
        m_extraConstraintsFile = optionvalue;
    }
    else if (option == "CHECK-ASSERTION")
    {
        m_checkAssertion = stringToBoolean(optionvalue);
    }
    else if(option == "LAST-DIMENSION-BITVEC")
    {
        m_lastDimensionBitVec = stringToBoolean(optionvalue);
    }
    else if(option == "DO-BIT-BLASTING")
    {
        m_doBitBlasting = stringToBoolean(optionvalue);
    }
    else if(option == "ANTECEDENT-FILE")
    {
        m_sSTEAntecedentFile = optionvalue;
    }
    else if(option == "CONSEQUENT-FILE")
    {
        m_sSTEConsequentFile = optionvalue;
    }
    else if(option ==  "SELECT-READ-MODIFICATION")
    {
        m_convertSelectOnMemoryToReads = stringToBoolean(optionvalue);
    }
    else if(option == "PRINT-FULL-EXPRESSION")
    {
        m_printFullExpression = stringToBoolean(optionvalue);
    }
    else if(option == "DAG-ID-FILE")
    {
        m_dagNodeIdFile = optionvalue;
    }
    else if(option == "CONSEQUENT-EXACT-MATCH")
    {
        m_consequentExactMatch = stringToBoolean(optionvalue);
    }
    else if(option == "PERFORM-BMC")
    {
        m_performBMC = stringToBoolean(optionvalue);
    }
    else if(option == "TEMP-INFO-DIRECTORY")
    {
        m_sTempInfoDir = optionvalue;
	m_antecedentSignalsFile = m_sTempInfoDir + "/WSTE/AntecedentSignalsFile";
	m_consequentSignalsFile = m_sTempInfoDir + "/WSTE/ConsequentSignalsFile";
    }
    else if(option == "LOG-DIRECTORY")
    {
        m_sLogDir = optionvalue;
    }
    else if(option == "TIME-INFO-DIRECTORY")
    {
        m_sTimeInfoDir = optionvalue;
    }
    else if(option == "USER-INPUTS-FOR-STATE-SIGNALS-FILE")
    {
        m_sUserInputValuesForStateSignalsFile = optionvalue;
    }
    else if(option == "USER-INPUTS-FOR-PRIMARY-INPUTS-FILE") 
    {
	m_sUserInputValuesForPrimaryInputsFile = optionvalue;
    }
    else if(option == "GENERATE-ATOM-INFO-ONLY") 
    {
	m_bGenerateAtomInformationOnly = stringToBoolean(optionvalue);
    }
    else if(option == "INITIAL-MARKED-SIGNAL-FILE") 
    {
	m_initialMarkedSignalsFile = optionvalue;
    }
    else
    {
        cerr << "Invalid option " << option << " in configuration file\n";
        cerr << "Cannot read line \"" << line << "\"" << endl;
        return false;
    }
#undef DEFINE_OPTION
#undef DEFINE_OPTION_BOOL
#undef DEFINE_OPTION_INT
    return true;
}

/**
 * method that reads the configuration options from cmdline args specified by user
 * when adding a new option add an entry here to make it available from cmdline
 */
void t_ConfigurationOptions::readOptionsFromCmdline(map<string, string>& cmdline_options)
{
    for (map<string, string>::iterator it=cmdline_options.begin();
            it != cmdline_options.end(); ++it)
    {
        string current_option=it->first;
        string optionvalue=it->second;
        
        if (current_option=="--module-name")
        {
            m_sModuleName = optionvalue;
        }
        else if (current_option=="--cdfg-file")
        {
            m_sCDFGFileName = optionvalue;
        }
        else if (current_option=="--cdfggenarguments")
        {
            m_sCDFGGen_arguments = optionvalue;
        }
        else if (current_option=="--run-from")
        {
            m_sRunFromStageName = optionvalue;
        }
        else if (current_option=="--run-to")
        {
            m_sRunToStageName = optionvalue;
        }
        else if (current_option=="--signals")
        {
            m_sSignalsToObserveFileName = optionvalue;
        }
        else if (current_option=="--verbosity")
        {
            m_VerbosityLevel = stringToInteger(optionvalue);
        }
        else if (current_option=="--cycles")
        {
            m_nCycleCountForEvaluation = stringToInteger(optionvalue);
        }
        else if (current_option=="--fixedinput-flag")
        {
             m_fixed_input_simulation_flag = stringToInteger(optionvalue);
        }
        else if (current_option=="--fixedinput-file")
        {
            m_fixed_input_simulation_inputs_file = optionvalue;
        }
        else if (current_option=="--disable-recursion-restructure")
        {
            m_restructuring_disable_flag = stringToInteger(optionvalue);
        }
        else if (current_option=="-allow-cycle-signals")
        {
            m_allowCombinationalSignalsInCycle = true;
        }
        else if (current_option=="-generate-new-inputs")
        {
            m_generateNewInputs = true;
        }
        else if (current_option=="--ste-file-name")
        {
            m_sSTEFileName = optionvalue;
        }
        else if (current_option=="-wste")
        {
            m_bPerformWSTE = true;
        }
        else if (current_option=="--add-atoms")
        {
             m_CustomAtomIndicesFile = optionvalue;
        }
        else if (current_option=="--python-convert-file")
        {
            pythonConvertSTEFile = optionvalue;
        }
        else if (current_option=="--invalid-bit-generation-level")
        {
            invBitGenerationLevel = stringToInteger(optionvalue);
        }
        else if (current_option=="--path-separator")
        {
            m_pathSeparator = optionvalue;
        }
        else if (current_option=="--weaken-file")
        {
            m_weakenFile = optionvalue;
        }
        else if (current_option=="--pruning-level")
        {
            m_pruningLevel = stringToInteger(optionvalue);
        }
        else if (current_option=="-print-debug-output")
        {
            m_printDebugOutput = true;
        }
        else if (current_option=="--extra-user-constraints")
        {
            m_extraConstraintsFile = optionvalue;
        }
        else if (current_option=="-check-assertion")
        {
            m_checkAssertion = true;
        }
        else if (current_option=="-last-dimension-bitvec")
        {
            m_lastDimensionBitVec = true;
        }
        else if (current_option=="-do-bit-blasting")
        {
            m_doBitBlasting = true;
        }
        else if (current_option=="--antecedent-file")
        {
            m_sSTEAntecedentFile = optionvalue;
        }
        else if (current_option=="--consequent-file")
        {
            m_sSTEConsequentFile = optionvalue;
        }
        else if (current_option=="-select-read-modification")
        {
            m_convertSelectOnMemoryToReads = true;
        }
        else if (current_option=="-print-full-expression")
        {
            m_printFullExpression = true;
        }
        else if (current_option=="--dag-id-file")
        {
            m_dagNodeIdFile = optionvalue;
        }
        else if (current_option=="-consequent-exact-match")
        {
            m_consequentExactMatch = true;
        }
        else if (current_option=="-bmc")
        {
            m_performBMC = true;
        }
	else if(current_option == "-temp-info-dir")  {
	    m_sTempInfoDir = optionvalue;
	    m_antecedentSignalsFile = m_sTempInfoDir + "/WSTE/AntecedentSignalsFile";
	    m_consequentSignalsFile = m_sTempInfoDir + "/WSTE/ConsequentSignalsFile";
	}
	else if(current_option == "-log-dir")    {
	    m_sLogDir = optionvalue;
	}
	else if(current_option == "-time-info-dir")  {
	    m_sTimeInfoDir = optionvalue;
	}
	else if(current_option == "-user-inp-for-state-signals-file") {
	    m_sUserInputValuesForStateSignalsFile = optionvalue;
	}
	else if(current_option == "-user-inp-for-pi-signals-file") {
	    m_sUserInputValuesForPrimaryInputsFile = optionvalue;
	}
	else if(current_option == "-generate-atom-info-only") {
	    m_bGenerateAtomInformationOnly = stringToBoolean(optionvalue);
	}
	else if(current_option == "-initial-marked-signals-file") {
	    m_initialMarkedSignalsFile = optionvalue;
	}
	else  {
	    cerr << "Invalid option " << current_option << " in command line\n";
	}
    }
}

/**
 * the method returns a map contains the cmdline options and their corresponding
 * descriptions as key and value respectively, useful for showing help and parsing
 * cmdline options. when adding a new option add an entry here to make it available 
 * from cmdline
 * 
 * @return the map containing cmdline options
 */
map<string, vector<string> >& t_ConfigurationOptions::get_cmdline_options()
{
    static map<string, vector<string> > options;
    
    string env_file = getenv ("HELPFILE");
    
    if (env_file.empty())
    {
        cout << "set environment variable for the help file named \"cmdline_options\" under ConfigurationOptions" << endl;
        exit(-3);
    }
    else {
        // read help cmdline options from the help-file
        string line;
        ifstream infile;
        infile.open (env_file.c_str());

        if (! infile.is_open())
        {
            cout << "cant open " << env_file << endl;
            exit(-4);
        }

        string comment = "#";
        while (getline(infile, line))
        {
            // if the current line is not a comment, then take ou
            if(line.substr(0, comment.size()) != comment && line.length() != 0)
            {
                string delimiter="?";
                size_t pos = 0;
                string key;
                vector<string> desc;
                while ((pos = line.find(delimiter)) != string::npos)
                {
                    key = line.substr(0, line.find(delimiter));
                    line.erase(0, pos + delimiter.length());
                }
                delimiter="\\";
                while ((pos = line.find(delimiter)) != string::npos)
                {
                    desc.push_back(line.substr(0, line.find(delimiter)));
                    line.erase(0, pos + delimiter.length());
                }
                desc.push_back(line.substr(0, line.find(delimiter)));
                options.insert(make_pair(key, desc));
            }
        }
        infile.close();
    }
    return options;
}

#ifdef OLD
t_ConfigurationOptions::t_ConfigurationOptions(t_ConfigurationOptions& copyOptions)
{
    m_sModuleName = copyOptions.m_sModuleName;
    m_sCDFGFileName = copyOptions.m_sCDFGFileName;
    m_sRunFromStageName = copyOptions.m_sRunFromStageName;
    m_sSTEFileName = copyOptions.m_sSTEFileName;
    m_sRunToStageName = copyOptions.m_sRunToStageName;
    m_VerbosityLevel = copyOptions.m_VerbosityLevel;
    m_nCycleCountForEvaluation = copyOptions.m_nCycleCountForEvaluation;
    m_sSignalsToObserveFileName = copyOptions.m_sSignalsToObserveFileName;
    m_fixed_input_simulation_flag = copyOptions.m_fixed_input_simulation_flag;
    m_fixed_input_simulation_inputs_file = copyOptions.m_fixed_input_simulation_inputs_file;
    m_restructuring_disable_flag = copyOptions.m_restructuring_disable_flag;
    m_sCDFGGen_arguments = copyOptions.m_sCDFGGen_arguments;
    m_allowCombinationalSignalsInCycle = copyOptions.m_allowCombinationalSignalsInCycle;
    m_bPerformWSTE = copyOptions.m_bPerformWSTE;
    m_CustomAtomIndicesFile = copyOptions.m_CustomAtomIndicesFile;
}
#endif
