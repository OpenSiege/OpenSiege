
#pragma once

#include "SiegePos.hpp"
#include "SiegeRot.hpp"

#include <osg/MatrixTransform>

#include <unordered_map>

namespace ehb
{
    class Region : public osg::MatrixTransform
    {
        friend class ReaderWriterSiegeNodeList;

    public:

        Region() = default;

        const osg::MatrixTransform* targetNode() const;
        const uint32_t targetNodeGuid() const;

        const osg::MatrixTransform* transformForGuid(const uint32_t guid) const;

        const osg::Vec3 convertFromSiegePos(const SiegePos& siegePos) const;
        const osg::Quat convertFromSiegeRot(const SiegeRot& siegeRot) const;

    protected:

        virtual ~Region() = default;

    private:

        // holds a mapping from the guid to the final matrix transform of the placed nodes
        std::unordered_map<uint32_t, osg::ref_ptr<osg::MatrixTransform>> nodeMap;
    };
}