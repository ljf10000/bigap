#!/bin/js

var mod = this;

const name = 'product/helper/interface',
	WAN = {
		MAX: 9,
		COUNT: 2,
		RTTABLE: {
			NUMBER: 100
		}
	},
	LAN = {
		MAX: 2,
		COUNT: 1
	},
	intf = {
		lan: {
			ipaddr: '192.168.0.1',
			ifmask: '255.255.0.0',
			ifmasklen: 16,
			ifnet: '192.168.0.0',
			ifprefix: '192.168'
		},
		lan0: {
			ipaddr: '192.168.0.1',
			ifmask: '255.255.255.0',
			ifmasklen: 24,
			ifnet: '192.168.0.0',
			ifprefix: '192.168.0',
			ipbegin: 3 + WAN.MAX,
			ipend: 254
		}
	};

mod.WAN = WAN;
mod.LAN = LAN;

function ifname (type, idx) {
	return type + idx;
}

function wanname (idx) {
	return ifname('wan', idx);
}

function ifaddr (name) {
	return intf[name].ifaddr;
}

function ifmask (name) {
	return intf[name].ifmask;
}

function ifmasklen (name) {
	return intf[name].ifmasklen;
}

function ifnet (name) {
	return intf[name].ifnet;
}

function ifprefix (name) {
	return intf[name].ifprefix;
}

function ipbegin (name) {
	return intf[name].ipbegin;
}

function ipend (name) {
	return intf[name].ipend;
}

mod.ifname = ifname;
mod.wanname = wanname;
mod.ifaddr = ifaddr;
mod.ifmask = ifmask;
mod.ifmasklen = ifmasklen;
mod.ifnet = ifnet;
mod.ifpreifx = ifprefix;
mod.ipbegin = ipbegin;
mod.ipend = ipend;
