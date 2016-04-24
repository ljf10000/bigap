#!/bin/js

/*
* module: fd helper
*/
var mod = this,
	name = 'file/helper/fd';

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

mod.is_open = function(obj) {
	return typeof obj.fd === 'number' && obj.fd >= 0;
};

mod.is_close = function(obj) {
	return null === obj.fd || -1 === obj.fd;
};

mod.close = function(obj) {
	if (obj && mod.is_open(obj)) {
		__libc__.close(obj.fd);
		obj.fd = null;
	}

	return 0;
};

mod.fcntl = function(obj, cmd, flag) {
	if (undefined === flag) {
		return __libc__.fcntl(obj.fd, cmd);
	} else {
		return __libc__.fcntl(obj.fd, cmd, flag);
	}
};

mod.dup = function(obj) {
	return __libc__.dup(obj.fd);
};

mod.dup2 = function(obj, newfd) {
	return __libc__.dup2(obj.fd, newfd);
};

mod.read = function(obj, buffer) {
	return __libc__.read(obj.fd, buffer);
};

mod.readEx = function(obj, size) {
	return __libc__.readEx(obj.fd, size);
};

mod.readv = function(obj, buffers) {
	return __libc__.readv(obj.fd, buffers);
};

mod.write = function(obj, buffer) {
	return __libc__.write(obj.fd, buffer);
};

mod.writev = function(obj, buffers) {
	return __libc__.writev(obj.fd, buffers);
};

/* eof */
