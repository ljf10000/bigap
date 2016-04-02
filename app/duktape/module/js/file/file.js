#!/bin/js

/*
* module: file
*/
var mod = this,
	pt = mod.constructor.prototype,
	helper = {
		file: require('file/helper/file'),
		dir: require('fs/helper/dir')
	},
	fmode = require('file/helper/mode');

pt.$name = pt.$name || 'file/file';
pt.$debugger = new $Debugger(pt.$name);

function method (obj, funcname, fsafe) {
	return safefun(helper[obj.type][funcname], fsafe);
}

pt.is_open = function is_open (obj) {
	return method(obj, 'is_open', allways_false)(obj);
};

pt.is_close = function is_close (obj) {
	return method(obj, 'is_close', allways_true)(obj);
};

pt.open = function (obj, flag, mode) {
	return method(obj, 'open', function () {return obj;})(obj, flag, mode);
};

pt.close = function (obj) {
	return method(obj, 'close')(obj);
};

pt.stat = function (obj) {
	return method(obj, 'stat')(obj);
};

pt.lstat = function (obj) {
	return method(obj, 'lstat')(obj);
};

pt.seek = function (obj, offset, where) {
	return method(obj, 'seek')(obj, offset, where);
};

pt.rewind = function (obj) {
	return method(obj, 'rewind')(obj);
};

pt.tell = function (obj) {
	return method(obj, 'tell')(obj);
};

pt.sync = pt.flush = function (obj) {
	return method(obj, 'flush')(obj);
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
	is_open: function () {
		return pt.is_open(this);
	},

	is_close: function () {
		return pt.is_close(this);
	},

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
