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
	print('load 1');
	if (obj.filename && __libc__.fexist(obj.filename)) {
		print('load 1.1');
		obj.content = safe_reader(__my__.readtxt(obj.filename));
		print('load 1.2');
	} else {
		print('load 1.3');
		obj.content = safe_reader(maybe_function(reader)?{}:'');
		print('load 1.4');
	}
	print('load 2');
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
	obj.content = (reader && writer)?{}:'';
	obj.$name = function() { return name + '(' + obj.filename + ')'; };

	return obj;
}

mod.cache_w = function (filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer),{
		load: function () {
			load(this, reader);
		},
		save: function () {
			save(this, writer);
		}
	});
};

mod.cache_a = function (filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer),{
		save: function () {
			append(this, writer);
		}
	});
};

mod.direct_w = function (filename, reader, writer) {
	return new Proxy(bind({}, filename, reader, writer), {
		get: function (obj, key) {
			if (key==='content') {
				load(obj, reader);
			}

			return obj[key];
		},

		set: function (obj, key, value) {
			obj[key] = value;

			if (key==='content') {
				save(obj, writer);
			}
		}
	});
};

mod.direct_a = function (filename, reader, writer) {
	return new Proxy(bind({}, filename, reader, writer), {
		set: function (obj, key, value) {
			obj[key] = value;

			if (key==='content') {
				append(obj, writer);
			}
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
