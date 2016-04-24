#!/bin/js

/*
* module: pipe stream helper
*/
var mod = this,
	name = 'stream/helper/pipe',
	base = require('stream/helper/base');

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

mod.is_open = base.is_open;
mod.is_close = base.is_close;

mod.open = function(obj, mode) {
	if (obj && mod.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libc__.popen(obj.filename, obj.mode);
	}

	return obj;
};

mod.close = function(obj) {
	if (obj && mod.is_open(obj)) {
		__libc__.pclose(obj.stream);
		obj.stream = null;
	}
};

mod.read = function(obj, buffer) {
	return __libc__.fread(obj.stream, buffer);
};

mod.readEx = function(obj, size) {
	return __libc__.freadEx(obj.stream, size);
};

mod.error = function(obj) {
	return __libc__.ferror(obj.stream);
};

mod.eof = function(obj) {
	return __libc__.feof(obj.stream);
};

/* eof */
