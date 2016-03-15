#!/bin/js

var stream = require("stream");
var file = new stream.Stream("fin.js", "r");

var obj = {};
function close (obj) {
	print('obj closed');
};

obj.name = 'my obj';
__js__.objDestructor(obj, close);

