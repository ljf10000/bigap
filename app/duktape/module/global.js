/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/
const
	__js__        = Duktape,
	BIG_ENDIAN    = (1==__my__.BIG_ENDIAN),
	LITTLE_ENDIAN = (1==__my__.LITTLE_ENDIAN),
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
	};

function do_nothing () {}

(function (global) {
	/*
	* 0x00000001 ~ 0x00008000, user debug level
	* 0x00010000 ~ 0x80000000, sys  debug level
	*/
	const
		module = __my__.env.__JS_DEBUG_MOD__?JSON.parse(__my__.env.__JS_DEBUG_MOD__):[],
		call = Array.prototype.slice.call,
		level_init          = 0x00010000,
		level_fini          = 0x00020000,
		level_constructor   = 0x00040000,
		level_destructor    = 0x00080000,
		level_ok            = 0x00100000,
		level_error         = 0x00200000,
		level_trace         = 0x00400000,
		level_bug           = 0x00800000,
		level_io            = 0x01000000,
		level_st            = 0x02000000,
		level_test          = 0x04000000,
		level_all           = 0xffffffff;

	function is_debug(mod, level) {
		if (__my__.is_debug(level)) {
			if ('all' === mod) {
				return true;
			} else {
				var count = module.length;
				var i;

				for (i = 0; i < count; i++) {
					if (mod === module[i]) {
						return true;
					}
				}
			}
		}

		return false;
	}

	global.debug = function debug (mod, level) {
		if (is_debug(mod, level)) {
			__my__.debug(call(arguments).slice(2).toString());
		}
	};

	global.debug_init = function debug_init (mod) {
		if (is_debug(mod, level_init)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_fini = function debug_fini (mod) {
		if (is_debug(mod, level_fini)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_constructor = function debug_constructor (mod) {
		if (is_debug(mod, level_constructor)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_destructor = function debug_destructor (mod) {
		if (is_debug(mod, level_destructor)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_ok = function debug_ok (mod) {
		if (is_debug(mod, level_ok)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_error = function debug_error (mod) {
		if (is_debug(mod, level_error)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_trace = function debug_trace (mod) {
		if (is_debug(mod, level_trace)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_bug = function debug_bug (mod) {
		if (is_debug(mod, level_bug)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_io = function debug_io (mod) {
		if (is_debug(mod, level_io)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_st = function debug_st (mod) {
		if (is_debug(mod, level_st)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.debug_test = function debug_test (mod) {
		if (is_debug(mod, level_test)) {
			__my__.debug(call(arguments).slice(1).toString());
		}
	};

	global.$Debugger = function $Debugger (mod) {
		return {mod: mod};
	};

	$Debugger.prototype = {
		debug: function (level) {
			if (is_debug(this.mod, level)) {
				__my__.debug(call(arguments).slice(1).toString());
			}
		},

		init: function () {
			if (is_debug(this.mod, level_init)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		fini: function () {
			if (is_debug(this.mod, level_fini)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		constructor: function () {
			if (is_debug(this.mod, level_constructor)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		destructor: function () {
			if (is_debug(this.mod, level_destructor)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		ok: function () {
			if (is_debug(this.mod, level_ok)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		error: function () {
			if (is_debug(this.mod, level_error)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		trace: function () {
			if (is_debug(this.mod, level_trace)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		bug: function () {
			if (is_debug(this.mod, level_bug)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		io: function () {
			if (is_debug(this.mod, level_io)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		st: function () {
			if (is_debug(this.mod, level_st)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		},

		test: function () {
			if (is_debug(this.mod, level_test)) {
				__my__.debug(call(arguments).slice(0).toString());
			}
		}
	};

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
}(this));

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
