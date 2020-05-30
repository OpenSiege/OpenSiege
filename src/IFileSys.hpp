
#pragma once

#include <istream>
#include <set>
#include <string>

namespace ehb
{
    typedef std::set<std::string> FileList;
    typedef std::unique_ptr<std::istream> InputStream;

    class IFileSys
    {
    public:

        virtual ~IFileSys() = default;

        virtual InputStream createInputStream(const std::string & filename) = 0;

        virtual FileList getFiles() const = 0;
        virtual FileList getDirectoryContents(const std::string & directory) const = 0;
    };
}
