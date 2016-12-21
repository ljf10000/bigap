#!/bin/bash

./cli
./cli umc
./cli umc show

busybox watch -n 1 "./cli umc show"

