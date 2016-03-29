#!/bin/js

/*
* module: fd
*/

var fd = require('fd');

var mod = this;
var pt = mod.constructor.prototype;

pt.__proto__ = fd.constructor.prototype;
pt.name = pt.name || 'socket';
pt.debugger = new ModDebugger(pt.name);

pt.open = function (obj, domain, type, protocol) {
	if (obj && !fd.is_good(obj)) {
		obj.domain = domain;
		obj.type = type;
		obj.protocol = protocol || 0;

		obj.fd = __libc__.socket(domain, type, obj.protocol);
	}

	return obj;
};

mod.Socket = function (domain, type, protocol) {
	return pt.open(this, domain, type, protocol);
};

mod.Socket.prototype = {
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

__js__.destructor(true, mod.Socket.prototype, fd.close);
/* eof */
