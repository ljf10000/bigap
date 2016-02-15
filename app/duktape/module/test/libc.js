#!../../js

this.test = function() {
	var libc = __libc__;

	fmt.print_tag("libc")
	fmt.print_obj("libc", libc);

	fmt.print_tag("libc.stat");
	var stat = Object();
	libc.stat("./test.js", stat);
	fmt.print_obj("libc.stat", stat);

	fmt.print_tag("libc.mkdir");
	libc.mkdir("sb", 0);
	libc.mkdir("sb/sb", 0);
	libc.mkdir("sb/sb/sb", 0);
	libc.rmdir("sb", true);

	fmt.print_tag("libc.getcwd");
	print("getcwd=", libc.getcwd());

	fmt.print_tag("libc.chdir");
	libc.chdir("..");
	print("getcwd=", libc.getcwd());
	libc.chdir("test");
	print("getcwd=", libc.getcwd())
}
