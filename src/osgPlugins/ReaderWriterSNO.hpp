
#pragma once

#include <vector>
#include <algorithm>

#include <osg/Group>
#include <osgDB/ReaderWriter>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/PolygonMode>
#include <osg/ComputeBoundsVisitor>

#include <spdlog/spdlog.h>

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

        std::shared_ptr<spdlog::logger> log;
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
}
