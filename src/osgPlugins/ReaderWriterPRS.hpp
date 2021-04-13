
#pragma once

#include <osgDB/ReaderWriter>

namespace ehb
{
    class IFileSys;
    class ReaderWriterPRS : public osgDB::ReaderWriter
    {
    public:

        ReaderWriterPRS(IFileSys & fileSys);

        virtual ~ReaderWriterPRS() = default;

        virtual const char * className() const override;

        virtual bool acceptsExtension(const std::string & extension) const override;

        virtual ReadResult readObject(const std::string & filename, const osgDB::Options * options) const override;

        virtual ReadResult readObject(std::istream & stream, const osgDB::Options * options) const override;

    private:

        IFileSys & fileSys;
    };
}
