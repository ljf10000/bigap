#!/bin/js

/*
* module: stream
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/stream',
	base = require('stream/helper/base'),
	helper = {
		file: require('stream/helper/file'),
		pipe: require('stream/helper/pipe'),
		zlib: require('stream/helper/zlib'),
		bzip: require('stream/helper/bzip')
	};

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

function method (obj, funcname, fsafe) {
	return safe_function(helper[obj.type][funcname], fsafe);
}

pt.stream = base.stream;
pt.is_open = base.is_open;
pt.is_close = base.is_close;

pt.open = function (obj, mode) {
	return method(obj, 'open', function() {return obj;})(obj, mode);
};

pt.close = function (obj) {
	return method(obj, 'close')(obj);
};

pt.read = function (obj, buffer) {
	return method(obj, 'read')(obj, buffer);
};

pt.readEx = function (obj, size) {
	return method(obj, 'readEx')(obj, size);
};

pt.write = function (obj, buffer) {
	return method(obj, 'write')(obj, buffer);
};

pt.tell = function (obj) {
	return method(obj, 'tell')(obj);
};

pt.seek = function (obj, offset, where) {
	return method(obj, 'seek')(obj, offset, where);
};

pt.rewind = function (obj) {
	return method(obj, 'rewind')(obj);
};

pt.sync = pt.flush = function (obj) {
	return method(obj, 'sync')(obj);
};

pt.error = function (obj) {
	return method(obj, 'error')(obj);
};

pt.eof = function (obj) {
	return method(obj, 'eof', allways_true)(obj);
};

mod.Stream = function (filename, mode, type, pre_open) {
	base.stream(this,
	            pt.$name(),
	            maybe_string(filename),
	            mode,
	            helper.hasOwnProperty(type)?type:'file');

	if (true === pre_open) {
		pt.open(this);
	}
};

mod.Stream.prototype = {
	is_open: function () {
		return pt.is_open(this);
	},

	is_close: function () {
		return pt.is_close(this);
	},

	open: function (mode) {
		print('stream.open mode=', mode);
		return pt.open(this, mode);
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

	write: function (buffer) {
		return pt.write(this, buffer);
	},

	tell: function () {
		return pt.tell(this);
	},

	seek: function (offset, where) {
		return pt.seek(this, offset, where);
	},

	rewind: function () {
		return pt.rewind(this);
	},

	sync: function () {
		return pt.sync(this);
	},

	flush: function () {
		return pt.flush(this);
	},

	error: function () {
		return pt.error(this);
	},

	eof: function () {
		return pt.eof(this);
	}
};

__js__.destructor(true, mod.Stream.prototype, pt.close);

/* eof */
