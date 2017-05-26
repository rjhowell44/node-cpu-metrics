var cpuMetrics = require( './../../build/Release/cpu_metrics.node' );



function consume_heap()
{
    var garbage = [];
    
    
    for (var i=0; i<1000000; i++)
    {
        garbage.push("this is a long string to consume heap memory... lets make it a little longer... and longer still... why stop now");

    }
}

function level_4()
{
    consume_heap();
    consume_heap();
}

function level_3()
{
    consume_heap();
    level_4();
}

function level_2()
{
    consume_heap();
    level_3();
    level_4();
}

function level_1()
{
    for (var i=0; i<10; i++)
    {
        consume_heap();
    }
    level_2();
    level_3();
    level_4();
}

cpuMetrics.StartProfiling();

level_1();

console.log(cpuMetrics.StopProfiling());
