#!/bin/js

/*
* module: stream
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/stream',
	helper = {
		file: require('stream/helper/file'),
		pipe: require('stream/helper/pipe'),
		zlib: require('stream/helper/zlib'),
		bzip: require('stream/helper/bzip')
	};

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

function method (obj, funcname, fsafe) {
	print('method type=', obj.type);
	return safe_function(helper[obj.type][funcname], fsafe);
}

pt.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function (obj) {
	return null === obj.stream;
};

pt.open = function (obj, mode) {
	print('stream pt open mode=', mode);
	return method(obj, 'open', function() {return obj;})(obj, mode);
};

pt.close = function (obj) {
	print('stream pt close');
	return method(obj, 'close')(obj);
};

pt.read = function (obj, buffer) {
	print('stream pt read');
	return method(obj, 'read')(obj, buffer);
};

pt.readEx = function (obj, size) {
	print('stream pt readEx');
	return method(obj, 'readEx')(obj, size);
};

pt.write = function (obj, buffer) {
	print('stream pt write');
	return method(obj, 'write')(obj, buffer);
};

pt.tell = function (obj) {
	print('stream pt tell');
	return method(obj, 'tell')(obj);
};

pt.seek = function (obj, offset, where) {
	print('stream pt seek');
	return method(obj, 'seek')(obj, offset, where);
};

pt.rewind = function (obj) {
	print('stream pt rewind');
	return method(obj, 'rewind')(obj);
};

pt.sync = pt.flush = function (obj) {
	print('stream pt sync');
	return method(obj, 'sync')(obj);
};

pt.error = function (obj) {
	print('stream pt error');
	return method(obj, 'error')(obj);
};

pt.eof = function (obj) {
	print('stream pt eof');
	return method(obj, 'eof', allways_true)(obj);
};

mod.Stream = function (filename, mode, type, pre_open) {
	var tmp_filename = maybe_string(filename);

	this.filename = tmp_filename;
	this.mode = maybe_string(mode) || 'r';
	this.type = (must_string(type) && helper.hasOwnProperty(type))?type:'file';
	this.$name = function () { return pt.$name() + '(' + tmp_filename + ')'; };
	this.stream = null;

	print('stream.open pre_open=', pre_open);

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
