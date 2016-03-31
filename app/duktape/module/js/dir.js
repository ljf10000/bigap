#!/bin/js

/*
* module: dir
*/

var mod = this;
var pt = mod.constructor.prototype;
pt.$name = pt.$name || 'dir';
pt.$debugger = new $Debugger(pt.$name);

pt.getcwd = function () {
	return __libc__.getcwd();
};

pt.chdir = function (dir) {
	return __libc__.chdir(dir);
};

pt.link = function (olddir, newdir) {
	return __libc__.link(olddir, newdir);
};

pt.unlink = function (link) {
	return __libc__.unlink(link);
};

pt.symlink = function (olddir, newdir) {
	return __libc__.symlink(olddir, newdir);
};

pt.readlink = function (link) {
	return __libc__.readlink(link);
};

pt.realpath = function (link) {
	return __libc__.realpath(link);
};

pt.rmdir = function (dir, recursion) {
	return __libc__.rmdir(dir, recursion);
};

pt.remove = function (dir) {
	return __libc__.remove(dir);
};

pt.rename = function (oldfile, newfile) {
	return __libc__.rename(oldfile, newfile);
};

pt.mkdir = function (dir, mod) {
	return __libc__.mkdir(dir, mod);
};

pt.stat = function (file) {
	return __libc__.stat(file);
};

pt.lstat = function (file) {
	return __libc__.lstat(file);
};



pt.open = function (dir) {
	switch (typeof dir) {
		case 'number':
			return __libc__.fdopendir(dir);
		case 'string':
			return __libc__.opendir(dir);
		default:
			return null;
	}
};

pt.dirfd = function (DIR) {
	return __libc__.dirfd(DIR);
};

pt.read = function (DIR) {
	return __libc__.readdir(DIR);
};

pt.close = function (DIR) {
	return __libc__.closedir(DIR);
};

pt.rewind = function (DIR) {
	return __libc__.rewinddir(DIR);
};

pt.tell = function (DIR) {
	return __libc__.telldir(DIR);
};

pt.seek = function (DIR, pos) {
	__libc__.seekdir(DIR, pos);
};


/* eof */
