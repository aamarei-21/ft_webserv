#!/usr/bin/python3

from os import walk, getcwd, path, remove, rmdir

mypath = getcwd()

for root, dirnames, filenames in walk(mypath):
    if "/.git" in root:
        continue
    copies_file = []
    for filename in filenames:
        name, extension = path.splitext(filename)
        copies_file.extend([f"{name} {i}{extension}" for i in range(2, 300)])
    file_set = set(filenames)
    file_set.intersection_update(copies_file)
    if file_set:
        for file in file_set:
            try:
                remove(f"{root}/{file}")
                print(f"{root}/{file} removed")
            except OSError:
                print(f"{root}/{file} error removed")
copies_dir = []
for root, dirnames, filenames in walk(mypath):
    copies = []
    for dirname in dirnames:
        copies.extend([f"{dirname} {i}" for i in range(2, 20)])
    dir_set = set(dirnames)
    dir_set.intersection_update(copies)
    if dir_set:
        copies_dir.extend([f"{root}/{dirname}" for dirname in dir_set])
print("Dir copies:")
for dir_name in copies_dir:
    try:
        rmdir(dir_name)
        print(dir_name, "removed")
    except OSError:
        print(f"{dir_name} error removed")
