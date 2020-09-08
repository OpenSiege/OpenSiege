
#pragma once

#include <vector>
#include <osg/Group>
#include <osgDB/ReaderWriter>
#include <osg/MatrixTransform>
#include <osgText/Text>

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

        static void connect(osg::MatrixTransform* targetNode, uint32_t targetDoor, osg::MatrixTransform* connectNode, uint32_t connectDoor);

        void drawAllDoorLabels(bool on)
        {
            if (on)
            {
                if (doorLabelsGroup == nullptr)
                {
                    doorLabelsGroup = new osg::Group;

                    for (const auto& entry : doorXform)
                    {
                        auto text = new osgText::Text;
                        text->setAxisAlignment(osgText::Text::SCREEN);
                        text->setCharacterSize(1);
                        text->setText(std::to_string(entry.first));
                        text->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

                        auto doorXform = getMatrixForDoorId(entry.first);

                        auto doorMatTransform = new osg::MatrixTransform(doorXform);
                        doorMatTransform->addChild(text);

                        doorLabelsGroup->addChild(doorMatTransform);
                    }
                }

                addChild(doorLabelsGroup);

                return;
            }

            if (!on)
            {
                if (doorLabelsGroup != nullptr)
                    removeChild(doorLabelsGroup);

                return;
            }
        }

    protected:

        virtual ~SiegeNodeMesh() = default;

    private:

        const osg::Matrix getMatrixForDoorId(const uint32_t id)
        {
            for (const auto& entry : doorXform)
            {
                if (entry.first == id)
                {
                    return entry.second;
                    break;
                }
            }

            return osg::Matrix::identity();
        }

        std::vector<std::pair<uint32_t, osg::Matrix>> doorXform;
        osg::ref_ptr<osg::Group> doorLabelsGroup;
    };
}
