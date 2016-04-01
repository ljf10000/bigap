#!/bin/js

/*
* module: txt-file
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'file/helper/cache';
pt.$debugger = new $Debugger(pt.$name);

pt.load = function () {
	if (this.filename) {
		this.cache = __my__.readtxt(this.filename);
	}
};

pt.save = function () {
	if (this.filename) {
		__my__.writefile(this.filename, this.cache);
	}
};

pt.bind = function (filename) {
	return Object.create(pt, {
		$name: pt.$name + '(' + filename + ')',
		filename: filename
	});
};

/* eof */
