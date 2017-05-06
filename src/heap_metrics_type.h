#ifndef __HEAP_METRICS_TYPE_H__
#define __HEAP_METRICS_TYPE_H__

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
#include <math.h> // round()

#if defined(_MSC_VER)
#define ROUND(x) floor(x + 0.5)
#else
#define ROUND round
#endif


namespace heap_metrics 
{
    
class HeapMetricsType
{

public:
    size_t m_sizeAtCtor;
    size_t m_sizeAtPeak;
    size_t m_sizeAtLast;
    std::stringstream m_typeName;

public:
    HeapMetricsType(const char* typeName) :
        m_sizeAtCtor(0),
        m_sizeAtPeak(0),
        m_sizeAtLast(0) 
    { 
        m_typeName << typeName;
    }

    ~HeapMetricsType();

public:
    
    const char* GetTypeName () { return m_typeName.str().c_str(); } 

    inline void SetSizeAtCtor(size_t size) { m_sizeAtCtor = size; } 
    inline size_t GetSizeAtCtor() { return m_sizeAtCtor; }
    size_t GetKbSizeAtCtor() { return bytesToKb(m_sizeAtCtor); }
    std::string GetSizeAtCtorStr() { std::stringstream ss; ss << bytesToKbStr(m_sizeAtCtor).c_str(); return ss.str(); }

    inline void SetSizeAtPeak(size_t size) { m_sizeAtPeak = size; } 
    inline size_t GetSizeAtPeak() { return m_sizeAtPeak; }
    size_t GetKbSizeAtPeak() { return bytesToKb(m_sizeAtPeak); }
    std::string GetSizeAtPeakStr() { std::stringstream ss; ss << bytesToKbStr(m_sizeAtPeak).c_str(); return ss.str(); }

    inline void SetSizeAtLast(size_t size) { m_sizeAtLast = size; } 
    inline size_t GetSizeAtLast() { return m_sizeAtLast; }
    size_t GetKbSizeAtLast() { return bytesToKb(m_sizeAtLast); }
    std::string GetSizeAtLastStr() { std::stringstream ss; ss << bytesToKbStr(m_sizeAtLast).c_str(); return ss.str(); }

    size_t bytesToKb(int bytes)
    {
        return static_cast<size_t>(round(bytes / (((double) 1024 ) / 100) / (double) 100));        
    }
    
    std::string bytesToKbStr(int bytes) 
    {

        std::string kbNumberString = std::to_string(bytesToKb(bytes));
        std::stringstream kbNumberStringWithCommas;
        
        int insertPosition = kbNumberString.length() - 3;
        while (insertPosition > 0) {
            kbNumberString.insert(insertPosition, ",");
            insertPosition-=3;
        }        

        kbNumberStringWithCommas << kbNumberString.c_str() << " KB";

        return kbNumberStringWithCommas.str();
    }
};


};  // namespace heap_metrics

#endif // __HEAP_METRICS_TYPE_H__
