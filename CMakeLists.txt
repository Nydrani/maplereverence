cmake_minimum_required (VERSION 3.7.2)
project (MapleReverence)

set (MapleReverence_VERSION_MAJOR 1)
set (MapleReverence_VERSION_MINOR 0)
set (MapleReverence_VERSION_PATCH 0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")


add_library (WZFile wzfile.cpp)
add_library (WZTool wztool.cpp)

add_executable (MapleReverence main.cpp)
target_link_libraries (MapleReverence WZFile)
target_link_libraries (MapleReverence WZTool)

install (TARGETS MapleReverence DESTINATION bin)
