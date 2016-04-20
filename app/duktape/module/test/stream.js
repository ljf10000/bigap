#!/bin/js

print(1);
var stream = require('stream/stream');
print(2);
print(stream.__proto__ === stream.Stream.prototype);
fmt.oprint('stream', stream);
print(3);
var f = new stream.Stream('buffer.js', 'r');
print(4);
fmt.oprint('file', f);
print(5);
f.open();
print(6);

var b1 = f.readEx(10000);
print(7);
print(b1);
var b2 = Buffer(10000);
print(8);
var len = f.read(b2);
print(9);
f = null;
print(10);

var b3 = new Uint8Array(b2);
print(b3);
print(11);

stream.$debugger.destructor('asdf');
print(12);
