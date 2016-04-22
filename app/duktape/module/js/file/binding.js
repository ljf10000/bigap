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
		obj.content = safe_reader('');
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
	obj.content = (reader && writer)?{}:'';
	obj.$name = function() { return name + '(' + obj.filename + ')'; };

	return obj;
}

mod.cache = function (filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer),{
		load: function () {
			load(this, reader);
		},
		save: function () {
			save(this, writer);
		}
	});
};

mod.direct = function (filename, reader, writer) {
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

mod.append = function (filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer),{
		save: function () {
			append(this, writer);
		}
	});
};

mod.object = function (filename, type, reader, writer) {
	switch(type) {
		case 'direct': return mod.direct(filename, reader, writer);
		case 'append': return mod.append(filename, reader, writer);
		case 'cache': default: return mod.cache(filename, reader, writer);
	}
};

/* eof */
