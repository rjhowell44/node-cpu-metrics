#ifndef __HEAP_METRICS_TABLE_H__
#define __HEAP_METRICS_TABLE_H__

/*
The MIT License

Copyright (c) 2010-2017 Google, Inc. http://angularjs.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <sstream>

namespace heap_metrics 
{
    
class HeapMetricsTable
{
public:
    std::stringstream m_tableStart;
    std::stringstream m_tableCaption;
    std::stringstream m_tableFooter;
    std::stringstream m_tableColumnHeadings;
    std::stringstream m_tableRows;
    std::stringstream m_tableEnd;

public:
    HeapMetricsTable(const char* tableCaption, const char ** columnHeadings)
    { 
        m_tableStart   << "<table cellpadding='5'>\n";
        m_tableCaption << "  <caption><p><strong>" << tableCaption << "</strong></p></caption>\n"; 
        m_tableEnd     << "</table>\n";

        bool firstEntry = true;
        
        const char** currentHeading = columnHeadings;
        
        m_tableColumnHeadings << "  <tr>\n";
        
        while (*currentHeading) 
        {
            if (firstEntry)
            {
                m_tableColumnHeadings << "    <th align='left'>" << *currentHeading << "</th>\n";
                firstEntry = false;
            } 
            else
            {
                m_tableColumnHeadings << "    <th align='right'>" << *currentHeading << "</th>\n";
            }
            currentHeading++;
        }        
        
        m_tableColumnHeadings << "  </tr>\n";
    }

    ~HeapMetricsTable();

public:

    void AddTableCaption(const char* tableCaption)
    {
        m_tableCaption << "  <caption align='left'>" << tableCaption << "</caption>\n"; 
        
    }
    
    void AddTableRow(const char* tableRowHeading, std::vector<std::string>& tableRowData)
    {
        m_tableRows << "  <tr>\n";
        m_tableRows << "    <td align='left'>" << tableRowHeading << "</td>\n";


        for (std::vector<std::string>::iterator itor = tableRowData.begin() ; itor != tableRowData.end(); itor++)
        {
            m_tableRows << "    <td align='right'>" << itor->c_str() <<"</td>\n";
        }        

        m_tableRows << "  </tr>\n";
        
    }
    std::string GetTable()
    {
        std::stringstream m_htmlTtable;
    
        m_htmlTtable << m_tableStart.str() << m_tableCaption.str() << m_tableColumnHeadings.str() << m_tableRows.str() << m_tableEnd.str();

        return m_htmlTtable.str();
    } 

    
};

};  // namespace heap_metrics

#endif // __HEAP_METRICS_TABLE_H__
