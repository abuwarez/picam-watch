#!/bin/bash

[ $# -eq 1 ] || exit 1

for rec in $(ls /data/recs | grep -E "^[0-9]{6}x*$") 
do
	rec_no=$(echo $rec | sed 's/x//')
	echo "$rec $rec_no"
	[ "$rec_no" -lt "$1" ] && [ -f "/data/recs/$rec" ] && echo $rec && rm -f "/data/recs/$rec"
done

