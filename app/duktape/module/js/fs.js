#!/bin/js

/*
* module: fdset
*/

var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'fs';
pt.debugger = new ModDebugger(pt.$name);



/* eof */
