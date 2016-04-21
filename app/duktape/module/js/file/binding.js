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

	return obj;
};

mod.save = function (obj, writer) {
	var safe_writer = maybe_function(writer) || allways;

	if (obj.filename && obj.content) {
		__my__.writefile(obj.filename, safe_writer(obj.content));
	}

	mod.$debugger.trace('save ' + obj.filename + ':' + obj.content);

	return obj;
};

mod.bind = function (obj, filename, reader, writer) {
	obj.filename = maybe_string(filename);
	obj.$name = function() { return name + '(' + obj.filename + ')'; };
	obj.content = (reader && writer)?{}:'';

	return obj;
};

mod.Cache = function (filename, reader, writer) {
	mod.bind(this, filename, reader, writer);
};

mod.Cache.prototype = {
	load: function () {
		mod.load(this, reader);
	},
	save: function () {
		mod.save(this, writer);
	}
};

mod.Direct = function (filename, reader, writer) {
	return new Proxy(mod.bind({}, filename, reader, writer), {
		get: function (obj, key) {
			if (key==='content') {
				mod.load(obj, reader);
			}

			return obj[key];
		},

		set: function (obj, key, value) {
			obj[key] = value;

			if (key==='content') {
				mod.save(obj, writer);
			}
		}
	});
};

/* eof */
