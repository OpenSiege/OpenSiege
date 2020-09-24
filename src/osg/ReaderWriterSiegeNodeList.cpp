
#include "ReaderWriterSiegeNodeList.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

#include "ReaderWriterSNO.hpp"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

namespace ehb
{
    ReaderWriterSiegeNodeList::ReaderWriterSiegeNodeList(IFileSys& fileSys) : fileSys(fileSys)
    {
        log = spdlog::get("log");

        // load in all nodes defined in the siege_nodes folder
        static const std::string directory = "/world/global/siege_nodes"; int32_t fileCount = 0;
        fileSys.eachGasFile(directory, [this, &fileCount](const std::string& filename, std::unique_ptr<Fuel> doc)
            {
                log->debug("loading {}", filename);

                // first set of children is going to be [t:siege_nodes,n:*]
                for (const FuelBlock* root : doc->eachChild())
                {
                    for (const FuelBlock* node : root->eachChild())
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

        supportsExtension("gas", "Dungeon Siege Gas");
    }

    const char* ReaderWriterSiegeNodeList::className() const {

        return "Dungeon Siege Siege Node List Reader";
    }

    bool ReaderWriterSiegeNodeList::acceptsExtension(const std::string& extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "gas");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterSiegeNodeList::readNode(const std::string& filename, const osgDB::Options* options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        return readNode(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterSiegeNodeList::readNode(std::istream& stream, const osgDB::Options* options) const
    {
        // struct to hold each entry defined in [door*]
        struct DoorEntry
        {
            uint32_t id;
            uint32_t farDoor;
            uint64_t farGuid;
        };

        // holds a mapping from the guid to the final matrix transform of the placed nodes
        std::unordered_map<uint32_t, osg::ref_ptr<osg::MatrixTransform>> nodeMap;
        std::unordered_multimap<uint32_t, DoorEntry> doorMap;
        std::set<uint32_t> completeSet;

        if (Fuel doc; doc.load(stream))
        {
            osg::ref_ptr<osg::Group> regionGroup = new osg::Group;

            // region properties
            std::string actor_ambient_color = doc.valueOf("siege_node_list:actor_ambient_color");
            std::string actor_ambient_intensity = doc.valueOf("siege_node_list:actor_ambient_intensity");
            std::string ambient_color = doc.valueOf("siege_node_list:ambient_color");
            std::string ambient_intensity = doc.valueOf("siege_node_list:ambient_intesity");
            std::string environment_map = doc.valueOf("siege_node_list:environment_map");
            std::string object_ambient_color = doc.valueOf("siege_node_list:object_ambient_color");
            std::string object_ambient_intensity = doc.valueOf("siege_node_list:object_ambient_intensity");
            std::string targetnode = doc.valueOf("siege_node_list:targetnode");

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
            const uint32_t targetNodeGuid = doc.valueAsUInt("siege_node_list:targetnode");

            // easy access to targetGuid later on. though this is only valid when the region is initially loaded. not sure if
            // it will remain valid in future frames (pathfinding?)
            regionGroup->setUserValue("targetnode", targetNodeGuid);

            // recursive function to place every node in the region
            std::function<void(const uint32_t)> func = [&func, &doorMap, &nodeMap, &completeSet](const uint32_t guid)
            {
                if (completeSet.insert(guid).second)
                {
                    osg::ref_ptr<osg::MatrixTransform> targetNode = nodeMap.at(guid);

                    const auto range = doorMap.equal_range(guid);

                    for (auto entry = range.first; entry != range.second; ++entry)
                    {
                        osg::ref_ptr<osg::MatrixTransform> connectNode = nodeMap.at(entry->second.farGuid);

                        SiegeNodeMesh::connect(targetNode, entry->second.id, connectNode, entry->second.farDoor);

                        if (completeSet.count(entry->second.farGuid) == 0)
                        {
                            func(entry->second.farGuid);
                        }
                    }
                }
            };

            func(targetNodeGuid);

            // index 1 of the user data container will contain our target node xform for now
            if (osg::MatrixTransform* targetNodeXform = nodeMap.at(targetNodeGuid))
            {
                regionGroup->getOrCreateUserDataContainer()->addUserObject(targetNodeXform);
            }
            else
            {
                log->error ("catastrophic failure there is no target node xform");

                return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
            }

            log->debug("region loaded with {} nodes, targetGuid: 0x{:x}", regionGroup->getNumChildren(), targetNodeGuid);

            return regionGroup.release();
        }

        return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
    }
}
