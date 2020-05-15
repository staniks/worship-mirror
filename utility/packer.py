import binascii
import pathlib
import os
import sys
import zlib

resource_type_indirect = 0
resource_type_texture = 1
resource_type_shader = 2
resource_type_font = 3
resource_type_sound = 4

resource_types = {
    "tex": resource_type_texture,
    "txt": resource_type_indirect,
    "sha": resource_type_shader,
    "mfo": resource_type_font,
    "vs": resource_type_indirect,
    "fs": resource_type_indirect,
    "mp3": resource_type_sound,
    "wav": resource_type_sound,
    "lvl": resource_type_indirect
}

if len(sys.argv) != 2 + 1:
    print("usage: packer.py <source_directory> <destination_archive>")
    exit()

src_dir = sys.argv[1]
dst_file = sys.argv[2]

f = open(dst_file, 'w+b')
magic = int(0x726100020455414D).to_bytes(8, byteorder='little')
f.write(bytearray(magic))

file_list = []

for filename in pathlib.Path(src_dir).rglob('*'):
    filename = str(filename)
    if os.path.isdir(filename):
        continue
    file_extension = os.path.splitext(filename)[1][1:]
    if file_extension in resource_types.keys():
        file_list.append((filename, resource_types[file_extension]))

file_count = len(file_list).to_bytes(8, byteorder='little')
f.write(bytearray(file_count))

data_offset = 0

for (filename, resource_type) in file_list:
    print(f"compiling file {filename} as {resource_type}...")
    data_size = os.path.getsize(filename)
    data = open(filename, "rb").read()

    filename = filename.replace('\\', '/')

    normalized_filename = filename.replace('\\', '/')
    normalized_filename = filename[len(src_dir) + 1:]

    # Filename length
    filename_length = len(normalized_filename).to_bytes(2, byteorder='little')
    f.write(bytearray(filename_length))

    # Filename (ASCII)
    filename_ascii = normalized_filename.encode("ascii")
    f.write(bytearray(filename_ascii))

    # Resource type
    f.write(resource_type.to_bytes(1, byteorder='little'))

    # File data offset
    f.write(data_offset.to_bytes(8, byteorder='little'))

    # File data size
    f.write(data_size.to_bytes(8, byteorder='little'))

    # File data checksum
    f.write(zlib.adler32(data).to_bytes(4, byteorder='little'))

    data_offset += data_size

for (filename, resource_type) in file_list:
    data = open(filename, "rb").read()
    f.write(data)



