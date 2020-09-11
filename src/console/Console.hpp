
#pragma once

#include <string>

#include <ui/Widget.hpp>
#include <ui/TextLine.hpp>
#include <ui/ImageFont.hpp>

#include "ConsoleSignatureScanner.hpp"

namespace ehb
{
    class IGameStateMgr;
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

        Console(ImageFont* imageFont, IGameStateMgr& gameStateMgr);

        virtual ~Console() = default;

        void resetCaret();

        void handleCharacter(uint8_t character);

        void addLineToHistory();

        void removeLastCharacterFromInputLine();

        bool active();

        void toggle();
    };
}