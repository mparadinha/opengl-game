import os
import sys
import json
import struct
import math
import argparse

component_num = {
    "SCALAR": 1,
    "VEC2": 2, "VEC3": 3, "VEC4": 4,
    "MAT2": 4, "MAT3": 9, "MAT4": 16
}
component_bytes = {
    5120: 1, 5121: 1, # byte and unsigned byte
    5122: 2, 5123: 2, # short and unsigned short
    5125: 4, # uint
    5126: 4  # float
}
format_code = {
    5120: "b", 5121: "B",
    5122: "h", 5123: "H",
    5125: "I", 5126: "f"
}

def split_chunks(l, chunk_size):
    if chunk_size == 1: return l
    return [l[i:i + chunk_size] for i in range(0, len(l), chunk_size)]

class GLTFBuffer:
    def __init__(self, directory, json_buffer):
        self.path = directory + "/" + json_buffer["uri"]
        with open(self.path, "rb") as f:
            self.bytes = f.read()

    def read(self, accessor, buffer_view):
        start = accessor.get("byteOffset", 0) + buffer_view["byteOffset"]
        bytes_per_elem = component_num[accessor["type"]] * component_bytes[accessor["componentType"]]
        end = start + bytes_per_elem * accessor["count"]
        return self.bytes[start:end]

def convert_data(data, accessor):
    element_num = accessor["count"] * component_num[accessor["type"]]
    format_str = str(element_num) + format_code[accessor["componentType"]]
    unpacked = list(struct.unpack(format_str, data))
    elements = split_chunks(unpacked, component_num[accessor["type"]])

    objs = []
    if "MAT" in accessor["type"]:
        for mat in elements:
            objs.append(split_chunks(mat, int(accessor["type"][-1])))

    else:
        objs = elements

    return objs

def pprint_data(objs, accessor):
    for i, obj in enumerate(objs):
        if "MAT" in accessor["type"]: # matrix
            for line in map(list, zip(*obj)):
                print(line)
            print()
        else: # scalar or vector
            print(obj)

def main(args):
    filepath = args.filepath
    accessor_index = args.accessor
    info_only = args.info

    with open(filepath, "r") as f:
        json_file = json.load(f)

    accessor = json_file["accessors"][accessor_index]

    # print general info about accessor
    for k in ["count", "min", "max", "type"]:
        if k in accessor: print(k, accessor[k])

    if info_only: return

    bin_buffer = GLTFBuffer(os.path.dirname(filepath), json_file["buffers"][0])

    data = bin_buffer.read(accessor, json_file["bufferViews"][accessor["bufferView"]])

    objs = convert_data(data, accessor)

    pprint_data(objs, accessor)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath", help="path to json file")
    parser.add_argument("accessor", type=int, help="index of accessor")
    parser.add_argument("--info", action="store_true",
        help="only show accessor info and not data")

    args = parser.parse_args()
    main(args)
