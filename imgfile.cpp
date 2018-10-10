#include <iostream>
#include <string>
#include <memory>
#include <utility>

#include <zlib.h>
#include <unistd.h>

#include "wztool.hpp"
#include "imgfile.hpp"
#include "constants.hpp"

#include "nlohmann/json.hpp"

// fuck it
using json = nlohmann::json;


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

void IMGEntry::setValue(std::unique_ptr<IMGData> value) {
    this->value = std::move(value);
}

IMGData* IMGEntry::getValue() const {
    return value.get();
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


const std::string& IMGFile::getName() const {
    return name;
}

void IMGEntry::extract(json& obj, std::string depth) {
    // update path
    if (depth.find(".") != depth.npos) {
        depth.append(".").append(name);
    } else {
        depth.append(name);
    }

    if (type == IMGDataType::NONE) {
        // do something? or nothing?
    } else if (type == IMGDataType::SHORT) {
        auto castedVal = static_cast<const ShortIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else if (type == IMGDataType::INT) {
        auto castedVal = static_cast<const IntIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else if (type == IMGDataType::FLOAT) {
        auto castedVal = static_cast<const FloatIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else if (type == IMGDataType::DOUBLE) {
        auto castedVal = static_cast<const DoubleIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else if (type == IMGDataType::STRING) {
        auto castedVal = static_cast<const StringIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else if (type == IMGDataType::PROPERTY) {
        json propertyObj = json::object();
        for (auto& entry : entries) {
            entry->extract(propertyObj, depth);
        }
        obj.emplace(name, propertyObj);
    } else if (type == IMGDataType::CANVAS) {
        json canvasObj = json::object();

        // @TODO remove raw data for now
        auto castedVal = static_cast<const CanvasIMGData*>(getValue());
        // canvasObj.emplace("CANVAS_DATA", castedVal->getVal());
        canvasObj.emplace("CANVAS_DATA", nullptr);

        // extract to ofstream
        std::cout << "width: " << castedVal->getWidth() << " height: " << castedVal->getHeight() << '\n';
        std::cout << "unka : " << castedVal->getUnkA() << " unkb: " << castedVal->getUnkB() << '\n';
        std::cout << depth << ".rgb4444\n";
        std::ofstream outStream(depth + ".rgb4444", std::ios::out | std::ios::binary);
        outStream.write(reinterpret_cast<const char*>(&castedVal->getVal()[0]), castedVal->getVal().size());

        // @TODO convert raw to png images

        for (auto& entry : entries) {
            entry->extract(canvasObj, depth);
        }

        obj.emplace(name, canvasObj);
    } else if (type == IMGDataType::VECTOR) {
        auto castedVal = static_cast<const VectorIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else if (type == IMGDataType::CONVEX) {
        json convexObj = json::object();
        for (auto& entry : entries) {
            entry->extract(convexObj, depth);
        }
        obj.emplace(name, convexObj);
    } else if (type == IMGDataType::SOUND) {
        json soundObj = json::object();

        // @TODO remove raw data for now
        auto castedVal = static_cast<const SoundIMGData*>(getValue());
        // soundObj.emplace("header", castedVal->getHeader());
        // soundObj.emplace("data", castedVal->getData());
        soundObj.emplace("header", nullptr);
        soundObj.emplace("data", nullptr);

        // extract to ofstream
        std::cout << "header: " << castedVal->getHeader() << '\n';
        std::ofstream outStream(name + ".mp3", std::ios::out | std::ios::binary);
        outStream.write(reinterpret_cast<const char*>(&castedVal->getData()[0]), castedVal->getData().size());

        obj.emplace(name, soundObj);
    } else if (type == IMGDataType::UOL) {
        auto castedVal = static_cast<const UOLIMGData*>(getValue());
        obj.emplace(name, castedVal->getVal());
    } else {
        // should error here
        std::string exception("(Extract) Invalid entry parse method: ");
        exception += std::to_string(static_cast<uint8_t>(type));
        throw std::runtime_error(exception);
    }
}

void IMGFile::extract() {
    json obj = json::object();

    // store original path
    auto curPath = boost::filesystem::current_path();

    // create directory to hold file
    boost::filesystem::path filePath(name);
    boost::filesystem::create_directories(filePath);

    // move to new path
    chdir(filePath.c_str());

    // extract into memory
    std::string rootStr;
    root->extract(obj, rootStr);

    // print at name location
    std::cout << obj.dump(2) << '\n';

    // extract to ofstream
    std::ofstream outStream(filePath.replace_extension(".json").filename().c_str(), std::ios::out);
    outStream << obj;

    // restore old path
    chdir(curPath.c_str());
}

void IMGFile::print() const {
    root->print();
}

bool IMGFile::sanityCheck() {
    if (!accessor.is_open()) {
        std::cout << "File not open!" << std::endl;
        return false;
    }

    char firstByte = accessor.readByte();
    if (firstByte != maplereverence::imgEntryStringByte) {
        std::cout << "File not IMG file!" << std::endl;
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
    // entry->setName(entryType);

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
        if (canvasFlag == 0x00) {
            // do nothing
        } else if (canvasFlag == 0x01) {
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
        int32_t width = accessor.readCompressedInt();
        int32_t height = accessor.readCompressedInt();
        int32_t compc = accessor.readCompressedInt();
        int8_t compd = accessor.readByte();
        std::cout << "canvas: a " << width << " b " << height << " c " << compc << " d " << static_cast<int32_t>(compd) << "\n";

        // @TODO padding?
        accessor.readInt();

        // data length
        uint32_t dataLength = accessor.readUnsignedInt();
        std::cout << "canvas data length " << dataLength << '\n';

        // padding
        accessor.readByte();

        // read data
        std::vector<uint8_t> data = accessor.readData(dataLength);

        entry->setValue(std::unique_ptr<CanvasIMGData>(new CanvasIMGData(data, width, height, compc, compd)));
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
    } else if (entryType == "Sound_DX8") {
        entry->setType(IMGDataType::SOUND);

        // padding
        accessor.readByte();

        // length of sound in bytes
        uint32_t dataLength = accessor.readCompressedInt();

        // length of sound in ms
        // @TODO do nothing to sound ms for now
        uint32_t soundMs = accessor.readCompressedInt();
        (void)soundMs;

        // guid header apparently is always the same?
        auto curPos = accessor.tell();
        accessor.seek(51, std::ios::cur);

        // next byte is the wav header length apparently
        uint8_t wavHeaderLength = accessor.readUnsignedByte();

        // go back and read header
        // @TODO do nothing to header for now
        accessor.seek(curPos);
        std::vector<uint8_t> header = accessor.readData(wavHeaderLength + 53);

        // read sound data
        // @TODO seems i need to read + 1 for some stupid reason
        // because it reads the current byte and misses the last byte because of that
        std::vector<uint8_t> data = accessor.readData(dataLength + 1);

        entry->setValue(std::unique_ptr<SoundIMGData>(new SoundIMGData(header, data)));
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
    } else if (typeFlag == 0x05) {
        entry->setType(IMGDataType::DOUBLE);

        double data = accessor.readDouble();
        entry->setValue(std::unique_ptr<DoubleIMGData>(new DoubleIMGData(data)));
    } else if (typeFlag == 0x08) {
        entry->setType(IMGDataType::STRING);

        auto data = maplereverence::detectString(0x00, 0x01, accessor);
        entry->setValue(std::unique_ptr<StringIMGData>(new StringIMGData(data)));
    } else if (typeFlag == 0x09) {
        uint32_t byteSize = accessor.readUnsignedInt();
        // bytesize unused for now
        (void)byteSize;

        // check extended format (folder/other stuff)
        parseIMGEntryExtended(entry);
    } else {
        std::string exception("Unsupported entry type flag: ");
        exception += std::to_string(typeFlag);
        exception += " at offset: ";
        exception += std::to_string(accessor.tell());
        throw std::runtime_error(exception);
    }
}
