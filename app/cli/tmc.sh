#!/bin/bash

./cli
./cli tmc

for ((i=0; i<5000; i++)); do 
	./cli tmc insert sb$i 0 3 0 "echo tmc sb$i"
	echo "tmc insert sb$i error:$?"
done

busybox watch -n 1 "./cli tmc show"

