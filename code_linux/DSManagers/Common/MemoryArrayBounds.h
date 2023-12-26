
/*
 * File:   MemoryArrayBounds.h
 * Author: rajkumar
 *
 * Created on 20 January, 2011, 4:41 PM
 */

#include<vector>
#include<string>
#include<algorithm>
#include<fstream>

using namespace std;

#ifndef MEMORYARRAYBOUNDS_H
#define MEMORYARRAYBOUNDS_H

typedef struct t_memoryBounds
{
    int  m_lowerbound;
    int  m_upperbound;
    bool m_packed;
/**    t_memoryBounds(t_memoryBounds& mem)
    {
        this->m_lowerbound=mem.m_lowerbound;
        this->m_upperbound=mem.m_upperbound;
        this->m_packed=mem.m_packed;
    }
    bool operator= (const t_memoryBounds& mem)
    {
        this->m_lowerbound=mem.m_lowerbound;
        this->m_upperbound=mem.m_upperbound;
        this->m_packed=mem.m_packed;
    }
    t_memoryBounds()
    {
        
    } */
} t_MemoryBounds;


class t_MemoryArrayBoundsInfo
{
    public:
        vector<t_MemoryBounds> m_bounds;
        string                 m_variable_name;
        string m_variable_ss_name;
        int                    m_nDimensions;
        int                    m_linearised_lowerbound;
        int                    m_linearised_upperbound;
        int m_nBaseTypeWidth;

        t_MemoryArrayBoundsInfo()
        {
        }
        
        void setVariableName(const string& var_name,const string& ss_var_name)
        {
            m_variable_name=var_name;
            m_variable_ss_name=ss_var_name;
        }
        
        string getVariableName()
        {
            return m_variable_name;
        }
        bool addBoundsForNminus1thDimension(t_MemoryBounds memBounds)
        {
            m_bounds.push_back(memBounds);
            return true;
        }
        vector<t_MemoryBounds> getBoundsOfNthDimension(int dimension)
        {
            // Form a vector of dimensions from nth level
            vector<t_MemoryBounds>::iterator it = m_bounds.begin();
            int                              i  = 0;

            while ((it != m_bounds.end()) && (dimension > i))
            {
                it++;
                i++;
            }

            vector<t_MemoryBounds> boundsOfNthDimension;

            copy(it, m_bounds.end(), boundsOfNthDimension.begin());

            return boundsOfNthDimension;
        }
        void printMemoryEntryInXMLFormat(ofstream& fout)
        {
            fout << "<MEMORY>" << endl;
          fout << "VARIABLENAME=" << m_variable_name << endl;
          fout << "LINEAR-LOWERBOUND=" << m_linearised_lowerbound << endl;
          fout << "LINEAR-UPPERBOUND=" << m_linearised_upperbound << endl;
          fout << "NDIMENSIONS=" << m_nDimensions<<endl;
          fout << "NBASETYPEBITS="<< m_nBaseTypeWidth<<endl;
          fout << "V_DIMENSIONS=" << endl;
          fout << "<VECTOR>" << endl;
          vector<t_MemoryBounds>::iterator it=m_bounds.begin();
          while(it!=m_bounds.end())
          {
              fout << "<DIMENSION>" <<endl;
              fout <<"PACKED="<< (*it).m_packed << endl;
              fout <<"LOWERBOUND="<< (*it).m_lowerbound << endl;
              fout <<"UPPERBOUND="<< (*it).m_upperbound << endl;
              fout << "</DIMENSION>" << endl;
              it++;
          }
          fout << "</VECTOR>"<<endl;
          fout << "</MEMORY>" << endl;
        }
        t_MemoryArrayBoundsInfo(t_MemoryArrayBoundsInfo*& toCopy)
        {
            m_variable_name=toCopy->m_variable_name;
            m_variable_ss_name=toCopy->m_variable_ss_name;
            m_nDimensions=toCopy->m_nDimensions;
            m_linearised_lowerbound=toCopy->m_linearised_lowerbound;
            m_linearised_upperbound=toCopy->m_linearised_upperbound;
            m_nBaseTypeWidth = toCopy->m_nBaseTypeWidth;
            m_bounds=toCopy->m_bounds;
        }
};

#endif  /* MEMORYARRAYBOUNDS_H */

