#!/bin/js

/*
* module: file
*/
var mod = this,
	name = 'file/file',
	fmode = require('file/helper/mode'),
	helpers = {
		file: require('file/helper/file'),
		dir: require('fs/helper/dir')
	};

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

function method (obj, funcname, fsafe) {
	return safe_function(helpers[obj.type][funcname], fsafe);
}

mod.is_open = function is_open (obj) {
	return method(obj, 'is_open', allways_false)(obj);
};

mod.is_close = function is_close (obj) {
	return method(obj, 'is_close', allways_true)(obj);
};

mod.open = function (obj, flag, mode) {
	return method(obj, 'open', function () {return obj;})(obj, flag, mode);
};

mod.close = function (obj) {
	return method(obj, 'close')(obj);
};

mod.stat = function (obj) {
	return method(obj, 'stat')(obj);
};

mod.lstat = function (obj) {
	return method(obj, 'lstat')(obj);
};

mod.seek = function (obj, offset, where) {
	return method(obj, 'seek')(obj, offset, where);
};

mod.rewind = function (obj) {
	return method(obj, 'rewind')(obj);
};

mod.tell = function (obj) {
	return method(obj, 'tell')(obj);
};

mod.sync = mod.flush = function (obj) {
	return method(obj, 'flush')(obj);
};

mod.read = function (obj, buffer) {
	return method(obj, 'read')(obj, buffer);
};

mod.readEx = function (obj, size) {
	return method(obj, 'readEx')(obj, size);
};

mod.readv = function (obj, buffers) {
	return method(obj, 'readv')(obj, buffers);
};

mod.write = function (obj, buffer) {
	return method(obj, 'write')(obj, buffer);
};

mod.writev = function (obj, buffers) {
	return method(obj, 'writev')(obj, buffers);
};

function init (obj, filename, flag, mode) {
	obj.fd = null;
	obj.filename = maybe_string(filename);
	obj.flag = maybe_number(flag) || __libc__.O_RDONLY;
	obj.mode = maybe_number(mode) || 0;
	obj.fmode = __libc__.fexist(obj.filename)?__libc__.lstat(obj.filename).mode: 0;
	obj.ftype = fmode.ftype(obj.fmode);
	obj.$name = function () { return name + '(' + obj.filename + ')'; };

	__js__.destructor(obj, mod.close);

	return obj;
}

var pt = {
	is_open: function () {
		return mod.is_open(this);
	},

	is_close: function () {
		return mod.is_close(this);
	},

	open: function (flag, mode) {
		return mod.open(this, flag, mode);
	},

	close: function () {
		mod.close(this);
	},

	read: function (buffer) {
		return mod.read(this, buffer);
	},

	readEx: function (size) {
		return mod.readEx(this, size);
	},

	readv: function (buffers) {
		return mod.readv(this, buffers);
	},

	write: function (buffer) {
		return mod.write(this, buffer);
	},

	writev: function (buffers) {
		return mod.writev(this, buffers);
	},

	seek: function (offset, where) {
		return mod.seek(this, offset, where);
	},

	sync: function () {
		return mod.sync(this);
	},

	flush: function () {
		return mod.flush(this);
	}
};

mod.object = function (filename, flag, mode) {
	return Object.setPrototypeOf(init({}, filename, flag, mode), pt);
};

/* eof */
