#!/bin/js

var bname = 'stream/helper/base';
var b = require(bname);
fmt.oprint(bname, b);

var fname = 'stream/helper/file';
var f = require(fname);
fmt.oprint(fname, f);

var stream = {};
b.stream(stream, fname, 'stream/helper/file.js', 'r', 'file');
fmt.oprint('stream', stream);

print(f.open);
f.open(stream);
fmt.oprint('stream', stream);
print(6);

var b1 = f.readEx(stream, 10000);
print(7);
print(b1);
var b2 = Buffer(10000);
print(8);
var len = f.read(stream, b2);
print(9);
f = null;
print(10);

var b3 = new Uint8Array(b2);
print(b3);
print(11);

/* eof */
