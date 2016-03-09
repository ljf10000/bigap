#!/bin/js

var mod = this;

this.current    = __libc__.SEEK_CUR;
this.begin      = __libc__.SEEK_SET;
this.end        = __libc__.SEEK_END;

var __ok = function (obj) {
	return typeof obj.stream === 'pointer' && null !== obj.stream;
};

var __close = function (obj) {
	if (__ok(obj)) {
		if (obj.pipe) {
			__libc__.pclose(obj.stream);
		} else {
			__libc__.fclose(obj.stream);
		}

		obj.stream = null;
	}
};

this.Stream = function (filename, mode) {
	var pipe        = arguments[2]?arguments[2]:false;
	var that        = this;

	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.popen(filename, mode);
	} else {
		this.stream = __libc__.fopen(filename, mode);
	}

	this.__ok = function () {
		return __ok(that);
	};

	this.close = function () {
		__close(that);
	};

	this.read = function (buffer, size) {
		return __libc__.fread(buffer, size, 1, that.stream);
	};

	this.readEx = function (size) {
		return __libc__.freadEx(that.stream, size);
	};

	this.write = function (buffer, size) {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.fwrite(buffer, size, 1, that.stream);
		}
	};

	this.eof = function () {
		return __libc__.feof(that.stream);
	};

	this.error = function () {
		return __libc__.ferror(that.stream);
	};

	this.tell = function () {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.ftell(that.stream);
		}
	};

	this.seek = function (offset, where) {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.ftell(that.stream, where);
		}
	};

	this.flush = function () {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.fflush(that.stream);
		}
	};
};

Duktape.fin(mod.Stream.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		__close(obj);
	}

	if (obj === mod.Stream.prototype) {
        return;  // called for the prototype itself
    }

	__close(obj);
});
