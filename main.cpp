#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include "wzfile.hpp"
#include "constants.hpp"


int main() {
    std::map<std::string, std::unique_ptr<BasicWZFile>> files; 

    for (const auto& string : maplereverence::wzFileNames) {
        files.emplace(string, std::unique_ptr<BasicWZFile>(
                    new BasicWZFile(string)));
    }

    for (const auto& file : files) {
        file.second->print();
    }

    return 0;
}
