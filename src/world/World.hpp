
#pragma once

#include "SiegePos.hpp"

#include <osg/Group>

namespace ehb
{
    class Region;

    //! Responsible for drawing debug data into the world
    class World
    {
    public:

        static void drawDebugCircle(osg::Group& scene, Region& Region, const SiegePos& pos, float radius, osg::Vec4 color, const std::string& name);

        static void drawDebugDashedLine(osg::Group& scene, Region& Region, const SiegePos& a, const SiegePos& b, osg::Vec4 color, const std::string& name);

        static void drawDebugLine(osg::Group& scene, Region& Region, const SiegePos& a, const SiegePos& b, osg::Vec4 color, const std::string& name);

        static void drawDebugScreenLabel(osg::Group& scene, const osg::Vec3& position, const std::string& text);
        
    };
}