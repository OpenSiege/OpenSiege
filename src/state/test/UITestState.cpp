
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
        data_bar->setRect(0, 449, 640, 480);
        data_bar->loadTexture("b_gui_ig_mnu_statusbar", false);
        //data_bar->addDebugData();

        auto hp_button = new Widget;
        hp_button->setName("hp_button");
        hp_button->setRect(45, 439, 67, 471);
        hp_button->loadTexture("b_gui_ig_mnu_icon_health_up", false);
        hp_button->setUVRect(0.000000, 0.000000, 0.687500, 1.000000);
        //hp_button->addDebugData();

        scene.addChild(data_bar);
        scene.addChild(hp_button);
    }

    void UITestState::leave()
    {
        scene.removeChildren(0, scene.getNumChildren());
    }

    void UITestState::update(double deltaTime)
    {
    }

    bool UITestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
