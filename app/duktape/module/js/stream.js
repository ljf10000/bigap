#!/bin/js

print("stream 1");
this.current    = __libc__.SEEK_CUR;
this.begin      = __libc__.SEEK_SET;
this.end        = __libc__.SEEK_END;

print("stream 2");
this.Stream = function (filename, mode) {
	var pipe        = arguments[2]?arguments[2]:false

	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.fopen(filename, mode);
	} else {
		this.stream = __libc__.popen(filename, mode);
	}
};

print("stream 14");
