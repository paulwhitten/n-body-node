var addon = require('./build/Release/nBody.node');
var process = require('process');

// take initial high resolution timestamp
//var startTime = process.hrtime();

// Would be nice if the module was more generic and we passed the
// body specifics into the module.

addon.nBody(process.argv[2], (err, result) => {
    console.log(result.toPrecision(9));
});

// take timestamp on exit, printing results.
/*
process.on('exit', ()=> {
    var duration = process.hrtime(startTime);
    console.log(`Benchmark took ${duration[0] * 1e9 + duration[1]} nanoseconds`);
});
*/