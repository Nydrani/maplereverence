#include <string>

#ifndef MAPLEREVERENCE_CONSTANTS
#define MAPLEREVERENCE_CONSTANTS

namespace maplereverence {
    const std::string listWZName = "List.wz";
    const std::string wzFileNames[] = {"Map.wz"};
    //const std::string wzFileNames[] = {"Base.wz", "TamingMob.wz", "Etc.wz", "String.wz"};

    const int8_t encryptionKeyUnicodeLength = 26;
    const uint16_t encryptionKeyUnicode[] = {
        26027,  1353, 52583,  2647, 31640,  2695, 26092,
        35591, 29845, 27702, 22963, 24105, 22946, 32259,
        32191, 29899, 21392, 37926, 28440, 34657, 54992,
         7801, 21164, 21225, 31362, 59422};
    // this is just the 1 byte form of the above
    const int8_t encryptionKeyAsciiLength = 52;
    /*
    const uint8_t encryptionKeyAscii[] = {
        101, 171,   5,  73, 205, 103,  10,  87, 123,
        152,  10, 135, 101, 236, 139,   7, 116, 149,
        108,  54,  89, 179,  94,  41,  89, 162, 126,
          3, 125, 191, 116, 203,  83, 144, 148,  38,
        111,  24, 135,  97, 214, 208,  30, 121,  82,
        172,  82, 233, 122, 130, 232,  30};
    */
    // v62 encryption key first 27 found
    const uint8_t encryptionKeyAscii[] = {
        150, 174,  63, 164,  72, 250, 221, 144,  70,
        118,   5,  97, 151, 206, 120, 104,  43, 160,
         68, 143, 193,  86, 126,  50, 252, 225, 245,
          3, 125, 191, 116, 203,  83, 144, 148,  38, // unknown at 3
        111,  24, 135,  97, 214, 208,  30, 121,  82,
        172,  82, 233, 122, 130, 232,  30};
}

#endif
