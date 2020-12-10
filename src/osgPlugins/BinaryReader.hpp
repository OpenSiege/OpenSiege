
#pragma once

#include <vector>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Quat>
#include <string>

namespace ehb
{
    struct FourCC final
    {
        unsigned char c0, c1, c2, c3;
    };

    bool operator == (FourCC a, FourCC b) noexcept;
    bool operator != (FourCC a, FourCC b) noexcept;

    bool operator == (FourCC a, const char fccStr[]) noexcept;
    bool operator != (FourCC a, const char fccStr[]) noexcept;

    std::ostream& operator << (std::ostream& s, FourCC fcc);

    using ByteArray = std::vector<uint8_t>;

    class BinaryReader
    {
    public:

        BinaryReader(ByteArray fileData);
        ~BinaryReader() = default;

        void readBytes(void* buffer, size_t numBytes);
        void skipBytes(const size_t numBytes);

        uint8_t readUInt8();
        uint16_t readUInt16();
        uint32_t readUInt32();
        float readFloat32();
        double readDouble();

        bool readFourCC(FourCC& fcc);

        osg::Vec2 readVec2();
        osg::Vec3 readVec3();
        osg::Quat readQuat();

        std::string readString();

    private:

        bool swap;

        size_t readPosition;
        const ByteArray fileContents;
    };
}