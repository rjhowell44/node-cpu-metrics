var heap_metrics = require('./build/Release/heap_metrics.node');

module.exports = function GetHeapMetrics(){
  heap_metrics.GetHeapMetrics();
};  

module.exports = function DumHeapMetrics(){
  heap_metrics.GetHeapMetrics();
};  