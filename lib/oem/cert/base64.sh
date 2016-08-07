#!/bin/bash

b64() {
	local name="$1"

	cat ${name} | base64 > ${name}.base64
}

do_help() {
        echo "$0 oem service"
}

main() {
	local oem="$1"
	local service="$2"

        if ((2!=$#)); then
		do_help
		return 1
	elif [[ ! -d ${oem}/${service} ]]; then
		do_help
		return 1
	fi

	pushd ${oem}/${service}
	b64 ca.key
	b64 ca.crt
	b64 server.key
	b64 server.crt
	b64 client.key
	b64 client.crt
	popd
}

main "$@"
