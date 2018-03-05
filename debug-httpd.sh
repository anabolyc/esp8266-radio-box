#!/bin/bash

docker run --rm -ti --name lighttpd-instance \
    -v ${PWD}/files:/var/www/localhost \
    -p 8080:8080 \
    arulrajnet/lighttpd:latest