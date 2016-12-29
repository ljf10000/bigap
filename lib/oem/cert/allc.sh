#!/bin/bash

main() {
	./c.sh deft hms
	./c.sh deft lss
	./c.sh deft ums

	./c.sh raytight hms
	./c.sh raytight lss
	./c.sh raytight ums
}

main "$@"
