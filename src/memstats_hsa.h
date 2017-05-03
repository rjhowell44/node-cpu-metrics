#ifndef __HEAP_SNAPSHOT_ADAPTER_H__
#define __HEAP_SNAPSHOT_ADAPTER_H__

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


class HeapSnapshotAdapter 
{
    
public:

    class OutputStream {
    public:
        virtual ~OutputStream() { }
        virtual void Write(const v8::String& chunk) = 0;
        virtual void Close() = 0;
    };
    
    
    HeapSnapshotAdapter(const v8::HeapSnapshot* snapshot)
        : m_snapshot(snapshot)
    {}

    virtual ~HeapSnapshotAdapter();


private:
    const v8::HeapSnapshot* m_snapshot;
};


    
}; // HeapSnapshotAdapter

} // namespace memstats

#endif // __HEAP_SNAPSHOT_ADAPTER_H__
