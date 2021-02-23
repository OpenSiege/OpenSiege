
#pragma once

#include <cstdint>
#include <osg/Vec3>

namespace ehb
{
    struct SiegePos
    {
        osg::Vec3 Pos;
        uint32_t Node;
    };
}