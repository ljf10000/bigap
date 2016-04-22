#!/bin/js
/*
* module: base stream helper
*/
var mod = this,
	name = 'stream/helper/base';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && null !== obj.stream;
};

mod.is_close = function (obj) {
	return null === obj.stream;
};

/* eof */
