#!/bin/js

/*
* module: txt-file
*/
var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'file.json';
pt.debugger = new ModDebugger(pt.$name);

pt.load = function () {
	if (this.$filename) {
		this.content = JSON.parse(__my__.readtxt(this.$filename));
	}
};

pt.save = function () {
	if (this.$filename) {
		__my__.writefile(this.$filename, JSON.stringify(this.content));
	}
};

pt.bind = function (filename) {
	return Object.create(pt, {
		$name: pt.$name + '(' + filename + ')',
		$filename: filename,
	});
};

/* eof */
