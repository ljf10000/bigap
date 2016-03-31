#!/bin/js

/*
* module: fd
*/

var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'fd';
pt.$debugger = new ModDebugger(pt.$name);

pt.is_good = function (obj) {
	return typeof obj.fd === 'number' && obj.fd >= 0;
};

pt.close = function (obj) {
	if (obj && pt.is_good(obj)) {
		__libc__.close(obj.fd);

		obj.fd = -1;
	}

	return 0;
};

pt.fcntl = function (obj, cmd, flag) {
	if (undefined===flag) {
		return __libc__.fcntl(obj.fd, cmd);
	} else {
		return __libc__.fcntl(obj.fd, cmd, flag);
	}
};

pt.dup = function (obj) {
	return __libc__.dup(obj.fd);
};

pt.dup2 = function (obj, newfd) {
	return __libc__.dup2(obj.fd, newfd);
};

pt.read = function (obj, buffer) {
	return __libc__.read(obj.fd, buffer);
};

pt.readEx = function (obj, size) {
	return __libc__.readEx(obj.fd, size);
};

pt.readv = function (obj, buffers) {
	return __libc__.readv(obj.fd, buffers);
};

pt.write = function (obj, buffer) {
	return __libc__.write(obj.fd, buffer);
};

pt.writev = function (obj, buffers) {
	return __libc__.writev(obj.fd, buffers);
};

/* eof */
