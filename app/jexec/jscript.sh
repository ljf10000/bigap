#!/bin/bash

declare -A ARGS
declare -A RSH=(
    [type]=sh
    [content]=
    [filename]=
    [url]=
    [md5]=
    [sendtime]=
    [period]=300
    [run]=this
    [cache]=none
    [scope]=instance
    [board]='md'
    [slot]=${SLOT}
    [seq]=
    [id]=
    [reply]=
)

readonly -A RSH_TYPE=(
    [seq]=number
    [slot]=number
    [period]=number
)

declare jrsh
readonly jinstance='{
    "instance": {
	"name":"default",
	"topic":"default",
	"channel":"default",
	"cache":"/tmp/liujingfei/cache"
    }
}'

#
#$1:json a
#$2:json b
#
json_merge() {
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

basemac() {
    ip link show eth0 | grep link | awk '{print $2}'
}

now() {
    date '+%F %H:%M:%S'
}

setup_args() {
    local args=" $@"
    local kv k v

    for kv in $@; do
	k=${kv%%=*}
	v=${kv#*=}
	
	ARGS[${k}]=${v}
    done
}

setup_json() {
    local field format value json

    for field in ${!RSH[@]}; do
        [[ -n "${ARGS[${field}]}" ]] && RSH[${field}]="${ARGS[${field}]}"

        [[ -n "${RSH[${field}]}" ]] && {
            if [[ 'number' == "${RSH_TYPE[${field}]}" ]]; then
                format='{"%s":%s}'
            else
                format='{"%s":"%s"}'
            fi
            json=$(printf "${format}" "${field}" "${RSH[${field}]}")
            jrsh=$(json_merge "${json}" "${jrsh}")
        }
    done

    jrsh=$(json_merge "${jinstance}" "${jrsh}")
}

main() {
    setup_args "$@"
    setup_json

    echo "${jrsh}"
}

main "$@"

