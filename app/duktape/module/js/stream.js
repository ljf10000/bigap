#!/bin/js

var mod = this;

mod.name = 'stream';
mod.seek = {
	current:__libc__.SEEK_CUR,
	begin:  __libc__.SEEK_SET,
	end:    __libc__.SEEK_END
};
mod.type = {
	file: 1,
	pipe: 2,
	gzip: 3
};

var __ok = function (obj) {
	return typeof obj === 'object'
		&& typeof obj.stream === 'pointer'
		&& null !== obj.stream;
};

var __close = function (obj) {
	if (__ok(obj)) {
		switch(obj.type) {
			case mod.type.pipe:
				__libc__.pclose(obj.stream);

				break;
			case mod.type.gzip:
				__libz__.gzclose(obj.stream);
				break;
			case mod.type.file:  /* down */
			default:
				__libc__.fclose(obj.stream);
				break;
		}

		obj.stream = null;
	}
};

/*
mod.Stream = function (filename, mode, type) {
	var that        = this;

	that.filename   = filename;
	that.mode       = mode;
	that.name       = mod.name + '(' + filename + ')';
	switch(type) {
		case mod.type.pipe:
			that.type = type;
			that.stream = __libc__.popen(filename, mode);
			break;
		case mod.type.gzip:
			that.type = type;
			that.stream = __libz__.gzopen(filename, mode);
			break;
		case mod.type.file: // down
		default:
			that.type = type;
			that.stream = __libc__.fopen(filename, mode);
			break;
	}

	that.__ok = function () {
		return __ok(that);
	};

	that.close = function () {
		__close(that);
	};

	that.read = function (buffer) {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzread(that.stream, buffer);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.fread(that.stream, buffer);
		}
	};

	that.readEx = function (size) {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzreadEx(that.stream, size);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.freadEx(that.stream, size);
		}
	};

	that.write = function (buffer) {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzwrite(that.stream, buffer);
			case mod.type.file: // down
			default:
				return __libc__.fwrite(that.stream, buffer);
		}
	};

	that.eof = function () {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzeof(that.stream);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.feof(that.stream);
		}
	};

	that.error = function () {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzerror(that.stream);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.ferror(that.stream);
		}
	};

	that.tell = function () {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gztell(that.stream);
			case mod.type.file: // down
			default:
				return __libc__.ftell(that.stream);
		}
	};

	that.seek = function (offset, where) {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzseek(that.stream, offset, where);
			case mod.type.file: // down
			default:
				return __libc__.fseek(that.stream, offset, where);
		}
	};

	that.flush = function () {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzflush(that.stream);
			case mod.type.file: // down
			default:
				return __libc__.fflush(that.stream);
		}
	};
};
*/
mod.Stream = function (filename, mode, type) {
	var that = this;

	that.filename = filename;
	that.mode = mode;
	that.name = mod.name + '(' + filename + ')';
	switch (type) {
		case mod.type.pipe:
			that.type = type;
			that.stream = __libc__.popen(filename, mode);
			break;
		case mod.type.gzip:
			that.type = type;
			that.stream = __libz__.gzopen(filename, mode);
			break;
		case mod.type.file: // down
		default:
			that.type = type;
			that.stream = __libc__.fopen(filename, mode);
			break;
	}
};

mod.Stream.prototype = {
	ok: function () {
		return __ok(that);
	},

	close: function () {
		__close(that);
	},

	read: function (buffer) {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzread(that.stream, buffer);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.fread(that.stream, buffer);
		}
	},

	readEx: function (size) {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzreadEx(that.stream, size);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.freadEx(that.stream, size);
		}
	},

	write: function (buffer) {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzwrite(that.stream, buffer);
			case mod.type.file: // down
			default:
				return __libc__.fwrite(that.stream, buffer);
		}
	},

	error: function () {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzerror(that.stream);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.ferror(that.stream);
		}
	},

	tell: function () {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gztell(that.stream);
			case mod.type.file: // down
			default:
				return __libc__.ftell(that.stream);
		}
	},

	seek: function (offset, where) {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzseek(that.stream, offset, where);
			case mod.type.file: // down
			default:
				return __libc__.fseek(that.stream, offset, where);
		}
	},

	flush: function () {
		switch(obj.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzflush(that.stream);
			case mod.type.file: // down
			default:
				return __libc__.fflush(that.stream);
		}
	},

	eof: function () {
		switch(obj.type) {
			case mod.type.gzip:
				return __libz__.gzeof(that.stream);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.feof(that.stream);
		}
	}
};

__js__.classDestructor(mod.Stream.prototype, __close);

/* eof */