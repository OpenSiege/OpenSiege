
#include "FileNameMap.hpp"

#include <algorithm>
#include <sstream>
#include <osgDB/FileNameUtils>
#include "IFileSys.hpp"

namespace ehb
{
    // TODO: no really, make a real nnk parser
    static std::string trim(const std::string & str)
    {
        if (!str.size()) return str;
        std::string::size_type first = str.find_first_not_of( " \t" );
        std::string::size_type last = str.find_last_not_of( "  \t\r\n" );
        if ((first==str.npos) || (last==str.npos)) return std::string( "" );
        return str.substr( first, last-first+1 );
    }

    void FileNameMap::parseTree(std::unordered_map<std::string, std::string> & namingKeyMap, std::istream & stream)
    {
        for (std::string line; std::getline(stream, line);)
        {
            line = trim(line);

            if (line.empty())
            {
                continue;
            }

            if (line.front() != '#')
            {
                std::stringstream lineStream(line);

                std::string type, key, value, description, extra;

                std::getline(lineStream, type, '=');
                std::getline(lineStream, key, ',');
                std::getline(lineStream, value, ',');
                std::getline(lineStream, description, ',');
                std::getline(lineStream, extra);

                type = trim(type);
                key = trim(key);
                value = trim(value);
                extra = trim(extra);

                if (type == "TREE")
                {
                    std::replace(value.begin(), value.end(), '\\', '/');

                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                    std::transform(extra.begin(), extra.end(), extra.begin(), ::tolower);

                    auto index = key.find_last_of('_');

                    std::string fullFileName;

                    if (index != std::string::npos)
                    {
                        auto itr = namingKeyMap.find(key.substr(0, index));

                        if (itr != namingKeyMap.end()) {

                            fullFileName += itr->second;
                            fullFileName += value;
                            fullFileName += '/';

                            namingKeyMap.emplace(key, fullFileName);

                            log->debug("storing '{}' as '{}'", key, fullFileName);

                            // take care of fighting stances
                            if (!extra.empty())
                            {
                                std::stringstream extraStream(extra);
                                std::string stance;

                                while (std::getline(extraStream, stance, ','))
                                {
                                    stance = trim(stance);

                                    namingKeyMap.emplace(key + '_' + stance, fullFileName + stance + '/');

                                    log->debug("storing '{}_{}' as '{}{}/'", key, stance, fullFileName, stance);
                                }
                            }

                            continue;
                        }
                    }

                    fullFileName += value;

                    if (!fullFileName.empty())
                    {
                        fullFileName += '/';
                    }

                    namingKeyMap.emplace(key, fullFileName);

                    log->debug("storing '{}' as '{}'", key, fullFileName);
                }
            }
        }
    }

    FileNameMap::FileNameMap(IFileSys & fileSys)
    {
        log = spdlog::get("filesystem");

        std::set<std::string> std, ext;

        for (const auto & filename : fileSys.getDirectoryContents("/art"))
        {
            if (osgDB::getLowerCaseFileExtension(filename) == "nnk")
            {
                const std::string lowerCaseFileName = osgDB::convertToLowerCase(osgDB::getSimpleFileName(filename));

                if (lowerCaseFileName.compare(0, 9, "namingkey") == 0)
                {
                    std.emplace(filename);

                    log->debug("added standard naming key file {}", filename);
                }
                else
                {
                    ext.emplace(filename);

                    log->debug("added extension naming key file {}", filename);
                }
            }
        };

        std::list<std::string> eachFileName;

        for (auto itr = std.rbegin(); itr != std.rend(); ++itr)
        {
            eachFileName.push_back(*itr);

            break;
        }

        for (auto itr = ext.begin(); itr != ext.end(); ++itr)
        {
            eachFileName.push_back(*itr);
        }

        for (const std::string & filename : eachFileName)
        {
            std::stringstream stream;

            if (auto stream = fileSys.createInputStream(filename))
            {
                // process it...
                parseTree(keyMap, *stream);
            }
        }
    }

    std::string ehb::FileNameMap::findDataFile(const std::string & filename, const osgDB::Options * options, osgDB::CaseSensitivity caseSensitivity)
    {
        std::string actualFileName = filename;

        if (filename.find_first_of('/') == std::string::npos)
        {
            std::string resolvedFileName;

            for (std::string::size_type index = filename.rfind('_'); index != std::string::npos && index != 0; index = filename.rfind('_', index - 1))
            {
                auto itr = keyMap.find(filename.substr(0, index));

                if (itr != keyMap.end())
                {
                    resolvedFileName = itr->second + filename;
                    break;
                }
            }

            if (!resolvedFileName.empty())
            {
                const std::string prefix = filename.substr(0, 2);

                if (prefix == "a_" || prefix == "b_" || prefix == "m_" || prefix == "t_")
                {
                    actualFileName.clear();

                    actualFileName += "/art/";
                    actualFileName += resolvedFileName;
                }
            }
        }

        return actualFileName;
    }
}
