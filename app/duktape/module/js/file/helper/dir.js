#!/bin/js

/*
* module: dir helper
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'file/helper/dir';
pt.$debugger = new $Debugger(pt.$name);

pt.is_open = function is_open (obj) {
	return typeof obj.dir === 'pointer' && obj.dir;
};

pt.is_close = function is_close (obj) {
	return null === obj.dir;
};

pt.open = function (obj) {
	if (obj && is_close(obj)) {
		obj.dir = __libc__.opendir(obj.filename);
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && is_open(obj)) {
		__libc__.closedir(obj.dir);
		obj.dir = null;
	}

	return 0;
};

pt.read = function (obj) {
	return __libc__.readdir(obj.dir);
};

pt.tell = function (obj) {
	return __libc__.telldir(obj.dir);
};

pt.seek = function (obj, pos) {
	return __libc__.seekdir(obj.dir, pos);
};

pt.rewind = function (obj) {
	return __libc__.rewinddir(obj.dir);
};

/* eof */
