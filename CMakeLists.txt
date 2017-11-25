cmake_minimum_required (VERSION 3.7.2)
project (MapleReverence)

set (MapleReverence_VERSION_MAJOR 1)
set (MapleReverence_VERSION_MINOR 0)
set (MapleReverence_VERSION_PATCH 0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

# link boost libraries
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_library (WZTool wztool.cpp)
add_library (WZFile wzfile.cpp)
add_library (IMGFile imgfile.cpp)

add_executable (WZExtractor wzextractor.cpp)
add_executable (IMGExtractor imgextractor.cpp)

target_link_libraries (WZExtractor WZFile)
target_link_libraries (WZExtractor WZTool)
target_link_libraries (WZExtractor ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (WZExtractor ${Boost_SYSTEM_LIBRARY})

target_link_libraries (IMGExtractor IMGFile)
target_link_libraries (IMGExtractor WZTool)
target_link_libraries (IMGExtractor ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (IMGExtractor ${Boost_SYSTEM_LIBRARY})

install (TARGETS WZExtractor IMGExtractor DESTINATION bin)
