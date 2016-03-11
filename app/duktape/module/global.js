/* can not use require in this file */

function debug(level) {
	var newa = Array.prototype.slice.call(arguments);

	__my__.debug(level, newa.slice(1).toString());
}

var fmt = {
	oprint: function (name, obj) {
		var c;
	
		c[name] = obj;
	
		print(Duktape.enc('jc',c, null, 4));
	},
	
	separator: function (name, sep) {
		var s = sep?sep:.==========';
	
		print(s, name, s);
	}
};