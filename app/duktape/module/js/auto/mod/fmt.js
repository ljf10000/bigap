#!/bin/js

this.oprint = function (name, obj) {
	var c;

	c[name] = obj;

	print(Duktape.enc('jc',c));
};

this.separator = function (name, sep) {
	var s = sep?sep:"==========";

	print(s, name, s);
};
