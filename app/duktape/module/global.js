/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/
const __js__ = Duktape;

__js__.classFinalizer = function (prototype, close) {
	if (close) {
		__js__.fin(prototype, function (obj, heapDestruct) {
			var name;
			
			if (obj.name) {
				name = obj.name;
				print('name is obj.name');
			} else if (obj.prototype.name) {
				name = obj.prototype.name;
				print('name is obj.prototype.name');
			} else if (prototype.name) {
				name = prototype.name;
				print('name is prototype.name');
			} else {
				name = typeof prototype;
				print('name is typeof prototype');
			}
			// var name = prototype.name || obj.name || obj.prototype.name || typeof prototype;
			print('name =', name);
			
			if (heapDestruct) {
				for (var i=0; i<100; i++)
					print('heapDestruct');
				
				close(obj);

				debug_destructor(name, 'closed @fini');
			} else if (obj === mod.Stream.prototype) {
				for (var i=0; i<100; i++)
					print('obj === mod.Stream.prototype');
				
		        debug_destructor(name, 'called for the prototype itself');
		    } else {
				for (var i=0; i<100; i++)
		    		print('else');
				close(obj);
				
				debug_destructor(name, 'closed @destructor');
			}
		});
	} else {
		return __js__.fin(prototype);
	}
};

__js__.objFinalizer = function (x, close) {
	if (close) {
		__js__.fin(x, function (obj, heapDestruct) {
			var when = heapDestruct?'fini':'destructor';
			var name = obj.name || obj.prototype.name || typeof obj;

			debug_destructor(name, 'closed when', when);
			
			close(obj);
		});
	} else {
		return __js__.fin(x);
	}
};

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

const debug = function (level) {
	__my__.debug(level, Array.prototype.slice.call(arguments).slice(1).toString());
}

const debug_init = function () {
	__my__.debug(__debug__.init, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_fini = function () {
	__my__.debug(__debug__.fini, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_constructor = function () {
	__my__.debug(__debug__.constructor, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_destructor = function () {
	__my__.debug(__debug__.destructor, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_ok = function () {
	__my__.debug(__debug__.ok, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_error = function () {
	__my__.debug(__debug__.error, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_trace = function () {
	__my__.debug(__debug__.trace, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_bug = function () {
	__my__.debug(__debug__.bug, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_mod = function () {
	__my__.debug(__debug__.mod, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_io = function () {
	__my__.debug(__debug__.io, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_st = function () {
	__my__.debug(__debug__.st, Array.prototype.slice.call(arguments).slice(0).toString());
}

const debug_test = function () {
	__my__.debug(__debug__.test, Array.prototype.slice.call(arguments).slice(0).toString());
}

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

/* keep it last */
