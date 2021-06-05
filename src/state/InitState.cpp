
#include "InitState.hpp"

#include <spdlog/spdlog.h>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include "filesystem/LocalFileSys.hpp"
#include "cfg/IConfig.hpp"
#include "state/GameStateMgr.hpp"
#include "osg/FileNameMap.hpp"
#include "osgPlugins/ReaderWriterRAW.hpp"
#include "osgPlugins/ReaderWriterSNO.hpp"
#include "osgPlugins/ReaderWriterFont.hpp"
#include "osgPlugins/ReaderWriterSiegeNodeList.hpp"
#include "osgPlugins/ReaderWriterASP.hpp"
#include "osgPlugins/ReaderWriterPRS.hpp"

#include "ui/ImageFont.hpp"
#include "ui/Shell.hpp"
#include "ContentDb.hpp"
#include "console/Console.hpp"

namespace ehb
{
    void InitState::enter()
    {
        auto log = spdlog::get("filesystem");

        log->info("InitState::enter()");

        if (!fileSys.init(config))
        {
            gameStateMgr.request("ExitState");

            return;
        }

        // setup the NNK system and register it with OSG
        auto callback = new FileNameMap(fileSys);
        osgDB::Registry::instance()->setFindFileCallback(callback);

        // TODO: setup contentdb, world data, and other sub systems

        // TODO: setup osg reader writers
        {
            osgDB::Registry::instance()->addReaderWriter(new ReaderWriterRAW(fileSys));
            osgDB::Registry::instance()->addReaderWriter(new ReaderWriterSNO(fileSys));
            osgDB::Registry::instance()->addReaderWriter(new ReaderWriterASP(fileSys));
            osgDB::Registry::instance()->addReaderWriter(new ReaderWriterPRS(fileSys));
            osgDB::Registry::instance()->addReaderWriter(new ReaderWriterFont(fileSys));
            osgDB::Registry::instance()->addReaderWriter(new ReaderWriterSiegeNodeList(fileSys));
            
        }

        contentDb.init(fileSys);

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
        auto options = new osgDB::Options(std::string("font=b_gui_fnt_12p_copperplate-light"));
        osg::ref_ptr<ImageFont> imageFont = osgDB::readRefFile<ImageFont>("/ui/fonts/fonts.gas", options);
        console.font = imageFont;
        console.resetCaret();

        shell.init(fileSys, scene2d, 800, 600);

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
