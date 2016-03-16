#!/bin/js

var prototype = this.constructor.prototype;

prototype.name = prototype.name || 'stream';

prototype.seek = prototype.seek || {
	current:__libc__.SEEK_CUR,
	begin:  __libc__.SEEK_SET,
	end:    __libc__.SEEK_END
};

prototype.type = prototype.type || {
	file: 1,
	pipe: 2,
	gzip: 3
};

var name       = null;
var filename   = null;
var mode       = null;
var type       = prototype.type.file;
var stream     = null;

prototype.__close = function (obj) {
	if (obj
		&& typeof obj.stream === 'pointer'
		&& null !== obj.stream
		&& typeof obj.close  === 'function') {
		return obj.close();
	}
};

prototype.__init = function (filename, mode, type) {
	this.filename = filename;
	this.mode = mode;
	this.type = type;
	this.name = prototype.name + '(' + filename + ')';
};

prototype.ok = function() {
	return typeof this.stream === 'pointer' && this.stream;
};

prototype.nok = function() {
	return null === this.stream;
};

prototype.fopen = function (filename, mode) {
	if (this.nok()) {
		this.__init(filename, mode, prototype.type.file);

		this.stream = __libc__.fopen(filename, mode);
	}

	return this;
};

prototype.fclose = function () {
	if (this.ok()) {
		return __libc__.fclose(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.fread = function (buffer) {
	if (this.ok()) {
		return __libc__.fread(this.stream, buffer);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.freadEx = function (size) {
	if (this.ok()) {
		return __libc__.freadEx(obj.stream, size);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.fwrite = function (buffer) {
	if (this.ok()) {
		return __libc__.fwrite(this.stream, buffer);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.ftell = function () {
	if (this.ok()) {
		return __libc__.ftell(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.fseek = function (offset, where) {
	if (this.ok()) {
		return __libc__.fseek(this.stream, offset, where);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.fflush = function () {
	if (this.ok()) {
		return __libc__.fflush(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.ferror = function () {
	if (this.ok()) {
		return __libc__.ferror(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.feof = function () {
	if (this.ok()) {
		return __libc__.feof(this.stream);
	} else {
		return true;
	}
};

prototype.popen = function (filename, mode) {
	if (this.nok()) {
		this.__init(filename, mode, prototype.type.pipe);

		this.stream = __libc__.popen(filename, mode);
	}

	return this;
};

prototype.pclose = function () {
	if (this.ok()) {
		__libc__.pclose(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzopen = function (filename, mode) {
	if (this.nok()) {
		this.__init(filename, mode, prototype.type.gzip);

		this.stream = __libz__.gzopen(filename, mode);
	}

	return this;
};

prototype.gzclose = function () {
	if (this.ok()) {
		__libz__.gzclose(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzread = function (buffer) {
	if (this.ok()) {
		return __libz__.gzread(this.stream, buffer);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzreadEx = function (size) {
	if (this.ok()) {
		return __libz__.gzreadEx(this.stream, size);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzwrite = function (buffer) {
	if (this.ok()) {
		return __libz__.gzwrite(this.stream, buffer);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gztell = function () {
	if (this.ok()) {
		return __libz__.gztell(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzseek = function (offset, where) {
	if (this.ok()) {
		return __libz__.gzseek(this.stream, offset, where);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzflush = function () {
	if (this.ok()) {
		return __libz__.gzflush(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzerror = function () {
	if (this.ok()) {
		return __libz__.gzerror(this.stream);
	} else {
		return -__libc__.EBADF;
	}
};

prototype.gzeof = function () {
	if (this.ok()) {
		return __libz__.gzeof(this.stream);
	} else {
		return true;
	}
};

prototype.open = function (filename, mode, type) {
	switch(type) {
		case prototype.type.pipe:
			return this.popen(filename, mode);
		case prototype.type.gzip:
			return this.gzopen(filename, mode);
		case prototype.type.file:   // down
		default:
			return this.fopen(filename, mode);
	}
};

prototype.close = function () {
	switch(type) {
		case prototype.type.pipe:
			return this.pclose();
		case prototype.type.gzip:
			return this.gzclose();
		case prototype.type.file:   // down
		default:
			return this.fclose();
	}
};

prototype.read = function (buffer) {
	switch(type) {
		case prototype.type.gzip:
			return this.gzread(buffer);
		case prototype.type.file:   // down
		case prototype.type.pipe:   // down
		default:
			return this.fread(buffer);
	}
};

prototype.readEx = function (size) {
	switch(type) {
		case prototype.type.gzip:
			return this.gzreadEx(size);
		case prototype.type.file:   // down
		case prototype.type.pipe:   // down
		default:
			return this.freadEx(size);
	}
};

prototype.write = function (buffer) {
	switch(type) {
		case prototype.type.pipe:
			return -__libc__.ENOSUPPORT;
		case prototype.type.gzip:
			return this.gzwrite(buffer);
		case prototype.type.file:   // down
		default:
			return this.fwrite(buffer);
	}
};

prototype.tell = function () {
	switch(type) {
		case prototype.type.pipe:
			return -__libc__.ENOSUPPORT;
		case prototype.type.gzip:
			return this.gztell();
		case prototype.type.file:   // down
		default:
			return this.ftell();
	}
};

prototype.seek = function (offset, where) {
	switch(type) {
		case prototype.type.pipe:
			return -__libc__.ENOSUPPORT;
		case prototype.type.gzip:
			return this.gzseek(offset, where);
		case prototype.type.file:   // down
		default:
			return this.fseek(offset, where);
	}
};

prototype.flush = function () {
	switch(type) {
		case prototype.type.pipe:
			return -__libc__.ENOSUPPORT;
		case prototype.type.gzip:
			return this.gzflush();
		case prototype.type.file:   // down
		default:
			return this.fflush();
	}
};

prototype.error = function () {
	switch(type) {
		case prototype.type.gzip:
			return this.gzerror();
		case prototype.type.file:   // down
		case prototype.type.pipe:   // down
		default:
			return this.ferror();
	}
};

prototype.eof = function () {
	switch(type) {
		case prototype.type.gzip:
			return this.gzeof();
		case prototype.type.file:   // down
		case prototype.type.pipe:   // down
		default:
			return this.feof();
	}
};

__js__.classDestructor(prototype, prototype.__close);

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
	var obj = stream_base(filename, mode, prototype.type.file);

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
	var obj = stream_base(filename, mode, prototype.type.pipe);

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
	var obj = stream_base(filename, mode, prototype.type.gzip);

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
		case prototype.type.pipe:
			return pipe(filename, mode);
		case prototype.type.gzip:
			return gzip(filename, mode);
		case prototype.type.file: // down
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
		case prototype.type.pipe:
			this.stream = __libc__.popen(filename, mode);
			break;
		case prototype.type.gzip:
			this.stream = __libz__.gzopen(filename, mode);
			break;
		case prototype.type.file:
			this.stream = __libc__.fopen(filename, mode);
			break;
		default:
			this.type = prototype.type.file;
			this.stream = __libc__.fopen(filename, mode);
			break;
	}
};

prototype.Stream.prototype = {
	close: function () {
		if (typeof this.stream === 'pointer' && null !== this.stream) {
			switch(this.type) {
				case prototype.type.pipe:
					__libc__.pclose(this.stream);

					break;
				case prototype.type.gzip:
					__libz__.gzclose(this.stream);
					break;
				case prototype.type.file: // down
				default:
					__libc__.fclose(this.stream);
					break;
			}

			this.stream = null;
		}
	},

	read: function (buffer) {
		switch(this.type) {
			case prototype.type.gzip:
				return __libz__.gzread(this.stream, buffer);
			case prototype.type.file: // down
			case prototype.type.pipe: // down
			default:
				return __libc__.fread(this.stream, buffer);
		}
	},

	readEx: function (size) {
		switch(this.type) {
			case prototype.type.gzip:
				return __libz__.gzreadEx(this.stream, size);
			case prototype.type.file: // down
			case prototype.type.pipe: // down
			default:
				return __libc__.freadEx(this.stream, size);
		}
	},

	write: function (buffer) {
		switch(this.type) {
			case prototype.type.pipe:
				return -__libc__.ENOSUPPORT;
			case prototype.type.gzip:
				return __libz__.gzwrite(this.stream, buffer);
			case prototype.type.file: // down
			default:
				return __libc__.fwrite(this.stream, buffer);
		}
	},

	error: function () {
		switch(this.type) {
			case prototype.type.gzip:
				return __libz__.gzerror(this.stream);
			case prototype.type.file: // down
			case prototype.type.pipe: // down
			default:
				return __libc__.ferror(this.stream);
		}
	},

	tell: function () {
		switch(this.type) {
			case prototype.type.pipe:
				return -__libc__.ENOSUPPORT;
			case prototype.type.gzip:
				return __libz__.gztell(this.stream);
			case prototype.type.file: // down
			default:
				return __libc__.ftell(this.stream);
		}
	},

	seek: function (offset, where) {
		switch(this.type) {
			case prototype.type.pipe:
				return -__libc__.ENOSUPPORT;
			case prototype.type.gzip:
				return __libz__.gzseek(this.stream, offset, where);
			case prototype.type.file: // down
			default:
				return __libc__.fseek(this.stream, offset, where);
		}
	},

	flush: function () {
		switch(this.type) {
			case prototype.type.pipe:
				return -__libc__.ENOSUPPORT;
			case prototype.type.gzip:
				return __libz__.gzflush(this.stream);
			case prototype.type.file: // down
			default:
				return __libc__.fflush(this.stream);
		}
	},

	eof: function () {
		switch(this.type) {
			case prototype.type.gzip:
				return __libz__.gzeof(this.stream);
			case prototype.type.file: // down
			case prototype.type.pipe: // down
			default:
				return __libc__.feof(this.stream);
		}
	}
};

__js__.classDestructor(prototype.Stream.prototype, prototype.close);
*/

/* eof */