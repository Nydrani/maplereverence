#include <string>

#include "mapleaccessor.hpp"


#ifndef MAPLEREVERENCE_TOOLS
#define MAPLEREVERENCE_TOOLS

namespace maplereverence {
    std::string xorStringAscii(const std::string& enc);
    std::string xorStringUnicode(const std::string& enc);
    std::string detectString(uint8_t, uint8_t, MapleAccessor&);
}

#endif
