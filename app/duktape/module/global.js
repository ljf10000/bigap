/* 
* 1. can not use require in this file 
* 2. only use 'xxxxx'
* 3. must keep last empty line
*/

function debug(level) {
	var newa = Array.prototype.slice.call(arguments);

	__my__.debug(level, newa.slice(1).toString());
}

var fmt = (function (){
	this.oprint = function (name, obj) {
		var c;
	
		c[name] = obj;
	
		print(Duktape.enc('jc',c, null, 4));
	};
	
	this.separator = function (name, sep) {
		var s = sep?sep:'==========';
	
		print(s, name, s);
	};
}());

/* keep it last */