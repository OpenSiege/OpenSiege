
#include "UITestState.hpp"

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <ui/ImageFont.hpp>
#include <osg/PositionAttitudeTransform>
#include <state/GameStateMgr.hpp>

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

    class ConsoleSignatureScanner
    {
    public:

        ConsoleSignatureScanner(const std::string& text) : first(0), last(0)
        {
            data.resize(text.size());

            for (std::string::size_type i = 0; i < text.size(); i++)
            {
                data[i] = std::tolower(text[i]);
            }

            if (!next()) throw "no token found";
        }

        bool accept(const std::string& text)
        {
            if (data.compare(first, last - first, text) == 0)
            {
                next();
                return true;
            }

            return false;
        }

        bool expect(const std::string& text)
        {
            if (!accept(text))
            {
                return false;
                // throw "unexpected token found";
            }
        }

        bool token(std::string::size_type& first_, std::string::size_type& last_)
        {
            if (last < data.size())
            {
                first_ = first;
                last_ = last;

                next();

                return true;
            }

            return false;
        }

    private:

        bool next()
        {
            if (last < data.size())
            {
                first = data.find_first_not_of(' ', last);

                if (first == std::string::npos) { last = first; }

                if (data[first] == '\n') { last = first + 1; }
                else { last = data.find_first_of(" \n", first); }

                // std::cout << "found token: '" << data.substr(first, last - first) << "' @ (" << first << " to " << last << ") -> " << data[first] << std::endl;

                return true;
            }

            return false;
        }

    private:

        std::string data;
        std::string::size_type first, last;
    };

    class Console : public Widget
    {
    public:

        IGameStateMgr& gameStateMgr;
        
        int32_t width = 640, height = 480, characterSize = 12;
        int32_t maxLines = (height / characterSize);
        int32_t currentHistoryLine = 0;

        std::unique_ptr<TextLine> inputLine;
        std::vector<std::unique_ptr<TextLine>> history;

        osg::ref_ptr<ImageFont> font = nullptr;

        Console(ImageFont * imageFont, IGameStateMgr& gameStateMgr) : Widget(), font(imageFont), gameStateMgr(gameStateMgr)
        {
            inputLine = std::make_unique<TextLine>(*this);

            // slightly offset from full size of the window so we can see the outline
            setRect(2, 2, width - 2, height - 2);

            // hack in an outline
            addDebugData();

            inputLine->transform->setPosition(osg::Vec3(4, effectiveRect().bottom - characterSize, 0));
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
                auto input = inputLine->text.substr(1, inputLine->text.size()); // chop the leading >
                input += '\n'; // give input a newline so the parser can understand when the command is done

                ConsoleSignatureScanner scanner (input);

                std::size_t first, last;

                if (scanner.accept("setstate"))
                {
                    while (scanner.token(first, last))
                    {
                        spdlog::get("log")->info("name: {}", input.substr(first, last - first));

                        gameStateMgr.request(input.substr(first, last-first));
                    }
                }

                auto line = std::make_unique<TextLine>(*this);
                line->text = inputLine->text.substr(1, inputLine->text.size());
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

        bool active()
        {
            return getNodeMask() != 0;
        }

        void toggle()
        {
            if (!active())
            {
                setNodeMask(~0);
            }
            else
            {
                setNodeMask(0);
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
