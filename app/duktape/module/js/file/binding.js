#!/bin/js

/*
* module: file binding
*/
var mod = this,
	name = 'file/binding';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.load = function (obj, reader) {
	var safe_reader = maybe_function(reader) || allways;

	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = safe_reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = safe_reader('');
	}

	mod.$debugger.trace('load ' + obj.content);
};

mod.save = function (obj, writer) {
	var safe_writer = maybe_function(writer) || allways;

	if (obj.filename && obj.content) {
		__my__.writefile(obj.filename, safe_writer(obj.content));
	}

	mod.$debugger.trace('save ' + obj.filename + ':' + obj.content);
};

function init (obj, filename, is_object) {
	obj.filename = maybe_string(filename);
	obj.$name = function() { return name + '(' + obj.filename + ')'; };
	obj.content = is_object?{}:'';
}

mod.Json = function (filename, pre_load) {
	init(this, filename, true);

	if (undefined === pre_load || maybe_bool(pre_load)) {
		this.load();
	}
};

mod.Json.prototype = {
	load: function () {
		mod.load(this, JSON.parse);
	},
	save: function () {
		mod.save(this, JSON.stringify);
	}
};

mod.Cache = function (filename, pre_load) {
	init(this, filename);

	if (undefined === pre_load || maybe_bool(pre_load)) {
		this.load();
	}
};

mod.Cache.prototype = {
	load: function () {
		mod.load(this);
	},
	save: function () {
		mod.save(this);
	}
};

mod.Direct = function (filename) {
	var binding = {};

	init(binding, filename);

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
};

/* eof */
