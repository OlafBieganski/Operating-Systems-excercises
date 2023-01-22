#!/bin/bash
find -name "*.$1" -mtime -$2 -perm -444 -exec tar -rvf $3.tar {} \;