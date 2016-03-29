#!/bin/js

/*
* module: file
*/

var fd = require('fd');

var mod = this;
var pt = mod.constructor.prototype;

pt.__proto__ = fd.constructor.prototype;
pt.name = pt.name || 'file';
pt.debugger = new ModDebugger(pt.name);

pt.open = function (obj, filename, flag, mode) {
	if (obj && !fd.is_good(obj)) {
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

mod.File = function (filename, flag, mode) {
	return pt.open(this, filename, flag, mode);
};

mod.File.prototype = {
	read: function (buffer) {
		return fd.read(this, buffer);
	},

	readEx: function (size) {
		return fd.readEx(this, size);
	},

	readv: function (buffers) {
		return fd.readv(this, buffers);
	},

	write: function (buffer) {
		return fd.write(this, buffer);
	},

	writev: function (buffers) {
		return fd.writev(this, buffers);
	},

	seek: function (offset, where) {
		return fd.seek(this, offset, where);
	}
};

__js__.destructor(true, mod.File.prototype, fd.close);

/* eof */
