#!/bin/bash

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
	local var="$1"
	local file="$2"
        local line
	
	gf "$(tab 1)${var} = \"\" +"
        while read line; do
        	gf "$(tab 2)\"${line}\" +" 
        done < ${file}
	gf "$(tab 2)\"\""
}

g_all() {
	>${GOFILE}

	gf "package main"
	gf ""
        gf "import ("
        gf "$(tab 1). \"asdf\""
        gf ")"
	gf ""
	gf "/* !!!!! AUTO create by bigap, NOT edit it !!!!! */"
	gf ""
	gf "const ("
	g ${service}BuildinCert server.crt.base64
	g ${service}BuildinKey  server.key.base64
	g ${service}BuildinCa   ca.crt.base64
	gf ")"
	gf ""

	gf "var ${service}Cert = &BuildinCert{"
	gf "$(tab 1)Ca:   ${service}BuildinCa,"
	gf "$(tab 1)Key:  ${service}BuildinKey,"
	gf "$(tab 1)Cert: ${service}BuildinCert,"
	gf "}"
}

do_help() {
	echo "$0 oem service"
}

main() {
        oem="$1"
        service="$2"

        if ((2!=$#)); then
                do_help
                return 1
        elif [[ ! -d ${oem}/${service} ]]; then
                do_help
                return 1
        fi

	GOFILE=${service}.go

        pushd ${oem}/${service}
	g_all
        popd
}

main "$@"
