
#pragma once

#include <osg/PositionAttitudeTransform>

namespace ehb
{
    class Font;
    class Widget;
    class WidgetComponent : public osg::PositionAttitudeTransform
    {
    public:

        WidgetComponent() = default;

        virtual ~WidgetComponent() = default;

        virtual void resizeToWidget(const Widget& widget) = 0;
    };

    class CenterComponent : public WidgetComponent
    {
    public:

        CenterComponent(osg::Image* image, uint32_t horizontalBorder = 0, uint32_t verticalBorder = 0);

        virtual ~CenterComponent() = default;

        virtual void resizeToWidget(const Widget& widget) override;

    private:

        struct { uint32_t horizontal, vertical; } border;
    };

    class CornerComponent : public WidgetComponent
    {
    public:

        enum class Corner
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight
        };

    public:

        CornerComponent(Corner corner, osg::Image* image);

        virtual ~CornerComponent() = default;

        virtual void resizeToWidget(const Widget& widget) override;

    private:

        Corner corner;
        uint32_t width, height;
    };

    class SideComponent : public WidgetComponent
    {
    public:

        enum class Side
        {
            Left,
            Right,
            Top,
            Bottom
        };

    public:

        SideComponent(Side side, osg::Image* image, uint32_t border = 0);

        virtual ~SideComponent() = default;

        virtual void resizeToWidget(const Widget& widget) override;

    private:

        Side side;
        uint32_t border, width, height;
    };
}