
#pragma once

#include <vector>
#include <osg/Geode>
#include <osgDB/ReaderWriter>

#include <algorithm>

namespace ehb
{
    class IFileSys;
    class ReaderWriterSNO : public osgDB::ReaderWriter
    {
    public:

        ReaderWriterSNO(IFileSys & fileSys);

        virtual ~ReaderWriterSNO() = default;

        virtual const char * className() const override;

        virtual bool acceptsExtension(const std::string & extension) const override;

        virtual ReadResult readNode(const std::string & filename, const osgDB::Options * options) const override;

        virtual ReadResult readNode(std::istream & stream, const osgDB::Options * options) const override;

    private:

        IFileSys & fileSys;
    };

    class SiegeNodeMesh : public osg::Group
    {
        friend class ReaderWriterSNO;

    public:

        SiegeNodeMesh() = default;

        // void SRequestNodeConnection( SiegeId targetNode, DWORD targetDoor, SiegeId connectNode, DWORD connectDoor, bool bConnect, bool bForceComplete )

    protected:

        virtual ~SiegeNodeMesh() = default;

    private:

        std::vector<std::pair<unsigned int, osg::Matrix>> doorXform;
    };
}
