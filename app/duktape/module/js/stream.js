#!/bin/js

function Stream(filename, mode, pipe = false) {
	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.fopen(filename, mode);
	} else {
		this.stream = __libc__.popen(filename, mode);
	}
}

this.__ok = function () {
	return typeof this.stream === 'pointer' && null !== this.stream
};

this.__close = function () {
	if (this.__ok()) {
		if (this.pipe) {
			__libc__.pclose(this.stream);
		} else {
			__libc__.fclose(this.stream);
		}

		this.stream = null;
	}
};

Duktape.fin(Stream.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		obj.__close();
	}

	if (obj === Socket.prototype) {
        return;  // called for the prototype itself
    }

	obj.__close();
});

this.current    = __libc__.SEEK_CUR;
this.begin      = __libc__.SEEK_SET;
this.end        = __libc__.SEEK_END;

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

