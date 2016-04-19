#!/bin/js

/*
* module: pipe stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/pipe';

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function (obj) {
	return null === obj.stream;
};

pt.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libc__.popen(obj.filename, obj.mode);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && pt.is_open(obj)) {
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
