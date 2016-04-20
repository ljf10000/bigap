#!/bin/js

/*
* module: fdset helper
*/
var mod = this,
	name = 'io/helper/fdset';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.fdset = function () {
	return new Uint32Array(__libc__.__FD_SETSIZE/Uint32Array.BYTES_PER_ELEMENT);
};

/* eof */
