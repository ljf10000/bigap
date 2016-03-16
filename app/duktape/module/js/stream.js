#!/bin/js
var mod = this;
var prototype = mod.constructor.prototype;

const file = 1;
const pipe = 2;
const gzip = 3;

prototype.name = prototype.name || 'stream';

prototype.SEEK_CUR = __libc__.SEEK_CUR;
prototype.SEEK_SET = __libc__.SEEK_SET;
prototype.SEEK_END = __libc__.SEEK_END;

prototype.__good_type = function(type) {
	switch (type) {
		case pipe:   // down
		case gzip:   // down
		case file:   // down
			return type;
		default:
			return file;
	}
};

prototype.__ok = function(obj) {
	return obj && typeof obj.stream === 'pointer' && obj.stream;
};

prototype.__nok = function(obj) {
	return obj && (typeof obj.stream !== 'pointer' || obj.stream);
};

prototype.__init = function (obj, filename, mode, type) {
	obj.filename = filename;
	obj.mode = mode;
	obj.type = type;
	obj.name = prototype.name + '(' + filename + ')';
};

prototype.open = function (obj, filename, mode, type) {
	if (prototype.__nok(obj)) {
		type = prototype.__good_type(type);

		prototype.__init(obj, filename, mode, type);

		switch (type) {
			case pipe:
				obj.stream = __libc__.popen(filename, mode);
				break;
			case gzip:
				obj.stream = __libz__.gzopen(filename, mode);
				break;
			case file:   // down
			default:
				obj.stream = __libc__.fopen(filename, mode);
				break;
		}
	}

	return obj;
};

prototype.close = function (obj) {
	if (prototype.__ok(obj)) {
		switch (obj.type) {
			case pipe:
				__libc__.pclose(obj.stream);
				break;
			case gzip:
				__libz__.gzclose(obj.stream);
				break;
			case file:   // down
			default:
				__libc__.fclose(obj.stream);
				break;
		}

		obj.stream = null;
	}

	return 0;
};

prototype.read = function (obj, buffer) {
	switch(obj.type) {
		case gzip:
			return __libz__.gzread(obj.stream, buffer);
		case file:   // down
		case pipe:   // down
		default:
			return __libc__.fread(obj.stream, buffer);
	}
};

prototype.readEx = function (obj, size) {
	switch(obj.type) {
		case gzip:
			return __libz__.gzreadEx(obj.stream, size);
		case file:   // down
		case pipe:   // down
		default:
			return __libc__.freadEx(obj.stream, size);
	}
};

prototype.write = function (obj, buffer) {
	switch(obj.type) {
		case pipe:
			return -__libc__.ENOSUPPORT;
		case gzip:
			return __libz__.gzwrite(obj.stream, buffer);
		case file:   // down
		default:
			return __libc__.fwrite(obj.stream, buffer);
	}
};

prototype.tell = function (obj) {
	switch(type) {
		case pipe:
			return -__libc__.ENOSUPPORT;
		case gzip:
			return __libz__.gztell(obj.stream);
		case file:   // down
		default:
			return __libc__.ftell(obj.stream);
	}
};

prototype.seek = function (obj, offset, where) {
	switch(type) {
		case pipe:
			return -__libc__.ENOSUPPORT;
		case gzip:
			return __libz__.gzseek(obj.stream, offset, where);
		case file:   // down
		default:
			return __libc__.fseek(obj.stream, offset, where);
	}
};

prototype.flush = function (obj) {
	switch(type) {
		case pipe:
			return -__libc__.ENOSUPPORT;
		case gzip:
			return __libz__.gzflush(obj.stream);
		case file:   // down
		default:
			return __libc__.fflush(obj.stream);
	}
};

prototype.error = function (obj) {
	switch(type) {
		case gzip:
			return __libz__.gzerror(obj.stream);
		case file:   // down
		case pipe:   // down
		default:
			return __libc__.ferror(obj.stream);
	}
};

prototype.eof = function (obj) {
	switch(type) {
		case gzip:
			return __libz__.gzeof(obj.stream);
		case file:   // down
		case pipe:   // down
		default:
			return __libc__.feof(obj.stream);
	}
};

mod.Stream = function (filename, mode, type) {
	return prototype.open(this, filename, mode, type)
};

mode.Stream.prototype = {
	read: function (buffer) {
		return prototype.read(this, buffer);
	},

	readEx: function (size) {
		return prototype.readEx(this, size);
	},

	write: function (buffer) {
		return prototype.write(obj, buffer);
	},

	tell: function () {
		return prototype.tell(obj);
	},

	seek: function (offset, where) {
		return prototype.seek(obj, offset, where);
	},

	flush: function () {
		return prototype.flush(obj);
	},

	error: function () {
		return prototype.error(obj);
	},

	eof: function () {
		return prototype.eof(obj);
	}
};

__js__.classDestructor(mode.Stream.prototype, prototype.close);

/*
var stream_base = function (filename, mode, type) {
	var obj = {
		name: prototype.name + '(' + filename + ')',
		filename: filename,
		mode: mode,
		type: type,

		ok: function() {
			return typeof obj.stream === 'pointer' && obj.stream;
		},

		read: function (buffer) {
			return __libc__.fread(obj.stream, buffer);
		},

		readEx: function (size) {
			return __libc__.freadEx(obj.stream, size);
		},

		error: function () {
			return __libc__.ferror(obj.stream);
		},

		eof: function () {
			return __libc__.feof(obj.stream);
		}
	};

	__js__.objDestructor(obj, prototype.close);

	return obj;
};

var file = function (filename, mode) {
	var obj = stream_base(filename, mode, file);

	obj.close = function() {
		if (obj.ok()) {
			__libc__.fclose(obj.stream);

			obj.stream = null;
		}
	};

	obj.write = function (buffer) {
		return __libc__.fwrite(obj.stream, buffer);
    };

	obj.tell = function () {
		return __libc__.ftell(obj.stream);
	};

	obj.seek = function (offset, where) {
		return __libc__.fseek(obj.stream, offset, where);
	};

	obj.flush = function () {
		return __libc__.fflush(obj.stream);
	};

	obj.stream = __libc__.fopen(obj.filename, obj.mode);

	return obj.stream?obj:null;
};

var pipe = function (filename, mode) {
	var obj = stream_base(filename, mode, pipe);

	obj.close = function() {
		if (obj.ok()) {
			__libc__.pclose(obj.stream);

			obj.stream = null;
		}
	};

	obj.write = function (buffer) {
		return -__libc__.ENOSUPPORT;
	};

	obj.tell = function () {
		return -__libc__.ENOSUPPORT;
	};

	obj.seek = function (offset, where) {
		return -__libc__.ENOSUPPORT;
	};

	obj.flush = function () {
		return -__libc__.ENOSUPPORT;
	};

	obj.stream = __libc__.popen(obj.filename, obj.mode);

	return obj.stream?obj:null;
};

var gzip = function (filename, mode) {
	var obj = stream_base(filename, mode, gzip);

	obj.close = function() {
		if (obj.ok()) {
			__libz__.gzclose(obj.stream);

			obj.stream = null;
		}
	};

	obj.read = function (buffer) {
		return __libz__.gzread(obj.stream, buffer);
	};

	obj.readEx = function (size) {
		return __libz__.gzreadEx(obj.stream, size);
	};

	obj.write = function (buffer) {
		return __libz__.gzwrite(obj.stream, buffer);
	};

	obj.error = function () {
		return __libz__.gzerror(obj.stream);
	};

	obj.tell = function () {
		return __libz__.gztell(obj.stream);
	};

	obj.seek = function (offset, where) {
		return __libz__.gzseek(obj.stream, offset, where);
	};

	obj.flush = function () {
		return __libz__.gzflush(obj.stream);
	};

	obj.eof = function () {
		return __libz__.gzeof(obj.stream);
	};

	obj.stream = __libz__.gzopen(obj.filename, obj.mode);

	return obj.stream?obj:null;
};

var stream = function (filename, mode, type) {
	switch (type) {
		case pipe:
			return pipe(filename, mode);
		case gzip:
			return gzip(filename, mode);
		case file: // down
		default:
			return file(filename, mode);
	}
};

prototype.file = prototype.file || file;
prototype.pipe = prototype.pipe || pipe;
prototype.gzip = prototype.gzip || gzip;
prototype.stream = prototype.stream || stream;
*/

/*
prototype.Stream = function (filename, mode, type) {
	this.filename = filename;
	this.mode = mode;
	this.name = prototype.name + '(' + filename + ')';
	this.type = type;
	switch (type) {
		case pipe:
			this.stream = __libc__.popen(filename, mode);
			break;
		case gzip:
			this.stream = __libz__.gzopen(filename, mode);
			break;
		case file:
			this.stream = __libc__.fopen(filename, mode);
			break;
		default:
			this.type = file;
			this.stream = __libc__.fopen(filename, mode);
			break;
	}
};

prototype.Stream.prototype = {
	close: function () {
		if (typeof this.stream === 'pointer' && null !== this.stream) {
			switch(this.type) {
				case pipe:
					__libc__.pclose(this.stream);

					break;
				case gzip:
					__libz__.gzclose(this.stream);
					break;
				case file: // down
				default:
					__libc__.fclose(this.stream);
					break;
			}

			this.stream = null;
		}
	},

	read: function (buffer) {
		switch(this.type) {
			case gzip:
				return __libz__.gzread(this.stream, buffer);
			case file: // down
			case pipe: // down
			default:
				return __libc__.fread(this.stream, buffer);
		}
	},

	readEx: function (size) {
		switch(this.type) {
			case gzip:
				return __libz__.gzreadEx(this.stream, size);
			case file: // down
			case pipe: // down
			default:
				return __libc__.freadEx(this.stream, size);
		}
	},

	write: function (buffer) {
		switch(this.type) {
			case pipe:
				return -__libc__.ENOSUPPORT;
			case gzip:
				return __libz__.gzwrite(this.stream, buffer);
			case file: // down
			default:
				return __libc__.fwrite(this.stream, buffer);
		}
	},

	error: function () {
		switch(this.type) {
			case gzip:
				return __libz__.gzerror(this.stream);
			case file: // down
			case pipe: // down
			default:
				return __libc__.ferror(this.stream);
		}
	},

	tell: function () {
		switch(this.type) {
			case pipe:
				return -__libc__.ENOSUPPORT;
			case gzip:
				return __libz__.gztell(this.stream);
			case file: // down
			default:
				return __libc__.ftell(this.stream);
		}
	},

	seek: function (offset, where) {
		switch(this.type) {
			case pipe:
				return -__libc__.ENOSUPPORT;
			case gzip:
				return __libz__.gzseek(this.stream, offset, where);
			case file: // down
			default:
				return __libc__.fseek(this.stream, offset, where);
		}
	},

	flush: function () {
		switch(this.type) {
			case pipe:
				return -__libc__.ENOSUPPORT;
			case gzip:
				return __libz__.gzflush(this.stream);
			case file: // down
			default:
				return __libc__.fflush(this.stream);
		}
	},

	eof: function () {
		switch(this.type) {
			case gzip:
				return __libz__.gzeof(this.stream);
			case file: // down
			case pipe: // down
			default:
				return __libc__.feof(this.stream);
		}
	}
};

__js__.classDestructor(prototype.Stream.prototype, prototype.close);
*/

/* eof */