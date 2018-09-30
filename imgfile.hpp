#pragma once

#include <string>
#include <memory>
#include <vector>


#include "mapleaccessor.hpp"
#include "mapledata.hpp"

#include "nlohmann/json.hpp"


class IMGEntry {
    public:
        IMGEntry() {}
        ~IMGEntry() {}

        void setName(const std::string&);
        const std::string& getName() const;
        void setType(IMGDataType type);
        IMGDataType getType() const;
        void setValue(std::unique_ptr<IMGData>);
        IMGData* getValue() const;

        void addEntry(std::unique_ptr<IMGEntry>);
        const std::vector<std::unique_ptr<IMGEntry>>& getEntries() const;

        void extract(nlohmann::json&);
        void print() const;

    private:
        std::string name;
        IMGDataType type;
        std::unique_ptr<IMGData> value;
        std::vector<std::unique_ptr<IMGEntry>> entries;
};


class IMGFile {
    public:
        IMGFile(const std::string& name) : name(name), accessor(name) {
            std::ios::pos_type curPos = accessor.tell();
            if (!sanityCheck()) {
                std::string exception("Invalid data at offset: ");
                exception += std::to_string(accessor.tell());
                throw std::runtime_error(exception);
            }
            accessor.seek(curPos);

            root = std::unique_ptr<IMGEntry>(new IMGEntry());
            root->setName(name);

            buildIMGStructure(root.get());
        }
        ~IMGFile() {}

        const std::string& getName() const;

        void extract();
        void print() const;

    private:
        bool sanityCheck();
        void buildIMGStructure(IMGEntry*);
        void parseIMGEntryExtended(IMGEntry*);
        void parseIMGEntry(IMGEntry*);

        const std::string name;
        MapleAccessor accessor;

        std::unique_ptr<IMGEntry> root;
};

