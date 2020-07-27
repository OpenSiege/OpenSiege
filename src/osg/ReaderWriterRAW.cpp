
#include "ReaderWriterRAW.hpp"

#include <osg/Endian>
#include <osg/Image>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include "IFileSys.hpp"

namespace ehb
{
    static constexpr uint32_t RAW_MAGIC = 0x52617069;
    static constexpr uint32_t RAW_FORMAT_8888 = 0x38383838;

    ReaderWriterRAW::ReaderWriterRAW(IFileSys & fileSys) : fileSys(fileSys)
    {
        supportsExtension("raw", "Dungeon Siege RAW Image Format");
    }

    const char * ReaderWriterRAW::className() const {

        return "Dungeon Siege RAW Image Reader";
    }

    bool ReaderWriterRAW::acceptsExtension(const std::string & extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "raw");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterRAW::readImage(const std::string & filename, const osgDB::ReaderWriter::Options * options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        return readImage(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterRAW::readImage(std::istream & stream, const osgDB::ReaderWriter::Options * options) const
    {
        const bool bSwap = (osg::getCpuByteOrder() == osg::BigEndian);

        uint32_t magic, format;
        uint16_t flags, surfaceCount, width, height;

        stream.read((char *)&magic, sizeof(uint32_t));
        stream.read((char *)&format, sizeof(uint32_t));
        stream.read((char *)&flags, sizeof(uint16_t));
        stream.read((char *)&surfaceCount, sizeof(uint16_t));
        stream.read((char *)&width, sizeof(uint16_t));
        stream.read((char *)&height, sizeof(uint16_t));

        if (bSwap)
        {
            osg::swapBytes4((char *)&magic);
            osg::swapBytes4((char *)&format);
            osg::swapBytes2((char *)&flags);
            osg::swapBytes2((char *)&surfaceCount);
            osg::swapBytes2((char *)&width);
            osg::swapBytes2((char *)&height);
        }

        if (magic != RAW_MAGIC)
        {
            return ReadResult::FILE_NOT_HANDLED;
        }

        // TODO: handle the different possible formats available here
        if (format != RAW_FORMAT_8888)
        {
            return ReadResult::FILE_NOT_HANDLED;
        }

        // with the 8888 format each pixel is 4 bytes
        const uint32_t size = static_cast<uint32_t>(width) * static_cast<uint32_t>(height);

        uint32_t * data = new (std::nothrow) uint32_t[size];

        if (data != nullptr)
        {
            stream.read(static_cast<char *>(static_cast<void *>(data)), sizeof(uint32_t) * size);

            osg::ref_ptr<osg::Image> image = new osg::Image;

            image->setImage(width, height, 1,
                GL_RGBA,
                GL_BGRA,
                GL_UNSIGNED_BYTE,
                static_cast<unsigned char *>(static_cast<void *>(data)),
                osg::Image::USE_NEW_DELETE);

            image->flipVertical();

            return image.release();
        }

        return ReadResult::INSUFFICIENT_MEMORY_TO_LOAD;
    }
}
