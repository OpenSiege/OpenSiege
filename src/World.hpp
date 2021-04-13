
#pragma once

#include <string>
#include <osg/Vec3>
#include <osg/Vec4>

namespace osg
{
    class Group;
    class MatrixTransform;
}

namespace ehb
{
    class World
    {
    public:

        // static void drawDebugCircle(osg::Group& scene, WorldMap& worldMap, osg::Vec3 center, unsigned int nodeGuidA, float radius, osg::Vec4 color, const std::string& name);

        // static void drawDebugDashedLine(osg::Group& scene, WorldMap& worldMap, osg::Vec3 posA, unsigned int nodeGuidA, osg::Vec3 posB, unsigned int nodeGuidB, osg::Vec4 color, const std::string& name);

        static void drawDebugLine(osg::Group& scene, osg::Vec3 posA, const osg::MatrixTransform* xformA, osg::Vec3 posB, const osg::MatrixTransform* xformB, osg::Vec4 color, const std::string& name);

        static void drawDebugScreenLabel(osg::Group& scene, const osg::Vec3& position, const std::string& text);
    };
}
