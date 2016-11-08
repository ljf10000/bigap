#!/bin/js

var name = ['mod', 'my', 'libc', 'libcurl'];
var argc = __my__.argv.length;

if (0==argc) {
	argc = name.length;
} else {
	name = __my__.argv;
}

var i;
var mod;

for (i=0; i<argc; i++) {
	mod = require(name[i]);
	mod.test();
}
