#!/bin/js

/*
* module: mod
*/
var mod = this,
	name = 'net/helper/socket';

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

mod.open = function(obj, domain, type, protocol) {
	if (obj && !mod.is_good(obj)) {
		obj.domain = domain;
		obj.type = type;
		obj.protocol = protocol || 0;

		obj.fd = __libc__.socket(domain, type, obj.protocol);
	}

	return obj;
};

mod.Socket = function(domain, type, protocol) {
	return mod.open(this, domain, type, protocol);
};

mod.Socket.prototype = {
	read: function(buffer) {
		return mod.read(this, buffer);
	},

	readEx: function(size) {
		return mod.readEx(this, size);
	},

	readv: function(buffers) {
		return mod.readv(this, buffers);
	},

	write: function(buffer) {
		return mod.write(this, buffer);
	},

	writev: function(buffers) {
		return mod.writev(this, buffers);
	},

	seek: function(offset, where) {
		return mod.seek(this, offset, where);
	}
};

__js__.destructor(true, mod.Socket.prototype, mod.close);
/* eof */
