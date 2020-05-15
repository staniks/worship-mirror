from PIL import Image
import sys

TILESET_RESOLUTION = 512

original = Image.open(f"{sys.path[0]}/../data/tileset-diffuse.png")
resized = original.resize((TILESET_RESOLUTION, TILESET_RESOLUTION))
resized.save(f"{sys.path[0]}/tileset_textures.png", "PNG")
