#!/bin/bash

CERT_COUNT=${CERT_COUNT:-32}
CERT_SERVER_DOMAIN=${CERT_SERVER_DOMAIN:-superwalle.com}
CERT_CLIENT_DOMAIN=${CERT_CLIENT_DOMAIN:-weos.com}
CERT_DAYS=${CERT_DAYS:-7300}
CERT_BITS=${CERT_BITS:-8192}

do_help() {
        echo "$0 begin [end]"
}

prepare() {
	rm -fr demoCA
	mkdir -p ./demoCA/newcerts 
	touch demoCA/index.txt 
	touch demoCA/serial 
	echo 01 > demoCA/serial
}

cert() {
	local i="$1"

	openssl genrsa -out ca${i}.key ${CERT_BITS}
	openssl req -x509 -new -nodes -key ca${i}.key -subj "/CN=*.${CERT_SERVER_DOMAIN}" -days ${CERT_DAYS} -out ca${i}.crt
	openssl genrsa -out server${i}.key ${CERT_BITS}
	openssl req -new -key server${i}.key -subj "/CN=*.${CERT_SERVER_DOMAIN}" -out server${i}.csr
	openssl x509 -req -in server${i}.csr -CA ca${i}.crt -CAkey ca${i}.key -CAcreateserial -out server${i}.crt -days ${CERT_DAYS}

	openssl genrsa -out client${i}.key ${CERT_BITS}
	openssl req -new -key client${i}.key -subj "/CN=*.${CERT_CLIENT_DOMAIN}" -out client${i}.csr
#	openssl x509 -req -in client${i}.csr -CA ca${i}.crt -CAkey ca${i}.key -CAcreateserial -out client${i}.crt -days 5000
	echo extendedKeyUsage=clientAuth > client${i}.ext
	openssl x509 -req -in client${i}.csr -CA ca${i}.crt -CAkey ca${i}.key \
		-CAcreateserial -extfile client${i}.ext -out client${i}.crt -days ${CERT_DAYS}
}

cert_all() {
	local begin="$1"
	local end="$2"
	local i

	for ((i=begin; i<end; i++)); do
		cert ${i}
	done	
}

b64() {
	local file="$1"

	cat ${file} | base64 > ${file}.base64
}

b64_all() {
	local begin="$1"
	local end="$2"
	local i

	for ((i=begin; i<end; i++)); do
		b64 ca${i}.key
		b64 ca${i}.crt
		b64 server${i}.key
        	b64 server${i}.crt
		b64 client${i}.key
        	b64 client${i}.crt 
	done
}

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
        local begin="$1"
        local end="$2"
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
	echo "#define __THIS_CERT_INITER(_idx)    [_idx] = __OS_CERT_INITER(OS_CERT_client_crt##_idx, OS_CERT_client_key##_idx, OS_CERT_servier_crt##_idx)" >> ${finiter}
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
	local begin="$1"
	local end="$2"; end=${end:=$((begin+1))}
	local args=$#
	local i

        if ((2!=args && 1!=args)); then
		do_help
		return 1
	elif ((begin<0 || end<=begin || end > CERT_COUNT)); then
		do_help
		return 1
	fi

	cert_all ${begin} ${end}
	b64_all ${begin} ${end}
	c_all ${begin} ${end}
	inc_all
	initer
}

main "$@"
