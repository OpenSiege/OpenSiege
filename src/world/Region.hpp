
#pragma once

#include <osg/MatrixTransform>

#include <unordered_map>

namespace ehb
{
    class Region final : public osg::MatrixTransform
    {
        friend class ReaderWriterSiegeNodeList;

    public:

        Region() = default;

        osg::MatrixTransform* targetNode;
        uint32_t targetNodeGuid = 0;
#if 0
        const osg::MatrixTransform* targetNode() const;
        const uint32_t targetNodeGuid() const;
#endif

        const osg::MatrixTransform* transformForGuid(const uint32_t guid) const;

    protected:

        virtual ~Region() = default;

    private:

        // holds a mapping from the guid to the final matrix transform of the placed nodes
        std::unordered_map<uint32_t, osg::ref_ptr<osg::MatrixTransform>> nodeMap;
    };
}