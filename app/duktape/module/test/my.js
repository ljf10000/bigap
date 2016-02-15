#!../../js

this.test = function() {
	var my = __my__;
	
	fmt.print_tag("pipe");
	std = my.pipe('ls -al; ls ...');
	print("stdout =", std.stdout);
	print("stderr =", std.stderr);
	print("errno =", std.errno);

	fmt.print_tag("readtxt");
	buf = my.readtxt("test.txt");
	print("buf =", buf);

	fmt.print_tag("readline");
	err = my.readline("test.txt", function(line) {
		print("inline line =", line)

		return 0;
	});
	print("err =", err);

	fmt.print_tag("argv");
	print("name =", my.name);
	print("script =", my.script);
	print("args =", my.argv);
	print("args.length =", my.argv.length);

	fmt.print_tag("env");
	fmt.print_obj("env", my.env);
};
