#!/bin/js

/*
* module: stream
*/
var mod = this,
	pt = mod.constructor.prototype,
	helper = {
		file: require('stream/helper/file'),
		pipe: require('stream/helper/pipe'),
		zlib: require('stream/helper/zlib'),
		bzip: require('stream/helper/bzip')
	};

pt.__proto__ = require('stream/helper/base').constructor.prototype;
pt.$name = pt.$name || 'stream/stream';
pt.$debugger = new $Debugger(pt.$name);

function method (obj, funcname, fsafe) {
	return safefun(helper[obj.type][funcname], fsafe);
}

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
	var obj = {
		    filename: filename,
		  	mode: mode || 'r',
		  	type: helper.hasOwnProperty(type)?type:'file',
		  	$name: pt.$name + '(' + filename + ')',
		  	stream: null
	    };

	if (true === pre_open) {
		pt.open(obj);
	}

	return obj;
};

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

__js__.destructor(true, mod.Stream.prototype, pt.close);

/* eof */
