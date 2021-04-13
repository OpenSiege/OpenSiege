
#include "AspectMeshTestState.hpp"

#include <sstream>
#include "ContentDb.hpp"

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgAnimation/Animation>
#include <osgAnimation/BasicAnimationManager>
#include <spdlog/spdlog.h>

#include "osg/Aspect.hpp"

namespace ehb
{
    static const std::vector<std::string> templateTests = {

    "farmgirl",
    "farmboy",
    "dwarf",
    "skeleton_good"
    };

    void AspectMeshTestState::enter()
    {
        clearAndLoadMesh(templateTests[0]);
    }

    void AspectMeshTestState::leave()
    {
        scene.removeChildren(0, scene.getNumChildren());
    }

    void AspectMeshTestState::update(double deltaTime)
    {
    }

    bool AspectMeshTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
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
                clearAndLoadMesh(templateTests[0]);
            }
            else if (event.getKey() == '2')
            {
                clearAndLoadMesh(templateTests[1]);
            }
            else if (event.getKey() == '3')
            {
                clearAndLoadMesh(templateTests[2]);
            }
            else if (event.getKey() == '4')
            {
                clearAndLoadMesh(templateTests[3]);
            }
        }
        }
        return false;
    }

    void AspectMeshTestState::clearAndLoadMesh(const std::string& tmplName)
    {
        auto log = spdlog::get("game");

        scene.removeChildren(0, scene.getNumChildren());

        if (auto go = contentDb.getGameObjectTmpl(tmplName))
        {
            auto model = go->valueOf("aspect:model");
            const auto chore_prefix = go->valueOf("body:chore_dictionary:chore_prefix");
            const auto chore_fidget = go->valueOf("body:chore_dictionary:chore_fidget:anim_files:00");

            if (model.empty() || chore_prefix.empty() || chore_fidget.empty())
            {
                log->critical("super bad. something is empty on reading template={}", tmplName);
                return;
            }

            if (mesh = static_cast<Aspect*>(osgDB::readNodeFile(model + ".asp")); mesh != nullptr)
            {

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

                /* the below defines 9 stances for a template
                   the running theory is that 00 is a default fourcc code
                [chore_default]
                {
                    chore_stances = 0, 1, 2, 3, 4, 5, 6, 7, 8;
                    skrit = simple_loop;
                    [anim_files] { 00 = dff; }
                }
                */

                // i think 00 gets read as an integer which ends up choping the leading 0
                const auto animationFilename = chore_prefix + "0" + "_" + chore_fidget + ".prs";

                log->info("retrieved '{}' from the contentdb. it has the model={}, chore_prefix={}, chore_fidget={}, animationFilename={}", tmplName, model, chore_prefix, chore_fidget, animationFilename);

                if (auto animation = static_cast<osgAnimation::Animation*>(osgDB::readObjectFile(animationFilename)); animation != nullptr)
                {
                    log->info("loaded animation={} for use and its duration is={}", animationFilename, animation->getDuration());

                    osg::ref_ptr<osgAnimation::BasicAnimationManager> animManager = new osgAnimation::BasicAnimationManager;
                    animation->setPlayMode(osgAnimation::Animation::LOOP);

                    animManager->registerAnimation(animation);
                    animManager->playAnimation(animation);

                    mesh->setUpdateCallback(animManager);
                }

                scene.addChild(transform);
            }
        }
        else
        {
            log->error("failed to retrieve {} from the contentdb", tmplName);
        }
    }
}
