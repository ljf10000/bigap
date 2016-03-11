#!/bin/js

this.oprint = function (name, obj) {
	var c;

	c[name] = obj;

	print(Duktape.enc('jc',c));
};
