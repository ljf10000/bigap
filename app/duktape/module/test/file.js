#!/bin/js

var stream = require("stream");
fmt.separator("stream");
fmt.oprint("stream", stream);

var file = new stream.Stream("file.js", "r");
fmt.oprint("file", file);

var buffer = file.readEx(10000);
fmt.separator("begin read buffer");
print(buffer);
fmt.separator("end read buffer");

fmt.oprint("__js__", __js__);

debug(1, "sb", 1, 2, "3", file);
auto_global();

