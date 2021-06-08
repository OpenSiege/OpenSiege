
#pragma once

#include <vector>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Quat>
#include <cstring>
#include <string>

namespace ehb
{
    struct FourCC final
    {
        uint8_t c0, c1, c2, c3;
    };

    bool operator == (FourCC a, FourCC b) noexcept;
    bool operator != (FourCC a, FourCC b) noexcept;

    bool operator == (FourCC a, const char fccStr[]) noexcept;
    bool operator != (FourCC a, const char fccStr[]) noexcept;

    std::ostream& operator << (std::ostream& s, FourCC fcc);

    // Length of statically allocated C++ arrays.
    template<class T, size_t N>
    constexpr size_t arrayLength(const T(&)[N]) noexcept
    {
        return N;
    }

    // Zero fills a POD type, such as a C struct or union.
    template<class T>
    void clearPodObject(T& s) noexcept
    {
        static_assert(std::is_pod<T>::value, "Type must be Plain Old Data!");
        std::memset(&s, 0, sizeof(T));
    }

    // Zero fills a statically allocated array of POD or built-in types. Array length inferred by the compiler.
    template<class T, size_t N>
    void clearArray(T(&arr)[N]) noexcept
    {
        static_assert(std::is_pod<T>::value, "Type must be Plain Old Data!");
        std::memset(arr, 0, sizeof(T) * N);
    }

    // Zero fills an array of POD or built-in types, with array length provided by the caller.
    template<class T>
    void clearArray(T* arrayPtr, const size_t arrayLength) noexcept
    {
        static_assert(std::is_pod<T>::value, "Type must be Plain Old Data!");
        assert(arrayPtr != nullptr && arrayLength != 0);
        std::memset(arrayPtr, 0, sizeof(T) * arrayLength);
    }

    using ByteArray = std::vector<uint8_t>;

    class BinaryReader final
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

        bool swap = false;

        size_t readPosition = -1;
        const ByteArray fileContents;
    };
}