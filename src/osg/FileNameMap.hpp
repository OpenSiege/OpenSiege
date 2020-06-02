
#pragma once

#include <string>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <osgDB/Callbacks>

// NOTE: this class violates several of my design decisions, but the code was already written and we can come back around to clean it up later
namespace ehb
{
    class IFileSys;
    class FileNameMap : public osgDB::FindFileCallback
    {
    public:

        FileNameMap(IFileSys & fileSys);

        virtual ~FileNameMap() = default;

        std::string findDataFile(const std::string & filename, const osgDB::Options * options, osgDB::CaseSensitivity caseSensitivity) override;

    private:

        void parseTree(std::unordered_map<std::string, std::string>& namingKeyMap, std::istream& stream);

        std::unordered_map<std::string, std::string> keyMap;

        std::shared_ptr<spdlog::logger> log;
    };
}
