
#include "RegionTestState.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"
#include "osg/SiegeNodeMesh.hpp"
#include "world/Region.hpp"
#include "world/World.hpp"

#include <set>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgDB/Options>
#include <osg/ComputeBoundsVisitor>
#include <osgViewer/Viewer>

#include <osgUtil/Optimizer>

namespace ehb
{
    // since we are using software instancing we have to keep track of the guids of handled nodes as to not keep flipping things on and off
    class ToggleRegionLogicalFlags : public osg::NodeVisitor
    {

    std::set<uint32_t> unique;

    public:

        ToggleRegionLogicalFlags() : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {

        }

        virtual void apply(osg::Group& group) override
        {
            osg::MatrixTransform* xform = static_cast<osg::MatrixTransform*> (&group);
            SiegeNodeMesh* mesh = dynamic_cast<SiegeNodeMesh*>(xform->getChild(0));

            if (mesh != nullptr)
            {
                uint32_t guid; xform->getUserValue("guid", guid);

                if (unique.insert(guid).second)
                {
                    mesh->toggleLogicalNodeFlags();
                }
            }

            traverse(group);
        }

    };

    void RegionTestState::enter()
    {
        log = spdlog::get("log");
        // log->set_level(spdlog::level::debug);

        log->info("RegionTestState::enter()");

        region = static_cast<Region*> (osgDB::readNodeFile("/world/maps/multiplayer_world/regions/town_center/terrain_nodes/nodes.gas"));

        const osg::MatrixTransform* targetNodeXform = region->targetNode();
        uint32_t targetNodeGuid = region->targetNodeGuid();

        // TODO: is there a better way to do this?
        // re-position the camera based on the size of node and orient it up a little bit get a birds eye-view
        if (auto manipulator = viewer.getCameraManipulator())
        {
            if (const SiegeNodeMesh* mesh = static_cast<const SiegeNodeMesh*>(targetNodeXform->getChild(0)))
            {
                double radius = osg::maximum(double(mesh->getBound().radius()), 1e-6);
                double dist = 7.f * radius;

                manipulator->setHomePosition(mesh->getBound().center() + osg::Vec3d(0.0, -dist, 15.0f), mesh->getBound().center(), osg::Vec3d(0.0f, 0.0f, 1.0f));
                manipulator->home(1);
            }
            else
            {
                log->error("failed to find siegenodemesh attached to transform with guid: {}", targetNodeGuid);
            }
        }

        osgUtil::Optimizer optimizer;
        optimizer.optimize(region);

        // 0x070316b2
        // 0xd2e1b967

        World::drawDebugLine(scene, *region, SiegePos(0, 0, 0, 0x070316b2), SiegePos(0, 0, 0, 0xd2e1b967), osg::Vec4(0, 1, 0, 1), "test");

        scene.addChild(region);
    }

    void RegionTestState::leave()
    {
    }

    void RegionTestState::update(double deltaTime)
    {
    }

    bool RegionTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        if (event.getEventType() == osgGA::GUIEventAdapter::PUSH)
        {
            if (osgViewer::View* viewer = static_cast<osgViewer::View*> (&action))
            {
                // viewer always has a valid camera no need for a check
                osg::Camera* camera = viewer->getCamera();

                osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, event.getX(), event.getY());
                osgUtil::IntersectionVisitor visitor(intersector);
                camera->accept(visitor);

                if (intersector->containsIntersections())
                {
                    for (auto&& intersection : intersector->getIntersections())
                    {
                        for (auto node : intersection.nodePath)
                        {
                            if (uint32_t nodeGuid; node->getUserValue("guid", nodeGuid))
                            {
                                if (auto nodeXform = dynamic_cast<osg::MatrixTransform*> (node))
                                {
                                    log->info("node you clicked was: 0x{:x}", nodeGuid);

                                    static_cast<SiegeNodeMesh*>(nodeXform->getChild(0))->toggleBoundingBox();
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (event.getEventType() == osgGA::GUIEventAdapter::KEYUP)
        {
            if (event.getKey() == '2')
            {
                ToggleRegionLogicalFlags visitor;
                region->accept(visitor);

                static bool runOnce = false;

                if (!runOnce)
                {
                    osgUtil::Optimizer optimizer;
                    optimizer.optimize(region);

                    runOnce = true;
                }
            }
        }

        return false;
    }
}
