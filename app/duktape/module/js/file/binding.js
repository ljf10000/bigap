#!/bin/js

/*
* module: file binding
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'file/binding';
pt.$debugger = new $Debugger(pt.$name);

pt.load = function (obj) {
	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = obj.reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = obj.reader('');
	}
};

pt.save = function (obj, writer) {
	if (obj.filename && obj.content) {
		__my__.writefile(obj.filename, obj.writer(obj.content));
	}
};

pt.binding = function (filename, direct, reader, writer) {
	var obj = {
		$name: pt.$name + '(' + filename + ')',
		filename: filename,
		reader: reader || allways_pass,
		writer: writer || allways_pass
	};

	if (true === direct) {
		return new Proxy(obj, {
			get: function (obj, key) {
				if (key==='content') {
					pt.load(obj);
				}

				return obj[key];
			},

			set: function (obj, key, value) {
				obj[key] = value;

				if (key==='content') {
					pt.save(obj);
				}
			}
		});
	} else {
		return Object.create({
			load: function () {
				pt.load(this);
			},

			save: function () {
				pt.save(this);
			}
		}, obj);
	}
};

pt.jsonBinding = function (filename) {
	return pt.binding(filename, false, JSON.parse, JSON.stringify);
};

pt.txtCacheBinding = function (filename) {
	return pt.binding(filename, false);
};

pt.txtDirectBinding = function (filename) {
	return pt.binding(filename, true);
};

/* eof */
