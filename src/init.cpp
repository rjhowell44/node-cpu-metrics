/*
The MIT License

Copyright (c) 2010-2017 Google, Inc. http://angularjs.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in-
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
#include "heap_metrics.h"

extern "C" {
    
void init (v8::Handle <v8::Object> exports, v8::Handle<v8::Object> module)
{
    // Instantiate the Singleton Heap Metrics Object
    heap_metrics::InitHeapMetrics();

    // Map the client API's
    NODE_SET_METHOD(exports, "DumpHeapMetrics", heap_metrics::DumpHeapMetrics);
    NODE_SET_METHOD(exports, "ForceGC", heap_metrics::ForceGC);
}

// Associate the init() function (above) as the entry point to the Node module. 
// The function will be invoked on execution of the JavaScript require():
//
//     const heap_metrics = require( 'metrics.node' );

NODE_MODULE(heap_metrics, init);

}; // 
