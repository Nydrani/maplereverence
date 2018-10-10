#!/usr/bin/python3

from PIL import Image
import struct
import sys


if len(sys.argv) != 2:
    print("needs 1 arg")
    sys.exit(1)

img = Image.new('RGBA', (29, 33))

with open(sys.argv[1], "rb") as f:
    pixels = img.load()

    for i in range(img.size[1]):    # for every col:
        for j in range(img.size[0]):    # For every row
            bbyte = struct.unpack('<B', f.read(1))[0]
            abyte = struct.unpack('<B', f.read(1))[0]

            a = abyte & 0xf0
            r = abyte & 0x0f
            g = bbyte & 0xf0
            b = bbyte & 0x0f

            a |= (a >> 4)
            r |= (r << 4)
            g |= (g >> 4)
            b |= (b << 4)

            pixels[j, i] = (r, g, b, a)
img.show()
