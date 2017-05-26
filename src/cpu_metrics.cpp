/*
The MIT License

Copyright (c) 2017-Present, ROBERT HOWELL

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

#include <iostream>
#include <fstream>
#include <array>

#include "cpu_metrics.h"

namespace cpu_metrics 
{

// Global static pointer used to ensure a single instance of the class.
CpuMetrics* CpuMetrics::m_pInstance = NULL; 

const char* CpuMetrics::m_pColumnHeaders[] = { 
    "<button onclick='sortTable(this,0,1)'>Uid</button>", 
    "<button onclick='sortTable(this,1,0)'>Function</button>", 
    "<button onclick='sortTable(this,2,0)'>Script</button>", 
    "<button onclick='sortTable(this,3,1)'>Hits</button>", 
    "<button onclick='sortTable(this,4,1)'>Lines</button>", 
    "<button onclick='sortTable(this,5,0)'>De-opted</button>", 
    "<button onclick='sortTable(this,6,0)'>Bailed</button>", 
    NULL };


// Returns the singleton Object. 
CpuMetrics* CpuMetrics::GetInstance()
{
   // create the singlton instance on first call only    
   if (!m_pInstance)   
      m_pInstance = new CpuMetrics;

   return m_pInstance;
}

CpuMetrics::CpuMetrics()
{
    m_pIsolate = v8::Isolate::GetCurrent();
    m_pCpuProfiler = m_pIsolate->GetCpuProfiler();	
    
    m_pCpuMetricsTable = new CpuMetricsTable("CPU Metrics", m_pColumnHeaders);

    m_nodeUid = 0;
    m_sampleRate = 100;
    m_pCpuProfiler->SetSamplingInterval(m_sampleRate);
}

CpuMetrics::~CpuMetrics()
{
    m_pCpuProfiler->StopProfiling(v8::String::NewFromUtf8(m_pIsolate, "CPU Metrics"));
}

void CpuMetrics::StartProfiling(const v8::FunctionCallbackInfo<v8::Value> & args)
{

    m_pCpuProfiler->StartProfiling(v8::String::NewFromUtf8(m_pIsolate, "CPU Metrics"), true);

    args.GetReturnValue().Set(v8::Boolean::New(m_pIsolate, true));
}


void CpuMetrics::StopProfiling(const v8::FunctionCallbackInfo<v8::Value> & args)
{

    v8::CpuProfile * pCpuProfile = m_pCpuProfiler->StopProfiling(v8::String::NewFromUtf8(m_pIsolate, "CPU Metrics"));
    
    //v8::Local<v8::Value> filterValue = args.Data();
    //v8::Local<v8::String> filterString = filterValue.ToString(m_pIsolate);


    // ------------------------------------------------------------------------------------------
    // assemble the metrics table content - start with the table caption - 
    
    std::stringstream tableCaption;
    tableCaption << pCpuProfile->GetSamplesCount() << " samples collected over "
        << static_cast<int>((pCpuProfile->GetEndTime()-pCpuProfile->GetStartTime())/1000)
        << " ms with a sample rate of " << m_sampleRate << " us";
    m_pCpuMetricsTable->AddTableCaption(tableCaption.str().c_str());
    
    ProcessNode(pCpuProfile->GetTopDownRoot(), 0);

    // ------------------------------------------------------------------------------------------
    // Date and Timestamp for Dump
    
    std::array<char, 64> buffer;
    buffer.fill(0);
    time_t rawtime;
    time(&rawtime);
    const auto timeinfo = localtime(&rawtime);
    strftime(buffer.data(), sizeof(buffer), "%m-%d-%Y : %H-%M-%S", timeinfo);
    std::string timeStr(buffer.data());

    // ------------------------------------------------------------------------------------------
    // Assemble the pathspecs for the HTML and CSV files. 

    const char* HOME;
    if ((HOME = getenv("HOME")) == NULL)
    {
        args.GetReturnValue().Set(v8::String::NewFromUtf8(m_pIsolate, "HOME == NULL"));
    }
    
    std::stringstream htmlPath, csvPath;
    htmlPath << HOME << "/bin/cpu-metrics.html";
    csvPath << HOME << "/bin/cpu-metrics.csv";


    // ------------------------------------------------------------------------------------------
    // Assemble and stream the HTML content to file

    std::fstream mdFile;
    mdFile.open(htmlPath.str().c_str(), std::fstream::out);

    mdFile << "<article>\n";
    mdFile << "  </header>\n";
    mdFile <<    m_pCpuMetricsTable->GetTable().c_str();    
    mdFile <<    m_pCpuMetricsTable->GetScript().c_str();    
    mdFile << "<article>\n";

    mdFile.close();    

    
    // return the metrics
    args.GetReturnValue().Set(v8::Boolean::New(m_pIsolate, true));

    
}

void CpuMetrics::ProcessNode(const v8::CpuProfileNode * pCpuProfileNode, int depth)
{
    // increment the unique node id for all nodes, regardless of filter
    m_nodeUid++;
    
    // Get the script name with absolute path for this node, and convert to Utf8 string
    v8::Local< v8::String > v8strScriptName = pCpuProfileNode->GetScriptResourceName();
    v8::String::Utf8Value utf8ScriptName(v8strScriptName);

    // inatialize a Script Name stream object with the Utf8Value (* operator)
    std::stringstream ssScriptName;
    ssScriptName << *utf8ScriptName;

    if (ssScriptName.str().find("core") != std::string::npos)
    {
        v8::Local< v8::String > v8strFunctionName = pCpuProfileNode->GetFunctionName();
        v8::String::Utf8Value utf8FunctionName(v8strFunctionName);

        std::stringstream ssUid, ssFunctionName, ssHitCount, ssLineCount;

        for (int i=0; i<depth; i++) { ssFunctionName << ". "; }

        ssUid << m_nodeUid;
        ssFunctionName << *utf8FunctionName;
        ssHitCount << pCpuProfileNode->GetHitCount();
        ssLineCount << pCpuProfileNode->GetHitLineCount();
    
        std::string bailoutReasonStr = pCpuProfileNode->GetBailoutReason(); 
        std::string bailed;
        if (bailoutReasonStr.compare("no reason") == 0)
        {
            bailed = "No";
        }
        else
        {
            bailed =  "Yes";
            ssScriptName << "<pre> - bailed: " << bailoutReasonStr << "</pre>";
        }
        
        const std::vector<v8::CpuProfileDeoptInfo>& deoptInfos = pCpuProfileNode->GetDeoptInfos(); 
        std::string deopted = (deoptInfos.size() == 0) ? "No" : "Yes";
    
        std::vector<std::string> rowData = 
        { 
            ssUid.str(), 
            ssFunctionName.str(), 
            ssScriptName.str(), 
            ssHitCount.str(),
            ssLineCount.str(),
            deopted,
            bailed
        }; 
    
        m_pCpuMetricsTable->AddTableRow(rowData);
        
        depth++;
    }
    
    for (int i=0; i<pCpuProfileNode->GetChildrenCount(); i++)
    {
        ProcessNode(pCpuProfileNode->GetChild(i), depth );
    }

}


// ---------------------------------------------------------------------------------------
// Wrapper functions

void InitCpuMetrics()
{
    // First call to GetInstance() will instatiate the singlton object
	CpuMetrics::GetInstance();
}

void StartProfiling(const v8::FunctionCallbackInfo<v8::Value> & args)
{
	CpuMetrics::GetInstance()->StartProfiling( args );
}

void StopProfiling(const v8::FunctionCallbackInfo<v8::Value> & args)
{
	CpuMetrics::GetInstance()->StopProfiling( args );
}

} // namespace cpu_metrics