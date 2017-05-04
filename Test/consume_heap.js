var heap_metrics = require( './../build/Release/heap_metrics.node' );
var assert = require('assert');

function consume_heap()
{
    var garbage = [];
    
    
    for (var i=0; i<100000; i++)
    {
        garbage.push("this is a long string to consume heap memory.... lets make the string even longer... why stop here... even longer still");

    }
}

describe('DumpHeapMetrics', function(){
  it('should return true', function(){
    for(var i=0;i<100;i++){  
        consume_heap();  
    }
    assert(heap_metrics.DumpHeapMetrics());
  });
})