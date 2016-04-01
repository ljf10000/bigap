#!/bin/js

/*
* module: stream helper
*/
var mod = this,
	pt = mod.constructor.prototype,
	base = require('stream/fp/base'),
	file = require('stream/fp/file'),
	pipe = require('stream/fp/pipe'),
	zlib = require('stream/fp/zlib'),
	bzip = require('stream/fp/bzip');

pt.__proto__ = base.constructor.prototype;
pt.$name = pt.$name || 'stream/fp/stream';
pt.$debugger = new $Debugger(pt.$name);

function fproxy (obj, func, unsupport) {
	var type = pt.type;
	var proxy;

	switch(obj.type) {
		case type.file:
		default:
			proxy = file;
			break;
		case type.pipe:
			proxy = pipe;
			break;
		case type.zlib:
			proxy = zlib;
			break;
		case type.bzip:
			proxy = bzip;
			break;
	}

	return proxy[func] || unsupport || no_support;
}

pt.open = function (obj, mode) {
	return fproxy(obj, 'open', function() {return obj;})(obj, mode);
};

pt.close = function (obj) {
	return fproxy(obj, 'close')(obj);
};

pt.read = function (obj, buffer) {
	return fproxy(obj, 'read')(obj, buffer);
};

pt.readEx = function (obj, size) {
	return fproxy(obj, 'readEx')(obj, size);
};

pt.write = function (obj, buffer) {
	return fproxy(obj, 'write')(obj, buffer);
};

pt.tell = function (obj) {
	return fproxy(obj, 'tell')(obj);
};

pt.seek = function (obj, offset, where) {
	return fproxy(obj, 'seek')(obj, offset, where);
};

pt.rewind = function (obj) {
	return fproxy(obj, 'rewind')(obj);
};

pt.sync = pt.flush = function (obj) {
	return fproxy(obj, 'sync')(obj);
};

pt.error = function (obj) {
	return fproxy(obj, 'error')(obj);
};

pt.eof = function (obj) {
	return fproxy(obj, 'eof', allways_true)(obj);
};

/* eof */
