#include <string>

#include "constants.hpp"
#include "wztool.hpp"

namespace maplereverence {
    std::string xorStringAscii(const std::string& enc) {
        std::string clone = enc;
        int length = enc.length();

        for (int i = 0; i < length; ++i) {
            int keyPos = i % maplereverence::encryptionKeyAsciiLength;
            clone.at(i) ^= maplereverence::encryptionKeyAscii[keyPos];
        }

        return clone;
    }

    std::string xorStringUnicode(const std::string& enc) {
        std::string clone = enc;
        int length = enc.length();

        for (int i = 0; i < length; ++i) {
            int keyPos = i % maplereverence::encryptionKeyUnicodeLength;
            clone.at(i) ^= maplereverence::encryptionKeyUnicode[keyPos];
        }

        return clone;
    }
}
