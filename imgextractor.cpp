#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include <unistd.h>

#include <boost/filesystem.hpp>

#include "imgfile.hpp"
#include "constants.hpp"


int main(int argc, char* argv[]) {
    // exit if incorrect num of parameters
    if (argc != 2) {
        std::cout << "Usage: ./IMGExtractor <IMGFilePath>\n";
        std::cout << "Example: ./IMGExtractor maplestory\n";
        return EXIT_FAILURE_PARAM;
    }

    bool isFolder;
    const char* imgPath = argv[1];
    std::map<std::string, std::unique_ptr<IMGFile>> files; 

    // exit if nonexistent file
    if (boost::filesystem::is_regular_file(imgPath)) {
        isFolder = false;
    } else if (boost::filesystem::is_directory(imgPath)) {
        isFolder = true;
    } else {
        std::cout << "Usage: ./WZExtractor <IMGFolderPath|IMGFilePath>\n";
        std::cout << "Error: Folder/File '" << imgPath << "' does not exist\n";
        return EXIT_FAILURE_NONEXISTENT_FILE_OR_FOLDER;
    }

    // store cwd and traverse to img directory
    auto absPath = boost::filesystem::absolute(imgPath).remove_trailing_separator();
    auto curPath = boost::filesystem::current_path();
    // sanity check filePath
    if (absPath.string().find(maplereverence::wzExtension) == std::string::npos) {
        return EXIT_FAILURE_NON_EXTRACTED_WZ_FOLDER;
    }

    // move to img path (depending on if file or folder)
    if (isFolder) {
        chdir(absPath.c_str());
    } else {
        chdir(absPath.parent_path().c_str());
    }

    // now move to root wz path
    while (boost::filesystem::current_path().string().find(maplereverence::wzExtension) != std::string::npos) {
        chdir(boost::filesystem::current_path().parent_path().c_str());
        std::cout << boost::filesystem::current_path() << '\n';
    }

    // find relative path from target folder to root folder
    auto relativePath = absPath.lexically_relative(boost::filesystem::current_path());

    // load all img in the folder
    if (isFolder) {
        // populate map with img files from folder
        boost::filesystem::directory_iterator imgIt(relativePath);
        boost::filesystem::directory_iterator endIt;
        for (; imgIt != endIt; ++imgIt) {
            const boost::filesystem::path filePath(imgIt->path());

            // skip non img files
            if (filePath.extension() != maplereverence::imgExtension) {
                continue;
            }

            files.emplace(filePath.string(), std::unique_ptr<IMGFile>(
                        new IMGFile(filePath.string())));
        }
    // otherwise just load the single img file
    } else {
        files.emplace(relativePath.string(), std::unique_ptr<IMGFile>(
                    new IMGFile(relativePath.string())));
    }

    // restore back to original path
    chdir(curPath.c_str());

    // create directory, traverse and extract
    boost::filesystem::create_directory(maplereverence::imgExtractPath);
    chdir(maplereverence::imgExtractPath.c_str());

    // print output
    for (const auto& file : files) {
        std::cout << "===== " << file.second->getName() << " =====\n";
        file.second->print();
        std::cout << "\nExtracting: " << file.second->getName();
        std::cout << std::endl;;
        file.second->extract();
    }

    return EXIT_SUCCESS;
}

