#include <iostream>
#include <string>
#include "mapledata.hpp"

/*
IMGDataType IMGData::getType() const {
    return type;
}
*/

int16_t ShortIMGData::getVal() const {
    return val;
}

int32_t IntIMGData::getVal() const {
    return val;
}

float FloatIMGData::getVal() const {
    return val;
}

double DoubleIMGData::getVal() const {
    return val;
}

const std::string& StringIMGData::getVal() const {
    return val;
}


/*
std::ostream& operator<<(std::ostream& os, const IMGData& data) {
    if (data.getType() == IMGDataType::SHORT) {
        auto shortVal = static_cast<const ShortIMGData&>(data);
        os << shortVal.getVal();
    } else if (data.getType() == IMGDataType::INT) {
        auto intVal = static_cast<const IntIMGData&>(data);
        os << intVal.getVal();
    } else if (data.getType() == IMGDataType::FLOAT) {
        auto floatVal = static_cast<const FloatIMGData&>(data);
        os << floatVal.getVal();
    } else if (data.getType() == IMGDataType::DOUBLE) {
        auto doubleVal = static_cast<const DoubleIMGData&>(data);
        os << doubleVal.getVal();
    }

    return os;
}
*/
