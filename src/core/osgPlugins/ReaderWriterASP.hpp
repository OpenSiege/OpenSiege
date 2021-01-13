
#pragma once

#include <osgDB/ReaderWriter>

#include "spdlog/spdlog.h"

namespace ehb
{
    class IFileSys;
    class ReaderWriterASP : public osgDB::ReaderWriter
    {
    public:

        ReaderWriterASP(IFileSys & fileSys);

        virtual ~ReaderWriterASP() = default;

        virtual const char * className() const override;

        virtual bool acceptsExtension(const std::string & extension) const override;

        virtual ReadResult readNode(const std::string & filename, const osgDB::Options * options) const override;

        virtual ReadResult readNode(std::istream & stream, const osgDB::Options * options) const override;

    private:

        IFileSys & fileSys;

        std::shared_ptr<spdlog::logger> log;
    };
}
