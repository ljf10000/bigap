#!/bin/js

/*
* module: stream
*/
var mod = this,
	pt = mod.constructor.prototype,
	stream = require('helper/stream');

pt.__proto__ = stream.constructor.prototype;
pt.$name = pt.$name || 'stream';
pt.$debugger = new $Debugger(pt.$name);

mod.Stream = function (filename, mode, type, pre_open) {
	var obj = {
		filename: filename,
		mode: mode || 'r',
		type: type || pt.type.file,
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
