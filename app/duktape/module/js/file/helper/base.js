#!/bin/js
/*
* module: base stream helper
*/
var mod = this,
	name = 'file/helper/base';

mod.$name = function () { return name; };
mod.$debugger = new $Debugger(name);

mod.file = function (obj, name, filename, flag, mode) {
	var tmp_filename = maybe_string(filename),
		tmp_fmode = __libc__.fexist(tmp_filename)?__libc__.lstat(tmp_filename).mode: 0;

	obj.$name = function () { return name + '(' + tmp_filename + ')'; };
	obj.filename = tmp_filename;
	obj.flag = maybe_number(flag) || __libc__.O_RDONLY;
	obj.mode = maybe_number(mode) || 0;
	obj.fmode = tmp_fmode;
	obj.ftype = fmode.get_type(tmp_fmode);
	obj.fd = -1;

	return obj;
};

/* eof */
