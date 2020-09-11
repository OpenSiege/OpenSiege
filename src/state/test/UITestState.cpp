
#include "UITestState.hpp"

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <ui/ImageFont.hpp>
#include <osg/PositionAttitudeTransform>

#include "ui/Widget.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    struct TextLine
    {
        std::string text;
        osg::Vec4 color = {1.f, 1.f, 1.f, 1.f };
        osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
        osg::ref_ptr<osg::Drawable> drawable = nullptr;

        TextLine(osg::Group& parent)
        {
            parent.addChild(transform);
        }

        void build(const Font& font)
        {
            if (drawable != nullptr)
            {
                transform->removeChild(drawable);
            }

            drawable = font.createText(text, color);
            transform->addChild(drawable);
        }
    };

    class Console : public Widget
    {
    public:
        
        int32_t width = 640, height = 480, characterSize = 12;
        int32_t maxLines = (height / characterSize);
        int32_t currentHistoryLine = 0;

        std::unique_ptr<TextLine> inputLine;
        std::vector<std::unique_ptr<TextLine>> history;

        osg::ref_ptr<ImageFont> font = nullptr;

        Console(ImageFont * imageFont) : Widget(), font(imageFont)
        {
            inputLine = std::make_unique<TextLine>(*this);

            // slightly offset from full size of the window so we can see the outline
            setRect(2, 2, 638, 478);

            // hack in an outline
            addDebugData();

            inputLine->transform->setPosition(osg::Vec3(4, effectiveRect().bottom - (characterSize * 2), 0));
            resetCaret();
        }

        virtual ~Console() = default;

        void resetCaret()
        {
            inputLine->text = ">";
            inputLine->build(*font);
        }

        void handleCharacter(uint8_t character)
        {
            inputLine->text += character;
            inputLine->build(*font);
        }

        void addLineToHistory()
        {
            if (inputLine->text != ">")
            {
                spdlog::get("log")->info("Adding history to console @ {}", currentHistoryLine);

                auto line = std::make_unique<TextLine>(*this);
                line->text = inputLine->text.substr(1, inputLine->text.size()); // chop the leading >
                line->build(*font);

                line->transform->setPosition(osg::Vec3 (4, characterSize * currentHistoryLine, 0));

                history.emplace_back(std::move(line));

                currentHistoryLine++;

                resetCaret();
            }
        }

        void removeLastCharacterFromInputLine()
        {
            if (inputLine->text.size() > 1)
            {
                inputLine->text.erase(inputLine->text.length() - 1, 1);
                inputLine->build(*font);
            }
        }
    };

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

        console = new Console(imageFont);

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
        }
        return false;
    }
}
