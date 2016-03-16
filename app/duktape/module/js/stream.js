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

mod.constructor.prototype = {
	test: function () {
		print('test');
	}
};

var close = function (obj) {
	if (obj
		&& typeof obj.stream === 'pointer'
		&& null !== obj.stream
		&& typeof obj.close  === 'function') {
		return obj.close();
	}
};

var stream_base = function (filename, mode, type) {
	var obj = {
		name: mod.name + '(' + filename + ')',
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

	__js__.objDestructor(obj, close);

	return obj;
};

var file = function (filename, mode) {
	var obj = stream_base(filename, mode, mod.type.file);

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
	var obj = stream_base(filename, mode, mod.type.pipe);

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
	var obj = stream_base(filename, mode, mod.type.gzip);

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
		case mod.type.pipe:
			return pipe(filename, mode);
		case mod.type.gzip:
			return gzip(filename, mode);
		case mod.type.file: // down
		default:
			return file(filename, mode);
	}
};

mod.file = file;
mod.pipe = pipe;
mod.gzip = gzip;
mod.stream = stream;

/*
mod.Stream = function (filename, mode, type) {
	this.filename = filename;
	this.mode = mode;
	this.name = mod.name + '(' + filename + ')';
	this.type = type;
	switch (type) {
		case mod.type.pipe:
			this.stream = __libc__.popen(filename, mode);
			break;
		case mod.type.gzip:
			this.stream = __libz__.gzopen(filename, mode);
			break;
		case mod.type.file:
			this.stream = __libc__.fopen(filename, mode);
			break;
		default:
			this.type = mod.type.file;
			this.stream = __libc__.fopen(filename, mode);
			break;
	}
};

mod.Stream.prototype = {
	close: function () {
		if (typeof this.stream === 'pointer' && null !== this.stream) {
			switch(this.type) {
				case mod.type.pipe:
					__libc__.pclose(this.stream);

					break;
				case mod.type.gzip:
					__libz__.gzclose(this.stream);
					break;
				case mod.type.file: // down
				default:
					__libc__.fclose(this.stream);
					break;
			}

			this.stream = null;
		}
	},

	read: function (buffer) {
		switch(this.type) {
			case mod.type.gzip:
				return __libz__.gzread(this.stream, buffer);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.fread(this.stream, buffer);
		}
	},

	readEx: function (size) {
		switch(this.type) {
			case mod.type.gzip:
				return __libz__.gzreadEx(this.stream, size);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.freadEx(this.stream, size);
		}
	},

	write: function (buffer) {
		switch(this.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzwrite(this.stream, buffer);
			case mod.type.file: // down
			default:
				return __libc__.fwrite(this.stream, buffer);
		}
	},

	error: function () {
		switch(this.type) {
			case mod.type.gzip:
				return __libz__.gzerror(this.stream);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.ferror(this.stream);
		}
	},

	tell: function () {
		switch(this.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gztell(this.stream);
			case mod.type.file: // down
			default:
				return __libc__.ftell(this.stream);
		}
	},

	seek: function (offset, where) {
		switch(this.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzseek(this.stream, offset, where);
			case mod.type.file: // down
			default:
				return __libc__.fseek(this.stream, offset, where);
		}
	},

	flush: function () {
		switch(this.type) {
			case mod.type.pipe:
				return -__libc__.ENOSUPPORT;
			case mod.type.gzip:
				return __libz__.gzflush(this.stream);
			case mod.type.file: // down
			default:
				return __libc__.fflush(this.stream);
		}
	},

	eof: function () {
		switch(this.type) {
			case mod.type.gzip:
				return __libz__.gzeof(this.stream);
			case mod.type.file: // down
			case mod.type.pipe: // down
			default:
				return __libc__.feof(this.stream);
		}
	}
};

__js__.classDestructor(mod.Stream.prototype, close);
*/

/* eof */