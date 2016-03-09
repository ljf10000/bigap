#!/bin/js

var mod = this;

mod.current    = __libc__.SEEK_CUR;
mod.begin      = __libc__.SEEK_SET;
mod.end        = __libc__.SEEK_END;

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

mod.Stream = function (filename, mode) {
	var pipe        = arguments[2]?arguments[2]:false;
	var that        = this;

	that.filename   = filename;
	that.mode       = mode;
	that.pipe       = pipe;

	if (pipe) {
		that.stream = __libc__.popen(filename, mode);
	} else {
		that.stream = __libc__.fopen(filename, mode);
	}

	that.__ok = function () {
		return __ok(that);
	};

	that.close = function () {
		__close(that);
	};

	that.read = function (buffer, size) {
		return __libc__.fread(buffer, size, 1, that.stream);
	};

	that.readEx = function (size) {
		return __libc__.freadEx(that.stream, size);
	};

	that.write = function (buffer, size) {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.fwrite(buffer, size, 1, that.stream);
		}
	};

	that.eof = function () {
		return __libc__.feof(that.stream);
	};

	that.error = function () {
		return __libc__.ferror(that.stream);
	};

	that.tell = function () {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.ftell(that.stream);
		}
	};

	that.seek = function (offset, where) {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.ftell(that.stream, where);
		}
	};

	that.flush = function () {
		if (that.pipe) {
			return -(__libc__.ENOSUPPORT);
		} else {
			return __libc__.fflush(that.stream);
		}
	};
};

/*
Duktape.fin(mod.Stream.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		__close(obj);
	}

	if (obj === mod.Stream.prototype) {
        return;  // called for the prototype itself
    }

	__close(obj);
});
*/