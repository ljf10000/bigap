#!/bin/js

/*
* module: fdset helper
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'io/helper/fdset';
pt.$debugger = new $Debugger(pt.$name);

pt.fdset = function () {
	return new Uint32Array(__libc__.__FD_SETSIZE/Uint32Array.BYTES_PER_ELEMENT);
};

/* eof */
