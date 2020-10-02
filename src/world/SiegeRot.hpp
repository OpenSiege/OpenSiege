
#pragma once

#include <cstdint>
#include <osg/Quat>

namespace ehb
{
    class SiegeRot
    {
    public:

        SiegeRot() = default;

        SiegeRot(float x, float y, float z, float w, uint32_t node);

        osg::Quat Rot;
        uint32_t Node;

    };

    inline SiegeRot::SiegeRot(float x, float y, float z, float w, uint32_t node)
    {
        Rot.set(x, y, z, w);
        Node = node;
    }
}