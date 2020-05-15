import glob
import os
import sys
import json
import struct

TILE_SIZE = 32
TILESET_SIZE = 16

if len(sys.argv) != 2 + 1:
    print("usage: level-compiler.py <level-json> <compiled-level>")
    exit()

src = sys.argv[1]
dst = sys.argv[2]

with open(src) as json_file:
    loaded_level = json.load(json_file)

width = int(loaded_level["width"])
height = int(loaded_level["height"])

class tile_info:
    def __init__(self):
        self.type = 0
        self.wall_texture = 0
        self.floor_texture = 0
        self.ceiling_texture = 0

tile_info_list = []

for i in range(width * height):
    tile_info_list.append(tile_info())

def get_real_tile_index(tiled_tile_index, tile_sheet):
    index = int(tiled_tile_index - TILESET_SIZE*TILESET_SIZE*tile_sheet) - 1
    if index < 0:
        index = 0
    return index

class object:
    def __init__(self):
        self.type = 0
        self.x = 0.0
        self.y = 0.0

object_list = []

OBJECT_MAPPING = {
    "player-spawn": 0,
    "light-source-white": 1,
    "light-source-red": 2,
    "light-source-green": 3,
    "light-source-blue": 4,
    "light-source-orange": 5,
    "armor": 6,
    "armor-shard": 7,
    "health": 8,
    "shells": 9,
    "shotgun": 10,
    "grenades" : 11,
    "grenade-launcher": 12,
    "enemy-light": 13,
    "plasma-rifle": 14,
    "plasma-cells": 15,
    "enemy-medium": 16,
    "enemy-heavy": 17
}

for layer in loaded_level["layers"]:
    if layer["name"] == "tile-type":
        for i, value in enumerate(layer["data"]):
            tile_info_list[i].type = get_real_tile_index(value, 0)
    elif layer["name"] == "tile-texture-wall":
        for i, value in enumerate(layer["data"]):
            tile_info_list[i].wall_texture = get_real_tile_index(value, 1)
    elif layer["name"] == "tile-texture-floor":
        for i, value in enumerate(layer["data"]):
            tile_info_list[i].floor_texture = get_real_tile_index(value, 1)
    elif layer["name"] == "tile-texture-ceiling":
        for i, value in enumerate(layer["data"]):
            tile_info_list[i].ceiling_texture = get_real_tile_index(value, 1)
    elif layer["name"] == "entities":
        for tiled_obj in layer["objects"]:
            obj = object()
            obj.type = OBJECT_MAPPING[tiled_obj["name"]]
            obj.x = float(tiled_obj["x"]) / TILE_SIZE
            obj.y = float(tiled_obj["y"]) / TILE_SIZE
            object_list.append(obj)

f = open(dst, 'w+b')
magic = int(7815552959266505037).to_bytes(8, byteorder='little')
width = width.to_bytes(4, byteorder='little')
height = height.to_bytes(4, byteorder='little')
object_count = len(object_list).to_bytes(4, byteorder='little')
f.write(magic)
f.write(width)
f.write(height)
f.write(object_count)

for tile in tile_info_list:
    tile_type = tile.type.to_bytes(1, byteorder='little')
    wall_texture = tile.wall_texture.to_bytes(2, byteorder='little')
    floor_texture = tile.floor_texture.to_bytes(2, byteorder='little')
    ceiling_texture = tile.ceiling_texture.to_bytes(2, byteorder='little')

    f.write(tile_type)
    f.write(wall_texture)
    f.write(floor_texture)
    f.write(ceiling_texture)

for obj in object_list:
    object_type = obj.type.to_bytes(1, byteorder='little')
    object_x = bytearray(struct.pack("f", obj.x))
    object_y = bytearray(struct.pack("f", obj.y))

    f.write(object_type)
    f.write(object_x)
    f.write(object_y)