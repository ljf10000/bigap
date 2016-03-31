#!/bin/js

/*
* module: pt
*/
var mod = this;
var pt = mod.constructor.prototype;

pt.__proto__ = require('fd').constructor.prototype;
pt.$name = pt.$name || 'socket';
pt.$debugger = new ModDebugger(pt.$name);

pt.open = function (obj, domain, type, protocol) {
	if (obj && !pt.is_good(obj)) {
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
	}
};

__js__.destructor(true, mod.Socket.prototype, pt.close);
/* eof */
