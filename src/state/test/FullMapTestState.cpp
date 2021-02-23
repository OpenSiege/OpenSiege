
#include "FullMapTestState.hpp"

#include <osg/MatrixTransform>
#include <osg/ProxyNode>
#include <osgViewer/Viewer>

#include "world/WorldMapData.hpp"
#include "osg/SiegeNodeMesh.hpp"

namespace ehb
{
    void FullMapTestState::enter()
    {
        log = spdlog::get("world");

        log->info("FullMapTestState::enter()");

        const auto& worldData = worldMapDataCache.cache.at("multiplayer_world");

        log->info("Attempting to load [{}]", worldData.name);

        // town_center
        guid = 0xD5A3ACD9;

        // dump all currently loaded regions if there are any
        eachGuid.clear();

        // place our first region to be loaded into the queue
        eachGuid.emplace(*guid);

        for (const auto& entry : worldData.eachStitchForRegion(*guid))
        {
            eachGuid.emplace(entry.first);

            log->info("Adding stitch entry for [{}]", *guid);
        }

        for (const uint32_t regionGuid : eachGuid)
        {
            // check to see if the region has been loaded yet
            if (eachRegion.count(regionGuid) == 0)
            {
                const std::string regionFileName = worldData.nameForRegionGuid(regionGuid);

                log->info("Processing region [{}]", regionFileName);

                osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
                osg::ref_ptr<osg::ProxyNode> proxy = new osg::ProxyNode;

                proxy->setFileName(0, regionFileName);
                proxy->setName("PROXY LOADER FOR GUID: " + std::to_string(regionGuid));
                transform->addChild(proxy);
                transform->setName("TRANSFORM FOR GUID: " + std::to_string(regionGuid));
                transform->setUserValue("regionFileName", regionFileName);

                scene.addChild(transform);
                loadingQueue.emplace(regionGuid, transform);

                log->info("queueing up {} for proxy loading", regionFileName);
            }
        }
    }

    void FullMapTestState::leave()
    {
    }

    void FullMapTestState::update(double deltaTime)
    {
        // guid needs to be set for any loading to take place
        if (guid)
        {
            const auto& data = worldMapDataCache.cache.at("multiplayer_world");

            // check on every update if all of the regions in the loadingQueue have loaded
            const bool success = std::all_of(loadingQueue.begin(), loadingQueue.end(), [](const auto& entry)
                {
                    osg::MatrixTransform* transform = entry.second;
                    osg::ProxyNode* proxy = dynamic_cast<osg::ProxyNode*>(transform->getChild(0));

                    return proxy->getNumChildren() != 0;
                });

            if (success)
            {
                log->info("about to build around region 0x{:x} ({})", *guid, data.nameForRegionGuid(*guid));

                /*
                 * iterate over each region
                 * if it hasn't been loaded yet add it
                 * loop over all the nodes and add them to the eachNode map
                 */
                for (const auto& entry : loadingQueue)
                {
                    const uint32_t regionGuid = entry.first;
                    osg::MatrixTransform* transform = entry.second;

                    transform->setNodeMask(0);

                    if (eachRegion.count(regionGuid) == 0)
                    {
                        // adding transform to the map
                        // add every siege node in transform to eachNode
                        osg::ProxyNode* proxy = dynamic_cast<osg::ProxyNode*>(transform->getChild(0));
                        osg::Group* group = proxy->getChild(0)->asGroup();

                        for (unsigned int i = 0; i < group->getNumChildren(); i++)
                        {
                            osg::MatrixTransform* node = dynamic_cast<osg::MatrixTransform*>(group->getChild(i));

                            if (uint32_t guid; node->getUserValue("guid", guid))
                            {
                                eachNode.emplace(guid, node);
                            }
                            else
                            {
                                log->error("found a node without a guid");
                            }
                        }

                        log->info("added {} nodes to eachNode and eachNode new size is {}", group->getNumChildren(), eachNode.size());
                    }
                }

                log->info("{} nodes in eachNode after pruning", eachNode.size());

                // FIXME: pretty hacky, but can fix once we actually store the osg::Group
                for (const auto& entry : eachRegion)
                {
                    osg::MatrixTransform* transform = entry.second;

                    // transform->setMatrix(osg::Matrix());

                    if (loadingQueue.count(entry.first) == 0)
                    {
                        std::string regionFileName; transform->getUserValue("regionFileName", regionFileName);
                        log->info("purging {} from scene graph", regionFileName);

                        // removing transform from the map
                        // TODO: remove every siege node in transform to eachNode
                        osg::ProxyNode* proxy = dynamic_cast<osg::ProxyNode*>(transform->getChild(0));
                        osg::Group* group = proxy->getChild(0)->asGroup();

                        for (unsigned int i = 0; i < group->getNumChildren(); i++)
                        {
                            osg::MatrixTransform* node = dynamic_cast<osg::MatrixTransform*>(group->getChild(i));

                            if (uint32_t guid; node->getUserValue("guid", guid))
                            {
                                eachNode.erase(guid);
                            }
                            else
                            {
                                log->error("found a node without a guid");
                            }
                        }

                        transform->getParent(0)->removeChild(transform);
                    }
                }

                log->info("{} nodes in eachNode after adding", eachNode.size());

                eachRegion.clear();

                // grab our target region and add it as it doesn't need to be stitched
                osg::MatrixTransform* region1Xform = loadingQueue.at(*guid);
                eachRegion.emplace(*guid, region1Xform);

                region1Xform->setNodeMask(~0);

                eachRegion.emplace(*guid, region1Xform);

                if (auto manipulator = viewer.getCameraManipulator())
                {
                    if (const SiegeNodeMesh* mesh = static_cast<const SiegeNodeMesh*>(region1Xform->getChild(0)))
                    {
                        double radius = osg::maximum(double(mesh->getBound().radius()), 1e-6);
                        double dist = 7.f * radius;

                        manipulator->setHomePosition(mesh->getBound().center() + osg::Vec3d(0.0, -dist, 15.0f), mesh->getBound().center(), osg::Vec3d(0.0f, 0.0f, 1.0f));
                        manipulator->home(1);
                    }
                    else
                    {
                        // log->error("failed to find siegenodemesh attached to transform with guid: {}", targetNodeGuid);
                    }
                }

#if 1
                // iterate over each stitch that is available and connect our nodes together
                for (const auto& entry : data.eachStitchForRegion(*guid))
                {
                    const uint32_t region2Guid = entry.first;
                    const auto& stitch = entry.second;

                    osg::MatrixTransform* region2Xform = loadingQueue.at(region2Guid);

                    region2Xform->setNodeMask(~0);

                    osg::MatrixTransform* node1Xform = eachNode.at(stitch.node1);
                    osg::MatrixTransform* node2Xform = eachNode.at(stitch.node2);

                    // TODO: WorldTerrain class?
                    SiegeNodeMesh::connect(region1Xform, node1Xform, stitch.door1, region2Xform, node2Xform, stitch.door2);

                    eachRegion.emplace(region2Guid, region2Xform);
                }
#else
                std::function<void(uint32_t guid)> recurse = [&, this](uint32_t guid)
                {
                    //for (auto& [guid2, stitch] : stitchIndex.allStitches())
                    {
                        for (const auto& entry : data.eachStitchForRegion(guid))
                        {
                            const uint32_t region2Guid = entry.first;
                            const auto& stitch = entry.second;

                            if (eachRegion.count(region2Guid) == 0)
                            {
                                osg::MatrixTransform* region2Xform = loadingQueue.at(region2Guid);

                                region2Xform->setNodeMask(~0);

                                osg::MatrixTransform* node1Xform = eachNode.at(stitch.node1);
                                osg::MatrixTransform* node2Xform = eachNode.at(stitch.node2);

                                auto targetRegion = loadingQueue.at(data.regionForNode(stitch.node1));

                                log->info("stitching region: {} to {}", region2Xform->getName(), targetRegion->getName());

                                if (eachRegion.count(data.regionForNode(stitch.node1)) == 0)
                                {
                                    log->error("we might have a problem due to {} not being stitched yet they are connected to {}", targetRegion->getName());

                                    continue;
                                }

                                SiegeNodeMesh::connect(targetRegion, node1Xform, stitch.door1, region2Xform, node2Xform, stitch.door2);

                                eachRegion.emplace(region2Guid, region2Xform);

                                recurse(region2Guid);
                            }
                        }
                    }
                };

                recurse(*guid);
#endif

                log->info("done loading region 0x{:x}", *guid);

                guid.reset();
                loadingQueue.clear();
            }
        }
    }

    bool FullMapTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
