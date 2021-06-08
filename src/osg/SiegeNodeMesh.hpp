
#pragma once

#include <osg/Group>

namespace ehb
{
    class SiegeNodeMesh final : public osg::Group
    {
        friend class ReaderWriterSNO;

    public:

        struct Face final
        {
            osg::Vec3 a{ 0, 0, 0 }, b{ 0, 0, 0 }, c{ 0, 0, 0 }, normal{ 0, 0, 0 };
        };

        enum class FloorFlag : uint32_t
        {
            FLOOR_IGNORED = 536870912,
            FLOOR_FLOOR = 1073741825,
            FLOOR_WATER = 2147483648
        };

        struct LogicalNodeGrouping final : public osg::Group
        {
            uint8_t id = 0;
            uint32_t triangleCount = 0;

            FloorFlag flag = FloorFlag::FLOOR_IGNORED;

            osg::BoundingBox bbox;

            std::vector<Face> logicalNodeFaces;           
        };

    public:

        SiegeNodeMesh();

        // void SRequestNodeConnection( SiegeId targetNode, DWORD targetDoor, SiegeId connectNode, DWORD connectDoor, bool bConnect, bool bForceComplete )

        static void connect(osg::MatrixTransform* targetNode, uint32_t targetDoor, osg::MatrixTransform* connectNode, uint32_t connectDoor);

        virtual osg::BoundingSphere computeBound() const override;

        void toggleAllDoorLabels();

        void toggleBoundingBox();

        void toggleLogicalNodeFlags();

        //! TODO: private
        std::vector<LogicalNodeGrouping> logicalNodeGroupings;

    protected:

        virtual ~SiegeNodeMesh() = default;

    private:

        const osg::Matrix getMatrixForDoorId(const uint32_t id) const;

    private:

        std::vector<std::pair<uint32_t, osg::Matrix>> doorXform;

        /*
         * 0 = doors
         * 1 = bounding box
        */
        std::vector<osg::ref_ptr<osg::Group>> debugDrawingGroups;

        bool drawingDoorLabels = false;
        bool drawingBoundingBox = false;
        bool drawingLogicalNodeFlags = false;
    };

    inline SiegeNodeMesh::SiegeNodeMesh()
    {
        debugDrawingGroups.resize(3);
    }
}
