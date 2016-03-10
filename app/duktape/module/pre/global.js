function printo (name, obj) {
	for (var p in obj) {
	        print(name + "." + p + " = " + obj[p]);
	}
}

function debug(level) {
	var newa = Array.prototype.slice.call(arguments);

	__my__.debug(level, newa.slice(1).toString());
}
