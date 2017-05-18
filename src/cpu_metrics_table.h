#ifndef __CPU_METRICS_TABLE_H__
#define __CPU_METRICS_TABLE_H__

/*
The MIT License

Copyright (c) 2017-Present ROBERT HOWELL

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

namespace cpu_metrics 
{
    
class CpuMetricsTable
{
public:
    std::stringstream m_tableSyle;
    std::stringstream m_tableStart;
    std::stringstream m_tableCaption;
    std::stringstream m_tableFooter;
    std::stringstream m_tableColumnHeadings;
    std::stringstream m_tableRows;
    std::stringstream m_tableEnd;
    std::stringstream m_script;
    std::stringstream m_tableStyle;


public:
    CpuMetricsTable(const char* tableCaption, const char ** columnHeadings)
    { 
        m_tableStyle << "  <style> th { border-bottom: 1px solid #e9e9e9; } tr:nth-child(even) { background: #fafafa; } </style>\n";
        
        m_tableStart   << "  <table style='font-family:arial;' style='font-size:85%;' cellpadding='2;' id='cpu_metrics_table'>\n";
        m_tableCaption << "    <caption><p><strong>" << tableCaption << "</strong></p></caption>\n"; 
        m_tableEnd     << "  </table>\n";

        int col = 0;
        
        const char** currentHeading = columnHeadings;
        
        m_tableColumnHeadings << "  <tr>\n";
        
        while (*currentHeading) 
        {
            if (col < 3)
            {
                m_tableColumnHeadings << "    <th align='left'>" << *currentHeading << "</th>\n";
            } 
            else
            {
                m_tableColumnHeadings << "    <th align='right'>" << *currentHeading << "</th>\n";
            }
            currentHeading++;
            col++;
        }        
        
        m_tableColumnHeadings << "  </tr>\n";
        
        // one time assembly of the sort function script
        assembleScript();
    }

    ~CpuMetricsTable();

public:

    void AddTableCaption(const char* tableCaption)
    {
        m_tableCaption << "  <caption>" << tableCaption << "</caption>\n"; 
        
    }
    
    void AddTableRow(std::vector<std::string>& tableRowData)
    {
        m_tableRows << "  <tr>\n";
        
        int col = 0;

        for (std::vector<std::string>::iterator itor = tableRowData.begin() ; itor != tableRowData.end(); itor++)
        {
            if (col < 3) {
                m_tableRows << "    <td style='font-size:75%;' align='left'>" << itor->c_str() <<"</td>\n";
            } 
            else
            {
                m_tableRows << "    <td style='font-size:75%;' align='right'>" << itor->c_str() <<"</td>\n";
            }    
            col++;
        }        

        m_tableRows << "  </tr>\n";
        
    }
    std::string GetTable()
    {
        std::stringstream m_htmlTtable;
    
        m_htmlTtable << m_tableStyle.str() << m_tableStart.str() << m_tableCaption.str() << m_tableColumnHeadings.str() << m_tableRows.str() << m_tableEnd.str();

        return m_htmlTtable.str();
    } 
    
    std::string GetScript()
    {
        return m_script.str();
    }

private: 
    void assembleScript()
    {
        m_script << "  <script>\n";
        m_script << "    var ascending = true;\n";
        m_script << "    var currentColumn = -1;\n";
        m_script << "    function sortTable(column, isNumber) {\n";
        m_script << "      if (column == currentColumn) {\n";
        m_script << "          ascending = !ascending;\n";
        m_script << "      } else {\n";
        m_script << "          currentColumn = column;\n";
        m_script << "          ascending = true;\n";
        m_script << "      }\n";
        m_script << "      var table = document.getElementById('cpu_metrics_table');\n";
        m_script << "      var switching = true;\n";
        m_script << "      while (switching) {\n";
        m_script << "        switching = false;\n";
        m_script << "        var rows = table.getElementsByTagName('TR');\n";
        m_script << "        for (var i = 1; i < (rows.length - 1); i++) {\n";
        m_script << "          var shouldSwitch = false;\n";
        m_script << "          var x = rows[i].getElementsByTagName('TD')[column].innerHTML.toLowerCase();\n";
        m_script << "          var y = rows[i + 1].getElementsByTagName('TD')[column].innerHTML.toLowerCase();\n";
        m_script << "          x = x.replace(/. /g, '');\n";
        m_script << "          y = y.replace(/. /g, '');\n";
        m_script << "          x = (isNumber) ? parseInt(x) : x;\n";
        m_script << "          y = (isNumber) ? parseInt(y) : y;\n";
        m_script << "          if (ascending ? x>y : y>x) {\n";
        m_script << "            shouldSwitch= true;\n";
        m_script << "            break;\n";
        m_script << "          }\n";
        m_script << "        }\n";
        m_script << "        if (shouldSwitch) {\n";
        m_script << "          rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);\n";
        m_script << "          switching = true;\n";
        m_script << "        }\n";
        m_script << "      }\n";
        m_script << "    }\n";
        m_script << "  </script>\n";
    }
};

};  // namespace cpu_metrics

#endif // __CPU_METRICS_TABLE_H__
