
#include "InitState.hpp"

#include <spdlog/spdlog.h>

#include "LocalFileSys.hpp"

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

        // TODO: process NNK files

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
