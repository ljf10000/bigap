#!/bin/js

print("stream 1");
this.current    = __libc__.SEEK_CUR;
this.begin      = __libc__.SEEK_SET;
this.end        = __libc__.SEEK_END;

print("stream 2");
function Stream (filename, mode) {
	var pipe        = arguments[2]?arguments[2]:false

	this.filename   = filename;
	this.mode       = mode;
	this.pipe       = pipe;

	if (pipe) {
		this.stream = __libc__.fopen(filename, mode);
	} else {
		this.stream = __libc__.popen(filename, mode);
	}
}

print("stream 3");
this.__ok = function () {
	return typeof this.stream === 'pointer' && null !== this.stream
};

print("stream 4");
this.close = function () {
	if (this.__ok()) {
		if (this.pipe) {
			__libc__.pclose(this.stream);
		} else {
			__libc__.fclose(this.stream);
		}

		this.stream = null;
	}
};

print("stream 5");
Duktape.fin(Stream.prototype, function (obj, heapDestruct) {
	if (heapDestruct) {
		obj.close();
	}

	if (obj === Stream.prototype) {
        return;  // called for the prototype itself
    }

	obj.close();
});

print("stream 6");
this.read = function (buffer, size) {
	return __libc__.fread(buffer, size, 1, this.stream);
};

print("stream 7");
this.readEx = function (size) {
	return __libc__.freadEx(this.stream, size);
};

print("stream 8");
this.write = function (buffer, size) {
	if (this.pipe) {
		return -(__libc__.ENOSUPPORT);
	} else {
		return __libc__.fwrite(buffer, size, 1, this.stream);
	}
};

print("stream 9");
this.eof = function () {
	return __libc__.feof(this.stream);
};

print("stream 10");
this.error = function () {
	return __libc__.ferror(this.stream);
};

print("stream 11");
this.tell = function () {
	if (this.pipe) {
		return -(__libc__.ENOSUPPORT);
	} else {
		return __libc__.ftell(this.stream);
	}
};

print("stream 12");
this.seek = function (offset, where) {
	if (this.pipe) {
		return -(__libc__.ENOSUPPORT);
	} else {
		return __libc__.ftell(this.stream, where);
	}
};

print("stream 13");
this.flush = function () {
	if (this.pipe) {
		return -(__libc__.ENOSUPPORT);
	} else {
		return __libc__.fflush(this.stream);
	}
};

print("stream 14");
