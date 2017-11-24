#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <unistd.h>

#include <boost/filesystem.hpp>

#include "wztool.hpp"
#include "wzfile.hpp"
#include "constants.hpp"


const std::string& WZFile::getName() const {
    return name;
}

int8_t WZFile::readByte() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int8_t b;
    stream.read(reinterpret_cast<char *>(&b), sizeof(b));
    return b;
}

uint8_t WZFile::readUnsignedByte() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint8_t b;
    stream.read(reinterpret_cast<char *>(&b), sizeof(b));
    return b;
}

int16_t WZFile::readShort() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int16_t s;
    stream.read(reinterpret_cast<char *>(&s), sizeof(s));
    return s;
}

uint16_t WZFile::readUnsignedShort() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint16_t s;
    stream.read(reinterpret_cast<char *>(&s), sizeof(s));
    return s;
}

int32_t WZFile::readInt() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int32_t i;
    stream.read(reinterpret_cast<char *>(&i), sizeof(i));
    return i;
}

uint32_t WZFile::readUnsignedInt() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint32_t i;
    stream.read(reinterpret_cast<char *>(&i), sizeof(i));
    return i;
}

int64_t WZFile::readLong() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    int64_t l;
    stream.read(reinterpret_cast<char *>(&l), sizeof(l));
    return l;
}

uint64_t WZFile::readUnsignedLong() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    uint64_t l;
    stream.read(reinterpret_cast<char *>(&l), sizeof(l));
    return l;
}

std::string WZFile::readString() {
    //std::cout << "CurPos<<: " << stream.tellg() << '\n';

    std::string string;
    std::getline(stream, string, '\0' );
    return string;
}

std::string WZFile::readString(int length) {
    std::string string;
    for (int i = 0; i < length; ++i) {
        uint8_t val = readUnsignedByte();
        string += val;
    }
    return string;
}

int32_t WZFile::readCompressedInt() {
    int32_t val = readByte();

    if (val == -128) {
        val = readInt();
    }

    return val;
}

std::string WZFile::readEncryptedString() {
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

std::string WZFile::readEncryptedString(int offset) {
    auto curPos = stream.tellg();

    stream.seekg(offset, std::ios::beg);
    readByte();
    std::string string = readEncryptedString();
    stream.seekg(curPos);

    return string;
}

bool WZFile::sanityCheck() const {
    return stream.is_open();
}

bool BasicWZFile::sanityCheck() {
    if (!stream.is_open()) {
        return false;
    }

    // check the size of file is the size given by the bytes
    std::ios::pos_type curPos = stream.tellg();
    stream.seekg(0, std::ios::end);
    unsigned long fileSize = header.dataSize + header.headerSize;
    if (fileSize != static_cast<unsigned long>(stream.tellg())) {
        return false;
    }
    // go back to where we were
    stream.seekg(curPos);

    return true;
}


void BasicWZFile::readHeader() {
    header.fileType = readString(4);
    header.dataSize = readUnsignedLong();
    header.headerSize = readUnsignedInt();
    header.fileDesc = readString();
    header.version = readUnsignedShort();
}

void BasicWZFile::generateMapleEntries(MapleFolder* folder) {
    int32_t numFiles = readCompressedInt();
    std::cout << folder->getName() << " | Num files: " << numFiles << '\n';

    for (int i = 0; i < numFiles; ++i) {
        int curPos = stream.tellg();
        uint8_t flag = readUnsignedByte();
        // string symlink
        if (flag == 2) { //0b10
            int32_t stringOffset = readInt() + header.headerSize;
            std::string name = readEncryptedString(stringOffset);
            int32_t size = readCompressedInt();
            int32_t checksum = readCompressedInt();
            int32_t unknown = readInt();
            // @TODO change this to actually be a symlink?
            // @TODO this might actually be 'correct' since it's
            // @TODO a pointer to the actualy location in the file
            folder->addEntry(std::unique_ptr<MapleEntry>(
                        new MapleEntry(name, size, checksum, unknown, curPos)));
        // folder
        } else if (flag == 3) { //0b11
            std::string name = readEncryptedString();
            int32_t size = readCompressedInt();
            int32_t checksum = readCompressedInt();
            int32_t unknown = readInt();
            folder->addEntry(std::unique_ptr<MapleFolder>(
                        new MapleFolder(name, size, checksum, unknown, curPos)));
        // entry
        } else if (flag == 4) { //0b11
            std::string name = readEncryptedString();
            int32_t size = readCompressedInt();
            int32_t checksum = readCompressedInt();
            int32_t unknown = readInt();
            folder->addEntry(std::unique_ptr<MapleEntry>(
                        new MapleEntry(name, size, checksum, unknown, curPos)));
        } else {
            std::string exception("Unknown flag: ");
            exception += std::to_string(flag);
            throw std::invalid_argument(exception);
        }
    }

    for (const auto& entry : folder->getEntries()) {
        auto folder = dynamic_cast<MapleFolder*>(entry.get());
        if (folder != nullptr) {
            generateMapleEntries(folder);
        }
    }
}

void BasicWZFile::print() const {
    // print header
    std::cout << header.fileType << ' ' << header.dataSize << ' ';
    std::cout << header.headerSize << ' ' << header.fileDesc << ' ';
    std::cout << header.version << '\n';

    // print maple entries
    root->print();
}

void BasicWZFile::extract() {
    root->extract(stream);
}

void BasicWZFile::findDataOffsets(MapleFolder* folder) {
    // set folder offset to current
    folder->setDataOffset(stream.tellg());

    // @TODO folders might just be ALWAYS physically located after files
    // @TODO for easier tree traversing --> so then two loops arent required
    // files are located before folders
    for (const auto& entry : folder->getEntries()) {
        auto folder = dynamic_cast<MapleFolder*>(entry.get());
        if (folder == nullptr) {
            entry->setDataOffset(stream.tellg());
            stream.seekg(entry->getByteSize(), std::ios::cur);
        }
    }

    // folders are located after files
    for (const auto& entry : folder->getEntries()) {
        auto folder = dynamic_cast<MapleFolder*>(entry.get());
        if (folder != nullptr) {
            findDataOffsets(folder);
        }
    }
}


void MapleEntry::setDataOffset(int dataOffset) {
    this->dataOffset = dataOffset;
}

int MapleEntry::getDataOffset() const {
    return dataOffset;
}

uint32_t MapleEntry::getByteSize() const {
    return bytesize;
}

const std::string& MapleEntry::getName() const {
    return name;
}


void MapleEntry::print() const {
    std::cout << name << " | " << headerOffset << ' ';
    std::cout << dataOffset << " | " << bytesize << ' ';
    std::cout << checksum << ' ' << unknown << '\n';
}

void MapleEntry::extract(std::ifstream& stream) {
    // store cur pos
    auto curPos = stream.tellg();

    // move to pos and read bytesize into buffer
    stream.seekg(dataOffset, std::ios::beg);

    char sanityByte = stream.get();

    if (sanityByte == maplereverence::imgSanityByte) {
        stream.seekg(dataOffset, std::ios::beg);
        std::ofstream outStream(name, std::ios::out | std::ios::binary);
        std::copy_n(std::istreambuf_iterator<char>(stream), bytesize,
                std::ostreambuf_iterator<char>(outStream));
    }

    // restore old pos
    stream.seekg(curPos);
}

void MapleFolder::extract(std::ifstream& stream) {
    // make folder and traverse into it
    const boost::filesystem::path folderPath(getName());
    boost::filesystem::create_directory(folderPath);
    chdir(getName().c_str());

    for (const auto& entry : entries) {
        entry->extract(stream);
    }

    // traverse back
    chdir("../");
}

void MapleFolder::addEntry(std::unique_ptr<MapleEntry> pointer) {
    entries.emplace_back(std::move(pointer));
}

void MapleFolder::print() const {
    std::cout << "Folder size: " << entries.size() << ' ';
    MapleEntry::print(); 
    for (const auto& entry : entries) {
        entry->print();
    }
}

