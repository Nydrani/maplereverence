#include <iostream>
#include <algorithm>

#include <unistd.h>

#include <boost/filesystem.hpp>

#include "wzfile.hpp"
#include "constants.hpp"


bool BasicWZFile::sanityCheck() {
    if (!accessor.is_open()) {
        return false;
    }

    if (header.fileIdentifier != maplereverence::wzIdentifier) {
        return false;
    }

    // check the size of file is the size given by the bytes
    accessor.seek(0, std::ios::end);
    unsigned long fileSize = header.dataSize + header.headerSize;
    if (fileSize != static_cast<unsigned long>(accessor.tell())) {
        return false;
    }

    return true;
}


void BasicWZFile::readHeader() {
    header.fileIdentifier = accessor.readString(4);
    header.dataSize = accessor.readUnsignedLong();
    header.headerSize = accessor.readUnsignedInt();
    header.fileDesc = accessor.readString();
    header.version = accessor.readUnsignedShort();
}

void BasicWZFile::generateMapleEntries(MapleFolder* folder) {
    int32_t numFiles = accessor.readCompressedInt();
    std::cout << folder->getName() << " | Num files: " << numFiles << '\n';

    for (int i = 0; i < numFiles; ++i) {
        int curPos = accessor.tell();
        uint8_t flag = accessor.readUnsignedByte();
        // string symlink
        // @TODO this might just be a link to the name of the file
        // @TODO not actually a symlink to a file
        if (flag == 2) { //0b10
            // @TODO a +1 is required because the offset arrives at the 'flag' variable
            uint32_t stringOffset = accessor.readUnsignedInt() + header.headerSize + 1;
            std::string name = accessor.readEncryptedString(stringOffset);
            int32_t size = accessor.readCompressedInt();
            int32_t checksum = accessor.readCompressedInt();
            int32_t unknown = accessor.readInt();
            // @TODO change this to actually be a symlink?
            // @TODO this might actually be 'correct' since it's
            // @TODO a pointer to the actualy location in the file
            folder->addEntry(std::unique_ptr<MapleEntry>(
                        new MapleEntry(name, size, checksum, unknown, curPos)));
        // folder
        } else if (flag == 3) { //0b11
            std::string name = accessor.readEncryptedString();
            int32_t size = accessor.readCompressedInt();
            int32_t checksum = accessor.readCompressedInt();
            int32_t unknown = accessor.readInt();
            folder->addEntry(std::unique_ptr<MapleFolder>(
                        new MapleFolder(name, size, checksum, unknown, curPos)));
        // entry
        } else if (flag == 4) { //0b11
            std::string name = accessor.readEncryptedString();
            int32_t size = accessor.readCompressedInt();
            int32_t checksum = accessor.readCompressedInt();
            int32_t unknown = accessor.readInt();
            folder->addEntry(std::unique_ptr<MapleEntry>(
                        new MapleEntry(name, size, checksum, unknown, curPos)));
        } else {
            std::string exception("Unknown flag: ");
            exception += std::to_string(flag);
            exception += " at offset: ";
            exception += std::to_string(accessor.tell());
            throw std::runtime_error(exception);
        }
    }

    for (const auto& entry : folder->getEntries()) {
        auto* folder = dynamic_cast<MapleFolder*>(entry.get());
        if (folder != nullptr) {
            generateMapleEntries(folder);
        }
    }
}

void BasicWZFile::print() const {
    // print header
    std::cout << header.fileIdentifier << ' ' << header.dataSize << ' ';
    std::cout << header.headerSize << ' ' << header.fileDesc << ' ';
    std::cout << header.version << '\n';

    // print maple entries
    root->print();
}

void BasicWZFile::extract() {
    root->extract(accessor);
}

const std::string& BasicWZFile::getName() const {
    return name;
}

void BasicWZFile::findDataOffsets(MapleFolder* folder) {
    // set folder offset to current
    folder->setDataOffset(accessor.tell());

    // files are located before folders
    for (const auto& entry : folder->getEntries()) {
        auto* folder = dynamic_cast<MapleFolder*>(entry.get());
        if (folder == nullptr) {
            entry->setDataOffset(accessor.tell());
            accessor.seek(entry->getByteSize(), std::ios::cur);
        }
    }

    // folders are located after files
    for (const auto& entry : folder->getEntries()) {
        auto* folder = dynamic_cast<MapleFolder*>(entry.get());
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

void MapleEntry::extract(MapleAccessor& accessor) {
    // store cur pos
    auto curPos = accessor.tell();

    // move to pos and read bytesize into buffer
    accessor.seek(dataOffset, std::ios::beg);

    char sanityByte = accessor.readByte();

    if (sanityByte != maplereverence::imgEntryStringByte) {
        std::string exception("Invalid sanity byte: ");
        exception += std::to_string(sanityByte);
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }

    // extract data
    accessor.seek(dataOffset, std::ios::beg);
    std::ofstream outStream(name, std::ios::out | std::ios::binary);
    std::copy_n(std::istreambuf_iterator<char>(accessor.getStream()),
            bytesize, std::ostreambuf_iterator<char>(outStream));

    // restore old pos
    accessor.seek(curPos);
}

void MapleFolder::extract(MapleAccessor& stream) {
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

const std::vector<std::unique_ptr<MapleEntry>>&
MapleFolder::getEntries() const {
    return entries;
}
