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
    
    m_HeapSizeLimitAtCtor = heapStatistics.heap_size_limit();
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

void HeapMetrics::GetHeapMetrics(const v8::FunctionCallbackInfo<v8::Value> & args)
{
    // Update the running peak heap, usage, and total physical sizes
    UpdateHeapMetrics();

    // ------------------------------------------------------------------------------------------
    // assemble the metrics content - start with the number of GC events
    
    v8::Local<v8::Object> heapMetrics = v8::Object::New(m_pIsolate);  

    heapMetrics->Set(v8::String::NewFromUtf8(m_pIsolate, "Heap Size Limit"), v8::String::NewFromUtf8(m_pIsolate,  m_pUsedHeapSize->bytesToKbStr(m_HeapSizeLimitAtCtor).c_str()));
    heapMetrics->Set(v8::String::NewFromUtf8(m_pIsolate, "GC Prologue Notifications"), v8::Integer::New(m_pIsolate, m_GCPrologueCallbackCalled));

    // ------------------------------------------------------------------------------------------
    // assemble the heap used metrics
    
    v8::Local<v8::Object> heapUsed = v8::Object::New(m_pIsolate);  

    heapUsed->Set(v8::String::NewFromUtf8(m_pIsolate, "At Ctor"), v8::String::NewFromUtf8(m_pIsolate, m_pUsedHeapSize->GetSizeAtCtorStr().c_str()));
    heapUsed->Set(v8::String::NewFromUtf8(m_pIsolate, "At Peak"), v8::String::NewFromUtf8(m_pIsolate, m_pUsedHeapSize->GetSizeAtPeakStr().c_str()));
    heapUsed->Set(v8::String::NewFromUtf8(m_pIsolate, "At last"), v8::String::NewFromUtf8(m_pIsolate, m_pUsedHeapSize->GetSizeAtLastStr().c_str()));

    heapMetrics->Set(v8::String::NewFromUtf8(m_pIsolate, "Heap Used Metrics"), heapUsed); 

    // ------------------------------------------------------------------------------------------
    // assemble the heap size metrics
    
    v8::Local<v8::Object> heapSize = v8::Object::New(m_pIsolate);  

    heapSize->Set(v8::String::NewFromUtf8(m_pIsolate, "At Ctor"), v8::String::NewFromUtf8(m_pIsolate, m_pTotalHeapSize->GetSizeAtCtorStr().c_str()));
    heapSize->Set(v8::String::NewFromUtf8(m_pIsolate, "At Peak"), v8::String::NewFromUtf8(m_pIsolate, m_pTotalHeapSize->GetSizeAtPeakStr().c_str()));
    heapSize->Set(v8::String::NewFromUtf8(m_pIsolate, "At last"), v8::String::NewFromUtf8(m_pIsolate, m_pTotalHeapSize->GetSizeAtLastStr().c_str()));

    heapMetrics->Set(v8::String::NewFromUtf8(m_pIsolate, "Heap Size Metrics"), heapSize); 
    
    // ------------------------------------------------------------------------------------------
    // assemble the physical size metrics
    v8::Local<v8::Object> physicalSize = v8::Object::New(m_pIsolate);  

    physicalSize->Set(v8::String::NewFromUtf8(m_pIsolate, "At Ctor"), v8::String::NewFromUtf8(m_pIsolate, m_pTotalPhysicalSize->GetSizeAtCtorStr().c_str()));
    physicalSize->Set(v8::String::NewFromUtf8(m_pIsolate, "At Peak"), v8::String::NewFromUtf8(m_pIsolate, m_pTotalPhysicalSize->GetSizeAtPeakStr().c_str()));
    physicalSize->Set(v8::String::NewFromUtf8(m_pIsolate, "At last"), v8::String::NewFromUtf8(m_pIsolate, m_pTotalPhysicalSize->GetSizeAtLastStr().c_str()));

    heapMetrics->Set(v8::String::NewFromUtf8(m_pIsolate, "Physical Size Metrics"), physicalSize); 
    
    
    // ------------------------------------------------------------------------------------------
    // return the metrics to the calling script

    args.GetReturnValue().Set(heapMetrics);
}


void HeapMetrics::DumpHeapMetrics(const v8::FunctionCallbackInfo<v8::Value> & args)
{
    // Update the running peak heap, usage, and total physical sizes
    UpdateHeapMetrics();

    // ------------------------------------------------------------------------------------------
    // assemble the metrics table content - start with the table caption - number of GC events
    
    std::stringstream tableCaption;
    tableCaption << "Peak values checked over " << m_GCPrologueCallbackCalled << " GC Prologue Notifications \n";
    m_pHeapMetricsTable->AddTableCaption(tableCaption.str().c_str());
    
    // ------------------------------------------------------------------------------------------
    // Add row data to the table - the "At Ctor" , "At Peak", and "At Last" values (columns) for the "Heap Used", "Heap Size", and "Physical Size" metrics (rows)
    
    std::vector<std::string> usedHeapRow =      { m_pUsedHeapSize->GetSizeAtCtorStr(),      m_pUsedHeapSize->GetSizeAtPeakStr(),      m_pUsedHeapSize->GetSizeAtLastStr()      }; 
    std::vector<std::string> totalHeapRow =     { m_pTotalHeapSize->GetSizeAtCtorStr(),     m_pTotalHeapSize->GetSizeAtPeakStr(),     m_pTotalHeapSize->GetSizeAtLastStr()     };    
    std::vector<std::string> totalPhysicalRow = { m_pTotalPhysicalSize->GetSizeAtCtorStr(), m_pTotalPhysicalSize->GetSizeAtPeakStr(), m_pTotalPhysicalSize->GetSizeAtLastStr() };
        
    m_pHeapMetricsTable->AddTableRow("Used Heap Size", usedHeapRow);
    m_pHeapMetricsTable->AddTableRow("Total Heap Size", totalHeapRow);
    m_pHeapMetricsTable->AddTableRow("Total Physical Size", totalPhysicalRow);
    
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
    htmlPath << HOME << "/bin/heap-metrics.html";
    csvPath << HOME << "/bin/heap-metrics.csv";


    // ------------------------------------------------------------------------------------------
    // Assemble and stream the HTML content to file

    std::fstream mdFile;
    mdFile.open(htmlPath.str().c_str(), std::fstream::out);

    mdFile << "<article>\n";
    mdFile << "  <header>\n";    
    mdFile << "    <h1> Node Heap Metrics</h1>\n";
    mdFile << "  </header>\n";
    mdFile << "  <p>\n";
    mdFile << "    V8 Heap Profiler metrics recorded " << timeStr << "\n"; 
    mdFile << "  </br>\n";
    mdFile << "    Heap size limit of " << m_pUsedHeapSize->bytesToKbStr(m_HeapSizeLimitAtCtor) << " at node custruction (Ctor)\n";
    mdFile << "  </p>\n";
    mdFile <<    m_pHeapMetricsTable->GetTable().c_str();    
    mdFile << "  <table cellpadding='2'>\n";
    mdFile << "    <tr>\n";
    mdFile << "      <td align='left'><font size='1.5'><a href='https://mit-license.org/'target='_blank'>Data captured by npm node-heap-metrics</a></font></td>\n";
    mdFile << "      <td><font size='1.5'><a href='https://mit-license.org/'target='_blank'>Module usage licensed under MIT</a></font></td>\n";
    mdFile << "      <td align='right'><font size='1.5'><a href='https://v8docs.nodesource.com/'target='_blank'>Official V8 Documentation</a></font></td>\n";
    mdFile << "    </tr>\n";
    mdFile << "  </table>\n";
    mdFile << "<article>\n";

    mdFile.close();    
    
    // ------------------------------------------------------------------------------------------
    // Assemble and stream the CSV content to file
    std::fstream csvFile;
    csvFile.open(csvPath.str().c_str(), std::fstream::out);
    
    csvFile << "Peak Sizes, " <<  m_pUsedHeapSize->GetKbSizeAtPeak() << ", " << m_pTotalHeapSize->GetKbSizeAtPeak() << ", " << m_pTotalPhysicalSize->GetKbSizeAtPeak() << "\n";
    csvFile << "GC Prologue Notifications, " << m_GCPrologueCallbackCalled << "\n";
    
    csvFile.close();    

    // ------------------------------------------------------------------------------------------
    // Successful Dump

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
    // First call to GetInstance() will instatiate the singlton object
	HeapMetrics::GetInstance();
}

void GetHeapMetrics(const v8::FunctionCallbackInfo<v8::Value> & args)
{
	HeapMetrics::GetInstance()->GetHeapMetrics( args );
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