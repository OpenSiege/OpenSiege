
#pragma once

#include "ConsoleSignatureScanner.hpp"

#include <string>
#include <ui/Widget.hpp>
#include <ui/TextLine.hpp>
#include <ui/ImageFont.hpp>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

namespace ehb
{
    class IGameStateMgr;
    class Console : public Widget
    {
    public:

        IGameStateMgr& gameStateMgr;
        osg::Group& scene3d;
        osg::Group& scene2d;

        int32_t width = 640, height = 480, characterSize = 12;
        int32_t maxLines = (height / characterSize);
        int32_t currentHistoryLine = 0;

        std::unique_ptr<TextLine> inputLine;
        std::vector<std::unique_ptr<TextLine>> history;

        osg::ref_ptr<ImageFont> font = nullptr;

        Console(IGameStateMgr& gameStateMgr, osg::Group& scene3d, osg::Group& scene2d);

        // this is a fake handler
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action);

        virtual ~Console() = default;

        void resetCaret();

        void handleCharacter(uint8_t character);

        void addLineToHistory();

        void removeLastCharacterFromInputLine();

        bool active();

        void toggle();
    };
}