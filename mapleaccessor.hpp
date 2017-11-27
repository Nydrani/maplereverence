#include <iostream>
#include <fstream>


#ifndef MAPLEREVERENCE_MAPLEACCESSOR
#define MAPLEREVERENCE_MAPLEACCESSOR

class MapleAccessor {
    public:
        MapleAccessor(const std::string& name)
            : stream(name, std::ios::in | std::ios::binary) {}
        virtual ~MapleAccessor() {};

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
        std::string readEncryptedString(int32_t offset);
        void seek(std::ifstream::pos_type p);
        void seek(std::ifstream::off_type off, std::ios_base::seekdir dir);
        std::ifstream::pos_type tell();
        bool is_open() const;

        std::ifstream& getStream();
    private:
        std::ifstream stream;
};

#endif
