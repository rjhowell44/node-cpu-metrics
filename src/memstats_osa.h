#ifndef __OUTPUT_STREAM_ADAPTER_H__
#define __OUTPUT_STREAM_ADAPTER_H__

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
#include <node.h>

namespace memstats 
{

class HeapOutputStream {
public:
    virtual ~HeapOutputStream() { }
    
    virtual void Write( void* data, int size ) = 0;
    virtual void Close() = 0;
};
    


class OutputStreamAdapter : public v8::OutputStream 
{

public:
    OutputStreamAdapter( HeapOutputStream* oStream )
        : m_oStream( oStream ) { }
        
    void EndOfStream() { m_oStream->Close(); }
    
    int GetChunkSize() { return 102400; }
    
    WriteResult WriteAsciiChunk( char* data, int size )
    {
        m_oStream->Write( ( void* )data, size );
        return kContinue;
    }
     
    WriteResult WriteHeapStatsChunk( v8::HeapStatsUpdate * data, int count )
    {
        m_oStream->Write( ( void* )data, count );
        return kContinue;
    }

private:
    HeapOutputStream* m_oStream;
    
};

} // namespace memstats

#endif // __OUTPUT_STREAM_ADAPTER_H__
