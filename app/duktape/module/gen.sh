#!/bin/bash

js2code() {
	local list="$@"
	local name

	for name in ${list}; do
		echo "#define duk_${name}_CODE \\"> ${name}.code
		
		while read line; do 
			echo "\"${line}\\n\" \\" >> ${name}.code
		done < ${name}.js
		
		echo "/* end */" >> ${name}.code
	done
}

main() {
	js2code global
}

main "$@"
