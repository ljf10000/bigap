#!/bin/js

var base = require('modbase');

this.name = "mod2";
this.hello = function() {
	base.hello();
	print("hello", this.name);
};

print('load', this.name);
