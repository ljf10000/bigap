#!/bin/js

var stream = require("stream");
fmt.oprint("stream", stream);

var file = new stream.Stream("file.js", "r");
fmt.oprint("file", file);

var buffer = file.readEx(10000);
print("read buffer =", buffer.toString());

auto_load_test();
debug(1, "sb", 1, 2, "3", file);

