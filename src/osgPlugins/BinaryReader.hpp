
#pragma once

#include <vector>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Quat>
#include <string>

namespace ehb
{
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