#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <vector>

#include <boost/variant.hpp>

#include "mapleaccessor.hpp"

#ifndef MAPLEREVERENCE_IMGFILE
#define MAPLEREVERENCE_IMGFILE

enum class IMGDataType : uint8_t {
    NONE,
    SHORT,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    PROPERTY,
    CANVAS,
    VECTOR,
    CONVEX,
    SOUND
};

typedef boost::variant<int16_t, int32_t, double, float, std::string>
IMGDataVariant;

class IMGEntry {
    public:
        IMGEntry() {}
        virtual ~IMGEntry() {}

        void setName(const std::string&);
        const std::string& getName() const;
        void setType(const IMGDataType);
        IMGDataType getType() const;
        void setValue(const IMGDataVariant&);
        const IMGDataVariant& getValue() const;

        virtual void print() const;

    private:
        std::string name;
        IMGDataType type;
        IMGDataVariant value;
};

class IMGCategory : public IMGEntry {
    public:
        IMGCategory() : IMGEntry() {}
        ~IMGCategory() {}

        void setByteSize(const int);
        int getByteSize() const;

        void addEntry(std::unique_ptr<IMGEntry>);
        void print() const;
        const std::vector<std::unique_ptr<IMGEntry>>& getEntries() const;

    private:
        int byteSize;
        std::vector<std::unique_ptr<IMGEntry>> entries;
};

class IMGFile {
    public:
        IMGFile(const std::string& name) : name(name), accessor(name) {
            std::ios::pos_type curPos = accessor.tell();
            if (!sanityCheck()) {
                std::string exception("Invalid data at offset: ");
                exception += std::to_string(accessor.tell());
                throw std::runtime_error(exception);
            }
            accessor.seek(curPos);

            root = std::unique_ptr<IMGCategory>(new IMGCategory());

            buildIMGStructure(root.get());
        }
        ~IMGFile() {}

        const std::string& getName() const;

        void print() const;

    private:
        bool sanityCheck();
        void buildIMGStructure(IMGCategory*);
        void parseIMGCategory(IMGCategory*);
        void parseIMGEntry(IMGEntry*);

        const std::string name;
        MapleAccessor accessor;

        std::unique_ptr<IMGCategory> root;
};


std::ostream& operator<<(std::ostream& os, const IMGDataType& obj);
        
#endif
