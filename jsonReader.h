//
// Created by jgrant on 2022-09-15.
//

#ifndef DATA_PARSER_REV2_JSONREADER_H
#define DATA_PARSER_REV2_JSONREADER_H

#include <iostream>
#include <fstream>
#include "cmake-build-debug/json.hpp"

using json = nlohmann::json;

class jsonReader {
public:
    std::fstream formatFile;
    json data;
    std::string currentTag;
    std::string currentParameter;
    int currentPosition;
    std::string currentType;
    jsonReader(std::string fileName);
    bool read_next_format();
    bool find_match(const std::string &tag);
};


#endif //DATA_PARSER_REV2_JSONREADER_H
