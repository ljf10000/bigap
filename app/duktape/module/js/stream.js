#!/bin/js

var mod = this;

mod.name    = 'stream';
mod.current = __libc__.SEEK_CUR;
mod.begin   = __libc__.SEEK_SET;
mod.end     = __libc__.SEEK_END;

var __ok = function (obj) {
	return typeof obj === 'object'
		&& typeof obj.stream === 'pointer'
		&& null !== obj.stream;
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
	var that        = this;
	var pipe        = arguments[2]?arguments[2]:false;

	that.filename   = filename;
	that.mode       = mode;
	that.pipe       = pipe;
	that.name       = mod.name + '(' + filename + ')';

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

	that.read = function (buffer) {
		return __libc__.fread(that.stream, buffer);
	};

	that.readEx = function (size) {
		return __libc__.freadEx(that.stream, size);
	};

	that.write = function (buffer) {
		if (that.pipe) {
			return -__libc__.ENOSUPPORT;
		} else {
			return __libc__.fwrite(that.stream, buffer);
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
			return -__libc__.ENOSUPPORT;
		} else {
			return __libc__.ftell(that.stream);
		}
	};

	that.seek = function (offset, where) {
		if (that.pipe) {
			return -__libc__.ENOSUPPORT;
		} else {
			return __libc__.fseek(that.stream, offset, where);
		}
	};

	that.flush = function () {
		if (that.pipe) {
			return -__libc__.ENOSUPPORT;
		} else {
			return __libc__.fflush(that.stream);
		}
	};
};

Duktape.fin(mod.Stream.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		__close(obj);

		debug_destructor("stream closed @fini");
	} else if (obj === mod.Stream.prototype) {
        return;  // called for the prototype itself
    } else {
		__close(obj);

		debug_destructor("stream closed @destructor");
	}
});


//__js__.classFinalizer(mod.Stream.prototype, __close);
