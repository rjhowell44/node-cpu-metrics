# Node Heap Metrics

[![Build Status](https://travis-ci.org/rjhowell44/node-heap-metrics.svg?branch=master)](https://travis-ci.org/rjhowell44/node-heap-metrics)
[![Space Metric](http://robert.stridespace.com/spaces/2652/metrics/3231/badge?token=6ca6adddd90184c574387d838e1330ee664fb65b)](http://robert.stridespace.com/spaces/2652/schema/Node-6/Metrics/heap-metrics.md "Node-6 Peak Heap Metrics")
[![Space Metric](http://robert.stridespace.com/spaces/2652/metrics/3232/badge?token=de0d6fe4ad59572dcf429918f1080cbe446aca3c)](http://robert.stridespace.com/spaces/2652/schema/Node-6/Metrics/heap-metrics.md "GC Prologue Notifications")

---
## Installation

```
- npm install node-heap-metrics

```

---
### Usage


```javascript
// require statement will enable V8 Heap Profiling
var heap_metrics = require( 'heap_metrics.node' );

//  ---- run some tests ----

// optionally, force final GC before dump (requires --expose_gc)
heap_metrics.ForceGC();

// dump metrics to file
heap_metrics.DumpHeapMetrics()
```


---
### Description
Enable and dump `Heap Profiler Metrics` to monitor the effects of code changes on heap usage for the life of the branch. 

Node Heap Metrics is written in C++, wrapped as a Node. The implementation makes use of Google's open source V8 Heap Profiler included with Node Js.  ([Official V8 Documentation](https://v8docs.nodesource.com/))

Heap metrics, maintained by the node, are update on V8's garbage collection (GC) Prologue Notifications -- in other words, just before GC -- ensuring peak measurements for the following metrics:
 * Heap Used
 * Heap Size
 * Physical Size

The class is implemented as a singleton. The first script to `require( 'heap_metrics.node' )` will intantiate the  object and **start heap profiling**. All subsequent calls to require() will return the singleton object will profilling is ongoing. 
This allows for the start profiling to be enabled from one script or node,  and the DumpHeapMetrics() function to be called from another.   

Calling `heap_metrics.DumpHeapMetrics()` will produce 2 files: 1) **heap_metrics.html** 2) **heap_metrics.csv** -- both in `$(HOME)/bin` which must exist prior to calling. (as of the current, initial release of 1.0.0)  

#### The HTML file will render as follows:

#### The .csv (comma seperated values) file will contain the peak sizes ( heap used, heap size, physical size ) and the number of GC events. 

```
Peak Sizes, 22048, 40491, 36661
GC Prologue Notifications, 201
```

**Optionally** - you can then push the metric files -- along with the test results, code coverage, and other metrics -- to [Testspace](www.testspace.com) in your CI .yml file (Travis, Circle CI, AppYayor, etc) 
(requires a free Testspace account)

```
  - testspace test*.xml coverage.xml heap_metrics.html{heap_metrics.csv}
  
```

---
### Metric Trends
The following timeline charts were produced by pushing changes to `Test/consume_heap.js` to this repository.  The testscript consumes heap memory which can be increasing or deacresing the for loop limit. The `metric files` are pushed from `.travis.yml` file above. 

![heap_metrics](https://github.com/rjhowell44/node-heap-metrics/blob/master/images/heap-usage-vs-gc-events.png)

