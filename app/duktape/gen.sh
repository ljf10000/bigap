#!/bin/bash

js2code() {
	local list="$@"
	local name

	for name in ${list}; do
		echo "#define duk_${name}_CODE \\"> ${name}.h
		
		while read line; do 
			echo "\"${line}\\n\" \\" >> ${name}.h
		done < ${name}.js
		
		echo "/* end */" >> ${name}.h
	done
}

main() {
	pushd module
	js2code global
	popd
}

main "$@"
