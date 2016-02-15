#!/bin/bash

main() {
	echo "stdout"
	echo "stderr" > 2

}

main "$@"
