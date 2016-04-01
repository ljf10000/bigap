#!/bin/js

/*
* module: dir
*/
var mod = this,
	pt = mod.constructor.prototype,
	dir = require('fs/helper/dir');

pt.__proto__ = dir.constructor.prototype;
pt.$name = pt.$name || 'fs/dir';
pt.$debugger = new $Debugger(pt.$name);

mod.Dir = function (filename, pre_open) {
	var obj = {
		filename: filename,
		dir: null
	};

	if (true === pre_open) {
		pt.open(obj);
	}

	return obj;
};

mod.Dir.prototype = {
	is_open: function () {
		return pt.is_open(this);
	},

	is_close: function () {
		return pt.is_close(this);
	},

	open: function () {
		return pt.open(this);
	},

	close: function () {
		pt.close(this);
	},

	read: function () {
		return pt.read(this);
	},

	tell: function () {
		return pt.tell(this);
	},

	seek: function (pos) {
		return pt.seek(this, pos);
	},

	rewind: function () {
		return pt.seek(rewind);
	}
};

__js__.destructor(true, mod.Dir.prototype, pt.close);

/* eof */
