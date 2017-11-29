#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "wztool.hpp"
#include "imgfile.hpp"
#include "constants.hpp"


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

void IMGEntry::addEntry(std::unique_ptr<IMGEntry> entry) {
    entries.emplace_back(std::move(entry));
}

const std::vector<std::unique_ptr<IMGEntry>>&
IMGEntry::getEntries() const {
    return entries;
}

void IMGEntry::print() const {
    std::cout << name << '\n';
    std::cout << "Num children: " << entries.size() << '\n';
    for (auto& entry : entries) {
        entry->print();
    }
}


void IMGValue::setValue(std::unique_ptr<IMGData> value) {
    this->value = std::move(value);
}

IMGData* IMGValue::getValue() const {
    return value.get();
}

void IMGValue::print() const {
    std::cout << value.get();
    IMGEntry::print();
}


const std::string& IMGFile::getName() const {
    return name;
}

void IMGFile::print() const {
    root->print();
}

bool IMGFile::sanityCheck() {
    if (!accessor.is_open()) {
        return false;
    }

    char firstByte = accessor.readByte();
    if (firstByte != maplereverence::imgEntryStringByte) {
        return false;
    }

    return true;
}

void IMGFile::buildIMGStructure(IMGEntry* root) {
    parseIMGEntry(root);
}

void IMGFile::parseIMGEntry(IMGEntry* entry) {
    std::string entryType = maplereverence::detectString(
            maplereverence::imgEntryStringByte,
            maplereverence::imgEntryLinkByte, accessor);

    // update entry name 
    entry->setName(entryType);

    if (entryType == "Property") {
        entry->setType(IMGDataType::PROPERTY);

        // padding
        accessor.readShort();

        // parse num entries
        int32_t numEntries = accessor.readCompressedInt();
        std::cout << "Num Entries: " << numEntries << '\n';
        for (int i = 0; i < numEntries; ++i) {
            auto entry = std::unique_ptr<IMGValue>(new IMGValue());
            parseIMGValue(entry.get());
            entry->addEntry(std::move(entry));
        }

    } else if (entryType == "Canvas") {
        entry->setType(IMGDataType::CANVAS);

        // padding
        accessor.readByte();

        uint8_t canvasFlag = accessor.readUnsignedByte();
        std::string string;
        if (canvasFlag == 0x01) {
            // padding?
            accessor.readShort();

            // parse num entries
            int32_t numEntries = accessor.readCompressedInt();
            std::cout << "Num Entries: " << numEntries << '\n';
            for (int i = 0; i < numEntries; ++i) {
                auto entry = std::unique_ptr<IMGValue>(new IMGValue());
                parseIMGValue(entry.get());
                entry->addEntry(std::move(entry));
            }
        } else {
            std::string exception("(CANVAS) Invalid string parse method: ");
            exception += std::to_string(canvasFlag);
            throw std::runtime_error(exception);
        }

        // 8 bytes
        accessor.readCompressedInt();
        accessor.readCompressedInt();
        accessor.readCompressedInt();
        accessor.readByte();
        accessor.readInt();

        // data length
        uint32_t dataLength = accessor.readUnsignedInt();
        std::cout << dataLength << '\n';

        // padding
        accessor.readByte();

        // read data
        std::vector<uint8_t> data = accessor.readData(dataLength);

        std::cout << accessor.tell() << '\n';

        // @TODO store canvas data in value
        // entry->setValue(data);
    } else if (entryType == "Shape2D#Vector2D") {
        entry->setType(IMGDataType::VECTOR);
        int32_t x = accessor.readCompressedInt();
        int32_t y = accessor.readCompressedInt();
        (void)x;
        (void)y;
        // @TODO unused x, y for now, make a point imgdata
    } else if (entryType == "UOL") {
        entry->setType(IMGDataType::UOL);

        // padding
        accessor.readByte();

        // read and store string
        auto string = maplereverence::detectString(0x00, 0x01, accessor);
    } else {
        std::string exception("Unsupported entry type: ");
        exception += entryType;
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}

void IMGFile::parseIMGValue(IMGValue* value) {
    auto name = maplereverence::detectString(0x00, 0x01, accessor);

    // update value name
    value->setName(name);

    uint8_t typeFlag = accessor.readUnsignedByte();
    if (typeFlag == 0x00) {
        value->setType(IMGDataType::NONE);
    } else if (typeFlag == 0x02) {
        value->setType(IMGDataType::SHORT);

        int16_t data = accessor.readShort();
        value->setValue(std::unique_ptr<ShortIMGData>(new ShortIMGData(data)));
    } else if (typeFlag == 0x03) {
        value->setType(IMGDataType::INT);

        int32_t data = accessor.readCompressedInt();
        value->setValue(std::unique_ptr<IntIMGData>(new IntIMGData(data)));
    } else if (typeFlag == 0x04) {
        value->setType(IMGDataType::FLOAT);

        float data = accessor.readCompressedFloat();
        value->setValue(std::unique_ptr<FloatIMGData>(new FloatIMGData(data)));
    } else if (typeFlag == 0x08) {
        value->setType(IMGDataType::STRING);

        auto data = maplereverence::detectString(0x00, 0x01, accessor);
        value->setValue(std::unique_ptr<StringIMGData>(new StringIMGData(data)));
    } else if (typeFlag == 0x09) {
        value->setType(IMGDataType::EXTENDED);

        uint32_t byteSize = accessor.readUnsignedInt();
        // bytesize unused for now
        (void)byteSize;

        auto entry = std::unique_ptr<IMGEntry>(new IMGEntry());
        parseIMGEntry(entry.get());
        value->addEntry(std::move(entry));
    } else {
        std::string exception("Unsupported data type flag: ");
        exception += std::to_string(typeFlag);
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}
