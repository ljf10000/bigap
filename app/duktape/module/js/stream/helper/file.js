#!/bin/js

/*
* module: file stream helper
*/
print('file 1');
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/file';
print('file 2');
var base = require('stream/helper/base');
print('file 2.1');
print(Object.setPrototypeOf);
print('file 2.2');
print(pt.constructor);
print(pt.constructor === Object.constructor);
print(pt.constructor === Object.prototype.constructor);
print(pt.constructor instanceof Object);
print(pt.constructor instanceof Function);
pt.constructor.prototype = base.__proto__;
print('file 2.3');
Object.setPrototypeOf(pt, base.__proto__);
print('file 3');
pt.$name = function () { return name; };
print('file 4');
pt.$debugger = new $Debugger(name);
print('file 5');
pt.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libc__.fopen(obj.filename, obj.mode);
	}

	return obj;
};
print('file 6');
pt.close = function (obj) {
	if (obj && pt.is_open(obj)) {
		__libc__.fclose(obj.stream);
		obj.stream = null;
	}
};
print('file 7');
pt.read = function (obj, buffer) {
	return __libc__.fread(obj.stream, buffer);
};
print('file 8');
pt.readEx = function (obj, size) {
	return __libc__.freadEx(obj.stream, size);
};
print('file 9');
pt.write = function (obj, buffer) {
	return __libc__.fwrite(obj.stream, buffer);
};
print('file 10');
pt.tell = function (obj) {
	return __libc__.ftell(obj.stream);
};
print('file 11');
pt.seek = function (obj, offset, where) {
	return __libc__.fseek(obj.stream, offset, where);
};
print('file 11');
pt.rewind = function (obj) {
	return __libc__.fseek(obj.stream, 0, __libc__.SEEK_SET);
};
print('file 13');
pt.sync = pt.flush = function (obj) {
	return __libc__.fflush(obj.stream);
};
print('file 14');
pt.error = function (obj) {
	return __libc__.ferror(obj.stream);
};
print('file 15');
pt.eof = function (obj) {
	return __libc__.feof(obj.stream);
};
print('file 16');
/* eof */
