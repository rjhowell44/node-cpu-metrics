const heap_metrics = require( './build/Release/heap_metrics.node' );



function use_mem()
{
    var garbage = [];
    
    
    for (var i=0; i<100000; i++)
    {
        garbage.push("this is a long string to consume heap memory.... lets make the string even longer... why stop here... even longer still");

    }
}

for (var i=0; i<100; i++)
{
    use_mem();    
}

console.log(heap_metrics.DumpHeapStats());



