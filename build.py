import os
import sys
import shutil
import glob
import subprocess

flags = ("Wall", "pedantic")
include_dirs = ("./include", "/usr/include")
libs = ("dl", "glfw", "assimp")
src_dirs = (".", "./systems")
build_folder = "build"

flags = tuple("-" + f for f in flags)
include_flags = tuple("-I" + i for i in include_dirs)
lib_flags = tuple("-l" + l for l in libs)

src_files = []
for src_dir in src_dirs:
    src_files.extend(glob.glob(os.path.join(src_dir, "*.cpp")))
src_files = tuple(src_files)

objs = []

script_timestamp = os.path.getmtime(__file__)

def run_cmd(args, show=True): 
    if show: print(" ".join(args))
    return os.system(" ".join(args))

# decorator for having static local vars in functions
def static_var(name, value):
    def decorate(func):
        setattr(func, name, value)
        return func
    return decorate

@static_var("failed", False)
def build_obj(source_file, directory, force=False):
    src = os.path.join(directory, source_file)
    obj = os.path.join(directory, build_folder, source_file.replace(".cpp", ".o"))
    objs.append(obj)

    # check if files was altered since last build (or if its a new file)
    if os.path.getmtime(src) < os.path.getmtime(obj) and os.path.exists(obj) and not force:
        return # no need to compile this one

    ret_code = run_cmd(
        ["g++", "-c", "-o", obj, src, *flags, *include_flags, *lib_flags]
    )
    if ret_code != 0: build_obj.failed = True

@static_var("failed", False)
def build_exe(executable):
    ret_code = run_cmd(
        ["g++", "-o", executable, *tuple(objs), *flags, *include_flags, *lib_flags]
    )
    if ret_code != 0: build_exe.failed = True

def clean():
    for src_dir in src_dirs:
        folder = os.path.join(src_dir, build_folder)
        print("removing '{}' folder".format(folder))
        shutil.rmtree(folder)

def main(args):
    if "clean" in args:
        clean()
        return

    # make sure all build directories are created
    for src_dir in src_dirs:
        build_path = os.path.join(src_dir, build_folder)
        if not os.path.exists(build_path):
            print("creating '{}' as build directory".format(build_path))
            os.mkdir(build_path)
    
    # build all the objects
    for src in src_files:
        build_obj(src.split("/")[-1], "/".join(src.split("/")[:-1]),
            force="force" in args)

    # link final executable
    build_exe("thingy")

if __name__ == "__main__":
    main(sys.argv)
