
#include "BinaryReader.hpp"
#include <cstring>
#include <osg/Endian>

namespace ehb
{
    BinaryReader::BinaryReader(ByteArray fileData) : fileContents(fileData), readPosition(0)
    {
        swap = (osg::getCpuByteOrder() == osg::BigEndian);
    }

    void BinaryReader::readBytes(void* buffer, size_t numBytes)
    {
        if (readPosition == fileContents.size() || (readPosition + numBytes) > fileContents.size())
        {
            // out of bytes to read
            return;
        }

        const uint8_t* dataPtr = fileContents.data() + readPosition;
        std::memcpy(buffer, dataPtr, numBytes);
        readPosition += numBytes;
    }

    void BinaryReader::skipBytes(const size_t numBytes)
    {
        readPosition += numBytes;
    }

    uint8_t BinaryReader::readUInt8()
    {
        uint8_t read = 0;
        readBytes(&read, sizeof(read));

        if (swap) osg::swapBytes((char *)&read, 1);

        return read;
    }

    uint16_t BinaryReader::readUInt16()
    {
        uint16_t read = 0;
        readBytes(&read, sizeof(read));

        if (swap) osg::swapBytes2((char *)&read);

        return read;
    }

    uint32_t BinaryReader::readUInt32()
    {
        uint32_t read = 0;
        readBytes(&read, sizeof(read));

        if (swap) osg::swapBytes4((char *)&read);

        return read;
    }

    float BinaryReader::readFloat32()
    {
        float read = 0.0f;
        readBytes(&read, sizeof(read));

        if (swap) osg::swapBytes4((char *)&read);

        return read;
    }

    double BinaryReader::readDouble()
    {
        double read = 0.0;
        readBytes(&read, sizeof(read));

        if (swap) osg::swapBytes8((char *)&read);

        return read;
    }

    osg::Vec2 BinaryReader::readVec2()
    {
        float read[2];
        readBytes(&read, sizeof(read));

        return osg::Vec2(read[0], read[1]);
    }
    osg::Vec3 BinaryReader::readVec3()
    {
        osg::Vec3 v;
        readBytes(&v, sizeof(osg::Vec3));
        return v;
    }
    osg::Quat BinaryReader::readQuat()
    {
        float read[4];
        readBytes(&read, sizeof(read));
        return osg::Quat(read[0], read[1], read[2], read[3]);
    }

    std::string BinaryReader::readString()
    {
        // std::getline ???

        std::string tmp;
        for (;;)
        {
            char c = '\0';
            readBytes(&c, 1);
            if (c == '\0')
            {
                break;
            }
            tmp.push_back(c);
        }

        return tmp;
    }
}
