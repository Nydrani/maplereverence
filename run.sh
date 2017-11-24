#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: ./run.sh <MaplestoryFolderPath>"
    exit 1
fi

cd $(dirname $0)
./build/bin/MapleReverence $1 | tee output
