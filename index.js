var heapMetrics = require('./build/Release/heap_metrics.node');

module.exports = function getHeapMetrics()
{
    return heapMetrics.GetHeapMetrics();
};

module.exports = function dumpHeapMetrics()
{
    return heapMetrics.DumpHeapMetrics();
};