#!/bin/js

/*
* module: file stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/file',
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
		obj.stream = __libc__.fopen(obj.filename, obj.mode);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && base.is_open(obj)) {
		__libc__.fclose(obj.stream);
		obj.stream = null;
	}
};

pt.read = function (obj, buffer) {
	return __libc__.fread(obj.stream, buffer);
};

pt.readEx = function (obj, size) {
	return __libc__.freadEx(obj.stream, size);
};

pt.write = function (obj, buffer) {
	return __libc__.fwrite(obj.stream, buffer);
};

pt.tell = function (obj) {
	return __libc__.ftell(obj.stream);
};

pt.seek = function (obj, offset, where) {
	return __libc__.fseek(obj.stream, offset, where);
};

pt.rewind = function (obj) {
	return __libc__.fseek(obj.stream, 0, __libc__.SEEK_SET);
};

pt.sync = pt.flush = function (obj) {
	return __libc__.fflush(obj.stream);
};

pt.error = function (obj) {
	return __libc__.ferror(obj.stream);
};

pt.eof = function (obj) {
	return __libc__.feof(obj.stream);
};

/* eof */
