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

__js__.destructor(true, mode.Stream.prototype, prototype.close);

/* eof */