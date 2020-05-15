import glob
import os
import shutil
import sys

DATA_DIR = f"{sys.path[0]}/data"
LEVEL_STAGING_DIR = f"{sys.path[0]}/worship-tiled/levels-staging"
LEVEL_EXTENSION = "json"

for filename in glob.iglob(LEVEL_STAGING_DIR + f'**/*.{LEVEL_EXTENSION}', recursive=True):
    new_filename = filename[:-len(LEVEL_EXTENSION)] + "lvl"
    print(f"compiling {filename} to {new_filename}...")
    os.system(f"python3 {sys.path[0]}/utility/level-compiler.py {filename} {new_filename}")
    shutil.copy2(new_filename, DATA_DIR)
