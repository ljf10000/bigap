#!/bin/bash

. ./include.in

GOFILE=buildin.go

gf() {
	echo "$@" >> ${GOFILE}
}

ipriv() {
        local idx="$1"

        echo $((idx-16))
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
	local idx=$(ipriv ${i})
        local line
	
	gf "$(tab 1)${tag}${idx} = \"\" +"
        while read line; do
        	gf "$(tab 2)\"${line}\" +" 
        done < ${name}.base64
	gf "$(tab 2)\"\""
}

g_all() {
	local begin="$1"
	local end="$2"
	local idx

	>${GOFILE}

	gf "package hello"
	gf ""
	gf "/* !!!!! AUTO create by bigap, NOT edit it !!!!! */"
	gf ""
	gf "const ("
	for ((i=begin; i<end; i++)); do
		g privateCert server $i crt
		g privateKey  server $i key
		g privateCa   ca     $i crt
	done
	gf ")"

	gf "var privateCerts = [certCount]helloCert{"
	for ((i=begin; i<end; i++)); do
		idx=$(ipriv $i)
		gf "$(tab 1)${idx}: helloCert{"
		gf "$(tab 2)cert: privateCert${idx},"
		gf "$(tab 2)key:  privateKey${idx},"
		gf "$(tab 2)ca:   privateCa${idx},"
		gf "$(tab 1)},"
	done
	gf "}"
}

main() {
        local begin="$1"
        local end="$2"; end=${end:=$((begin+1))}

	if ((begin<16)); then
		do_help

		exit 1
	fi

        try_help "$@"

        g_all ${begin} ${end}
}

main "$@"
