#!/bin/bash

. ./include.in

c() {
	local service="$1"
        local i="$2"
        local type="$3"
        local name=${service}${i}.${type}
        local tag=OS_CERT_${service}_${type}${i}
        local line

        > ${name}.c
        echo "#undef ${tag}" > ${name}.c
        echo "#define ${tag} \\" >> ${name}.c
        while read line; do
                echo "    \"${line}\" \\" >> ${name}.c
        done < ${name}.base64
        echo "/* end */" >> ${name}.c
}

c_all() {
        local i

        for ((i=begin; i<end; i++)); do
#               c ca ${i} crt
#		c ca ${i} key
                c client ${i} crt
                c client ${i} key
		c server ${i} crt
#		c server ${i} key
        done
}

inc() {
	local service="$1"
        local type="$2"
        local file=${service}.${type}.c
        local i

        > ${file}
        for ((i=0; i<CERT_COUNT; i++)); do
                echo "#include \"oem/cert/weos/${service}${i}.${type}.c\"" >> ${file}
        done
}

inc_all() {
#	inc ca crt 
#	inc ca key
	inc client crt
	inc client key
	inc server crt
#	inc server key
}

initer() {
	local finiter=this_cert_initer.c
	local fcookie=fcookie_cert_initer.c
	local i
	
	> ${finiter}
	echo "#define __THIS_CERT_INITER(_idx)    [_idx] = __OS_CERT_INITER(OS_CERT_client_crt##_idx, OS_CERT_client_key##_idx, OS_CERT_server_crt##_idx)" >> ${finiter}
	echo "#include \"oem/cert/weos/client.crt.c\"" >> ${finiter}
	echo "#include \"oem/cert/weos/client.key.c\"" >> ${finiter}
	echo "#include \"oem/cert/weos/server.crt.c\"" >> ${finiter}
	echo "#define __THIS_CERT_INITERS { \\" >> ${finiter}
	for ((i=0; i<CERT_COUNT; i++)); do
		echo "    __THIS_CERT_INITER(${i}), \\" >> ${finiter}
	done
	echo "} /* end */" >> ${finiter}

	> ${fcookie}
	echo "#define FCOOKIE_CERT_INITERS \\" >> ${fcookie}
	for ((i=0; i<CERT_COUNT; i++)); do
		echo "    FCOOKIE_CERT_INITER(${i}), \\" >> ${fcookie}
	done
	echo "/* end */" >> ${fcookie}
}

main() {
	try_help "$@"
	c_all "$@" 
	inc_all
	initer
}

main "$@"
