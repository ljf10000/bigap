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

mod.File = function (filename, flag, mode) {
	var that        = this;
	var mode        = arguments[2]?arguments[2]:false;
	var fd          = -1;

	that.filename   = filename;
	that.flag       = flag;
	that.mode       = mode;

	if (3==arguments) {
		fd = __libc__.open(filename, flag, mode);
	} else {
		fd = __libc__.open(filename, flag);
	}

	that.__ok = function () {
		return __ok(that);
	};

	that.close = function () {
		__close(that);
	};

	that.read = function (buffer, size) {
		return __libc__.read(that.fd, buffer, size);
	};

	that.readEx = function (size) {
		return __libc__.readEx(that.fd, size);
	};

	that.write = function (buffer, size) {
		return __libc__.write(that.fd, buffer, size);
	};

	that.pread = function (buffer, size, offset) {
		return __libc__.pread(that.fd, buffer, size, offset);
	};

	that.pwrite = function (buffer, size) {
		return __libc__.pwrite(that.fd, buffer, size, offset);
	};

	that.lseek = function (offset, where) {
		return __libc__.lseek(that.fd, offset, where);
	};
};

Duktape.fin(mod.File.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		__close(obj);
	}

	if (obj === mod.File.prototype) {
        return;  // called for the prototype itself
    }

	__close(obj);

	print("file closed");
});
