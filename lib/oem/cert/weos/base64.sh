#!/bin/bash

. ./include.in

b64() {
	local file="$1"

	cat ${file} | base64 > ${file}.base64
}

b64_all() {
	local begin="$1"
	local end="$2"
	local i

	for ((i=begin; i<end; i++)); do
		b64 ca${i}.key
		b64 ca${i}.crt
		b64 server${i}.key
        	b64 server${i}.crt
		b64 client${i}.key
        	b64 client${i}.crt 
	done
}

main() {
	local begin="$1"
	local end="$2"; end=${end:=$((begin+1))}

	try_help "$@"

	b64_all ${begin} ${end}
}

main "$@"
