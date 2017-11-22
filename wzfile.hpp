#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <set>

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
          unknown(unknown), offset(offset)
        {}
        virtual ~MapleEntry() {};

        virtual void print() const;
    private:
        const std::string name;
        const uint32_t bytesize;
        const int32_t checksum;
        const uint32_t unknown;
        const int offset;
};

class MapleFolder: public MapleEntry {
    public:
        MapleFolder(const std::string& name,
                const uint32_t bytesize,
                const int32_t checksum,
                const uint32_t unknown,
                const int offset)
        : MapleEntry(name, bytesize, checksum, unknown, offset)
        {}
        ~MapleFolder() {}

        // @TODO
        //void extractMapleData(std::ifstream&);
        void addEntry(std::unique_ptr<MapleEntry>);
        void print() const;
        const std::set<std::unique_ptr<MapleEntry>>& getEntries() const {
            return entries;
        }
    private:
        std::set<std::unique_ptr<MapleEntry>> entries;
};


class WZFile {
    public:
        WZFile(const std::string& name)
            : name(name), stream(name, std::ios::in | std::ios::binary) {}
        virtual ~WZFile() {};
    protected:
        int8_t readByte();
        uint8_t readUnsignedByte();
        int16_t readShort();
        uint16_t readUnsignedShort();
        int32_t readInt();
        uint32_t readUnsignedInt();
        int64_t readLong();
        uint64_t readUnsignedLong();
        std::string readString();
        std::string readString(int length);
        int32_t readCompressedInt();
        std::string readEncryptedString();
        std::string readEncryptedString(int offset);
        virtual bool sanityCheck() const;

        const std::string name;
        std::ifstream stream;
    private:
};

class BasicWZFile : public WZFile {
    public:
        BasicWZFile(const std::string& name)
            : WZFile(name) {
            if (!WZFile::sanityCheck()) {
                std::cout << "Invalid Data!\n";
                return;
            }

            readHeader();
            if (!sanityCheck()) {
                std::cout << "Invalid Header!\n";
                return;
            }

            root = std::unique_ptr<MapleFolder>(
                    new MapleFolder("Root", 0, 0, 0, 0));
            std::cout << "Reading Entries\n";
            generateMapleEntries(root.get());
        }
        ~BasicWZFile() {}

        void print() const;
    protected:
        using WZFile::sanityCheck;
        bool sanityCheck();
    private:
        struct Header {
            std::string fileType;
            uint64_t dataSize;
            uint32_t headerSize;
            std::string fileDesc;
            uint16_t version;
        };
        Header header;
        void readHeader();
        void generateMapleEntries(MapleFolder *folder);

        std::unique_ptr<MapleFolder> root;
};

class ListWZFile : public WZFile {
    public:
        ListWZFile(const std::string& name) : WZFile(name) {}
        ~ListWZFile() {}
    private:
        bool test;
};

#endif
