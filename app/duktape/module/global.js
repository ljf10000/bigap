/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/
const
	__js__        = Duktape,
	BIG_ENDIAN    = (1===__my__.BIG_ENDIAN),
	LITTLE_ENDIAN = (1===__my__.LITTLE_ENDIAN),
	OS_BITS       = 8*__my__.SIZEOF_POINTER,
	fmt = {
		oprint: function (name, obj) {
			var root = {};

			root[name] = obj;

			print(Duktape.enc('jc', root, null, 4));
		},

		separator: function (name, sep) {
			var s = sep?sep:'==========';

			print(s, name, s);
		}
	},
	$debug_level_init          = 0x00010000,
	$debug_level_fini          = 0x00020000,
	$debug_level_constructor   = 0x00040000,
	$debug_level_destructor    = 0x00080000,
	$debug_level_ok            = 0x00100000,
	$debug_level_error         = 0x00200000,
	$debug_level_trace         = 0x00400000,
	$debug_level_bug           = 0x00800000,
	$debug_level_io            = 0x01000000,
	$debug_level_st            = 0x02000000,
	$debug_level_test          = 0x04000000,
	$debug_level_all           = 0xffffffff;

function do_nothing () {}
function no_support() { return -__LIBC__.ENOSUPPORT; }
function allways_null() { return null; }
function allways_undefined() { return undefined; }
function allways_false() { return false; }
function allways_true() { return true; }
function allways_empty() { return {}; }
function allways_pass(obj) { return obj; }
function safefun(f, fsafe) {
	return (typeof f === 'function' && f) || fsafe || no_support;
}

// $Debugger
(function (global) {
	/*
	* 0x00000001 ~ 0x00008000, user debug level
	* 0x00010000 ~ 0x80000000, sys  debug level
	*/
	const
		modules = __my__.env.__JMOD__?JSON.parse(__my__.env.__JMOD__):[],
		call = Array.prototype.slice.call;

	function is_debug(mod, level) {
		if (__my__.is_debug(level)) {
			if ('all' === mod) {
				return true;
			} else {
				var count = modules.length;
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

	global.debug = function (mod, level) {
		if (is_debug(mod, level)) {
			__my__.debug(call(arguments).slice(2).toString());
		}
	};

	global.debug_init = function (mod) {
		if (is_debug(mod, $debug_level_init)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_fini = function (mod) {
		if (is_debug(mod, $debug_level_fini)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_constructor = function (mod) {
		if (is_debug(mod, $debug_level_constructor)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_destructor = function (mod) {
		if (is_debug(mod, $debug_level_destructor)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_ok = function (mod) {
		if (is_debug(mod, $debug_level_ok)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_error = function (mod) {
		if (is_debug(mod, $debug_level_error)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_trace = function (mod) {
		if (is_debug(mod, $debug_level_trace)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_bug = function (mod) {
		if (is_debug(mod, $debug_level_bug)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_io = function (mod) {
		if (is_debug(mod, $debug_level_io)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_st = function (mod) {
		if (is_debug(mod, $debug_level_st)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_test = function (mod) {
		if (is_debug(mod, $debug_level_test)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.$Debugger = function (mod) {
		this.mod = mod;
	};

	global.$Debugger.prototype = {
		debug: function (level) {
			print('debug init 1');
			if (is_debug(this.mod, level)) {
				print('debug init 2');
				var s = call(arguments).slice(1).toString();
				print('debug init:' + s);
				__my__.debug(call(arguments).slice(1).toString());
			}
		},

		init: function () {
			if (is_debug(this.mod, $debug_level_init)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		fini: function () {
			if (is_debug(this.mod, $debug_level_fini)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		constructor: function () {
			if (is_debug(this.mod, $debug_level_constructor)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		destructor: function () {
			if (is_debug(this.mod, $debug_level_destructor)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		ok: function () {
			if (is_debug(this.mod, $debug_level_ok)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		error: function () {
			if (is_debug(this.mod, $debug_level_error)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		trace: function () {
			if (is_debug(this.mod, $debug_level_trace)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		bug: function () {
			if (is_debug(this.mod, $debug_level_bug)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		io: function () {
			if (is_debug(this.mod, $debug_level_io)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		st: function () {
			if (is_debug(this.mod, $debug_level_st)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		test: function () {
			if (is_debug(this.mod, $debug_level_test)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		}
	};
}(this));

// __js__
(function () {
	__js__.destructor = function (is_class, x, close) {
		if (typeof close === 'function') {
			__js__.fin(x, function (obj, heapDestruct) {
				var name;
				if (is_class) {
					name = obj.$name || obj.prototype.$name || x.$name || typeof x;
				} else {
					name = obj.$name || obj.prototype.$name || typeof obj;
				}

				if (is_class && obj === x) {
			        debug_destructor('all', name, 'skip, called for the prototype itself');
			    } else if (heapDestruct) {
					close(obj);

					debug_destructor('all', name, 'closed @fini');
			    } else {
					close(obj);

					debug_destructor('all', name, 'closed @destructor');
				}
			});
		} else {
			return __js__.fin(x);
		}
	};
}());

// Object
(function () {
	var pt = Object.prototype;

	pt.setPrototypeOf = pt.setPrototypeOf || function(obj, proto) {
	  obj.__proto__ = proto;
	  return obj;
	};
}());

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

// TypedArray
(function () {
	function bits_mask (obj, bit) {
		return 1<<(bit%(8*obj.BYTES_PER_ELEMENT));
	}

	function bits_elt (obj, bit) {
		return Math.floor(bit/(8*obj.BYTES_PER_ELEMENT));
	}

	function bits_set (obj, bit) {
		obj[bits_elt(obj, bit)] |= bits_mask(obj, bit);
	}

	function bits_clr (obj, bit) {
		obj[bits_elt(obj, bit)] &= ~bits_mask(obj, bit);
	}

	function bits_isset (obj, bit) {
		return 0 != (obj[bits_elt(obj, bit)] & bits_mask(obj, bit));
	}

	var typearrays = [Uint8Array, Uint16Array, Uint32Array];
	var i, func, count = typearrays.length;

	for (i=0; i<count; i++) {
		func = typearrays[i];

		func.prototype.BITS_PER_ELEMENT = 8*func.BYTES_PER_ELEMENT;
		func.prototype.bits_set = function (bit) {
			bits_set(this, bit);
		};
		func.prototype.bits_clr = function (bit) {
			bits_clr(this, bit);
		};
		func.prototype.bits_isset = function (bit) {
			return bits_isset(this, bit);
		};
	}
}());

/* eof */
