/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/

function debug(level) {
	__my__.debug(level, Array.prototype.slice.call(arguments).slice(1).toString());
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