
#include "DialogBox.hpp"

#include <spdlog/spdlog.h>
#include <osgDB/ReadFile>
#include "Shell.hpp"
#include "WidgetComponent.hpp"

#if defined (WIN32) && defined (DialogBox)
#undef DialogBox
#endif

namespace ehb
{
    DialogBox::DialogBox(Shell & shell) : Widget(shell)
    {
    }

    void DialogBox::createCommonCtrl(const std::string & value)
    {
        auto log = spdlog::get("log");

        auto bottomLeftCornerImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_bottom_left_corner") + ".raw");
        auto bottomRightCornerImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_bottom_right_corner") + ".raw");
        auto bottomSideImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_bottom_side") + ".raw");
        auto leftSideImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_left_side") + ".raw");
        auto rightSideImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_right_side") + ".raw");
        auto topLeftCornerImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_top_left_corner") + ".raw");
        auto topRightCornerImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_top_right_corner") + ".raw");
        auto topSideImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_top_side") + ".raw");
        auto fillImage = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_fill") + ".raw");

        if (!fillImage)
        {
            std::string value2 = value;

            if (const auto itr = value.find('_'); itr != std::string::npos)
            {
                value2.erase(itr, std::string::npos);

                log->info("DialogBox({}): searching for {}_fill instead of {}_fill", value2, value);

                fillImage = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_fill") + ".raw");
            }
        }

        if (bottomLeftCornerImage && bottomRightCornerImage && bottomSideImage && leftSideImage && rightSideImage && topLeftCornerImage && topRightCornerImage && topSideImage && fillImage)
        {
            addChild(new CornerComponent(CornerComponent::Corner::BottomLeft, bottomLeftCornerImage));
            addChild(new CornerComponent(CornerComponent::Corner::BottomRight, bottomRightCornerImage));
            addChild(new SideComponent(SideComponent::Side::Bottom, bottomSideImage, leftSideImage->t()));
            addChild(new SideComponent(SideComponent::Side::Left, leftSideImage, topSideImage->t()));
            addChild(new SideComponent(SideComponent::Side::Right, rightSideImage, topSideImage->t()));
            addChild(new CornerComponent(CornerComponent::Corner::TopLeft, topLeftCornerImage));
            addChild(new CornerComponent(CornerComponent::Corner::TopRight, topRightCornerImage));
            addChild(new SideComponent(SideComponent::Side::Top, topSideImage, leftSideImage->t()));
            addChild(new CenterComponent(fillImage, leftSideImage->s(), topSideImage->t()));

            Widget::createCommonCtrl(value);
        }
    }
}
