#!/bin/js

this.test = function() {
	var my = __my__;
	
	fmt.separator("pipe");
	std = my.pipe('ls -al; ls ...');
	print("stdout =", std.stdout);
	print("stderr =", std.stderr);
	print("errno =", std.errno);

	fmt.separator("readtxt");
	buf = my.readtxt("test.txt");
	print("buf =", buf);

	fmt.separator("readline");
	err = my.readline("test.txt", function(line) {
		print("inline line =", line)

		return 0;
	});
	print("err =", err);

	fmt.separator("argv");
	print("name =", my.name);
	print("script =", my.script);
	print("args =", my.argv);
	print("args.length =", my.argv.length);

	fmt.separator("env");
	fmt.oprint("env", my.env);
};
