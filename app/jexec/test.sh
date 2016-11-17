#!/bin/bash

declare shell

jargument() {
	local argc="$#"
	local i json

	((0==argc)) && return 0
	argc=$((argc-1))

	json='{"argument":['
	for ((i=0; i<argc; i++)); do
		json+="\"$1\","; shift
	done
	json+="\"$1\"]}"

	echo "$json"
}

jmerge() {
	local a="$1"; shift
	local b="$*"

	if [[ -z "${a}" ]]; then
		echo "${b}"
	elif [[ -z "${b}" ]]; then
		echo "${a}"
	else
		echo "${a}" | jq -j -c ". * ${b}"
	fi
}

do_exec() {
	local json="$1"; shift

	json=$(jmerge "${json}" "$(jargument $@)")
	
	echo "exec json=${json}"

	$(pwd)/jexec "${json}"
}

do_file() {
	local file=$(pwd)/execfile.${shell}
	local json=$(printf '{"filename":"%s"}' "${file}")

	echo "do file"	
	echo "file=${file}"
	echo "json=${json}"

	do_exec "${json}" "$@"
}

do_content() {
	local file=$(pwd)/${shell}.content
	local decode=$(cat ${file} | base64 -w 0)
	local json=$(printf '{"content":"%s"}' "${decode}")

	echo "do content"
	echo "file=${file}"
	echo "decode=${decode}"
        echo "json=${json}"

	do_exec "${json}" "$@"
}

main() {
	local type="$1"; shift

	case ${type} in
	file|content)
		;;
	*)
		echo "$0 file|content"

		return 1 
		;;
	esac

	case ${SHELL} in
	/bin/js)
		shell=js
		;;
	/bin/bash|*)
		shell=sh
		;;
	esac

	echo "shell=${shell}"

	do_${type} "$@"
}

main "$@"
