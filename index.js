var addon = require('./build/Release/nBody.node');
var process = require('process');

var startTime = process.hrtime();

addon.nBody(process.argv[2], (err, result) => {
    console.log(result.toPrecision(9));
});

process.on('exit', ()=> {
    var duration = process.hrtime(startTime);
    console.log(`Benchmark took ${duration[0] * 1e9 + duration[1]} nanoseconds`);
});