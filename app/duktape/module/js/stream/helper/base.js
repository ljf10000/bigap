#!/bin/js

/*
* module: base stream helper
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'stream/helper/base';
pt.$debugger = new $Debugger(pt.$name);

pt.type = {
	file: 0,
	pipe: 1,
	zlib: 2,
	bzip: 3
};

pt.is_open = function is_open (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function is_close (obj) {
	return null === obj.stream;
};

/* eof */
