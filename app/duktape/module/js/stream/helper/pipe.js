#!/bin/js

/*
* module: pipe stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/pipe',
	base = require('stream/helper/base');

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.stream = function (obj, name, filename, mode, type) {
	return base.stream(obj, name, filename, mode, type);
};

pt.is_open = function (obj) {
	return base.is_open(obj);
};

pt.is_close = function (obj) {
	return base.is_close(obj);
};

pt.open = function (obj, mode) {
	if (obj && base.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libc__.popen(obj.filename, obj.mode);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && base.is_open(obj)) {
		__libc__.pclose(obj.stream);
		obj.stream = null;
	}
};

pt.read = function (obj, buffer) {
	return __libc__.fread(obj.stream, buffer);
};

pt.readEx = function (obj, size) {
	return __libc__.freadEx(obj.stream, size);
};

pt.error = function (obj) {
	return __libc__.ferror(obj.stream);
};

pt.eof = function (obj) {
	return __libc__.feof(obj.stream);
};

/* eof */
