#!/bin/js

/*
* module: cpuset helper
*/
var mod = this,
	pt = mod.__proto__;

pt.$name = pt.$name || 'cpu/helper/cpuset';
pt.$debugger = new $Debugger(pt.$name);

pt.cpuset = function () {
	return new Uint32Array(__libc__.__CPU_SETSIZE/Uint32Array.BYTES_PER_ELEMENT);
};

/* eof */
