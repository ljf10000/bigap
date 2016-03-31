#!/bin/js

/*
* module: file
*/
var mod = this;
var pt = mod.constructor.prototype;

pt.__proto__ = require('fd').constructor.prototype;
pt.$name = pt.$name || 'file';
pt.$debugger = new ModDebugger(pt.$name);

pt.open = function (obj, filename, flag, mode) {
	if (obj && !pt.is_good(obj)) {
		obj.filename = filename;
		obj.flag = flag || __libc__.O_RDONLY;
		obj.mode = mode || 0;

		obj.fd = __libc__.open(filename, obj.flag, obj.mode);
	}

	return obj;
};

pt.seek = function (obj, offset, where) {
	return __libc__.fseek(obj.fd, offset, where);
};

pt.sync = function (obj) {
	return __libc__.fsync(obj.fd);
};

pt.flush = pt.sync;

mod.File = function (filename, flag, mode) {
	return pt.open(this, filename, flag, mode);
};

mod.File.prototype = {
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
