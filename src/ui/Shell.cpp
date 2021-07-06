
#include "Shell.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

#include "Widget.hpp"
#include "Button.hpp"
#include "DialogBox.hpp"
#include "Text.hpp"

#include <osgGA/GUIEventAdapter>
#include <osgUtil/RenderBin>
#include <osgDB/ReadFile>

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
        // log->set_level(spdlog::level::debug);

        log->info("Shell is starting up");

        screen.width = width;
        screen.height = height;
        
        if (auto stream = fileSys.createInputStream("/ui/interfaces/common/common_control_art.gas"))
        {
            if (Fuel doc; doc.load(*stream))
            {
                if (const auto node = doc.child("common_control_art"))
                {
                    for (const auto attr : node->eachAttribute())
                    {
                        ctrlArt[attr.name] = attr.value;
                    }
                }
            }
        }

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

    std::string Shell::mapCtrlArt(const std::string& key) const
    {
        const auto itr = ctrlArt.find(key);
        return itr != ctrlArt.end() ? itr->second : "";
    }

    Widget* Shell::findWidget(const std::string& name, const std::string& interface) const
    {
        for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (osg::Group* group = scene2d->getChild(i)->asGroup())
            {
                if (group->getName() == interface)
                {
                    for (uint32_t j = 0; j < group->getNumChildren(); j++)
                    {
                        if (group->getChild(j)->getName() == name)
                        {
                            return dynamic_cast<Widget*>(group->getChild(j));
                        }
                    }

                    break;
                }
            }
        }

        return nullptr;
    }

    Widget* Shell::findWidget(const std::string& name)
    {
        for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (osg::Group* group = scene2d->getChild(i)->asGroup())
            {
                for (uint32_t j = 0; j < group->getNumChildren(); j++)
                {
                    if (group->getChild(j)->getName() == name)
                    {
                        return dynamic_cast<Widget*>(group->getChild(j));

                        break;
                    }
                }
            }
        }

        return nullptr;
    }

    void Shell::activateInterface(const std::string& path, bool show)
    {
        auto log = spdlog::get("log");

        if (const auto colon = path.find_last_of(':'); colon != std::string::npos && path.size() > colon + 1)
        {
            const std::string name = path.substr(colon + 1);

            // check for an already loaded interface
            for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
            {
                if (scene2d->getChild(i)->getName() == name)
                {
                    if (show) scene2d->getChild(i)->setNodeMask(~0);

                    return;
                }
            }

            // transform path into an actual file path
            std::string interfaceFileName = path;
            std::replace(interfaceFileName.begin(), interfaceFileName.end(), ':', '/');

            if (path.find("ui:interfaces:multiplayer") == std::string::npos)
            {
                interfaceFileName += '/';
                interfaceFileName += name;
            }

            interfaceFileName += ".gas";
            interfaceFileName.insert(0, 1, '/');

            if (auto node = osgDB::readRefNodeFile(interfaceFileName))
            {
                scene2d->setName(name);
                scene2d->addChild(node);

                if (auto group = node->asGroup())
                {
                    for (uint32_t i = 0; i < group->getNumChildren(); i++)
                    {
                        if (auto widget = dynamic_cast<Widget*>(group->getChild(i)))
                        {
                            widget->resizeToScreenResolution(screen.width, screen.height);
                            // TODO: process messages
                        }
                    }

                    std::optional<int> shiftX, shiftY;

                    if (std::string value; group->getUserValue("centered", value))
                    {
                        if (Widget* widget = findWidget(value, group->getName()))
                        {
                            int32_t x = (screen.width - widget->width()) / 2;
                            int32_t y = (screen.height - widget->height()) / 2;

                            shiftX = x - widget->effectiveRect().left;
                            shiftY = y - widget->effectiveRect().right;
                        }
                    }

                    if (shiftX && shiftY)
                    {
                        for (uint32_t i = 0; i < group->getNumChildren(); i++)
                        {
                            if (auto widget = dynamic_cast<Widget*>(group->getChild(i)))
                            {
                                // TODO: drag widget
                            }
                        }
                    }
                }
            }

            if (show)
            {
                showInterface(name);
            }
            else
            {
                hideInterface(name);
            }
        }
    }

    void Shell::deactivateInterface(const std::string& interface)
    {
        for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (scene2d->getChild(i)->getName() == interface)
            {
                scene2d->removeChild(i);

                break;
            }
        }

        updateBackToFront();
    }

    void Shell::showInterface(const std::string& interface)
    {
        for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (scene2d->getChild(i)->getName() == interface)
            {
                scene2d->getChild(i)->setNodeMask(~0);

                break;
            }
        }

        updateBackToFront();
    }

    void Shell::hideInterface(const std::string& interface)
    {
        for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (scene2d->getChild(i)->getName() == interface)
            {
                scene2d->getChild(i)->setNodeMask(0);

                break;
            }
        }

        updateBackToFront();
    }

    Widget* Shell::createDefaultWidgetOfType(const std::string& type)
    {
        Widget* widget = nullptr;

        if (type == "window") widget = new Widget(*this);
        else if (type == "button") widget = new Button(*this);
        else if (type == "checkbox") widget = new Widget(*this);
        else if (type == "slider") widget = new Widget(*this);
        else if (type == "listbox") widget = new Widget(*this);
        else if (type == "radio_button") widget = new Widget(*this);
        else if (type == "button_multistage") widget = new Widget(*this);
        else if (type == "text") widget = new Text(*this);
        // else if (type == "cursor") { widget = new Cursor(*this); widget->setLayer(0xffff); }
        else if (type == "dockbar") widget = new Widget(*this);
        else if (type == "gridbox") widget = new Widget(*this);
        else if (type == "popupmenu") widget = new Widget(*this);
        else if (type == "item") widget = new Widget(*this);
        else if (type == "itemslot") widget = new Widget(*this);
        else if (type == "infoslot") widget = new Widget(*this);
        else if (type == "status_bar") widget = new Widget(*this);
        else if (type == "edit_box") widget = new Widget(*this);
        else if (type == "combo_box") widget = new Widget(*this);
        else if (type == "listener") widget = new Widget(*this);
        else if (type == "listreport") widget = new Widget(*this);
        else if (type == "chat_box") widget = new Widget(*this);
        else if (type == "text_box") widget = new Widget(*this);
        else if (type == "dialog_box") widget = new DialogBox(*this);
        else if (type == "tab") widget = new Widget(*this);

        if (widget)
        {
            widget->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, false);
            widget->getOrCreateStateSet()->setMode(GL_BLEND, true);
            widget->getOrCreateStateSet()->setRenderBinDetails(999, "UIShell");
        }

        return widget;
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

        for (uint32_t i = 0; i < scene2d->getNumChildren(); i++)
        {
            if (auto group = scene2d->getChild(i)->asGroup())
            {
                if (group->getNodeMask() != 0)
                {
                    for (uint32_t j = 0; j < group->getNumChildren(); j++)
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