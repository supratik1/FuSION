/*
 * File:   LogManager.h
 * Author: rajkumar
 *
 * Created on August 18, 2010, 4:46 PM
 */

#ifndef _LOGMANAGER_H
#define	_LOGMANAGER_H
#include <iostream>
#include <fstream>
#include <string>
#include "helping_functions.h"
using namespace std;

typedef enum t_verbosityLevel {
    c_DebugLevelVerbosity = 4,
    c_SignalTypeLoggingLevelVerbosity=3,
    c_RunLevelVerbosity = 2,
    c_RestructureCycleMessages=1,
    c_NoMessageLevelVerbosity = 0
} t_VerbosityLevel;

class t_Logger {
public:
    void LOG(string message, ostream *&logFile,t_VerbosityLevel current_message_level);
    void LOG(const char *message, ostream *&logFile,t_VerbosityLevel current_message_level);
    void LOG(string message, ofstream *&logFile,t_VerbosityLevel current_message_level);
    void LOG(const char *message, ofstream *&logFile,t_VerbosityLevel current_message_level);
    static t_Logger* getInstance();
    t_VerbosityLevel getVerbosityLevel();
    ~t_Logger();
    void removeLogManager();
protected:
    t_Logger(t_VerbosityLevel verbLevel);
private:
    long int m_logger_count;
    t_VerbosityLevel m_current_verbosity;
    //static t_Logger* m_logManagerObject;
};

inline void t_Logger::LOG(string message, ostream *&logFile, t_VerbosityLevel current_message_level)
{
     if (m_current_verbosity < current_message_level)
    {
        return;
    }
    else
    {
        *logFile << m_logger_count << " - ";
        *logFile << message << "\n";
        logFile->flush();
        m_logger_count++;
    }
}
inline void t_Logger::LOG(const char *message, ostream *&logFile,t_VerbosityLevel current_message_level)
{
    if (m_current_verbosity < current_message_level)
    {
        return;
    }
    else
    {
        *logFile << m_logger_count << " - ";
        *logFile << message << "\n";
        logFile->flush();
        m_logger_count++;
    }
}

inline void t_Logger::LOG(string message, ofstream *&logFile, t_VerbosityLevel current_message_level)
{
    if (!logFile->is_open())
    {
        return;
    }
    else if (m_current_verbosity < current_message_level)
    {
        return;
    }
    else
    {
        *logFile << m_logger_count << " - ";
        *logFile << message << "\n";
        //logFile->flush();
        m_logger_count++;
    }
}
inline void t_Logger::LOG(const char *message, ofstream *&logFile,t_VerbosityLevel current_message_level)
{
    if (!logFile->is_open())
    {
        return;
    }
    else if (m_current_verbosity < current_message_level)
    {
        return;
    }
    else
    {
        *logFile << m_logger_count << " - ";
        *logFile << message << "\n";
        //logFile->flush();
        m_logger_count++;
    }
}

#endif	/* _LOGMANAGER_H */
