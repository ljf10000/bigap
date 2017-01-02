#!/bin/bash

. ./include.in

GOPACKAGE=main
GOTAG=buildin
GOFILE=${GOTAG}.go
GOTYPE=BuildinCert

GOCERT=${GOTAG}Cert
GOKEY=${GOTAG}Key
GOCA=${GOTAG}Ca

GOVAR=${GOTAG}Certs
GOCOUNT=certCount

gf() {
	echo "$@" >> ${GOFILE}
}

tab() {
	local n="$1"
	local i

	for ((i=0; i<n; i++)) {
		echo -n "    "
	}
}

g() {
	local tag="$1"
	local service="$2"
        local i="$3"
        local type="$4"
        local name=${service}${i}.${type}
        local line
	
	gf "$(tab 1)${tag}${i} = \"\" +"
        while read line; do
        	gf "$(tab 2)\"${line}\" +" 
        done < ${name}.base64
	gf "$(tab 2)\"\""
}

g_all() {
	>${GOFILE}

	gf "package ${GOPACKAGE}"
	gf ""
	gf "import ("
	gf "$(tab 1). \"asdf\""
	gf ")"
	gf ""
	gf "/* !!!!! AUTO create by bigap, NOT edit it !!!!! */"
	gf ""
	gf "const ("
	for ((i=begin; i<end; i++)); do
		g ${GOCERT} server $i crt
		g ${GOKEY}  server $i key
		g ${GOCA}   ca     $i crt
	done
	gf ")"
	gf ""

	gf "var ${GOVAR} = [${GOCOUNT}]${GOTYPE}{"
	for ((i=begin; i<end; i++)); do
		gf "$(tab 1)${i}: ${GOTYPE}{"
		gf "$(tab 2)Cert: ${GOCERT}${i},"
		gf "$(tab 2)Key:  ${GOKEY}${i},"
		gf "$(tab 2)Ca:   ${GOCA}${i},"
		gf "$(tab 1)},"
	done
	gf "}"
}

main() {
        try_help "$@"
        g_all "$@" 
}

main "$@"
