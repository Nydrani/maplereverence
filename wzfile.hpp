#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include "mapleaccessor.hpp"

#ifndef MAPLEREVERENCE_WZFILE
#define MAPLEREVERENCE_WZFILE

class MapleEntry {
    public:
        MapleEntry(const std::string& name,
                const uint32_t bytesize,
                const int32_t checksum,
                const uint32_t unknown,
                const int offset)
            : name(name), bytesize(bytesize), checksum(checksum),
            unknown(unknown), headerOffset(offset), dataOffset(0) {}
        virtual ~MapleEntry() {};

        void setDataOffset(int);
        int getDataOffset() const;
        uint32_t getByteSize() const;
        const std::string& getName() const;
        virtual void extract(MapleAccessor&);
        virtual void print() const;
    private:
        const std::string name;
        const uint32_t bytesize;
        const int32_t checksum;
        const uint32_t unknown;
        const int headerOffset;
        int dataOffset;
};

class MapleFolder: public MapleEntry {
    public:
        MapleFolder(const std::string& name,
                const uint32_t bytesize,
                const int32_t checksum,
                const uint32_t unknown,
                const int offset)
            : MapleEntry(name, bytesize, checksum, unknown, offset) {}
        ~MapleFolder() {}

        void extract(MapleAccessor&);
        void addEntry(std::unique_ptr<MapleEntry>);
        void print() const;
        const std::vector<std::unique_ptr<MapleEntry>>& getEntries() const {
            return entries;
        }
    private:
        std::vector<std::unique_ptr<MapleEntry>> entries;
};


class BasicWZFile {
    public:
        BasicWZFile(const std::string& name) : name(name), accessor(name) {

            readHeader();
            if (!sanityCheck()) {
                std::cout << "Invalid Header\n";
                return;
            }

            root = std::unique_ptr<MapleFolder>(
                    new MapleFolder(name, 0, 0, 0, accessor.tell()));

            generateMapleEntries(root.get());
            findDataOffsets(root.get());
        }
        ~BasicWZFile() {}

        void print() const;
        void extract();
        const std::string& getName() const;

    private:
        bool sanityCheck();
        void readHeader();
        void generateMapleEntries(MapleFolder* folder);
        void findDataOffsets(MapleFolder* folder);

        struct Header {
            std::string fileType;
            uint64_t dataSize;
            uint32_t headerSize;
            std::string fileDesc;
            uint16_t version;
        };
        Header header;

        const std::string name;
        MapleAccessor accessor;

        std::unique_ptr<MapleFolder> root;
};

class ListWZFile {
    public:
        ListWZFile(const std::string& name) : name(name) {}
        ~ListWZFile() {}
    private:
        const std::string name;
};

#endif
