
#pragma once

#include <string>
#include <osg/PositionAttitudeTransform>
#include <osg/Vec4>
#include "Justification.hpp"


namespace ehb
{
    class Font;
    struct TextLine final
    {
        std::string text;
        osg::Vec4 color = { 1.f, 1.f, 1.f, 1.f };
        JUSTIFICATION justification = JUSTIFICATION::justify_left;
        osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
        osg::ref_ptr<osg::Drawable> drawable = nullptr;

    public:

        TextLine(osg::Group& parent);
        TextLine(osg::Group& parent, const std::string& text, const osg::Vec4& color = { 1.f, 1.f, 1.f, 1.f });

        void build(const Font& font);
    };
}
