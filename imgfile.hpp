#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <vector>


#ifndef MAPLEREVERENCE_IMGFILE
#define MAPLEREVERENCE_IMGFILE

class IMGFile {
    public:
        IMGFile(const std::string& name)
            : name(name), stream(name, std::ios::in | std::ios::binary) {}
        virtual ~IMGFile() {};

        const std::string& getName() const;
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

        const std::string name;
        std::ifstream stream;
    private:
};

class BasicIMGFile : public IMGFile {
    public:
        BasicIMGFile(const std::string& name)
            : IMGFile(name) {
            if (!sanityCheck()) {
                std::cout << "Invalid Data\n";
                return;
            }


            buildIMGStructure();
        }
        ~BasicIMGFile() {}

        void print() const;
    protected:
        bool sanityCheck();
    private:
        void buildIMGStructure();

};

#endif
