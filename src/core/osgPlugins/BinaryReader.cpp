
#include "BinaryReader.hpp"
#include <cstring>
#include <ostream>
#include <osg/Endian>

namespace ehb
{
    bool operator == (const FourCC a, const FourCC b) noexcept
    {
        return (a.c0 == b.c0) && (a.c1 == b.c1) &&
            (a.c2 == b.c2) && (a.c3 == b.c3);
    }

    bool operator != (const FourCC a, const FourCC b) noexcept
    {
        return (a.c0 != b.c0) || (a.c1 != b.c1) ||
            (a.c2 != b.c2) || (a.c3 != b.c3);
    }

    bool operator == (const FourCC a, const char fccStr[]) noexcept
    {
        return (a.c0 == fccStr[0]) && (a.c1 == fccStr[1]) &&
            (a.c2 == fccStr[2]) && (a.c3 == fccStr[3]);
    }

    bool operator != (const FourCC a, const char fccStr[]) noexcept
    {
        return (a.c0 != fccStr[0]) || (a.c1 != fccStr[1]) ||
            (a.c2 != fccStr[2]) || (a.c3 != fccStr[3]);
    }

    std::ostream& operator << (std::ostream& s, const FourCC fcc)
    {
        s << static_cast<char>(fcc.c0) << static_cast<char>(fcc.c1)
            << static_cast<char>(fcc.c2) << static_cast<char>(fcc.c3);
        return s;
    }
}

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

    bool BinaryReader::readFourCC(FourCC& fcc)
    {
        if (readPosition == fileContents.size() || (readPosition + sizeof(FourCC)) > fileContents.size())
        {
            // out of bytes to read
            return false;
        }

        const unsigned char* dataPtr = fileContents.data() + readPosition;
        std::memcpy(&fcc, dataPtr, sizeof(FourCC));
        readPosition += sizeof(FourCC);

        return true;
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
