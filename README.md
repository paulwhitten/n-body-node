# N-body node.js module

A node.js native addon for the [Computer Language Benchmarks Game](http://benchmarksgame.alioth.debian.org/).

The existing node submissions for this problem are either blocking or implement the solution in JavaScript.  Node is not meant to run computationally intensive apps in JS.

Here is how it could be done, as an asynchronous native module.

## Build

You need node-gyp to build.

Run `npm install node-gyp -g` to install node-gyp.

Run ` npm install` to build.

## Running

Execute by running `node index.js x`, where x is the number of iterations.



