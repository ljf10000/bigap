#!/bin/js

/*
* module: zlib stream helper
*/
var mod = this,
	name = 'stream/helper/zlib',
	base = require('stream/helper/base');

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.is_open = base.is_open;
mod.is_close = base.is_close;

mod.open = function (obj, mode) {
	if (obj && mod.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libz__.gzopen(obj.filename, obj.mode);
	}

	return obj;
};

mod.close = function (obj) {
	if (obj && mod.is_open(obj)) {
		__libz__.gzclose(obj.stream);
		obj.stream = null;
	}
};

mod.read = function (obj, buffer) {
	return __libz__.gzread(obj.stream, buffer);
};

mod.readEx = function (obj, size) {
	return __libz__.gzreadEx(obj.stream, size);
};

mod.write = function (obj, buffer) {
	return __libz__.gzwrite(obj.stream, buffer);
};

mod.tell = function (obj) {
	return __libz__.gztell(obj.stream);
};

mod.seek = function (obj, offset, where) {
	return __libz__.gzseek(obj.stream, offset, where);
};

mod.rewind = function (obj) {
	return __libz__.gzseek(obj.stream, 0, __libc__.SEEK_SET);
};

mod.sync = mod.flush = function (obj) {
	return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
};

mod.error = function (obj) {
	return __libz__.gzerror(obj.stream);
};

mod.eof = function (obj) {
	return __libz__.gzeof(obj.stream);
};

mod.stream = function (obj, name, filename, mode, type) {
	if (obj.constructor === Object) {
		__js__.destructor(false, obj, mod.close);
	}

	return base.stream(obj, name, filename, mode, type);
};

/* eof */
