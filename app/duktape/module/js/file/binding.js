#!/bin/js

/*
* module: file binding
*/
var mod = this,
	pt = mod.__proto__,
	name = 'file/binding';

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.load = function (obj) {
	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = obj.reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = obj.reader('');
	}

	pt.$debugger.trace('load ' + obj.content);
};

pt.save = function (obj, writer) {
	pt.$debugger.trace('save ' + obj.filename + ':' + obj.content);

	if (obj.filename && obj.content) {
		__my__.writefile(obj.filename, obj.writer(obj.content));
	}
};

pt.binding = function (filename, direct, reader, writer) {
	var tmp_filename = maybe_string(filename),
		binding = {
		$name: name + '(' + tmp_filename + ')',
		filename: tmp_filename,
		reader: maybe_function(reader) || allways,
		writer: maybe_function(writer) || allways
	};

	if (true === direct) {
		return new Proxy(binding, {
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
		binding.content = {};

		binding.__proto__ = {
			load: function () {
				pt.load(binding);
			},
			save: function () {
				pt.save(binding);
			}
		};

		return binding;
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
