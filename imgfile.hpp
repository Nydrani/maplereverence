#include <string>
#include <memory>
#include <vector>

#include <boost/variant.hpp>

#include "mapleaccessor.hpp"
#include "mapledata.hpp"


#ifndef MAPLEREVERENCE_IMGFILE
#define MAPLEREVERENCE_IMGFILE

class IMGEntry {
    public:
        IMGEntry() {}
        virtual ~IMGEntry() {}

        void setName(const std::string&);
        const std::string& getName() const;
        void setType(IMGDataType type);
        IMGDataType getType() const;

        virtual void addEntry(std::unique_ptr<IMGEntry>);
        const std::vector<std::unique_ptr<IMGEntry>>& getEntries() const;

        virtual void print() const;

    private:
        std::string name;
        IMGDataType type;
        std::vector<std::unique_ptr<IMGEntry>> entries;
};

class IMGValue: public IMGEntry {
    public:
        IMGValue() {}
        ~IMGValue() {}

        void setValue(std::unique_ptr<IMGData>);
        IMGData* getValue() const;
        void print() const;

    private:
        std::unique_ptr<IMGData> value;
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

            buildIMGStructure(root.get());
        }
        ~IMGFile() {}

        const std::string& getName() const;

        void print() const;

    private:
        bool sanityCheck();
        void buildIMGStructure(IMGEntry*);
        void parseIMGEntry(IMGEntry*);
        void parseIMGValue(IMGValue*);

        const std::string name;
        MapleAccessor accessor;

        std::unique_ptr<IMGEntry> root;
};


#endif
