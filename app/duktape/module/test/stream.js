#!/bin/js

var stream= require('stream/stream');
var f = stream.stream('stream.js', 'r', 'file');
f.open();
print(f.readEx(10000));

f = null;

/* eof */
