#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "wztool.hpp"
#include "imgfile.hpp"
#include "constants.hpp"


const std::string& IMGFile::getName() const {
    return name;
}

int8_t IMGFile::readByte() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int8_t b;
    stream.read(reinterpret_cast<char *>(&b), sizeof(b));
    return b;
}

uint8_t IMGFile::readUnsignedByte() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint8_t b;
    stream.read(reinterpret_cast<char *>(&b), sizeof(b));
    return b;
}

int16_t IMGFile::readShort() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int16_t s;
    stream.read(reinterpret_cast<char *>(&s), sizeof(s));
    return s;
}

uint16_t IMGFile::readUnsignedShort() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint16_t s;
    stream.read(reinterpret_cast<char *>(&s), sizeof(s));
    return s;
}

int32_t IMGFile::readInt() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int32_t i;
    stream.read(reinterpret_cast<char *>(&i), sizeof(i));
    return i;
}

uint32_t IMGFile::readUnsignedInt() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint32_t i;
    stream.read(reinterpret_cast<char *>(&i), sizeof(i));
    return i;
}

int64_t IMGFile::readLong() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int64_t l;
    stream.read(reinterpret_cast<char *>(&l), sizeof(l));
    return l;
}

uint64_t IMGFile::readUnsignedLong() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint64_t l;
    stream.read(reinterpret_cast<char *>(&l), sizeof(l));
    return l;
}

std::string IMGFile::readString() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    std::string string;
    std::getline(stream, string, '\0' );
    return string;
}

std::string IMGFile::readString(int length) {
    std::string string;
    for (int i = 0; i < length; ++i) {
        uint8_t val = readUnsignedByte();
        string += val;
    }
    return string;
}

int32_t IMGFile::readCompressedInt() {
    int32_t val = readByte();

    if (val == -128) {
        val = readInt();
    }

    return val;
}

std::string IMGFile::readEncryptedString() {
    int8_t flag = readByte();
    int32_t strLength;
    std::string string;
    
    // todo read hex again
    if (flag == 0) {
        return string;
    }

    // 2 byte char
    if (flag > 0) {
        // @TODO remove when done
        std::cout << "~~~~~~DECRYPTING UNICODE~~~~~\n";

        uint16_t mask = 0xAAAA;
        // maximum number (127) (flag for compression)
        if (flag == 0x7f) {
            strLength = readInt();
        } else {
            strLength = flag;
        }

        if (strLength <= 0) {
            std::string string("String length must be greater than 0");
            throw std::invalid_argument(string);
        }

        string = readString(strLength);
        // decrypt string
        for (int i = 0; i < strLength; ++i) {
            string.at(i) ^= mask;
            ++mask;
        }
        //string = maplereverence::xorStringUnicode(string);
        string = maplereverence::xorStringAscii(string);

        std::cout << string << ' ' << stream.tellg() << '\n';
    }

    // 1 byte char
    if (flag < 0) {
        // minimum number (-128) (flag for compression)
        if (flag == -0x80) {
            strLength = readInt();
        } else {
            strLength = -flag;
        }

        if (strLength <= 0) {
            std::string string("String length must be greater than 0");
            throw std::invalid_argument(string);
        }

        string = readString(strLength);
        // decrypt string
        uint8_t mask = 0xAA;
        for (int i = 0; i < strLength; ++i) {
            string.at(i) ^= mask;
            ++mask;
        }
        string = maplereverence::xorStringAscii(string);

        std::cout << string << ' ' << stream.tellg() << '\n';
    }

    return string;
}

std::string IMGFile::readEncryptedString(int offset) {
    auto curPos = stream.tellg();

    stream.seekg(offset, std::ios::beg);
    readByte();
    std::string string = readEncryptedString();
    stream.seekg(curPos);

    return string;
}

bool BasicIMGFile::sanityCheck() {
    if (!stream.is_open()) {
        return false;
    }

    return true;
}

void BasicIMGFile::buildIMGStructure() {
}

void BasicIMGFile::print() const {
}
