
#include "TextLine.hpp"

#include "Font.hpp"

namespace ehb
{
    TextLine::TextLine(osg::Group& parent)
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
