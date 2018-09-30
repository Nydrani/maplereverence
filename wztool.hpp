#pragma once

#include <string>

#include "mapleaccessor.hpp"


namespace maplereverence {
    std::string xorStringAscii(const std::string&);
    std::u16string xorStringUnicode(const std::u16string&);
    std::string detectString(uint8_t, uint8_t, MapleAccessor&);
}

