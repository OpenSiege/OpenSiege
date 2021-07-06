
#pragma once

#include <osg/Vec4>
#include "Font.hpp"
#include "Justification.hpp"
#include "Widget.hpp"

#include "TextLine.hpp"

namespace ehb
{
    class TextComponent;
    class Text : public Widget
    {
        friend class ReaderWriterUI;

    public:

        Text(Shell & shell);

        virtual ~Text() = default;

        virtual void resizeToScreenResolution(uint32_t newScreenWidth, uint32_t newScreenHeight) override;

        void setColor(const osg::Vec4 & value);
        const osg::Vec4 & color() const;

        void setJustification(JUSTIFICATION value);
        JUSTIFICATION justification() const;

        void setText(const std::string & value, bool resizeWidget = false);
        const std::string & text() const;

    private:

        TextLine line;
        osg::ref_ptr<Font> font;
    };

    inline const osg::Vec4 & Text::color() const
    {
        return line.color;
    }

    inline JUSTIFICATION Text::justification() const
    {
        return line.justification;
    }

    inline const std::string & Text::text() const
    {
        return line.text;
    }
}
