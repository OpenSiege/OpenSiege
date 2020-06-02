
#include "SiegeNodeTestState.hpp"

#include <sstream>

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <spdlog/spdlog.h>

#include "osg/ReaderWriterSNO.hpp"

namespace ehb
{
    void SiegeNodeTestState::enter()
    {
        auto log = spdlog::get("game");

        static const std::string meshName = "t_grs01_houses_generic-a-log.sno";

        auto mesh = dynamic_cast<SiegeNodeMesh*>(osgDB::readNodeFile(meshName));
        if (mesh != nullptr)
        {
            log->info("Loaded {}", meshName);

            auto transform = new osg::MatrixTransform;
            transform->addChild(mesh);

            scene.addChild(transform);
        }
        else
        {
            log->error("Failed to load {}", meshName);
        }
    }

    void SiegeNodeTestState::leave()
    {
    }

    void SiegeNodeTestState::update(double deltaTime)
    {
    }

    bool SiegeNodeTestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
