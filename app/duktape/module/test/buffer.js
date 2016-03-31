#!/bin/js

var stream = require("stream");
fmt.oprint('stream', stream);

var file = new stream.Stream("buffer.js", "r");
file.open();
fmt.oprint('file', file);

var b1 = file.readEx(10000);
print(b1);
var b2 = Buffer(10000);
var len = file.read(b2);
file = null;

var b3 = new Uint8Array(b2);
print(b3);

stream.$debugger.destructor("asdf");
