#!/bin/js

/*
* module: file helper
*/
var mod = this,
	name = 'file/helper/file',
	base = require('file/helper/base'),
	fmode = require('file/helper/mode'),
	fd = require('file/helper/fd');

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);


mod.is_open = fd.is_open;
mod.is_close= fd.is_close;
mod.close   = fd.close;
mod.fcntl   = fd.fcntl;
mod.dup     = fd.dup;
mod.dup2    = fd.dup2;
mod.read    = fd.read;
mod.readEx  = fd.readEx;
mod.readv   = fd.readv;
mod.write   = fd.write;
mod.writev  = fd.writev;

mod.open = function (obj, flag, mode) {
	if (must_object(obj) && mod.is_close(obj)) {
		flag = maybe_number(flag) || obj.flag;
		mode = maybe_number(mode) || obj.mode;

		obj.fd = __libc__.open(obj.filename, flag, mode);
		if (mod.is_open(obj)) {
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

mod.stat = function (obj) {
	return __libc__.stat(obj.filename);
};

mod.lstat = function (obj) {
	return __libc__.lstat(obj.filename);
};

mod.seek = function (obj, offset, where) {
	return __libc__.lseek(obj.fd, offset, where);
};

mod.rewind = function (obj) {
	var type = mod.type;

	switch(obj.type) {
		case type.file:
			return __libc__.lseek(obj.fd, 0, __libc__.SEEK_SET);
		case type.dir:
			return __libc__.rewinddir(obj.dir);
		default:
			return no_support();
	}
};

mod.sync = mod.flush = function (obj) {
	return __libc__.fsync(obj.fd);
};

mod.stream = function (obj, name, filename, flag, mode) {
	if (obj.constructor === Object) {
		__js__.destructor(false, obj, mod.close);
	}

	return base.file(obj, name, filename, flag, mode);
};

/* eof */
