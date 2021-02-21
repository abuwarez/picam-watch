#!/bin/bash
if [[ ! "$1" =~ ^[0-9]+$ ]]; then
       	echo "ugh $1"
	exit 1
fi

IDX=$1

while :; do
	printf -v IN_FILE "%06d" $IDX
	[ ! -f $IF_FILE ] && IN_FILE="${IN_FILE}x"

	echo "Processing $IN_FILE ..."
	if [[ -f $IN_FILE ]]; then
		NEXT_FRAME_OFFSET_HEX=$(hexdump -C $IN_FILE | grep -m 1 "00 00 00 01 27 64 00 28  ac 2b 40 5a 1e d0 0f 12" | awk '{print $1}')
		NEXT_FRAME_OFFSET=$((16#$NEXT_FRAME_OFFSET_HEX))
		echo "  NFOH $NEXT_FRAME_OFFSET_HEX ($NEXT_FRAME_OFFSET)"
		if [[ "$NEXT_FRAME_OFFSET_HEX" =~ ^[0-9a-fA-F]+$ ]]; then
			if [[ "$NEXT_FRAME_OFFSET" -gt 0 ]]; then
				if [[ ! -z "$CRT_OUT_FILE" ]]; then
					# append data before offet to current output file
					echo "  copying $NEXT_FRAME_OFFSET bytes to $CRT_OUT_FILE"
					dd bs=$NEXT_FRAME_OFFSET count=1 if=$IN_FILE >> $CRT_OUT_FILE || exit 1
				fi

				CRT_OUT_FILE="t${IN_FILE}.h264"
				echo "  copying from $NEXT_FRAME_OFFSET bytes to $CRT_OUT_FILE"
				dd bs=$NEXT_FRAME_OFFSET skip=1 if=$IN_FILE >> $CRT_OUT_FILE || exit 1

			else
				CRT_OUT_FILE="t${IN_FILE}.h264"
				echo "  copying full to $CRT_OUT_FILE (zero offset)"
				cp $IN_FILE $CRT_OUT_FILE

			fi

		else
			if [[ ! -z $CRT_OUT_FILE ]]; then
				# no new frame offset found, fully copying current in file to current out file
				echo "  copyind full to $CRT_OUT_FILE (no offset)"
				dd if=$IN_FILE >> $CRT_OUT_FILE || exit 1
			else
				echo "  probably invalid. skipping"
			fi

		fi
	else
		echo "No such file $IN_FILE. Stopping"
		break

	fi
	let 'IDX=IDX+1'
done
		

