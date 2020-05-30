
#include "LocalFileSys.hpp"

#include <fstream>
#include <osgDB/FileNameUtils>

namespace ehb
{
    InputStream LocalFileSys::createInputStream(const std::string & filename_)
    {
        std::string filename = osgDB::convertToLowerCase(filename_);
        if (filename.front() == '/' || filename.front() == '\\')
        {
            filename.erase(0, 1);
        }

        if (auto stream = std::make_unique<std::ifstream>(rootDir / filename, std::ios_base::binary); stream->is_open())
        {
            return stream;
        }

        return InputStream();
    }

    FileList LocalFileSys::getFiles() const
    {
        FileList result;

        try
        {
            for (auto & itr : fs::recursive_directory_iterator(rootDir))
            {
                const auto & filename = itr.path();

                if (fs::is_directory(filename) || fs::is_regular_file(filename))
                {
                    // result.emplace(fs::relative(filename, rootDir).string());
                    result.emplace(osgDB::convertFileNameToUnixStyle(filename.string().substr(rootDir.string().size())));
                }
            }
        }
        catch (std::exception & e)
        {
            log->warn("LocalFileSys::getFiles(): {}", e.what());
        }

        return result;
    }

    FileList LocalFileSys::getDirectoryContents(const std::string & directory_) const
    {
        std::string directory = osgDB::convertToLowerCase(directory_);
        if (directory.front() == '/' || directory.front() == '\\')
        {
            directory.erase(0, 1);
        }

        FileList result;

        try
        {
            for (auto & itr : fs::directory_iterator(rootDir / directory))
            {
                const auto & filename = itr.path();

                if (fs::is_directory(filename) || fs::is_regular_file(filename))
                {
                    // result.emplace(fs::relative(filename, rootDir).string());
                    result.emplace(osgDB::convertFileNameToUnixStyle(filename.string().substr(rootDir.string().size())));
                }
            }
        }
        catch (std::exception & e)
        {
            log->warn("LocalFileSys::getDirectoryContents({}): {}", directory, e.what());
        }

        return result;
    }
}
