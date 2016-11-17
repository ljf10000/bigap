#!/bin/js

function main() {
	var count = __my__.argv.length;
	var i;

	for (i=0; i<count; i++) {
		print('argv[', i, ']=', __my__.argv[i]);
	}
}

main();

