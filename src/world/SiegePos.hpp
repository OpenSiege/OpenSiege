
#pragma once

#include <cstdint>
#include <osg/Vec3>

namespace ehb
{
    class SiegePos
    {
    public:

        SiegePos() = default;

        SiegePos(float x, float y, float z, uint32_t node);

        osg::Vec3 Pos;
        uint32_t Node;

    };

    inline SiegePos::SiegePos(float x, float y, float z, uint32_t node)
    {
        Pos.set(x, y, z);
        Node = node;
    }
}