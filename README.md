# Node Heap Metrics

[![Build Status](https://travis-ci.org/rjhowell44/node-heap-metrics.svg?branch=master)](https://travis-ci.org/rjhowell44/node-heap-metrics)
[![Space Metric](http://robert.stridespace.com/spaces/2652/metrics/3231/badge?token=6ca6adddd90184c574387d838e1330ee664fb65b)](http://robert.stridespace.com/spaces/2652/schema/Node-6/Metrics/heap-metrics.md "Node-6 Peak Heap Metrics")
[![Space Metric](http://robert.stridespace.com/spaces/2652/metrics/3232/badge?token=de0d6fe4ad59572dcf429918f1080cbe446aca3c)](http://robert.stridespace.com/spaces/2652/schema/Node-6/Metrics/heap-metrics.md "GC Prologue Notifications")

---
## Under Construction

---
## Installation

```
- npm install node-heap-metrics

```


---
### Description
Dump `Peak Heap Usage` during test execution - to monitor the effects of code changes on heap usage for the life of the branch. 

Node Heap Metrics is written in C++, wrapped as a JS node. The implementation makes use of the Google open source V8 Heap Profiler included with Node Js.  [Official V8 Documentation](https://v8docs.nodesource.com/)

The class is implemented a singleton. The first script to `require( 'heap_metrics.node' )` will intantiate the singleton object and **start heap profiling**. All subsequent calls to `require()` will simply return the singleton object. 
This allows for the `start profiling` and `dump metric` actions to be broken up between 2 scripts to be called before and after test execution respectively.

heap_metrics.DumpHeapMetrics() will produce 2 files:
 * heap_metrics.html 
  
 
 * heap_metrics.csv
```
Peak Sizes, 22048, 40491, 36661
GC Prologue Notifications, 201
```

with the following content.
 
**Optionally** - you can then upload the files -- along with the test results, code coverage, and other metrics -- to [Testspace](www.testspace.com) in your CI .yml file (Travis, Circle CI, AppYayor, etc) 
(requires a free Testspace account)

```
  - testspace test*.xml coverage.xml heap_metrics.html{heap_metrics.csv}
  
```

![heap_metrics](https://github.com/rjhowell44/node-heap-metrics/blob/master/images/heap-usage-vs-gc-events.png)

---
### Usage


```javascript
// require statement will enable V8 Heap Profiling
var heap_metrics = require( 'heap_metrics.node' );

// optinally, force GC before dump (requires --expose_gc)
heap_metrics.ForceGC();

// dump metrics to file
heap_metrics.DumpHeapMetrics()
```
