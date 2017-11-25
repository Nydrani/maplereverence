#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: ./wzextractor.sh <MaplestoryFolderPath>"
    exit 1
fi

cd $(dirname $0)
./build/bin/WZExtractor $1 | tee output
