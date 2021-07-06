
#pragma once

#include "Widget.hpp"

#include <osg/Switch>

namespace ehb
{
    class Button final : public Widget
    {
    public:

        Button(Shell & shell);

        virtual ~Button() = default;

        virtual void createCommonCtrl(const std::string & value) override;

    private:

        osg::ref_ptr<osg::Switch> baseSwitch = nullptr;
    };
}
