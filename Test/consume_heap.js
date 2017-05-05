var heap_metrics = require( './../build/Release/heap_metrics.node' );
var assert = require('assert');

function consume_heap()
{
    var garbage = [];
    
    
    for (var i=0; i<1000; i++)
    {
        garbage.push("this is a long string to consume heap memory");

    }
}


describe('DumpHeapMetrics', function(){
  it('should return true', function(){
    for(var i=0;i<10;i++){  
        consume_heap();  
    }
    assert(heap_metrics.DumpHeapMetrics());
  });
})