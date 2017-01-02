#!/bin/bash

. ./include.in

main() {
	try_help "$@"

	./cert.sh "$@"
	./base64.sh "$@"
	./c.sh "$@"
	./go.sh "$@"
}

main "$@"
