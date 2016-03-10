function printo (name, obj) {
	for (var p in obj) {
	        print(name + "." + p + " = " + obj[p]);
	}
}

function debugger(level) {
	__my__.debugger(level, Array.prototype.slice.call(arguments).slice(1).toString());
}
