#define duk_global_CODE \
"/*\n" \
"* 1. can not use require in this file\n" \
"* 2. only use 'xxxxx'\n" \
"* 3. must keep last empty line\n" \
"*/\n" \
"function do_nothing() {}\n" \
"function no_support() { return -__LIBC__.ENOSUPPORT; }\n" \
"\n" \
"function must(obj, type) { return typeof obj === type;}\n" \
"function must_object(obj) { return obj && must(obj, 'object'); }\n" \
"function must_function(obj) { return must(obj, 'function'); }\n" \
"function must_string(obj) { return must(obj, 'string'); }\n" \
"function must_number(obj) { return must(obj, 'number'); }\n" \
"function must_bool(obj) { return must(obj, 'boolean'); }\n" \
"\n" \
"function maybe(obj, type) { return typeof obj === type ? obj : undefined;}\n" \
"function maybe_object(obj) { return (obj && typeof obj === 'object') ? obj : undefined; }\n" \
"function maybe_function(obj) { return maybe(obj, 'function'); }\n" \
"function maybe_string(obj) { return maybe(obj, 'string'); }\n" \
"function maybe_number(obj) { return maybe(obj, 'number'); }\n" \
"function maybe_bool(obj) { return maybe(obj, 'boolean'); }\n" \
"\n" \
"function allways(obj) { return obj; }\n" \
"function allways_string(obj) { return obj.toString(); }\n" \
"function allways_null() { return null; }\n" \
"function allways_undefined() { return undefined; }\n" \
"function allways_false() { return false; }\n" \
"function allways_true() { return true; }\n" \
"function allways_empty() { return {}; }\n" \
"\n" \
"function safe_function(f, safe) {\n" \
"return maybe_function(f) || maybe_function(safe) || no_support;\n" \
"}\n" \
"\n" \
"function safe_string(s, safe) {\n" \
"return maybe_string(s) || maybe_string(safe) || UNKNOW_STRING;\n" \
"}\n" \
"\n" \
"function safe_number(n, safe) {\n" \
"return maybe_number(n) || maybe_number(safe) || UNKNOW_NUMBER;\n" \
"}\n" \
"\n" \
"const\n" \
"__js__        = Duktape,\n" \
"BIG_ENDIAN    = (1===__my__.BIG_ENDIAN),\n" \
"LITTLE_ENDIAN = (1===__my__.LITTLE_ENDIAN),\n" \
"OS_BITS       = 8*__my__.SIZEOF_POINTER,\n" \
"UNKNOW_STRING = 'unknow',\n" \
"UNKNOW_NUMBER = 0,\n" \
"fmt = {\n" \
"oprint: function(name, obj) {\n" \
"if (must_string(name) && must_object(obj)) {\n" \
"var root = {};\n" \
"\n" \
"root[name] = obj;\n" \
"\n" \
"print(Duktape.enc('jc', root, null, 4));\n" \
"} else if (must_object(name)) { // not input name, just obj\n" \
"print(Duktape.enc('jc', name, null, 4));\n" \
"} else {\n" \
"print(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"separator: function(name, sep) {\n" \
"var s = maybe_string(sep)?sep:'==========';\n" \
"\n" \
"print(s, name, s);\n" \
"}\n" \
"},\n" \
"$LOG_EMERG            = 0,\n" \
"$LOG_ALERT            = 1,\n" \
"$LOG_CRIT             = 2,\n" \
"$LOG_ERROR            = 3,\n" \
"$LOG_WARNING          = 4,\n" \
"$LOG_NOTICE           = 5,\n" \
"$LOG_INFO             = 6,\n" \
"$LOG_DEBUG            = 7;\n" \
"\n" \
"// Object\n" \
"(function() {\n" \
"do_nothing();\n" \
"}());\n" \
"\n" \
"// Array\n" \
"(function() {\n" \
"var pt = Array.prototype;\n" \
"\n" \
"pt.includes = function(element, fromIndex) {\n" \
"var i, count = this.length;\n" \
"\n" \
"for (i=fromIndex; i<count; i++) {\n" \
"if (element===this[i]) {\n" \
"return true;\n" \
"}\n" \
"}\n" \
"\n" \
"return false;\n" \
"};\n" \
"}());\n" \
"\n" \
"// TypedArray\n" \
"(function() {\n" \
"function bits_mask(obj, bit) {\n" \
"return 1<<(bit%(8*obj.BYTES_PER_ELEMENT));\n" \
"}\n" \
"\n" \
"function bits_elt(obj, bit) {\n" \
"return Math.floor(bit/(8*obj.BYTES_PER_ELEMENT));\n" \
"}\n" \
"\n" \
"function bits_set(obj, bit) {\n" \
"obj[bits_elt(obj, bit)] |= bits_mask(obj, bit);\n" \
"}\n" \
"\n" \
"function bits_clr(obj, bit) {\n" \
"obj[bits_elt(obj, bit)] &= ~bits_mask(obj, bit);\n" \
"}\n" \
"\n" \
"function bits_isset(obj, bit) {\n" \
"return 0 != (obj[bits_elt(obj, bit)] & bits_mask(obj, bit));\n" \
"}\n" \
"\n" \
"var arrays = [Uint8Array, Uint16Array, Uint32Array];\n" \
"var i, pt, count = arrays.length;\n" \
"\n" \
"for (i=0; i<count; i++) {\n" \
"pt = arrays[i].prototype;\n" \
"\n" \
"pt.BITS_PER_ELEMENT = 8*pt.BYTES_PER_ELEMENT;\n" \
"pt.bits_set = function(bit) {\n" \
"bits_set(this, bit);\n" \
"};\n" \
"pt.bits_clr = function(bit) {\n" \
"bits_clr(this, bit);\n" \
"};\n" \
"pt.bits_isset = function(bit) {\n" \
"return bits_isset(this, bit);\n" \
"};\n" \
"}\n" \
"}());\n" \
"\n" \
"// $Debugger\n" \
"(function(global) {\n" \
"/*\n" \
"* 0x00000001 ~ 0x00008000, user debug level\n" \
"* 0x00010000 ~ 0x80000000, sys  debug level\n" \
"*/\n" \
"var modules = __my__.env.__JMOD__?JSON.parse(__my__.env.__JMOD__):[],\n" \
"slice = Array.prototype.slice,\n" \
"pt;\n" \
"\n" \
"function is_debug(mod, level) {\n" \
"var count = modules.length;\n" \
"\n" \
"if (count > 0 && __my__.is_debug(level)) {\n" \
"if ('all' === mod) {\n" \
"return true;\n" \
"} else {\n" \
"var i;\n" \
"\n" \
"for (i = 0; i < count; i++) {\n" \
"if (mod === modules[i]) {\n" \
"return true;\n" \
"}\n" \
"}\n" \
"}\n" \
"}\n" \
"\n" \
"return false;\n" \
"}\n" \
"\n" \
"global.$Debugger = function(mod) {\n" \
"this.mod = function() { return mod; };\n" \
"};\n" \
"\n" \
"global.$Debugger.prototype = pt = {\n" \
"INIT: 0x00010000,\n" \
"FINI: 0x00020000,\n" \
"CONSTRUCTOR: 0x00040000,\n" \
"DESTRUCTOR: 0x00080000,\n" \
"OK: 0x00100000,\n" \
"ERROR: 0x00200000,\n" \
"TRACE: 0x00400000,\n" \
"BUG: 0x00800000,\n" \
"IO: 0x01000000,\n" \
"ST: 0x02000000,\n" \
"TEST: 0x04000000,\n" \
"ALL: 0xffffffff,\n" \
"\n" \
"debug: function(level) {\n" \
"if (is_debug(this.mod(), level)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"init: function() {\n" \
"if (is_debug(this.mod(), pt.INIT)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"fini: function() {\n" \
"if (is_debug(this.mod(), pt.FINI)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"constructor: function() {\n" \
"if (is_debug(this.mod(), pt.CONSTRUCTOR)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"destructor: function() {\n" \
"if (is_debug(this.mod(), pt.DESTRUCTOR)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"ok: function() {\n" \
"if (is_debug(this.mod(), pt.OK)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"error: function() {\n" \
"if (is_debug(this.mod(), pt.ERROR)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"trace: function() {\n" \
"if (is_debug(this.mod(), pt.TRACE)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"bug: function() {\n" \
"if (is_debug(this.mod(), pt.BUG)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"io: function() {\n" \
"if (is_debug(this.mod(), pt.IO)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"st: function() {\n" \
"if (is_debug(this.mod(), pt.ST)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"test: function() {\n" \
"if (is_debug(this.mod(), pt.TEST)) {\n" \
"__my__.debug(slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug = function(mod, level) {\n" \
"if (is_debug(mod, level)) {\n" \
"__my__.debug(slice.call(arguments).slice(2).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_init = function(mod) {\n" \
"if (is_debug(mod, pt.INIT)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_fini = function(mod) {\n" \
"if (is_debug(mod, pt.FINI)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_constructor = function(mod) {\n" \
"if (is_debug(mod, pt.CONSTRUCTOR)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_destructor = function(mod) {\n" \
"if (is_debug(mod, pt.DESTRUCTOR)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_ok = function(mod) {\n" \
"if (is_debug(mod, pt.OK)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_error = function(mod) {\n" \
"if (is_debug(mod, pt.ERROR)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_trace = function(mod) {\n" \
"if (is_debug(mod, pt.TRACE)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_bug = function(mod) {\n" \
"if (is_debug(mod, pt.BUG)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_io = function(mod) {\n" \
"if (is_debug(mod, pt.IO)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_st = function(mod) {\n" \
"if (is_debug(mod, pt.ST)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"global.debug_test = function(mod) {\n" \
"if (is_debug(mod, pt.TEST)) {\n" \
"__my__.debug(slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"}(this));\n" \
"\n" \
"// __js__\n" \
"(function () {\n" \
"__js__.destructor = function(x, close, is_class) {\n" \
"if (typeof close === 'function') {\n" \
"return __js__.fin(x, function(obj, heapDestruct) {\n" \
"var name = maybe_function(obj.$name)\n" \
"|| maybe_function(obj.prototype.$name)\n" \
"|| maybe_function(x.$name)\n" \
"|| function() { return typeof x;},\n" \
"info;\n" \
"\n" \
"if (is_class && obj === x) {\n" \
"info = 'closed skip, called for the prototype itself';\n" \
"} else if (heapDestruct) {\n" \
"close(obj);\n" \
"info = 'closed @fini';\n" \
"} else {\n" \
"close(obj);\n" \
"info = 'closed @destructor';\n" \
"}\n" \
"print(name(), info);\n" \
"debug_destructor('all', name(), info);\n" \
"});\n" \
"} else {\n" \
"return __js__.fin(x);\n" \
"}\n" \
"};\n" \
"}());\n" \
"\n" \
"/* eof */\n" \
/* end */
