
#pragma once

#include <string>
#include <osg/PositionAttitudeTransform>
#include <osg/Vec4>

namespace ehb
{
    class Font;
    struct TextLine final
    {
        std::string text;
        osg::Vec4 color = { 1.f, 1.f, 1.f, 1.f };
        osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
        osg::ref_ptr<osg::Drawable> drawable = nullptr;

    public:

        TextLine(osg::Group& parent);

        void build(const Font& font);
    };
}
