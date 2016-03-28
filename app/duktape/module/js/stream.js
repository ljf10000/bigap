#!/bin/js

/*
* module: stream
*/

var mod = this;
var pt = mod.constructor.prototype;
pt.name = pt.name || 'stream';
pt.type = {
	file: 1,
	pipe: 2,
	zlib: 3,
	bzip: 4
};
pt.debugger = new ModDebugger(pt.name);

function is_good_obj(obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
}

pt.open = function (obj, filename, mode, type) {
	if (obj && !is_good_obj(obj) && __libc__.fexist(filename)) {
		obj.filename = filename;
		obj.mode = mode || 'r';
		obj.type = type || pt.type.file;
		obj.name = pt.name + '(' + filename + ')';

		switch (obj.type) {
			case pt.type.file:
				obj.stream = __libc__.fopen(filename, mode);
				break;
			case pt.type.pipe:
				obj.stream = __libc__.popen(filename, mode);
				break;
			case pt.type.zlib:
				obj.stream = __libz__.gzopen(filename, mode);
				break;
			case pt.type.bzip:
				obj.stream = __libbz__.bzopen(filename, mode);
				break;
			default:
				obj.stream = null;
				break;
		}
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && is_good_obj(obj)) {
		switch (obj.type) {
			case pt.type.file:
				__libc__.fclose(obj.stream);
				break;
			case pt.type.pipe:
				__libc__.pclose(obj.stream);
				break;
			case pt.type.zlib:
				__libz__.gzclose(obj.stream);
				break;
			case pt.type.bzip:
				__libbz__.bzclose(obj.stream);
				break;
			default:
				break;
		}

		obj.stream = null;
	}

	return 0;
};

pt.read = function (obj, buffer) {
	switch(obj.type) {
		case pt.type.file: // down
		case pt.type.pipe:
			return __libc__.fread(obj.stream, buffer);
		case pt.type.zlib:
			return __libz__.gzread(obj.stream, buffer);
		case pt.type.bzip:
			return __libbz__.bzread(obj.stream, buffer);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.readEx = function (obj, size) {
	switch(obj.type) {
		case pt.type.file: // down
		case pt.type.pipe:
			return __libc__.freadEx(obj.stream, size);
		case pt.type.zlib:
			return __libz__.gzreadEx(obj.stream, size);
		case pt.type.bzip:
			return __libbz__.bzreadEx(obj.stream, size);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.write = function (obj, buffer) {
	switch(obj.type) {
		case pt.type.file:
			return __libc__.fwrite(obj.stream, buffer);
		case pt.type.zlib:
			return __libz__.gzwrite(obj.stream, buffer);
		case pt.type.bzip:
			return __libbz__.bzwrite(obj.stream, buffer);
		case pt.type.pipe: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.tell = function (obj) {
	switch(type) {
		case pt.type.file:
			return __libc__.ftell(obj.stream);
		case pt.type.zlib:
			return __libz__.gztell(obj.stream);
		case pt.type.pipe: // down
		case pt.type.bzip: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.seek = function (obj, offset, where) {
	switch(type) {
		case pt.type.file:
			return __libc__.fseek(obj.stream, offset, where);
		case pt.type.pipe:
			return -__libc__.ENOSUPPORT;
		case pt.type.zlib:
			return __libz__.gzseek(obj.stream, offset, where);
		case pt.type.bzip: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.flush = function (obj) {
	switch(type) {
		case pt.type.file:
			return __libc__.fflush(obj.stream);
		case pt.type.zlib:
			return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
		case pt.type.bzip:
			return __libbz__.bzflush(obj.stream);
		case pt.type.pipe: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.error = function (obj) {
	switch(type) {
		case pt.type.file: // down
		case pt.type.pipe:
			return __libc__.ferror(obj.stream);
		case pt.type.zlib:
			return __libz__.gzerror(obj.stream);
		case pt.type.bzip:
			return __libbz__.bzerror(obj.stream);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.eof = function (obj) {
	switch(type) {
		case pt.type.file: // down
		case pt.type.pipe:
			return __libc__.feof(obj.stream);
		case pt.type.zlib:
			return __libz__.gzeof(obj.stream);
		case pt.type.bzip: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

mod.Stream = function (filename, mode, type) {
	return pt.open(this, filename, mode, type)
};

mod.Stream.prototype = {
	read: function (buffer) {
		return pt.read(this, buffer);
	},

	readEx: function (size) {
		return pt.readEx(this, size);
	},

	write: function (buffer) {
		return pt.write(this, buffer);
	},

	tell: function () {
		return pt.tell(this);
	},

	seek: function (offset, where) {
		return pt.seek(this, offset, where);
	},

	flush: function () {
		return pt.flush(this);
	},

	error: function () {
		return pt.error(this);
	},

	eof: function () {
		return pt.eof(this);
	}
};

__js__.destructor(true, mod.Stream.prototype, pt.close);

/* eof */
