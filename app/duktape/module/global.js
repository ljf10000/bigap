/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/
const __js__ = Duktape;

/*
* 0x00000001 ~ 0x00008000, user debug level
* 0x00010000 ~ 0x80000000, sys  debug level
*/
const __debug__ = {
	init:	0x00010000,
	fini:	0x00020000,
	constructor:0x00040000,
	destructor:	0x00080000,
	ok:		0x00100000,
	error:	0x00200000,
	trace:	0x00400000,
	bug:	0x00800000,
	mod:	0x01000000,
	io:		0x02000000,
	st:		0x04000000,
	
	test: 	0x80000000
};

function debug(level) {
	__my__.debug(level, Array.prototype.slice.call(arguments).slice(1).toString());
}

function debug_init() {
	__my__.debug(__debug__.init, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_fini() {
	__my__.debug(__debug__.fini, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_constructor() {
	__my__.debug(__debug__.constructor, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_destructor() {
	__my__.debug(__debug__.destructor, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_ok() {
	__my__.debug(__debug__.ok, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_error() {
	__my__.debug(__debug__.error, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_trace() {
	__my__.debug(__debug__.trace, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_bug() {
	__my__.debug(__debug__.bug, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_mod() {
	__my__.debug(__debug__.mod, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_io() {
	__my__.debug(__debug__.io, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_st() {
	__my__.debug(__debug__.st, Array.prototype.slice.call(arguments).slice(0).toString());
}

function debug_test() {
	__my__.debug(__debug__.test, Array.prototype.slice.call(arguments).slice(0).toString());
}

var fmt = {
	oprint: function (name, obj) {
		var c = {};
	
		c[name] = obj;
	
		print(Duktape.enc('jc', c, null, 4));
	},
	
	separator: function (name, sep) {
		var s = sep?sep:'==========';
	
		print(s, name, s);
	}
};

/* keep it last */
