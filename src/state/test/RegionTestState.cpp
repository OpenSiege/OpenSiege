
#include "RegionTestState.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

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
