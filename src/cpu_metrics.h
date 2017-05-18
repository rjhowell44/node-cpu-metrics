#ifndef __CPU_METRICS_H__
#define __CPU_METRICS_H__

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

#include <vector>

#include <v8.h>
#include <v8-profiler.h>

#include <node.h>
#include <node_object_wrap.h>

#include "cpu_metrics_table.h"

namespace cpu_metrics 
{
    
class CpuMetrics : public node::ObjectWrap
{
    private:
    
        // Instance pointer for the singleton object
        static CpuMetrics * m_pInstance;
        
        v8::Isolate * m_pIsolate;
        
        // CPU profiler to enable/disable sampling.
        v8::CpuProfiler * m_pCpuProfiler;
        int m_sampleRate;
        
        CpuMetricsTable * m_pCpuMetricsTable;
        
        
    protected:
        CpuMetrics();
        ~CpuMetrics();
    
    public:
        static CpuMetrics * GetInstance();
        void StartProfiling(const v8::FunctionCallbackInfo<v8::Value> & args);
        void StopProfiling(const v8::FunctionCallbackInfo<v8::Value> & args);
        
    private:
        void ProcessNode(const v8::CpuProfileNode * pCpuProfileNode, int depth);
        static const char* m_pColumnHeaders[];

};

// C style wrapper functions - node entry points

/// Called by init.cpp to instantiate the singlton CpuMetrics object.
void InitCpuMetrics();

// Called be the 'client' script to start the CPU profiler
void StartProfiling(const v8::FunctionCallbackInfo<v8::Value> & args);

// Called be the 'client' script to stop the CPU profiler and return the profile
void StopProfiling(const v8::FunctionCallbackInfo<v8::Value> & args);

};  // namespace cpu_metrics

#endif // __CPU_METRICS_H__
