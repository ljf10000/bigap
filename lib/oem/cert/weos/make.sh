#!/bin/bash

. ./include.in

main() {
	local begin="$1"
	local end="$2"; end=${end:=$((begin+1))}

	try_help "$@"

	./cert.sh "$@"
	./base64.sh "$@"
	./c.sh "$@"
	./go.sh "$@"
}

main "$@"
