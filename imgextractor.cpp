#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include <unistd.h>

#include <boost/filesystem.hpp>

#include "imgfile.hpp"
#include "constants.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE_PARAM 1
#define EXIT_FAILURE_NONEXISTENT_FOLDER 2


int main(int argc, char* argv[]) {
    // exit if incorrect num of parameters
    if (argc != 2) {
        std::cout << "Usage: ./IMGExtractor <IMGFolderPath>\n";
        std::cout << "Example: ./IMGExtractor maplestory\n";
        std::cout << std::flush;
        return EXIT_FAILURE_PARAM;
    }

    const char* imgFolder = argv[1];
    std::map<std::string, std::unique_ptr<IMGFile>> files; 

    // exit if nonexistent folder
    if (!boost::filesystem::is_directory(imgFolder)) {
        std::cout << "Usage: ./IMGExtractor <IMGFolderPath>\n";
        std::cout << "Error: Folder '" << imgFolder << "' does not exist\n";
        return EXIT_FAILURE_NONEXISTENT_FOLDER;
    }

    // store cwd and traverse to wz directory
    auto curPath = boost::filesystem::current_path();
    chdir(imgFolder);

    // populate map with wz files from folder
    boost::filesystem::directory_iterator imgIt(".");
    boost::filesystem::directory_iterator endIt;
    for (; imgIt != endIt; ++imgIt) {
        const boost::filesystem::path filePath(imgIt->path().filename());

        // skip non img files
        if (filePath.extension() != maplereverence::imgExtension) {
            continue;
        }
        
        std::cout << filePath << '\n';
        files.emplace(filePath.string(), std::unique_ptr<IMGFile>(
                    new IMGFile(filePath.string())));
    }

    // restore back to original path
    chdir(curPath.c_str());

    // create directory, traverse and extract
    //boost::filesystem::create_directory(maplereverence::imgExtractPath);
    //chdir(maplereverence::imgExtractPath.c_str());

    for (const auto& file : files) {
        std::cout << "===== " << file.second->getName() << " =====\n";
        file.second->print();
        std::cout << std::endl;;
    }

    return EXIT_SUCCESS;
}
