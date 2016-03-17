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
const __debug_level__ = {
	init:	0x00010000,
	fini:	0x00020000,
	constructor:0x00040000,
	destructor:	0x00080000,
	ok:		0x00100000,
	error:	0x00200000,
	trace:	0x00400000,
	bug:	0x00800000,
	io:		0x01000000,
	st:		0x02000000,
	test: 	0x04000000,
	
	all:	0xffffffff
};
print(__my__.env.__JS_DEBUG_MOD__);
const __debug_module__ = JSON.parse(__my__.env.__JS_DEBUG_MOD__);
print(__my__.env.__JS_DEBUG_MOD__);

const __is_debug = function (mod, level) {
	if (__my__.is_debug(level)) {
		if ('all' === mod) {
			return true;
		} else {
			var count = __debug_module__.length;
			var i;
			
			for (i=0; i<count; i++) {
				if (mod === __debug_module__[i]) {
					return true;	
				}
			}
		}
	}

	return false;
};

const debug = function (mod, level) {
	if (__is_debug(mod, level)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(2).toString());
	}
};

const debug_init = function (mod) {
	if (__is_debug(mod, __debug_level__.init)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_fini = function (mod) {
	if (__is_debug(mod, __debug_level__.fini)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_constructor = function (mod) {
	if (__is_debug(mod, __debug_level__.constructor)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_destructor = function (mod) {
	if (__is_debug(mod, __debug_level__.destructor)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_ok = function (mod) {
	if (__is_debug(mod, __debug_level__.ok)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_error = function (mod) {
	if (__is_debug(mod, __debug_level__.error)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_trace = function (mod) {
	if (__is_debug(mod, __debug_level__.trace)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_bug = function (mod) {
	if (__is_debug(mod, __debug_level__.bug)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_io = function (mod) {
	if (__is_debug(mod, __debug_level__.io)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_st = function (mod) {
	if (__is_debug(mod, __debug_level__.st)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const debug_test = function (mod) {
	if (__is_debug(mod, __debug_level__.test)) {
		__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
	}
};

const ModDebugger = function (mod) {
	this.mod = mod;	
};

ModDebugger.prototype = {
/*
	debug: function (level) {
		if (__is_debug(this.mod, level)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());
		}
	},
	init: function () {
		if (__is_debug(this.mod, __debug_level__.init)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	fini: function () {
		if (__is_debug(this.mod, __debug_level__.fini)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	constructor: function () {
		if (__is_debug(this.mod, __debug_level__.constructor)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	destructor: function () {
		if (__is_debug(this.mod, __debug_level__.destructor)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	ok: function () {
		if (__is_debug(this.mod, __debug_level__.ok)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	error: function () {
		if (__is_debug(this.mod, __debug_level__.error)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	trace: function () {
		if (__is_debug(this.mod, __debug_level__.trace)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	bug: function () {
		if (__is_debug(this.mod, __debug_level__.bug)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},

	io: function () {
		if (__is_debug(this.mod, __debug_level__.io)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
	
	st: function () {
		if (__is_debug(this.mod, __debug_level__.st)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	},
*/
	test: function () {
		if (__is_debug(this.mod, __debug_level__.test)) {
			__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());
		}
	}
};
const dbg = new ModDebugger('all');

const fmt = {
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
/*
__js__.destructor = function (is_class, x, close) {
	if (typeof close === 'function') {
		__js__.fin(x, function (obj, heapDestruct) {
			var name;
			if (is_class) {
				name = obj.name || obj.prototype.name || x.name || typeof x;
			} else {
				name = obj.name || obj.prototype.name || typeof obj;
			}

			if (is_class && obj === x) {
		        dbg.destructor(name, 'skip, called for the prototype itself');
		    } else if (heapDestruct) {
				close(obj);

				dbg.destructor(name, 'closed @fini');
		    } else {
				close(obj);

				dbg.destructor(name, 'closed @destructor');
			}
		});
	} else {
		return __js__.fin(x);
	}
};
*/
/* eof */
