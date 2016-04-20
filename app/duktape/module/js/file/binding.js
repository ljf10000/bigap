#!/bin/js

/*
* module: file binding
*/
var mod = this,
	name = 'file/binding';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.load = function (obj) {
	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = obj.reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = obj.reader('');
	}

	mod.$debugger.trace('load ' + obj.content);
};

mod.save = function (obj, writer) {
	mod.$debugger.trace('save ' + obj.filename + ':' + obj.content);

	if (obj.filename && obj.content) {
		__my__.writefile(obj.filename, obj.writer(obj.content));
	}
};

mod.bind = function (filename, direct, reader, writer) {
	var tmp_filename = maybe_string(filename),
		binding = {
			$name: function() { return name + '(' + tmp_filename + ')'; },
			filename: tmp_filename,
			reader: maybe_function(reader) || allways,
			writer: maybe_function(writer) || allways
		};

	if (true === direct) {
		return new Proxy(binding, {
			get: function (obj, key) {
				if (key==='content') {
					mod.load(obj);
				}

				return obj[key];
			},

			set: function (obj, key, value) {
				obj[key] = value;

				if (key==='content') {
					mod.save(obj);
				}
			}
		});
	} else {
		binding.content = {};

		binding.__proto__ = {
			load: function () {
				mod.load(binding);
			},
			save: function () {
				mod.save(binding);
			}
		};

		return binding;
	}
};

mod.bind_json = function (filename) {
	return mod.bind(filename, false, JSON.parse, JSON.stringify);
};

mod.bind_txt_cache = function (filename) {
	return mod.bind(filename, false);
};

mod.bind_txt_direct = function (filename) {
	return mod.bind(filename, true);
};

/* eof */
