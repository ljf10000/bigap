#!/bin/js

/*
* module: txt-file
*/
var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'file.direct';
pt.$debugger = new ModDebugger(pt.$name);

pt.load = do_nothing;
pt.save = do_nothing;

pt.bind = function (filename) {
	return new Proxy({
		$name: pt.$name + '(' + filename + ')'
	}, {
		get: function (obj, key) {
			if (key==='content') {
				if (__libc__.fexist(filename)) {
					return __my__.readtxt(filename);
				} else {
					return '';
				}
			} else {
				return obj[key];
			}
		},

		set: function (obj, key, value) {
			if (key==='content') {
				__my__.writefile(filename, value);
			} else {
				obj[key] = value;
			}
		}
	});
};

/* eof */
