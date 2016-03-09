#!/bin/js

print("stream 1");
this.current    = __libc__.SEEK_CUR;
this.begin      = __libc__.SEEK_SET;
this.end        = __libc__.SEEK_END;

print("stream 2");
this.Stream = function (filename, mode) {
		this.stream = __libc__.fopen(filename, mode);
};

print("stream 14");
