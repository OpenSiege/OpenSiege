
#include "UITestState.hpp"

#include <osgDB/ReadFile>
#include <osgGA/GUIEventAdapter>

#include "state/GameStateMgr.hpp"
#include "ui/ImageFont.hpp"
#include "ui/TextLine.hpp"
#include "ui/Widget.hpp"
#include "ui/Button.hpp"
#include "ui/Shell.hpp"
#include "IFileSys.hpp"

namespace ehb
{
    void UITestState::enter()
    {
        log = spdlog::get("log");
        log->info("UITestState::enter()");

        std::string font = "b_gui_fnt_12p_copperplate-light";

#if 0
        auto options = new osgDB::Options(std::string("font=") + font);
        osg::ref_ptr<ImageFont> imageFont = osgDB::readRefFile<ImageFont>("/ui/fonts/fonts.gas", options);

        if (imageFont != nullptr)
        {
            TextLine* line = new TextLine(scene);

            line->text = "this is some test text";

            line->build(*imageFont);
        }
        else
        {
            spdlog::get("log")->error("failed to load {}", font);
        }
#endif
    }

    void UITestState::leave()
    {
        shell.removeAllWidgets();
    }

    void UITestState::update(double deltaTime)
    {
    }

    bool UITestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        switch (event.getEventType())
        {
        case (osgGA::GUIEventAdapter::KEYUP):
        {
            if (event.getKey() == '1')
            {
                shell.removeAllWidgets();
            }
            else if (event.getKey() == '2')
            {
                shell.removeAllWidgets();
                shell.activateInterface("ui:interfaces:backend:data_bar");
                shell.activateInterface("ui:interfaces:backend:character");
                //shell.activateInterface("ui:interfaces:backend:defeat_dialog");

                if (auto data_bar = shell.findWidget("data_bar", "data_bar"))
                {
                    spdlog::get("log")->debug("data_bar found");
                    spdlog::get("log")->debug("{} width is {} and height is {} and draw_order is {}", data_bar->getName(), data_bar->width(), data_bar->height(), data_bar->drawOrder());
                }
            }
        }
        }

        return false;
    }

    bool UITestState::expectingWidthAndHeight(const Widget* widget, uint32_t expectedWidth, uint32_t expectedHeight)
    {
        if (widget == nullptr)
        {
            log->critical("expectingWidthAndHeight() is handling a null widget - break point me");

            return false;
        }

        const auto name = widget->getName();
        if (name == "")
        {
            // TODO: debug or dsmod only mode?
            log->critical("we are loading a widget with an empty name, please set the name properly");

            return false;
        }

        if (widget->width() != expectedWidth)
        {
            log->critical("{} is expecting to have a width of {} but has calculated a width of {} - very bad", name, expectedWidth, widget->width());

            return false;
        }

        if (widget->height() != expectedHeight)
        {
            log->critical("{} is expecting to have a height of {} but has calculated a height of {} - very bad", name, expectedHeight, widget->height());

            return false;
        }

        log->info("{} has been loaded with a width of {} and a height of {} calculated from {}", name, widget->width(), widget->height(), widget->effectiveRect());

        return true;
    }
}
