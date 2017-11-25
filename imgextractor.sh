#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: ./imgextractor.sh <IMGFolderPath>"
    exit 1
fi

cd $(dirname $0)
./build/bin/IMGExtractor $1 | tee output
