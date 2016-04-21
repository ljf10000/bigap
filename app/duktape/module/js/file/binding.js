#!/bin/js

/*
* module: file binding
*/
var mod = this,
	name = 'file/binding';
print('loading', name);
mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.load = function (obj, reader) {
	print('load', obj.filename, 1);
	var safe_reader = maybe_function(reader) || allways;
	print('load', obj.filename, 2);

	if (obj.filename && __libc__.fexist(obj.filename)) {
		print('load', obj.filename, 2.1);
		obj.content = safe_reader(__my__.readtxt(obj.filename));
	} else {
		print('load', obj.filename, 2.2);
		obj.content = safe_reader('');
	}
	print('load', obj.filename, 3);

	mod.$debugger.trace('load ' + obj.content);
};

mod.save = function (obj, writer) {
	print('safe', obj.filename, 1);
	var safe_writer = maybe_function(writer) || allways;
	print('safe', obj.filename, 2);

	if (obj.filename && obj.content) {
		print('safe', obj.filename, 2.1);
		__my__.writefile(obj.filename, safe_writer(obj.content));
	}
	print('safe', obj.filename, 3);

	mod.$debugger.trace('save ' + obj.filename + ':' + obj.content);
};

function init (obj, filename, is_object) {
	obj.filename = maybe_string(filename);
	obj.$name = function() { return name + '(' + obj.filename + ')'; };
	obj.content = is_object?{}:'';
}

mod.Json = function (filename, pre_load) {
	init(this, filename, true);

	if (pre_load) {
		this.load();
	}

	print('bind', obj.filename);
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

	if (pre_load) {
		this.load();
	}

	print('bind', obj.filename);
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

	print('bind', binding.filename);

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

print('loaded', name);
/* eof */
