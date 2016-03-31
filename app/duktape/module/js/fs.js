#!/bin/js

/*
* module: fdset
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'fs';
pt.$debugger = new $Debugger(pt.$name);

/* eof */
