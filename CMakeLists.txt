cmake_minimum_required (VERSION 3.7.2)
project (MapleReverence)

set (MapleReverence_VERSION_MAJOR 1)
set (MapleReverence_VERSION_MINOR 0)
set (MapleReverence_VERSION_PATCH 0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

# some weird stuff with imagemagick
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
add_definitions(-DMAGICKCORE_HDRI_ENABLE=0)


# link boost libraries
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)


add_library (WZTool wztool.cpp)
add_library (MapleAccessor mapleaccessor.cpp)
add_library (WZFile wzfile.cpp)
add_library (IMGFile imgfile.cpp)
add_library (MapleData mapledata.cpp)

# include libraries so i can build?
target_include_directories(IMGFile PRIVATE ${ImageMagick_Magick++_INCLUDE_DIR})

# link third party library shit (only to img file atm)
target_link_libraries (MapleAccessor WZTool)
target_link_libraries (IMGFile MapleData)
target_link_libraries (IMGFile ${ZLIB_LIBRARIES})
target_link_libraries (IMGFile ${ImageMagick_Magick++_LIBRARY})

add_executable (WZExtractor wzextractor.cpp)
add_executable (IMGExtractor imgextractor.cpp)


target_link_libraries (WZExtractor WZFile)
target_link_libraries (WZExtractor WZTool)
target_link_libraries (WZExtractor MapleAccessor)
target_link_libraries (WZExtractor ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (WZExtractor ${Boost_SYSTEM_LIBRARY})


target_link_libraries (IMGExtractor IMGFile)
target_link_libraries (IMGExtractor WZTool)
target_link_libraries (IMGExtractor MapleAccessor)
target_link_libraries (IMGExtractor ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (IMGExtractor ${Boost_SYSTEM_LIBRARY})

install (TARGETS WZExtractor IMGExtractor DESTINATION bin)
