#!/bin/js

var mod = this;

var __ok = function (obj) {
	return typeof obj.fd === 'number' && obj.fd >= 0;
};

var __close = function (obj) {
	if (__ok(obj)) {
		__libc__.close(obj.fd);

		obj.fd = -1;
	}
};

mod.Fs = function (fd) {
	var that        = this;

	that.fd = fd;

	that.__ok = function () {
		return __ok(that);
	};

	that.close = function () {
		__close(that);
	};

	that.read = function (buffer) {
		return __libc__.read(that.fd, buffer);
	};

	that.write = function (buffer) {
		return __libc__.write(that.fd, buffer);
	};
};

Duktape.fin(mod.Fs.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		__close(obj);
	}

	if (obj === mod.Fs.prototype) {
        return;  // called for the prototype itself
    }

	__close(obj);

	print("file closed");
});
