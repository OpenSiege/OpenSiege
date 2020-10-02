
#include "Region.hpp"

namespace ehb
{
    const osg::MatrixTransform* Region::targetNode() const
    {
        return static_cast<const osg::MatrixTransform*>(getUserDataContainer()->getUserObject(8));
    }

    const uint32_t Region::targetNodeGuid() const
    {
        /*
         * this function could also be written as
         * uint32_t targetNodeGuid; getUserValue("targetnode", targetNodeGuid);
         * the below just seems a bit more "sane" 
         */

        if (auto node = targetNode())
        {
            uint32_t guid; node->getUserValue("guid", guid);
            return guid;
        }

        return 0;
    }

    const osg::MatrixTransform* Region::transformForGuid(const uint32_t guid) const
    {
        if (auto itr = nodeMap.find(guid); itr != nodeMap.end())
        {
            return itr->second;
        }

        return nullptr;
    }

    const osg::Vec3 Region::convertFromSiegePos(const SiegePos& siegePos) const
    {
        if (siegePos.Node == 0) return {};

        if (const osg::MatrixTransform* transform = transformForGuid(siegePos.Node))
        {
            osg::Matrix copy = transform->getMatrix();
            copy.preMultTranslate(siegePos.Pos);

            return copy.getTrans();
        }

        return {};
    }

    const osg::Quat Region::convertFromSiegeRot(const SiegeRot& siegeRot) const
    {
        if (siegeRot.Node == 0) return {};

        if (const osg::MatrixTransform* transform = transformForGuid(siegeRot.Node))
        {
            osg::Matrix copy = transform->getMatrix();
            copy.preMultRotate(siegeRot.Rot);

            return copy.getRotate();
        }

        return {};
    }
}