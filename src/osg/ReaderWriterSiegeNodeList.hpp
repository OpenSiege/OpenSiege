
#pragma once

#include <vector>
#include <osgDB/ReaderWriter>

namespace ehb
{
    class IFileSys;
    class ReaderWriterSiegeNodeList : public osgDB::ReaderWriter
    {
    public:

        ReaderWriterSiegeNodeList(IFileSys& fileSys);

        virtual ~ReaderWriterSiegeNodeList() = default;

        virtual const char* className() const override;

        virtual bool acceptsExtension(const std::string& extension) const override;

        virtual ReadResult readNode(const std::string& filename, const osgDB::Options* options) const override;

        virtual ReadResult readNode(std::istream& stream, const osgDB::Options* options) const override;

    private:

        IFileSys& fileSys;
    };
}
