
#pragma once

#include <osgDB/ReaderWriter>

namespace ehb
{
    class IFileSys;
    class FuelBlock;
    class Shell;
    class Widget;
    class ReaderWriterUI : public osgDB::ReaderWriter
    {
    public:

        ReaderWriterUI(IFileSys & fileSys, Shell & shell);

        virtual ~ReaderWriterUI() = default;

        virtual const char * className() const override;

        virtual bool acceptsExtension(const std::string & extension) const override;

        virtual ReadResult readNode(const std::string & filename, const osgDB::Options * options) const override;

        virtual ReadResult readNode(std::istream & stream, const osgDB::Options * options) const override;

    private:

        Widget * readWidget(const FuelBlock * node) const;

    private:

        IFileSys & fileSys;
        Shell & shell;
    };
}
