#!/bin/js

/*
* module: file binding
*/
var mod = this,
	name = 'file/binding';

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

function load(obj, reader) {
	var safe_reader = maybe_function(reader) || allways;

	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = safe_reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = safe_reader(maybe_function(reader)?'{}':'');
	}

	mod.$debugger.trace('load ' + obj.content);

	return obj;
}

function loadl(array, reader) {
	var safe_reader = maybe_function(reader) || allways;
	var count = 0;

	if (array.filename && __libc__.fexist(array.filename)) {
		__my__.readline(array.filename, function (line) {
			array[count++] = safe_reader(line);
		});
	}

	return array;
}

function write(obj, append, writer) {
	var safe_writer = maybe_function(writer) || allways_string;

	if (obj.filename && obj.content) {
		var writefile = append?__my__.appendfile:__my__.writefile;

		writefile(obj.filename, safe_writer(obj.content));
	}

	mod.$debugger.trace(append?'append ':'save ' + obj.filename + ':' + obj.content);

	return obj;
}

function writel(array, writer) {
	var safe_writer = maybe_function(writer) || allways_string;
	var i, count = array.length;

	if (array.filename) {
		__my__.cleanfile(array.filename);

		for (i=0; i<count; i++) {
			if (array[i]) {
				__my__.appendfile(array.filename, safe_writer(array[i]));
			}
		}
	}

	return array;
}

function save(obj, writer) {
	return write(obj, false, writer);
}

function append(obj, writer) {
	return write(obj, true, writer);
}

function bind(obj, filename, reader, writer) {
	obj.filename = maybe_string(filename);
	obj.content = (maybe_function(reader) && maybe_function(writer))?{}:'';
	obj.$name = function() { return name + '(' + obj.filename + ')'; };

	return obj;
}

function bindl(array, filename) {
	array.filename = maybe_string(filename);
	array.$name = function() { return name + '(' + array.filename + ')'; };

	return array;
}

mod.cache_l = function(filename, reader, writer) {
	return Object.setPrototypeOf(bindl([], filename, reader, writer), {
		load: function () {
			loadl(this, reader);
		},
		save: function () {
			writel(this, writer);
		}
	});
};

mod.cache_w = function(filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer), {
		load: function () {
			load(this, reader);
		},
		save: function () {
			save(this, writer);
		}
	});
};

mod.cache_a = function(filename, reader, writer) {
	return Object.setPrototypeOf(bind({}, filename, reader, writer), {
		save: function () {
			append(this, writer);
		}
	});
};

mod.direct_w = function(filename, reader, writer) {
	return new Proxy(bind({}, filename, reader, writer), {
		get: function(obj, key) {
			if (key==='content') {
				load(obj, reader);
			}

			return obj[key];
		},

		set: function(obj, key, value) {
			obj[key] = value;

			if (key==='content') {
				save(obj, writer);
			}
		}
	});
};

mod.direct_a = function(filename, reader, writer) {
	return new Proxy(bind({}, filename, reader, writer), {
		set: function (obj, key, value) {
			obj[key] = value;

			if (key==='content') {
				append(obj, writer);
			}
		}
	});
};

/* eof */
