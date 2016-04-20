#!/bin/js

/*
* module: bzip stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/bzip',
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
		obj.stream = __libbz__.bzopen(obj.filename, obj.mode);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && base.is_open(obj)) {
		__libbz__.bzclose(obj.stream);
		obj.stream = null;
	}
};

pt.read = function (obj, buffer) {
	return __libbz__.bzread(obj.stream, buffer);
};

pt.readEx = function (obj, size) {
	return __libbz__.bzreadEx(obj.stream, size);
};

pt.write = function (obj, buffer) {
	return __libbz__.bzwrite(obj.stream, buffer);
};

pt.sync = pt.flush = function (obj) {
	return __libbz__.bzflush(obj.stream);
};

pt.error = function (obj) {
	return __libbz__.bzerror(obj.stream);
};

/* eof */
