
#pragma once

#include <istream>
#include <memory>
#include <set>
#include <string>

#include <spdlog/spdlog.h>
#include <osgDB/FileNameUtils>

#include "gas/Fuel.hpp"

namespace ehb
{
    typedef std::set<std::string> FileList;
    typedef std::unique_ptr<std::istream> InputStream;

    class IConfig;
    class IFileSys
    {
    public:

        virtual ~IFileSys() = default;

        virtual bool init(IConfig& config) = 0;

        virtual InputStream createInputStream(const std::string & filename) = 0;

        virtual FileList getFiles() const = 0;
        virtual FileList getDirectoryContents(const std::string & directory) const = 0;

        void eachGasFile(const std::string& directory, std::function<void(const std::string&, std::unique_ptr<Fuel>)> func);

        //! get the last folder or filename of the given path
        std::string filename(const std::string& path);
    };

    inline void IFileSys::eachGasFile(const std::string& directory, std::function<void(const std::string&, std::unique_ptr<Fuel>)> func)
    {
        auto log = spdlog::get("log");

        for (const auto& filename : getFiles())
        {
            if (osgDB::getLowerCaseFileExtension(filename) == "gas")
            {
                if (filename.find(directory) == 0)
                {
                    if (auto stream = createInputStream(filename))
                    {
                        if (auto doc = std::make_unique<Fuel>(); doc->load(*stream))
                        {
                            func(filename, std::move(doc));
                        }
                        else
                        {
                            log->error("{}: could not parse", filename);
                        }
                    }
                    else
                    {
                        log->error("{}: could not create input stream", filename);
                    }
                }
            }
        }
    }

    inline std::string IFileSys::filename(const std::string& path)
    {
        if (auto find = path.find_last_of("/"); find != std::string::npos)
        {
            return path.substr(find + 1, path.length());
        }

        return {};
    }
}
