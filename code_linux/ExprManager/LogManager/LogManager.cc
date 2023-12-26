#include "LogManager.h"
//t_Logger* t_Logger::m_logManagerObject = NULL;

t_Logger::t_Logger(t_VerbosityLevel verbo_mode)
{
//    if (this->m_logManagerObject != NULL)
//    {
//        cout << " ERROR ! Trying to create multiple Log managers" << endl;
//        return;
//    }
    cout << "Log Manager Object is created" << endl;
    this->m_current_verbosity = verbo_mode;
    this->m_logger_count = 0;
    //this->m_logManagerObject = this;
}
t_Logger::~t_Logger()
{
    cout << "Number of Log entries made totally:" << m_logger_count << endl;
    cout << "\n LogManager Object is deleted \n\n" << endl;
    
    //delete this->m_logManagerObject;
    //this->m_logManagerObject = NULL;
}
void t_Logger::removeLogManager()
{
    cout<<"Not removing Logmanager. Auto free later"<<endl;
}
t_VerbosityLevel t_Logger::getVerbosityLevel()
{
    return this->m_current_verbosity;
}

t_Logger* t_Logger::getInstance()
{
//    if (m_logManagerObject == NULL)
//    {
//        //m_logManagerObject = new t_Logger(c_RunLevelVerbosity);
//        m_logManagerObject = new t_Logger((t_VerbosityLevel) t_ConfigurationOptions::getConfig()->m_VerbosityLevel);
//    }
//    return m_logManagerObject;
    static t_Logger logManagerObject((t_VerbosityLevel) t_ConfigurationOptions::getConfig()->m_VerbosityLevel);
    return &logManagerObject;
}
