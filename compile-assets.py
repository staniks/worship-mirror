import pathlib
import os
import shutil
import sys

bin_dir = f"{sys.path[0]}/build/worship"
skip_textures = False
pathlib.Path(bin_dir).mkdir(parents=True, exist_ok=True)

if os.name == 'nt':
    if len(sys.argv) < 2:
        print("no configuration specified (Debug or Release)")
        exit()
    bin_dir = f"{sys.path[0]}/build/{sys.argv[1]}"

if "--skip-textures" in sys.argv:
    skip_textures = True

if not skip_textures:
    for filename in pathlib.Path(f"{sys.path[0]}/data").rglob('*.png'):
        filename = str(filename)
        print(f"converting texture {filename}...")
        new_filename = filename[:-3] + "tex"
        os.system(f"python3 {sys.path[0]}/utility/texturizer.py \"{filename}\" \"{new_filename}\"")

os.system(f"python3 {sys.path[0]}/compile-levels.py")
os.system(f"python3 {sys.path[0]}/utility/packer.py {sys.path[0]}/data {bin_dir}/data.mau")
