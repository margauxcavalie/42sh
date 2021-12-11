#!/bin/sh

while read LINE
do
    if echo "$LINE" | grep -qx "\s*"
    then
        echo "it is empty"
    elif echo "$LINE" | grep -qx "[a-zA-Z]*"
    then
        echo "it is a word"
    elif echo "$LINE" | grep -qx "[0-9]"
    then
        echo "it is a digit"
    elif echo "$LINE" | grep -qx "[0-9][0-9]*"
    then
        echo "it is a number"
    elif echo "$LINE" | grep -qx "[a-zA-Z0-9]*"
    then
        echo "it is an alphanum"
    else
        echo "it is too complicated"
        exit 0
    fi
done
