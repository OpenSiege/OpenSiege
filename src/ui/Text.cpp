
#include "Text.hpp"

#include "WidgetComponent.hpp"

namespace ehb
{
    Text::Text(Shell & shell) : Widget(shell), line(*this)
    {
    }

    void Text::resizeToScreenResolution(uint32_t newScreenWidth, uint32_t newScreenHeight)
    {
        Widget::resizeToScreenResolution(newScreenWidth, newScreenHeight);

        Rect rect = effectiveRect();

        unsigned int textWidth = 0, textHeight = 0;

        if (line.drawable)
        {
            const osg::BoundingBox & box = line.drawable->getBoundingBox();

            textWidth = box.xMax() - box.xMin();
            textHeight = box.yMax() - box.yMin();

            // used for draw order sorting
            line.drawable->setUserValue("draw_order", drawOrder());
            line.drawable->setUserValue("layer", layer());
        }

        int x;

        switch (line.justification)
        {
        case JUSTIFICATION::justify_left: x = rect.left;
            break;

        case JUSTIFICATION::justify_right: x = rect.right - textWidth;
            break;

            // TODO: check if textWidth > widget.width!
        case JUSTIFICATION::justify_center: x = rect.left + (width() - textWidth) / 2;
            break;

        default: x = 0;
            break;
        }

        line.transform->setPosition(osg::Vec3(x, rect.top, 0));
    }

    void Text::setColor(const osg::Vec4 & value)
    {
        line.color = value;

        if (font)
        {
            line.build(*font);
            resizeToScreenResolution(screenWidth(), screenHeight());
        }
    }

    void Text::setJustification(JUSTIFICATION value)
    {
        line.justification = value;

        resizeToScreenResolution(screenWidth(), screenHeight());
    }

    void Text::setText(const std::string & value, bool resizeWidget)
    {
        line.text = value;

        if (font)
        {
            if (resizeWidget)
            {
                uint32_t newWidth = font->getTextWidth(value);
                uint32_t newHeight = font->getTextHeight();

                resize(newWidth, newHeight);
            }

            line.build(*font);
            resizeToScreenResolution(screenWidth(), screenHeight());
        }
    }
}
