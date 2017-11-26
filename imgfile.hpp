#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <vector>

#include "mapleaccessor.hpp"

#ifndef MAPLEREVERENCE_IMGFILE
#define MAPLEREVERENCE_IMGFILE

enum IMGDataType {
    SHORT,
    INT,
    FLOAT,
    DOUBLE,
    STRING
};

enum IMGCategoryType {
    PROPERTY,
    CANVAS,
    VECTOR,
    CONVEX,
    SOUND
};

class IMGCategory {
    public:
        IMGCategory(const std::string& name,
                const int size,
                const IMGCategoryType type)
            : name(name), size(size), type(type) {}
        ~IMGCategory() {}

    private:
        const std::string name;
        const int size;
        const IMGCategoryType type;
};

class IMGEntry {
    public:
        IMGEntry(const std::string& name,
                const IMGDataType type)
            : name(name), type(type) {}
        ~IMGEntry() {}
    private:
        const std::string name;
        const IMGDataType type;
};

class IMGFile {
    public:
        IMGFile(const std::string& name) : name(name), accessor(name) {
            if (!sanityCheck()) {
                std::cout << "Invalid Data\n";
                return;
            }

            buildIMGStructure();
        }
        ~IMGFile() {}

        void print() const;
        const std::string& getName() const;

    private:
        bool sanityCheck();
        void buildIMGStructure();
        void parseIMGCategory();
        void parseIMGEntry();

        const std::string name;
        MapleAccessor accessor;
};

#endif
