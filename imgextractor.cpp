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

    const char* imgFileName = argv[1];

    // exit if nonexistent file
    if (!boost::filesystem::is_regular_file(imgFileName)) {
        std::cout << "Usage: ./IMGExtractor <IMGFilePath>\n";
        std::cout << "Error: File '" << imgFileName << "' does not exist\n";
        return EXIT_FAILURE_NONEXISTENT_FILE;
    }

    // store cwd and traverse to wz directory
    auto curPath = boost::filesystem::current_path();
    auto filePath = boost::filesystem::path(imgFileName);
    chdir(filePath.parent_path().c_str());

    // load file
    std::cout << filePath.filename() << '\n';
    IMGFile imgFile(filePath.filename().string());

    // restore back to original path
    chdir(curPath.c_str());

    // create directory, traverse and extract
    boost::filesystem::create_directory(maplereverence::imgExtractPath);
    chdir(maplereverence::imgExtractPath.c_str());

    // print output
    std::cout << "===== " << imgFile.getName() << " =====\n";
    imgFile.print();
    std::cout << "Extracting: " << imgFile.getName();
    std::cout << std::endl;;
    imgFile.extract();
    std::cout << std::endl;;

    return EXIT_SUCCESS;
}
