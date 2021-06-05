
#include "UITestState.hpp"

#include <osgDB/ReadFile>
#include <ui/ImageFont.hpp>
#include <state/GameStateMgr.hpp>

#include "ui/TextLine.hpp"
#include "ui/Widget.hpp"
#include "ui/Shell.hpp"

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

        auto data_bar = new Widget;
        data_bar->setName("data_bar");
        data_bar->setRect(0, 449, 640, 480);
        data_bar->loadTexture("b_gui_ig_mnu_statusbar", false);
        //data_bar->addDebugData();

        auto dwidth = data_bar->width(); auto dheight = data_bar->height();
        log->info("{} width is {} and height is {}", data_bar->getName(), data_bar->width(), data_bar->height());        

        auto hp_button = new Widget;
        hp_button->setName("hp_button");
        hp_button->setRect(45, 439, 67, 471);
        hp_button->loadTexture("b_gui_ig_mnu_icon_health_up", false);
        hp_button->setUVRect(0.000000, 0.000000, 0.687500, 1.000000);
        //hp_button->addDebugData();

        auto hpwidth = hp_button->width(); auto hpheight = hp_button->height();
        log->info("{} width is {} and height is {}", hp_button->getName(), hpwidth, hpheight);

        shell.addWidget(data_bar);
        shell.addWidget(hp_button);
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
