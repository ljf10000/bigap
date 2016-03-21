#!/bin/bash

main() {
	./c.sh deft lms
	./c.sh deft lss
	./c.sh deft ums

	./c.sh raytight lms
	./c.sh raytight lss
	./c.sh raytight ums
}

main "$@"
