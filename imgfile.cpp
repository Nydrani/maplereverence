#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "wztool.hpp"
#include "imgfile.hpp"
#include "constants.hpp"


void IMGFile::print() const {
}

const std::string& IMGFile::getName() const {
    return name;
}

bool IMGFile::sanityCheck() {
    if (!accessor.is_open()) {
        return false;
    }

    char firstByte = accessor.readByte();
    if (firstByte != maplereverence::imgCategoryStringByte) {
        return false;
    }

    return true;
}

void IMGFile::buildIMGStructure() {
}

void IMGFile::parseIMGCategory() {
}

void IMGFile::parseIMGEntry() {
}
