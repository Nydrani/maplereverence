#pragma once

#include <iostream>
#include <fstream>
#include <vector>


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
        float readFloat();
        double readDouble();
        std::string readString();
        std::string readString(int length);
        int32_t readCompressedInt();
        float readCompressedFloat();
        std::string readEncryptedString();
        std::string readEncryptedString(uint32_t offset);
        std::vector<uint8_t> readData(uint32_t length);
        void seek(std::ifstream::pos_type p);
        void seek(std::ifstream::off_type off, std::ios_base::seekdir dir);
        std::ifstream::pos_type tell();
        bool is_open() const;

        std::ifstream& getStream();
    private:
        std::ifstream stream;
};

