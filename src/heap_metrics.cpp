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

#include <iostream>
#include <fstream>
#include <array>

#include "heap_metrics.h"

namespace heap_metrics 
{

// calback prototype
void UpdateHeapValuesGCC(v8::Isolate *isolate, v8::GCType type, v8::GCCallbackFlags flags);

// Global static pointer used to ensure a single instance of the class.
HeapMetrics* HeapMetrics::m_pInstance = NULL; 

const char* HeapMetrics::m_pColumnHeaders[] = { "Metric", "Size At Ctor", "Size At Peak", "Size At Last", NULL };


// Returns the singleton Object. 
HeapMetrics* HeapMetrics::GetInstance()
{
   // create the singlton instance on first call only    
   if (!m_pInstance)   
      m_pInstance = new HeapMetrics;

   return m_pInstance;
}

HeapMetrics::HeapMetrics()
{
    m_pIsolate = v8::Isolate::GetCurrent();
    m_pHeapProfiler = m_pIsolate->GetHeapProfiler();	
    
    m_pIsolate->AddGCPrologueCallback(UpdateHeapValuesGCC, v8::kGCTypeAll); 
    m_GCPrologueCallbackCalled = 0;

    m_pUsedHeapSize = new HeapMetricsType("Used Heap Size");
    m_pTotalHeapSize = new HeapMetricsType("Total Heap Size");
    m_pTotalPhysicalSize = new HeapMetricsType("Total Physical Size");
    
    m_pHeapProfiler->StartTrackingHeapObjects();    

    // Get the current heap statistics
    v8::HeapStatistics heapStatistics;
    m_pIsolate->GetHeapStatistics(&heapStatistics);
    
    m_pHeapMetricsTable = new HeapMetricsTable("Heap Metrics", m_pColumnHeaders);

    // Initialize the 'At Ctor' Heap Stats for each type.
    m_pUsedHeapSize->SetSizeAtCtor(heapStatistics.used_heap_size());
    m_pTotalHeapSize->SetSizeAtCtor(heapStatistics.total_heap_size());
    m_pTotalPhysicalSize->SetSizeAtCtor(heapStatistics.total_physical_size());
}

HeapMetrics::~HeapMetrics()
{
    m_pHeapProfiler->StopTrackingHeapObjects(); 

    if(m_pUsedHeapSize)
    {
        delete m_pUsedHeapSize;
    }

    if(m_pTotalHeapSize)
    {
        delete m_pUsedHeapSize;
    }

    if(m_pTotalPhysicalSize)
    {
        delete m_pTotalPhysicalSize;
    }

    if(m_pHeapMetricsTable)
    {
        delete m_pHeapMetricsTable;
    }
}


void HeapMetrics::DumpHeapMetrics(const v8::FunctionCallbackInfo<v8::Value> & args)
{
    v8::HeapStatistics heapStatistics;

    // Get the current heap statistics
    m_pIsolate->GetHeapStatistics(&heapStatistics);
    
    // Update the running peak heap, usage, and total physical sizes
    UpdateHeapMetrics();

    std::stringstream tableCaption;
    tableCaption << "Peak values checked over " << m_GCPrologueCallbackCalled << " GC Prologue Notifications \n";
    m_pHeapMetricsTable->AddTableCaption(tableCaption.str().c_str());
    
    // Add row data to the table
    std::vector<std::string> usedHeapRow = { m_pUsedHeapSize->GetSizeAtCtorStr(), m_pUsedHeapSize->GetSizeAtPeakStr(), m_pUsedHeapSize->GetSizeAtLastStr() }; 

    std::vector<std::string> totalHeapRow = { m_pTotalHeapSize->GetSizeAtCtorStr(), m_pTotalHeapSize->GetSizeAtPeakStr(), m_pTotalHeapSize->GetSizeAtLastStr() };    

    std::vector<std::string> totalPhysicalRow = { m_pTotalPhysicalSize->GetSizeAtCtorStr(), m_pTotalPhysicalSize->GetSizeAtPeakStr(), m_pTotalPhysicalSize->GetSizeAtLastStr() };
        
    m_pHeapMetricsTable->AddTableRow("Used Heap Size", usedHeapRow);
    m_pHeapMetricsTable->AddTableRow("Total Heap Size", totalHeapRow);
    m_pHeapMetricsTable->AddTableRow("Total Physical Size", totalPhysicalRow);
    
    std::array<char, 64> buffer;
    buffer.fill(0);
    time_t rawtime;
    time(&rawtime);
    const auto timeinfo = localtime(&rawtime);
    strftime(buffer.data(), sizeof(buffer), "%m-%d-%Y : %H-%M-%S", timeinfo);
    std::string timeStr(buffer.data());

    // if(args.Length() < 1 || !args[0]->IsArrayt()) {
    //     // bad dump destination, so return false
    //     args.GetReturnValue().Set(v8::Boolean::New(m_pIsolate, false));
    // }

const char* HOME;
    if ((HOME = getenv("HOME")) == NULL)
    {
        args.GetReturnValue().Set(v8::String::NewFromUtf8(m_pIsolate, "HOME == NULL"));
    }
    
    std::stringstream mdPath;
    mdPath << HOME << "/bin/heap-metrics.md";

    std::fstream mdFile;
    mdFile.open(mdPath.str().c_str(), std::fstream::out);
    
    mdFile << "## Node Heap Metrics\n";
    mdFile << "  * V8 Heap Profiler metrics recorded " << timeStr << "\n"; 
    mdFile << "  * Heap size limit of " << m_pUsedHeapSize->bytesToKbStr(heapStatistics.heap_size_limit()) << " at module custruction (Ctor) \n";
    mdFile << "\n";
    mdFile << "---\n";
    mdFile << m_pHeapMetricsTable->GetTable().c_str();    
    mdFile << "\n";
    mdFile << "\n";
    mdFile << "<table>\n";
    mdFile << "  <tr>\n";
    mdFile << "    <td align='left'><font size='1.5'><a href='https://mit-license.org/'>Data captured by npm node-heap-metrics</a></font></td>\n";
    mdFile << "    <td><font size='1.5'><a href='https://mit-license.org/'>Module usage licensed under MIT</a></font></td>\n";
    mdFile << "    <td align='right'><font size='1.5'><a href='https://v8docs.nodesource.com/'>Official V8 Documentation</a></font></td>\n";
    mdFile << "  </tr>\n";
    mdFile << "</table>\n";

    mdFile.close();    
    
    std::stringstream csvPath;
    csvPath << HOME << "/bin/heap-metrics.csv";

    std::fstream csvFile;
    csvFile.open(csvPath.str().c_str(), std::fstream::out);
    
    csvFile << "Peak Sizes, " <<  m_pUsedHeapSize->GetSizeAtPeakStr() << ", " << m_pTotalHeapSize->GetSizeAtPeakStr() << ", " << m_pTotalPhysicalSize->GetSizeAtPeakStr() << "\n";
    
    csvFile.close();    

    // bool return
    args.GetReturnValue().Set(v8::Boolean::New(m_pIsolate, true));
}

void HeapMetrics::UpdateHeapMetrics()
{
    v8::HeapStatistics heapStatistics;

    // Get the current heap statistics
    m_pIsolate->GetHeapStatistics(&heapStatistics);

    // Update the 'At Peak' Heap Stats for each type.
    m_pUsedHeapSize->SetSizeAtPeak(std::max(m_pUsedHeapSize->GetSizeAtPeak(), heapStatistics.used_heap_size()));
    m_pTotalHeapSize->SetSizeAtPeak(std::max(m_pTotalHeapSize->GetSizeAtPeak(), heapStatistics.total_heap_size()));
    m_pTotalPhysicalSize->SetSizeAtPeak(std::max(m_pTotalPhysicalSize->GetSizeAtPeak(), heapStatistics.total_physical_size()));

    // Update the 'At Last' Heap Stats for each type.
    m_pUsedHeapSize->SetSizeAtLast(heapStatistics.used_heap_size());
    m_pTotalHeapSize->SetSizeAtLast(heapStatistics.total_heap_size());
    m_pTotalPhysicalSize->SetSizeAtLast(heapStatistics.total_physical_size());
    
    m_GCPrologueCallbackCalled++;
}


void UpdateHeapValuesGCC(v8::Isolate *isolate, v8::GCType type, v8::GCCallbackFlags flags)
{
	HeapMetrics * pHeapMetrics = HeapMetrics::GetInstance();
	
	pHeapMetrics->UpdateHeapMetrics();
}

// ---------------------------------------------------------------------------------------
// Wrapper functions

void InitHeapMetrics()
{
    // Instatiate the singlton object
	HeapMetrics::GetInstance();
}

void DumpHeapMetrics(const v8::FunctionCallbackInfo<v8::Value> & args)
{
	HeapMetrics::GetInstance()->DumpHeapMetrics( args );
}

void ForceGC(const v8::FunctionCallbackInfo<v8::Value> & args)
{
    v8::Isolate::GetCurrent()->RequestGarbageCollectionForTesting(v8::Isolate::kFullGarbageCollection);
}    

    
} // namespace heap_metrics