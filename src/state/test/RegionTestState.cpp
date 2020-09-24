
#include "RegionTestState.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"
#include "osg/ReaderWriterSNO.hpp"

#include <set>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgDB/Options>
#include <osg/ComputeBoundsVisitor>
#include <osgViewer/Viewer>

namespace ehb
{
    void RegionTestState::enter()
    {
        log = spdlog::get("log");
        log->set_level(spdlog::level::debug);

        log->info("RegionTestState::enter()");

        auto region = osgDB::readNodeFile("/world/maps/multiplayer_world/regions/town_center/terrain_nodes/nodes.gas");
        uint32_t targetNodeGuid = 0; region->getUserValue("targetnode", targetNodeGuid);

        osg::MatrixTransform* targetNodeXform = dynamic_cast<osg::MatrixTransform *>(region->getOrCreateUserDataContainer()->getUserObject(8));

        // TODO: is there a better way to do this?
        // re-position the camera based on the size of node and orient it up a little bit get a birds eye-view
        if (auto manipulator = viewer.getCameraManipulator())
        {
            if (SiegeNodeMesh* mesh = static_cast<SiegeNodeMesh*>(targetNodeXform->getChild(0)))
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

        scene.addChild(region);

        #if 0
        // use std::filesystem?
        static const std::string mapsFolder = "/world/maps/";
        static const std::string worldName = "multiplayer_world";
        static const std::string regionName = "town_center";

        static const std::string fullRegionPath = mapsFolder + worldName + "/regions/" + regionName;
        static const std::string mainDotGas = fullRegionPath + "/main.gas";
        static const std::string nodesDotGas = fullRegionPath + "/terrain_nodes/nodes.gas";

        log->debug("attempting to load region @ path {}", mainDotGas);

        osg::ref_ptr<osg::Group> regionGroup = new osg::Group;

        scene.addChild(regionGroup);

        // main.gas
        if (auto stream = fileSys.createInputStream(mainDotGas))
        {
            if (Fuel doc; doc.load(*stream))
            {
                // this is mandatory but i'm not sure all the fields under it are required
                if (auto child = doc.child("region"))
                {
                    std::string created = child->valueOf("created");
                    std::string description = child->valueOf("description");
                    std::string guid = child->valueOf("guid");
                    std::string lastmodified = child->valueOf("lastmodified");
                    std::string notes = child->valueOf("notes");
                    std::string scid_range = child->valueOf("scid_range");
                    std::string se_version = child->valueOf("se_version");

                    // hold onto our region guid for lookup in the future
                    regionGroup->setName(guid);
                }
            }
        }
        else
        {
            log->error("failed to load region @ {}", mainDotGas);

            return;
        }

        log->debug("attempting to load nodesDotGas @ path {}", nodesDotGas);



        // nodes.gas
        if (auto stream = fileSys.createInputStream(nodesDotGas))
        {

        }
        else
        {
            log->error("failed to load nodesDotGas @ {}", nodesDotGas);
        }

        #endif
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
                                if (auto xform = dynamic_cast<osg::MatrixTransform*> (node))
                                {
                                    log->info("node you clicked was: 0x{:x}", nodeGuid);

                                    static_cast<SiegeNodeMesh*>(xform->getChild(0))->toggleBoundingBox();
                                }
                            }
                        }
                    }
                }
            }
        }

        return false;
    }
}
