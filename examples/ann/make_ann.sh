#!/bin/sh

if ! [ -f ann_1.1.2.zip ]; then
    echo "ann_1.1.2.zip not found, downloading..."
    wget https://www.cs.umd.edu/~mount/ANN/Files/1.1.2/ann_1.1.2.zip
fi

if ! [ -f ann_1.1.2.zip ]; then
    echo "ann_1.1.2.zip not found."
    exit 1
fi

rm -r -f ann_1.1.2
unzip -q ann_1.1.2.zip

cd ann_1.1.2/src ; make linux-g++
