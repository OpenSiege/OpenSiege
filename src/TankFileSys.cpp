
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

    void TankFileSys::init(IConfig & config)
    {
    }
}
