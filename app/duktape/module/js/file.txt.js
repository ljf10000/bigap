#!/bin/js

/*
* module: txt-file
*/
var mod = this;
var prototype = mod.constructor.prototype;
prototype.name = prototype.name || 'file.txt';

mod.debugger = new ModDebugger(prototype.name);

mod.Txt = function (filename) {
	return new Proxy({
		filename: filename,
		name: prototype.name + '(' + filename + ')'
	}, {
		get: function (obj, prop) {
			if (prop==='content') {
				if (__libc__.fexist(obj.filename)) {
					return __my__.readtxt(obj.filename);
				} else {
					return '';
				}
			}
		},

		set: function (obj, prop, value) {
			if (prop==='content') {
				__my__.writefile(obj.filename, value);
			}
		}
	});
};

/* eof */
