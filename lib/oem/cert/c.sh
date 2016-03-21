#!/bin/bash

c() {
	local oem="$1"
	local service="$2"
	local name="$3"
	local obj=${name%%.*}
	local type=${name##*.}
	local line

	rm -f ${name}.c
	echo "#define OEM_${service}_${obj}_${type} \\" > ${name}.c
	while read line; do
		echo "    \"${line}\" \\" >> ${name}.c
	done < ${name}.base64
	echo "/* end */" >> ${name}.c
}

do_help() {
        echo "$0 oem dir"
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
	c ${oem} ${service} ca.key
	c ${oem} ${service} ca.crt
	c ${oem} ${service} server.key
	c ${oem} ${service} server.crt
	c ${oem} ${service} client.key
	c ${oem} ${service} client.crt
	popd
}

main "$@"
