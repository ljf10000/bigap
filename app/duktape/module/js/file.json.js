#!/bin/js

/*
* module: txt-file
*/
var mod = this;
var pt = mod.constructor.prototype;
pt.name = pt.name || 'file.json';
pt.debugger = new ModDebugger(pt.name);

pt.proxy = function (filename) {
	return new Proxy({
		filename: filename,
		name: pt.name + '(' + filename + ')'
	}, {
		get: function (obj, key) {
			if (key==='content') {
				if (__libc__.fexist(obj.filename)) {
					return __my__.readtxt(obj.filename);
				} else {
					return '';
				}
			} else {
				return obj[key];
			}
		},

		set: function (obj, key, value) {
			if (key==='content') {
				__my__.writefile(obj.filename, value);
			} else {
				obj[key] = value;
			}
		}
	});
};

/* eof */
