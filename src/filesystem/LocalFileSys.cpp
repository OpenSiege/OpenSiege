
#include "LocalFileSys.hpp"

#include <fstream>
#include <osgDB/FileNameUtils>

#include "cfg/IConfig.hpp"

namespace ehb
{
    bool LocalFileSys::init(IConfig& config)
    {
        log = spdlog::get("filesystem");

        installPath = config.getString("ds-install-path");
        bitsPath = config.getString("bits");

        // no install passed, fail out of init
        if (installPath.empty()) return false;

        // prevent accidental double up of params
        if (bitsPath == installPath)
        {
            log->warn("--ds-install-path and --bits path are the same - setting bits path to \"\"");
            bitsPath = "";
        }

        // lets use the same gpg verify file
        if (auto verify = createInputStream("/config/startup_verify.gpg"); verify == nullptr)
        {
            return false;
        }

        log->info("LocalFileSys has started and is primed with path [{}]", installPath.string());

        return true;
    }

    InputStream LocalFileSys::createInputStream(const std::string & filename_)
    {
        std::string filename = osgDB::convertToLowerCase(filename_);
        if (filename.front() == '/' || filename.front() == '\\')
        {
            filename.erase(0, 1);
        }

        // search bits path first if it exists
        if (fs::exists(bitsPath))
        {
            if (auto stream = std::make_unique<std::ifstream>(bitsPath / filename, std::ios_base::binary); !stream->is_open())
            {
                return stream;
            }
        }

        // fail back to the install path for the file
        if (auto stream = std::make_unique<std::ifstream>(installPath / filename, std::ios_base::binary); stream->is_open())
        {
            return stream;
        }

        return InputStream();
    }

    FileList LocalFileSys::getFiles() const
    {
        FileList result;

        // ensure bits gets loaded to the set first as they should take priority of the original game files
        if (fs::exists(bitsPath))
        {
            try
            {
                for (auto& itr : fs::recursive_directory_iterator(bitsPath))
                {
                    const auto& filename = itr.path();

                    if (fs::is_directory(filename) || fs::is_regular_file(filename))
                    {
                        auto test = osgDB::convertFileNameToUnixStyle(filename.string().substr(bitsPath.string().size()));
                        result.emplace(test);
                    }
                }
            }
            catch (std::exception& e)
            {
                log->warn("LocalFileSys::getFiles(): {}", e.what());
            }
        }

        try
        {
            for (auto & itr : fs::recursive_directory_iterator(installPath))
            {
                const auto & filename = itr.path();

                if (fs::is_directory(filename) || fs::is_regular_file(filename))
                {
                    // result.emplace(fs::relative(filename, rootDir).string());
                    auto test = osgDB::convertFileNameToUnixStyle(filename.string().substr(installPath.string().size()));
                    result.emplace(test);
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

        // ensure bits gets loaded to the set first as they should take priority of the original game files
        if (fs::exists(bitsPath))
        {
            try
            {
                for (auto& itr : fs::directory_iterator(bitsPath / directory))
                {
                    const auto& filename = itr.path();

                    if (fs::is_directory(filename) || fs::is_regular_file(filename))
                    {
                        result.emplace(osgDB::convertFileNameToUnixStyle(filename.string().substr(bitsPath.string().size())));
                    }
                }
            }
            catch (std::exception& e)
            {
                log->warn("LocalFileSys::getDirectoryContents({}): {}", directory, e.what());
            }
        }

        try
        {
            for (auto & itr : fs::directory_iterator(installPath / directory))
            {
                const auto & filename = itr.path();

                if (fs::is_directory(filename) || fs::is_regular_file(filename))
                {
                    // result.emplace(fs::relative(filename, rootDir).string());
                    result.emplace(osgDB::convertFileNameToUnixStyle(filename.string().substr(installPath.string().size())));
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
