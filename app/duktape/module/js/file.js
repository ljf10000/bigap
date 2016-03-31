#!/bin/js

/*
* module: file
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.__proto__ = require('fd').constructor.prototype;
pt.$name = pt.$name || 'file';
pt.$debugger = new $Debugger(pt.$name);

pt.open = function (obj) {
	if (obj && pt.is_close(obj)) {
		obj.fd = __libc__.open(obj.filename, obj.flag, obj.mode);
	}

	return obj;
};

pt.stat = function (obj) {
	return __libc__.fstat(obj.fd);
};

pt.seek = function (obj, offset, where) {
	return __libc__.fseek(obj.fd, offset, where);
};

pt.sync = function (obj) {
	return __libc__.fsync(obj.fd);
};

pt.flush = pt.sync;

mod.File = function (filename, flag, mode, open) {
	var obj = {
		filename: filename,
		flag: flag || __libc__.O_RDONLY,
		mode: mode || 0,

		fd: -1
	};

	if (true === open) {
		pt.open(obj);
	}

	return obj;
};

mod.File.prototype = {
	open: function () {
		return pt.open(this);
	},

	close: function () {
		pt.close(this);
	},

	read: function (buffer) {
		return pt.read(this, buffer);
	},

	readEx: function (size) {
		return pt.readEx(this, size);
	},

	readv: function (buffers) {
		return pt.readv(this, buffers);
	},

	write: function (buffer) {
		return pt.write(this, buffer);
	},

	writev: function (buffers) {
		return pt.writev(this, buffers);
	},

	seek: function (offset, where) {
		return pt.seek(this, offset, where);
	},

	sync: function () {
		return pt.sync(this);
	},

	flush: function () {
		return pt.flush(this);
	}
};

__js__.destructor(true, mod.File.prototype, pt.close);

/* eof */
