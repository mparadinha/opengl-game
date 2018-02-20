import os
import sys
import shutil
import glob
import subprocess

flags = ("-Wall", "-pedantic")
include_dirs = ("./include", "/usr/include")
libs = ("dl", "glfw", "assimp")
src_dirs = (".", "./systems")
build_folder = "build"

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
    result = subprocess.run(
        args,
        stderr=subprocess.PIPE, stdout=subprocess.PIPE,
        universal_newlines=True
    )
    if result.stdout: print(result.stdout, file=sys.stdout)
    if result.stderr: print(result.stderr, file=sys.stderr)

def build_obj(source_file, directory, force=False):
    src = os.path.join(directory, source_file)
    obj = os.path.join(directory, build_folder, source_file.replace(".cpp", ".o"))
    objs.append(obj)

    # check if files was altered since last build
    if os.path.getmtime(src) < script_timestamp and os.path.exists(obj) and not force:
        return # no need to compile this one

    run_cmd(
        ["g++", "-c", "-o", obj, src, *flags, *include_flags, *lib_flags]
    )

    # touch the file to update the timestamp (so we can check on the next build)
    run_cmd(["touch", src], show=False)

def build_exe(executable):
    run_cmd(
        ["g++", "-o", executable, *tuple(objs), *flags, *include_flags, *lib_flags]
    )

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

    # touch this script at the end of compilation so that any file with a later
    # stamp indicates it was changed
    run_cmd(["touch", __file__], show=False)

if __name__ == "__main__":
    main(sys.argv)
