
#include "UITestState.hpp"

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <ui/ImageFont.hpp>
#include <osg/PositionAttitudeTransform>
#include <state/GameStateMgr.hpp>

#include "ui/TextLine.hpp"
#include "ui/Widget.hpp"
#include "console/Console.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void UITestState::enter()
    {
        auto log = spdlog::get("log");
        log->info("UITestState::enter()");

        //viewer.getCamera()->setClearColor(osg::Vec4(0.f, 0.f, 0.f, 1.f));

        // remove this once widgets are in
        scene.getOrCreateStateSet()->setMode(GL_DEPTH_TEST, false);
        scene.getOrCreateStateSet()->setMode(GL_BLEND, true);

        std::string font = "b_gui_fnt_12p_copperplate-light";

        auto options = new osgDB::Options(std::string("font=") + font);
        osg::ref_ptr<ImageFont> imageFont = osgDB::readRefFile<ImageFont>("/ui/fonts/fonts.gas", options);

        #if 0
        if (imageFont != nullptr)
        {
            TextLine * line = new TextLine(scene);

            line->text = "this is some test text";

            line->build(*imageFont);
        }
        else
        {
            spdlog::get("log")->error("failed to load {}", font);
        }
        #endif

        auto data_bar = new Widget;
        data_bar->setRect(0, 449, 640, 480);
        data_bar->loadTexture("b_gui_ig_mnu_statusbar", false);
        //data_bar->addDebugData();

        auto hp_button = new Widget;
        hp_button->setRect(45, 439, 67, 471);
        hp_button->loadTexture("b_gui_ig_mnu_icon_health_up", false);
        hp_button->setUVRect(0.000000, 0.000000, 0.687500, 1.000000);
        //hp_button->addDebugData();

        console = new Console(imageFont, gameStateMgr);

        scene.addChild(data_bar);
        scene.addChild(hp_button);
        scene.addChild(console);
    }

    void UITestState::leave()
    {
    }

    void UITestState::update(double deltaTime)
    {
    }

    bool UITestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        switch (event.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                int32_t key = event.getKey();
                if (key == osgGA::GUIEventAdapter::KEY_Backquote)
                {
                    console->toggle();

                    return true;
                }

                if (console->active())
                {
                    if (key > 0 && key < 256)
                    {
                        console->handleCharacter(event.getKey());
                    }
                    else if (key == osgGA::GUIEventAdapter::KEY_Return)
                    {
                        console->addLineToHistory();
                    }
                    else if (key == osgGA::GUIEventAdapter::KEY_BackSpace)
                    {
                        console->removeLastCharacterFromInputLine();
                    }

                    return true;
                }

                spdlog::get("log")->info("console isn't active, letting action fall through");
            }
        }
        return false;
    }
}
