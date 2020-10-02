
#include "SiegeNodeTestState.hpp"

#include <sstream>

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <spdlog/spdlog.h>

#include "osg/SiegeNodeMesh.hpp"

namespace ehb
{
    void SiegeNodeTestState::enter()
    {
        auto log = spdlog::get("game");

        // static const std::string meshName = "t_grs01_houses_generic-a-log.sno";
        static const std::string meshName = "t_xxx_wal_08-thck.sno";

        mesh = dynamic_cast<SiegeNodeMesh*>(osgDB::readNodeFile(meshName));
        if (mesh != nullptr)
        {
            log->info("Loaded {}", meshName);

            auto t1 = new osg::MatrixTransform;
            t1->addChild(mesh);

            // TODO: is there a better way to do this?
            // re-position the camera based on the size of node and orient it up a little bit get a birds eye-view
            if (auto manipulator = viewer.getCameraManipulator())
            {
                double radius = osg::maximum(double(mesh->getBound().radius()), 1e-6);
                double dist = 3.5f * radius;

                manipulator->setHomePosition(mesh->getBound().center() + osg::Vec3d(0.0, dist, 7.0f), mesh->getBound().center(), osg::Vec3d(0.0f, 0.0f, 1.0f));
                manipulator->home(1);
            }

            auto t2 = new osg::MatrixTransform;
            t2->addChild(mesh);

            SiegeNodeMesh::connect(t1, 4, t2, 8);

            scene.addChild(t1);
            scene.addChild(t2);
        }
        else
        {
            log->error("Failed to load {}", meshName);
        }
    }

    void SiegeNodeTestState::leave()
    {
        scene.removeChildren(0, scene.getNumChildren());
    }

    void SiegeNodeTestState::update(double deltaTime)
    {
    }

    bool SiegeNodeTestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        switch (event.getEventType())
        {
            case (osgGA::GUIEventAdapter::KEYUP):
            {
                if (event.getKey() == '1')
                {
                    mesh->toggleAllDoorLabels();
                    mesh->toggleBoundingBox();
                }
                else if (event.getKey() == '2')
                {
                    mesh->toggleLogicalNodeFlags();
                }
            }
        }

        return false;
    }
}
