/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/
const __js__ = Duktape;

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

function do_nothing () {}

(function (global) {
	/*
	* 0x00000001 ~ 0x00008000, user debug level
	* 0x00010000 ~ 0x80000000, sys  debug level
	*/
	const
		debug_module = __my__.env.__JS_DEBUG_MOD__?JSON.parse(__my__.env.__JS_DEBUG_MOD__):[],
		slice = Array.prototype.slice,
		debug_level_init        = 0x00010000,
		debug_level_fini        = 0x00020000,
		debug_level_constructor = 0x00040000,
		debug_level_destructor  = 0x00080000,
		debug_level_ok          = 0x00100000,
		debug_level_error       = 0x00200000,
		debug_level_trace       = 0x00400000,
		debug_level_bug         = 0x00800000,
		debug_level_io          = 0x01000000,
		debug_level_st          = 0x02000000,
		debug_level_test        = 0x04000000,
		debug_level_all         = 0xffffffff;

	function is_debug(mod, level) {
		if (__my__.is_debug(level)) {
			if ('all' === mod) {
				return true;
			} else {
				var count = debug_module.length;
				var i;

				for (i = 0; i < count; i++) {
					if (mod === debug_module[i]) {
						return true;
					}
				}
			}
		}

		return false;
	}

	global.debug = function debug (mod, level) {
		if (is_debug(mod, level)) {
			__my__.debug(slice.call(arguments).slice(2).toString());
		}
	};

	global.debug_init = function debug_init (mod) {
		if (is_debug(mod, debug_level_init)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_fini = function debug_fini (mod) {
		if (is_debug(mod, debug_level_fini)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_constructor = function debug_constructor (mod) {
		if (is_debug(mod, debug_level_constructor)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_destructor = function debug_destructor (mod) {
		if (is_debug(mod, debug_level_destructor)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_ok = function debug_ok (mod) {
		if (is_debug(mod, debug_level_ok)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_error = function debug_error (mod) {
		if (is_debug(mod, debug_level_error)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_trace = function debug_trace (mod) {
		if (is_debug(mod, debug_level_trace)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_bug = function debug_bug (mod) {
		if (is_debug(mod, debug_level_bug)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_io = function debug_io (mod) {
		if (is_debug(mod, debug_level_io)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_st = function debug_st (mod) {
		if (is_debug(mod, debug_level_st)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.debug_test = function debug_test (mod) {
		if (is_debug(mod, debug_level_test)) {
			__my__.debug(slice.call(arguments).slice(1).toString());
		}
	};

	global.$Debugger = function $Debugger (mod) {
		return {mod: mod};
	};

	$Debugger.prototype = {
		debug: function (level) {
			if (is_debug(this.mod, level)) {
				__my__.debug(slice.call(arguments).slice(1).toString());
			}
		},

		init: function () {
			if (is_debug(this.mod, debug_level_init)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		fini: function () {
			if (is_debug(this.mod, debug_level_fini)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		constructor: function () {
			if (is_debug(this.mod, debug_level_constructor)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		destructor: function () {
			if (is_debug(this.mod, debug_level_destructor)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		ok: function () {
			if (is_debug(this.mod, debug_level_ok)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		error: function () {
			if (is_debug(this.mod, debug_level_error)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		trace: function () {
			if (is_debug(this.mod, debug_level_trace)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		bug: function () {
			if (is_debug(this.mod, debug_level_bug)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		io: function () {
			if (is_debug(this.mod, debug_level_io)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		st: function () {
			if (is_debug(this.mod, debug_level_st)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		},

		test: function () {
			if (is_debug(this.mod, debug_level_test)) {
				__my__.debug(slice.call(arguments).slice(0).toString());
			}
		}
	};

	__js__.destructor = function (is_class, x, close) {
		if (typeof close === 'function') {
			__js__.fin(x, function (obj, heapDestruct) {
				var name;
				if (is_class) {
					name = obj.$name || obj.prototype.$name || x.name || typeof x;
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
	var i, pt, count = typearrays.length;

	for (i=0; i<count; i++) {
		pt = typearrays[i].prototype;

		pt.BITS_PER_ELEMENT = 8*pt.BYTES_PER_ELEMENT;
		pt.bits_set = function (bit) {
			bits_set(this, bit);
		};
		pt.bits_clr = function (bit) {
			bits_clr(this, bit);
		};
		pt.bits_isset = function (bit) {
			return bits_isset(this, bit);
		};
	}
}());

/* eof */
