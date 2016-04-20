#!/bin/js

/*
* module: uri
*/
var mod = this,
	name = 'net/uri';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.scheme = {
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
