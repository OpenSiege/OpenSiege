
#include "AspectMeshTestState.hpp"

#include <sstream>

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <spdlog/spdlog.h>

#include "osg/Aspect.hpp"

namespace ehb
{
    void AspectMeshTestState::enter()
    {
        auto log = spdlog::get("game");

        static const std::string meshName = "m_c_gah_fg_pos_a1.asp";
        
        if (mesh = dynamic_cast<Aspect*>(osgDB::readNodeFile(meshName)); mesh != nullptr)
        {
            log->info("Loaded {}", meshName);

            transform = new osg::MatrixTransform;
            transform->addChild(mesh);

#if 1
            // TODO: is there a better way to do this?
            // re-position the camera based on the size of the mesh and orient it up a little bit get a birds eye-view
            if (auto manipulator = viewer.getCameraManipulator())
            {
                double radius = osg::maximum(double(mesh->getBound().radius()), 1e-6);
                double dist = 3.5f * radius;

                // farmgril is facing away from the camer so invert our distance - trying to keep this as close to the siege node test state
                manipulator->setHomePosition(mesh->getBound().center() + osg::Vec3d(0.0, -dist, 3.0f), mesh->getBound().center(), osg::Vec3d(0.0f, 0.0f, 1.0f));
                manipulator->home(1);
            }
#endif

            // applying a skeleton messes with the bounding box size and expands it way more than it should
            mesh->applySkeleton();

            scene.addChild(transform);
        }
        else
        {
            log->error("Failed to load {}", meshName);
        }
    }

    void AspectMeshTestState::leave()
    {
        scene.removeChildren(0, scene.getNumChildren());
    }

    void AspectMeshTestState::update(double deltaTime)
    {
    }

    bool AspectMeshTestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        switch (event.getEventType())
        {
            case (osgGA::GUIEventAdapter::KEYUP):
            {
                if (event.getKey() == 'z')
                {
                    mesh->toggleBoundingBox();
                }
                else if (event.getKey() == '1')
                {
                    clearAndLoadMesh(farmgirl[0]);
                }
                else if (event.getKey() == '2')
                {
                    clearAndLoadMesh(farmgirl[1]);
                }
                else if (event.getKey() == '3')
                {
                    clearAndLoadMesh(farmgirl[2]);
                }
                else if (event.getKey() == '4')
                {
                    clearAndLoadMesh(farmgirl[3]);
                }
                else if (event.getKey() == '5')
                {
                    clearAndLoadMesh(farmgirl[4]);
                }
                else if (event.getKey() == '6')
                {
                    clearAndLoadMesh(farmgirl[5]);
                }
            }
        }
        return false;
    }

    void AspectMeshTestState::clearAndLoadMesh(const std::string& filename)
    {
        if (filename.empty()) return;

        if (mesh = dynamic_cast<Aspect*>(osgDB::readNodeFile(filename)); mesh != nullptr)
        {
            spdlog::get("log")->info("loading new fg mesh");

            mesh->applySkeleton();

            transform->removeChildren(0, 1);
            transform->addChild(mesh);
        }
    }
}
