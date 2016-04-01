#!/bin/js

/*
* module: file mode helper
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'file/fp/mode';
pt.$debugger = new $Debugger(pt.$name);

pt.type = {
	nothing: 0,

	file: 1,
	dir: 2,
	chr: 3,
	blk: 4,
	reg: 5,
	fifo: 6,
	link: 7,
	sock: 8
};

pt.get_type = function (mode) {
	switch (mode & __libc__.S_IFMT) {
		case __libc__.S_IFSOCK: return pt.type.sock;
		case __libc__.S_IFLNK:  return pt.type.link;
		case __libc__.S_IFREG:  return pt.type.reg;
		case __libc__.S_IFBLK:  return pt.type.blk;
		case __libc__.S_IFDIR:  return pt.type.dir;
		case __libc__.S_IFCHR:  return pt.type.chr;
		case __libc__.S_IFIFO:  return pt.type.fifo;
		default:                return pt.type.nothing;
	}
};

pt.is_sock = function (mode) {
	return __libc__.S_IFSOCK === (mode & __libc__.S_IFMT);
};

pt.is_link = function (mode) {
	return __libc__.S_IFLNK === (mode & __libc__.S_IFMT);
};

pt.is_reg = function (mode) {
	return __libc__.S_IFREG === (mode & __libc__.S_IFMT);
};

pt.is_blk = function (mode) {
	return __libc__.S_IFBLK === (mode & __libc__.S_IFMT);
};

pt.is_dir = function (mode) {
	return __libc__.S_IFDIR === (mode & __libc__.S_IFMT);
};

pt.is_chr = function (mode) {
	return __libc__.S_IFCHR === (mode & __libc__.S_IFMT);
};

pt.is_fifo = function (mode) {
	return __libc__.S_IFIFO === (mode & __libc__.S_IFMT);
};

pt.isset_uid = function (mode) {
	return __libc__.S_ISUID === (mode & __libc__.S_ISUID);
};

pt.isset_uid = function (mode) {
	return __libc__.S_ISGID === (mode & __libc__.S_ISGID);
};

pt.isset_vtx = function (mode) {
	return __libc__.S_ISVTX === (mode & __libc__.S_ISVTX);
};

pt.is_owner_r = function (mode) {
	return __libc__.S_IRUSR === (mode & __libc__.S_IRWXU);
};

pt.is_owner_w = function (mode) {
	return __libc__.S_IWUSR === (mode & __libc__.S_IRWXU);
};

pt.is_owner_x = function (mode) {
	return __libc__.S_IXUSR === (mode & __libc__.S_IRWXU);
};

pt.is_group_r = function (mode) {
	return __libc__.S_IRGRP === (mode & __libc__.S_IRWXG);
};

pt.is_group_w = function (mode) {
	return __libc__.S_IWGRP === (mode & __libc__.S_IRWXG);
};

pt.is_group_x = function (mode) {
	return __libc__.S_IXGRP === (mode & __libc__.S_IRWXG);
};

pt.is_other_r = function (mode) {
	return __libc__.S_IROTH === (mode & __libc__.S_IRWXO);
};

pt.is_other_w = function (mode) {
	return __libc__.S_IWOTH === (mode & __libc__.S_IRWXO);
};

pt.is_other_x = function (mode) {
	return __libc__.S_IXOTH === (mode & __libc__.S_IRWXO);
};

/* eof */
