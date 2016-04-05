#!/bin/js

/*
* module: base stream helper
*/
var mod = this,
	pt = mod.__proto__;

pt.$name = pt.$name || 'stream/helper/base';
pt.$debugger = new $Debugger(pt.$name);

pt.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function (obj) {
	return null === obj.stream;
};

/* eof */
