
#pragma once

#include "ConsoleSignatureScanner.hpp"

#include <string>
#include <any>
#include <ui/Widget.hpp>
#include <ui/TextLine.hpp>
#include <ui/ImageFont.hpp>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

namespace ehb
{
    class IConfig;
    class IGameStateMgr;
    class Console : public Widget
    {
    public:

        // the console shouldn't rely on the filesystem and should use a system font
        // TODO: disconnect the console from ImageFonts
        osg::ref_ptr<ImageFont> font = nullptr;

        Console(IConfig& config, IGameStateMgr& gameStateMgr, osg::Group& scene3d, osg::Group& scene2d);

        virtual ~Console() = default;

        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action);

        bool active();

        void toggle();

        void resetCaret();

    private:

        void handleCharacter(uint8_t character);

        void addLineToHistory();

        void removeLastCharacterFromInputLine();

        void restorePreviousHistoryLineToConsole();

    private:

        struct ConsoleContext
        {
            std::any activeVariable;
        };
        
    private:

        IConfig& config;
        IGameStateMgr& gameStateMgr;
        osg::Group& scene3d;
        osg::Group& scene2d;

        int32_t characterSize = 12;
        int32_t maxLines = (height / characterSize);
        int32_t currentHistoryLine = 0;
        int32_t cycleHistoryLine = 0;
        int32_t width, height;

        std::unique_ptr<TextLine> inputLine;
        std::vector<std::unique_ptr<TextLine>> history;

        ConsoleContext context;
    };
}