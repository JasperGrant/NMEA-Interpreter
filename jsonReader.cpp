//
// Created by jgrant on 2022-09-15.
//

#include "jsonReader.h"


jsonReader::jsonReader(std::string fileName) {
    jsonReader::formatFile.open(fileName);
    jsonReader::data = json::parse(formatFile);
}

bool jsonReader::read_next_format() {
    static int format = 0;
    if(jsonReader::data[jsonReader::currentTag][format]["parameter"] != nullptr){
        currentParameter = jsonReader::data[jsonReader::currentTag][format]["parameter"];
        currentPosition = jsonReader::data[jsonReader::currentTag][format]["position"];
        currentType = jsonReader::data[jsonReader::currentTag][format]["type"];
        format++;
        return true;
    }
    format = 0;
    return false;
}

bool jsonReader::find_match(const std::string& tag){
    if(jsonReader::data.contains(tag)){
        jsonReader::currentTag = tag;
        return true;
    }
    return false;
}
