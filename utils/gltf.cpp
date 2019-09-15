#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib> // for str to number conversions
#include <stdio.h> // for c file reading function (for binary file)

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

#include "gltf.h"
#include "gltf_utils.h"

using namespace gltf;

uri_file_t::uri_file_t(std::string filepath, unsigned int bytes) {
    file = fopen(filepath.c_str(), "rb");
    if(file == NULL) {
        std::cout << "ERROR: couldn't open binary file: " << filepath << std::endl;
    }
}

uri_file_t::~uri_file_t() {
    std::cout << "closing uri!\n";
    fclose(file);
}

void uri_file_t::seek(unsigned int offset) {
    fseek(file, offset, SEEK_SET);
}

file_t::file_t(const std::string& filepath) { 
    std::ifstream in(filepath);
    if(!in) {
        std::cerr << "could not open gltf file @ " << filepath << std::endl;
    }

    // read whole file into a string for easier access
    std::string str;    
    in.seekg(0, std::ios::end);
    str.reserve(in.tellg());
    in.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>());

    str = clean_obj(str, '{', '}');

    // TODO: figure out way to make a dispatcher map with different constructors
    // of different types

    auto pairs = split_pairs(str);
    for(auto& pair : pairs) {
        if(pair.first == "accessors") build_obj_list<accessor_t>(pair.second, accessors);
        if(pair.first == "bufferViews") build_obj_list<buffer_view_t>(pair.second, buffer_views);
        if(pair.first == "buffers") build_obj_list<buffer_t>(pair.second, buffers);
        if(pair.first == "skins") build_obj_list<skin_t>(pair.second, skins);
        if(pair.first == "animations") build_obj_list<animation_t>(pair.second, animations);
        if(pair.first == "nodes") build_obj_list<node_t>(pair.second, nodes);
        if(pair.first == "materials") build_obj_list<material_t>(pair.second, materials);
        if(pair.first == "meshes") build_obj_list<mesh_t>(pair.second, meshes);
        if(pair.first == "scenes") build_obj_list<scene_t>(pair.second, scenes);

        if(pair.first == "asset") {
            auto asset_pairs = split_pairs(clean_obj(pair.second, '{', '}'));
            asset = asset_t(asset_pairs);
        }
    }

    scene = std::stoul(map_get(pairs, "scene", "0"));
}

buffer_t::buffer_t(std::map<std::string, std::string>& pairs) {
    byte_length = std::stoul(pairs["byteLength"]);
    uri = read_str(pairs["uri"]);
}

buffer_view_t::buffer_view_t(std::map<std::string, std::string>& pairs) { 
    buffer = std::stoul(pairs["buffer"]);
    byte_offset = std::stoul(pairs["byteOffset"]);
    byte_length = std::stoul(pairs["byteLength"]);
    byte_stride = std::stoul(map_get(pairs, "bufferStride", "0"));
    target = std::stoul(map_get(pairs, "target", "0"));
}

accessor_t::accessor_t(std::map<std::string, std::string>& pairs) {
    buffer_view = std::stoul(pairs["bufferView"]);
    byte_offset = std::stoul(map_get(pairs, "byteOffset", "0"));
    component_type = std::stoul(pairs["componentType"]);
    count = std::stoul(pairs["count"]);
    type = read_str(pairs["type"]);

    if(pairs.count("max") != 0) { max = to_float_vec(pairs["max"]); }
}

primitive_t::primitive_t(std::map<std::string, std::string>& pairs) {
    mode = std::stoul(map_get(pairs, "mode", "0"));
    indices = std::stoul(pairs["indices"]);
    material = std::stoul(map_get(pairs, "material", "0"));
    for(auto& pair : split_pairs(clean_obj(pairs["attributes"], '{', '}'))) {
        attributes[pair.first] = std::stoul(pair.second);
    }
}

mesh_t::mesh_t(std::map<std::string, std::string>& pairs) {
    name = map_get(pairs, "name", "");
    build_obj_list<primitive_t>(pairs["primitives"], primitives);
}

skin_t::skin_t(std::map<std::string, std::string>& pairs) {
    inverse_bind_matrices = std::stoul(pairs["inverseBindMatrices"]);
    joints = to_uint_vec(pairs["joints"]);
    skeleton = std::stoul(pairs["skeleton"]);
}

node_t::node_t(std::map<std::string, std::string>& pairs) {
    children = to_uint_vec(pairs["children"]);
    mesh = std::stoul(map_get(pairs, "mesh", "0"));
    camera = std::stoul(map_get(pairs, "camera", "0"));
    skin = std::stoul(map_get(pairs, "skin", "0"));
    name = map_get(pairs, "name", "");

    if(pairs.count("matrix")) {
        matrix = to_mat4(pairs["matrix"]);
    }
    else {
        scale = to_vec3(map_get(pairs, "scale", "[1,1,1]"));
        translation = to_vec3(map_get(pairs, "translation", "[0,0,0]"));
        rotation = to_quat(map_get(pairs, "rotation", "[1,0,0,0]"));

        glm::mat4 trans_mat = glm::translate(glm::mat4(1), translation),
            scale_mat = glm::scale(glm::mat4(1), scale),
            rot_mat = glm::mat4_cast(rotation);
        matrix = trans_mat * rot_mat * scale_mat;
    }
}

target_t::target_t(std::map<std::string, std::string>& pairs) {
    node = std::stoul(pairs["node"]);
    path = read_str(pairs["path"]);
}

channel_t::channel_t(std::map<std::string, std::string>& pairs) {
    auto target_pairs = split_pairs(clean_obj(pairs["target"], '{', '}')); 
    target = target_t(target_pairs);
    sampler = std::stoul(pairs["sampler"]);
}

sampler_t::sampler_t(std::map<std::string, std::string>& pairs) {
    input = std::stoul(pairs["input"]);
    output = std::stoul(pairs["output"]);
    interpolation = read_str(pairs["interpolation"]);
}

animation_t::animation_t(std::map<std::string, std::string>& pairs) {
    build_obj_list<channel_t>(pairs["channels"], channels);
    build_obj_list<sampler_t>(pairs["samplers"], samplers);
}

asset_t::asset_t(std::map<std::string, std::string>& pairs) {
    generator = pairs["generator"];
    version = pairs["version"];
}

pbr_metallic_t::pbr_metallic_t(std::map<std::string, std::string>& pairs) {
    base_color_factor = to_float_vec(pairs["baseColorFactor"]);
    metallic_factor = std::stof(pairs["metallicFactor"]);
}

material_t::material_t( std::map<std::string, std::string>& pairs) {
    name = pairs["name"];
    auto pbr_pairs = split_pairs(clean_obj(pairs["pbrMetallicRoughness"], '{', '}'));
    pbr_metallic_roughness = pbr_metallic_t(pbr_pairs);
}

scene_t::scene_t( std::map<std::string, std::string>& pairs) {
    name = read_str(map_get(pairs, "name", ""));
    nodes = to_uint_vec(pairs["nodes"]);
}
