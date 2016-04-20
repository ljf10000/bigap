#!/bin/js

var base = require('modbase');

this.name = "mod1";
this.hello = function() {
	base.hello();
	print("hello", this.name);
};

print('load', this.name);
