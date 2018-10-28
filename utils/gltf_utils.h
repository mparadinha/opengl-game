#ifndef GLTF_UTILS_H
#define GLTF_UTILS_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<std::string> split_str(std::string source, char separator);

std::map<std::string, std::string> split_pairs(const std::string& in);
std::vector<std::string> split_obj_list(const std::string& in);
int32_t find_with_nesting(const std::string& in, char target);
std::string read_str(const std::string& in);
std::string clean_whitespace(const std::string& in);
std::string clean_obj(const std::string& in, char start, char end);
std::string map_get(std::map<std::string, std::string> map, std::string key, std::string fallback);

template<typename T>
void build_obj_list(const std::string& in, std::vector<T>& objs) {
    for(auto& obj_str : split_obj_list(clean_obj(in, '[', ']'))) {
        auto pairs = split_pairs(obj_str);
        objs.push_back(T(pairs));
    }
}

std::vector<unsigned int> to_uint_vec(const std::string& in);
std::vector<float> to_float_vec(const std::string& in);
glm::mat4 to_mat4(const std::string& in);
glm::vec3 to_vec3(const std::string& in);
glm::quat to_quat(const std::string& in);

#endif // include guard
