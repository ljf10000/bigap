#!/bin/js

/*
* module: zlib stream helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/helper/zlib',
	base = require('stream/helper/base');

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.is_open = base.is_open;
pt.is_close = base.is_close;

pt.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libz__.gzopen(obj.filename, obj.mode);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && pt.is_open(obj)) {
		__libz__.gzclose(obj.stream);
		obj.stream = null;
	}
};

pt.read = function (obj, buffer) {
	return __libz__.gzread(obj.stream, buffer);
};

pt.readEx = function (obj, size) {
	return __libz__.gzreadEx(obj.stream, size);
};

pt.write = function (obj, buffer) {
	return __libz__.gzwrite(obj.stream, buffer);
};

pt.tell = function (obj) {
	return __libz__.gztell(obj.stream);
};

pt.seek = function (obj, offset, where) {
	return __libz__.gzseek(obj.stream, offset, where);
};

pt.rewind = function (obj) {
	return __libz__.gzseek(obj.stream, 0, __libc__.SEEK_SET);
};

pt.sync = pt.flush = function (obj) {
	return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
};

pt.error = function (obj) {
	return __libz__.gzerror(obj.stream);
};

pt.eof = function (obj) {
	return __libz__.gzeof(obj.stream);
};

pt.stream = function (obj, name, filename, mode, type) {
	if (obj.constructor === Object) {
		__js__.destructor(false, obj, pt.close);
	}

	return base.stream(obj, name, filename, mode, type);
};

/* eof */
