#!/bin/js

print('base 1');
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
print('base 2');
/* eof */
