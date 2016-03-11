#!/bin/js

function oprint (name, obj) {
	var container = {};
	container[name] = obj;

	print(Duktape.enc('jc',container));
}

var fmt = require("fmt");
printo("fmt", fmt);

var stream = require("stream");
printo("stream", stream);

oprint("stream", stream);

var file = new stream.Stream("file.js", "r");
printo("file", file);

var buffer = file.readEx(10000);
print("read buffer =", buffer.toString());

auto_load_test();
debug(1, "sb", 1, 2, "3", file);

