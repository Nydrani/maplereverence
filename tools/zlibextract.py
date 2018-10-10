#!/usr/bin/python3

import zlib
import sys


if len(sys.argv) != 2:
    print("incorrect params: " + len(sys.argv) + "| Usage: ./zlibextract <zlibbedfile>")
    sys.exit(1)

with open(sys.argv[1], "rb") as f:
    decompressor = zlib.decompressobj()
    decompressed = decompressor.decompress(f.read())

    with open(sys.argv[1] + "dec", "wb") as b:
        b.write(decompressed)
