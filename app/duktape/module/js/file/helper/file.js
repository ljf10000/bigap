#!/bin/js

/*
* module: file helper
*/
var mod = this,
	name = 'file/helper/file',
	fmode = require('file/helper/mode'),
	fd = require('file/helper/fd');

mod.$name = function() { return name; };
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
mod.init    = fd.init;

mod.open = function(obj, flag, mode) {
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
			obj.ftype = obj.ftype || fmode.ftype(obj.fmode);
		}
	}

	return obj;
};

mod.stat = function(obj) {
	return __libc__.stat(obj.filename);
};

mod.lstat = function(obj) {
	return __libc__.lstat(obj.filename);
};

mod.seek = function(obj, offset, where) {
	return __libc__.lseek(obj.fd, offset, where);
};

mod.rewind = function(obj) {
	return __libc__.lseek(obj.fd, 0, __libc__.SEEK_SET);
};

mod.sync = mod.flush = function(obj) {
	return __libc__.fsync(obj.fd);
};

/* eof */
