#!/bin/bash

seq 32 | xargs -I{} curl -s http://localhost:44444/ | grep "PORT" |
sed "s/<\/\?[a-z]\+>//g" | sed "s/^[[:space:]]*//" | sort | uniq -c

