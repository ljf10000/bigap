#!/bin/js

/*
* module: cpuset helper
*/
var mod = this,
    name = 'cpu/helper/cpuset';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.cpuset = function () {
	return new Uint32Array(__libc__.__CPU_SETSIZE/Uint32Array.BYTES_PER_ELEMENT);
};

/* eof */
