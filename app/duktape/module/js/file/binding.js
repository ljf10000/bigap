#!/bin/js

/*
* module: file binding
*/
var mod = this,
	name = 'file/binding';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

function load (obj, reader) {
	var safe_reader = maybe_function(reader) || allways;

	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = safe_reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = safe_reader(maybe_function(reader)?'{}':'');
	}

	mod.$debugger.trace('load ' + obj.content);

	return obj;
}

function write (obj, append, writer) {
	var safe_writer = maybe_function(writer) || allways_string;

	if (obj.filename && obj.content) {
		var writefile = append?__my__.appendfile:__my__.writefile;

		writefile(obj.filename, safe_writer(obj.content));
	}

	mod.$debugger.trace(append?'append ':'save ' + obj.filename + ':' + obj.content);

	return obj;
}

function save (obj, writer) {
	return write(obj, false, writer);
}

function append (obj, writer) {
	return write(obj, true, writer);
}

function bind (obj, filename, reader, writer) {
	obj.filename = maybe_string(filename);
	obj.content = (maybe_function(reader) && maybe_function(writer))?{}:'';
	obj.$name = function() { return name + '(' + obj.filename + ')'; };

	return obj;
}

mod.cache_w = function (filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer),{
		load: function () {
			print('cache_w load 1');
			load(this, reader);
			print('cache_w load 2');
		},
		save: function () {
			print('cache_w save 1');
			save(this, writer);
			print('cache_w save 2');
		}
	});
};

mod.cache_a = function (filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer),{
		save: function () {
			print('cache_a save 1');
			append(this, writer);
			print('cache_a save 2');
		}
	});
};

mod.direct_w = function (filename, reader, writer) {
	return new Proxy(bind({}, filename, reader, writer), {
		get: function (obj, key) {
			print('direct_w get 1');
			if (key==='content') {
				load(obj, reader);
			}

			print('direct_w get 2');
			return obj[key];
		},

		set: function (obj, key, value) {
			print('direct_w set 1');
			obj[key] = value;

			if (key==='content') {
				save(obj, writer);
			}
			print('direct_w set 2');
		}
	});
};

mod.direct_a = function (filename, reader, writer) {
	return new Proxy(bind({}, filename, reader, writer), {
		get: function (obj, key) {
			print('direct_a get 1');

			return obj[key];
		},

		set: function (obj, key, value) {
			print('direct_a set 1');
			obj[key] = value;

			if (key==='content') {
				append(obj, writer);
			}
			print('direct_a set 2');
		}
	});
};

mod.object = function (filename, is_direct, is_append, reader, writer) {
	var binding;

	if (is_direct) {
		if (is_append) {
			binding = mod.direct_a;
		} else {
			binding = mod.direct_w;
		}
	} else {
		if (is_append) {
			binding = mod.cache_a;
		} else {
			binding = mod.cache_w;
		}
	}

	return binding(filename, reader, writer);
};

/* eof */
