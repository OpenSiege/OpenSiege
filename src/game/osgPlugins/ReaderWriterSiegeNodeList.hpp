
#pragma once

#include <vector>
#include <osgDB/ReaderWriter>

#include <spdlog/spdlog.h>

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

        const std::string& resolveFileName(const std::string& filename) const;

    private:

        IFileSys& fileSys;

        // guid=0xa2010103, filename=t_cry01_cave-1c;
        std::unordered_map<std::string, std::string> meshFileNameToGuidKeyMap;

        std::shared_ptr<spdlog::logger> log;
    };

    inline const std::string& ReaderWriterSiegeNodeList::resolveFileName(const std::string& filename) const
    {
        const auto itr = meshFileNameToGuidKeyMap.find(filename);

        return itr != meshFileNameToGuidKeyMap.end() ? itr->second : filename;
    }
}
