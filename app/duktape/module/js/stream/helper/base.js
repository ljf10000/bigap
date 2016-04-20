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

mod.stream = function (obj, name, filename, mode, type) {
	var tmp_filename = maybe_string(filename);

	obj.filename = tmp_filename;
	obj.mode = maybe_string(mode) || 'r';
	obj.type = maybe_string(type) || 'file';
	obj.$name = function () { return name + '(' + tmp_filename + ')'; };
	obj.stream = null;

	return obj;
};

/* eof */
