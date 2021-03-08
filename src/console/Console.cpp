
#include "Console.hpp"

#include <sstream>
#include "state/IGameStateMgr.hpp"
#include "cfg/IConfig.hpp"
#include "IFileSys.hpp"
#include "StringTool.hpp"
#include "osg/SiegeNodeMesh.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    Console::Console(IConfig& config, IFileSys& fileSys, IGameStateMgr& gameStateMgr, osg::Group& scene3d, osg::Group& scene2d) : Widget(), config(config), fileSys(fileSys), gameStateMgr(gameStateMgr), scene3d(scene3d), scene2d(scene2d)
    {
        getOrCreateStateSet()->setMode(GL_DEPTH_TEST, false);
        getOrCreateStateSet()->setMode(GL_BLEND, true);

        inputLine = std::make_unique<TextLine>(*this);

        width = config.getInt("width", 640);
        height = config.getInt("height", 480);

        // slightly offset from full size of the window so we can see the outline
        setRect(2, 2, width - 2, height - 2);

        // hack in an outline
        addDebugData();

        inputLine->transform->setPosition(osg::Vec3(4, effectiveRect().bottom - characterSize, 0));

        context.activeVariable.reset();
    }

    bool Console::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        switch (event.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                int32_t key = event.getKey();

                if (key > 0 && key < 256)
                {
                    handleCharacter(event.getKey());
                }
                else if (key == osgGA::GUIEventAdapter::KEY_Return)
                {
                    addLineToHistory();
                }
                else if (key == osgGA::GUIEventAdapter::KEY_BackSpace)
                {
                    removeLastCharacterFromInputLine();
                }
                else if (key == osgGA::GUIEventAdapter::KEY_Up)
                {
                    restorePreviousHistoryLineToConsole();
                }

                return true;
            }
        }
        return false;
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

            if (scanner.accept("help"))
            {
                std::stringstream ss;

                ss << "setstate <stateName>\nclearscene <world/gui>";

                for (const auto& line : StringTool::split(ss.str(), '\n'))
                {
                    auto helpLine = std::make_unique<TextLine>(*this);
                    helpLine->text = line;
                    helpLine->build(*font);

                    helpLine->transform->setPosition(osg::Vec3(4, characterSize * currentHistoryLine, 0));

                    history.emplace_back(std::move(helpLine));

                    currentHistoryLine++;
                }

            }
            else if (scanner.accept("setactive"))
            {
                if (scanner.token(first, last))
                {
                    auto param = input.substr(first, last - first);

                    if (param == "widget")
                    {
                        if (scanner.token(first, last))
                        {
                            auto param2 = input.substr(first, last - first);

                            for (uint32_t i = 0; i < scene2d.getNumChildren(); ++i)
                            {
                                if (auto node = scene2d.getChild(i); node != nullptr)
                                {
                                    if (node->getName() == param2)
                                    {
                                        if (auto widget = dynamic_cast<Widget*>(node); widget != nullptr)
                                        {
                                            context.activeVariable = widget;
                                        }

                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else if (param == "siegenode")
                    {
                        if (scanner.token(first, last))
                        {
                            auto param2 = input.substr(first, last - first);

                            for (uint32_t i = 0; i < scene3d.getNumChildren(); ++i)
                            {
                                if (auto node = scene3d.getChild(i); node != nullptr)
                                {
                                    if (node->getName() == param2)
                                    {
                                        if (auto mesh = dynamic_cast<SiegeNodeMesh*>(node); mesh != nullptr)
                                        {
                                            context.activeVariable = mesh;
                                        }

                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    std::string error = ">error: setactive requires 3 parameters 'setactive <widget/siegenode> <name>'";
                    inputLine->text = error;
                    spdlog::get("log")->error("{}", error);

                    addLineToHistory();
                }
            }
            else if (scanner.accept("hideactive"))
            {
                if (context.activeVariable.has_value())
                {
                    if (auto node = std::any_cast<Widget*>(context.activeVariable); node != nullptr)
                    {
                        node->setNodeMask(0);
                    }
                    else if (auto node = std::any_cast<SiegeNodeMesh*>(context.activeVariable); node != nullptr)
                    {
                        node->setNodeMask(0);
                    }
                }
            }
            else if (scanner.accept("showactive"))
            {
                if (context.activeVariable.has_value())
                {
                    if (auto node = std::any_cast<Widget*>(context.activeVariable); node != nullptr)
                    {
                        node->setNodeMask(~0);
                    }
                    else if (auto node = std::any_cast<SiegeNodeMesh*>(context.activeVariable); node != nullptr)
                    {
                        node->setNodeMask(~0);
                    }
                }
            }
            else if (scanner.accept("setstate"))
            {
                while (scanner.token(first, last))
                {
                    // spdlog::get("log")->info("name: {}", input.substr(first, last - first));

                    gameStateMgr.request(input.substr(first, last - first));
                }
            }
            else if (scanner.accept("clearscene"))
            {
                if (scanner.token(first, last))
                {
                    auto param = input.substr(first, last - first);
                    
                    if (param == "world")
                    {
                        scene3d.removeChildren(0, scene3d.getNumChildren());
                    }
                    else if (param == "gui")
                    {
                        scene2d.removeChildren(0, scene2d.getNumChildren());
                    }
                }
            }
            else if (scanner.accept("dumpfilesys"))
            {
                auto log = spdlog::get("filesystem");

                log->info("STARTING FILESYSTEM DUMP");

                for (auto file : fileSys.getFiles())
                {
                    log->info("{}", file);
                }

                log->info("FINISHED FILESYSTEM DUMP");
            }

            auto line = std::make_unique<TextLine>(*this);
            line->text = inputLine->text.substr(1, inputLine->text.size());
            line->build(*font);

            line->transform->setPosition(osg::Vec3(4, characterSize * currentHistoryLine, 0));

            history.emplace_back(std::move(line));

            currentHistoryLine++;

            // reset our cycle history line to make sure our up arrow works as intended
            cycleHistoryLine = history.size() - 1;

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

    void Console::restorePreviousHistoryLineToConsole()
    {
        // if there is no history then we should just bail out
        if (history.size() == 0)
            return;

        // prevent the history from cycling below 0
        if (cycleHistoryLine < 0) cycleHistoryLine = 0;

        if (auto& lineOfHistory = history.at(cycleHistoryLine); lineOfHistory != nullptr)
        {
            inputLine->text = ">" + lineOfHistory->text;
            inputLine->build(*font);
        }

        cycleHistoryLine--;
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