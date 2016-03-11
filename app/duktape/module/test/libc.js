#!/bin/js

this.test = function() {
	var libc = __libc__;

	fmt.separator("libc")
	fmt.oprint("libc", libc);

	fmt.separator("libc.stat");
	var stat = Object();
	libc.stat("./test.js", stat);
	fmt.oprint("libc.stat", stat);

	fmt.separator("libc.mkdir");
	libc.mkdir("sb", 0);
	libc.mkdir("sb/sb", 0);
	libc.mkdir("sb/sb/sb", 0);
	libc.rmdir("sb", true);

	fmt.separator("libc.getcwd");
	print("getcwd=", libc.getcwd());

	fmt.separator("libc.chdir");
	libc.chdir("..");
	print("getcwd=", libc.getcwd());
	libc.chdir("test");
	print("getcwd=", libc.getcwd())
}
