#!/bin/js

/*
* module: stream
*/
var mod = this,
	pt = mod.__proto__;

pt.$name = pt.$name || 'stream';
pt.$debugger = new $Debugger(pt.$name);

pt.type = {
	file: 0,
	pipe: 1,
	zlib: 2,
	bzip: 3
};

pt.is_open = function is_open (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function is_close (obj) {
	return null === obj.stream;
};

pt.open = function (obj, mode) {
	if (obj && is_close(obj)) {
		var type = pt.type;

		mode = mode || obj.mode;

		switch (obj.type) {
			case type.file:
				obj.stream = __libc__.fopen(obj.filename, mode);
				break;
			case type.pipe:
				obj.stream = __libc__.popen(obj.filename, mode);
				break;
			case type.zlib:
				obj.stream = __libz__.gzopen(obj.filename, mode);
				break;
			case type.bzip:
				obj.stream = __libbz__.bzopen(obj.filename, mode);
				break;
			default:
				obj.stream = null;
				break;
		}

		if (pt.is_open(obj)) {
			/*
			* maybe re-open with other mode
			*/
			obj.mode = mode;
		}
	}

	return obj;
};

pt.close = function (obj) {
	if (obj && is_open(obj)) {
		var type = pt.type;

		switch (obj.type) {
			case type.file:
				__libc__.fclose(obj.stream);
				break;
			case type.pipe:
				__libc__.pclose(obj.stream);
				break;
			case type.zlib:
				__libz__.gzclose(obj.stream);
				break;
			case type.bzip:
				__libbz__.bzclose(obj.stream);
				break;
			default:
				break;
		}

		obj.stream = null;
	}
};

pt.read = function (obj, buffer) {
	var type = pt.type;

	switch(obj.type) {
		case type.file: // down
		case type.pipe:
			return __libc__.fread(obj.stream, buffer);
		case type.zlib:
			return __libz__.gzread(obj.stream, buffer);
		case type.bzip:
			return __libbz__.bzread(obj.stream, buffer);
		default:
			return no_support();
	}
};

pt.readEx = function (obj, size) {
	var type = pt.type;

	switch(obj.type) {
		case type.file: // down
		case type.pipe:
			return __libc__.freadEx(obj.stream, size);
		case type.zlib:
			return __libz__.gzreadEx(obj.stream, size);
		case type.bzip:
			return __libbz__.bzreadEx(obj.stream, size);
		default:
			return no_support();
	}
};

pt.write = function (obj, buffer) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.fwrite(obj.stream, buffer);
		case type.zlib:
			return __libz__.gzwrite(obj.stream, buffer);
		case type.bzip:
			return __libbz__.bzwrite(obj.stream, buffer);
		case type.pipe: // down
		default:
			return no_support();
	}
};

pt.tell = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.ftell(obj.stream);
		case type.zlib:
			return __libz__.gztell(obj.stream);
		case type.pipe: // down
		case type.bzip: // down
		default:
			return no_support();
	}
};

pt.seek = function (obj, offset, where) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.fseek(obj.stream, offset, where);
		case type.zlib:
			return __libz__.gzseek(obj.stream, offset, where);
		case type.pipe: // down
		case type.bzip: // down
		default:
			return no_support();
	}
};

pt.rewind = function (obj) {
	return pt.seek(obj, 0, __libc__.SEEK_SET);
};

pt.sync = pt.flush = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file:
			return __libc__.fflush(obj.stream);
		case type.zlib:
			return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
		case type.bzip:
			return __libbz__.bzflush(obj.stream);
		case type.pipe: // down
		default:
			return no_support();
	}
};

pt.error = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file: // down
		case type.pipe:
			return __libc__.ferror(obj.stream);
		case type.zlib:
			return __libz__.gzerror(obj.stream);
		case type.bzip:
			return __libbz__.bzerror(obj.stream);
		default:
			return no_support();
	}
};

pt.eof = function (obj) {
	var type = pt.type;

	switch(obj.type) {
		case type.file: // down
		case type.pipe:
			return __libc__.feof(obj.stream);
		case type.zlib:
			return __libz__.gzeof(obj.stream);
		case type.bzip: // down
		default:
			return no_support();
	}
};

mod.Stream = function (filename, mode, type, pre_open) {
	var obj = {
		filename: filename,
		mode: mode || 'r',
		type: type || pt.type.file,
		$name: pt.$name + '(' + filename + ')',

		stream: null
	};

	if (true === pre_open) {
		pt.open(obj);
	}

	return obj;
};

mod.Stream.prototype = {
	is_open: function () {
		return pt.is_open(this);
	},

	is_close: function () {
		return pt.is_close(this);
	},

	open: function (mode) {
		return pt.open(this, mode);
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

	rewind: function () {
		return pt.rewind(this);
	},

	sync: function () {
		return pt.sync(this);
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
