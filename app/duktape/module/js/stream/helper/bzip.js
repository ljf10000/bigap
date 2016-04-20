#!/bin/js

/*
* module: bzip stream helper
*/
var mod = this,
	name = 'stream/helper/bzip',
	base = require('stream/helper/base');

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.is_open = base.is_open;
mod.is_close = base.is_close;

mod.open = function (obj, mode) {
	if (obj && mod.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libbz__.bzopen(obj.filename, obj.mode);
	}

	return obj;
};

mod.close = function (obj) {
	if (obj && mod.is_open(obj)) {
		__libbz__.bzclose(obj.stream);
		obj.stream = null;
	}
};

mod.read = function (obj, buffer) {
	return __libbz__.bzread(obj.stream, buffer);
};

mod.readEx = function (obj, size) {
	return __libbz__.bzreadEx(obj.stream, size);
};

mod.write = function (obj, buffer) {
	return __libbz__.bzwrite(obj.stream, buffer);
};

mod.sync = mod.flush = function (obj) {
	return __libbz__.bzflush(obj.stream);
};

mod.error = function (obj) {
	return __libbz__.bzerror(obj.stream);
};

mod.stream = function (obj, name, filename, mode, type) {
	if (obj.constructor === Object) {
		__js__.destructor(false, obj, mod.close);
	}

	return base.stream(obj, name, filename, mode, type);
};

/* eof */
