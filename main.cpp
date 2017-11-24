#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include <unistd.h>

#include <boost/filesystem.hpp>

#include "wzfile.hpp"
#include "constants.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE_PARAM 1
#define EXIT_FAILURE_NONEXISTENT_FOLDER 2


int main(int argc, char* argv[]) {
    // exit if incorrect num of parameters
    if (argc != 2) {
        std::cout << "Usage: ./MapleReverence <MaplestoryFolderPath>";
        std::cout << "Example: ./MapleReverence maplestory";
        std::cout << std::endl;
        return EXIT_FAILURE_PARAM;
    }

    const char* wzFolder = argv[1];
    std::map<std::string, std::unique_ptr<BasicWZFile>> files; 

    // exit if nonexistent folder
    if (!boost::filesystem::is_directory(wzFolder)) {
        std::cout << "Error: Folder does not exist" << std::endl;
        return EXIT_FAILURE_NONEXISTENT_FOLDER;
    }

    // populate map with wz files from folder
    boost::filesystem::directory_iterator wzIt(wzFolder);
    boost::filesystem::directory_iterator endIt;
    for (; wzIt != endIt; ++wzIt) {
        const boost::filesystem::path filePath(wzIt->path().filename());
        std::cout << filePath << '\n';

        // skip non wz files
        if (filePath.extension() != maplereverence::wzExtension) {
            continue;
        }
        
        // skip List.wz
        if (filePath.string() == maplereverence::listWZName) {
            continue;
        }

        files.emplace(filePath.string(), std::unique_ptr<BasicWZFile>(
                    new BasicWZFile(filePath.string())));
    }

    // create directory, traverse and extract
    boost::filesystem::create_directory(maplereverence::extractPath);
    chdir(maplereverence::extractPath.c_str());

    for (const auto& file : files) {
        std::cout << "===== " << file.second->getName() << " =====\n";
        file.second->print();
        std::cout << "Extracting: " << file.second->getName() << '\n';
        file.second->extract();
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
