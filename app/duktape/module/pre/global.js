function printo (name, obj) {
	for (var p in obj) {
	        print(name + "." + p + " = " + obj[p]);
	}
}

function __debug(level) {
	var newa = Array.prototype.slice.call(arguments);
	print("__debug", newa);
	
	__my__.debugger(level, newa.slice(1).toString());
}
