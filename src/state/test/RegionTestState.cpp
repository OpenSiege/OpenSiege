
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
#include <osg/AnimationPath>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TrackballManipulator>

#include <osgUtil/Optimizer>

namespace osg
{
    static std::ostream& operator << (std::ostream& s, const osg::Vec3& pos)
    {
        s << "osg::Vec3(x: " << pos.x() << ", y: " << pos.y() << ", z: " << pos.z() << ")";

        return s;
    }
}

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

    using Scid = uint32_t;

    // TODO: skrit object
    struct DevPathPoint : public osg::Referenced
    {
        Scid next_scid = 0;
        std::string path_name;
        float radius = 0.0f;
        osg::Matrix finalPlacement;
    };

    using ScidToDevPointMap = std::map<Scid, osg::ref_ptr<DevPathPoint>>;

    // scid
    ScidToDevPointMap devPathPoints;

    osg::ref_ptr<osg::AnimationPath> resolveDevPathBasedOnName(const std::string& path_name, Region *region)
    {
        auto log = spdlog::get("log");

        if (devPathPoints.empty())
        {
            log->error("No dev path points available for resolution of '{}'", path_name);

            return {};
        }

        ScidToDevPointMap resolvedDevPathPoints;

        // iterate all the points and store a new map
        // probably not necessary
        for (auto pathPoint : devPathPoints)
        {
            // make sure we belong to this path
            if (pathPoint.second->path_name == path_name)
            {
                resolvedDevPathPoints.emplace(pathPoint);
            }
        }

        log->info("resolvedPathPoint count {}", resolvedDevPathPoints.size());

        osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

        // setup our starting path point
        Scid startingPoint = 0;

        // make sure we don't hit duplicates when placing the path
        std::set<Scid> completeSet;

        // recursive function to setup dev path points
        std::function<void(const Scid)> func = [&func, &path, &region, &resolvedDevPathPoints, &completeSet](const Scid scid)
        {
            for (auto pathPoint : resolvedDevPathPoints)
            {
                if (completeSet.insert(scid).second)
                {
                    const osg::ref_ptr<DevPathPoint> this_control_point = resolvedDevPathPoints.at(scid);
                    osg::ref_ptr<DevPathPoint> next_control_point = nullptr; // if next_scid is 0 then this will stay null and we are at the end of the path
                    
                    if (this_control_point->next_scid != 0)
                        next_control_point = resolvedDevPathPoints.at(this_control_point->next_scid);

                    // decompose our global position for control points
                    osg::Vec3 this_translation;
                    osg::Quat this_rotation;
                    this_control_point->finalPlacement.decompose(this_translation, this_rotation, osg::Vec3(), osg::Quat());

                    // offset so we're not in the terrain
                    this_translation.set(this_translation.x(), this_translation.y() + 5, this_translation.z());

                    // don't apply rotation yet because we want a nice straight line
                    double deltaTime = 3;
                    auto size = path->getTimeControlPointMap().size();
                    path->insert(deltaTime * size, osg::AnimationPath::ControlPoint(this_translation));

                    spdlog::get("log")->info("starting path point @ {}", this_translation);

                    // add the next control point without rotation which we will duplicate and add the rotation to
                    // 75% of path taken up by the point to point without rotation
                    if (next_control_point != nullptr)
                    {
                        // decompose our global position for control points
                        osg::Vec3 next_translation;
                        osg::Quat next_rotation;
                        next_control_point->finalPlacement.decompose(next_translation, next_rotation, osg::Vec3(), osg::Quat());

                        // offset so we're not in the terrain
                        next_translation.set(next_translation.x(), next_translation.y() + 5, this_translation.z());

                        double remainingDeltaTime = deltaTime * .25;
                        path->insert(remainingDeltaTime * size, osg::AnimationPath::ControlPoint(next_translation));
                        spdlog::get("log")->info("inserting duplicate next point @ delta time {} @ position {}", remainingDeltaTime * size, next_translation);
                    }
                     
                    auto transform = new osg::MatrixTransform;
                    transform->setMatrix(this_control_point->finalPlacement);
                    auto mesh = dynamic_cast<Aspect*>(osgDB::readNodeFile("m_i_glb_object-waypoint.asp"));
                    transform->addChild(mesh);

                    region->addChild(transform);

                    if (completeSet.count(resolvedDevPathPoints.at(scid)->next_scid) == 0)
                    {
                        // 0 denotes end of path?
                        if (resolvedDevPathPoints.at(scid)->next_scid != 0)
                            func(resolvedDevPathPoints.at(scid)->next_scid);
                    }
                    else
                    {
                        spdlog::get("log")->error("{} error", resolvedDevPathPoints.at(scid)->next_scid);
                    }
                }
            }
        };

        func(resolvedDevPathPoints.begin()->first);

        log->info("{} path points resolved", path->getTimeControlPointMap().size());

        return path;
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
            if (osg::MatrixTransform* xform = static_cast<osg::MatrixTransform*> (&group))
            {
                if (xform->getNumChildren() > 0)
                {
                    if (SiegeNodeMesh* mesh = dynamic_cast<SiegeNodeMesh*>(xform->getChild(0)))
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
        }
    };

    void RegionTestState::enter()
    {
        log = spdlog::get("log");
        // log->set_level(spdlog::level::debug);

        log->info("RegionTestState::enter()");

        // variablizing these out as it helps my brain think about the world class a bit more
#if 0
        const std::string worldName = "multiplayer_world";
        const std::string regionName = "town_center";
#else
        const std::string worldName = "opensiege";
        const std::string regionName = "r1";
#endif

        // man it would be really nice to convert these to fuel paths...
        const std::string worldPath = "/world/maps/" + worldName;
        const std::string regionPath = worldPath + "/regions/" + regionName;

        // man it would be really nice to convert these to fuel paths...
        const std::string nodesDotGas = regionPath + "/terrain_nodes/nodes.gas";
        //const std::string objectsPath = regionPath + "/objects/regular/";
        const std::string objectsPath = regionPath + "/objects/";

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

        // collect all files under our object path for loading
#if 0
        for (const auto& filename : { "non_interactive.gas", "actor.gas" })
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
                            auto orientation = placement->valueAsQuat("orientation");

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
                                    copy.preMultRotate(orientation);
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
#endif

        /*
[t:dev_path_point,n:0x03200b91]
{
    [dev_path_point]
    {
      x next_scid = 0x03200BB6;
        path_name = "zdistance_sign_1_river_forest";
      f radius = 15;
    }
    [placement]
    {
      p position = -1.40903,0,1.30428,0x090aa818;
    }
}

[t:dev_path_point,n:0x03200bb6]
{
    [dev_path_point]
    {
      x next_scid = 0x03200BCF;
        path_name = "zdistance_sign_1_river_forest";
      f radius = 15;
    }
    [placement]
    {
      q orientation = 0,-0.707107,0,0.707107;
      p position = -1.10753,0,1.3217,0x43c87f4a;
    }
}
        
        */
#if 1
        if (InputStream testStream = fileSys.createInputStream(objectsPath + "test.gas"); testStream != nullptr)
        {
            if (Fuel doc; doc.load(*testStream))
            {
                auto go = contentDb.getGameObjectTmpl("dev_path_point");

                if (go != nullptr)
                {
                    // dev_path_point is a gizmo
                    auto model = go->valueOf("gizmo:model") + ".asp";
                    auto mesh = dynamic_cast<Aspect*>(osgDB::readNodeFile("m_i_glb_object-waypoint.asp"));

                    /*
                    * let do this in 2 passes:
                    * pass 1: position all path points
                    * pass 2: orient path points - if there is a orientation then just set it, if there is no orientation get the previous point to look at the current point
                    */

                    // first pass
                    for (auto dev_path_point : doc.eachChild())
                    {
                        // dev_path_point->write(std::cout);

                        auto my_scid = std::stoul(dev_path_point->name(), nullptr, 16);
                        auto position = valueAsSiegePos(dev_path_point->valueOf("placement:position"));

                        auto dev_path_point_component = dev_path_point->child("dev_path_point");
                        osg::ref_ptr<DevPathPoint> point = new DevPathPoint;
                        point->next_scid = dev_path_point_component->valueAsUInt("next_scid");
                        point->path_name = dev_path_point_component->valueAsString("path_name");
                        point->radius = dev_path_point_component->valueAsFloat("radius");

                        devPathPoints.emplace(my_scid, point);

                        if (auto localNode = region->transformForGuid(position.guid); localNode != nullptr)
                        {
                            osg::Matrix copy = localNode->getMatrix();
                            copy.preMultTranslate(position.pos);

                            point->finalPlacement = copy;
                        }
                        else
                        {
                            // log->debug("failed to find local node for scud {}", tmpl);

                        }
                    }

                    // first pass
                    for (auto dev_path_point : doc.eachChild())
                    {
                        // dev_path_point->write(std::cout);

                        static osg::Quat defaultValue = { 0.0f, 0.0f, 0.0f, 1.f };

                        auto my_scid = std::stoul(dev_path_point->name(), nullptr, 16);
                        auto orientation = dev_path_point->valueAsQuat("placement:orientation");

                        auto point = devPathPoints[my_scid];

                        if (orientation == defaultValue)
                        {
                            if (point->next_scid != 0)
                            {
                                auto next_point = devPathPoints[point->next_scid];

                                osg::Vec3 point_pos, next_point_pos;
                                point->finalPlacement.decompose(point_pos, osg::Quat(), osg::Vec3(), osg::Quat());
                                next_point->finalPlacement.decompose(next_point_pos, osg::Quat(), osg::Vec3(), osg::Quat());

                                osg::Quat rotation;
                                rotation.makeRotate(point_pos, next_point_pos);

                                point->finalPlacement.preMultRotate(rotation);
                            }
                        }

                        int foo = 55;

#if 0
                        osg::ref_ptr<DevPathPoint> point = new DevPathPoint;
                        point->next_scid = dev_path_point_component->valueAsUInt("next_scid");
                        point->path_name = dev_path_point_component->valueAsString("path_name");
                        point->radius = dev_path_point_component->valueAsFloat("radius");

                        devPathPoints.emplace(my_scid, point);

                        if (auto localNode = region->transformForGuid(position.guid); localNode != nullptr)
                        {
                            osg::Matrix copy = localNode->getMatrix();
                            copy.preMultTranslate(position.pos);

                            point->finalPlacement = copy;
                        }
                        else
                        {
                            // log->debug("failed to find local node for scud {}", tmpl);
                        }
#endif
                    }
#if 0
                    for (auto dev_path_point : doc.eachChild())
                    {
                        // dev_path_point->write(std::cout);

                        auto my_scid = std::stoul(dev_path_point->name(), nullptr, 16);

                        auto placement = dev_path_point->child("placement");
                        auto position = valueAsSiegePos(placement->valueOf("position"));
                        auto orientation = placement->valueAsQuat("orientation");                        

                        auto dev_path_point_component = dev_path_point->child("dev_path_point");
                        osg::ref_ptr<DevPathPoint> point = new DevPathPoint;
                        point->next_scid = dev_path_point_component->valueAsUInt("next_scid");
                        point->path_name = dev_path_point_component->valueAsString("path_name");
                        point->radius = dev_path_point_component->valueAsFloat("radius");

                        devPathPoints.emplace(my_scid, point);

                        if (auto localNode = region->transformForGuid(position.guid); localNode != nullptr)
                        {
                            osg::Matrix copy = localNode->getMatrix();
                            copy.preMultTranslate(position.pos);
                            copy.preMultRotate(orientation);

                            point->finalPlacement = copy;
                        }
                        else
                        {
                        // log->debug("failed to find local node for scud {}", tmpl);
                       
                    }
#endif

                    // stores only the dev path points for the given name
                    auto osgAnimationPath = resolveDevPathBasedOnName("test_dev_path", region);
                    auto manipulator = new osgGA::AnimationPathManipulator(osgAnimationPath);

                    viewer.setCameraManipulator(manipulator);

                    auto cameraMesh = dynamic_cast<Aspect*>(osgDB::readNodeFile("m_i_glb_object-camera.asp"));
                    auto cameraXform = new osg::MatrixTransform;
                    cameraXform->addChild(cameraMesh);

                    currentPath = osgAnimationPath;
                    apcb->setAnimationPath(osgAnimationPath);
                    cameraXform->setUpdateCallback(apcb.get());

                    region->addChild(cameraXform);
                }
                else
                {
                log->error("attempting to visualize path points but 'dev_path_point' is not available");
                }
            }
        }
#endif

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
            if (event.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
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

                                        if (selectedSiegeNode != nullptr)
                                        {
                                            selectedSiegeNode->toggleBoundingBox();
                                            selectedSiegeNode->toggleAllDoorLabels();
                                        }

                                        if (selectedSiegeNode = static_cast<SiegeNodeMesh*>(nodeXform->getChild(0)); selectedSiegeNode != nullptr)
                                        {
                                            selectedSiegeNode->toggleBoundingBox();
                                            selectedSiegeNode->toggleAllDoorLabels();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (event.getEventType() == osgGA::GUIEventAdapter::KEYUP)
        {
            if (event.getKey() == '1')
            {
                viewer.setCameraManipulator(new osgGA::TrackballManipulator());
            }
            else if (event.getKey() == '2')
            {
                apcb = new osg::AnimationPathCallback(currentPath);
            }
            else if (event.getKey() == '3')
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
