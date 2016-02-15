#!../../js

var mod1 = require("mod1");
var mod2 = require("mod2");

this.test = function() {
	mod1.hello();
	mod2.hello();
	print("hello mod");
};
