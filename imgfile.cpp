#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "wztool.hpp"
#include "imgfile.hpp"
#include "constants.hpp"


std::ostream& operator<<(std::ostream& os, const IMGDataType& obj) {
   os << static_cast<std::underlying_type<IMGDataType>::type>(obj);
   return os;
}

void IMGEntry::setName(const std::string& name) {
    this->name = name;
}

const std::string& IMGEntry::getName() const {
    return name;
}

void IMGEntry::setType(const IMGDataType type) {
    this->type = type;
}

IMGDataType IMGEntry::getType() const {
    return type;
}

void IMGEntry::setValue(const IMGDataVariant& value) {
    this->value = value;
}

const IMGDataVariant& IMGEntry::getValue() const {
    return value;
}

void IMGEntry::print() const {
    std::cout << name << " | " << type << '\n';
}


void IMGCategory::setByteSize(const int byteSize) {
    this->byteSize = byteSize;
}

int IMGCategory::getByteSize() const {
    return byteSize;
}

void IMGCategory::addEntry(std::unique_ptr<IMGEntry> entry) {
    entries.emplace_back(std::move(entry));
}

void IMGCategory::print() const {
    IMGEntry::print(); 
    std::cout << "Category size: " << entries.size() << " | ";
    std::cout << "Byte size: " << byteSize << '\n';
    for (const auto& entry : entries) {
        entry->print();
    }
}

const std::vector<std::unique_ptr<IMGEntry>>&
IMGCategory::getEntries() const {
    return entries;
}


void IMGFile::print() const {
}

const std::string& IMGFile::getName() const {
    return name;
}

bool IMGFile::sanityCheck() {
    if (!accessor.is_open()) {
        return false;
    }

    char firstByte = accessor.readByte();
    if (firstByte != maplereverence::imgCategoryStringByte) {
        return false;
    }

    return true;
}

void IMGFile::buildIMGStructure(IMGCategory* root) {
    parseIMGCategory(root);
}

void IMGFile::parseIMGCategory(IMGCategory* category) {
    uint8_t stringParseMethod = accessor.readUnsignedByte();
    std::string categoryType;

    if (stringParseMethod == maplereverence::imgCategoryStringByte) {
        categoryType = accessor.readEncryptedString();
    } else if (stringParseMethod == maplereverence::imgCategoryLinkByte) {
        int32_t offset = accessor.readInt();
        categoryType = accessor.readEncryptedString(offset);
    } else {
        std::string exception("Invalid string parse method: ");
        exception += std::to_string(stringParseMethod);
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }

    // update category name 
    category->setName(categoryType);

    if (categoryType == "Property") {
        // update category type
        category->setType(IMGDataType::PROPERTY);

        // padding
        accessor.readShort();

        // parse num entries
        int32_t numEntries = accessor.readCompressedInt();
        for (int i = 0; i < numEntries; ++i) {
            auto entry = std::unique_ptr<IMGEntry>(new IMGEntry());
            parseIMGEntry(entry.get());
            category->addEntry(std::move(entry));
        }

    //} else if (categoryType == "Canvas") {
    //    category->setType(IMGDataType::CANVAS);
    } else {
        std::string exception("Unsupported category type: ");
        exception += categoryType;
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}

void IMGFile::parseIMGEntry(IMGEntry* entry) {
    uint8_t stringParseMethod = accessor.readUnsignedByte();
    std::string name;

    if (stringParseMethod == 0x00) {
        name = accessor.readEncryptedString();
    } else if (stringParseMethod == 0x01) {
        int32_t offset = accessor.readInt();
        name = accessor.readEncryptedString(offset);
    } else {
        std::string exception("Invalid string parse method: ");
        exception += std::to_string(stringParseMethod);
        throw std::runtime_error(exception);
    }

    // update entry name
    entry->setName(name);

    uint8_t typeFlag = accessor.readUnsignedByte();
    if (typeFlag == 0x00) {
        entry->setType(IMGDataType::NONE);
    } else if (typeFlag == 0x02) {
        entry->setType(IMGDataType::SHORT);

        int16_t data = accessor.readShort();
        entry->setValue(data);
    //} else if (typeFlag == 0x03) {
    //} else if (typeFlag == 0x04) {
    } else {
        std::string exception("Unsupported data type flag: ");
        exception += std::to_string(typeFlag);
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}
