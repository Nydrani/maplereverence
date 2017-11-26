#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <vector>

#include "mapleaccessor.hpp"

#ifndef MAPLEREVERENCE_IMGFILE
#define MAPLEREVERENCE_IMGFILE

class IMGCategory {
};

class IMGEntry {
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

        const std::string name;
        MapleAccessor accessor;

};

#endif
