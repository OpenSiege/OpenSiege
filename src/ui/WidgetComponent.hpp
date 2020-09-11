
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
}