#include <string>

#include <boost/filesystem.hpp>


#ifndef MAPLEREVERENCE_CONSTANTS
#define MAPLEREVERENCE_CONSTANTS

namespace maplereverence {
    const std::string listWZName("List.wz");
    const std::string wzIdentifier("PKG1");
    const std::string wzExtension(".wz");
    const std::string imgExtension(".img");
    const std::string wzExtractPath("wzextracted");
    const std::string imgExtractPath("imgextracted");
    const char imgEntryStringByte = 0x73;
    const char imgEntryLinkByte = 0x1B;

    // unicode is 2 bytes of ascii joined together
    const int8_t encryptionKeyUnicodeLength = 26;
    const int8_t encryptionKeyAsciiLength = 52;

    const uint16_t encryptionKeyUnicode[] = {
        26027,  1353, 52583,  2647, 31640,  2695, 26092,
        35591, 29845, 27702, 22963, 24105, 22946, 32259,
        32191, 29899, 21392, 37926, 28440, 34657, 54992,
         7801, 21164, 21225, 31362, 59422};
    // v55 encryption key
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

    const uint8_t encryptionKeyAsciiHex[] = {
        0x96, 0xAE, 0x3F, 0xA4, 0x48, 0xFA, 0xDD, 0x90, 0x46,
        0x76, 0x05, 0x61, 0x97, 0xCE, 0x78, 0x68, 0x2B, 0xA0,
        0x44, 0x8F, 0xC1, 0x56, 0x7E, 0x32, 0xFC, 0xE1, 0xF5,

          3, 125, 191, 116, 203,  83, 144, 148,  38, // unknown at 3
        111,  24, 135,  97, 214, 208,  30, 121,  82,
        172,  82, 233, 122, 130, 232,  30};

    const std::string doubleDecrypter = "3C059309E6556D21F4C5B1D42179C0D1911BF8327FE9BEF33E2231";
    const uint8_t doubleDecryptList[] = { 
        0x3C, 0x05, 0x93, 0x09, 0xE6, 0x55, 0x6D, 0x21, 0xF4,
        0xC5, 0xB1, 0xD4, 0x21, 0x79, 0xC0, 0xD1, 0x91, 0x1B,
        0xF8, 0x32, 0x7F, 0xE9, 0xBE, 0xF3, 0x3E, 0x22, 0x31,

        0xC6, 0xBB, 0x78, 0xBC, 0x02, 0x99, 0x5B, 0x58, 0xEB, // unknown at here
        0xA1, 0xD7, 0x57, 0xB0, 0x04, 0x03, 0xCA, 0xAC, 0x84,
        0x7B, 0x8A, 0x30, 0xA0, 0x59, 0x34, 0xC3};
}

#endif
