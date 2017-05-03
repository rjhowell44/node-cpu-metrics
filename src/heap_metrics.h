#ifndef __HEAP_STATISTICS_H__
#define __HEAP_STATISTICS_H__

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

#include <v8.h>
#include <v8-profiler.h>
#include <node.h>
#include <node_object_wrap.h>

#include "heap_metrics_type.h"
#include "heap_metrics_table.h"

namespace heap_metrics 
{
    
class HeapStats : public node::ObjectWrap
{
    private:
    
        // Instance pointer for the singleton object
        static HeapStats * m_pInstance;
        
        v8::Isolate * m_pIsolate;
        
        // Heap profiler to enable/disable object tracking.
        v8::HeapProfiler * m_pHeapProfiler;	
        
        HeapMetricsType * m_pUsedHeapSize;
        HeapMetricsType * m_pTotalHeapSize;
        HeapMetricsType * m_pTotalPhysicalSize;
        
        HeapMetricsTable * m_pHeapMetricsTable;
        
        int m_GCPrologueCallbackCalled;


    protected:
        HeapStats();
        ~HeapStats();
    
    public:
        static HeapStats * GetInstance();
        void DumpHeapStats(const v8::FunctionCallbackInfo<v8::Value> & args);
        
        // Note: to be called by the GCC prologue call back only.  
        // Must be public to expose to the C callback.
        void UpdateHeapStats();

    private:
        std::string bytesToStr(int bytes);
        static const char* m_pColumnHeaders[];
        
};

// C style wrapper functions

/// Called by init.cpp to instantiate the singlton HeapStats object.
void InitHeapStats();

// Called be the 'using' script to Dump all current statistics to heapstats.md
void DumpHeapStats(const v8::FunctionCallbackInfo<v8::Value> & args);

// Called by the 'client' script to Force GC/ S
void ForceGC(const v8::FunctionCallbackInfo<v8::Value> & args);


};  // namespace HeapStats

#endif // __HEAP_STATISTICS_H__
