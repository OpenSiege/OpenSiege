
#include "Console.hpp"

#include <spdlog/spdlog.h>

#include "state/IGameStateMgr.hpp"

namespace ehb
{
    Console::Console(ImageFont* imageFont, IGameStateMgr& gameStateMgr) : Widget(), font(imageFont), gameStateMgr(gameStateMgr)
    {
        inputLine = std::make_unique<TextLine>(*this);

        // slightly offset from full size of the window so we can see the outline
        setRect(2, 2, width - 2, height - 2);

        // hack in an outline
        addDebugData();

        inputLine->transform->setPosition(osg::Vec3(4, effectiveRect().bottom - characterSize, 0));
        resetCaret();
    }

    void Console::resetCaret()
    {
        inputLine->text = ">";
        inputLine->build(*font);
    }

    void Console::handleCharacter(uint8_t character)
    {
        inputLine->text += character;
        inputLine->build(*font);
    }

    void Console::addLineToHistory()
    {
        if (inputLine->text != ">")
        {
            auto input = inputLine->text.substr(1, inputLine->text.size()); // chop the leading >
            input += '\n'; // give input a newline so the parser can understand when the command is done

            ConsoleSignatureScanner scanner(input);

            std::size_t first, last;

            if (scanner.accept("setstate"))
            {
                while (scanner.token(first, last))
                {
                    spdlog::get("log")->info("name: {}", input.substr(first, last - first));

                    gameStateMgr.request(input.substr(first, last - first));
                }
            }

            auto line = std::make_unique<TextLine>(*this);
            line->text = inputLine->text.substr(1, inputLine->text.size());
            line->build(*font);

            line->transform->setPosition(osg::Vec3(4, characterSize * currentHistoryLine, 0));

            history.emplace_back(std::move(line));

            currentHistoryLine++;

            resetCaret();
        }
    }

    void Console::removeLastCharacterFromInputLine()
    {
        if (inputLine->text.size() > 1)
        {
            inputLine->text.erase(inputLine->text.length() - 1, 1);
            inputLine->build(*font);
        }
    }

    bool Console::active()
    {
        return getNodeMask() != 0;
    }

    void Console::toggle()
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