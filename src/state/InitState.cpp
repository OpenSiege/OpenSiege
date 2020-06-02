
#include "InitState.hpp"

#include <spdlog/spdlog.h>
#include <osgDB/Registry>

#include "LocalFileSys.hpp"
#include "cfg/IConfig.hpp"
#include "state/GameStateMgr.hpp"
#include "osg/FileNameMap.hpp"

namespace ehb
{
    void InitState::enter()
    {
        auto log = spdlog::get("filesystem");

        log->info("InitState::enter()");

        if (!fileSys.init(config))
        {
            // TODO: shut down the engine
        }

        // setup the NNK system and register it with OSG
        auto callback = new FileNameMap(fileSys);
        osgDB::Registry::instance()->setFindFileCallback(callback);

        // TODO: setup contentdb, world data, and other sub systems

        // TODO: setup osg reader writers

        // TODO: any asset and engine preloading from gas files
        if (auto stream = fileSys.createInputStream("/ui/config/preload_textures/preload_textures.gas"))
        {
            if (Fuel doc; doc.load(*stream))
            {
                if (auto child = doc.child("preload_textures"))
                {
                    log->info("Preloading {} textures defined in preload_textures.gas", child->eachAttribute().size());

                    for (const auto attribute : child->eachAttribute())
                    {
                        // TODO: preload actual textures
                    }
                }
            }
        }

        // TODO: any other up front init (gui?) that needs to be done

        // TODO: go to the next state
        if (const std::string state = config.getString("state"); !state.empty())
        {
            gameStateMgr.request(state);

            return;
        }
    }

    void InitState::leave()
    {
    }

    void InitState::update(double deltaTime)
    {
    }

    bool InitState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
