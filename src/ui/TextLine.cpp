
#include "TextLine.hpp"

#include "Font.hpp"

namespace ehb
{
    TextLine::TextLine(osg::Group& parent)
    {
        parent.addChild(transform);
    }

    TextLine::TextLine(osg::Group& parent, const std::string& text, const osg::Vec4& color) : text(text), color(color)
    {
        parent.addChild(transform);
    }

    void TextLine::build(const Font& font)
    {
        if (drawable != nullptr)
        {
            transform->removeChild(drawable);
        }

        drawable = font.createText(text, color);
        transform->addChild(drawable);
    }
}
