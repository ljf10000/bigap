#!/bin/js

/*
* module: file mode helper
*/
var mod = this,
	name = 'file/helper/mode';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.type = {
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

mod.get_type = function (mode) {
	switch (safe_number(mode) & __libc__.S_IFMT) {
		case __libc__.S_IFSOCK: return mod.type.sock;
		case __libc__.S_IFLNK:  return mod.type.link;
		case __libc__.S_IFREG:  return mod.type.reg;
		case __libc__.S_IFBLK:  return mod.type.blk;
		case __libc__.S_IFDIR:  return mod.type.dir;
		case __libc__.S_IFCHR:  return mod.type.chr;
		case __libc__.S_IFIFO:  return mod.type.fifo;
		default:                return mod.type.nothing;
	}
};

mod.is_sock = function (mode) {
	return __libc__.S_IFSOCK === (safe_number(mode) & __libc__.S_IFMT);
};

mod.is_link = function (mode) {
	return __libc__.S_IFLNK === (safe_number(mode) & __libc__.S_IFMT);
};

mod.is_reg = function (mode) {
	return __libc__.S_IFREG === (safe_number(mode) & __libc__.S_IFMT);
};

mod.is_blk = function (mode) {
	return __libc__.S_IFBLK === (safe_number(mode) & __libc__.S_IFMT);
};

mod.is_dir = function (mode) {
	return __libc__.S_IFDIR === (safe_number(mode) & __libc__.S_IFMT);
};

mod.is_chr = function (mode) {
	return __libc__.S_IFCHR === (safe_number(mode) & __libc__.S_IFMT);
};

mod.is_fifo = function (mode) {
	return __libc__.S_IFIFO === (safe_number(mode) & __libc__.S_IFMT);
};

mod.isset_uid = function (mode) {
	return __libc__.S_ISUID === (safe_number(mode) & __libc__.S_ISVTX);
};

mod.isset_uid = function (mode) {
	return __libc__.S_ISGID === (safe_number(mode) & __libc__.S_ISVTX);
};

mod.isset_vtx = function (mode) {
	return __libc__.S_ISVTX === (safe_number(mode) & __libc__.S_ISVTX);
};

mod.is_owner_r = function (mode) {
	return __libc__.S_IRUSR === (safe_number(mode) & __libc__.S_IRWXU);
};

mod.is_owner_w = function (mode) {
	return __libc__.S_IWUSR === (safe_number(mode) & __libc__.S_IRWXU);
};

mod.is_owner_x = function (mode) {
	return __libc__.S_IXUSR === (safe_number(mode) & __libc__.S_IRWXU);
};

mod.is_group_r = function (mode) {
	return __libc__.S_IRGRP === (safe_number(mode) & __libc__.S_IRWXG);
};

mod.is_group_w = function (mode) {
	return __libc__.S_IWGRP === (safe_number(mode) & __libc__.S_IRWXG);
};

mod.is_group_x = function (mode) {
	return __libc__.S_IXGRP === (safe_number(mode) & __libc__.S_IRWXG);
};

mod.is_other_r = function (mode) {
	return __libc__.S_IROTH === (safe_number(mode) & __libc__.S_IRWXO);
};

mod.is_other_w = function (mode) {
	return __libc__.S_IWOTH === (safe_number(mode) & __libc__.S_IRWXO);
};

mod.is_other_x = function (mode) {
	return __libc__.S_IXOTH === (safe_number(mode) & __libc__.S_IRWXO);
};

/* eof */
