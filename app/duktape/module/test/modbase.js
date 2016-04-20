#!/bin/js

this.name = "modbase";
this.hello = function() {
	print("hello", this.name);
};

print('load', this.name);
