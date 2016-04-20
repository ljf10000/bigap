#!/bin/js

/*
* module: base stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/base';

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function (obj) {
	return null === obj.stream;
};

pt.stream = function (obj, name, filename, mode, type) {
	var tmp_filename = maybe_string(filename);

	obj.filename = tmp_filename;
	obj.mode = maybe_string(mode) || 'r';
	obj.type = must_string(type) || 'file';
	obj.$name = function () { return name + '(' + tmp_filename + ')'; };
	obj.stream = null;

	return obj;
};

/* eof */
