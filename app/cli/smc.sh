#!/bin/bash

./cli
./cli smc

for ((i=0; i<500; i++)); do 
	./cli smc insert normal sb$i "sleep 10; echo smc sb$i"
	echo "smc insert sb$i error:$?"
done

busybox watch -n 1 "./cli smc show"

