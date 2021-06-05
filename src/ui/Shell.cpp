
#include "Shell.hpp"

#include <osgGA/GUIEventAdapter>

namespace ehb
{
    void Shell::init(IFileSys& fileSys, osg::Group* root, uint32_t width, uint32_t height)
    {
        log = spdlog::get("log");
        log->set_level(spdlog::level::debug);

        log->info("Shell is starting up");

        screen.width = width;
        screen.height = height;
        
        // TODO: common control art

        // TOOD: edu tooltips

        // TODO: tooltips

        if (root != nullptr)
        {
            scene2d = root;

            log->debug("scene2d has been setup correctly for the shell");
        }
        else
        {
            log->critical("Attempted to setup the shell but the group is nullptr. Stuff is about to go bad!");
        }
    }

    bool Shell::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        // since the Shell will get initialized on application startup, its possible an event will hit it before the group is valid
        // so make sure we just bail out
        if (scene2d == nullptr) log->debug("event was incoming but scene 2d is a bust");

        switch (event.getEventType())
        {
            case osgGA::GUIEventAdapter::PUSH:
            {
                log->debug("shell is handling push event");
            }
            break;

            case osgGA::GUIEventAdapter::RELEASE:
            {
                log->debug("shell is handling release event");
            }
            break;

            case osgGA::GUIEventAdapter::DOUBLECLICK:
            {
                log->debug("shell is handling doubleclick event");
            }
            break;

            case osgGA::GUIEventAdapter::DRAG:
            {
                log->debug("shell is handling drag event");
            }
            break;

            case osgGA::GUIEventAdapter::MOVE:
            {
                log->debug("shell is handling move event");
            }
            break;

            case osgGA::GUIEventAdapter::KEYDOWN:
            {
                log->debug("shell is handling keydown event");
            }
            break;

            case osgGA::GUIEventAdapter::KEYUP:
            {
                log->debug("shell is handling keyup event");
            }
            break;

            case osgGA::GUIEventAdapter::RESIZE:
            {
                log->debug("shell is handling resize event");

                screen.width = event.getWindowWidth();
                screen.height = event.getWindowHeight();
            }
            break;

            case osgGA::GUIEventAdapter::SCROLL:
            {
                log->debug("shell is handling scroll event");
            }
            break;

            default: break;
        }

        return false;
    }
}