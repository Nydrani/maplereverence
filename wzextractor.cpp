#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include <unistd.h>

#include <boost/filesystem.hpp>

#include "wzfile.hpp"
#include "constants.hpp"


int main(int argc, char* argv[]) {
    // exit if incorrect num of parameters
    if (argc != 2) {
        std::cout << "Usage: ./WZExtractor <MaplestoryFolderPath>\n";
        std::cout << "Example: ./WZExtractor maplestory\n";
        return EXIT_FAILURE_PARAM;
    }

    const char* wzFolder = argv[1];
    std::map<std::string, std::unique_ptr<BasicWZFile>> files; 

    // exit if nonexistent folder
    if (!boost::filesystem::is_directory(wzFolder)) {
        std::cout << "Usage: ./WZExtractor <MaplestoryFolderPath>\n";
        std::cout << "Error: Folder '" << wzFolder << "' does not exist\n";
        return EXIT_FAILURE_NONEXISTENT_FOLDER;
    }

    // store cwd and traverse to wz directory
    auto curPath = boost::filesystem::current_path();
    chdir(wzFolder);

    // populate map with wz files from folder
    boost::filesystem::directory_iterator wzIt(".");
    boost::filesystem::directory_iterator endIt;
    for (; wzIt != endIt; ++wzIt) {
        const boost::filesystem::path filePath(wzIt->path().filename());

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

    // restore back to original path
    chdir(curPath.c_str());

    // create directory, traverse and extract
    boost::filesystem::create_directory(maplereverence::wzExtractPath);
    chdir(maplereverence::wzExtractPath.c_str());

    for (const auto& file : files) {
        std::cout << "===== " << file.second->getName() << " =====\n";
        file.second->print();
        std::cout << "\nExtracting: " << file.second->getName();
        std::cout << std::endl;;
        file.second->extract();
    }

    return EXIT_SUCCESS;
}

