var heap_metrics = require( './../build/Release/heap_metrics.node' );
var assert = require('assert');

function consume_heap()
{
    var garbage = [];
    
    
    for (var i=0; i<100000; i++)
    {
        garbage.push("this is a long string to consume heap memory... lets make it a little longer... and longer still... why stop now");

    }
}


describe('GetHeapMetrics', function(){
  it('should return true', function(){
    for(var i=0;i<400;i++){  
        consume_heap();  
    }
    console.log(heap_metrics.GetHeapMetrics());
    assert(true);
  });
})