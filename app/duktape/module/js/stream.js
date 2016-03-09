#!/bin/js

var that = this;

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

	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.popen(filename, mode);
	} else {
		this.stream = __libc__.fopen(filename, mode);
	}

	this.__ok = function () {
		return __ok(this);
	};

	this.close = function () {
		__close(this);
	};

	this.read = function (buffer, size) {
		return __libc__.fread(buffer, size, 1, this.stream);
	};

	this.readEx = function (size) {
		return __libc__.freadEx(this.stream, size);
	};

	this.write = function (buffer, size) {
		if (this.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.fwrite(buffer, size, 1, this.stream);
		}
	};

	this.eof = function () {
		return __libc__.feof(this.stream);
	};

	this.error = function () {
		return __libc__.ferror(this.stream);
	};

	this.tell = function () {
		if (this.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.ftell(this.stream);
		}
	};

	this.seek = function (offset, where) {
		if (this.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.ftell(this.stream, where);
		}
	};

	this.flush = function () {
		if (this.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.fflush(this.stream);
		}
	};
};

Duktape.fin(that.Stream.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		__close(obj);
	}

	if (obj === that.Stream.prototype) {
        return;  // called for the prototype itself
    }

	__close(obj);
});
