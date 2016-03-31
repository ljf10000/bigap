#!/bin/js

/*
* module: cpuset
*/
var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'cpuset';
pt.$debugger = new ModDebugger(pt.$name);
pt.fdset = function () {
	return new Uint32Array(__libc__.__CPU_SETSIZE/Uint32Array.BYTES_PER_ELEMENT);
};

/* eof */
