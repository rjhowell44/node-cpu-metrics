var heapMetrics = require('heap-metrics'),
    assert = require('assert');

describe('DumpHeapMetrics', function () {
    it('should return true', function () {
        assert(heapMetrics.DumpHeapMetrics());
    });
});