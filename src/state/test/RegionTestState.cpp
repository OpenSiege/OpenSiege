
#include "RegionTestState.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"
#include "osg/ReaderWriterSNO.hpp"

#include <set>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgDB/Options>

#include <spdlog/spdlog.h>

namespace ehb
{
    void RegionTestState::enter()
    {
        auto log = spdlog::get("log");
        log->set_level(spdlog::level::debug);

        log->info("RegionTestState::enter()");

        // the first step is to map all the siege node file names to their corresponding guid
        {
            // [mesh_file*] { filename = t_cry01_cave-lc; guid=0xa2010103; }
            static const std::string directory = "/world/global/siege_nodes"; int32_t fileCount = 0;
            fileSys.eachGasFile(directory, [this, &fileCount, log](const std::string& filename, auto doc)
                {
                    log->debug("loading {}", filename);

                    // first set of children is going to be [t:siege_nodes,n:*]
                    for (auto root : doc->eachChild())
                    {
                        for (auto node : root->eachChild())
                        {
                            const auto itr = meshFileNameToGuidKeyMap.emplace(node->valueOf("guid"), osgDB::convertToLowerCase(node->valueOf("filename")));

                            if (itr.second != true)
                            {
                                log->debug("duplicate mesh mapping found: tried to insert filename {} for guid {}, but found filename {} there already", node->valueOf("filename"), node->valueOf("guid"), itr.first->second);
                            }
                        }
                    }

                    fileCount++;
                });

            log->debug("after loading {} files there are {} entries in our mesh filename to key map", fileCount, meshFileNameToGuidKeyMap.size());
        }

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
            if (Fuel doc; doc.load(*stream))
            {
                // region properties
                std::string actor_ambient_color = doc.valueOf("siege_node_list:actor_ambient_color");
                std::string actor_ambient_intensity = doc.valueOf("siege_node_list:actor_ambient_intensity");
                std::string ambient_color = doc.valueOf("siege_node_list:ambient_color");
                std::string ambient_intensity = doc.valueOf("siege_node_list:ambient_intesity");
                std::string environment_map = doc.valueOf("siege_node_list:environment_map");
                std::string object_ambient_color = doc.valueOf("siege_node_list:object_ambient_color");
                std::string object_ambient_intensity = doc.valueOf("siege_node_list:object_ambient_intensity");
                std::string targetnode = doc.valueOf("siege_node_list:targetnode");

                // struct to hold each entry defined in [door*]
                struct DoorEntry
                {
                    uint32_t id;
                    uint32_t farDoor;
                    uint64_t farGuid;
                };

                std::unordered_multimap<uint32_t, DoorEntry> doorMap;
                std::unordered_map<uint32_t, osg::ref_ptr<osg::MatrixTransform>> nodeMap;
                std::set<uint32_t> completeSet;

                for (const auto node : doc.eachChildOf("siege_node_list"))
                {
                    const uint32_t nodeGuid = node->valueAsUInt("guid");
                    const std::string meshGuid = osgDB::convertToLowerCase(node->valueOf("mesh_guid"));

                    log->debug("dealing with nodeGuid: {} with meshGuid: {}", nodeGuid, meshGuid);

                    // handle all [door*] entrires
                    for (const auto doorEntry : node->eachChild())
                    {
                        DoorEntry e;

                        e.id = doorEntry->valueAsUInt("id");
                        e.farDoor = doorEntry->valueAsUInt("fardoor");
                        e.farGuid = std::stoul(doorEntry->valueOf("farguid"), nullptr, 16);

                        doorMap.emplace(nodeGuid, std::move(e));
                    }

                    if (const std::string meshFileName = resolveFileName(meshGuid); meshFileName != meshGuid)
                    {
                        if (auto mesh = dynamic_cast<SiegeNodeMesh*>(osgDB::readNodeFile(meshFileName + ".sno", new osgDB::Options)))
                        {
                            // log->debug("handling mesh: {}", meshFileName);

                            osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform;

                            regionGroup->addChild(xform);
                            xform->addChild(mesh);

                            nodeMap.emplace(nodeGuid, xform);
                        }
                        else
                        {
                            log->error("couldn't find mesh guid {} ({})", meshGuid, meshFileName);
                        }
                    }
                    else
                    {
                        log->error("mesh guid {} is not listed in {}", meshGuid, "/world/global/siege_nodes");
                    }
                }

                // now position the entire region
                const uint32_t targetGuid = doc.valueAsUInt("siege_node_list:targetnode");

                // recursive function to place every node in the region
                std::function<void(const uint32_t)> func = [&func, &doorMap, &nodeMap, &completeSet, log](const uint32_t guid)
                {
                    if (completeSet.insert(guid).second)
                    {
                        auto targetNode = nodeMap.at(guid);

                        const auto range = doorMap.equal_range(guid);

                        for (auto entry = range.first; entry != range.second; ++entry)
                        {
                            auto connectNode = nodeMap.at(entry->second.farGuid);

                            SiegeNodeMesh::connect(targetNode, entry->second.id, connectNode, entry->second.farDoor);

                            if (completeSet.count (entry->second.farGuid) == 0)
                            {
                                func (entry->second.farGuid);
                            }
                        }
                    }
                };

                func(targetGuid);

                log->debug("region loaded with {} nodes, targetGuid: 0x{:x}", regionGroup->getNumChildren(), targetGuid);
            }
        }
        else
        {
            log->error("failed to load nodesDotGas @ {}", nodesDotGas);
        }
    }

    void RegionTestState::leave()
    {
    }

    void RegionTestState::update(double deltaTime)
    {
    }

    bool RegionTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
