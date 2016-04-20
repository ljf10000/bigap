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
	return typeof obj.dir === 'pointer' && obj.dir;
};

mod.is_close = function is_close (obj) {
	return null === obj.dir;
};

mod.open = function (obj) {
	if (obj && is_close(obj)) {
		obj.dir = __libc__.opendir(obj.filename);
	}

	return obj;
};

mod.close = function (obj) {
	if (obj && is_open(obj)) {
		__libc__.closedir(obj.dir);
		obj.dir = null;
	}

	return 0;
};

mod.read = function (obj) {
	return __libc__.readdir(obj.dir);
};

mod.tell = function (obj) {
	return __libc__.telldir(obj.dir);
};

mod.seek = function (obj, pos) {
	return __libc__.seekdir(obj.dir, pos);
};

mod.rewind = function (obj) {
	return __libc__.rewinddir(obj.dir);
};

mod.stream = function (obj, name, filename, flag, mode) {
	if (obj.constructor === Object) {
		__js__.destructor(false, obj, mod.close);
	}

	return base.file(obj, name, filename, flag, mode);
};

/* eof */
