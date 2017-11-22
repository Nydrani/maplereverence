#!/bin/bash

cd $(dirname $0)
./build/bin/MapleReverence | tee output
