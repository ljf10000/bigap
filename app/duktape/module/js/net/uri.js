#!/bin/js

/*
* module: uri
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'uri';
pt.$debugger = new $Debugger(pt.$name);

pt.scheme = {
	file: 1,
	http: 2,
	https: 3,
	ftp: 4,
	tcp: 5,
	udp: 6,
	unix: 7,
	netlink: 8
};

/* eof */
