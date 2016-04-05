#!/bin/js

/*
* module: file binding
*/
var mod = this,
	pt = mod.constructor.prototype;

pt.$name = pt.$name || 'file/binding';
pt.$debugger = new $Debugger(pt.$name);

pt.load = function (obj) {
	if (obj.filename && __libc__.fexist(obj.filename)) {
		obj.content = obj.reader(__my__.readtxt(obj.filename));
	} else {
		obj.content = obj.reader('');
	}
};

pt.save = function (obj, writer) {
	if (obj.filename && obj.content) {
		__my__.writefile(obj.filename, obj.writer(obj.content));
	}
};

pt.binding = function (filename, direct, reader, writer) {
	var binding = {
		$name: pt.$name + '(' + filename + ')',
		filename: filename,
		reader: reader || allways_pass,
		writer: writer || allways_pass
	},  new_binding;

	//pt.$debugger.init(binding);
	fmt.oprint('binding', binding);

	if (true === direct) {
		new_binding = new Proxy(binding, {
			get: function (obj, key) {
				if (key==='content') {
					pt.load(obj);
				}

				return obj[key];
			},

			set: function (obj, key, value) {
				obj[key] = value;

				if (key==='content') {
					pt.save(obj);
				}
			}
		});
	} else {
		print('new_binding 1');
		new_binding = Object.create({}, binding);
		print('new_binding 2');

		new_binding = Object.create({
			load: function () {
				pt.load(binding);
			},

			save: function () {
				pt.save(binding);
			}
		}, binding);
		print('new_binding 3');
	}

	//pt.$debugger.init(new_binding);
	fmt.oprint('new_binding', new_binding);

	return new_binding;
};

pt.jsonBinding = function (filename) {
	return pt.binding(filename, false, JSON.parse, JSON.stringify);
};

pt.txtCacheBinding = function (filename) {
	return pt.binding(filename, false);
};

pt.txtDirectBinding = function (filename) {
	return pt.binding(filename, true);
};

/* eof */
