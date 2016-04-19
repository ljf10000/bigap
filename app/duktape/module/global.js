/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/
print('global 1');
function do_nothing () {}
function no_support () { return -__LIBC__.ENOSUPPORT; }

function must (obj, type) { return typeof obj === type;}
function must_object (obj) { return obj && must(obj, 'object'); }
function must_function (obj) { return must(obj, 'function'); }
function must_string (obj) { return must(obj, 'string'); }
function must_number (obj) { return must(obj, 'number'); }
function must_bool (obj) { return must(obj, 'boolean'); }

function maybe (obj, type) { return typeof obj === type ? obj : undefined;}
function maybe_object (obj) { return (obj && typeof obj === 'object') ? obj : undefined; }
function maybe_function (obj) { return maybe(obj, 'function'); }
function maybe_string (obj) { return maybe(obj, 'string'); }
function maybe_number (obj) { return maybe(obj, 'number'); }
function maybe_bool (obj) { return maybe(obj, 'boolean'); }

function allways (obj) { return obj; }
function allways_null () { return null; }
function allways_undefined () { return undefined; }
function allways_false () { return false; }
function allways_true () { return true; }
function allways_empty () { return {}; }

function safe_function (f, safe) {
	return maybe_function(f) || maybe_function(safe) || no_support;
}

function safe_string (s, safe) {
	return maybe_string(s) || maybe_string(safe) || UNKNOW_STRING;
}

function safe_number (n, safe) {
	return maybe_number(n) || maybe_number(safe) || UNKNOW_NUMBER;
}
print('global 2');
const
	__js__        = Duktape,
	BIG_ENDIAN    = (1===__my__.BIG_ENDIAN),
	LITTLE_ENDIAN = (1===__my__.LITTLE_ENDIAN),
	OS_BITS       = 8*__my__.SIZEOF_POINTER,
	UNKNOW_STRING = 'unknow',
	UNKNOW_NUMBER = 0,
	fmt = {
		oprint: function (name, obj) {
			var root = {};

			root[name] = obj;

			print(Duktape.enc('jc', root, null, 4));
		},

		separator: function (name, sep) {
			var s = maybe_string(sep)?sep:'==========';

			print(s, name, s);
		}
	},
	$LOG_EMERG            = 0,
	$LOG_ALERT            = 1,
	$LOG_CRIT             = 2,
	$LOG_ERROR            = 3,
	$LOG_WARNING          = 4,
	$LOG_NOTICE           = 5,
	$LOG_INFO             = 6,
	$LOG_DEBUG            = 7;
print('global 3');
// Object
(function () {
	do_nothing();
}());
print('global 4');
// Array
(function () {
	var pt = Array.prototype;

	pt.includes = function (element, fromIndex) {
		var i, count = this.length;

		for (i=fromIndex; i<count; i++) {
			if (element===this[i]) {
				return true;
			}
		}

		return false;
	};
}());
print('global 5');
// TypedArray
(function () {
	print('global TypedArray 1');
	function bits_mask (obj, bit) {
		return 1<<(bit%(8*obj.BYTES_PER_ELEMENT));
	}
	print('global TypedArray 2');
	function bits_elt (obj, bit) {
		return Math.floor(bit/(8*obj.BYTES_PER_ELEMENT));
	}
	print('global TypedArray 3');

	function bits_set (obj, bit) {
		obj[bits_elt(obj, bit)] |= bits_mask(obj, bit);
	}
	print('global TypedArray 4');

	function bits_clr (obj, bit) {
		obj[bits_elt(obj, bit)] &= ~bits_mask(obj, bit);
	}
	print('global TypedArray 5');

	function bits_isset (obj, bit) {
		return 0 != (obj[bits_elt(obj, bit)] & bits_mask(obj, bit));
	}
	print('global TypedArray 6');

	var arrays = [Uint8Array, Uint16Array, Uint32Array];
	print('global TypedArray 7');
	var i, pt, count = arrays.length;
	print('global TypedArray 8');

	for (i=0; i<count; i++) {
		print('global TypedArray 8.1');
		pt = arrays[i].prototype;
		print('global TypedArray 8.2');

		pt.BITS_PER_ELEMENT = 8*pt.BYTES_PER_ELEMENT;
		print('global TypedArray 8.3');
		pt.bits_set = function (bit) {
			bits_set(this, bit);
		};
		print('global TypedArray 8.4');
		pt.bits_clr = function (bit) {
			bits_clr(this, bit);
		};
		print('global TypedArray 8.5');
		pt.bits_isset = function (bit) {
			return bits_isset(this, bit);
		};
		print('global TypedArray 8.6');
	}
	print('global TypedArray 9');
}());
print('global 6');
// $Debugger
(function (global) {
	/*
	* 0x00000001 ~ 0x00008000, user debug level
	* 0x00010000 ~ 0x80000000, sys  debug level
	*/
	var modules = __my__.env.__JMOD__?JSON.parse(__my__.env.__JMOD__):[],
		slice = Array.prototype.slice,
		pt;

	function is_debug(mod, level) {
		var count = modules.length;

		if (count > 0 && __my__.is_debug(level)) {
			if ('all' === mod) {
				return true;
			} else {
				var i;

				for (i = 0; i < count; i++) {
					if (mod === modules[i]) {
						return true;
					}
				}
			}
		}

		return false;
	}

	global.$Debugger = function (mod) {
		this.mod = function () { return mod; };
	};

	global.$Debugger.prototype = pt = {
		INIT: 0x00010000,
		FINI: 0x00020000,
		CONSTRUCTOR: 0x00040000,
		DESTRUCTOR: 0x00080000,
		OK: 0x00100000,
		ERROR: 0x00200000,
		TRACE: 0x00400000,
		BUG: 0x00800000,
		IO: 0x01000000,
		ST: 0x02000000,
		TEST: 0x04000000,
		ALL: 0xffffffff,

		debug: function (level) {
			if (is_debug(this.mod(), level)) {
				__my__.debug(slice.call(arguments).slice(1).toString());
			}
		},

		init: function () {
			if (is_debug(this.mod(), pt.INIT)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		fini: function () {
			if (is_debug(this.mod(), pt.FINI)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		constructor: function () {
			if (is_debug(this.mod(), pt.CONSTRUCTOR)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		destructor: function () {
			if (is_debug(this.mod(), pt.DESTRUCTOR)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		ok: function () {
			if (is_debug(this.mod(), pt.OK)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		error: function () {
			if (is_debug(this.mod(), pt.ERROR)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		trace: function () {
			if (is_debug(this.mod(), pt.TRACE)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		bug: function () {
			if (is_debug(this.mod(), pt.BUG)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		io: function () {
			if (is_debug(this.mod(), pt.IO)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		st: function () {
			if (is_debug(this.mod(), pt.ST)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		test: function () {
			if (is_debug(this.mod(), pt.TEST)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		}
	};

	global.debug = function (mod, level) {
		if (is_debug(mod, level)) {
			__my__.debug(slice.call(arguments).slice(2).toString());
		}
	};

	global.debug_init = function (mod) {
		if (is_debug(mod, pt.INIT)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_fini = function (mod) {
		if (is_debug(mod, pt.FINI)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_constructor = function (mod) {
		if (is_debug(mod, pt.CONSTRUCTOR)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_destructor = function (mod) {
		if (is_debug(mod, pt.DESTRUCTOR)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_ok = function (mod) {
		if (is_debug(mod, pt.OK)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_error = function (mod) {
		if (is_debug(mod, pt.ERROR)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_trace = function (mod) {
		if (is_debug(mod, pt.TRACE)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_bug = function (mod) {
		if (is_debug(mod, pt.BUG)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_io = function (mod) {
		if (is_debug(mod, pt.IO)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_st = function (mod) {
		if (is_debug(mod, pt.ST)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_test = function (mod) {
		if (is_debug(mod, pt.TEST)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};
}(this));
print('global 7');
// __js__
(function () {
	__js__.destructor = function (is_class, x, close) {
		if (typeof close === 'function') {
			__js__.fin(x, function (obj, heapDestruct) {
				var name = maybe_function(obj.$name)
						|| maybe_function(obj.prototype.$name)
						|| maybe_function(x.$name)
						|| function () { return typeof x;},
					info;

				if (is_class && obj === x) {
					info = 'closed skip, called for the prototype itself';
			    } else if (heapDestruct) {
					close(obj);
					info = 'closed @fini';
			    } else {
					close(obj);
					info = 'closed @destructor';
				}

				debug_destructor('all', name(), info);
			});
		} else {
			return __js__.fin(x);
		}
	};
}());
print('global 8');
/* eof */
