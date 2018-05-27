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

using namespace gltf;

//TODO: use the bottom function to make this whole parsing better and more
// understandable. (read_obj and read_obj_list)
//TODO: use a macro for this huge if-else

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

template<typename T = unsigned char> // default to reading bytes
std::vector<T> uri_file_t::read(unsigned int offset, unsigned int count) {
    std::cout << "read(" << offset << ", " << count << ")" << std::endl;

    seek(offset); // set file pointer to the offset position

    T* data = new T[count];
    if(!data) {
        std::cout << "ERROR: couldn't create array for " << count * sizeof(T)
            << " bytes" << std::endl;
    }

    fread(data, sizeof(T), count, file);

    // convert c style array to c++ std::vector
    std::vector<T> vec(data, data + count);

    delete data;

    return vec;
}
// instanciate various types of call to templated functions
// has to be exist else compiler would give "undefined reference"
template std::vector<unsigned short> uri_file_t::read(unsigned int offset, unsigned count);
template std::vector<unsigned char> uri_file_t::read(unsigned int offset, unsigned count);
template std::vector<unsigned int> uri_file_t::read(unsigned int offset, unsigned count);
template std::vector<float> uri_file_t::read(unsigned int offset, unsigned count);

file_t::file_t(std::string filepath) {
    std::ifstream in(filepath);

    if(!in.is_open()) {
        std::cout << "[GLTF PARSER] error opening gltf file @ " << filepath << std::endl;
    }

    char ch;
    std::string name;
    while(true) {
        name = read_string(in);
        //std::cout << "file_t::name: " << name << std::endl;
        if(name == "accessors") {
            while(in >> ch && ch != ']') {
                accessors.push_back(accessor_t(in));
            }
        }
        else if(name == "bufferViews") {
            while(in >> ch && ch != ']') {
                buffer_views.push_back(buffer_view_t(in));
            }
            //std::cout << "buffer_views.size(): " << buffer_views.size() << std::endl;
        }
        else if(name == "animations") {
            while(in >> ch && ch != ']') {
                animations.push_back(animation_t(in));
            }
            //std::cout << "animations.size(): " << animations.size() << std::endl;
        }
        else if(name == "buffers") {
            while(in >> ch && ch != ']') {
                buffers.push_back(buffer_t(in));
            }
            //std::cout << "buffers.size(): " << buffers.size() << std::endl;
        }
        else if(name == "skins") {
            while(in >> ch && ch != ']') {
                skins.push_back(skin_t(in));
            }
            //std::cout << "skins.size(): " << skins.size() << std::endl;
        }
        else if(name == "nodes") {
            while(in >> ch && ch != ']') {
                nodes.push_back(node_t(in));
            }
            //std::cout << "nodes.size(): " << nodes.size() << std::endl;
        }
        else if(name == "asset") {
            asset.init(in);
        }
        else if(name == "materials") {
            while(in >> ch && ch != ']') {
                materials.push_back(material_t(in));
            }
            //std::cout << "materials.size(): " << materials.size() << std::endl;
        }
        else if(name == "scene") {
            std::string number;
            std::getline(in, number, ',');
            scene = to_uint(number);
        }
        else if(name == "scenes") {
            while(in >> ch && ch != ']') {
                scenes.push_back(scene_t(in));
            }
            //std::cout << "scenes.size(): " << scenes.size() << std::endl;
        }
        else if(name == "meshes") {
            while(in >> ch && ch != ']') {
                meshes.push_back(mesh_t(in));
            }
            //std::cout << "meshes.size(): " << meshes.size() << std::endl;
        }
        else {
            std::cout << "something else found: " << name << std::endl;
            break;
        }
    }
}

primitive_t::primitive_t(std::ifstream& in) {
    for(int i = 0; i < 3; i++) {
        std::string s = read_string(in);
        if(s == "attributes") {
            std::map<std::string, std::string> attrs = get_pairs(in);
            for(const auto &pair : attrs) {
                attributes[pair.first] = to_uint(pair.second);
            }
        }
        else if(s == "indices") {
            indices = read_uint(in);
        }
        else if(s == "material") {
            material = read_uint(in);
        }
    }
    // consume file until of object <=> until a '}' char
    //while(in >> ch && ch != '}') std::cout << "primitive_t::ch: " << ch << std::endl;
    //HACK: ^ this *should* be executed but because read_uint reads the last } char
    // i had to take it out
    //TODO: fix this when the todos at the top of the file are done
}

mesh_t::mesh_t(std::ifstream& in) {
    char ch;
    for(int i = 0; i < 2; i++) {
        std::string s = read_string(in);
        if(s == "name") {
            name = read_string(in);
        }
        else if(s == "primitives") {
            while(in >> ch && ch != ']') {
                primitives.push_back(primitive_t(in));
            }
        }
    }
    // consume file until the next '}' character
    while(in >> ch && ch != '}') std::cout << "mesh::ch: " << ch << std::endl;
}

scene_t::scene_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    name = pairs["name"];
    nodes = read_uint_vector(pairs["nodes"]);
}

node_t::node_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    children = read_uint_vector(pairs["children"]);
    mesh = to_uint(pairs["mesh"]);
    camera = to_uint(pairs["camera"]);
    skin = to_uint(pairs["skin"]);
    name = pairs["name"];

    if(pairs.count("matrix") == 1) {
        matrix = read_glm_matrix(pairs["matrix"]);
    }
    else {
        glm::vec3 s(1), t(0);
        if(pairs.count("scale")) read_glm_vector(pairs["scale"]);
        if(pairs.count("translation")) read_glm_vector(pairs["translation"]);
        glm::quat r;
        if(pairs.count("rotation")) r = read_glm_quat(pairs["rotation"]);

        glm::mat4 translation = glm::translate(glm::mat4(1), t),
            scale = glm::scale(glm::mat4(1), s),
            rotation = glm::mat4_cast(r);
        matrix = translation * rotation * scale;
    }
}

skin_t::skin_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    inverse_bind_matrix = to_uint(pairs["inverse_bind_matrix"]);
    joints = read_uint_vector(pairs["joints"]);
    skeleton = to_uint(pairs["skeleton"]);
}

void asset_t::init(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    generator = pairs["generator"];
    version = pairs["version"];
}

buffer_t::buffer_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    byte_length = to_uint(pairs["byteLength"]);
    uri = pairs["uri"];
}

accessor_t::accessor_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    buffer_view = to_uint(pairs["bufferView"]);
    byte_offset = to_uint(pairs["byteOffset"]);
    type = pairs["type"];
    component_type = to_uint(pairs["componentType"]);
    count = to_uint(pairs["count"]);
    max = read_float_vector(pairs["max"]);
    min = read_float_vector(pairs["min"]);
}

buffer_view_t::buffer_view_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    buffer = to_uint(pairs["buffer"]);
    byte_offset = to_uint(pairs["byteOffset"]);
    byte_length = to_uint(pairs["byteLength"]);
    byte_stride = to_uint(pairs["byteStride"]);
    target = to_uint(pairs["target"]);
}

animation_t::animation_t(std::ifstream& in) {
    char ch;
    for(int i = 0; i < 2; i++) { // need to read two strings
        std::string name = read_string(in);
        if(name == "channels") {
            while(in >> ch && ch != ']') {
                channels.push_back(channel_t(in));
            }
        }
        else if(name == "samplers") {
            while(in >> ch && ch != ']') {
                samplers.push_back(sampler_t(in));
            }
        }
    }

    // consume file until closing bracket
    while(in >> ch && ch != '}');
}

channel_t::channel_t(std::ifstream& in) {
    for(int i = 0; i < 2; i++) { // need to read to strings
        std::string name = read_string(in);
        if(name == "sampler") {
            std::string number;
            std::getline(in, number, ',');
            number = number.substr(number.find(':') + 1, number.length());
            clean_str(number);
            sampler = to_uint(number);
        }
        else if(name == "target") {
            this->target = target_t(in);
        }
    }
    // consume file until closing bracket
    char ch;
    while(in >> ch && ch != '}');
}

sampler_t::sampler_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);
    input = to_uint(pairs["input"]);
    output = to_uint(pairs["output"]);
    interpolation = pairs["interpolation"];
}

target_t::target_t(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);
    node = to_uint(pairs["node"]);
    path = pairs["path"];
}

void pbr_metallic_t::init(std::ifstream& in) {
    std::map<std::string, std::string> pairs = get_pairs(in);

    base_color_factor = read_float_vector(pairs["baseColorFactor"]);
    metallic_factor = to_f(pairs["metallic_factor"]);
}

material_t::material_t(std::ifstream& in) {
    char ch;
    for(int i = 0; i < 2; i++) {
        std::string s = read_string(in);
        if(s == "name") {
            name = read_string(in);
        }
        else if(s == "pbrMetallicRoughness") {
            pbr_metallic_roughness.init(in);
        }
    }
    // consume file until closing bracket
    while(in >> ch && ch != '}');
}

std::map<std::string, std::string> get_pairs(std::ifstream& in) {
    std::string content;
    std::getline(in, content, '}');
    std::size_t start_index = content.find_first_of('{');
    if(start_index != std::string::npos) {
        content = content.substr(start_index);
    }
    clean_str(content);
    return get_basic_pairs(content);
}
    
std::map<std::string, std::string> get_basic_pairs(std::string content) {
    std::vector<std::string> items, str_pairs = split_str(content, ':');

    // split string into list of items to be processed later into a map 
    for(unsigned int i = 0; i < str_pairs.size(); i++) {
        std::string pair = str_pairs[i];

        if(i != str_pairs.size() - 1) {
            std::vector<std::string> pair_v = split_str_last_of(pair, ',');
            for(std::string p : pair_v) {
                items.push_back(p);
            }
        }
        else {
            // the last "pair" is just a single attribute.
            // prevents splitting the last of element of vectors
            items.push_back(pair);
        }
    }

    std::map<std::string, std::string> pairs;
    for(unsigned int i = 0; i < items.size(); i += 2) {
        pairs[items[i]] = items[i + 1];
    }

    return pairs;
}

// read file until a string is found and return it
std::string read_string(std::ifstream& in) {
    char ch;
    std::string proto_str;
    while(in >> ch) {
        if(ch == '"') {
            std::getline(in, proto_str, '"');
            break;
        }
    }

    return proto_str;
}

float to_f(std::string s) { return std::strtof(s.c_str(), NULL); }
unsigned int to_uint(std::string s) { return std::strtoul(s.c_str(), NULL, 10); }

std::vector<float> read_float_vector(std::string source) {
    std::vector<float> numbers;
    for(std::string number_s : split_vector_str(source)) {
        numbers.push_back(to_f(number_s));
    }
    return numbers;
}

std::vector<unsigned int> read_uint_vector(std::string source) {
    std::vector<unsigned int> numbers;
    for(std::string number_s : split_vector_str(source)) {
        numbers.push_back(to_uint(number_s));
    }
    return numbers;
}

std::vector<std::string> split_vector_str(std::string source) {
    std::size_t start = source.find("["), end = source.find("]");
    std::string numbers_s = source.substr(start + 1, end);
    return split_str(numbers_s, ',');
}

std::vector<std::string> split_str_last_of(std::string source, char separator) {
    std::vector<std::string> res;
    std::size_t index = source.find_last_of(',');
    res.push_back(source.substr(0, index));
    if(index != std::string::npos) {
        res.push_back(source.substr(index + 1, source.length()));
    }
    return res;
}

std::vector<std::string> split_str(std::string source, char separator) {
    std::vector<std::string> res;
    std::istringstream iss(source);
    std::string line;
    while(std::getline(iss, line, separator)) {
        res.push_back(line);
    }
    return res;
}

bool remove_char(char c) {
    return (c == ' ' || c == '\n' || c == '"' || c == '{');
}
void clean_str(std::string& s) {
    s.erase(std::remove_if(s.begin(), s.end(), remove_char), s.end());
}

std::vector<std::string> read_pair(std::ifstream& in) {
    // create vector with two strings
    std::string pair_str;
    std::getline(in, pair_str, ',');
    // remove all whitespace and unwanted chars (e.g. " or ') from string
    pair_str.erase(std::remove_if(pair_str.begin(), pair_str.end(), remove_char), pair_str.end());
    std::cout << "pair_str: " << pair_str << std::endl;

    return split_str(pair_str, ':');
}

unsigned int read_uint(std::ifstream& in) {
    const std::string number_chars = "0123456789.+-e";
    std::vector<char> chars;
    char ch;
    // read until start of number
    while(in >> ch && number_chars.find(ch) == std::string::npos);
    while(in >> ch && number_chars.find(ch) != std::string::npos) {
        chars.push_back(ch);
    }

    return to_uint(std::string(chars.begin(), chars.end()));
}

glm::mat4 read_glm_matrix(std::string source) {
    std::vector<float> numbers = read_float_vector(source);
    //TODO: check that "numbers" has at least 4x4 elements and handle it
    glm::mat4 m;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m[i][j] = numbers[4 * i + j];

    return m;
}

glm::vec3 read_glm_vector(std::string source) {
    std::vector<float> numbers = read_float_vector(source);
    //TODO: handle erro if numbers doesn't have 3 elements
    glm::vec3 v;
    for(int i = 0; i < 3; i++) v[i] = numbers[i];
    return v;
}

glm::quat read_glm_quat(std::string source) {
    std::vector<float> numbers = read_float_vector(source);
    //TODO: make sure numbers has 4 elements
    glm::quat q;
    for(int i = 0; i < 4; i++) q[i] = numbers[i];
    return q;
}

// read a json object (anything between { and }) into a string.
// nesting is taken into account and the parent object is return including all
// of its child objects
// note: this function assumes that the initial { char has already been read
std::string read_obj(std::ifstream& in) {
    int nesting = 0;
    std::vector<char> chars;

    char ch;
    while(in >> ch) {
        switch(ch) {
        case '{':
            nesting++; break;
        case '}':
            nesting--; break;
        default:
            chars.push_back(ch); break;
        }

        if(nesting < 0) break;
    }

    return std::string(chars.begin(), chars.end());
}

// helper function to read a list of json objects taking nesting into account
// so that these objects can have lists inside them
// this function assumes that the initial [ char has already been read
std::vector<std::string> read_obj_list(std::ifstream& in) {
    int nesting = 0;
    std::vector<std::string> objs;

    char ch;
    while(in >> ch) {
        switch(ch) {
        case '[':
            nesting++; break;
        case ']':
            nesting--; break;
        default:
            objs.push_back(read_obj(in)); break;
        }

        if(nesting < 0) break;
    }

    return objs;
}
