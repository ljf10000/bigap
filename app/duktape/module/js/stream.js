#!/bin/js

/*
* module: stream
*/

var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'stream';
pt.$debugger = new $Debugger(pt.$name);

pt.type = {
	file: 1,
	pipe: 2,
	zlib: 3,
	bzip: 4
};

pt.is_open = function is_open (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function is_close (obj) {
	return null === obj.stream;
};

pt.open = function (obj) {
	if (obj && is_close(obj)) {
		var pttype = pt.type;

		switch (obj.type) {
			case pttype.file:
				obj.stream = __libc__.fopen(filename, mode);
				break;
			case pttype.pipe:
				obj.stream = __libc__.popen(filename, mode);
				break;
			case pttype.zlib:
				obj.stream = __libz__.gzopen(filename, mode);
				break;
			case pttype.bzip:
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
	if (obj && is_open(obj)) {
		var pttype = pt.type;

		switch (obj.type) {
			case pttype.file:
				__libc__.fclose(obj.stream);
				break;
			case pttype.pipe:
				__libc__.pclose(obj.stream);
				break;
			case pttype.zlib:
				__libz__.gzclose(obj.stream);
				break;
			case pttype.bzip:
				__libbz__.bzclose(obj.stream);
				break;
			default:
				break;
		}

		obj.stream = null;
	}
};

pt.read = function (obj, buffer) {
	var pttype = pt.type;

	switch(obj.type) {
		case pttype.file: // down
		case pttype.pipe:
			return __libc__.fread(obj.stream, buffer);
		case pttype.zlib:
			return __libz__.gzread(obj.stream, buffer);
		case pttype.bzip:
			return __libbz__.bzread(obj.stream, buffer);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.readEx = function (obj, size) {
	var pttype = pt.type;

	switch(obj.type) {
		case pttype.file: // down
		case pttype.pipe:
			return __libc__.freadEx(obj.stream, size);
		case pttype.zlib:
			return __libz__.gzreadEx(obj.stream, size);
		case pttype.bzip:
			return __libbz__.bzreadEx(obj.stream, size);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.write = function (obj, buffer) {
	var pttype = pt.type;

	switch(obj.type) {
		case pttype.file:
			return __libc__.fwrite(obj.stream, buffer);
		case pttype.zlib:
			return __libz__.gzwrite(obj.stream, buffer);
		case pttype.bzip:
			return __libbz__.bzwrite(obj.stream, buffer);
		case pttype.pipe: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.tell = function (obj) {
	var pttype = pt.type;

	switch(type) {
		case pttype.file:
			return __libc__.ftell(obj.stream);
		case pttype.zlib:
			return __libz__.gztell(obj.stream);
		case pttype.pipe: // down
		case pttype.bzip: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.seek = function (obj, offset, where) {
	var pttype = pt.type;

	switch(type) {
		case pttype.file:
			return __libc__.fseek(obj.stream, offset, where);
		case pttype.pipe:
			return -__libc__.ENOSUPPORT;
		case pttype.zlib:
			return __libz__.gzseek(obj.stream, offset, where);
		case pttype.bzip: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.flush = function (obj) {
	var pttype = pt.type;

	switch(type) {
		case pttype.file:
			return __libc__.fflush(obj.stream);
		case pttype.zlib:
			return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
		case pttype.bzip:
			return __libbz__.bzflush(obj.stream);
		case pttype.pipe: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.error = function (obj) {
	var pttype = pt.type;

	switch(type) {
		case pttype.file: // down
		case pttype.pipe:
			return __libc__.ferror(obj.stream);
		case pttype.zlib:
			return __libz__.gzerror(obj.stream);
		case pttype.bzip:
			return __libbz__.bzerror(obj.stream);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

pt.eof = function (obj) {
	var pttype = pt.type;

	switch(type) {
		case pttype.file: // down
		case pttype.pipe:
			return __libc__.feof(obj.stream);
		case pttype.zlib:
			return __libz__.gzeof(obj.stream);
		case pttype.bzip: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

mod.Stream = function (filename, mode, type, open) {
	var obj = {
		filename: filename,
		mode: mode || 'r',
		type: type || pt.type.file,
		$name: pt.$name + '(' + filename + ')',

		stream: null
	};

	if (true === open) {
		pt.open(obj);
	}

	return obj;
};

mod.Stream.prototype = {
	open: function () {
		return pt.open(this);
	},

	close: function () {
		pt.close(this);
	},

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
