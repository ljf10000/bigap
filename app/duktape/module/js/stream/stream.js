#!/bin/js

/*
* module: stream
*/
var mod = this,
	pt = mod.__proto__,
	name = 'stream/stream',
	bzip = {}, file = {}, pipe = {}, zlib = {};

pt.$name = function () { return name; };
pt.$debugger = new $Debugger(name);

pt.is_open = function (obj) {
	return typeof obj.stream === 'pointer' && obj.stream;
};

pt.is_close = function (obj) {
	return null === obj.stream;
};

print('stream', 1);
bzip.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libbz__.bzopen(obj.filename, obj.mode);
	}

	return obj;
};
print('stream', 1.2);

bzip.close = function (obj) {
	if (obj && pt.is_open(obj)) {
		__libbz__.bzclose(obj.stream);
		obj.stream = null;
	}
};
print('stream', 1.3);

bzip.read = function (obj, buffer) {
	return __libbz__.bzread(obj.stream, buffer);
};
print('stream', 1.4);
bzip.readEx = function (obj, size) {
	return __libbz__.bzreadEx(obj.stream, size);
};
print('stream', 1.5);
bzip.write = function (obj, buffer) {
	return __libbz__.bzwrite(obj.stream, buffer);
};
print('stream', 1.6);
bzip.sync = bzip.flush = function (obj) {
	return __libbz__.bzflush(obj.stream);
};
print('stream', 1.7);
bzip.error = function (obj) {
	return __libbz__.bzerror(obj.stream);
};

print('stream', 2);
file.open = function (obj, mode) {
	print('mode=', mode);
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		print('filename=',obj.filename, 'mode=', obj.mode);
		obj.stream = __libc__.fopen(obj.filename, obj.mode);
	}

	return obj;
};

file.close = function (obj) {
	if (obj && pt.is_open(obj)) {
		__libc__.fclose(obj.stream);
		obj.stream = null;
	}
};

file.read = function (obj, buffer) {
	return __libc__.fread(obj.stream, buffer);
};

file.readEx = function (obj, size) {
	return __libc__.freadEx(obj.stream, size);
};

file.write = function (obj, buffer) {
	return __libc__.fwrite(obj.stream, buffer);
};

file.tell = function (obj) {
	return __libc__.ftell(obj.stream);
};

file.seek = function (obj, offset, where) {
	return __libc__.fseek(obj.stream, offset, where);
};

file.rewind = function (obj) {
	return __libc__.fseek(obj.stream, 0, __libc__.SEEK_SET);
};

file.sync = file.flush = function (obj) {
	return __libc__.fflush(obj.stream);
};

file.error = function (obj) {
	return __libc__.ferror(obj.stream);
};

file.eof = function (obj) {
	return __libc__.feof(obj.stream);
};


print('stream', 3);
pipe.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libc__.popen(obj.filename, obj.mode);
	}

	return obj;
};

pipe.close = function (obj) {
	if (obj && pt.is_open(obj)) {
		__libc__.pclose(obj.stream);
		obj.stream = null;
	}
};

pipe.read = function (obj, buffer) {
	return __libc__.fread(obj.stream, buffer);
};

pipe.readEx = function (obj, size) {
	return __libc__.freadEx(obj.stream, size);
};

pipe.error = function (obj) {
	return __libc__.ferror(obj.stream);
};

pipe.eof = function (obj) {
	return __libc__.feof(obj.stream);
};


zlib.open = function (obj, mode) {
	if (obj && pt.is_close(obj)) {
		obj.mode = mode || obj.mode;
		obj.stream = __libz__.gzopen(obj.filename, obj.mode);
	}

	return obj;
};

print('stream', 4);
zlib.close = function (obj) {
	if (obj && pt.is_open(obj)) {
		__libz__.gzclose(obj.stream);
		obj.stream = null;
	}
};

zlib.read = function (obj, buffer) {
	return __libz__.gzread(obj.stream, buffer);
};

zlib.readEx = function (obj, size) {
	return __libz__.gzreadEx(obj.stream, size);
};

zlib.write = function (obj, buffer) {
	return __libz__.gzwrite(obj.stream, buffer);
};

zlib.tell = function (obj) {
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

zlib.seek = function (obj, offset, where) {
	return __libz__.gzseek(obj.stream, offset, where);
};

zlib.rewind = function (obj) {
	return __libz__.gzseek(obj.stream, 0, __libc__.SEEK_SET);
};

zlib.sync = zlib.flush = function (obj) {
	return __libz__.gzflush(obj.stream, __libz__.Z_NO_FLUSH);
};

zlib.error = function (obj) {
	return __libz__.gzerror(obj.stream);
};

zlib.eof = function (obj) {
	return __libz__.gzeof(obj.stream);
};

print('stream', 5);
var helper = {
	file: file,
	pipe: pipe,
	zlib: zlib,
	bzip: bzip
};

function method (obj, funcname, fsafe) {
	print('method type=', obj.type);
	return safe_function(helper[obj.type][funcname], fsafe);
}

print('stream', 6);
pt.open = function (obj, mode) {
	print('stream pt open mode=', mode);
	return method(obj, 'open', function() {return obj;})(obj, mode);
};

pt.close = function (obj) {
	print('stream pt close');
	return method(obj, 'close')(obj);
};

pt.read = function (obj, buffer) {
	print('stream pt read');
	return method(obj, 'read')(obj, buffer);
};

pt.readEx = function (obj, size) {
	print('stream pt readEx');
	return method(obj, 'readEx')(obj, size);
};

pt.write = function (obj, buffer) {
	print('stream pt write');
	return method(obj, 'write')(obj, buffer);
};

pt.tell = function (obj) {
	print('stream pt tell');
	return method(obj, 'tell')(obj);
};

pt.seek = function (obj, offset, where) {
	print('stream pt seek');
	return method(obj, 'seek')(obj, offset, where);
};

pt.rewind = function (obj) {
	print('stream pt rewind');
	return method(obj, 'rewind')(obj);
};

pt.sync = pt.flush = function (obj) {
	print('stream pt sync');
	return method(obj, 'sync')(obj);
};

pt.error = function (obj) {
	print('stream pt error');
	return method(obj, 'error')(obj);
};

pt.eof = function (obj) {
	print('stream pt eof');
	return method(obj, 'eof', allways_true)(obj);
};

print('stream', 7);
mod.Stream = function (filename, mode, type, pre_open) {
	var tmp_filename = maybe_string(filename);

	this.filename = tmp_filename;
	this.mode = maybe_string(mode) || 'r';
	this.type = (must_string(type) && helper.hasOwnProperty(type))?type:'file';
	this.$name = function () { return pt.$name() + '(' + tmp_filename + ')'; };
	this.stream = null;

	print('stream.open pre_open=', pre_open);

	if (true === pre_open) {
		pt.open(this);
	}
};

print('stream', 8);
mod.Stream.prototype = {
	is_open: function () {
		return pt.is_open(this);
	},

	is_close: function () {
		return pt.is_close(this);
	},

	open: function (mode) {
		print('stream.open mode=', mode);
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

print('stream', 9);
/* eof */
