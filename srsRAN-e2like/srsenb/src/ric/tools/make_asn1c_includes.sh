#!/bin/bash

#
# Lifted from the OAI codebase and heavily modified.
#

set -x

GENERATED_FULL_DIR=$1
shift
ASN1_SOURCE_FILES="$1"
shift
export ASN1C_PREFIX=$1
shift
options=$*
done_flag="$GENERATED_FULL_DIR"/done
newer=0
if [ -e "$done_flag" ]; then
    for sf in $ASN1_SOURCE_FILES ; do
	if [ "$done_flag" -ot $sf ] ; then
	    newer=1
	    break
	fi
    done
else
    newer=1
fi
if [ $newer -eq 1 ]; then
   rm -f "$GENERATED_FULL_DIR"/${ASN1C_PREFIX}*.c "$GENERATED_FULL_DIR"/${ASN1C_PREFIX}*.h
   mkdir -p "$GENERATED_FULL_DIR"
   asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example $options -D $GENERATED_FULL_DIR $ASN1_SOURCE_FILES |& egrep -v "^Copied|^Compiled" | sort -u
fi
touch $done_flag
