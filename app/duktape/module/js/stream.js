#!/bin/js

print("stream 1");
function Stream(filename, mode, pipe = false) {
	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.fopen(filename, mode);
	} else {
		this.stream = __libc__.popen(filename, mode);
	}
}

print("stream 7");
this.readEx = function (size) {
	return __libc__.freadEx(this.stream, size);
};


print("stream 14");
