#include <iostream>
#include <string>
#include <memory>
#include <utility>

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
    std::cout << name << " | " << value.get();
    if (entries.size() > 0) {
        std::cout << " | Num files: " << entries.size();
    }
    std::cout << '\n';

    for (auto& entry : entries) {
        entry->print();
    }
}

void IMGEntry::setValue(std::unique_ptr<IMGData> value) {
    this->value = std::move(value);
}

IMGData* IMGEntry::getValue() const {
    return value.get();
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
    parseIMGEntryExtended(root);
}

void IMGFile::parseIMGEntryExtended(IMGEntry* entry) {
    std::cout << "Extended: " << accessor.tell() << '\n';

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
        std::cout << "(PROPERTY) Num Entries: " << numEntries << '\n';
        for (int i = 0; i < numEntries; ++i) {
            auto newEntry = std::unique_ptr<IMGEntry>(new IMGEntry());
            parseIMGEntry(newEntry.get());
            entry->addEntry(std::move(newEntry));
        }

        entry->setValue(std::unique_ptr<NoneIMGData>(new NoneIMGData()));
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
            std::cout << "(CANVAS) Num Entries: " << numEntries << '\n';
            for (int i = 0; i < numEntries; ++i) {
                auto newEntry = std::unique_ptr<IMGEntry>(new IMGEntry());
                parseIMGEntry(newEntry.get());
                entry->addEntry(std::move(newEntry));
            }
        } else {
            std::string exception("(CANVAS) Invalid entry parse method: ");
            exception += std::to_string(canvasFlag);
            throw std::runtime_error(exception);
        }

        // @TODO seems to be some stuff
        // x, y?
        accessor.readCompressedInt();
        accessor.readCompressedInt();
        accessor.readCompressedInt();
        accessor.readByte();

        // @TODO padding?
        accessor.readInt();

        // data length
        uint32_t dataLength = accessor.readUnsignedInt();

        // padding
        accessor.readByte();

        // read data
        std::vector<uint8_t> data = accessor.readData(dataLength);

        entry->setValue(std::unique_ptr<CanvasIMGData>(new CanvasIMGData(data)));
    } else if (entryType == "Shape2D#Vector2D") {
        entry->setType(IMGDataType::VECTOR);

        int32_t x = accessor.readCompressedInt();
        int32_t y = accessor.readCompressedInt();
        std::pair<int32_t, int32_t> point(x, y);

        entry->setValue(std::unique_ptr<VectorIMGData>(new VectorIMGData(point)));
    } else if (entryType == "Shape2D#Convex2D") {
        entry->setType(IMGDataType::CONVEX);

        // parse num entries
        int32_t numEntries = accessor.readCompressedInt();
        std::cout << "(CONVEX) Num Entries: " << numEntries << '\n';
        for (int i = 0; i < numEntries; ++i) {
            auto newEntry = std::unique_ptr<IMGEntry>(new IMGEntry());
            parseIMGEntryExtended(newEntry.get());
            entry->addEntry(std::move(newEntry));
        }

        entry->setValue(std::unique_ptr<NoneIMGData>(new NoneIMGData()));
    } else if (entryType == "UOL") {
        entry->setType(IMGDataType::UOL);

        // padding
        accessor.readByte();

        // read and store string
        auto string = maplereverence::detectString(0x00, 0x01, accessor);

        entry->setValue(std::unique_ptr<UOLIMGData>(new UOLIMGData(string)));
    } else {
        std::string exception("Unsupported extended entry type: ");
        exception += entryType;
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}

void IMGFile::parseIMGEntry(IMGEntry* entry) {
    std::cout << "Entry: " << accessor.tell() << '\n';
    auto name = maplereverence::detectString(0x00, 0x01, accessor);

    // update entry name
    entry->setName(name);

    uint8_t typeFlag = accessor.readUnsignedByte();
    if (typeFlag == 0x00) {
        entry->setType(IMGDataType::NONE);

        entry->setValue(std::unique_ptr<NoneIMGData>(new NoneIMGData()));
    } else if (typeFlag == 0x02) {
        entry->setType(IMGDataType::SHORT);

        int16_t data = accessor.readShort();
        entry->setValue(std::unique_ptr<ShortIMGData>(new ShortIMGData(data)));
    } else if (typeFlag == 0x03) {
        entry->setType(IMGDataType::INT);

        int32_t data = accessor.readCompressedInt();
        entry->setValue(std::unique_ptr<IntIMGData>(new IntIMGData(data)));
    } else if (typeFlag == 0x04) {
        entry->setType(IMGDataType::FLOAT);

        float data = accessor.readCompressedFloat();
        entry->setValue(std::unique_ptr<FloatIMGData>(new FloatIMGData(data)));
    } else if (typeFlag == 0x08) {
        entry->setType(IMGDataType::STRING);

        auto data = maplereverence::detectString(0x00, 0x01, accessor);
        entry->setValue(std::unique_ptr<StringIMGData>(new StringIMGData(data)));
    } else if (typeFlag == 0x09) {
        entry->setType(IMGDataType::EXTENDED);

        uint32_t byteSize = accessor.readUnsignedInt();
        // bytesize unused for now
        (void)byteSize;

        auto newEntry = std::unique_ptr<IMGEntry>(new IMGEntry());
        parseIMGEntryExtended(newEntry.get());
        entry->addEntry(std::move(newEntry));
        entry->setValue(std::unique_ptr<NoneIMGData>(new NoneIMGData()));
    } else {
        std::string exception("Unsupported entry type flag: ");
        exception += std::to_string(typeFlag);
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}
