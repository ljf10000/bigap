#!/bin/js

print('base 1');
/*
* module: base stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/base';
print('base 2');
pt.$name = function () { return name; };
print('base 3');
pt.$debugger = new $Debugger(name);
print('base 4');
pt.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};
print('base 5');
pt.is_close = function (obj) {
	return null === obj.stream;
};
print('base 6');
/* eof */
