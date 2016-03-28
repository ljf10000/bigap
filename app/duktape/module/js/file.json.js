#!/bin/js

/*
* module: txt-file
*/
var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'file.cache';
pt.debugger = new ModDebugger(pt.$name);
pt.load = function () {
	var fobj = Object.create(pt, JSON.parse(__my__.readtxt(this.$filename)));
	fobj.$name = this.$name;
	fobj.$filename = this.$filename;

	return fobj;
};

pt.save = function () {
	var obj = JSON.parse(JSON.stringify(this));
	delete obj.$name;
	delete obj.$filename;

	__my__.writefile(this.$filename, JSON.stringify(obj));
};

pt.bind = function (filename) {
	return Object.create(pt, {
		$name: pt.$name + '(' + filename + ')',
		$filename: filename,
	});
};

/* eof */
