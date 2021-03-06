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

    std::u16string xorStringUnicode(const std::u16string& enc) {
        std::u16string clone = enc;
        int length = enc.length();

        for (int i = 0; i < length; ++i) {
            int keyPos = i % maplereverence::encryptionKeyUnicodeLength;

            // create 2 bit mask
            uint16_t mask = maplereverence::encryptionKeyAscii[keyPos * 2];
            mask += maplereverence::encryptionKeyAscii[(keyPos * 2) + 1] << 8;

            clone.at(i) ^= mask;
        }

        return clone;
    }

    std::string detectString(uint8_t newFlag, uint8_t linkFlag, MapleAccessor& accessor) {
        uint8_t stringParseMethod = accessor.readUnsignedByte();
        std::string string;

        if (stringParseMethod == newFlag) {
            string = accessor.readEncryptedString();
        } else if (stringParseMethod == linkFlag) {
            int32_t offset = accessor.readInt();
            string = accessor.readEncryptedString(offset);
        } else {
            std::string exception("Invalid string parse method: ");
            exception += std::to_string(stringParseMethod);
            exception += " at offset: ";
            exception += std::to_string(accessor.tell());
            throw std::runtime_error(exception);
        }

        return string;
    }
}
