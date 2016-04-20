#!/bin/js

var stream= require('stream/stream');
var f = new stream.Stream('stream.js', 'r', 'file', true);
print(f.readEx(10000));

f = null;

/* eof */
