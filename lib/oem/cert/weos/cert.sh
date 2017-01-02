#!/bin/bash

. ./include.in

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
	local i

	for ((i=begin; i<end; i++)); do
		cert ${i}
	done	
}

main() {
	try_help "$@"
	cert_all "$@" 
}

main "$@"
