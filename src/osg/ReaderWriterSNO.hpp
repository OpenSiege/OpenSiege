
#pragma once

#include <vector>
#include <osg/Group>
#include <osgDB/ReaderWriter>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/PolygonMode>

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

    // https://github.com/xarray/osgRecipes/blob/master/cookbook/chapter8/ch08_07/OctreeBuilder.cpp
    // flipped the parameters for ease of use with ds bboxes
    static osg::Group* createBoxForDebug(const osg::Vec3& min, const osg::Vec3& max)
    {
        osg::Vec3 dir = max - min;
        osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array(10);
        (*va)[0] = min + osg::Vec3(0.0f, 0.0f, 0.0f);
        (*va)[1] = min + osg::Vec3(0.0f, 0.0f, dir[2]);
        (*va)[2] = min + osg::Vec3(dir[0], 0.0f, 0.0f);
        (*va)[3] = min + osg::Vec3(dir[0], 0.0f, dir[2]);
        (*va)[4] = min + osg::Vec3(dir[0], dir[1], 0.0f);
        (*va)[5] = min + osg::Vec3(dir[0], dir[1], dir[2]);
        (*va)[6] = min + osg::Vec3(0.0f, dir[1], 0.0f);
        (*va)[7] = min + osg::Vec3(0.0f, dir[1], dir[2]);
        (*va)[8] = min + osg::Vec3(0.0f, 0.0f, 0.0f);
        (*va)[9] = min + osg::Vec3(0.0f, 0.0f, dir[2]);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(va.get());
        geom->addPrimitiveSet(new osg::DrawArrays(GL_QUAD_STRIP, 0, 10));

        osg::ref_ptr<osg::Group> group = new osg::Group;
        group->addChild(geom.get());
        group->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
        group->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        return group.release();
    }

    static osg::Group* createBoxForDebug(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
    {
        return createBoxForDebug(osg::Vec3(minX, minY, minZ), osg::Vec3(maxX, maxY, maxY));
    }

    class SiegeNodeMesh : public osg::Group
    {
        friend class ReaderWriterSNO;

    public:

        SiegeNodeMesh()
        {
            debugDrawingGroups.resize(2);
        }

        // void SRequestNodeConnection( SiegeId targetNode, DWORD targetDoor, SiegeId connectNode, DWORD connectDoor, bool bConnect, bool bForceComplete )

        static void connect(osg::MatrixTransform* targetNode, uint32_t targetDoor, osg::MatrixTransform* connectNode, uint32_t connectDoor);

        // TODO: is OSG taking care of this for us?
        const osg::BoundingBoxd& siegeBoundingBox() { return boundingBox; }
        const osg::BoundingSphered& siegeBoundingSphere() { return boundingSphere; }

        void toggleAllDoorLabels()
        {
            if (!drawingDoorLabels)
            {
                if (debugDrawingGroups[0] == nullptr)
                {
                    debugDrawingGroups[0] = new osg::Group;

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

                        debugDrawingGroups[0]->addChild(doorMatTransform);
                    }
                }

                addChild(debugDrawingGroups[0]);

                drawingDoorLabels = true;

                return;
            }

            if (drawingDoorLabels)
            {
                if (debugDrawingGroups[0] != nullptr)
                    removeChild(debugDrawingGroups[0]);

                drawingDoorLabels = false;

                return;
            }
        }

        void toggleBoundingBox()
        {
            if (!drawingBoundingBox)
            {
                if (debugDrawingGroups[1] == nullptr)
                {
                    debugDrawingGroups[1] = new osg::Group;

                    debugDrawingGroups[1]->addChild(createBoxForDebug(boundingBox._min, boundingBox._max));
                }

                addChild(debugDrawingGroups[1]);

                drawingBoundingBox = true;

                return;
            }

            if (drawingBoundingBox)
            {
                if (debugDrawingGroups[1] != nullptr)
                    removeChild(debugDrawingGroups[1]);

                drawingBoundingBox = false;

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

        osg::BoundingBoxd boundingBox;
        osg::BoundingSphered boundingSphere;
        
        /*
         * 0 = doors
         * 1 = bounding box
        */
        std::vector<osg::ref_ptr<osg::Group>> debugDrawingGroups;

        bool drawingDoorLabels = false;
        bool drawingBoundingBox = false;
    };
}
