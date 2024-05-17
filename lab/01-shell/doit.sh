#!/bin/bash

FILE=$1
OPERATION=$2

if [[ $OPERATION == "read" ]]; then
    cat "$FILE"
elif [[ $OPERATION == "write" ]]; then
    echo "547 MYFILE" >> "$FILE"
else
    echo "Invalid operation"
    exit 2
fi

exit 0
