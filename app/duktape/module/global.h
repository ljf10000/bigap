#define duk_global_CODE \
"/*\n" \
"* 1. can not use require in this file\n" \
"* 2. only use 'xxxxx'\n" \
"* 3. must keep last empty line\n" \
"*/\n" \
"const __js__ = Duktape;\n" \
"\n" \
"const fmt = {\n" \
"oprint: function (name, obj) {\n" \
"var c = {};\n" \
"\n" \
"c[name] = obj;\n" \
"\n" \
"print(Duktape.enc('jc', c, null, 4));\n" \
"},\n" \
"\n" \
"separator: function (name, sep) {\n" \
"var s = sep?sep:'==========';\n" \
"\n" \
"print(s, name, s);\n" \
"}\n" \
"};\n" \
"\n" \
"/*\n" \
"* 0x00000001 ~ 0x00008000, user debug level\n" \
"* 0x00010000 ~ 0x80000000, sys  debug level\n" \
"*/\n" \
"const __debug_level__ = {\n" \
"init:	0x00010000,\n" \
"fini:	0x00020000,\n" \
"constructor:0x00040000,\n" \
"destructor:	0x00080000,\n" \
"ok:		0x00100000,\n" \
"error:	0x00200000,\n" \
"trace:	0x00400000,\n" \
"bug:	0x00800000,\n" \
"io:		0x01000000,\n" \
"st:		0x02000000,\n" \
"test: 	0x04000000,\n" \
"\n" \
"all:	0xffffffff\n" \
"};\n" \
"\n" \
"const __debug_module__ = __my__.env.__JS_DEBUG_MOD__?JSON.parse(__my__.env.__JS_DEBUG_MOD__):[];\n" \
"\n" \
"const __is_debug = function (mod, level) {\n" \
"if (__my__.is_debug(level)) {\n" \
"if ('all' === mod) {\n" \
"return true;\n" \
"} else {\n" \
"var count = __debug_module__.length;\n" \
"var i;\n" \
"\n" \
"for (i=0; i<count; i++) {\n" \
"if (mod === __debug_module__[i]) {\n" \
"return true;\n" \
"}\n" \
"}\n" \
"}\n" \
"}\n" \
"\n" \
"return false;\n" \
"};\n" \
"\n" \
"const debug = function (mod, level) {\n" \
"if (__is_debug(mod, level)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(2).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_init = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.init)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_fini = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.fini)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_constructor = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.constructor)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_destructor = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.destructor)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_ok = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.ok)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_error = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.error)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_trace = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.trace)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_bug = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.bug)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_io = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.io)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_st = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.st)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const debug_test = function (mod) {\n" \
"if (__is_debug(mod, __debug_level__.test)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"};\n" \
"\n" \
"const ModDebugger = function (mod) {\n" \
"this.mod = mod;\n" \
"};\n" \
"\n" \
"ModDebugger.prototype = {\n" \
"debug: function (level) {\n" \
"if (__is_debug(this.mod, level)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(1).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"init: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.init)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"fini: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.fini)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"constructor: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.constructor)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"destructor: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.destructor)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"ok: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.ok)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"error: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.error)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"trace: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.trace)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"bug: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.bug)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"io: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.io)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"st: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.st)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"},\n" \
"\n" \
"test: function () {\n" \
"if (__is_debug(this.mod, __debug_level__.test)) {\n" \
"__my__.debug(Array.prototype.slice.call(arguments).slice(0).toString());\n" \
"}\n" \
"}\n" \
"};\n" \
"\n" \
"__js__.destructor = function (is_class, x, close) {\n" \
"if (typeof close === 'function') {\n" \
"__js__.fin(x, function (obj, heapDestruct) {\n" \
"var name;\n" \
"if (is_class) {\n" \
"name = obj.name || obj.prototype.name || x.name || typeof x;\n" \
"} else {\n" \
"name = obj.name || obj.prototype.name || typeof obj;\n" \
"}\n" \
"\n" \
"if (is_class && obj === x) {\n" \
"debug_destructor('all', name, 'skip, called for the prototype itself');\n" \
"} else if (heapDestruct) {\n" \
"close(obj);\n" \
"\n" \
"debug_destructor('all', name, 'closed @fini');\n" \
"} else {\n" \
"close(obj);\n" \
"\n" \
"debug_destructor('all', name, 'closed @destructor');\n" \
"}\n" \
"});\n" \
"} else {\n" \
"return __js__.fin(x);\n" \
"}\n" \
"};\n" \
"\n" \
"/* eof */\n" \
/* end */
