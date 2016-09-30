#!/bin/js

var mod = this,
    ipmod = require('net/ip');

const name = 'net/ippair';

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

function ippair (ip, mask) {
	return {
		ip: ipmod.ipobj(ip),
		mask: ipmod.ipobj(mask)
	}
}

function ipnet (obj) {
	var ip = __libc__.ntohl(obj.ip.i);
	var mask = __libc__.ntohl(obj.mask.i);

	var net = ip & mask;

	return __libc__.htonl(net);
}

function ipnetstring (obj) {
	var net = ipnet(obj);

	return __libc__.inet_ntoa(net);
}

mod.ipnet = ipnet;

pt = {
	ipaddr: function () {
		return this.ip.ipaddr;
	},
	ipstring: function () {
		return this.ip.ipstring;
	},
	ipmask: function () {
		return this.mask.ipaddr;
	},
	ipmaskstring: function () {
		return this.mask.ipstring;
	},
	ipmasklen: function () {
		return ipmod.masklen(this.mask.ipaddr);
	},
	ipnet: function () {
		return ipnet(this);
	},
	ipnetstring: function () {
		return ipnetstring(this);
	}
};

mod.object = function(ip, mask) {
	return Object.setPrototypeOf(ippair(ip, mask), pt);
};
