#!/bin/js

/*
* module: fdset
*/

var mod = this;
var pt = mod.constructor.prototype;
pt.name = pt.name || 'fdset';
pt.debugger = new ModDebugger(pt.name);

const __NFDBITS = __libc__.__NFDBITS;
const __FD_SETSIZE = __libc__.__FD_SETSIZE;

function fd_mast (fd) {
	return 1 << (fd % __NFDBITS);
}

function fd_elt(fd) {
	return Math.floor(fd/__NFDBITS);
}

pt.zero = function (obj) {
	obj.sets.fill(0);
};

pt.set = function (obj, fd) {
	obj.sets[fd_elt(fd)] |= fd_mast(fd);
};

pt.clr = function (obj, fd) {
	obj.sets[fd_elt(fd)] &= ~fd_mast(fd);
};

pt.isset = function (obj, fd) {
	return 0 != (obj.sets[fd_elt(fd)] & fd_mast(fd));
};

mod.FdSet = function () {
	this.sets = new Uint32Array(__FD_SETSIZE/__NFDBITS);
};

mod.FdSet.prototype = {
	zero: function () {
		pt.zero(this);
	},
	set: function (fd) {
		pt.set(this, fd);
	},
	clr: function (fd) {
		pt.clr(this, fd);
	},
	isset: function (fd) {
		pt.isset(this, fd);
	}
};
/* eof */
