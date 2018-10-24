#include <iostream>
#include <sstream>

#include "gltf_utils.h"

/* string utils */

std::vector<std::string> split_str(std::string source, char separator) {
    std::vector<std::string> res;
    std::istringstream iss(source);
    std::string line;
    while(std::getline(iss, line, separator)) {
        res.push_back(line);
    }
    return res;
}

/* gltf object specific utils */

std::string map_get(std::map<std::string, std::string> map, std::string key, std::string fallback) {
    if(map.count(key) == 0) return fallback;
    return map[key];
}

// assumes obj input string does not have the opening/closing brackets {}
std::map<std::string, std::string> split_pairs(const std::string& in) {
    std::map<std::string, std::string> pairs;

    std::string current = clean_whitespace(in);
    bool done = false;

    while(!done) {
        std::string key = read_str(current);
        uint32_t start_of_value = current.find_first_of(':') + 1;
        int32_t end_of_value = find_with_nesting(current, ',');
        //std::cout << "end: " << end_of_value << std::endl;

        // last pair. use the rest of the string
        if(end_of_value == -1) {
            end_of_value = current.size();
            done = true;
        }

        //std::cout << "star, end: " << start_of_value << ", " << end_of_value << std::endl;
        pairs[key] = current.substr(start_of_value, end_of_value - start_of_value);

        if(!done) current = current.substr(end_of_value + 1);
    }

    return pairs;
}

// divide a list of objs (without the []) into a list of objs without {}
std::vector<std::string> split_obj_list(const std::string& in) {
    std::vector<std::string> objs;

    std::string current = clean_whitespace(in);

    bool done = false;
    while(!done) {
        int32_t end_of_obj = find_with_nesting(current, ',');

        if(end_of_obj == -1) {
            objs.push_back(clean_obj(current, '{', '}'));

            done = true;
        }
        else {
            std::string obj = current.substr(0, end_of_obj);
            objs.push_back(clean_obj(obj, '{', '}'));
 
            current = current.substr(end_of_obj + 1);
        }
    }

    return objs;
}

// find index of first occurence of target taking nesting into account
// return -1 if not found
int32_t find_with_nesting(const std::string& in, char target) {
    uint32_t nesting_curly = 0, nesting_square = 0;
    //std::cout << "char, curly, square:\n";
    for(uint32_t i = 0; i < in.size(); i++) {
        if(in[i] == '[') nesting_square++;
        if(in[i] == ']') nesting_square--;
        if(in[i] == '{') nesting_curly++;
        if(in[i] == '}') nesting_curly--;
        //std::cout << i << ": " << in[i] << " :: " << nesting_curly << " :: " << nesting_square << std::endl;

        if(in[i] == target && nesting_square == 0 && nesting_curly == 0) {
            return i;
        }
    }

    return -1;
}

std::string read_str(const std::string& in) {
    uint32_t open = in.find_first_of('"');
    uint32_t close = in.find_first_of('"', open + 1);
    return in.substr(open + 1, close - open - 1);
}

// return the input string without any whitespace. whitespace between
// corresponding quotes is preserved
std::string clean_whitespace(const std::string& in) {
    std::vector<char> chars;
    bool inside_string = false;
    for(auto ch : in) {
        if(ch == '"') inside_string = !inside_string;

        if(std::isspace(ch) && inside_string) chars.push_back(ch);
        else if(!std::isspace(ch)) chars.push_back(ch);
    }

    return std::string(chars.begin(), chars.end());
}

// remove the object delimiters, {} and [] only for outer most object
std::string clean_obj(const std::string& in, char start, char end) {
    return in.substr(
        in.find_first_of(start) + 1,
        in.find_last_of(end) - 1);
}

// TODO: use new c++11 functions to convert between numbers and strings
// std::stoi, std::stof, std::stod, etc...
// TODO: rewrite all the ctors to use the new templated function and these
// new functions
// TODO: rewrite functions to convert a string obj (non clean) and return
// a glm mat4, vec3, vec4

/* number/vector/matrix parsing utils */

std::vector<unsigned int> to_uint_vec(const std::string& in) {
    std::vector<unsigned int> res;
    for(auto& n : split_str(clean_obj(in, '[', ']'), ',')) {
        res.push_back(std::stoul(n));
    }
    return res;
}

std::vector<float> to_float_vec(const std::string& in) {
    std::vector<float> res;
    for(auto& n : split_str(clean_obj(in, '[', ']'), ',')) {
        res.push_back(std::stof(n));
    }
    return res;
}

glm::mat4 to_mat4(const std::string& in) {
    auto floats = to_float_vec(in);
    return glm::make_mat4(floats.data());
}

glm::vec3 to_vec3(const std::string& in) {
    auto floats = to_float_vec(in);
    return glm::make_vec3(floats.data());
}

glm::quat to_quat(const std::string& in) {
    auto floats = to_float_vec(in);
    return glm::make_quat(floats.data());
}
