#!/bin/js

print('exec js file');
var i, count = __my__.argv.length;
for (i=0; i<count; i++) {
	print('argv[', i, ']=', __my__.argv[i]);
}
