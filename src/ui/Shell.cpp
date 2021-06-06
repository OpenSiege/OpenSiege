
#include "Shell.hpp"

#include "Widget.hpp"

#include <osgGA/GUIEventAdapter>
#include <osgUtil/RenderBin>

#include <spdlog/spdlog.h>

namespace ehb
{
    // working from https://github.com/gwaldron/osgearth/blob/master/src/osgEarth/ScreenSpaceLayout.cpp
    struct SortCallback : public osgUtil::RenderBin::SortCallback
    {
        virtual void sortImplementation(osgUtil::RenderBin* bin) override
        {
            auto& renderLeafList = bin->getRenderLeafList();

            bin->copyLeavesFromStateGraphListToRenderLeafList();

            std::sort(renderLeafList.begin(), renderLeafList.end(), [](const osgUtil::RenderLeaf* lhs, const osgUtil::RenderLeaf* rhs)
                {
                    uint32_t l1 = 0, l2 = 0;
                    uint32_t z1 = 0, z2 = 0;

                    lhs->getDrawable()->getUserValue("layer", l1);
                    rhs->getDrawable()->getUserValue("layer", l2);

                    lhs->getDrawable()->getUserValue("draw_order", z1);
                    rhs->getDrawable()->getUserValue("draw_order", z2);

                    if (l1 == l2) return z1 < z2;
                    return l1 < l2;
                });
        }
    };
}

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

            // turn off depth testing since we will be using DS z order sorting
            scene2d->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, false);

            // ensure proper blending for the entire shell
            scene2d->getOrCreateStateSet()->setMode(GL_BLEND, true);

            // ds style z order sorting
            osg::ref_ptr<osgUtil::RenderBin> renderBin = osgUtil::RenderBin::createRenderBin("UIShell");
            renderBin->setSortCallback(new SortCallback);

            osgUtil::RenderBin::addRenderBinPrototype("UIShell", renderBin);

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

    void Shell::addWidget(Widget* widget)
    {
        if (widget != nullptr)
        {
            scene2d->addChild(widget);
            
            updateBackToFront();
        }
    }

    void Shell::updateBackToFront()
    {
        backToFront.clear();

        for (unsigned int i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (auto group = scene2d->getChild(i)->asGroup())
            {
                if (group->getNodeMask() != 0)
                {
                    for (unsigned int j = 0; j < group->getNumChildren(); j++)
                    {
                        if (auto widget = dynamic_cast<Widget*>(group->getChild(j)))
                        {
                            backToFront.emplace_back(widget);
                        }
                    }
                }
            }
        }

        std::sort(backToFront.begin(), backToFront.end(), [](const auto& lhs, const auto& rhs)
            {
                if (lhs->isTopMost() == rhs->isTopMost()) return lhs->drawOrder() < rhs->drawOrder();
                return rhs->isTopMost();
            });
    }

    void Shell::removeAllWidgets()
    {
        scene2d->removeChildren(0, scene2d->getNumChildren());
    }
}