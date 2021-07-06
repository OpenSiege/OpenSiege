
#include "Button.hpp"

#include <algorithm>
#include <spdlog/spdlog.h>
#include <osgDB/ReadFile>
#include "Shell.hpp"
#include "WidgetComponent.hpp"

namespace ehb
{
    Button::Button(Shell & shell) : Widget(shell)
    {
    }

    void Button::createCommonCtrl(const std::string & value)
    {
        auto log = spdlog::get("log");

        // template types:
        // - button
        //   - value_up, value_down, value_hov
        //   - value_button_up, value_button_down, value_button_hov
        //   - value_left_up, value_left_down, value_left_hov,
        //     value_center_up, value_center_down, value_center_hov,
        //     value_right_up, value_right_down, value_right_hov

        osg::ref_ptr<osg::Switch> base = new osg::Switch;

        {
            auto up = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_up") + ".raw");
            auto down = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_down") + ".raw");
            auto hov = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_hov") + ".raw");

            if (up && down && hov)
            {
                base->addChild(new CenterComponent(up), true);
                base->addChild(new CenterComponent(down));
                base->addChild(new CenterComponent(hov));

                addChild(base);
                baseSwitch = base.release();

                Widget::createCommonCtrl(value);

                return;
            }
        }

        {
            auto up = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_button_up") + ".raw");
            auto down = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_button_down") + ".raw");
            auto hov = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_button_hov") + ".raw");

            if (up && down && hov)
            {
                base->addChild(new CenterComponent(up), true);
                base->addChild(new CenterComponent(down));
                base->addChild(new CenterComponent(hov));

                addChild(base);
                baseSwitch = base.release();

                Widget::createCommonCtrl(value);

                return;
            }
        }

        {
            auto leftUp = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_left_up") + ".raw");
            auto leftDown = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_left_down") + ".raw");
            auto leftHov = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_left_hov") + ".raw");

            auto centerUp = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_center_up") + ".raw");
            auto centerDown = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_center_down") + ".raw");
            auto centerHov = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_center_hov") + ".raw");

            auto rightUp = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_right_up") + ".raw");
            auto rightDown = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_right_down") + ".raw");
            auto rightHov = osgDB::readRefImageFile(shell.mapCtrlArt(value + "_right_hov") + ".raw");

            if (!leftUp && !leftDown && !leftHov && !centerUp && !centerDown && !centerHov && !rightUp && !rightDown && !rightHov)
            {
                std::string value2 = value;
                value2.erase(std::remove(value2.begin(), value2.end(), '_'), value2.end());

                leftUp = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_left_up") + ".raw");
                leftDown = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_left_down") + ".raw");
                leftHov = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_left_hov") + ".raw");

                centerUp = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_center_up") + ".raw");
                centerDown = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_center_down") + ".raw");
                centerHov = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_center_hov") + ".raw");

                rightUp = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_right_up") + ".raw");
                rightDown = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_right_down") + ".raw");
                rightHov = osgDB::readRefImageFile(shell.mapCtrlArt(value2 + "_right_hov") + ".raw");
            }

            if (leftUp && leftDown && leftHov && centerUp && centerDown && centerHov && rightUp && rightDown && rightHov)
            {
                osg::Group * up = new osg::Group, * down = new osg::Group, * hov = new osg::Group;

                base->addChild(up, true);
                base->addChild(down, false);
                base->addChild(hov, false);

                up->addChild(new SideComponent(SideComponent::Side::Left, leftUp));
                up->addChild(new CenterComponent(centerUp, leftUp->s()));
                up->addChild(new SideComponent(SideComponent::Side::Right, rightUp));

                down->addChild(new SideComponent(SideComponent::Side::Left, leftDown));
                down->addChild(new CenterComponent(centerDown, leftDown->s()));
                down->addChild(new SideComponent(SideComponent::Side::Right, rightDown));

                hov->addChild(new SideComponent(SideComponent::Side::Left, leftHov));
                hov->addChild(new CenterComponent(centerHov, leftHov->s()));
                hov->addChild(new SideComponent(SideComponent::Side::Right, rightHov));

                addChild(base);
                baseSwitch = base.release();

                Widget::createCommonCtrl(value);

                return;
            }
        }
    }
}
