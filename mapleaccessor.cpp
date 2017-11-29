#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>

#include "wztool.hpp"
#include "mapleaccessor.hpp"


int8_t MapleAccessor::readByte() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int8_t b;
    stream.read(reinterpret_cast<char *>(&b), sizeof(b));
    return b;
}

uint8_t MapleAccessor::readUnsignedByte() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint8_t b;
    stream.read(reinterpret_cast<char *>(&b), sizeof(b));
    return b;
}

int16_t MapleAccessor::readShort() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int16_t s;
    stream.read(reinterpret_cast<char *>(&s), sizeof(s));
    return s;
}

uint16_t MapleAccessor::readUnsignedShort() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint16_t s;
    stream.read(reinterpret_cast<char *>(&s), sizeof(s));
    return s;
}

int32_t MapleAccessor::readInt() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int32_t i;
    stream.read(reinterpret_cast<char *>(&i), sizeof(i));
    return i;
}

uint32_t MapleAccessor::readUnsignedInt() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint32_t i;
    stream.read(reinterpret_cast<char *>(&i), sizeof(i));
    return i;
}

int64_t MapleAccessor::readLong() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int64_t l;
    stream.read(reinterpret_cast<char *>(&l), sizeof(l));
    return l;
}

uint64_t MapleAccessor::readUnsignedLong() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint64_t l;
    stream.read(reinterpret_cast<char *>(&l), sizeof(l));
    return l;
}

float MapleAccessor::readFloat() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    float f;
    stream.read(reinterpret_cast<char *>(&f), sizeof(f));
    return f;
}

std::string MapleAccessor::readString() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    std::string string;
    std::getline(stream, string, '\0' );
    return string;
}

std::string MapleAccessor::readString(int length) {
    std::string string;
    for (int i = 0; i < length; ++i) {
        uint8_t val = readUnsignedByte();
        string += val;
    }
    return string;
}

int32_t MapleAccessor::readCompressedInt() {
    int8_t flag = readByte();
    int32_t val = flag;

    if (flag == -128) {
        val = readInt();
    }

    return val;
}

float MapleAccessor::readCompressedFloat() {
    int8_t flag = readByte();
    float val = 0;

    if (flag == -128) {
        val = readFloat();
    }

    return val;
}

std::string MapleAccessor::readEncryptedString() {
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

std::string MapleAccessor::readEncryptedString(uint32_t offset) {
    auto curPos = stream.tellg();

    stream.seekg(offset, std::ios::beg);
    std::string string = readEncryptedString();
    stream.seekg(curPos);

    return string;
}

std::vector<uint8_t> MapleAccessor::readData(uint32_t length) {
    // initialise and reserve length
    std::vector<uint8_t> data;
    data.reserve(length);

    // extract data
    std::copy_n(std::istreambuf_iterator<char>(stream),
            length, data.begin());

    return data;
}

void MapleAccessor::seek(std::ifstream::pos_type pos) {
    stream.seekg(pos);
}

void MapleAccessor::seek(std::ifstream::off_type off, std::ios_base::seekdir dir) {
    stream.seekg(off, dir);
}

std::ifstream::pos_type MapleAccessor::tell() {
    return stream.tellg();
}

bool MapleAccessor::is_open() const {
    return stream.is_open();
}

std::ifstream& MapleAccessor::getStream() {
    return stream;
}
