
#pragma once

#include <osgDB/ReaderWriter>

namespace ehb
{
    class IFileSys;
    class ReaderWriterRAW : public osgDB::ReaderWriter
    {
    public:

        ReaderWriterRAW(IFileSys & fileSys);

        virtual ~ReaderWriterRAW() = default;

        virtual const char * className() const override;

        virtual bool acceptsExtension(const std::string & extension) const override;

        virtual ReadResult readImage(const std::string & filename, const osgDB::ReaderWriter::Options * options) const override;

        virtual ReadResult readImage(std::istream & stream, const osgDB::ReaderWriter::Options * options) const override;

    private:

        IFileSys & fileSys;
    };
}
