
#include "ReaderWriterUI.hpp"

#include <spdlog/spdlog.h>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include "gas/Fuel.hpp"
#include "IFileSys.hpp"

#include "ui/Shell.hpp"
#include "ui/Widget.hpp"
#include "ui/Text.hpp"

namespace ehb
{
    ReaderWriterUI::ReaderWriterUI(IFileSys & fileSys, Shell & shell) : fileSys(fileSys), shell(shell)
    {
        supportsExtension("gas", "Dungeon Siege Gas");
    }

    const char * ReaderWriterUI::className() const
    {
        return "Dungeon Siege Siege User Interface Reader";
    }

    bool ReaderWriterUI::acceptsExtension(const std::string & extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "gas");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterUI::readNode(const std::string & filename, const osgDB::Options * options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        return readNode(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterUI::readNode(std::istream & stream, const osgDB::Options * options) const
    {
        auto log = spdlog::get("log");

        if (Fuel doc; doc.load(stream))
        {
            if (!doc.eachChild().empty())
            if (auto root = doc.eachChild()[0])
            {
                if (root->type() == "interface" || root->valueAsBool("interface") || root->valueOf("type") == "interface")
                {
                    osg::ref_ptr<osg::Group> group = new osg::Group;

                    group->setName(root->name());

                    // std::optional<unsigned int> resX, resY;
                    const int32_t intendedResolutionWidth = root->valueAsInt("intended_resolution_width", -1);
                    const int32_t intendedResolutionHeight = root->valueAsInt("intended_resolution_height", -1);

                    if (const std::string value = root->valueOf("centered"); !value.empty())
                    {
                        group->setUserValue("centered", value);
                    }

                    for (const auto node : root->eachChild())
                    {
                        // if (node->name() != "staging_area_button_start_game") continue;
                        // if (node->type() != "status_bar") continue;
                        // log->info("attempting to load element {} of type {}", node->name(), node->type());

                        if (Widget* widget = readWidget(node))
                        {
                            // TODO: store this is interface parent instead... grab from there?
                            if (intendedResolutionWidth != -1 && intendedResolutionHeight != -1)
                            {
                                widget->resX = intendedResolutionWidth;
                                widget->resY = intendedResolutionHeight;
                            }

                            for (const auto child : node->eachChild())
                            {
                                if (Widget* childWidget = readWidget(child))
                                {
                                    widget->addChild(childWidget);
                                }
                            }

                            widget->parentInterface = root->name();

                            group->addChild(widget);
                        }
                        else
                        {
                            log->error("unknown widget {} of type {} found in {}", node->name(), node->type(), root->name());
                        }
                    }

                    return group.release();
                }
            }
        }

        return ReadResult::FILE_NOT_HANDLED;
    }

    Widget * ReaderWriterUI::readWidget(const FuelBlock * node) const
    {
        if (Widget * widget = shell.createDefaultWidgetOfType(node->type()))
        {
            widget->setName(node->name());

            if (Rect value; fromString(node->valueOf("rect"), value))
            {
                widget->base = value;
            }

            widget->z.passThrough = node->valueAsBool("pass_through");
            widget->z.consumable = node->valueAsBool("consumable");
            widget->z.value = node->valueAsUInt("draw_order");
            if (node->valueAsBool("topmost")) widget->setTopMost(true);

            widget->center.x = node->valueAsBool("center_x");
            widget->center.y = node->valueAsBool("center_y");
            widget->stretch.x = node->valueAsBool("stretch_x");
            widget->stretch.y = node->valueAsBool("stretch_y");

            widget->setVisible(node->valueAsBool("visible", true));

            // widget->consumable = node->valueAsBool("consumable");
            // widget->enabled = node->valueAsBool("enabled");

            widget->attr.group = node->valueOf("group");
            widget->attr.dockGroup = node->valueOf("dock_group");
            // widget->alpha = node->valueAsFloat("alpha", 1.f);

            if (node->valueAsBool("is_left_anchor")) widget->anchor.left = node->valueAsInt("left_anchor");
            if (node->valueAsBool("is_right_anchor")) widget->anchor.right = node->valueAsInt("right_anchor");
            if (node->valueAsBool("is_top_anchor")) widget->anchor.top = node->valueAsInt("top_anchor");
            if (node->valueAsBool("is_bottom_anchor")) widget->anchor.bottom = node->valueAsInt("bottom_anchor");

            if (node->valueAsBool("common_control")) widget->createCommonCtrl(node->valueOf("common_template"));

            if (const std::string value = node->valueOf("texture"); !value.empty() && value != "none")
            {
                widget->loadTexture(value, false);
            }

            if (NormalizedRect value; fromString(node->valueOf("uvcoords"), value))
            {
                widget->setUVRect(value.left, value.top, value.right, value.bottom);
            }

            if (const std::string value = node->valueOf("wrap_mode"); value == "tiled")
            {
                widget->setTiledTexture (true);
            }

            // parse the [message] block
            const std::string msg_ = "msg_", action_ = "action_";

            for (const auto & attr : node->eachAttrOf("messages"))
            {
            }
            // done parsing the [message] block

            // common control buttons have text against them
            if (node->type() == "button" && widget->isCommonControl())
            {
                for (const auto& child : node->eachChild())
                {
                    if (child->type() == "text")
                    {
#if 0
                        // this causes some weird endless looping
                        Widget * text = readWidget(child);
                        widget->addChild(widget);
#endif
                        Text* textWidget = dynamic_cast<Text*>(shell.createDefaultWidgetOfType("text"));

                        textWidget->line.color = node->valueAsColor("font_color");

                        // TODO: what would this actually do? is this even used? pretty sure it isn't
                        // textWidget->fontSize = node->valueOf("font_size");
                        if (const std::string value = node->valueOf("font_type"); !value.empty())
                        {
                            osg::ref_ptr<osgDB::Options> options = new osgDB::Options(std::string("font=") + value);

                            textWidget->font = osgDB::readRefFile<Font>("/ui/fonts/fonts.gas", options);
                        }

                        if (JUSTIFICATION value; fromString(std::string("justify_") + node->valueOf("justify", "left"), value))
                        {
                            // textWidget->setJustification(value);
                            textWidget->line.justification = value;
                        }

                        if (const std::string value = node->valueAsString("text"); !value.empty())
                        {
                            textWidget->setText(value);
                        }

                        widget->addChild(textWidget);
                    }
                }
            }

            if (node->type() == "chat_box")
            {
            }

            if (node->type() == "listbox")
            {
            }

            if (node->type() == "status_bar")
            {
            }

            if (node->type() == "text")
            {
                Text* textWidget = dynamic_cast<Text*>(widget);

                textWidget->line.color = node->valueAsColor("font_color");

                // TODO: what would this actually do? is this even used? pretty sure it isn't
                // textWidget->fontSize = node->valueOf("font_size");
                if (const std::string value = node->valueOf("font_type"); !value.empty())
                {
                    osg::ref_ptr<osgDB::Options> options = new osgDB::Options(std::string("font=") + value);

                    textWidget->font = osgDB::readRefFile<Font>("/ui/fonts/fonts.gas", options);
                }

                if (JUSTIFICATION value; fromString(std::string("justify_") + node->valueOf("justify", "left"), value))
                {
                    // textWidget->setJustification(value);
                    textWidget->line.justification = value;
                }

                if (const std::string value = node->valueAsString("text"); !value.empty())
                {
                    textWidget->setText(value);
                }
            }

            if (node->type() == "text_box")
            {
            }

            return widget;
        }

        return nullptr;
    }
}
