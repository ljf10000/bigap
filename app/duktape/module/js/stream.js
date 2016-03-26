#!/bin/js

/*
* module: stream
*/

var mod = this;
var prototype = mod.constructor.prototype;
prototype.name = prototype.name || 'stream';

prototype.filetype = 1;
prototype.pipetype = 2;
prototype.zlibtype = 3;
prototype.bziptype = 4;

prototype.SEEK_CUR = __libc__.SEEK_CUR;
prototype.SEEK_SET = __libc__.SEEK_SET;
prototype.SEEK_END = __libc__.SEEK_END;

prototype.open = function (obj, filename, mode, type) {
	if (obj && (typeof obj.stream !== 'pointer' || null === obj.stream)) {
		obj.filename = filename;
		obj.mode = mode;
		obj.type = type;
		obj.name = prototype.name + '(' + filename + ')';

		switch (type) {
			case prototype.filetype:
				obj.stream = __libc__.fopen(filename, mode);
				break;
			case prototype.pipetype:
				obj.stream = __libc__.popen(filename, mode);
				break;
			case prototype.zlibtype:
				obj.stream = __libz__.gzopen(filename, mode);
				break;
			case prototype.bziptype:
				obj.stream = __libbz__.bzopen(filename, mode);
				break;
			default:
				obj.type = 0; // invalid type
				break;
		}
	}

	return obj;
};

prototype.close = function (obj) {
	if (obj && typeof obj.stream === 'pointer' && obj.stream) {
		switch (obj.type) {
			case prototype.filetype:
				__libc__.fclose(obj.stream);
				break;
			case prototype.pipetype:
				__libc__.pclose(obj.stream);
				break;
			case prototype.zlibtype:
				__libz__.gzclose(obj.stream);
				break;
			case prototype.bziptype:
				__libbz__.bzclose(obj.stream);
				break;
			default:
				break;
		}

		obj.stream = null;
	}

	return 0;
};

prototype.read = function (obj, buffer) {
	switch(obj.type) {
		case prototype.filetype: // down
		case prototype.pipetype:
			return __libc__.fread(obj.stream, buffer);
		case prototype.zlibtype:
			return __libz__.gzread(obj.stream, buffer);
		case prototype.bziptype:
			return __libbz__.bzread(obj.stream, buffer);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.readEx = function (obj, size) {
	switch(obj.type) {
		case prototype.filetype: // down
		case prototype.pipetype:
			return __libc__.freadEx(obj.stream, size);
		case prototype.zlibtype:
			return __libz__.gzreadEx(obj.stream, size);
		case prototype.bziptype:
			return __libbz__.bzreadEx(obj.stream, size);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.write = function (obj, buffer) {
	switch(obj.type) {
		case prototype.filetype:
			return __libc__.fwrite(obj.stream, buffer);
		case prototype.zlibtype:
			return __libz__.gzwrite(obj.stream, buffer);
		case prototype.bziptype:
			return __libbz__.bzwrite(obj.stream, buffer);
		case prototype.pipetype: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.tell = function (obj) {
	switch(type) {
		case prototype.filetype:
			return __libc__.ftell(obj.stream);
		case prototype.zlibtype:
			return __libz__.gztell(obj.stream);
		case prototype.pipetype: // down
		case prototype.bziptype: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.seek = function (obj, offset, where) {
	switch(type) {
		case prototype.filetype:
			return __libc__.fseek(obj.stream, offset, where);
		case prototype.pipetype:
			return -__libc__.ENOSUPPORT;
		case prototype.zlibtype:
			return __libz__.gzseek(obj.stream, offset, where);
		case prototype.bziptype: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.flush = function (obj) {
	switch(type) {
		case prototype.filetype:
			return __libc__.fflush(obj.stream);
		case prototype.zlibtype:
			return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
		case prototype.bziptype:
			return __libbz__.bzflush(obj.stream);
		case prototype.pipetype: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.error = function (obj) {
	switch(type) {
		case prototype.filetype: // down
		case prototype.pipetype:
			return __libc__.ferror(obj.stream);
		case prototype.zlibtype:
			return __libz__.gzerror(obj.stream);
		case prototype.bziptype:
			return __libbz__.bzerror(obj.stream);
		default:
			return -__libc__.ENOSUPPORT;
	}
};

prototype.eof = function (obj) {
	switch(type) {
		case prototype.filetype: // down
		case prototype.pipetype:
			return __libc__.feof(obj.stream);
		case prototype.zlibtype:
			return __libz__.gzeof(obj.stream);
		case prototype.bziptype: // down
		default:
			return -__libc__.ENOSUPPORT;
	}
};

mod.debugger = new ModDebugger(prototype.name);

mod.Stream = function (filename, mode, type) {
	return prototype.open(this, filename, mode, type)
};

mod.Stream.prototype = {
	read: function (buffer) {
		return prototype.read(this, buffer);
	},

	readEx: function (size) {
		return prototype.readEx(this, size);
	},

	write: function (buffer) {
		return prototype.write(this, buffer);
	},

	tell: function () {
		return prototype.tell(this);
	},

	seek: function (offset, where) {
		return prototype.seek(this, offset, where);
	},

	flush: function () {
		return prototype.flush(this);
	},

	error: function () {
		return prototype.error(this);
	},

	eof: function () {
		return prototype.eof(this);
	}
};

__js__.destructor(true, mod.Stream.prototype, prototype.close);

/* eof */
