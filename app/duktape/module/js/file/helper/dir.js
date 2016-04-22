#!/bin/js

/*
* module: dir helper
*/
var mod = this,
	name = 'file/helper/dir',
	base = require('file/helper/base');

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

mod.is_open = function is_open (obj) {
	return typeof obj.fd === 'pointer' && obj.fd;
};

mod.is_close = function is_close (obj) {
	return null === obj.fd;
};

mod.open = function (obj) {
	if (obj && is_close(obj)) {
		obj.fd = __libc__.opendir(obj.filename);
	}

	return obj;
};

mod.close = function (obj) {
	if (obj && is_open(obj)) {
		__libc__.closedir(obj.fd);
		obj.fd = null;
	}

	return 0;
};

mod.read = function (obj) {
	return __libc__.readdir(obj.fd);
};

mod.tell = function (obj) {
	return __libc__.telldir(obj.fd);
};

mod.seek = function (obj, pos) {
	return __libc__.seekdir(obj.fd, pos);
};

mod.rewind = function (obj) {
	return __libc__.rewinddir(obj.fd);
};

/* eof */
