#!/bin/js

var mod = this;

const name = 'net/ip',
	zero = {
		i: 0,
		s: '0.0.0.0'
	};

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

function ipsave (obj, ip) {
	switch (typeof ip) {
		case 'number':
			obj.i = ip;
			obj.s = __libc__.inet_ntoa(ip);

			return obj;
		case 'string':
			obj.i = __libc__.inet_aton(ip);
			obj.s = ip;

			return obj;
		default:
			return zero;
	}
}

function masklen (mask) {
	var len = 0, i;

	// network sort ==> host sort
	mask = __libc__.ntohl(mask);

	for (i=31; i>=0; i--) {
		if (mask & (1<<i)) {
			len++;
		}
	}

	return len;
}

function maskgen (masklen) {
	var i, mask = 0;

	masklen = (masklen>=0 && masklen<=32)?masklen:0;

	for (i=31; i>=(32-masklen); i--) {
		mask |= 1<<i;
	}

	// network sort
	return __libc__.htonl(mask);
}

mod.masklen = masklen;
mod.maskgen = maskgen;

pt = {
	ipstring: function (ip) {
		if (undefined===ip) {
			return this.s;
		} else {
			return ipsave(this, ip);
		}
	},
	ipaddr: function (ip) {
		if (undefined===ip) {
			return this.i;
		} else {
			return ipsave(this, ip);
		}
	}
};

mod.object = function(ip) {
	return Object.setPrototypeOf(ipsave({}, ip), pt);
};
