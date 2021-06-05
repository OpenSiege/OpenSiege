
#include "UITestState.hpp"

#include <osgDB/ReadFile>
#include <state/GameStateMgr.hpp>

#include "ui/ImageFont.hpp"
#include "ui/TextLine.hpp"
#include "ui/Widget.hpp"
#include "ui/Shell.hpp"
#include "IFileSys.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void UITestState::enter()
    {
        auto log = spdlog::get("log");
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

        if (auto stream = fileSys.createInputStream("/ui/interfaces/backend/data_bar/data_bar.gas"))
        {
            log->debug("opened data_bar.gas");

            if (Fuel doc; doc.load(*stream))
            {
                // intentionally add the hp button first so we can test that our z sorting is working
                // the health potion button should end up on top of the data_bar

                auto hp_button = new Widget(shell); hp_button->buildWidgetFromFuelBlock(doc.child("data_bar:button_health_potions"));
                log->info("{} width is {} and height is {}", hp_button->getName(), hp_button->width(), hp_button->height());
                shell.addWidget(hp_button);

                auto data_bar = new Widget(shell);
                data_bar->buildWidgetFromFuelBlock(doc.child("data_bar:data_bar"));
                log->info("{} width is {} and height is {}", data_bar->getName(), data_bar->width(), data_bar->height());

                shell.addWidget(data_bar);
            }
        }
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
        return false;
    }
}
