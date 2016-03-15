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
	ok: function () {
		return __ok(this);
	},

	close: function () {
		__close(this);
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

__js__.classDestructor(mod.Stream.prototype, __close);

/* eof */