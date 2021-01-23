
#include "TankFileSys.hpp"

namespace ehb
{
    InputStream TankFileSys::createInputStream(const std::string & filename_)
    {
        return nullptr;
    }

    FileList TankFileSys::getFiles() const
    {
        FileList result;
        return result;
    }

    FileList TankFileSys::getDirectoryContents(const std::string & directory_) const
    {
        FileList result;
        return result;
    }

    bool TankFileSys::init(IConfig & config)
    {
        // not implemented so just fail out
        return false;
    }
}
