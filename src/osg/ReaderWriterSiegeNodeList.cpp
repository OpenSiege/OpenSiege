
#include "ReaderWriterSiegeNodeList.hpp"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "IFileSys.hpp"

namespace ehb
{
    ReaderWriterSiegeNodeList::ReaderWriterSiegeNodeList(IFileSys& fileSys) : fileSys(fileSys)
    {
        supportsExtension("gas", "Dungeon Siege Gas");
    }

    const char* ReaderWriterSiegeNodeList::className() const {

        return "Dungeon Siege Siege Node List Reader";
    }

    bool ReaderWriterSiegeNodeList::acceptsExtension(const std::string& extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "gas");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterSiegeNodeList::readNode(const std::string& filename, const osgDB::Options* options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        return readNode(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterSiegeNodeList::readNode(std::istream& stream, const osgDB::Options* options) const
    {
        return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
    }
}
