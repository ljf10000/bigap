#!/bin/js

/*
* module: fdset helper
*/
var mod = this,
	pt = mod.__proto__,
	name = 'io/helper/fdset';

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.fdset = function () {
	return new Uint32Array(__libc__.__FD_SETSIZE/Uint32Array.BYTES_PER_ELEMENT);
};

/* eof */
