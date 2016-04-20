#!/bin/js

var fname = 'stream/helper/file';
var f = require(fname);
fmt.oprint(fname, f);
fmt.oprint(fname, f.__proto__);

var stream = {};
f.stream(stream, fname, 'test.js', 'r', 'file');

fmt.oprint('stream before open', stream);
f.open(stream);
fmt.oprint('stream after open', stream);

print(f.readEx(stream, 10000));

/* eof */
