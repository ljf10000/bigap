#!/bin/js

print("stream 1");
Global.Stream = function (filename, mode, pipe = false) {
	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.fopen(filename, mode);
	} else {
		this.stream = __libc__.popen(filename, mode);
	}
}

print("stream 14");
