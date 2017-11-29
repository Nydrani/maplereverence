#include <memory>
#include <iostream>
#include <vector>

#include "mapledata.hpp"


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

const std::pair<int32_t, int32_t>& VectorIMGData::getVal() const {
    return val;
}

const std::pair<int32_t, int32_t>& ConvexIMGData::getVal() const {
    return val;
}

const std::vector<uint8_t>& CanvasIMGData::getVal() const {
    return val;
}

const std::vector<uint8_t>& SoundIMGData::getHeader() const {
    return header;
}

const std::vector<uint8_t>& SoundIMGData::getData() const {
    return data;
}

const std::string& UOLIMGData::getVal() const {
    return val;
}


std::ostream& operator<<(std::ostream& os, const std::vector<uint8_t>& data) {
    os << "not printing raw data2 ";
    (void)data;
    /*
    for (const auto& val : data) {
        os << unsigned(val);
    }
    */
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::pair<int32_t, int32_t>& data) {
    os << "printing vector2 ";
    os << data.first;
    os << " ";
    os << data.second;
    return os;
}

std::ostream& operator<<(std::ostream& os, IMGData* data) {
    if (dynamic_cast<const NoneIMGData*>(data)) {
        os << "printing none";
        return os;
    } else if (auto shortVal = dynamic_cast<const ShortIMGData*>(data)) {
        os << "printing short ";
        os << shortVal->getVal();
        return os;
    } else if (auto intVal = dynamic_cast<const IntIMGData*>(data)) {
        os << "printing int ";
        os << intVal->getVal();
        return os;
    } else if (auto floatVal = dynamic_cast<const FloatIMGData*>(data)) {
        os << "printing float ";
        os << floatVal->getVal();
        return os;
    } else if (auto doubleVal = dynamic_cast<const DoubleIMGData*>(data)) {
        os << "printing double ";
        os << doubleVal->getVal();
        return os;
    } else if (auto stringVal = dynamic_cast<const StringIMGData*>(data)) {
        os << "printing string ";
        os << stringVal->getVal();
        return os;
    } else if (auto vectorVal = dynamic_cast<const VectorIMGData*>(data)) {
        os << "printing vector ";
        os << vectorVal->getVal();
        return os;
    } else if (auto convexVal = dynamic_cast<const ConvexIMGData*>(data)) {
        os << "printing convex ";
        os << convexVal->getVal();
        return os;
    } else if (auto canvasVal = dynamic_cast<const CanvasIMGData*>(data)) {
        os << "printing canvas ";
        os << canvasVal->getVal();
        return os;
    } else if (auto soundVal = dynamic_cast<const SoundIMGData*>(data)) {
        os << "printing sound ";
        os << soundVal->getHeader();
        os << soundVal->getData();
        return os;
    } else if (auto uolVal = dynamic_cast<const UOLIMGData*>(data)) {
        os << "printing uol ";
        os << uolVal->getVal();
        return os;
    } else {
        os << "printing unknown\n";
    }

    return os;
}
