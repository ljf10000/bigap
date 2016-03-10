#!/bin/js

this.print_tag = function (name) {
	print("=========== test", name, "===========");
};

this.print_obj = function (name, obj) {
	for (var p in obj) {
	        print(name + "." + p + " = " + obj[p]);
	}
};
