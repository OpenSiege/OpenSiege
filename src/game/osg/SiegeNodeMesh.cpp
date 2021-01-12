
#include "SiegeNodeMesh.hpp"

#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/ComputeBoundsVisitor>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>

#include <spdlog/spdlog.h>

namespace ehb
{
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

    // custom compute our bounding box as what OSG does by default seems to be larger than what the actual node is
    osg::BoundingSphere SiegeNodeMesh::computeBound() const
    {
        osg::ComputeBoundsVisitor cbv;
        const_cast<SiegeNodeMesh*>(this)->accept(cbv);

        osg::BoundingSphere sphere; sphere.expandBy(cbv.getBoundingBox());
        return sphere;
    }

    void SiegeNodeMesh::connect(osg::MatrixTransform* targetNode, uint32_t targetDoor, osg::MatrixTransform* connectNode, uint32_t connectDoor)
    {
        auto log = spdlog::get("log");

        // const auto targetMesh = dynamic_cast<SiegeNodeMesh *>(targetNode->getParent(0));
        // const auto connectMesh = dynamic_cast<SiegeNodeMesh *>(connectNode->getParent(0));

        const auto targetMesh = dynamic_cast<SiegeNodeMesh*>(targetNode->getChild(0));
        const auto connectMesh = dynamic_cast<SiegeNodeMesh*>(connectNode->getChild(0));

        if (!targetMesh) { log->error("SiegeNode::connect - targetNode has no SiegeNode parent"); return; }
        if (!connectMesh) { log->error("SiegeNode::connect - connectNode has no SiegeNode parent"); return; }

        const osg::Matrix* m1 = nullptr, * m2 = nullptr;

        for (const auto& entry : targetMesh->doorXform)
        {
            if (entry.first == targetDoor)
            {
                m1 = &entry.second;
                break;
            }
        }

        if (!m1) { log->error("couldn't find targetDoor {}", targetDoor); return; }

        for (const auto& entry : connectMesh->doorXform)
        {
            if (entry.first == connectDoor)
            {
                m2 = &entry.second;
                break;
            }
        }

        if (!m2) { log->error("couldn't find connectDoor {}", connectDoor); return; }

        osg::Matrix xform;

        /*
         * lets start at the location of the destination door
         * we want to use an inverse here to account for the fact that
         * we're currently placing the center of connectNode at the location
         * of its door and will be connecting this to door 1
         */
        xform = osg::Matrix::inverse(*m2);

        // account for flipping from door 1 to door 2
        xform.postMultRotate(osg::Quat(osg::DegreesToRadians(180.0), osg::Vec3(0, 1, 0)));

        // now transform by the first door...
        xform.postMult(*m1);

        // and adjust for the node we're connecting to
        xform.postMult(targetNode->getMatrix());

        // "Hold on to your butts." - Ray Arnold
        connectNode->setMatrix(xform);
    }

    void SiegeNodeMesh::toggleAllDoorLabels()
    {
        if (!drawingDoorLabels)
        {
            if (debugDrawingGroups[0] == nullptr)
            {
                debugDrawingGroups[0] = new osg::Group;

                for (const auto& entry : doorXform)
                {
                    auto text = new osgText::Text;
                    text->setAxisAlignment(osgText::Text::SCREEN);
                    text->setCharacterSize(1);
                    text->setText(std::to_string(entry.first));
                    text->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

                    auto doorXform = getMatrixForDoorId(entry.first);

                    auto doorMatTransform = new osg::MatrixTransform(doorXform);
                    doorMatTransform->addChild(text);

                    debugDrawingGroups[0]->addChild(doorMatTransform);
                }
            }

            addChild(debugDrawingGroups[0]);

            drawingDoorLabels = true;

            return;
        }

        if (drawingDoorLabels)
        {
            if (debugDrawingGroups[0] != nullptr)
                removeChild(debugDrawingGroups[0]);

            drawingDoorLabels = false;

            return;
        }
    }

    void SiegeNodeMesh::toggleBoundingBox()
    {
        if (!drawingBoundingBox)
        {
            if (debugDrawingGroups[1] == nullptr)
            {
                debugDrawingGroups[1] = new osg::Group;

                osg::ComputeBoundsVisitor cbv;
                accept(cbv);

                debugDrawingGroups[1]->addChild(createBoxForDebug(cbv.getBoundingBox()._min, cbv.getBoundingBox()._max));
            }

            addChild(debugDrawingGroups[1]);

            drawingBoundingBox = true;

            return;
        }

        if (drawingBoundingBox)
        {
            if (debugDrawingGroups[1] != nullptr)
                removeChild(debugDrawingGroups[1]);

            drawingBoundingBox = false;

            return;
        }
    }

    void SiegeNodeMesh::toggleLogicalNodeFlags()
    {
        if (!drawingLogicalNodeFlags)
        {
            if (debugDrawingGroups[2] == nullptr)
            {
                debugDrawingGroups[2] = new osg::Group;

                for (auto& logicalFlags : logicalNodeGroupings)
                {
                    osg::Vec4 color;

                    switch (logicalFlags.flag)
                    {
                        case SiegeNodeMesh::FLOOR_FLOOR: color.set(0.0f, 1.0f, 0.0f, 0.5f); break;
                        case SiegeNodeMesh::FLOOR_IGNORED: color.set(1.0f, 0.0f, 0.0f, 0.5f); break;
                        case SiegeNodeMesh::FLOOR_WATER: color.set(0.0f, 0.0f, 1.0f, 0.5f); break;
                        default: color.set(1.0f, 1.0f, 1.0f, 1.0f); break;
                    }


                    for (auto& face : logicalFlags.logicalNodeFaces)
                    {
                        osg::Vec3Array* vertices = new osg::Vec3Array(3);
                        (*vertices)[0] = face.a;
                        (*vertices)[1] = face.b;
                        (*vertices)[2] = face.c;

                        osg::Vec4Array* colors = new osg::Vec4Array(3);
                        (*colors)[0] = color;
                        (*colors)[1] = color;
                        (*colors)[2] = color;

                        // generate our geometry for our triangles
                        osg::Geometry* geometry = new osg::Geometry;
                        geometry->setVertexArray(vertices);
                        geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
                        geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, vertices->size()));
                        geometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));
                        geometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(-1.0f, -1.0f));

                        debugDrawingGroups[2]->addChild(geometry);
                    }
                }
            }

            addChild(debugDrawingGroups[2]);

            drawingLogicalNodeFlags = true;

            return;
        }

        if (drawingLogicalNodeFlags)
        {
            if (debugDrawingGroups[2] != nullptr)
                removeChild(debugDrawingGroups[2]);

            drawingLogicalNodeFlags = false;

            return;
        }
    }

    const osg::Matrix SiegeNodeMesh::getMatrixForDoorId(const uint32_t id) const
    {
        for (const auto& entry : doorXform)
        {
            if (entry.first == id)
            {
                return entry.second;
                break;
            }
        }

        return osg::Matrix::identity();
    }
}
