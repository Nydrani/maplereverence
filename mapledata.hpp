#pragma once

#include <memory>
#include <vector>
#include <utility>

#include <boost/variant.hpp>


// @TODO unused
struct SoundIMGContainer {
        std::vector<uint8_t> header;
        std::vector<uint8_t> data;
};

// @TODO unused (meant to convert classes to single variant
using IMGTypeContainer = boost::variant<int16_t, int32_t, float, double, std::string,
    std::pair<int32_t, int32_t>, std::vector<uint8_t>,
    struct SoundIMGContainer>;


enum class IMGDataType : uint8_t {
    NONE,
    SHORT,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    PROPERTY,
    CANVAS,
    VECTOR,
    CONVEX,
    SOUND,
    UOL
};

class IMGData {
    public:
        IMGData() {}
        //IMGData(IMGDataType type) : type(type) {}
        virtual ~IMGData() {};

        // IMGDataType getType() const;

    private:
        // IMGDataType type;
};

class NoneIMGData : public IMGData {
    public:
        NoneIMGData() {}
        ~NoneIMGData() {}
};

class ShortIMGData : public IMGData {
    public:
        ShortIMGData(int16_t val) : val(val) {}
        //ShortIMGData(int16_t val) : IMGData(IMGDataType::SHORT), val(val) {}
        ~ShortIMGData() {}

        int16_t getVal() const;

    private:
        int16_t val;
};

class IntIMGData : public IMGData {
    public:
        IntIMGData(int32_t val) : val(val) {}
        //IntIMGData(int32_t val) : IMGData(IMGDataType::INT), val(val) {}
        ~IntIMGData() {}

        int32_t getVal() const;

    private:
        int32_t val;
};

class FloatIMGData : public IMGData {
    public:
        FloatIMGData(float val) : val(val) {}
        //FloatIMGData(float val) : IMGData(IMGDataType::FLOAT), val(val) {}
        ~FloatIMGData() {}

        float getVal() const;

    private:
        float val;
};

class DoubleIMGData : public IMGData {
    public:
        DoubleIMGData(double val) : val(val) {}
        //DoubleIMGData(double val) : IMGData(IMGDataType::DOUBLE), val(val) {}
        ~DoubleIMGData() {}

        double getVal() const;

    private:
        double val;
};

class StringIMGData : public IMGData {
    public:
        StringIMGData(const std::string& val) : val(val) {}
        //StringIMGData(const std::string& val) : IMGData(IMGDataType::STRING), val(val) {}
        ~StringIMGData() {}

        const std::string& getVal() const;

    private:
        std::string val;
};

class VectorIMGData : public IMGData {
    public:
        VectorIMGData(const std::pair<int32_t, int32_t>& val) : val(val) {}
        //StringIMGData(const std::string& val) : IMGData(IMGDataType::STRING), val(val) {}
        ~VectorIMGData() {}

        const std::pair<int32_t, int32_t>& getVal() const;

    private:
        std::pair<int32_t, int32_t> val;
};

class ConvexIMGData : public IMGData {
    public:
        ConvexIMGData(const std::pair<int32_t, int32_t>& val) : val(val) {}
        //StringIMGData(const std::string& val) : IMGData(IMGDataType::STRING), val(val) {}
        ~ConvexIMGData() {}

        const std::pair<int32_t, int32_t>& getVal() const;

    private:
        std::pair<int32_t, int32_t> val;
};

class CanvasIMGData : public IMGData {
    public:
        CanvasIMGData(const std::vector<uint8_t>& val, int32_t width, int32_t height, int32_t unkA, int8_t unkB)
            : val(val), width(width), height(height), unkA(unkA), unkB(unkB) {}
        //StringIMGData(const std::string& val) : IMGData(IMGDataType::STRING), val(val) {}
        ~CanvasIMGData() {}

        const std::vector<uint8_t>& getVal() const;
        int32_t getWidth() const;
        int32_t getHeight() const;
        int32_t getUnkA() const;
        int8_t getUnkB() const;


    private:
        std::vector<uint8_t> val;
        int32_t width;
        int32_t height;
        int32_t unkA;
        int8_t unkB;
};

class SoundIMGData : public IMGData {
    public:
        SoundIMGData(const std::vector<uint8_t>& header,
                const std::vector<uint8_t>& data) {
            container.header = header;
            container.data = data;
        }
        ~SoundIMGData() {}

        const std::vector<uint8_t>& getHeader() const;
        const std::vector<uint8_t>& getData() const;

    private:
        // @TODO unused for now
        struct SoundIMGContainer container;
        std::vector<uint8_t> header;
        std::vector<uint8_t> data;
};

class UOLIMGData : public IMGData {
    public:
        UOLIMGData(const std::string& val) : val(val) {}
        //UOLIMGData(const std::string& val) : IMGData(IMGDataType::STRING), val(val) {}
        ~UOLIMGData() {}

        const std::string& getVal() const;

    private:
        std::string val;
};

// output operator
std::ostream& operator<<(std::ostream&, const std::vector<uint8_t>&);
std::ostream& operator<<(std::ostream&, const std::pair<int32_t, int32_t>&);
std::ostream& operator<<(std::ostream&, IMGData*);

