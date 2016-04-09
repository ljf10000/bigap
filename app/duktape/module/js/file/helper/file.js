#!/bin/js

/*
* module: file helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'file/helper/file';
	fmode = require('file/helper/mode');

pt.__proto__ = require('file/helper/fd').__proto__;
pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.open = function (obj, flag, mode) {
	if (obj && pt.is_close(obj)) {
		flag = flag || obj.flag;
		mode = mode || obj.mode;

		obj.fd = __libc__.open(obj.filename, flag, mode);
		if (pt.is_open(obj)) {
			obj.flag = flag;
			obj.mode = mode;

			/*
			* maybe file not exist, but after open, the file is created
			*/
			obj.fmode = obj.fmode || __libc__.lstat(obj.filename).mode;
			obj.ftype = obj.ftype || fmode.get_type(obj.fmode);
		}
	}

	return obj;
};

pt.stat = function (obj) {
	return __libc__.stat(obj.filename);
};

pt.lstat = function (obj) {
	return __libc__.lstat(obj.filename);
};

pt.seek = function (obj, offset, where) {
	return __libc__.lseek(obj.fd, offset, where);
};

pt.rewind = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.lseek(obj.fd, 0, __libc__.SEEK_SET);
		case type.dir:
			return __libc__.rewinddir(obj.dir);
		default:
			return no_support();
	}
};

pt.sync = pt.flush = function (obj) {
	return __libc__.fsync(obj.fd);
};

/* eof */
