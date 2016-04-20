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
	print('base.stream 1');
	var tmp_filename = maybe_string(filename);
	print('base.stream 2');

	obj.filename = tmp_filename;
	print('base.stream 3');
	obj.mode = maybe_string(mode) || 'r';
	print('base.stream 4');
	obj.type = maybe_string(type) || 'file';
	print('base.stream 5');
	obj.$name = function () { return name + '(' + tmp_filename + ')'; };
	print('base.stream 6');
	obj.stream = null;
	print('base.stream 7');

	return obj;
};

print('load', name);
/* eof */
