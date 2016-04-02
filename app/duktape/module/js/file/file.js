#!/bin/js

/*
* module: file
*/
var mod = this,
	pt = mod.constructor.prototype,
	fd = require('io/helper/fd'),
	dir = require('fs/helper/dir'),
	fmode = require('file/helper/mode');

pt.$name = pt.$name || 'file/file';
pt.$debugger = new $Debugger(pt.$name);

pt.type = {
	file: 1,
	pipe: 2,
	sock: 3,
	dir: 4
};

function method (obj, funcname) {
	var type = pt.type;
	var helper;

	switch(obj.type) {
		case type.dir:
			helper = dir;
			break;
		case type.file:
		case type.pipe:
		case type.sock:
		default:
			helper = fd;
			break;
	}

	return helper[funcname] || no_support;
}

pt.is_open = function is_open (obj) {
	return method(obj, 'is_open')(obj);
};

pt.is_close = function is_close (obj) {
	return method(obj, 'is_close')(obj);
};

pt.open = function (obj, flag, mode) {
	if (obj && pt.is_close(obj)) {
		var type = pt.type;

		switch(obj.type) {
			case type.file:
			case type.pipe:
				flag = flag || obj.flag;
				mode = mode || obj.mode;

				obj.fd = __libc__.open(obj.filename, flag, mode);
				if (fd.is_open(obj)) {
					obj.flag = flag;
					obj.mode = mode;

					/*
					* maybe file not exist, but after open, the file is created
					*/
					obj.fmode = obj.fmode || __libc__.lstat(filename).mode;
					obj.ftype = obj.ftype || fmode.get_type(obj.fmode);
				}

				break;
			case type.sock:
				break;
			case type.dir:
				obj.dir = __libc__.opendir(obj.filename);
				break;
			default:
				break;
		}
	}

	return obj;
};

pt.close = function (obj) {
	return method(obj, 'close')(obj);
};

pt.stat = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
		case type.dir:
			return __libc__.stat(obj.filename);
		default:
			return no_support();
	}
};

pt.lstat = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
		case type.dir:
			return __libc__.lstat(obj.filename);
		default:
			return no_support();
	}
};

pt.seek = function (obj, offset, where) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.lseek(obj.fd, offset, where);
		case type.dir:
			return __libc__.seekdir(obj.dir, offset/* pos */);
		default:
			return no_support();
	}
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

pt.tell = function (obj) {
	return method(obj, 'tell')(obj);
};

pt.sync = pt.flush = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.fsync(obj.fd);
		default:
			return no_support();
	}
};

pt.read = function (obj, buffer) {
	return method(obj, 'read')(obj, buffer);
};

pt.readEx = function (obj, size) {
	return method(obj, 'readEx')(obj, size);
};

pt.readv = function (obj, buffers) {
	return method(obj, 'readv')(obj, buffers);
};

pt.write = function (obj, buffer) {
	return method(obj, 'write')(obj, buffer);
};

pt.writev = function (obj, buffers) {
	return method(obj, 'writev')(obj, buffers);
};

mod.File = function (filename, flag, mode, pre_open) {
	var tmp_fmode = __libc__.fexist(filename)?__libc__.lstat(filename).mode: 0,
		obj = {
			filename: filename,
			flag: flag || __libc__.O_RDONLY,
			mode: mode || 0,
			fmode: tmp_fmode,
			ftype: fmode.get_type(tmp_fmode),
			fd: -1
		};

	if (true === pre_open) {
		pt.open(obj);
	}

	return obj;
};

mod.File.prototype = {
	open: function (flag, mode) {
		return pt.open(this, flag, mode);
	},

	close: function () {
		pt.close(this);
	},

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
	},

	sync: function () {
		return pt.sync(this);
	},

	flush: function () {
		return pt.flush(this);
	}
};

__js__.destructor(true, mod.File.prototype, pt.close);

/* eof */
