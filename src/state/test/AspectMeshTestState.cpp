
#include "AspectMeshTestState.hpp"

#include <sstream>
#include "ContentDb.hpp"

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgAnimation/Animation>
#include <osgAnimation/BasicAnimationManager>

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
        log = spdlog::get("log");

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
            // first set of hotkeys to swap the model
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
            // second set to swap to run animation
            else if (event.getKey() == 'w')
            {
                if (isFidgeting)
                {
                    isFidgeting = false;
                    animationManager->stopAll();
                    animationManager->playAnimation(walkAnimation);
                }
                else
                {
                    isFidgeting = true;
                    animationManager->stopAll();
                    animationManager->playAnimation(fidgetAnimation);
                }
            }
        }
        }
        return false;
    }

    void AspectMeshTestState::clearAndLoadMesh(const std::string& tmplName)
    {
        scene.removeChildren(0, scene.getNumChildren());

        isFidgeting = true;
        fidgetAnimation = nullptr;
        walkAnimation = nullptr;
        animationManager = nullptr;

        if (auto go = contentDb.getGameObjectTmpl(tmplName))
        {
            auto model = go->valueOf("aspect:model");
            const auto chore_prefix = go->valueOf("body:chore_dictionary:chore_prefix");
            const auto chore_fidget = go->valueOf("body:chore_dictionary:chore_fidget:anim_files:00");
            auto chore_walk = go->valueOf("body:chore_dictionary:chore_walk:anim_files:00");

            // well this is a silly workaround
            if (chore_walk.empty()) chore_walk = go->valueOf("body:chore_dictionary:chore_walk:anim_files:05");

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
                const auto fidgetFilename = chore_prefix + "0" + "_" + chore_fidget + ".prs";
                const auto walkFilename = chore_prefix + "0" + "_" + chore_walk + ".prs";

                log->info("retrieved '{}' from the contentdb. it has the model={}, chore_prefix={}, chore_fidget={}, fidgetFilename={} walkFilename={}", tmplName, model, chore_prefix, chore_fidget, fidgetFilename, walkFilename);

                fidgetAnimation = static_cast<osgAnimation::Animation*>(osgDB::readObjectFile(fidgetFilename));
                walkAnimation = static_cast<osgAnimation::Animation*>(osgDB::readObjectFile(walkFilename));

                if (fidgetAnimation != nullptr && walkAnimation != nullptr)
                {
                    log->info("loaded fidget and walk animations for use and durations are {} and {} respectively", fidgetAnimation->getDuration(), walkAnimation->getDuration());

                    animationManager = new osgAnimation::BasicAnimationManager;
                    fidgetAnimation->setPlayMode(osgAnimation::Animation::LOOP);
                    walkAnimation->setPlayMode(osgAnimation::Animation::LOOP);

                    animationManager->registerAnimation(fidgetAnimation);
                    animationManager->registerAnimation(walkAnimation);

                    // start our fidget
                    animationManager->playAnimation(fidgetAnimation);

                    mesh->setUpdateCallback(animationManager);
                }
                else
                {
                    log->critical("failed to load a fidget or walk animation");
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
