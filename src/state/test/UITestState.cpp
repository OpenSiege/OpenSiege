
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
                if (auto gas = doc.child("data_bar:data_bar"))
                {
                    log->debug("reading in health potion gas block");

                    // shell is required for all widgets
                    auto data_bar = new Widget(shell);

                    // test our name setting
                    data_bar->setName(gas->name());

                    // im trying to keep custom types out of the gas parser
                    auto rectValue = gas->valueAsInt4("rect");
                    data_bar->setRect(rectValue[0], rectValue[1], rectValue[2], rectValue[3]);
                    log->debug("{} {} {} {}", rectValue[0], rectValue[1], rectValue[2], rectValue[3]);

                    data_bar->loadTexture(gas->valueOf("texture"), false);

                    // data_bar->addDebugData();

                    auto dwidth = data_bar->width(); auto dheight = data_bar->height();
                    log->info("{} width is {} and height is {}", data_bar->getName(), data_bar->width(), data_bar->height());

                    shell.addWidget(data_bar);
                }

                if (auto gas = doc.child("data_bar:button_health_potions"))
                {
                    auto hp_button = new Widget(shell);
                    hp_button->setName(gas->name());
                    auto rectValue = gas->valueAsInt4("rect");
                    hp_button->setRect(rectValue[0], rectValue[1], rectValue[2], rectValue[3]);
                    hp_button->loadTexture(gas->valueOf("texture"), false);
                    auto uvRectValue = gas->valueAsFloat4("uvcoords");
                    hp_button->setUVRect(uvRectValue[0], uvRectValue[1], uvRectValue[2], uvRectValue[3]);

                    // hp_button->addDebugData();

                    auto hpwidth = hp_button->width(); auto hpheight = hp_button->height();
                    log->info("{} width is {} and height is {}", hp_button->getName(), hpwidth, hpheight);

                    shell.addWidget(hp_button);
                }
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
