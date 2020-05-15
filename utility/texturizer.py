from PIL import Image
import glob
import os
import sys

if len(sys.argv) != 2 + 1:
    print("usage: texturizer.py <source_image> <destination_image>")
    exit()

src = sys.argv[1]
dst = sys.argv[2]

original = Image.open(src)
original = original.convert("RGBA")

rgba_stream = list(original.getdata())
rgba_stream = [component for rgba in rgba_stream for component in rgba]

f = open(dst, 'w+b')
magic = [0x4D, 0x41, 0x55, 0x04, 0x02, 0x74, 0x65, 0x78]
width = original.width.to_bytes(4, byteorder='little')
height = original.height.to_bytes(4, byteorder='little')
f.write(bytearray(magic))
f.write(width)
f.write(height)
f.write(bytearray(rgba_stream))