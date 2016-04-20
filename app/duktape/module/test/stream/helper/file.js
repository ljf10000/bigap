#!/bin/js

print(1);
var fname = 'stream/helper/file';
var bname = 'stream/helper/base';
var f = require(fname);
fmt.oprint(fname, f);

var b = require(bname);
fmt.oprint(bname, b);

var stream = {};
b.stream(stream, fname, 'stream.js', 'r', 'file');

f.open(stream);
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

/* eof */
