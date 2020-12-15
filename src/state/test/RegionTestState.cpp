
#include "RegionTestState.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"
#include "osg/SiegeNodeMesh.hpp"
#include "osg/Aspect.hpp"
#include "ContentDb.hpp"
#include "world/Region.hpp"
#include "spdlog/fmt/ostr.h"

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
    struct SiegePos
    {
        uint32_t guid = 0;
        osg::Vec3 pos;

        explicit SiegePos(float x, float y, float z, uint32_t guid) : guid(guid)
        {
            pos.x() = x;
            pos.y() = y;
            pos.z() = z;
        }
    };

    // SiegePos pos = valueAsSiegePos("-1.11579,0,-3.83528,0xc4660a9d");
    SiegePos valueAsSiegePos(const std::string& value)
    {
        const auto itr1 = value.find(',');
        const auto itr2 = value.find(',', itr1 + 1);
        const auto itr3 = value.find(',', itr2 + 2);

        float x = std::stof(value.substr(0, itr1));
        float y = std::stof(value.substr(itr1 + 1, itr2 - itr1 - 1));
        float z = std::stof(value.substr(itr2 + 1, itr3 - itr2 - 1));
        unsigned int node = std::stoul(value.substr(itr3 + 1), nullptr, 16);

        return SiegePos(x, y, z, node);
    }
}

static std::ostream& operator << (std::ostream& s, const ehb::SiegePos& pos)
{
    s << "SiegePos(x: " << pos.pos.x() << ", y: " << pos.pos.y() << ", z: " << pos.pos.z() << ", guid: " << pos.guid << ")";
    return s;
}

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

            if (xform->getNumChildren() > 0)
            {
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
        }

    };

    void RegionTestState::enter()
    {
        log = spdlog::get("log");
        // log->set_level(spdlog::level::debug);

        log->info("RegionTestState::enter()");

        // variablizing these out as it helps my brain think about the world class a bit more
        const std::string worldName = "multiplayer_world";
        const std::string regionName = "town_center";

        // man it would be really nice to convert these to fuel paths...
        const std::string worldPath = "/world/maps/" + worldName;
        const std::string regionPath = worldPath + "/regions/" + regionName;

        // man it would be really nice to convert these to fuel paths...
        const std::string nodesDotGas = regionPath + "/terrain_nodes/nodes.gas";
        const std::string objectsPath = regionPath + "/objects/regular/";

        region = static_cast<Region*> (osgDB::readNodeFile(nodesDotGas));

        const osg::MatrixTransform* targetNodeXform = region->targetNode();
        uint32_t targetNodeGuid = region->targetNodeGuid();

        // TODO: is there a better way to do this?
        // re-position the camera based on the size of the target node and orient it up a little bit get a birds eye-view
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

        log->set_level(spdlog::level::debug);

        // collect all files under our object path for loading
        for (const auto& filename : { "non_interactive.gas" })
        {
            if (auto stream = fileSys.createInputStream(objectsPath + filename))
            {
                if (Fuel doc; doc.load(*stream))
                {
                    log->info("loading objects from: {}", objectsPath + filename);

                    for (const auto& node : doc.eachChild())
                    {
                        const auto tmpl = node->type();
                        const auto scid = node->name();

                        log->debug("loading {} with scid {}", tmpl, scid);

                        if (auto placement = node->child("placement"))
                        {
                            auto position = valueAsSiegePos(placement->valueOf("position"));

                            auto go = contentDb.getGameObjectTmpl(tmpl);

                            auto model = go->valueOf("aspect:model") + ".asp";

                            if (auto mesh = dynamic_cast<Aspect*>(osgDB::readNodeFile(model)); mesh != nullptr)
                            {
                                // applying skeletons causes the update thread to run away. this needs to be looked at
                                // mesh->applySkeleton();

                                auto transform = new osg::MatrixTransform;

                                transform->addChild(mesh);

                                if (auto localNode = region->transformForGuid(position.guid); localNode != nullptr)
                                {
                                    osg::Matrix copy = localNode->getMatrix();
                                    copy.preMultTranslate(position.pos);
                                    transform->setMatrix(copy);
                                }
                                else
                                {
                                    log->debug("failed to find local node for {}", tmpl);
                                }

                                region->addChild(transform);
                            }
                            else
                            {
                                log->error("unable to load model: {}", model);
                            }
                        }
                        else
                        {
                            log->debug("{} has no placement block - CRITICAL", tmpl);
                        }
                    }
                }
            }
            else
            {
                log->error("failed to load an object file under path: {}", objectsPath + filename);
            }
        }

        osgUtil::Optimizer optimizer;
        optimizer.optimize(region);

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
                log->info("enabling logical flags");

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
