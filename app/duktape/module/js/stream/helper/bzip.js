#!/bin/js

/*
* module: bzip stream helper
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.__proto__ = require('stream/helper/base').constructor.prototype;
pt.$name = pt.$name || 'stream/helper/bzip';
pt.$debugger = new $Debugger(pt.$name);

pt.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libbz__.bzopen(obj.filename, obj.mode);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && pt.is_open(obj)) {
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
