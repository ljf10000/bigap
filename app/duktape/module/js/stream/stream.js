#!/bin/js

/*
* module: stream
*/
var mod = this,
	name = 'stream/stream',
	base = require('stream/helper/base'),
	helper = {
		file: require('stream/helper/file'),
		pipe: require('stream/helper/pipe'),
		zlib: require('stream/helper/zlib'),
		bzip: require('stream/helper/bzip')
	};

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

function method (obj, funcname, fsafe) {
	return safe_function(helper[obj.type][funcname], fsafe);
}

mod.is_open = base.is_open;
mod.is_close = base.is_close;

mod.open = function (obj, mode) {
	return method(obj, 'open', function() {return obj;})(obj, mode);
};

mod.close = function (obj) {
	return method(obj, 'close')(obj);
};

mod.read = function (obj, buffer) {
	return method(obj, 'read')(obj, buffer);
};

mod.readEx = function (obj, size) {
	return method(obj, 'readEx')(obj, size);
};

mod.write = function (obj, buffer) {
	return method(obj, 'write')(obj, buffer);
};

mod.tell = function (obj) {
	return method(obj, 'tell')(obj);
};

mod.seek = function (obj, offset, where) {
	return method(obj, 'seek')(obj, offset, where);
};

mod.rewind = function (obj) {
	return method(obj, 'rewind')(obj);
};

mod.sync = mod.flush = function (obj) {
	return method(obj, 'sync')(obj);
};

mod.error = function (obj) {
	return method(obj, 'error')(obj);
};

mod.eof = function (obj) {
	return method(obj, 'eof', allways_true)(obj);
};

mod.stream = function (obj, name, filename, mode, type) {
	if (obj.constructor === Object) {
		__js__.destructor(false, obj, mod.close);
	}

	return base.stream(obj, name, filename, mode, type);
};

mod.Stream = function (filename, mode, type, pre_open) {
	base.stream(this,
	            mod.$name(),
	            maybe_string(filename),
	            mode,
	            helper.hasOwnProperty(type)?type:'file');

	if (true === pre_open) {
		mod.open(this);
	}
};

mod.Stream.prototype = {
	is_open: function () {
		return mod.is_open(this);
	},

	is_close: function () {
		return mod.is_close(this);
	},

	open: function (mode) {
		print('stream.open mode=', mode);
		return mod.open(this, mode);
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

	write: function (buffer) {
		return mod.write(this, buffer);
	},

	tell: function () {
		return mod.tell(this);
	},

	seek: function (offset, where) {
		return mod.seek(this, offset, where);
	},

	rewind: function () {
		return mod.rewind(this);
	},

	sync: function () {
		return mod.sync(this);
	},

	flush: function () {
		return mod.flush(this);
	},

	error: function () {
		return mod.error(this);
	},

	eof: function () {
		return mod.eof(this);
	}
};

__js__.destructor(true, mod.Stream.prototype, mod.close);

/* eof */
