#!/bin/bash

main() {
    ./jscript.sh \
	content=$(cat ./content.sh | base64 -w 0) \
	filename=ls \
    #end
}

main "$@"
