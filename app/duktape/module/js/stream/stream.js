#!/bin/js

/*
* module: stream
*/
print('stream 0');
var mod = this,
	pt = mod.__proto__,
	name = 'stream/stream',
	helper = {
		file: require('stream/helper/file'),
		pipe: require('stream/helper/pipe'),
		zlib: require('stream/helper/zlib'),
		bzip: require('stream/helper/bzip')
	};
print('stream 1');
pt.constructor.prototype = require('stream/helper/base').__proto__;
pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);
print('stream 2');
function method (obj, funcname, fsafe) {
	return safe_function(helper[obj.type][funcname], fsafe);
}
print('stream 3');
pt.open = function (obj, mode) {
	return method(obj, 'open', function() {return obj;})(obj, mode);
};
print('stream 4');
pt.close = function (obj) {
	return method(obj, 'close')(obj);
};
print('stream 5');
pt.read = function (obj, buffer) {
	return method(obj, 'read')(obj, buffer);
};
print('stream 6');
pt.readEx = function (obj, size) {
	return method(obj, 'readEx')(obj, size);
};
print('stream 7');
pt.write = function (obj, buffer) {
	return method(obj, 'write')(obj, buffer);
};
print('stream 8');
pt.tell = function (obj) {
	return method(obj, 'tell')(obj);
};
print('stream 9');
pt.seek = function (obj, offset, where) {
	return method(obj, 'seek')(obj, offset, where);
};
print('stream 10');
pt.rewind = function (obj) {
	return method(obj, 'rewind')(obj);
};
print('stream 11');
pt.sync = pt.flush = function (obj) {
	return method(obj, 'sync')(obj);
};
print('stream 12');
pt.error = function (obj) {
	return method(obj, 'error')(obj);
};
print('stream 13');
pt.eof = function (obj) {
	return method(obj, 'eof', allways_true)(obj);
};
print('stream 14');
mod.Stream = function (filename, mode, type, pre_open) {
	var tmp_filename = maybe_string(filename),
		obj = {
		    filename: tmp_filename,
		  	mode: maybe_string(mode) || 'r',
		  	type: (must_string(type) && helper.hasOwnProperty(type))?type:'file',
		  	$name: pt.$name() + '(' + tmp_filename + ')',
		  	stream: null
	    };

	if (true === pre_open) {
		pt.open(obj);
	}

	return obj;
};
print('stream 15');
mod.Stream.prototype = {
	is_open: function () {
		return pt.is_open(this);
	},

	is_close: function () {
		return pt.is_close(this);
	},

	open: function (mode) {
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
print('stream 16');
__js__.destructor(true, mod.Stream.prototype, pt.close);
print('stream 17');
/* eof */
