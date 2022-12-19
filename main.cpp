#include <iostream>
#include <vector>
#include "json.hpp"
#include "jsonReader.h"
#include "windows.h"
#include "math.h"

using json = nlohmann::json;

struct time {
    int hours;
    int minutes;
    float seconds;
};

float altitude;
float heading;
struct time time_param;
struct time latitude;
struct time longitude;

enum dataType {
    INTEGER, FLOATING_POINT, TIME, UNRECOGNIZED_TYPE, CHARACTER
};

enum parameter {
    LONGITUDE, LATITUDE, ALTITUDE, HEADING, TIMEPARAM, UNRECOGNIZED_PARAM, LONG_SIGN, LAT_SIGN
};

//Function to get corresponding enum from string
enum dataType get_type(const std::string &typeString) {
    if (typeString == "integer") {
        return INTEGER;
    } else if (typeString == "floating_point") {
        return FLOATING_POINT;
    } else if (typeString == "time") {
        return TIME;
    } else if (typeString == "character") {
        return CHARACTER;
    } else {
        return UNRECOGNIZED_TYPE;
    }
}

//Function to get corresponding enum from string
enum parameter get_parameter(const std::string &paramString) {
    if (paramString == "longitude") {
        return LONGITUDE;
    } else if (paramString == "latitude") {
        return LATITUDE;
    } else if (paramString == "altitude") {
        return ALTITUDE;
    } else if (paramString == "heading") {
        return HEADING;
    } else if (paramString == "time") {
        return TIMEPARAM;
    } else if (paramString == "long_sign") {
        return LONG_SIGN;
    } else if (paramString == "lat_sign") {
        return LAT_SIGN;
    } else {
        return UNRECOGNIZED_PARAM;
    }
}


std::vector<std::string> process_line_of_stream(std::stringstream stream, char delim) {
    std::string line;
    std::vector<std::string> output;
    while (getline(stream, line, delim)) {
        output.push_back(line);
    }
    return output;
}


void insert_value(enum parameter name, enum dataType type, const std::string &value) {
    //Define intermediate value that allows the function to be two seperate steps
    float result;
    //Return if value is empty
    if (value.empty()) {
        return;
    }
    switch (type) {
        case INTEGER:
            result = stoi(value);
            break;
        case FLOATING_POINT:
            result = stof(value);
            break;
        case TIME:
            //Special time processing is done later
            result = stof(value);
            break;
        case CHARACTER:
            result = value[0];
            break;
        default:
            std::cout << "ERROR: Unrecognized Type" << std::endl;
            break;
    }
    switch (name) {
        case LONGITUDE:
            longitude.seconds = fmod(result,100);
            longitude.hours = result/100;
            break;
        case LATITUDE:
            latitude.seconds = fmod(result,100);
            latitude.hours = result/100;
            break;
        case ALTITUDE:
            altitude = result;
            break;
        case HEADING:
            heading = result;
            break;
        case TIMEPARAM:
            time_param.seconds = fmod(result,100);
            time_param.hours = result/10000;
            time_param.minutes = result/100 - time_param.hours*100;
            break;
        case LONG_SIGN:
            if(result == 'W') longitude.hours*=(-1);
            break;
        case LAT_SIGN:
            if(result == 'S') latitude.hours*=(-1);
            break;
        default:
            std::cout << "ERROR: Unrecognized Parameter" << std::endl;
            break;
    }

}

//Function takes an fstream and then returns a vector of the ',' seperated strings in the first line of that file
std::vector<std::string> process_line_of_stream(std::fstream &stream) {
    //Setup output and multiuse line variable
    std::vector<std::string> output;
    std::string line;
    //Get first line from stream
    getline(stream, line);
    //Make stringstream from that line
    std::stringstream lineStream(line);
    //Read individual csvs in stream
    while (getline(lineStream, line, ',')) {
        output.push_back(line);
    }
    //Return the combined vector
    return output;
}


std::vector<std::string> process_line_of_stream(std::stringstream stream){
    std::string line;
    std::vector<std::string> output;
    while (getline(stream, line, ',')) {
        output.push_back(line);
    }
    return output;
}

void reset_all() {
    latitude.hours = 0;
    latitude.minutes = 0;
    latitude.seconds = 0;
    longitude.hours = 0;
    longitude.minutes = 0;
    longitude.seconds = 0;
    altitude = 0;
    heading = 0;
    time_param.hours = 0;
    time_param.minutes = 0;
    time_param.seconds = 0;
}

int main() {
    HANDLE h_serial = CreateFile("COM3", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (h_serial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::cout << "ERROR: Serial port not found" << std::endl;
        }
        std::cout << "ERROR: 1" << std::endl;
    }

    DCB dcbSerialParam = {0};
    dcbSerialParam.DCBlength = sizeof(dcbSerialParam);

    if (!GetCommState(h_serial, &dcbSerialParam)) {
        std::cout << "ERROR: 2" << std::endl;
    }

    dcbSerialParam.BaudRate = CBR_9600;
    dcbSerialParam.ByteSize = 8;
    dcbSerialParam.StopBits = ONESTOPBIT;
    dcbSerialParam.Parity = NOPARITY;

    if (!SetCommState(h_serial, &dcbSerialParam)) {
        std::cout << "ERROR: 3" << std::endl;
    }

    COMMTIMEOUTS timeout = {0};
    timeout.ReadIntervalTimeout = 60;
    timeout.ReadTotalTimeoutConstant = 60;
    timeout.ReadTotalTimeoutMultiplier = 15;
    timeout.WriteTotalTimeoutConstant = 60;
    timeout.WriteTotalTimeoutMultiplier = 8;
    if (!SetCommTimeouts(h_serial, &timeout)) {
        std::cout << "ERROR: 4" << std::endl;
    }


    //PART 1: SETUP ----------------------------------------------------------------------------------------------------
    //Current row of input.txt
    std::vector<std::string> inputRow;
    //fstream for input file
    std::fstream inputFile("input.txt", std::ios::in);
    //Custom class for reading from json
    jsonReader myReader("format.json");

    //PART 2:LOOP THROUGH INPUT LINES-----------------------------------------------------------------------------------

    //Main loop
    while (1) {
        char sBuff[1] = {0};
        DWORD dwread = 0;
        std::string GPSstring = "";
        while (sBuff[0] != '\n') {
            if (!ReadFile(h_serial, sBuff, 1, &dwread, NULL)) {
                std::cout << "ERROR: 5" << std::endl;
            } else {
                GPSstring.push_back(sBuff[0]);
            }

        }

        //Fill a new row of input each time
        inputRow = process_line_of_stream(std::stringstream(GPSstring));
        //Check to make sure input has not reached EOF
        if (inputFile.eof()) return 0;

        //PART 3: LOOP THROUGH FORMAT LINES-----------------------------------------------------------------------------

        //Check if tag is in format file
        if (myReader.find_match(inputRow[0])) {
            //Loops through each format line
            while (myReader.read_next_format()) {
                //Insert values
                insert_value(get_parameter(myReader.currentParameter), get_type(myReader.currentType),
                             inputRow[myReader.currentPosition]);
            }

            //PART 4: DISPLAY OUTPUT----------------------------------------------------------------------------------------

            std::cout << " Lat: " << latitude.hours <<  ":" << latitude.seconds << " Long: " << longitude.hours << ":" << longitude.seconds << " Alt: "
                      << altitude << " Head: " << heading << " Time: " << time_param.hours << ":" << time_param.minutes
                      << ":" << time_param.seconds << std::endl;
            //Reset parameters
            reset_all();
            //If tag unrecognized, print error
        } else {
            //std::cout << "Unrecognized format: " << inputRow[0] << std::endl;
        }


    }
}
