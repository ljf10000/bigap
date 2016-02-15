#!../../js

function main() {
	var fd = __libc__.socket(__libc__.AF_UNIX, __libc__.SOCK_DGRAM, 0);
	var client = {
		"family": __libc__.AF_UNIX,
		"abstract": true,
		"path": "/tmp/.stimerc.unix"
	};
	var server = {
		"family": __libc__.AF_UNIX,
		"abstract": true,
		"path": "/tmp/.stimerd.unix"
	};
	var err, len, i, args = " "
	for (i=0; i<__my__.argv.length; i++) {
                args = args + " " + __my__.argv[i]
        }

	err = __libc__.bind(fd, client);
	err =__libc__.connect(fd, server);
	len =__libc__.send(fd, args, args.length, 0);
}

main()

